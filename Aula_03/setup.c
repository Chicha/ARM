/*EXPERIMENTO 3
-Programar o Timer 4 p/ gerar PWM com frequencia de 1 KHz
- Programar os canais 1,2,3 do Timer 4 nas saidas dos LED's 1,2,3 (P12, PD13, PD14). -> Alternate Function
- Inicializar o PWM p/ operar com o Duty cycle: LED1 -> 5%, LED2 -> 20%, LED3 -> 75%
- Habilitar um botao no pino PB2 p/ Funcionar por interrupcao , utilizando o pull-up interno.
- Rotina principal: variar o duty cycle do LED1 de 0% a 100% em loop.
- Variar Duty Cycle do LED2 de 100% a 0% em loop. */

/* Includes */
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "setup.h"

/* Private typedef */
GPIO_InitTypeDef        GPIO_InitStructure;
RCC_ClocksTypeDef       RCC_ClockFreq;
NVIC_InitTypeDef        NVIC_InitStructure;
EXTI_InitTypeDef        EXTI_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef       TIM_OCInitStructure;

/* Private variables */
u32 dutyCycle[4] = {0,0,0,0};
u32 varTIM_Period = 41999;

/* Prototypes of functions */
void set_varTIM_Period(u32 valor);
u32  get_varTIM_Period();
u32  getDutyCycle(u32 channel);
void setDutyCycle(u32 channel,u32 valor);
void setup(void);
void setup_Clock_Sys(void);
void setup_GPIO(void);
void setup_GPIO_PIN();
void setup_Timer4_PWM(void);
void setup_Timer_2_Priority(void);
void setup_Timer_2();
void setup_Interrupt(void);
void SysTick_Configuration(void);

/* Functions of variables */
void set_varTIM_Period(u32 valor)
{
    varTIM_Period = valor;
}

u32 getDutyCycle(u32 channel)
{
    return dutyCycle[channel];
}

void setDutyCycle(u32 channel,u32 valor)
{
    dutyCycle[channel] = valor;
}

u32 get_varTIM_Period()
{
    return varTIM_Period;
}

/* Function for setups */
void setup(void)
{
    setup_Clock_Sys();
    setup_GPIO();
    setup_Timer4_PWM();
    setup_Timer_2();
    setup_Interrupt();
    SysTick_Configuration();
}

void setup_Clock_Sys(void)
{
    SystemInit();
}

void setup_GPIO(void)
{
    /* GPIOD Periph clock enable */
    RCC_AHB1PeriphClockCmd(
        RCC_AHB1Periph_GPIOA |
        RCC_AHB1Periph_GPIOB |
        RCC_AHB1Periph_GPIOC |
        RCC_AHB1Periph_GPIOD,
        ENABLE);

    /* Configure PD12, PD13, PD14 in Alternate Function mode */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF; //MODE ALTERNATE FUNCTION
    setup_GPIO_PIN();
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Alternating functions for pins PD12, PD13, PD14 ==> connect to TIM4 PWM OUTPUT*/
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4); // MODE ALTERNATE FUNCTION
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Configure PB1 output pushpull mode */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    setup_GPIO_PIN();
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure PA0 input mode no pull up or pull down */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    setup_GPIO_PIN();
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure PB2 input mode internal pull up */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void setup_GPIO_PIN()
{
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
}

void setup_Timer4_PWM(void)
{
    /* TIM4 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    // Fills each TIM_TimeBaseInitStruct member with its default value.
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

    varTIM_Period = 41999;

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period        = varTIM_Period;
    TIM_TimeBaseStructure.TIM_Prescaler     = 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    /* PWM Duty Cycle variables set */
    dutyCycle[0] =  2099; /*   5% duty cycle */
    dutyCycle[1] =  8399; /*  20% duty cycle */
    dutyCycle[2] = 31499; /*  75% duty cycle */
    dutyCycle[3] = 41999; /* 100% duty cycle */

    /* PWM Mode configuration */
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;

    /* PWM Mode configuration: Channel 1 */
    TIM_OCInitStructure.TIM_Pulse = dutyCycle[0]; /* 05% duty cycle  */
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

    /* PWM Mode configuration: Channel 2 */
    TIM_OCInitStructure.TIM_Pulse = dutyCycle[1]; /* 20% duty cycle  */
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

    /* PWM Mode configuration: Channel 3 */
    TIM_OCInitStructure.TIM_Pulse = dutyCycle[2]; /* 75% duty cycle  */
    TIM_OC3Init(TIM4, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_Cmd(TIM4, ENABLE);
}

void setup_Timer_2_Priority()
{
    NVIC_ClearPendingIRQ(TIM2_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

void setup_Timer_2()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

    TIM_TimeBaseStructure.TIM_Prescaler     = 1;
    TIM_TimeBaseStructure.TIM_Period        = get_varTIM_Period();
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);

    setup_Timer_2_Priority();

    TIM_Cmd(TIM2, ENABLE);

}

void setup_Interrupt_PA0(void)
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
    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void setup_Interrupt_PB2(void)
{
    /* Connect EXTI Line2 to PB2 pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource2);

    /* Configure EXTI Line2 - PB2 */
    EXTI_InitStructure.EXTI_Line    = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI Line2 Interrupt to the lowest priority - PB2 */
    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

void setup_Interrupt_SysTick()
{
    /* Configure the SysTick Handler Priority: Preemption priority and subpriority */
    /* Set SysTick Priority to 1 */
    NVIC_SetPriority(SysTick_IRQn, 1);
}

void setup_Interrupt(void)
{
    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    setup_Interrupt_PA0();
    setup_Interrupt_PB2();
    setup_Interrupt_SysTick();
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
