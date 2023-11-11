/*
 * my_lis2dw12.h
 *
 *  Created on: Nov 9, 2023
 *      Author: mzeml
 */

#ifndef MY_LIS2DW12_H_
#define MY_LIS2DW12_H_

#include <stdbool.h>
#include "lis2dw12_reg.h"

#define LIS2DW12_ID				0x44U // LIS2DW12 Device Identification (Who am I)
#define LIS2DW12_WAKEUP_THS		4
#define LIS2DW12_WAKEUP_DUR		2
#define LIS2DW12_LIR			1

//ACC



bool my_lis2dw12_init ( stmdev_ctx_t* ) ;
uint8_t my_lis2dw12_get_id ( stmdev_ctx_t* ) ;
void my_lis2dw12_int1_wu_enable ( stmdev_ctx_t* ctx ) ;
void my_lis2dw12_int1_wu_disable ( stmdev_ctx_t* ctx ) ;
//void my_is2dw12_print_conf ( void* ) ;


#endif /* MY_LIS2DW12_H_ */
