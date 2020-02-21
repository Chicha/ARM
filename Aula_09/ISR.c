/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_usart.h"
#include "Inicializacoes.h"
#include "LoopPrincipal.h"
#include "Controlador_PID.h"
#include "DAC_GeradorDeSinais.h"

/***************************************************************************//**
 * typedef unions and structures
 ******************************************************************************/
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

/* Private variables ---------------------------------------------------------*/
vs32 TimingDelay;
s32 toggleLED = 0;
u8 contRx = 0;
u32 clearBuffRX = 0;

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

void USART2_IRQHandler(void)
{
	uint8_t USARTdata;

	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);

		USARTdata = USART_ReceiveData(USART2);
		/* eco char received */
		USART_SendChar(USART2, USARTdata);
	}
}

/**
 * @brief  This function handles USART3 global interrupt request.
 * @param  None
 * @retval None
 */
void USART3_IRQHandler(void)
{
	uint8_t USARTdata;

	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);

		USARTdata = USART_ReceiveData(USART3);
		clearBuffRX = 0;

		// guardo o byte recebido no vetor RxVector
		RxVector[contRx] = USARTdata;

		if (++contRx > 6)
		{
			contRx = 0;
			// recebi o set point
			if (RxVector[0] == 's' && RxVector[1] == 'e' && RxVector[2] == 't' && RxVector[6] == 'x')
			{
				// set point recebe o valor do byte (0 a 255)
				setPoint = (float) RxVector[3];

				// envio o set point recebido via USART
				UART_SendString(USART3, "Set Point = ");
				mostraNumeroViaUSART(USART3, setPoint);

				// inicializo cálculo da integral
				integralParcial = 0.0;
			}
			// recebi constantes do PID
			if (RxVector[0] == 'p' && RxVector[1] == 'i' && RxVector[2] == 'd' && RxVector[6] == 'x')
			{
				// Kp do controlador PID
				K_p = (float) RxVector[3] / 10.0;
				// Ki do controlador PID
				K_i = (float) RxVector[4] / 100.0;
				// Kd do controlador PID
				K_d = (float) RxVector[5] / 10.0;

				// envio Kp, Ki e Kd recebidos via USART
				UART_SendString(USART3, "Kp = ");
				mostrafloatViaUSART(USART3, RxVector[3]);
				UART_SendString(USART3, "Ki = ");
				mostrafloatViaUSART(USART3, RxVector[4]/10);
				UART_SendString(USART3, "Kd = ");
				mostrafloatViaUSART(USART3, RxVector[5]);

				// inicializo cálculo da integral
				integralParcial = 0.0;
			}

			// pisca LED14
			GPIO_SetBits(GPIOD, GPIO_Pin_14);
			Delay(2000000);
			GPIO_ResetBits(GPIOD, GPIO_Pin_14);
		}
	}
}

/* Timer 2 interrupt routine */
void TIM2_IRQHandler(void)
{
	//if interrupt happens then do this
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //clear interrupt flag

		/* Toggle LED1 */
//		GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
		// delta t de amostragem do sinal de temperatura ==> 100 ms
		flagIntTimer2 = 1;

		//contRx = 0;

	}
}

/**
 * @brief  This function handles External line 0 interrupt request ==> PA0 push button
 * @param  None
 * @retval None
 */
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		/* Clear the EXTI line 0 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line0);

		pushButtonPA0 = 1;

	    /* Change the selected waves forms */
	    SelectedWavesForm = !SelectedWavesForm;


		// pisca LED
		GPIO_SetBits(GPIOD, GPIO_Pin_15);
		Delay(2000000);
		GPIO_ResetBits(GPIOD, GPIO_Pin_15);

		// deboucing
	    Delay(10000000);
	    EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

/**
 * @brief  This function handles External line 2 interrupt request.
 * @param  None
 * @retval None
 */
void EXTI2_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line2) != RESET)
	{
		/* Clear the EXTI line 2 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line2);

	}
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 * Setup SysTick Timer for 1 msec interrupts  */
void SysTick_Handler(void)
{
	if (TimingDelay != 0)
		TimingDelay--;

	// reset contRX depois de 4 seg de inatividade na USART
	if (clearBuffRX++ > 4000)
	{
		clearBuffRX = 0;
		contRx = 0;
	}

}

