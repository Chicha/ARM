/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "setup.h"
#include "loop.h"

/* typedef unions and structures */
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef       TIM_OCInitStructure;

/* Private variables */
vs32 TimingDelay;
s32  toggleLED = 0;

/* Timer 2 interrupt routine */
void TIM2_IRQHandler(void)
{
    //if interrupt happens then do this
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        /* clear interrupt flag */
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        setDutyCycle(4,getDutyCycle(4)+10);
        if (getDutyCycle(4) > get_varTIM_Period())
        {
            setDutyCycle(4,0);
        }
        TIM_SetCompare4(TIM4,getDutyCycle(4));
    }
}

void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        /* Clear the EXTI line 0 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line0);
        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == SET)
        {
            setDutyCycle(2,getDutyCycle(2)+1);
            if (getDutyCycle(2) > get_varTIM_Period())
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
    if (EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
        /* Clear the EXTI line 0 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line2);
        while (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2) == RESET)
        {
            setDutyCycle(2,getDutyCycle(2)-1);
            if (getDutyCycle(2) < 1)
            {
                setDutyCycle(2,get_varTIM_Period());
            }
            /* change Duty Cycle TIMx Channel 2 */
            TIM_SetCompare2(TIM4, getDutyCycle(2));
            Delay(1000);
        }
    }
}

/* Setup SysTick Timer for 1 msec interrupts  */
void SysTick_Handler(void)
{
    if (TimingDelay != 0)
    {
        TimingDelay--;
    }
}
