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
		if(presscount > 100){
			presscount = 0;
			while(1){
				LedBlink(10,3);
				LedPower(0);
				currentmode = LOWPOWER_MODE;
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
	if(KEYPRESSED)
	{
		SetPowerMode(RECOVERING_MODE);
	}
	else if(POWERCHARGING)
	{
		GPIO_Init(CHARGE_DETECTGPIOx,CHARGE_DETECTGPIOPinx,GPIO_MODE_IN_FL_NO_IT);
		SetPowerMode(CHARGING_MODE);
	}
	
}

