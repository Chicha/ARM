/* Counter have 24-bit down counter */
void     TIM2_IRQHandler(void);

void     EXTI0_IRQHandler(void);

/* Setup SysTick Timer for 1 msec interrupts  */
void     SysTick_Handler(void);

int      get_timmingDelay(void);

void     set_timmingDelay(uint32_t delay);

uint32_t get_timeBlinkLED3(void);

void     set_timeBlinkLED3(uint32_t delay);
