#include "powermode.h"


volatile enum POWERMODE currentmode = RECOVERING_MODE;

void PowerOn(void){
	
	//CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV8);
	LaserOn();
	LASERPOWEROFF;
	
	GPIO_Init(CHARGE_DETECTGPIOx,CHARGE_DETECTGPIOPinx,GPIO_MODE_IN_FL_NO_IT);
	EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_FALL_LOW);
	
	Lis3On();
	ButtonOn();
	
	LedBlink(1,3);
	LedPower(255);
	
	currentmode = NORMAL_MODE;
}



void PowerSave(void)
{
	
	Lis3Off();
	LaserOff();	
	enableInterrupts();
	GPIO_WriteHigh(GPIOC,GPIO_PIN_5);
	GPIO_Init(BUTTONGPIOx,BUTTONPINx,GPIO_MODE_IN_FL_IT);
	GPIO_Init(CHARGE_DETECTGPIOx,CHARGE_DETECTGPIOPinx,GPIO_MODE_IN_FL_IT);
	halt();
	
	disableInterrupts();
	GPIO_WriteLow(GPIOC,GPIO_PIN_5);
	GPIO_Init(BUTTONGPIOx,BUTTONPINx,GPIO_MODE_IN_FL_NO_IT);
	GPIO_Init(CHARGE_DETECTGPIOx,CHARGE_DETECTGPIOPinx,GPIO_MODE_IN_FL_NO_IT);
}

enum POWERMODE GetPowerMode(void){
	return currentmode;
}

void SetPowerMode(enum POWERMODE powermode){
	currentmode = powermode;
}




