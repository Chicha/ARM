/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);
void loop(void);
void piscaLED1(void);
void delay_ms(u32 miliSegundos);
void UARTSend(USART_TypeDef* USARTx, unsigned char *pucBuffer, unsigned long ulCount);
void USART_SendChar(USART_TypeDef* USARTx, uint8_t Char);
void UART_SendString(USART_TypeDef *UARTx, char *pStr);
