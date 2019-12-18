#ifndef __LASER_CONTROL_H
#define __LASER_CONTROL_H

#include "main.h"
#include "lis3mod.h"

#define LASERGPIOx	GPIOC
#define LASERPINx		GPIO_PIN_7	
#define LASERPOWERGPIOx GPIOD
#define LASERPOWERPINx GPIO_PIN_4


#define LIGHTSENSORGPIOx GPIOC
#define LIGHTSENSORPINx	GPIO_PIN_4



#define LASERPOWERON	GPIO_WriteHigh(LASERPOWERGPIOx,LASERPOWERPINx)
#define LASERPOWEROFF	GPIO_WriteLow(LASERPOWERGPIOx,LASERPOWERPINx)


void LaserOn(void);
void LaserOff(void);
void LaserWork(void);
void LedPower(uint8_t powerindex);
void LaserPower(uint8_t powerindex);
void LedBlink(uint8_t blinkspeed,uint16_t repeats);
void ChargingTips(void);

#endif