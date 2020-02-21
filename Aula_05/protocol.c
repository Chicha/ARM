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
#include "loop.h"
#include "setup.h"
#include "protocol.h"

u8  buffer[8];
u32 posicao;
u32 pacote;

void setPosicao(u32 Posicao)
{
    posicao = Posicao;
}

u32 getPosicao(void)
{
    return posicao;
}

u32 getPacote(void)
{
    return pacote;
}

u32 setPacote(u32 valor)
{
    pacote = valor;
    return pacote;
}

void erro(void)
{
    pacote = 0;
    USART_SendChar(USART2,posicao + 48);
    USART_SendString(USART2," Formato Errado\r\n");
}

u32 porcentagem(u8 valor)
{
    // 165 = 41999/255;
    return valor*165;
}

void rxProtocol(u8 rxData)

{

    pacote = 400;

    buffer[posicao] = rxData;

    posicao++;

    if(posicao > 4)
    {

        if( buffer[0] != 0x00 )
        {
            erro();
            return;
        }

        if( buffer[4] != 0xFF )
        {
            erro();
            return;
        }

        setDutyCycle(0,porcentagem(buffer[1]));
        setDutyCycle(1,porcentagem(buffer[2]));
        setDutyCycle(2,porcentagem(buffer[3]));

        TIM_SetCompare1(TIM4,getDutyCycle(0));
        TIM_SetCompare2(TIM4,getDutyCycle(1));
        TIM_SetCompare3(TIM4,getDutyCycle(2));

        posicao = 0;

    }
}
