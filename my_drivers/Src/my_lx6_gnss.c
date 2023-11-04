/*
 * my_lx6_gnss.c
 *
 *  Created on: Oct 27, 2023
 *      Author: mzeml
 */

#include "my_lx6_gnss.h"


bool my_lx6_get_coordinates ( uint16_t active_time_ths )
{
	uint8_t rxd_byte = 0 ;
	tim_seconds = 0 ;
	HAL_TIM_Base_Start_IT ( &htim6 ) ;
	my_lx6_on () ;
	while ( tim_seconds < active_time_ths  ) // 1200 = 10 min.
	{
		HAL_UART_Receive ( HUART_Lx6 , &rxd_byte , 1 , UART_TIMEOUT ) ;
	}
	HAL_TIM_Base_Stop_IT ( &htim6 ) ;
	my_lx6_off () ;
	return true ;
}
