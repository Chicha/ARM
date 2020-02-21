/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_adc.h"
#include "setup.h"
#include "loop.h"
#include "ISR.h"

void leituraSensorTemperatura(void);
void printIntHex(int value);

/* Private define */

/* Private macro */

/* Private variables */
u8 var = 0;

/* Private functions */

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

u16 startConversorADC1(u8 channel)
{
    /* ADC1 regular channel 6 configuration
     * PA1 = [ADC1_IN1] --> ADC1 Channel 1
     * PA6 = [ADC1_IN6] --> ADC1 Channel 6 */
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_112Cycles);

    u16 resultadoConvAD = 0;

    /* Start ADC1 Software Conversion
     * Enables the selected ADC1 software start conversion of the regular channels */
    ADC_SoftwareStartConv(ADC1);

    // aguardo o final da conversï¿½o
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

    // guardo o resultado da conversï¿½o na variï¿½vel inteira de 16 bits
    if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != RESET)
    {
        resultadoConvAD = ADC_GetConversionValue(ADC1);
        ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
    }

    // retorno o resultado da conversï¿½o
    return resultadoConvAD;
}

float leituraADC1Channel6(void)
{
    u32 resConv = 0;
    u32 var;
    // mï¿½dia das leituras do ADC --> estabilizaï¿½ï¿½o do sinal
    for (var = 0; var < 100; ++var)
    {
        resConv = resConv + startConversorADC1(ADC_Channel_6);

    }
    resConv = resConv / 100;
    UART_SendString(USART2, "Valor Inteiro ADC1 Ch6: ");
    printIntHex(resConv);

    // Vdd =  2,9 V e resoluï¿½ï¿½o do ADC = 12 bits = 2^12 = 4096 - 1 = 4095 steps
    float ch6Float = (2.9 / 4095.0) * (float) resConv;
    return ch6Float;
}

u32 mediaLeituraSensorTemperatura(u32 nrIteracoes)
{
    u32 mediaResConv = 0;
    u32 var;
    // media das leituras do ADC --> estabilizacao do sinal
    for (var = 0; var < nrIteracoes; ++var)
    {
        mediaResConv = mediaResConv + startConversorADC1(ADC_Channel_1);

    }

    mediaResConv = mediaResConv / nrIteracoes;

    return mediaResConv;
}

u32 mediaLeituraSetPoint(u32 nrIteracoes)
{
    u32 mediaResConv = 0;
    u32 var;
    // media das leituras do ADC --> estabilizacao do sinal
    for (var = 0; var < nrIteracoes; ++var)
    {
        mediaResConv = mediaResConv + startConversorADC1(ADC_Channel_6);

    }

    mediaResConv = mediaResConv / nrIteracoes;

    return mediaResConv;
}

char convNumEmCaractere(int Numero)
{
    unsigned char Caractere;

    if (Numero > 0 && Numero < 10)
    {
        Caractere = Numero + 0x30;
    }
    else
    {
        Caractere = '0';
    }

    return (Caractere);
}

void printIntHex(int value)
{
    int i, j;
    unsigned char c[10];
    for (i = 0; i < 8; i++)
    {
        j = value >> (i * 4) & 0x0F;
        if (j < 10)
        {
            c[7 - i] = j + '0';
        }
        else
        {
            c[7 - i] = j + 55;
        }
    }

    c[8] = '\r';
    c[9] = '\n';

    UARTSend(USART2, c, sizeof(c));

}

void printTensao(int valor)
{
    int i,j;
    int valorSeparado,resto = valor;
    int exp10[6] = {0,10,100,1000,10000,100000};
    unsigned char c[9];
    j = 0;
    USART_SendString(USART2," Tensao: ");
    for(i = 0;i < 5;i++)
    {
        valorSeparado = resto / exp10[4-i];
        resto         = resto % exp10[4-i];
        c[j]          = valorSeparado + '0';
        if(i == 0)
        {
            j++;
        }
        j++;
    }
    c[1] = ',';
    c[6] = ' ';
    c[7] = 'V';
    c[8] = '\n';
    UARTSend(USART2, c, sizeof(c));
}

void printTemperatura(int valor)
{
    int i,j;
    int valorSeparado,resto = valor;
    int exp10[6] = {0,10,100,1000,10000,100000};
    unsigned char c[10];
    j = 0;
    USART_SendString(USART2,"Temperatura: ");
    for(i = 0;i < 5;i++)
    {
        valorSeparado = resto / exp10[4-i];
        resto         = resto % exp10[4-i];
        c[j] = valorSeparado + '0';
        if(i == 1)
        {
            j++;
        }
        j++;
    }
    c[3] = ',';
    c[6] = ' ';
    c[7] = '°';
    c[8] = 'C';
    c[9] = '\n';
    UARTSend(USART2, c, sizeof(c));
}

void loop(void)
{
    USART_SendString(USART2,"Hello World\r\n");
    int resConvAD = 0,resConvAD6 = 0;
    // loop infinito
    while (1)
    {
        resConvAD = mediaLeituraSensorTemperatura(1000);
        resConvAD6 = mediaLeituraSetPoint(1000);

        if(resConvAD > 4095)
        {
            resConvAD = 4095;
        }
        if(resConvAD6 > 4095)
        {
            resConvAD6 = 4095;
        }
        printTemperatura(resConvAD*22000/273);
        printTensao(resConvAD6*22000/273);
        USART_SendChar(USART2,'\n');
        setDutyCycle(2,10*resConvAD6);
        TIM_SetCompare4(TIM4,getDutyCycle(2));
    }
}
