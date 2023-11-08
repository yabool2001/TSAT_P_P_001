/*
 * my_lx6_gnss.h
 *
 *  Created on: Oct 27, 2023
 *      Author: mzeml
 */

#ifndef MY_LX6_GNSS_H_
#define MY_LX6_GNSS_H_

#include <stdbool.h>
#include "main.h"
#include "my_nmea.h"

#define HUART_DBG						&huart2
#define HUART_Lx6						&huart3
#define MY_TIMER						&htim6
#define NMEA_3D_FIX						'3'
#define NMEA_MESSAGE_SIZE				250
#define MY_GNSS_NMEA_MAX_SIZE			12 // 10 + ew. znak minus + '\0'
#define MY_GNSS_MIN_TNS					3 // Minimalna ilość satelitów
#define MY_GNSS_MIN_TNS_TIME_THS		10 // Czas w jakim powinno być co najmniej MY_GNSS_NMEA_GSV_MIN_TNS satelitów
#define GNSS_MAX_ACTIVE_TIME			60 // Więcej niż 60 s nie ma sensu, bo to oznacza, że nie będzie jak wysłać do satelite Astrocast
#define NMEA_FIX_PDOP_STRING_BUFF_SIZE	5


extern RTC_HandleTypeDef hrtc;

extern TIM_HandleTypeDef htim6;

extern UART_HandleTypeDef huart2 ;
extern UART_HandleTypeDef huart3 ;

extern uint16_t tim_seconds ;

bool my_lx6_get_coordinates ( uint16_t , double , double* , int32_t* , int32_t* ) ;


#endif /* MY_LX6_GNSS_H_ */
