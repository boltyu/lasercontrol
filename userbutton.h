#ifndef __USER_BUTTON_H
#define __USER_BUTTON_H

#include "main.h"

#define BUTTONGPIOx GPIOD
#define BUTTONPINx GPIO_PIN_6
#define BUTTONPRESSED 0
#define BUTTONNOTHING 1
#define KEYPRESSED GPIO_ReadInputPin(BUTTONGPIOx,BUTTONPINx) == BUTTONPRESSED

void ButtonOn(void);
void ButtonFunction(void);

INTERRUPT void EXTI_PORTD_IRQHandler(void); /* EXTI PORTD */

#endif