#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_adc.h"
#include "Inicializacoes.h"
#include "LoopPrincipal.h"
#include "ISR.h"
#include "Controlador_OnOff.h"
#include "Controlador_PID.h"

// public variables
// vari�veis globais, para serem capturadas pelo STMStudio
float saidaControladorPID;
float sinalPWM;
float sinalPWMsaidaPID;
float sinalPWMsetPointTemperatura;

u8 flagIntTimer2 = 0;
// controle da temperatura --> PID controller ==> setpoint inicial = 40.0 oC
float setPoint = 60.0;
// valores iniciais de Kp, Ki e Kd
float K_p = 5.0;
float K_i = 0.03;
float K_d = 3.0;

float temperatura_t0 = 0.0;
float sinalErro_t0 = 0.0;
float integralParcial = 0.0;
float integral_PID = 0.0;
float proporcional_PID = 0.0;
float derivativo_PID = 0.0;

void mainControladorPID(void)
{
	// medi��o inicial do valor da temperatura no instante de tempo t0 ==> temperatura(t0)
	medicaoTempEnvUSART();
	temperatura_t0 = temperaturaFloat;

	// valor inicial do sinal de erro no instante de tempo t0  ==> erro(t0)
	sinalErro_t0 = setPoint - temperatura_t0;

	// loop infinito
	while (1)
	{
		// medi��o do valor da temperatura e envio resultado via USART3
		medicaoTempEnvUSART();

		// c�lculo do erro na entrada do controlador
		sinalErro = setPoint - temperaturaFloat;

		// LED piloto do sinal de erro
		ledPilotoErro();

		// envio erro via USART3
//		mostraSinalErroFloat(sinalErro);

		// controlador PID
		saidaControladorPID = PID_controller(sinalErro, K_p, K_i, K_d);

		// convers�o da sa�da float do controlador PID para o valor correspondente int 32 bits utilizado no DC do PWM
		sinalPWMsaidaPID = convTempFloat2DC_PWM(saidaControladorPID);

		// convers�o do setpoint da temperatura (float) para o valor correspondente int 32 bits utilizado no DC do PWM
		sinalPWMsetPointTemperatura = convTempFloat2DC_PWM(setPoint);

		// C�lculo do duty cycle do PWM
		dutyCyclePWM(sinalPWMsaidaPID, sinalPWMsetPointTemperatura);

		// delta t de amostragem do sinal de temperatura ==> 100 ms
		// aguardo a interrup��o do Timer 2
		while (!flagIntTimer2)
			;
		flagIntTimer2 = 0;

		/* Toggle LED */
		GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
	}
}

/* C�lculo do duty cycle que ser� utilizado para gerar o PWM atrav�s do Timer */
void dutyCyclePWM(float sinal1, float sinal2)
{
	// o Dyty Cycle final � a soma do SetPoint com a sa�da do controlador
	//	sinalPWM = sinal1 + sinal2;

	// controlador PID original... sem o la�o do SetPoint. Apenas o sinal do erro ser� utilizado
	sinalPWM = sinal1;

	// testando os limites ==> Duty Cycle tem que ser menor ou igual � varTIM4_Period
	if (sinalPWM > varTIM4_Period)
		sinalPWM = varTIM4_Period;
	if (sinalPWM < 0)
		sinalPWM = 0;

	u32 sinalDC_PWM = sinalPWM;

	/* carrego o resultado do duty cycle no TIMER
	 * change Duty Cycle TIM4 Channel 2 */
	TIM_SetCompare2(TIM4, sinalDC_PWM);
}

/* Controlador PID
 * Par�metros de entrada:
 * sinalErroEntrada = sinal de erro da temperatura na entrada do controlador
 * Kp = constante proporcional
 * Ki = constante integral
 * Kd = constante derivativa
 * Par�metro de retorno: Valor float da atua��o do controle sobre o sinal de erro */
