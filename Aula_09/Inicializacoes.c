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

/* Private typedef -----------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
RCC_ClocksTypeDef RCC_ClockFreq;
NVIC_InitTypeDef NVIC_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
USART_InitTypeDef USART_InitStructure;
ADC_InitTypeDef ADC_InitStructure;
DAC_InitTypeDef DAC_InitStructure;

/* Private variables ---------------------------------------------------------*/
u32 dutyClycleChannel1 = 0;
u32 dutyClycleChannel2 = 0;
u32 dutyClycleChannel3 = 0;
u32 dutyClycleChannel4 = 0;
u32 varTIM4_Period = 0;

// variáveis globais, para serem monitoradas pelo STMStudio
float temperaturaFloat;
float sinalErro;

void inicializacoes(void)
{
	Clock_Sys_Inits();
	GPIO_Inits();
	Timer2_Inits();
	Timer4_PWM_Inits();
	Timer6_Inits();
	USART_Inits();
	ADC1_Inits();
	DAC1_Inits();
	Interruption_Inits();
	SysTick_Configuration();
}

void Clock_Sys_Inits(void)
{
	SystemInit();
}

void GPIO_Inits(void)
{
	/* GPIO Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);

	/* DMA1 clock enable (to be used with DAC) */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	/* GPIOA Configurations ------------------------------------------------------------- */

	/* Configure PA0 input mode no pull up or pull down */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure PA1 ADC1 Channel pin as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* DAC channel 1 & 2 (DAC_OUT1 = PA.4)(DAC_OUT2 = PA.5) configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

//	/* Configuração da USART2
//	 * PA2 = USART2_TX --> Alternate Function as OUTPUT */
//	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//	/* Configuração da USART2
//	 * PA3 = USART2_RX --> Alternate Function as INPUT PULL DOWN */
//	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);

//	/* Configuração da USART1
//	 * PA9 = USART1_TX --> Alternate Function as OUTPUT */
//	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//	/* Configuração da USART1
//	 * PA10 = USART1_RX --> Alternate Function as INPUT PULL DOWN */
//	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* GPIOB Configurations ------------------------------------------------------------- */

	/* Configure PB1 output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure PB2 input mode internal pull up */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure PB3, PB4, PB5 for LEDs output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure PB7, PB8 in Alternate Function mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Alternating functions for pins PB7, PB8 ==> connect to TIM4 PWM OUTPUT*/
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM4);

	/* Configuração da USART3
	 * PB10 = USART3_TX --> Alternate Function as OUTPUT */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configuração da USART3
	 * PB11 = USART3_RX --> Alternate Function as INPUT */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* GPIOC Configurations ------------------------------------------------------------- */

	/* GPIOD Configurations ------------------------------------------------------------- */

	/* Configure PD12, PD13 in Alternate Function mode */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
	/* Alternating functions for pins PD12, PD13 ==> connect to TIM4 PWM OUTPUT*/
//	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
//	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
	/* Configure PD12, PD13, PD14, PD15 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void USART_Inits(void)
{
	// USARTs initialization

	/*  Deinitializes the USARTx peripheral registers to their default reset values. */
//	USART_DeInit(USART1);
//	USART_DeInit(USART2);
	USART_DeInit(USART3);

	/* Enable USARTs clock     */
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* USART configured as follow:
	 - BaudRate = 9600 baud
	 - Word Length = 8 Bits
	 - One Stop Bit
	 - No parity
	 - Hardware flow control disabled (No RTS and CTS signals)
	 - Receive and transmit enabled
	 - USART Clock disabled
	 - USART CPOL: Clock is active low
	 - USART CPHA: Data is captured on the middle
	 - USART LastBit: The clock pulse of the last data bit is not output to the SCLK pin
	 */
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	// initializes USART1, USART2 e USART3
//	USART_Init(USART1, &USART_InitStructure);
//	USART_Init(USART2, &USART_InitStructure);
	USART_Init(USART3, &USART_InitStructure);

	// USART1, USART2 e USART3 enable
