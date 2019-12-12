#include "laserctrl.h"


uint16_t conversion_value = 0;
extern volatile uint8_t xyzdata[OUT_COUNT];
int8_t leddircache;
uint8_t xyzbuffer[10];
uint8_t xyzbufferc = 0;

void LaserOn(void){
	
	/* light sensor */
	GPIO_Init(LIGHTSENSORGPIOx,LIGHTSENSORPINx,GPIO_MODE_IN_FL_NO_IT);
	ADC1_DeInit();
	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,ADC1_CHANNEL_2,ADC1_PRESSEL_FCPU_D2,\
						ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL2,
						DISABLE);
	ADC1_Cmd(ENABLE);					
	/* power switch */					
	GPIO_Init(LASERPOWERGPIOx,LASERPOWERPINx,GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_WriteHigh(LASERPOWERGPIOx,LASERPOWERPINx);
	LASERPOWERON;
	//pwm out 
	GPIO_Init(LASERGPIOx,LASERPINx,GPIO_MODE_OUT_PP_HIGH_FAST);
	//LED out
	GPIO_Init(LEDGPIOx,LEDGPIOPINx,GPIO_MODE_OUT_PP_HIGH_FAST);
	
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1,ENABLE);
	TIM1_DeInit();
	TIM1_TimeBaseInit(0, TIM1_COUNTERMODE_UP, U8_MAX, 0);
	
	  //PC6:TIM1-CH1:LED
	TIM1_OC1Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_DISABLE, U8_MAX/2,
               TIM1_OCPOLARITY_HIGH, TIM1_OCNPOLARITY_LOW, TIM1_OCIDLESTATE_SET, 
               TIM1_OCNIDLESTATE_RESET);
	
	//PC7:TIM1-CH2:LASER
	TIM1_OC2Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_DISABLE, U8_MAX/2,
               TIM1_OCPOLARITY_HIGH, TIM1_OCNPOLARITY_LOW, TIM1_OCIDLESTATE_SET, 
               TIM1_OCNIDLESTATE_RESET);

							 
	TIM1_OC1PreloadConfig(ENABLE);
	TIM1_OC2PreloadConfig(ENABLE);
	TIM1_ARRPreloadConfig(ENABLE);
	
  TIM1_CtrlPWMOutputs(ENABLE);
	TIM1_Cmd(ENABLE);

	//enableInterrupts();
	
}

void LaserWork(void){
	uint8_t i,idata;uint16_t aa;
	uint16_t tmp = 0;
	ADC1_StartConversion();
	Mult_Read(DEVICEADDRESS,OUT_BASEREG,6,xyzdata);
	
	if(xyzdata[1] > 128)
	{
		if(xyzdata[5] < 128)
			xyzdata[5] = 255 - xyzdata[5];
		xyzbuffer[xyzbufferc] = xyzdata[5];
		xyzbufferc ++;
		if(xyzbufferc == 10)
			xyzbufferc = 0;
		for(i = 0;i< 10;i ++)
		{
			tmp += xyzbuffer[i];
		}				
		xyzdata[5] = tmp/10;
		if(xyzdata[5] < 20 || xyzdata[5] > 235)
		{
			leddircache ++;
		}else{
			leddircache --;
		}
	}
	
	if(leddircache > 20){
		LedPower(255);
		leddircache = 0;
	}else if(leddircache < -20)
	{
		LedPower(0);
		leddircache = 0;
	}
	
	/*if(ADC1_GetFlagStatus(ADC1_FLAG_EOC) != RESET){
		aa = ADC1_GetConversionValue();
		if(aa > 1000){
			TIM1_SetCompare1(aa - 900);
		}
	}*/
}

void LaserOff(void){
	LASERPOWEROFF;
	TIM1_CtrlPWMOutputs(DISABLE);	
	TIM1_Cmd(DISABLE);
	TIM2_Cmd(DISABLE);
	GPIO_Init(LASERPOWERGPIOx,LASERPOWERPINx,GPIO_MODE_IN_FL_NO_IT);
	GPIO_Init(LASERGPIOx,LASERPINx,GPIO_MODE_IN_FL_NO_IT);
	ADC1_Cmd(DISABLE);
}


void LedPower(uint8_t powerindex)
{
	TIM1_SetCompare1(powerindex);
}

void LaserPower(uint8_t powerindex)
{
	TIM2_SetCompare2(powerindex);
	if(powerindex == 0)
	{
		LASERPOWEROFF;
	}else{
		LASERPOWERON;
	}
}

void ChargingTips(void)
{
	disableInterrupts();
	LASERPOWEROFF;
	while(1)
	{
		if(GPIO_ReadInputPin(CHARGE_DETECTGPIOx,CHARGE_DETECTGPIOPinx) == NOPOWERCHARGE){
			delay(50);
			if(GPIO_ReadInputPin(CHARGE_DETECTGPIOx,CHARGE_DETECTGPIOPinx) == NOPOWERCHARGE){
				break;
			}
		}
		LedBlink(1,3);
	}
	LedPower(255);
	enableInterrupts();
	LASERPOWERON;
}

void LedBlink(uint8_t blinkspeed,uint16_t repeats)
{
	uint16_t a = 0,d = 1;
	
	if(blinkspeed > 64 || blinkspeed == 0)
		blinkspeed = 1;

	while(repeats)
	{
		if(d)
		{
			if(a < U8_MAX){
				a += blinkspeed;
			}else{
				delay(1000);
				d = 0;
				repeats --;
			}
		}else
		{
			if(a > 0){
				a -= blinkspeed;
			}else{
				d = 1;
				delay(1000);
			}
		}
		if( a < U8_MAX && a > 0)
			LedPower(a);
		delay(50);
	}
}





