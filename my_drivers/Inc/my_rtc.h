/*
 * my_rtc.h
 *
 *  Created on: Oct 27, 2023
 *      Author: mzeml
 */

#ifndef MY_RTC_H_
#define MY_RTC_H_

void set_my_rtc_time_from_nmea_rmc ( const char* ) ;
void get_my_rtc_time	( void ) ;
void set_my_rtc_alarm	( uint8_t , uint8_t , uint32_t , uint8_t ) ;

#endif /* MY_RTC_H_ */
