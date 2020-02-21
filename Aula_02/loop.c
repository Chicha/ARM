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
#include "interrupt.h"

/* Variavies */
int var = 0;

/* Lista de Funcoes */
void loop(void);
void piscaLED3(void);
void Delay(uint32_t count);
void delay_ms(uint32_t miliSegundos);
void piscaLED3(void);

/* Funcoes */
void loop(void)
{
    while (1){
        piscaLED3();
    }
}

void piscaLED3(void)
{
    delay_ms(250);
    GPIO_ToggleBits(GPIOD,GPIO_Pin_14);
}

void Delay(uint32_t count)
{
    while (count != RESET)
    {
        count--;
    }
}

void delay_ms(uint32_t miliSegundos)
{

    if(get_timeBlinkLED3() == 0){
        set_timeBlinkLED3(miliSegundos);
    }

    while (get_timeBlinkLED3() != 0);

    return;

}
