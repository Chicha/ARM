/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);
void loopPrincipal(void);
void piscaLED1(void);
void delay_ms(u32 miliSegundos);
void UARTSend(USART_TypeDef* USARTx, unsigned char *pucBuffer, unsigned long ulCount);
void USART_SendChar(USART_TypeDef* USARTx, uint8_t Char);
void UART_SendString(USART_TypeDef *UARTx, char *pStr);
void pwm_DC(void);
void principalInits(void);
void printIntHex(int ivalue);
u16 startConversorADC1(void);
void leituraSensorTemperatura(void);
float calculoTempFloat(u32 resIntConvAD);
void mostraTemperaturaFloat(float tempFloat);
u32 mediaLeituraSensorTemperatura(u32 nrIteracoes);
char convNumEmCaractere(int Numero);
void piscaLED_D14(void);
void mostraTemperaturaFloatV2(USART_TypeDef* USARTx, float tempFloat);
void mostraSinalErroFloat(float sinalErroFloat);
void mostraNumeroViaUSART(USART_TypeDef* USARTx, s32 intNumero);
void mostrafloatViaUSART(USART_TypeDef* USARTx, s32 intNumero);
void DAC_denteDeSerra(void);
void DAC_triangular(void);

/* Public variables ---------------------------------------------------------*/
extern u8 RxVector[];

