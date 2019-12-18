#include "laserctrl.h"
#include "math.h"

uint16_t conversion_value = 0;
extern volatile uint8_t xyzdata[OUT_COUNT];
volatile int8_t xyz[OUT_COUNT];
int8_t leddircache;
//int16_t laserlumcache = 0;
uint8_t zcache[10],ycache[10];
uint8_t cachec = 0;
/*volatile double arctanalpha;
double alpha;
volatile double arctanbeta;
double beta;*/

void LaserOn(void){
	
	/* light sensor */
	GPIO_Init(GPIOC,GPIO_PIN_5,GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_WriteLow(GPIOC,GPIO_PIN_5);
	
	GPIO_Init(LIGHTSENSORGPIOx,LIGHTSENSORPINx,GPIO_MODE_IN_FL_NO_IT);
	ADC1_DeInit();
	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,ADC1_CHANNEL_2,ADC1_PRESSEL_FCPU_D2,\
						ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL2,
						DISABLE);
	ADC1_Cmd(ENABLE);					
	/* power switch */					
	GPIO_Init(LASERPOWERGPIOx,LASERPOWERPINx,GPIO_MODE_OUT_PP_LOW_SLOW);
	LASERPOWEROFF;
	//pwm out 
	GPIO_Init(LASERGPIOx,LASERPINx,GPIO_MODE_OUT_PP_HIGH_FAST);
	//LED out
	GPIO_Init(LEDGPIOx,LEDGPIOPINx,GPIO_MODE_OUT_PP_HIGH_FAST);
	
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1,ENABLE);
	TIM1_DeInit();
	TIM1_TimeBaseInit(0, TIM1_COUNTERMODE_UP, U8_MAX, 0);
	
	  //PC6:TIM1-CH1:LED
	TIM1_OC1Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_DISABLE, U8_MAX,
               TIM1_OCPOLARITY_HIGH, TIM1_OCNPOLARITY_LOW, TIM1_OCIDLESTATE_SET, 
               TIM1_OCNIDLESTATE_RESET);
	
	//PC7:TIM1-CH2:LASER
	TIM1_OC2Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_DISABLE, U8_MAX/4,
               TIM1_OCPOLARITY_HIGH, TIM1_OCNPOLARITY_LOW, TIM1_OCIDLESTATE_SET, 
               TIM1_OCNIDLESTATE_RESET);

							 
	TIM1_OC1PreloadConfig(ENABLE);
	TIM1_OC2PreloadConfig(ENABLE);
	TIM1_ARRPreloadConfig(ENABLE);
	
  TIM1_CtrlPWMOutputs(ENABLE);
	TIM1_Cmd(ENABLE);

	ADC1_StartConversion();
	//enableInterrupts();
	
}

void LaserWork(void){
	uint8_t i;
	uint16_t lval;
	uint16_t tmp = 0;

	
	Mult_Read(DEVICEADDRESS,OUT_BASEREG,6,xyzdata);
	
	if(xyzdata[OUT_Z_H] < 128)
		zcache[cachec] = 255 - xyzdata[OUT_Z_H];
	else
		zcache[cachec] = xyzdata[OUT_Z_H];
	cachec ++;
	if(cachec == 10)
		cachec = 0;
	for(i = 0;i< 10;i ++)
	{
		tmp += zcache[i];
	}				
	tmp = tmp/10;
	if((tmp < 50 || tmp > 205) && (xyzdata[OUT_Y_H] < 50 || xyzdata[OUT_Y_H] > 205))
	{
		if(xyzdata[OUT_X_H] > 128)
			leddircache ++;
	}else{
		leddircache --;
	}

	
	
	if(leddircache > 15){
		LASERPOWERON;
		leddircache = 0;
	}else if(leddircache < -15)
	{
		LASERPOWEROFF;
		leddircache = 0;
	}
	
	for(i = 0;i< OUT_COUNT;i++)
	{
		xyz[i] = (int8_t)xyzdata[i];
	}
	/*
	arctanalpha = atan(xyz[OUT_Z_H] / sqrt((double)(xyz[OUT_Y_H]*xyz[OUT_Y_H] + xyz[OUT_X_H]*xyz[OUT_X_H])));
	alpha = arctanalpha * 180/3.14159;
	arctanbeta = atan(xyz[OUT_Y_H] / sqrt((double)(xyz[OUT_X_H]*xyz[OUT_X_H] + xyz[OUT_Z_H]*xyz[OUT_Z_H])));
	beta = arctanbeta * 180 / 3.14159;
	*/
	if(ADC1_GetFlagStatus(ADC1_FLAG_EOC) != RESET){
		
		lval = ADC1_GetConversionValue();

		
		if(lval < 64){
			TIM1_SetCompare2(64);
		}else{
			if(lval < 1024)
				TIM1_SetCompare2(64+lval/5);
		}
		ADC1_StartConversion();
	}
	
	
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
	//TIM2_SetCompare2(powerindex);
	if(powerindex == 0)
	{
		LASERPOWEROFF;
	}else{
		LASERPOWERON;
	}
}

void ChargingTips(void)
{
	LaserOn();
	LASERPOWEROFF;
	GPIO_Init(CHARGE_DETECTGPIOx,CHARGE_DETECTGPIOPinx,GPIO_MODE_IN_FL_NO_IT);
	GPIO_Init(BUTTONGPIOx,BUTTONPINx,GPIO_MODE_IN_FL_NO_IT);

	while(1)
	{
		LedBlink(1,1);
		if(KEYPRESSED)
		{
			delay(5);
			if(KEYPRESSED){
				SetPowerMode(RECOVERING_MODE);
				return;
			}
		}
		if(!POWERCHARGING)
		{
			delay(30);
			if(!POWERCHARGING)
				break;
		}
	}
	
	GPIO_Init(CHARGE_DETECTGPIOx,CHARGE_DETECTGPIOPinx,GPIO_MODE_IN_FL_IT);
	enableInterrupts();
	PowerSave();
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
			}
		}else
		{
			if(a > 0){
				a -= blinkspeed;
			}else{
				d = 1;
				delay(1000);
				repeats --;
			}
		}
		if( a < U8_MAX && a > 0)
			LedPower(a);
		delay(50);
		
	}
}