//	USART_Cmd(USART1, ENABLE);
//	USART_Cmd(USART2, ENABLE);
	USART_Cmd(USART3, ENABLE);

	/* Enable the USART Receive interrupt: this interrupt is generated when the USART receive data register is not empty */
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

void Timer2_Inits(void)
{
	// habilito o Clock do Timer 2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	// Fills each TIM_TimeBaseInitStruct member with its default value.
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

	// TIM_Prescaler = (84 MHz * T) / 10000 = 8400 * T
	// para T=0,5s ==> TIM_Prescaler = 4200

	//setting timer 2 interrupt to 10hz (84 MHz / 840 / 10000 = 10hz --> T=0,1s)
	// interrupção a cada 100 ms --> delta t do controlador PID;
	TIM_TimeBaseStructure.TIM_Prescaler = 840;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 10000 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* Enables or disables the specified TIM interrupts. */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);
}

void Timer4_PWM_Inits(void)
{
	/*
	 * Alternate functions
	 * PB7 --> TIMR4 Channel 2 PWM output
	 * PB8 --> TIMR4 Channel 3 PWM output
	 * */

	/* TIM4 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	// Fills each TIM_TimeBaseInitStruct member with its default value.
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

	/*
	 * TIM4 is connected to APB1 bus --> works at 84MHz by default
	 * Timer count frequency is set with:
	 * timer_tick_frequency = Timer_default_frequency / (TIM_Prescaler + 1)
	 * doing TIM_Prescaler = 1 then
	 * timer_tick_frequency = 84000000 / (1 + 1) = 42000000
	 *
	 * Timer 4 is 16 bits ==> it counts from 0 to 65535 (maximum value for TIM_Prescaler and TIM_Period)
	 *
	 * PWM_frequency = timer_tick_frequency / (TIM_Period + 1)
	 * or TIM_Period = (timer_tick_frequency / PWM_frequency) - 1
	 * if PWM_frequency = 1 kHz
	 * TIM_Period = (42000000 / 1000) - 1 = 839
	 *
	 * */

	/* public Timer Period variable */
	varTIM4_Period = 41999;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = varTIM4_Period;
	TIM_TimeBaseStructure.TIM_Prescaler = 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/*
	 * PWM Mode configuration
	 * Duty Cycle equation:
	 * pulse_length = ((TIM_Period + 1) * DutyCycle) / 100 - 1
	 * where DutyCycle is in percent, between 0 and 100%
	 *
	 * Exemples:
	 * 5% duty cycle:     	TIM_Pulse = (((41999 + 1) * 5) / 100) - 1 = 2099
	 * 20% duty cycle:     	TIM_Pulse = (((41999 + 1) * 20) / 100) - 1 = 8399
	 * 75% duty cycle:     	TIM_Pulse = (((41999 + 1) * 75) / 100) - 1 = 31499
	 * 100% duty cycle:    	TIM_Pulse = (((41999 + 1) * 100) / 100) - 1 = 41999
	 *
	 * Remember: if (pulse_length > TIM_Period) --> output HIGH all the time
	 *
	 * */

	/* PWM Duty Cycle variables set */
	dutyClycleChannel1 = 2099; /* 5% duty cycle  */
	dutyClycleChannel2 = 8399; /* 20% duty cycle  */
	dutyClycleChannel3 = 31499; /* 75% duty cycle  */

	/* PWM Mode configuration */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	/* PWM Mode configuration: Channel1 */
//	TIM_OCInitStructure.TIM_Pulse = dutyClycleChannel1; /* 5% duty cycle  */
//	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
//	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	/* PWM Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_Pulse = dutyClycleChannel2; /* 20% duty cycle  */
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

	/* PWM Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_Pulse = dutyClycleChannel3; /* 75% duty cycle  */
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

	/* PWM Mode configuration: Channel4 */
//	TIM_OCInitStructure.TIM_Pulse = 629;  /* 75% duty cycle  */
//	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
//	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	/* TIM4 enable counter */
	TIM_Cmd(TIM4, ENABLE);
}

