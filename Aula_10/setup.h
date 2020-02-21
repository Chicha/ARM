/* Includes */
#include "misc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_syscfg.h"

/* Functions to set and return variable */
u32  getDutyCycle(u32 channel);
void setDutyCycle(u32 channel,u32 valor);
void setVarTIM_Period(u32 valor);
u32  getVarTIM_Period();

/* Funtions */
void setup();
