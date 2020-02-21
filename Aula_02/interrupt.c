/* Programar 1 - Botão (PA0) p/ inverter o LED2 (PD13) por interrupção                      *
 * Programar 2 - Timer 2 p/ piscar (inverter) o LED1 (PD12) a cada 200ms (por interrupção)  *
 * Programar 3 - Fazer piscar o led 3 (PD14) a cada 500ms na rotina principal               */
/* Includes */
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_tim.h"
#include "setup.h"
#include "loop.h"
#include "interrupt.h"

/* Variaveis */
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef       TIM_OCInitStructure;
uint32_t TimingDelay = 0;
uint8_t  toggleLED   = 0;
uint32_t blinkLED3   = 0;

/* Lista de funcoes */
int       get_timmingDelay(void);           /* Retorna o valor do TimingDelay                           */
/* Define o valor do TimingDelay */
void      set_timmingDelay(uint32_t delay);
/* Pisca o LED1 a cada 200ms */
void      TIM2_IRQHandler(void);
/* Alterna o estado do LED2 quando o botao e pressionado */
void      EXTI0_IRQHandler(void);
/* Funcao de interrupcao do SysTick */
void      SysTick_Handler(void);
/* Retorna o valor do tempo restante para alterar o estado do LED3 */
uint32_t  get_timeBlinkLED3(void);
/* Configura o tempo para alternar estado LED3 */
void      set_timeBlinkLED3(uint32_t delay);

/* Funcoes                                      */

/* Retorna do tempo restante do LED3 */
uint32_t get_timeBlinkLED3()
{
    return blinkLED3;
}

/* Define o valor do timingDelay                */
void set_timeBlinkLED3(uint32_t delay)
{
    blinkLED3 = delay;
}

/* Retorna o valor do timingDelay               */
int get_timmingDelay()
{
    return TimingDelay;
}

/* Define o valor do timingDelay                */
void set_timmingDelay(uint32_t delay)
{
    TimingDelay = delay;
}

/* Funcao de Interrupcao do Timer 2             *
 * Timer 2 pisca o LED1 a cada 200ms            */
void TIM2_IRQHandler(void)
{
    /* if interrupt happens then do this        *
     * Verifica se a funcao realmente aconteceu */
    if (TIM_GetITStatus(TIM2, TIM_IT_Update))
    {
        /* Clear interrupt flag - Se nao apagar ela entra denovo*/
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        /* Alterna o estado do pino do LED1 */
        GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
    }
}

/* debouncing treatment - Realiza o tratamento do bounce *
 * adicionando um delay na funcao */
void debounce(){
	Delay(10000);
	u32 var;
	for (var = 0; var < 0xAFFFF; ++var)
	{
		if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0) == SET)
		{
			return;
		}
	}
}

/* Funcao de Interrupcao externa 0              */
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0))
    {
        /* Toggle LED2 */
        GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
		//debouncing treatment
		debounce();
    	/* Clear the EXTI line 0 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

/* Setup SysTick Timer for 1 ms interrupts */
void SysTick_Handler(void)
{
    if (TimingDelay){
        TimingDelay--;
    }
    if(blinkLED3 != 0){
        blinkLED3--;
    }
}
