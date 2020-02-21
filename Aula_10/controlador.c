/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_adc.h"
#include "setup.h"
#include "loop.h"
#include "ISR.h"
#include "controlador.h"

#define ON     1
#define OFF   -1
#define F   1000

float erroAnt   = 0;
float tempAnt   = 0;
u32 hotter      = OFF;
u32 cont        = 0;
u32 temperatura = 0;
u32 referencia  = 0;
u32 controlador = 0;
s32 error = 0;

int KP = 10;
int KI = 10;
int KD = 30;

float temperaturaF = 0;
float referenciaF = 0;
float controladorAtual = 0;
float controladorAnteAnterior = 0;
float controladorAnterior = 0;
float erroPID = 0;
float erroPIDAnterior = 0;
float PI = 0;
float PD = 0;
float P = 0;
float saidaControlador = 0;

float temperaturaAtual = 0;
float temperaturaAnterior = 0;

void setKp(int value)
{
	KP = value;
}

void setKd(int value)
{
	KD = value;
}

void setKi(int value)
{
	KI = value;
}

float calcPIDtoPWM(float PID)
{
	return 204.09*PID + 6541;
}

void PWM(int pwm,float temp)
{
    temperaturaAtual = temp;
    TIM_SetCompare2(TIM4,pwm);
}

void OnOff(int ref,int temp)
{
    temperatura = temp;
    referencia = ref;
    int erro = ref - temp;
    printTemperatura(erro,"Erro : ");
    if(ref > temp)
    {
        setDutyCycle(2,41999);
    }
    else
    {
        setDutyCycle(2,0);
    }
    TIM_SetCompare2(TIM4,getDutyCycle(2));
}

void OnOffHisterese(int ref,int temp,int dt)
{
    int histerese;
    histerese = ref + hotter*dt;
    referencia = histerese;
    temperatura = temp;
    cont++;

    int erro = histerese - temp;

    if(erro > 0)
    {
        setDutyCycle(2,41999);
        hotter = ON;
    }
    else
    {
        setDutyCycle(2,0);
        hotter = OFF;
    }

    TIM_SetCompare2(TIM4,getDutyCycle(2));
}

void ControladorP(int ref,int temp)
{

	referencia  = ref;
    temperatura = temp;
    error = ref - temp;
    printTemperatura(ref,"Erro Controlador P : ");

    if(ref < temp)
    {
        controlador = 0;
    }
    else
    {
        if(P*error > getVarTIM_Period())
        {
            controlador = getVarTIM_Period();
        }
        else
        {
            controlador = error*P; // Ganho P
        }
    }

    setDutyCycle(2,controlador);
    TIM_SetCompare2(TIM4,getDutyCycle(2));

}

float calcI(void)
{
	PI = (controladorAnteAnterior + controladorAnterior)/KI;
	return PI;
}

float calcD(void)
{
	PD = (erroPID - erroPIDAnterior)*KD;
	return PD;
}

float calcP(void)
{
	P = erroPID*KP;
	return P;
}

void ControladorPID(float ref,float temp)
{
	temperaturaAtual = temp;
	referenciaF      = ref;
    erroPID          = ref - temp;
    controladorAtual   = calcP(); // Ganho Proporcional
    saidaControlador = calcPIDtoPWM(controladorAtual);
    if(saidaControlador < 0)
    {
    	saidaControlador = 0;
	}
    else
    {
		if(saidaControlador > getVarTIM_Period())
		{
			saidaControlador = getVarTIM_Period();
		}
	}
    controlador = (int) saidaControlador;
    TIM_SetCompare2(TIM4,controlador);
    erroPIDAnterior         = erroPID;
    temperaturaAnterior     = temperaturaAtual;
    controladorAnteAnterior = controladorAnterior;
    controladorAnterior     = controladorAtual;
}
