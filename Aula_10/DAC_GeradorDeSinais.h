/* Private function prototypes -----------------------------------------------*/
void TIM6_Config(void);
void STM_EVAL_KeyButton(void);

void DAC_Ch1_EscalatorConfig(void);
void DAC_Ch2_SineWaveConfig(void);

void DAC_Ch1_NoiseConfig(void);
void DAC_Ch2_TriangleConfig(void);

void DAC_Main(void);

/* Private define ------------------------------------------------------------*/
#define DAC_DHR12R2_ADDRESS    0x40007414
#define DAC_DHR8R1_ADDRESS     0x40007410

/* public variables ------------------------------------------------------------*/
extern vu8 pushButtonPA0;
extern vu8 SelectedWavesForm;
