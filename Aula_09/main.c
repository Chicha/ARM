//void IOToggle(void);
//void GPIO_IOToggle_example(void);

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "Inicializacoes.h"
#include "LoopPrincipal.h"

int main(void)
{

	inicializacoes();

	loopPrincipal();

	while(1)
    {
    }
}

