/***************************************************************************//**
 * Declare function prototype
 ******************************************************************************/
void inicializacoes(void);
void GPIO_Inits(void);
void Clock_Sys_Inits(void);
void Timer2_Inits(void);
void Interruption_Inits(void);
void SysTick_Configuration(void);
void Timer4_PWM_Inits(void);
void USART_Inits(void);
void ADC1_Inits(void);
void DAC1_Inits(void);
void Timer6_Inits(void);

/* Public variables ---------------------------------------------------------*/
extern u32 dutyClycleChannel1;
extern u32 dutyClycleChannel2;
extern u32 dutyClycleChannel3;
extern u32 dutyClycleChannel4;
extern u32 varTIM4_Period;
extern float temperaturaFloat;
extern float sinalErro;

