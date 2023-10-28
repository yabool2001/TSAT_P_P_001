/*
 * my_rtc.c
 *
 *  Created on: Oct 27, 2023
 *      Author: mzeml
 */
#include "my_rtc.h"

void set_my_rtc_time_from_nmeam_rmc ( const char* m )
{
	/*
	  RTC_TimeTypeDef sTime;
	  RTC_DateTypeDef sDate;

	  sTime.Hours = Hours;
	  sTime.Minutes = Minutes;
	  sTime.Seconds = Seconds;
	  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	  */
}
void get_my_rtc_time ( void )
{
	//__NOP () ;
}
void set_my_rtc_alarm ( uint8_t Hours , uint8_t Minutes , uint32_t Seconds , uint8_t Date )
{
	//__NOP () ;
}
