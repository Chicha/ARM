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

#define  nByte 8

u32 getPacote(void);
u32 setPacote(u32 valor);
void setPosicao(u32 Posicao);
u32 getPosicao(void);


/* Funções */
void rxProtocol(u8 rxData);
void erro(void);

extern u32 pacote;
extern u32 posicao;
