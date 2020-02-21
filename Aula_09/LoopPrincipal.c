/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dac.h"
#include "Inicializacoes.h"
#include "LoopPrincipal.h"
#include "ISR.h"
#include "Controlador_OnOff.h"
#include "Controlador_PID.h"
#include <stdio.h>
#include "DAC_GeradorDeSinais.h"

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
u8 var = 0;

/* Public variables ---------------------------------------------------------*/
u8 RxVector[10];

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Loop Principal
 * @param  None
 * @retval None
 */
void loopPrincipal(void)
{
	principalInits();

	// programação do DAC como gerador de sinais
	DAC_Main();

	while (1)
	{
		//DAC_denteDeSerra();

		DAC_triangular();

		GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
	}
}

/* sinal dente de serra no canal 1 (PA4) do DAC */
void DAC_denteDeSerra(void)
{
	s32 var;
	for (var = 0; var < 4096; ++var)
	{
		DAC_SetChannel1Data(DAC_Align_12b_R, var);
		Delay(10);
	}

//	Delay(100000);
}

/* sinal triangular no canal 2 (PA5) do DAC */
void DAC_triangular(void)
{
	s32 var;
	for (var = 0; var < 4096; ++var)
	{
		DAC_SetChannel2Data(DAC_Align_12b_R, var);
		Delay(10);
	}
	for (var = 4095; var > 0; --var)
	{
		DAC_SetChannel2Data(DAC_Align_12b_R, var);
		Delay(10);
	}

//	Delay(100000);
}

/**
 * @brief  inicializações das variáveis da rotina principal.
 * @param  None
 * @retval None
 */
void principalInits(void)
{
	u8 var;
	// inicalizo a variável pública RxVector
	for (var = 0; var < 10; ++var)
	{
		RxVector[var] = 0;
	}

	// teste de envio de um número int 32 bits via USART
	printIntHex(0x01ABCDEF);

	// outra forma de se enviar uma string, enviando também o tamanho da mesma
	u8 frase3[] = "Teste USART3 ==> OK!\r\n";
	UARTSend(USART3, frase3, sizeof(frase3));

	// rotina para se enviar string via USART
	UART_SendString(USART3, "programação do DAC como gerador de sinais\r\n");
}

/**
 * @brief  rotina que calcula o valor real (float) da temperatura lida no sensor LM35
 * @param  valor inteiro de 12 bits lido no conv. A/D ajustado para direita (u32 resIntConvAD)
 * @retval retorno o valor real (float) do valor lido da temperatura
 */
float calculoTempFloat(u32 resIntConvAD)
{
	// Vdd_placa V (valor medido do Vdd da placa ~3,0V)
	float VddPlaca = 2.88;

	/* LM35 ==> 10 mV/oC
	 * Temperatura = 100 x valor lido no LM35
	 * ADC ==> 12 bits ==> 0x0000 = 0 V e 0x0FFF (4095) = Vdd_placa V (valor medido do Vdd da placa ~3,0V)
	 * V_adc = (Vdd_placa / 4095) * resIntConvAD V
	 * Temperatura = 100 * V_adc */
	float tempFloat = 100 * (VddPlaca / 4095) * resIntConvAD;

	return tempFloat;
}

/**
 * @brief  rotina que envia através de uma das USARTs uma string de caracteres
 *  		o valor float da temperatura com 3 dígitos (0,00°C a 999°C)
 * @param  USARTx, valor real (float tempFloat) do valor lido da temperatura
 * @retval None
 */
void mostraTemperaturaFloatV2(USART_TypeDef* USARTx, float tempFloat)
{
	/* arredondando a temperatura lida para três dígitos (0,00°C a 999°C) */
	s32 temperaturaInt = (int) (tempFloat * 10);

	unsigned char tempChar[8];
	u8 var;
	for (var = 0; var < 7; ++var)
	{
		tempChar[var] = 0;
	}

	u32 pos = temperaturaInt / 1000;
	if (pos == 0)
		tempChar[0] = ' ';
	else
		tempChar[0] = convNumEmCaractere(pos);

	pos = (temperaturaInt / 100) % 100;
	tempChar[1] = convNumEmCaractere(pos);
	pos = (temperaturaInt / 10) % 10;
	tempChar[2] = convNumEmCaractere(pos);
	tempChar[3] = '.';
	pos = temperaturaInt % 10;
	tempChar[4] = convNumEmCaractere(pos);
	tempChar[5] = '°';
	tempChar[6] = 'C';

	UART_SendString(USARTx, "Temperatura: ");

	UARTSend(USARTx, tempChar, sizeof(tempChar));

	UART_SendString(USARTx, "\r\n");
}

/**
 * @brief  rotina que envia através das USART3 uma string de
 * 			caracteres correspondente ao valor float da temperatura
 * @param  valor real (float tempFloat) do valor lido da temperatura
 * @retval None
 */