/**
 * @brief  TIM6 Configuration ==> usado como triger do DAC
 * @note   TIM6 configuration is based on APB1 frequency
 * @note   TIM6 Update event occurs each TIM6CLK/256
 * @param  None
 * @retval None
 */
void Timer6_Inits(void)
{
	/* TIM6 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	/* --------------------------------------------------------
	 TIM6 input clock (TIM6CLK) is set to 2 * APB1 clock (PCLK1),
	 since APB1 prescaler is different from 1.
	 TIM6CLK = 2 * PCLK1
	 TIM6CLK = HCLK / 2 = SystemCoreClock /2

	 TIM6 Update event occurs each TIM6CLK/256

	 Note:
	 SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
	 Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
	 function to update SystemCoreClock variable value. Otherwise, any configuration
	 based on this variable will be incorrect.

	 ----------------------------------------------------------- */

	/*
	 * TIM6 is connected to APB1 bus --> works at 84MHz by default
	 * Timer count frequency is set with:
	 * timer_tick_frequency = Timer_default_frequency / (TIM_Prescaler + 1)
	 */

	/* Time base configuration */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = 255;
	TIM_TimeBaseStructure.TIM_Prescaler = 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

	/* TIM6 TRGO selection */
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

	/* TIM6 enable counter */
	TIM_Cmd(TIM6, ENABLE);
}

void ADC1_Inits(void)
{
	/* Fills each ADC_InitStruct member with its default value.
	 * Reset ADC init structure parameters values */
	ADC_StructInit(&ADC_InitStructure);

	/* Enable ADC1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* ADC1 structure configurations */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 regular channel 1 configuration
	 * PA1 = [ADC1_IN1] --> ADC1 Channel 1 */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_112Cycles);

//	ADC_TempSensorVrefintCmd(ENABLE);
//
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 1, ADC_SampleTime_112Cycles);

	/* disables the ADC1 continuous conversion mode  */
	ADC_ContinuousModeCmd(ADC1, DISABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* Enables the selected ADC1 software start conversion of the regular channels */
	ADC_SoftwareStartConv(ADC1);

	/* Returns the last ADC1 conversion result data for regular channel */
	ADC_GetConversionValue(ADC1);
}

void DAC1_Inits(void)
{
	/* DAC Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	/* Deinitializes the DAC peripheral registers to their default reset values */
	DAC_DeInit();

	/* DAC channel1 and channel basic Configuration */
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);

	/* Enable DAC Channel1 and channel 2 */
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_Cmd(DAC_Channel_2, ENABLE);

	/* set zero to outputs */
	DAC_SetDualChannelData(DAC_Align_12b_R, 0, 0);
}

void Interruption_Inits(void)
{
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Connect EXTI Line0 to PA0 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	/* Connect EXTI Line2 to PB2 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource2);

	/* Configure EXTI Line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Configure EXTI Line2 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable and set EXTI Line2 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable Timer2 IRQ */
	NVIC_ClearPendingIRQ(TIM2_IRQn);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable and set USART1 Interrupt to the lowest priority */
//	NVIC_ClearPendingIRQ(USART1_IRQn);
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	/* Enable and set USART2 Interrupt to the lowest priority */
//	NVIC_ClearPendingIRQ(USART2_IRQn);
//	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	/* Enable and set USART3 Interrupt to the lowest priority */
	NVIC_ClearPendingIRQ(USART3_IRQn);
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Configure the SysTick Handler Priority: Preemption priority and subpriority */
	/* Set SysTick Priority to 3 */
	NVIC_SetPriority(SysTick_IRQn, 3);
}

void SysTick_Configuration(void)
{
	/* Setup SysTick Timer for 1 msec interrupts  */
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		/* Capture error */
		while (1)
			;
	}
}
