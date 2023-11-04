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

#define HUART_Lx6	&huart3
#define MY_TIMER	&htim6

extern RTC_HandleTypeDef hrtc;

extern TIM_HandleTypeDef htim6;

extern UART_HandleTypeDef huart3;

extern uint16_t tim_seconds ;

bool my_lx6_get_coordinates ( uint16_t ) ;


#endif /* MY_LX6_GNSS_H_ */
