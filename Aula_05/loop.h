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
#include "setup.h"

/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);
void loop(void);
void piscaLED1(void);
void delay_ms(u32 miliSegundos);
void UARTSend(USART_TypeDef* USARTx, unsigned char *pucBuffer, unsigned long ulCount);
void USART_SendChar(USART_TypeDef* USARTx, uint8_t Char);
void UART_SendString(USART_TypeDef *UARTx, char *pStr);
