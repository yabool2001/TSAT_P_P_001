/*
 * my_lis2dw12.h
 *
 *  Created on: Nov 9, 2023
 *      Author: mzeml
 */

#ifndef MY_LIS2DW12_H_
#define MY_LIS2DW12_H_

#include "stm32g0xx_hal.h"

#include "main.h"
#include "lis2dw12_reg.h"

#define LIS2DW12_ID						0x44U// LIS2DW12 Device Identification (Who am I)


//ACC


int32_t	my_lis2dw12_platform_write ( void* , uint8_t , const uint8_t* , uint16_t ) ;
int32_t	my_lis2dw12_platform_read ( void* , uint8_t , uint8_t* , uint16_t ) ;
bool my_lis2dw12_init ( void* ) ;
uint8_t my_lis2dw12_get_id ( stmdev_ctx_t* ) ;


#endif /* MY_LIS2DW12_H_ */
