/* Includes */
#include "misc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dac.h"
#include "setup.h"

/* Private typedef */
DAC_InitTypeDef         DAC_InitStructure;
GPIO_InitTypeDef        GPIO_InitStructure;
RCC_ClocksTypeDef       RCC_ClockFreq;
EXTI_InitTypeDef        EXTI_InitStructure;
NVIC_InitTypeDef        NVIC_InitStructure;
USART_InitTypeDef       USART_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef       TIM_OCInitStructure;
ADC_InitTypeDef         ADC_InitStructure;

/* Private variables */
u32 dutyCycle[4]  = {0,0,0,0};
u32 varTIM_Period = 0;

/* Prototype functions */
u32  getVarTIM_Period();
void setVarTIM_Period(u32 valor);
u32  getDutyCycle(u32 channel);
void setDutyCycle(u32 channel,u32 valor);

/* setup Interrupt Functions */
void setup_Interrupt_SysTicK(void);
void setup_Interrupt_USART(void);
void setup_Interrupt_USART1(void);
void setup_Interrupt_USART2(void);
void setup_Interrupt_USART3(void);
void setup_Interrupt_Timer2(void);
void setup_Interrupt_Line(void);
void setup_Interrupt_Line0(void);
void setup_Interrupt_Line2(void);
void setup_Interrupt(void);

/* Functions setup Timers */
void setup_Timer4_PWM(void);
void setup_Timer2(void);
void setup_USART(void);

/* Function setup GPIO */
void setup_GPIO_ModeAF_NOPULL(void);
void setup_GPIO_DAC(void);
void setup_GPIO_PORTA(void);
void setup_GPIO_PORTB(void);
void setup_GPIO_PORTD(void);
void setup_GPIO_USART(void);
void setup_GPIO_USART3(void);
void setup_GPIO_USART2(void);
void setup_GPIO_USART1(void);
void setup_GPIO(void);

/* Clocks Sys */
void SysTick_Configuration(void);
void setup_Clock_Sys(void);

/* Main function */
void setup(void);

/* Functions */
u32 getVarTIM_Period()
{
    return varTIM_Period;
}

void setVarTIM_Period(u32 valor)
{
    varTIM_Period = valor;
}

u32 getDutyCycle(u32 channel)
{
    return dutyCycle[channel-1];
}

void setDutyCycle(u32 channel,u32 valor)
{
    dutyCycle[channel-1] = valor;
}

void SysTick_Configuration(void)
{
    /* Setup SysTick Timer for 1 msec interrupts  */
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        /* Capture error */
        while (1);
    }
}

void setup_Interrupt_SysTicK(void)
{
    NVIC_InitStructure.NVIC_IRQChannel    = SysTick_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /* Configure the SysTick Handler Priority: Preemption priority and subpriority */
    /* Set SysTick Priority to 3 */
    NVIC_SetPriority(SysTick_IRQn, 3);
}

void setup_Interrupt_USART(void)
{
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
}

