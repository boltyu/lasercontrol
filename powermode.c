#include "powermode.h"

volatile enum POWERMODE currentmode = RECOVERING_MODE;

void PowerOn(void){
	
	//CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV8);
	enableInterrupts();
	GPIO_Init(CHARGE_DETECTGPIOx,CHARGE_DETECTGPIOPinx,GPIO_MODE_IN_FL_IT);
	EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_FALL_ONLY);
		
	Lis3On();
	ButtonOn();
	LaserOn();
	
	
	LedPower(255);
	currentmode = NORMAL_MODE;
}



void PowerSave(void)
{
	LedBlink(10,3);
	Lis3Off();
	LaserOff();	
	GPIO_Init(BUTTONGPIOx,BUTTONPINx,GPIO_MODE_IN_FL_IT);
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_FALL_ONLY);
	
	currentmode = LOWPOWER_MODE;
}

enum POWERMODE GetPowerMode(void){
	return currentmode;
}

void SetPowerMode(enum POWERMODE powermode){
	currentmode = powermode;
}




