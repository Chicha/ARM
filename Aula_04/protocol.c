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
    USART_SendChar(USART2,posicao + 48);
    USART_SendString(USART2,"Formato Errado\r\n");
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

        USART_SendChar(USART2, posicao + 30);

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

        TIM_SetCompare1(TIM4,porcentagem(buffer[1]));
        TIM_SetCompare2(TIM4,porcentagem(buffer[2]));
        TIM_SetCompare3(TIM4,porcentagem(buffer[3]));

        posicao = 0;

    }
}
