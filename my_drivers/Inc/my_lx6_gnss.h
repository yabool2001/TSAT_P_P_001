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

#define HUART_DBG				&huart2
#define HUART_Lx6				&huart3
#define MY_TIMER				&htim6
#define NMEA_3D_FIX				'3'
#define NMEA_MESSAGE_SIZE		250
#define MY_GNSS_RMC_TIME_THS	10
#define MY_GNSS_NMEA_MAX_SIZE	12 // 10 + ew. znak minus + '\0'

extern RTC_HandleTypeDef hrtc;

extern TIM_HandleTypeDef htim6;

extern UART_HandleTypeDef huart2 ;
extern UART_HandleTypeDef huart3 ;

extern uint16_t tim_seconds ;

bool my_lx6_get_coordinates ( uint16_t , double , double* , int32_t* , int32_t* ) ;


#endif /* MY_LX6_GNSS_H_ */