void setup_Interrupt_USART3(void)
{
    /* Enable and set USART3 Interrupt to the lowest priority */
    NVIC_ClearPendingIRQ(USART3_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    setup_Interrupt_USART();
    NVIC_Init(&NVIC_InitStructure);
}

/* Enable and set USART2 Interrupt to the lowest priority */
void setup_Interrupt_USART2(void)
{
    NVIC_ClearPendingIRQ(USART2_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/* Enable and set USART1 Interrupt to the lowest priority */
void setup_Interrupt_USART1(void)
{
    /* Enable and set USART2 Interrupt to the lowest priority */
    NVIC_ClearPendingIRQ(USART1_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

void setup_Interrupt_Timer2(void)
{
    /* Enable Timer2 IRQ */
    NVIC_ClearPendingIRQ(TIM2_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void setup_Interrupt_Line(void)
{
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
}

void setup_Interrupt_Line0(void)
{
    /* Connect EXTI Line0 to PA0 pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
    /* Configure EXTI Line0 */
    EXTI_InitStructure.EXTI_Line    = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    /* Enable and set EXTI Line0 Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    setup_Interrupt_Line();
    NVIC_Init(&NVIC_InitStructure);
}

void setup_Interrupt_Line2(void)
{
    /* Connect EXTI Line2 to PB2 pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource2);
    /* Configure EXTI Line2 */
    EXTI_InitStructure.EXTI_Line    = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    /* Enable and set EXTI Line2 Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    setup_Interrupt_Line();
    NVIC_Init(&NVIC_InitStructure);
}

void setup_Interrupt(void)
{
    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    setup_Interrupt_Line0();
    setup_Interrupt_Line2();
    setup_Interrupt_Timer2();
    setup_Interrupt_USART1();
    setup_Interrupt_USART2();
    setup_Interrupt_USART3();
    setup_Interrupt_SysTicK();
}

void setup_Timer2(void)
{
    /* Enable clock of Timer 2 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

    /* Fill eaach TIM_TimeBaseInitStruct member with its default value  */
    /* setting timer 2 interrupt to 2hz (84 MHz / 3600 / 11666 = 2hz --> T=1/2s) */
    TIM_TimeBaseStructure.TIM_Prescaler     = 3600;
    TIM_TimeBaseStructure.TIM_Period        = 11666 - 1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    /* Enables or disables the specified TIM interrupts */
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);

    /* TIM2 enable counter */
    TIM_Cmd(TIM2,ENABLE);

}

void setup_Timer4_PWM(void)
{

    /* TIM4 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

    /* Fill each TIM_TimeBaseStructInit member with its default value */
    /* public Timer Period variable */
    varTIM_Period = 41999;

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period        = varTIM_Period;
    TIM_TimeBaseStructure.TIM_Prescaler     = 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    /* PWM Duty Cycle variables set */
    dutyCycle[0] =  2099;
    dutyCycle[1] =  8399;
    dutyCycle[2] = 31499;

    /* PWM Mode configuration */
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;

    /* PWM Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_Pulse = dutyCycle[0]; /* 5% duty cycle  */
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

    /* PWM Mode configuration: Channel2 */
    TIM_OCInitStructure.TIM_Pulse = dutyCycle[1]; /* 20% duty cycle  */
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

    /* PWM Mode configuration: Channel3 */
    TIM_OCInitStructure.TIM_Pulse = dutyCycle[2]; /* 75% duty cycle  */
    TIM_OC3Init(TIM4, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

    /* PWM Mode configuration: Channel3 */
   TIM_OCInitStructure.TIM_Pulse = dutyCycle[2]; /* 75% duty cycle  */
   TIM_OC4Init(TIM4, &TIM_OCInitStructure);
   TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

    /* TIM3 enable counter */
    TIM_Cmd(TIM4, ENABLE);
}

void setup_USART(void)
{
    /*  Reinitializes the USARTx peripheral registers to their default reset values. */
    USART_DeInit(USART1);
    USART_DeInit(USART2);
    USART_DeInit(USART3);

    /* Enable USARTs clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    USART_InitStructure.USART_BaudRate   = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits   = USART_StopBits_1;
    USART_InitStructure.USART_Parity     = USART_Parity_No;
    USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    /* Initializes USART1, USART2 and USART3 */
    USART_Init(USART1, &USART_InitStructure);
    USART_Init(USART2, &USART_InitStructure);
    USART_Init(USART3, &USART_InitStructure);

    /* USART1, USART2 e USART3 enable */
    USART_Cmd(USART1, ENABLE);
    USART_Cmd(USART2, ENABLE);
    USART_Cmd(USART3, ENABLE);

    /* Enable the USART Receive interrupt: this interrupt is
    generated when the USART receive data register is not empty */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

}

void setup_GPIO_PORTA(void)
{
    /* GPIOA Configurations */

    /* Configure PA0 input mode no pull up or pull down */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}

void setup_GPIO_PORTB(void)
{

    /* GPIOB Configurations */

    /* Configure PB1 output pushpull mode */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    /* Configure PB2 input mode internal pull up */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure PB3, PB4, PB5 for LEDs output pushpull mode */
    GPIO_InitStructure.GPIO_Pin   =
        GPIO_Pin_3 |
        GPIO_Pin_4 |
        GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

}

void setup_GPIO_PORTD(void)
{
    /* GPIOD Configurations */

    /* Configure PD12, PD13, PD14 in Alternate Function mode */
    GPIO_InitStructure.GPIO_Pin   =
        GPIO_Pin_12 |
        GPIO_Pin_13 |
        GPIO_Pin_14 |
        GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Alternating functions for pins PD12, PD13, PD14 ==> connect to TIM4 PWM OUTPUT*/
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);

}

void setup_GPIO_USART1(void)
{
    /* Configuracao da USART1
     * PA9 = USART1_TX --> Alternate Function as OUTPUT */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configuracao da USART1
     * PA10 = USART1_RX --> Alternate Function as INPUT PULL DOWN */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void setup_GPIO_USART2(void)
{
    /* Configuracao da USART2
     * PA2 = USART2_TX --> Alternate Function as OUTPUT */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configuracao da USART2
     * PA3 = USART2_RX --> Alternate Function as INPUT PULL DOWN */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void setup_GPIO_USART3(void)
{
    /* Configuracao da USART3
     * PB10 = USART3_TX --> Alternate Function as OUTPUT */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configuracao da USART3
     * PB11 = USART3_RX --> Alternate Function as INPUT */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

}

void setup_GPIO_ModeAF_NOPULL(void)
{
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
}

void setup_GPIO_USART(void)
{
    setup_GPIO_USART1();
    setup_GPIO_USART2();
    setup_GPIO_USART3();
}

void setup_GPIO_ADC(void)
{
    /* Configure PA1 ADC1 Channel 1 pin as analog input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure PA6 ADC1 Channel 6 pin as analog input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void setup_GPIO_DAC(void)
{
    /* Configure PA4 DAC1 Channel 1 pin as analog input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void setup_DAC(void)
{
	/* DAC Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	/* Deinitializes the DAC peripheral registers to their default reset values */
	DAC_DeInit();

	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	/* Fornece ganho de corrente no DAC */
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	/* Enable DAC Channel1 */
	DAC_Cmd(DAC_Channel_1, ENABLE);

	DAC_SetDualChannelData(DAC_Align_12b_R,0,0);

}
void setup_GPIO(void)
{
    /* GPIO Periph clock enable */
    RCC_AHB1PeriphClockCmd(
        RCC_AHB1Periph_GPIOA | /* Enable Port A */
        RCC_AHB1Periph_GPIOB | /* Enable Port B */
        RCC_AHB1Periph_GPIOC | /* Enable Port C */
        RCC_AHB1Periph_GPIOD , /* Enable Port D */
    ENABLE);

    /* GPIOA Configurations */
    setup_GPIO_PORTA();
    /* GPIOB Configurations */
    setup_GPIO_PORTB();
    /* GPIOC Configurations */

    /* GPIOD Configurations */
    setup_GPIO_PORTD();
    /* USART Configurations */
    setup_GPIO_USART();
    setup_GPIO_ADC();
    setup_GPIO_DAC();
}

void setup_Clock_Sys(void)
{
    SystemInit();
}

void setup_ADC(void)
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

    /* ADC1 regular channel 6 configuration
    * PA6 = [ADC1_IN6] --> ADC1 Channel 6 */
    //  ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_112Cycles);

    /* disables the ADC1 continuous conversion mode  */
    ADC_ContinuousModeCmd(ADC1, DISABLE);

    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);

    /* Enables the selected ADC1 software start conversion of the regular channels */
    ADC_SoftwareStartConv(ADC1);

    /* Returns the last ADC1 conversion result data for regular channel */
    ADC_GetConversionValue(ADC1);
}

void DMA_Setup(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

void setup(void)
{
    setup_Clock_Sys();
    setup_GPIO();
    setup_Timer2();
    setup_Timer4_PWM();
    setup_USART();
    setup_ADC();
    setup_DAC();
    setup_Interrupt();
    SysTick_Configuration();
}
