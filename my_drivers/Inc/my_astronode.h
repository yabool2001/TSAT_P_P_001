/*
 * my_astronode.h
 *
 *  Created on: Oct 23, 2023
 *      Author: mzeml
 */

#ifndef ASTROCAST_INC_MY_ASTRONODE_H_
#define ASTROCAST_INC_MY_ASTRONODE_H_

#define MY_ASTRO_INIT_TIME	60

#include "main.h"

extern RTC_HandleTypeDef hrtc;

extern TIM_HandleTypeDef htim6;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern uint16_t tim_seconds ;

bool my_astro_init ( void ) ;
void my_astro_reset_astronode ( void ) ;
void my_astro_write_coordinates ( int32_t , int32_t ) ;

#endif /* ASTROCAST_INC_MY_ASTRONODE_H_ */