void mostraTemperaturaFloat(float tempFloat)
{
	/* supondo a temperatura lida com dois dígitos (0°C a 99°C)
	 * faz a a conversão da média da temperatura para inteiro e x10 */
	s32 temperaturaInt = (int) (tempFloat * 10);
	unsigned char tempChar[7];
	u8 var;
	for (var = 0; var < 6; ++var)
	{
		tempChar[var] = 0;
	}

	u32 pos = temperaturaInt / 100;
	tempChar[0] = convNumEmCaractere(pos);
	pos = (temperaturaInt / 10) % 10;
	tempChar[1] = convNumEmCaractere(pos);
	tempChar[2] = '.';
	pos = temperaturaInt % 10;
	tempChar[3] = convNumEmCaractere(pos);
	tempChar[4] = '°';
	tempChar[5] = 'C';

	UART_SendString(USART3, "Temperatura Medida: ");

	UARTSend(USART3, tempChar, sizeof(tempChar));

	UART_SendString(USART3, "\r\n");
}

/**
 * @brief  rotina que envia através das USART3 uma string de caracteres
 * 			correspondente ao valor float do sinal de erro da temperatura
 * 			medido nos controladores
 * @param  valor real (float tempFloat) do valor lido da temperatura
 * @retval None
 */
void mostraSinalErroFloat(float sinalErroFloat)
{
	// faz a a conversão da média do erro para inteiro e x10
	s32 erroInt = (int) (sinalErroFloat * 10);
	unsigned char tempChar[8];
	u8 var;
	for (var = 0; var < 7; ++var)
	{
		tempChar[var] = 0;
	}

	unsigned char sinal = ' ';
	if (sinalErro < 0.0)
	{
		sinal = '-';
		// valor absoluto do erro para ser convertido em string de caracteres
		erroInt = -erroInt;
	}
	tempChar[0] = sinal;

	u32 pos = erroInt / 100;
	tempChar[1] = convNumEmCaractere(pos);
	pos = (erroInt / 10) % 10;
	tempChar[2] = convNumEmCaractere(pos);
	tempChar[3] = '.';
	pos = erroInt % 10;
	tempChar[4] = convNumEmCaractere(pos);
	tempChar[5] = '°';
	tempChar[6] = 'C';

	UART_SendString(USART3, "erro: ");

	UARTSend(USART3, tempChar, sizeof(tempChar));

	UART_SendString(USART3, "\r\n");
}

/**
 * @brief  Média aritmética da leitura do sensor de temperatura LM35
 * @param  número de itereções para cálculo da média dos valores lidos
 * @retval valor inteiro médio de 12 bits da leitura do sensor
 */
u32 mediaLeituraSensorTemperatura(u32 nrIteracoes)
{
	if (nrIteracoes == 0)
		return 0;

	u32 mediaResConv = 0;
	u32 var;
	// média das leituras do ADC --> estabilização do sinal
	for (var = 0; var < nrIteracoes; ++var)
	{
		mediaResConv = mediaResConv + startConversorADC1();
	}
	mediaResConv = mediaResConv / nrIteracoes;

	return mediaResConv;
}

/**
 * @brief  Leitura do sensor de temperatura LM35
 * 		   envio o resultado inteiro via USART
 * @param  None
 * @retval None
 */
void leituraSensorTemperatura(void)
{
	u32 resConv = 0;
	u32 var;
	// média das leituras do ADC --> estabilização do sinal
	for (var = 0; var < 10; ++var)
	{
		resConv = resConv + startConversorADC1();

	}
	resConv = resConv / 10;
	printIntHex(resConv);
}

/**
 * @brief  Leitura do conversor ADC1 canal 1 (PA1)
 * @param  None
 * @retval resultado da conversão = inteiro de 16 bits
 */
u16 startConversorADC1(void)
{
	u16 resultadoConvAD = 0;

	/* Start ADC1 Software Conversion
	 * Enables the selected ADC1 software start conversion of the regular channels */
	ADC_SoftwareStartConv(ADC1);

	// aguardo o final da conversão
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
		;

	// guardo o resultado da conversão na variável inteira de 16 bits
	if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != RESET)
	{
		resultadoConvAD = ADC_GetConversionValue(ADC1);
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	}

	// retorno o resultado da conversão
	return resultadoConvAD;
}

/**
 * @brief  Envia via RS232 (USART3) um valor inteiro de 32 bits transformado em string de caracteres
 * @param  int ivalue 32 bits
 * @retval None
 */
void printIntHex(int ivalue)
{
	int i, j;
	unsigned char c[10];
	for (i = 0; i < 8; i++)
	{
		j = ivalue >> (i * 4) & 0x0F;
		if (j < 10)
		{
			c[7 - i] = j + '0';
		}
		else
		{
			c[7 - i] = j + 55;
		}
	}
	c[8] = '\r';
	c[9] = '\n';

	UARTSend(USART3, c, sizeof(c));
}

/**
 * @brief  rotina que envia através de uma das USARTs uma string de caracteres
 *  		o valor do número inteiro sinalizado com 3 dígitos (0 a +/-999)
 * @param  USARTx, número inteiro s32 intNumero
 * @retval None
 */
