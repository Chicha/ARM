/* Includes */
#include "misc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_adc.h"
#include "setup.h"
#include "loop.h"
#include "ISR.h"
#include "protocol.h"

/* typedef unions and structures */
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef       TIM_OCInitStructure;

/* Private variables ---------------------------------------------------------*/
vs32 TimingDelay;
s32  toggleLED = 0;
u8   aux = 0;

void USART3_IRQHandler(void);
void USART2_IRQHandler(void);

void setTimingDelay(vs32 valor)
{
    TimingDelay = valor;
}

vs32 getTimingDelay(void)
{
    return TimingDelay;
}

/**
 * @brief  This function handles USART1 global interrupt request.
 * @param  None
 * @retval None
 */
void USART1_IRQHandler(void)
{
    uint8_t USARTdata;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);

        USARTdata = USART_ReceiveData(USART1);

        /* eco char received */
        USART_SendChar(USART1, USARTdata);

    }
}

/* USART 2 Interrupt routine */
void USART2_IRQHandler(void)
{

    uint8_t USARTdata;

    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {

        USART_ClearITPendingBit(USART2, USART_IT_RXNE);

        USARTdata = USART_ReceiveData(USART2);

        // Eco char received
        USART_SendChar(USART2, USARTdata);

        rxProtocol(USARTdata);

    }

}

/* USART 3 Interrupt routine */
void USART3_IRQHandler(void)
{

    uint8_t USARTdata;

    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {

        USART_ClearITPendingBit(USART3, USART_IT_RXNE);

        USARTdata = USART_ReceiveData(USART3);

        /* eco char received */
        USART_SendChar(USART3, USARTdata);

        GPIO_ToggleBits(GPIOD, GPIO_Pin_15);

    }
}

/* Timer 2 interrupt routine */
void TIM2_IRQHandler(void)
{
    /* if interrupt happens then do this */
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {

        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //clear interrupt flag

        /* Toggle LED */
        GPIO_ToggleBits(GPIOB, GPIO_Pin_4);

    }
}

/* Extern Interrupt 0 routine */
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        /* Clear the EXTI line 0 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line0);

        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == SET)
        {

            setDutyCycle(2,getDutyCycle(2)+1);

            if (getDutyCycle(2) > getVarTIM_Period())
            {
                setDutyCycle(2,0);
            }

            /* change Duty Cycle TIMx Channel 2 */
            TIM_SetCompare2(TIM4, getDutyCycle(2));

            Delay(1000);

        }
    }
}

void EXTI2_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line2))
    {
        /* Clear the EXTI line 2 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

/* Setup SysTick Timer for 1 msec interrupts  */
void SysTick_Handler(void)
{
    if (TimingDelay != 0)
    {
        TimingDelay--;
    }

    if(--pacote != 0)
    {
        aux = 1;
    }
    else
    {
        posicao = 0;
    }
}
