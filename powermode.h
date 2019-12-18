#ifndef __POWER_MODE_H
#define __POWER_MODE_H

#include "main.h"

#define LEDGPIOx GPIOC
#define LEDGPIOPINx GPIO_PIN_6
#define CHARGE_DETECTGPIOx GPIOD
#define CHARGE_DETECTGPIOPinx GPIO_PIN_5

//#define NOPOWERCHARGE (GPIO_ReadInputPin(CHARGE_DETECTGPIOx,CHARGE_DETECTGPIOPinx) == 1)
#define POWERCHARGING (GPIO_ReadInputPin(CHARGE_DETECTGPIOx,CHARGE_DETECTGPIOPinx) == 0)



enum POWERMODE{
	NORMAL_MODE = 0,
	LOWPOWER_MODE,
	RECOVERING_MODE,
	CHARGING_MODE
};

void PowerOn(void);
void PowerSave(void);
enum POWERMODE GetPowerMode(void);
void SetPowerMode(enum POWERMODE powermode);

#endif