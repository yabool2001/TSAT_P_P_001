/*
 * my_lis2dw12.c
 *
 *  Created on: Nov 9, 2023
 *      Author: mzeml
 */

#include "my_lis2dw12.h"


/*
void lis2dw12_conf_print ( void )
{
	lis2dw12_wkup_threshold_get ( &lis2dw12_ctx , &reg8bit ) ;
	sprintf ( (char *)uart_tx_buff , "WAKE_UP_THS: %d\r\n" , reg8bit ) ;
	dbg_tx ( uart_tx_buff , strlen ( (char const*)uart_tx_buff ) ) ;

	lis2dw12_read_reg ( &lis2dw12_ctx , lis2dw12_CTRL1 , &reg8bit , 1 ) ;
	sprintf ( (char *)uart_tx_buff , "CTRL1: %d\r\n" , reg8bit ) ;
	dbg_tx ( uart_tx_buff , strlen ( (char const*)uart_tx_buff ) ) ;

	lis2dw12_read_reg ( &lis2dw12_ctx , lis2dw12_CTRL3 , &reg8bit , 1 ) ;
	sprintf ( (char *)uart_tx_buff , "CTRL3: %d\r\n" , reg8bit ) ;
	dbg_tx ( uart_tx_buff , strlen ( (char const*)uart_tx_buff ) ) ;

	lis2dw12_read_reg ( &lis2dw12_ctx , lis2dw12_CTRL4_INT1_PAD_CTRL , &reg8bit , 1 ) ;
	sprintf ( (char *)uart_tx_buff , "CTRL4: %d\r\n" , reg8bit ) ;
	dbg_tx ( uart_tx_buff , strlen ( (char const*)uart_tx_buff ) ) ;

	lis2dw12_read_reg ( &lis2dw12_ctx , lis2dw12_CTRL5_INT2_PAD_CTRL , &reg8bit , 1 ) ;
	sprintf ( (char *)uart_tx_buff , "CTRL5: %d\r\n" , reg8bit ) ;
	dbg_tx ( uart_tx_buff , strlen ( (char const*)uart_tx_buff ) ) ;

	lis2dw12_read_reg ( &lis2dw12_ctx , lis2dw12_CTRL6 , &reg8bit , 1 ) ;
	sprintf ( (char *)uart_tx_buff , "CTRL6: %d\r\n" , reg8bit ) ;
	dbg_tx ( uart_tx_buff , strlen ( (char const*)uart_tx_buff ) ) ;

	lis2dw12_read_reg ( &lis2dw12_ctx , lis2dw12_STATUS , &reg8bit , 1 ) ;
	sprintf ( (char *)uart_tx_buff , "STATUS: %d\r\n" , reg8bit ) ;
	dbg_tx ( uart_tx_buff , strlen ( (char const*)uart_tx_buff ) ) ;

	lis2dw12_read_reg ( &lis2dw12_ctx , lis2dw12_WAKE_UP_SRC , &reg8bit , 1 ) ;
	sprintf ( (char *)uart_tx_buff , "WAKE_UP_SRC: %d\r\n" , reg8bit ) ;
	dbg_tx ( uart_tx_buff , strlen ( (char const*)uart_tx_buff ) ) ;
}
*/
