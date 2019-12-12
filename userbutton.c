#include "userbutton.h"
//#include "stm8s_tim2.h"
static volatile	uint16_t presscount = 0;
extern volatile enum POWERMODE currentmode;

void ButtonOn(void){
	GPIO_Init(BUTTONGPIOx,BUTTONPINx,GPIO_MODE_IN_FL_NO_IT);
}

void ButtonFunction(void)
{
	if(KEYPRESSED){
		presscount ++;
		if(presscount > 500){
			presscount = 0;
			while(1){
				PowerSave();
				if(!KEYPRESSED)
				{
					delay(5);
					if(!KEYPRESSED)
						break;
				}
			}
		}
		}else{
		if(presscount > 5 &&  presscount < 50)
		{
			LedPower(255);
		}
		presscount = 0;
	}
}

@svlreg INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
	delay(5);
	
	if(GPIO_ReadInputPin(CHARGE_DETECTGPIOx,CHARGE_DETECTGPIOPinx) == POWERCHARGING)
	{
		if(GetPowerMode() == NORMAL_MODE)
			SetPowerMode(CHARGING_MODE);
	}
	if(GPIO_ReadInputPin(BUTTONGPIOx,BUTTONPINx) == BUTTONPRESSED)
	{
		SetPowerMode(RECOVERING_MODE);
	}

}

