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
#include "Inicializacoes.h"
#include "LoopPrincipal.h"
#include "ISR.h"
#include "Controlador_OnOff.h"


/**
 * @brief 	controle da temperatura no modo ON/OFF
 * 			temperaturaFloat e sinalErro são variáveis globais
 * @param 	float setPoint ==> valor do set point da temperatura
 * @retval 	sem
 */
void controleON_OFF(float setPoint)
{
	u32 resConvAD = mediaLeituraSensorTemperatura(10);
	temperaturaFloat = calculoTempFloat(resConvAD);

	sinalErro = setPoint - temperaturaFloat;

	// erro Negativo --> temperaturaFloat(medida) > setPoint --> Desligo(OFF) a saída de comando!
	if (sinalErro < 0.0)
	{
		// PB5 --> saída de controle (p/ entrada da interface de potência com a carga)
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);
		// PD15 --> LED piloto (LED Azul da placa)
		GPIO_ResetBits(GPIOD, GPIO_Pin_15);
	}
	// erro Positivo --> temperaturaFloat(medida) < setPoint --> Ligo(ON) a saída de comando!
	else
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_5);
		GPIO_SetBits(GPIOD, GPIO_Pin_15);
	}

	mostraTemperaturaFloatV2(USART3, temperaturaFloat);
	mostraSinalErroFloat(sinalErro);
}
