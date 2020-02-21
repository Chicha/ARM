/* Includes                                             */
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_tim.h"

/* Variaveis globais do arquivo                         */
GPIO_InitTypeDef        GPIO_InitStructure;
RCC_ClocksTypeDef       RCC_ClockFreq;
NVIC_InitTypeDef        NVIC_InitStructure;
EXTI_InitTypeDef        EXTI_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef       TIM_OCInitStructure;

/* Funcoes do arquivo                                   */
void setup(void);
void setup_CLK_SYS(void);
void setup_GPIO(void);
void setup_Interrupt(void);
void setup_Timer2(void);
void setup_Timer2_Priority(void);
void setup_EXTI0_interrupt(void);
void setup_EXTI0_Priority();
void setup_SysTick(void);
void setup_SysTick_Priority();
void setup_GPIO_OUT(void);
void setup_GPIO_IN(void);

/* Funcoes de configuracoes                             */
void setup(void)
{
	setup_CLK_SYS();
	setup_GPIO();
	setup_Timer2();
	setup_SysTick();
	setup_Interrupt();
}

/* Configura o clock interno do ARM                     */
void setup_CLK_SYS()
{
    SystemInit();
}

/* Habilita a portas de entradas do sistema             */
void setup_GPIO(void)
{
  /* Enable ports A, B, C and D */
    RCC_AHB1PeriphClockCmd(
    RCC_AHB1Periph_GPIOA | /* Enable port A      */
    RCC_AHB1Periph_GPIOB | /* Enable port B      */
    RCC_AHB1Periph_GPIOC | /* Enable port C      */
    RCC_AHB1Periph_GPIOD   /* Enable port D      */
    , ENABLE);             /* Set port as Enable */

    /* Configure PD12, PD13, PD14 and PD15 in output pushpull mode 	*/
    GPIO_InitStructure.GPIO_Pin =
    GPIO_Pin_12 | /* Setup pin 12 */
    GPIO_Pin_13 | /* Setup pin 13 */
    GPIO_Pin_14 | /* Setup pin 14 */
    GPIO_Pin_15;  /* Setup pin 15 */

    /* Set port as OUTPUT                                           */
    setup_GPIO_OUT();
    GPIO_Init(GPIOD,&GPIO_InitStructure);

    /* Configure PB1 output pushpull mode                           */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    /* Set port as OUTPUT                                           */
    setup_GPIO_OUT();
    /* Setup port B with values of GPIO_InitStructure               */
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure PA0 input mode without pull up or pull down        */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    /* Set port as INPUT                                            */
    setup_GPIO_IN();
    /* Setup port B with values of GPIO_InitStructure               */
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/* Setup port as OUTPUT                                 */
void setup_GPIO_OUT()
{
    /* Set pin as OUTPUT        */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    /* Set port as PULLUP       */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    /* Set frequency of port    */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    /* Set port without Pull    */
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
}

/* Setup port as INPUT                                  */
void setup_GPIO_IN()
{
    /* Set pin as INPUT             */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    /* Chose frequency of port      */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    /* Port without PULL            */
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
}

/* setup_Timer2(...)                                    *
 * Capture Compare 2 interrupt Flag (CC2IF)             *
 * generate interrupt every 250ms (0.25s)               *
 * SystemCoreClock = 168MHz                             *
 * TIMxCLK = SystemCoreClock/2                          *
 * Hence ticks = 0.1*168M/2 = 8,4M                      *
 * Prescaler = 8400-1; ARR = 1000 - 1                   */
void setup_Timer2()
{
    /* Enable clock of Timer 2                          */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    /* Fills each TIM_TimeBaseStructure default values  */
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    /* Setting timer 2 interrupt to 5 Hz                *
     * frequencia do barramento prescaler auto-reload   *
     * (84 MHz / 1440 /11666 = 4 Hz --> T = 0.25 s)     *
     * prescaler = 5*11666/84e                          *
     * Set a prescaler as 1440                          */
    TIM_TimeBaseStructure.TIM_Prescaler     = 1440;
    /* Set a counter up                                 */
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    /* Set value of reload                              */
    TIM_TimeBaseStructure.TIM_Period        = 11666 - 1;
    /* Set clock division                               */
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
    /* Enables or disables the specified TIM interrupts */
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    /* TIM2 enable counter                              */
    TIM_Cmd(TIM2, ENABLE);
    /* Configura a prioridade do Timer2 */
    setup_Timer2_Priority();
}

/* Configura as interrupcoes                            */
void setup_Interrupt(void)
{
    /* Enable SYSCFG clock  - Habilita o barramento APB2 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    /* Habilita o timer 2       */
    setup_Timer2();
    /* Habilita a interrupcao 0 */
    setup_EXTI0_interrupt();
    /* Habilita o SysTIck       */
    setup_SysTick_Priority();
}

/* Configura a interrupcao externa 0                    */
void setup_EXTI0_interrupt()
{
    /* Connect EXTI Line0 to PA0 pin                    */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
    /* Configure EXTI Line0                             */
    EXTI_InitStructure.EXTI_Line    = EXTI_Line0;
    /* Configura como interrupcao externa               */
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    /* Interrupcao por borda de subida                  */
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    /* Habilita interrupcao externa 0                   */
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    /* Funcao que configura a prioridade da interrupcao */
    setup_EXTI0_Priority();
}

/* Configura a prioridade da interrupcao externa 0      */
void setup_EXTI0_Priority()
{
    /* Enable and set EXTI Line0 Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/* Configura a prioridade do timer 2                    */
void setup_Timer2_Priority(void)
{
    /* Enable Timer2 IRQ                                */
    NVIC_ClearPendingIRQ(TIM2_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/* Configura o relogio interno                          */
void setup_SysTick_Priority()
{
    NVIC_InitStructure.NVIC_IRQChannel    = SysTick_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /* Configure the SysTick Handler Priority: *
     * Preemption priority and subpriority      *
     * Set SysTick Priority to 3                */
    NVIC_SetPriority(SysTick_IRQn, 3);
}

/* Interrupcao a cada 1ms                               */
void setup_SysTick(void)
{
    /* Setup SysTick Timer for 1ms interrupt    */
    if(SysTick_Config(SystemCoreClock / 1000))
    {
        /* Capture error                        */
        while(1);
    }
}
