// public variables
extern u8 flagIntTimer2;
extern float setPoint;
extern float K_p;
extern float K_i;
extern float K_d;
extern float temperatura_t0;
extern float sinalErro_t0;
extern float integralParcial;
extern float proporcional_PID;
extern float derivativo_PID;
extern float saidaControladorPID;
extern float sinalPWM;
extern float sinalPWMsaidaPID;
extern float sinalPWMsetPointTemperatura;

/* Private function prototypes -----------------------------------------------*/
void mainControladorPID(void);
u16 conversorAD_ch1(void);
void medicaoTempEnvUSART(void);
void ledPilotoErro(void);
float convTempFloat2DC_PWM(float tempFloatMedida);
float PID_controller(float erroPID, float Kp, float Ki, float Kd);
void dutyCyclePWM(float sinal1, float sinal2);

