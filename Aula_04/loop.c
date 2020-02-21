/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_usart.h"
#include "setup.h"
#include "loop.h"
#include "ISR.h"

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
u8 var = 0;

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Loop Principal
 * @param  None
 * @retval None
 */

void piscaLED1(void)
{
    delay_ms(100);
    GPIO_ToggleBits(GPIOB,GPIO_Pin_3);
}

/**
 * @brief  Delay Function.
 * @param  nCount:specifies the Delay time length.
 * @retval None
 */
void Delay(__IO uint32_t nCount)
{
    while (nCount--);
}

void USART_SendChar(USART_TypeDef* USARTx, uint8_t Char)
{
    /* Loop until Transmit data register empty flag */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    USART_SendData(USARTx, Char);
    /* Loop until the end of transmission = Transmission Complete flag */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
}

/**
 * @brief  Delay Function in ms.
 * @param  miliSegundos: specifies the Delay time length in ms.
 * @retval None
 */
void delay_ms(u32 miliSegundos)
{
    setTimingDelay(miliSegundos);
    while (getTimingDelay());
}

/* Function Name : UARTSend
 * Description   : Send a string to the UART.
 * Input         - pucBuffer: buffers to be printed.
 *               - ulCount  : buffer's length
 * Output        : None
 * Return        : None
 * */
void UARTSend(USART_TypeDef* USARTx, unsigned char *pucBuffer, unsigned long ulCount)
{
    while (--ulCount)
    {
        USART_SendChar(USARTx, *pucBuffer++);
    }
}

void USART_SendString(USART_TypeDef *USARTx,char *pStr)
{
    while(*pStr)
    {
        USART_SendChar(USARTx,*pStr);
        pStr++;
    }
}


void loop(void)
{
    USART_SendString(USART2,"Hello World\r \n");
    while (1)
    {
        piscaLED1();
    }
}