float PID_controller(float sinalErroEntrada, float Kp, float Ki, float Kd)
{
	// Delta t ==> per�odo de amostragem do sinal de temperatura = 100 ms
	float delta_t = 0.1;

	// parte proporcional do PID
	proporcional_PID = sinalErroEntrada * Kp;

	/* parte derivativa do PID
	 * derivada discreta de F(t) = delta F(t) / delta t = (F(t1) - F(t0)) / (t1 - t0)
	 * delta t = t1 - t0 = 100 ms
	 * Delta Erro ==> Erro(t1)[sinal do erro atual] - Erro(t0)[sinal do erro anterior] */
	float delta_erro = sinalErroEntrada - sinalErro_t0;
	// parte derivativa do PID ==> Kd * derivada do sinal do erro discreto pelo tempo
	derivativo_PID = Kd * delta_erro / delta_t;

	// parte integral do PID --> c�lculo da fun��o integral em delta t
	// c�lculo do valor m�dio da fun��o erro(t) no intervalo de tempo delta t = t1 - t0 = 100 ms
	float media_erro = (sinalErroEntrada + sinalErro_t0) / 2;
	// �rea m�dia em baixo da curva ==> sinal m�dio do erro X delta t
	float areaInstantanea = media_erro * delta_t;
	// a integral da fun��o erro = somat�rio das m�dias da fun��o em delta t
	integralParcial = integralParcial + areaInstantanea;
	// parte integral do PID
	integral_PID = Ki * integralParcial;

	float PID = proporcional_PID + derivativo_PID + integral_PID;

	// salvo o valor da fun��o erro atual na vari�viel sinalErro_t0 para os pr�ximos c�lculos
	sinalErro_t0 = sinalErroEntrada;

	return PID;
}

/**
 * @brief  rotina que faz a convers�o da temperatura Float para o valor correspondente do valor inteiro do DC do PWM
 * @param  float tempFloatMedida ==> valor real da temperatura medida pelol sensor LM35
 * @retval retorno valor float Y correspondente ao valor do DC do PWM
 * 			a sa�da do controlador pode ser negativo! Ent�o Y pode ser negativo
 */
float convTempFloat2DC_PWM(float tempFloatMedida)
{
	/* obtem-se dois pontos da curva, pr�ximos ao setpoint, para gerar a fun��o de convers�o da temperatura
	 * float em inteiro de 16 bits correspondente do gerator de PWM.
	 * Para isso, deve-se ajustar um valor de PWM emp�rido que fique abaixo do valor de setpoint e outro acima,
	 * esperar a temperatura esbilizar, e anotar o valor da temperatura e do int gerador do PWM
	 * tendo-se 2 pontos na reta linearizada.
	 * */

	// ponto 1 na reta DC[PWM] X Temperatura obtido empiricamente
	float dcPWM1 = 4000;
	float tempFloat1 = 54.6;

	// ponto 2 na reta DC[PWM] X Temperatura obtido empiricamente
	float dcPWM2 = 10000;
	float tempFloat2 = 96.5;

	/* tendo-se dois pois de uma reta, podemos obter sua equa��o Y = AX + B
	 * onde Y = dcPWM e X = tempFloat
	 * A = (Y1 - Y2)/(X1 - X2)
	 * B = Y2 - (A * X2)
	 * */
	float A = (dcPWM1 - dcPWM2) / (tempFloat1 - tempFloat2);
	float B = dcPWM2 - (A * tempFloat2);

	float Y;
	float X;

	X = tempFloatMedida;
	Y = A * X + B;

	return Y;
}

/* PD15 --> LED piloto (LED Azul da placa)
 * a sa�da PD15 ser� o LED piloto do sinal de erro */
void ledPilotoErro(void)
{
	if (sinalErro > 0.0)
	{
		/* PD15 ON --> erro > 0 */
		GPIO_SetBits(GPIOD, GPIO_Pin_15);
	}
	else
	{
		/* PD15 OFF --> erro < 0 */
		GPIO_ResetBits(GPIOD, GPIO_Pin_15);
	}
}

/* medi��o da temperatura em float e envio do resultado para USART3 */
void medicaoTempEnvUSART(void)
{
	// m�dia aritm�tica de 500 medidas do conv AD
	s32 mediaTempInt = mediaLeituraSensorTemperatura(500);

	// converto o resultado int AD em float e coloco na vari�vel global da temperatura
	temperaturaFloat = calculoTempFloat(mediaTempInt);

	// envio para o TX da USART3 o valor da temperatura
//	mostraTemperaturaFloatV2(USART3, temperaturaFloat);
}

/* leitura do conversor AD canal 1. Retorna o resultado inteiro de 16 bits */
u16 conversorAD_ch1(void)
{
	u16 resultadoConvAD = 0;

	/* Start ADC1 Software Conversion */
	/* Enables the selected ADC1 software start conversion of the regular channels */
	ADC_SoftwareStartConv(ADC1);

	// aguardo o final da convers�o
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
		;

	if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != RESET)
	{
		resultadoConvAD = ADC_GetConversionValue(ADC1);
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	}

	return resultadoConvAD;
}

