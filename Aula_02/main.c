#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "loop.h"
#include "setup.h"
#include "interrupt.h"

int main(void)
{
	setup();
	loop();
	while(1);
}