void mostraNumeroViaUSART(USART_TypeDef* USARTx, s32 intNumero)
{
	unsigned char tempChar[6];
	u8 var;
	for (var = 0; var < 6; ++var)
	{
		tempChar[var] = 0;
	}

	u32 numAbs;
	unsigned char sinal = ' ';
	if (intNumero < 0)
	{
		sinal = '-';
		// valor absoluto do numero para ser convertido em string de caracteres
		numAbs = -intNumero;
	}
	else
		numAbs = intNumero;

	tempChar[0] = sinal;

	u32 pos = numAbs / 100;
	tempChar[1] = convNumEmCaractere(pos);
	pos = (numAbs / 10) % 10;
	tempChar[2] = convNumEmCaractere(pos);
	pos = numAbs % 10;
	tempChar[3] = convNumEmCaractere(pos);

	UARTSend(USARTx, tempChar, sizeof(tempChar));
	UART_SendString(USARTx, "\r\n");
}

void mostrafloatViaUSART(USART_TypeDef* USARTx, s32 intNumero)
{
	unsigned char tempChar[6];
	u8 var;
	for (var = 0; var < 6; ++var)
	{
		tempChar[var] = 0;
	}

	u32 numAbs;
	unsigned char sinal = ' ';
	if (intNumero < 0)
	{
		sinal = '-';
		// valor absoluto do numero para ser convertido em string de caracteres
		numAbs = -intNumero;
	}
	else
		numAbs = intNumero;

	tempChar[0] = sinal;

	u32 pos = numAbs / 100;
	tempChar[1] = convNumEmCaractere(pos);
	pos = (numAbs / 10) % 10;
	tempChar[2] = convNumEmCaractere(pos);
	tempChar[3] = '.';
	pos = numAbs % 10;
	tempChar[4] = convNumEmCaractere(pos);

	UARTSend(USARTx, tempChar, sizeof(tempChar));
	UART_SendString(USARTx, "\r\n");
}

void piscaLED1(void)
{
	delay_ms(1000);

	/* Toggle LED */
	GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
}

void piscaLED_D14(void)
{
	GPIO_SetBits(GPIOD, GPIO_Pin_14);

	delay_ms(100);

	GPIO_ResetBits(GPIOD, GPIO_Pin_14);

	delay_ms(900);
}

void pwm_DC(void)
{
	u32 var;
	for (var = 0; var < varTIM4_Period; ++var)
	{
		/* change Duty Cycle TIMx Channel 1 */
		TIM_SetCompare1(TIM4, var);

		if (var < 2000)
			Delay(100000);
		else if (var >= 2000 && var < 6000)
			Delay(10000);
		else
			Delay(1000);
	}
}

/**
 * @brief 	converte um número inteiro de um dígito (0 à 9) em caractere ASCII
 * @param 	Numero (int)
 * @retval 	char (caractere ASCII)
 */
char convNumEmCaractere(int Numero)
{
	unsigned char Caractere;

	if (Numero > 0 && Numero < 10)
	{
		Caractere = Numero + 0x30;
	}
	else
	{
		Caractere = '0';
	}

	return (Caractere);
}

/**
 * @brief  Delay Function.
 * @param  nCount:specifies the Delay time length.
 * @retval None
 */
void Delay(__IO uint32_t nCount)
{
	while (nCount--)
	{
	}
}

/**
 * @brief  Delay Function in ms.
 * @param  miliSegundos: specifies the Delay time length in ms.
 * @retval None
 */
void delay_ms(u32 miliSegundos)
{
	TimingDelay = miliSegundos;
	while (TimingDelay)
		;
}

/*******************************************************************************
 * Function Name  : UARTSend
 * Description    : Send a string to the UART.
 * Input          : - pucBuffer: buffers to be printed.
 *                : - ulCount  : buffer's length
 * Output         : None
 * Return         : None
 *******************************************************************************/
void UARTSend(USART_TypeDef* USARTx, unsigned char *pucBuffer, unsigned long ulCount)
{
	while (--ulCount)
	{
		USART_SendChar(USARTx, *pucBuffer++);
	}
}

/*****************************************************************************
 * \brief		Send a string over the UART port. You can use printf() to send
 * 				a formated String over USART
 * 				O compilador sempre coloca no final da string um caracter nulo ('\0')
 * \param[in]	UARTx	The UART port you want to use.
 * \param[in]	pStr	Pointer to the string you want to send.
 * \return		None
 *****************************************************************************
 */
void UART_SendString(USART_TypeDef *UARTx, char *pStr)
{

	while (*pStr)
	{
		USART_SendChar(UARTx, *pStr);
		pStr++;
	}
}

/**
 * @brief:  Send a character to the UART.
 * @param:  USARTx: Select the USART or the UART peripheral.
 *   This parameter can be one of the following values:
 *   USART1, USART2, USART3, UART4 or UART5.
 * @param:  Char: the data to transmit.
 * @retval: Nome
 */
void USART_SendChar(USART_TypeDef* USARTx, uint8_t Char)
{
	/* Loop until Transmit data register empty flag */
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
		;
	USART_SendData(USARTx, Char);
	/* Loop until the end of transmission = Transmission Complete flag */
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
		;
}

