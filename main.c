/* MAIN.C file
 * 
 * Copyright (c) 2002-2005 STMicroelectronics
 */
#include "main.h"

main()
{
	uint16_t a = 0,d = 1,blinkspeed = 1;
	while(1)
	{
		switch(GetPowerMode()){
			case NORMAL_MODE:{
				if(POWERCHARGING)
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
						}
					}
					if( a < U8_MAX && a > 0)
						LedPower(a);
				}
				else
				{
					LedPower(255);
				}
				LaserWork();
				ButtonFunction();
			}break;
			case LOWPOWER_MODE:{
				PowerSave();
			}break;
			case CHARGING_MODE:{
				ChargingTips();
			}break;
			case RECOVERING_MODE:{	// default mode
				PowerOn();
				
			}break;
		}
		delay(5);
	}


}

void delay(uint32_t y)
{
	while(y--);
}

#ifdef USE_FULL_ASSERT
void assert_failed(u8* file, u32 line)
{ /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }
}
#endif