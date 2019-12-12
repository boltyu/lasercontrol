/* MAIN.C file
 * 
 * Copyright (c) 2002-2005 STMicroelectronics
 */
#include "main.h"

main()
{
	
	while(1)
	{
		switch(GetPowerMode()){
			case NORMAL_MODE:{
				LaserWork();
				ButtonFunction();
			}break;
			case LOWPOWER_MODE:{
				halt();
			}break;
			case CHARGING_MODE:{
				ChargingTips();
			}break;
			case RECOVERING_MODE:{	// default mode
				PowerOn();
				LedBlink(1,3);
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