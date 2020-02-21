/* Includes */
#include "misc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_usart.h"
#include "setup.h"
#include "loop.h"
#include "ISR.h"
#include "protocol.h"
#include "DAC_GeradorDeSinais.h"
/* typedef unions and structures */
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef       TIM_OCInitStructure;

/* Private variables ---------------------------------------------------------*/
vs32 TimingDelay;
s32  toggleLED = 0;
u8   aux = 0;

u8 flagTimer2 = 0;

void setFlagTimer2(u8 status)
{
	flagTimer2 = status;
}

u8   getFlagTimer2(void)
{
	return flagTimer2;
}

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
        flagTimer2 = 1;
    }
}

/* Extern Interrupt 0 routine */
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0)){

		EXTI_ClearITPendingBit(EXTI_Line0);

		pushButtonPA0 = 1;

		SelectedWavesForm != SelectedWavesForm;

		GPIO_SetBits(GPIOD,GPIO_PinSource15);
		Delay(20000);
		GPIO_ResetBits(GPIOD,GPIO_PinSource15);

		Delay(10000);
		EXTI_ClearITPendingBit(EXTI_Line0);

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

/* Setup SysTick Timer for 1 ms interrupts  */
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
