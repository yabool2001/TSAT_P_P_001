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

void my_rtc_set_dt_from_nmea_rmc ( const char* m )
{
	  RTC_TimeTypeDef sTime;
	  RTC_DateTypeDef sDate;

	  my_nmea_get_rmc_date_yy ( m , &sDate.Year ) ;
	  my_nmea_get_rmc_date_mm ( m , &sDate.Month ) ;
	  my_nmea_get_rmc_date_dd ( m , &sDate.Date ) ;
	  my_nmea_get_rmc_utc_hh ( m , &sTime.Hours ) ;
	  my_nmea_get_rmc_utc_mm ( m , &sTime.Minutes ) ;
	  my_nmea_get_rmc_utc_ss ( m , &sTime.Seconds ) ;
	  my_nmea_get_rmc_utc_sss ( m , &sTime.SubSeconds ) ;
	  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	  sTime.StoreOperation = RTC_STOREOPERATION_RESET;

	  HAL_RTCEx_EnableBypassShadow ( &hrtc ) ;
	  HAL_RTC_SetTime ( &hrtc , &sTime , RTC_FORMAT_BIN ) ;
	  HAL_RTC_SetDate ( &hrtc , &sDate , RTC_FORMAT_BIN ) ;
	  HAL_RTCEx_DisableBypassShadow ( &hrtc ) ;
}
void my_rtc_get_dt ( RTC_DateTypeDef* d , RTC_TimeTypeDef* t )
{
	HAL_RTC_GetDate ( &hrtc , d , RTC_FORMAT_BIN ) ;
	HAL_RTC_GetTime ( &hrtc , t , RTC_FORMAT_BIN ) ;
}
uint16_t my_rtc_get_time_s ( char* dt_s )
{
	RTC_DateTypeDef gDate;
	RTC_TimeTypeDef gTime;

	//char cdt[20];

	HAL_RTC_GetTime ( &hrtc , &gTime , RTC_FORMAT_BIN ) ;
	HAL_RTC_GetDate ( &hrtc , &gDate , RTC_FORMAT_BIN ) ;

	sprintf ( dt_s , "%4d.%02d.%02d %02d:%02d:%02d" , 2000 + gDate.Year , gDate.Month , gDate.Date , gTime.Hours , gTime.Minutes , gTime.Seconds) ;

	return (uint16_t) ( 2000 + gDate.Year ) ;
}
bool my_rtc_set_alarm ( uint32_t s )
{
	tim_seconds = 0 ;

	RTC_DateTypeDef 	d ;
	RTC_TimeTypeDef 	t ;
	RTC_AlarmTypeDef	a ;

	bool r = false ;

	my_rtc_get_dt ( &d , &t ) ;
	uint32_t alarm_ts = my_conv_rtc2timestamp ( &d , &t ) + s ;
	my_conv_timestamp2rtc ( alarm_ts , &d , &t ) ;
	a.AlarmTime.Hours = t.Hours ;
	a.AlarmTime.Minutes = t.Minutes ;
	a.AlarmTime.Seconds = t.Seconds ;
	a.AlarmTime.SubSeconds = 0 ;
	a.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
	a.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET ;
	a.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY ;
	a.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL ;
	a.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE ;
	a.Alarm = RTC_ALARM_A ;

	if ( HAL_RTC_SetAlarm ( &hrtc , &a , RTC_FORMAT_BIN ) == HAL_OK )
	{
		char s[60] = {0} ;
		sprintf ( s , "Alarm set to %lu" , alarm_ts ) ;
		send_debug_logs ( s ) ;
		r = true ;
	}

	/* Docelowo przetestować poniższą procedurę i wdrażać powszechnie
	HAL_TIM_Base_Start_IT ( MY_TIMER ) ;
	while ( tim_seconds < HAL_STD_TIME_OPS_THS )
	{
		if ( HAL_RTC_SetAlarm ( &hrtc , &a , RTC_FORMAT_BIN ) == HAL_OK )
		{
			r = true ;
			break ;
		}
	}
	HAL_TIM_Base_Stop_IT ( MY_TIMER ) ;
	*/
	return r ;
}
