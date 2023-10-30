/*
 * my_rtc.c
 *
 *  Created on: Oct 27, 2023
 *      Author: mzeml
 */
#include "my_rtc.h"

/*
** RMC: Recommended Minimum Specific GNSS Data. Time, date, position, course, and speed data provided by a GNSS receiver.
** Synopsis: $<TalkerID>RMC,<UTC>,<Status>,<Lat>,<N/S>,<Lon>,<E/W>,<SOG>,<COG>,<Date>,<MagVar>,<MagVarDir>,<ModeInd>,<NavStatus>*<Checksum><CR><LF>
** m message example from Quectel L86: "$GPRMC,085151.187,V,,,,,0.00,0.00,291023,,,N,V*3A". \r\n have been wiped out.
*/

extern RTC_HandleTypeDef hrtc ;

void set_my_rtc_from_nmea_rmc ( const char* m )
{
	  RTC_TimeTypeDef sTime;
	  RTC_DateTypeDef sDate;

	  get_my_nmea_rmc_date_yy ( m , &sDate.Year ) ;
	  get_my_nmea_rmc_date_mm ( m , &sDate.Month ) ;
	  get_my_nmea_rmc_date_dd ( m , &sDate.Date ) ;
	  get_my_nmea_rmc_utc_hh ( m , &sTime.Hours ) ;
	  get_my_nmea_rmc_utc_mm ( m , &sTime.Minutes ) ;
	  get_my_nmea_rmc_utc_ss ( m , &sTime.Seconds ) ;
	  get_my_nmea_rmc_utc_sss ( m , &sTime.SubSeconds ) ;
	  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	  sTime.StoreOperation = RTC_STOREOPERATION_RESET;

	  HAL_RTCEx_EnableBypassShadow ( &hrtc ) ;
	  HAL_RTC_SetTime ( &hrtc , &sTime , RTC_FORMAT_BIN ) ;
	  HAL_RTC_SetDate ( &hrtc , &sDate , RTC_FORMAT_BIN ) ;
	  HAL_RTCEx_DisableBypassShadow ( &hrtc ) ;
}
void get_my_rtc_time ( char* dt )
{
	RTC_DateTypeDef gDate;
	RTC_TimeTypeDef gTime;

	//char cdt[20];

	HAL_RTC_GetTime ( &hrtc , &gTime , RTC_FORMAT_BIN ) ;
	HAL_RTC_GetDate ( &hrtc , &gDate , RTC_FORMAT_BIN ) ;

	sprintf ( dt , "%4d.%02d.%02d %02d:%02d:%02d" , 2000 + gDate.Year , gDate.Month , gDate.Date , gTime.Hours , gTime.Minutes , gTime.Seconds) ;
}
void set_my_rtc_alarm ( uint8_t Hours , uint8_t Minutes , uint32_t Seconds , uint8_t Date )
{
	//__NOP () ;
}
