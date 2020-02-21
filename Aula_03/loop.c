/* Includes */
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "setup.h"
#include "loop.h"
#include "ISR.h"


/* Private variables */
u8 var = 0;


void loop(void)
{
    while (1)
    {
        piscaLED4();
    }
}

void piscaLED4(void)
{
    delay_ms(500);
    /* Toggle LED1 PD12 */
    GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
}

void Delay(__IO uint32_t nCount)
{
    while (nCount--);
}


void delay_ms(u32 miliSegundos)
{
    TimingDelay = miliSegundos;
    while (TimingDelay);
}
