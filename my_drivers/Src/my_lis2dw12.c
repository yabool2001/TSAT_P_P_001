/*
 * my_lis2dw12.c
 *
 *  Created on: Nov 9, 2023
 *      Author: mzeml
 */

#include "my_lis2dw12.h"

bool my_lis2dw12_init ( void* h )
{
	stmdev_ctx_t my_lis2dw12_ctx ;
	uint8_t rst = 1 ;
	lis2dw12_reg_t int_route ;

	my_lis2dw12_ctx.write_reg = my_lis2dw12_platform_write ;
	my_lis2dw12_ctx.read_reg = my_lis2dw12_platform_read ;
	my_lis2dw12_ctx.handle = h ;

	/*Restore default configuration */
	lis2dw12_reset_set ( &my_lis2dw12_ctx , PROPERTY_ENABLE ) ;
	do {
		lis2dw12_reset_get ( &my_lis2dw12_ctx, &rst ) ;
	} while ( rst ) ;

	if ( my_lis2dw12_get_id ( &my_lis2dw12_ctx ) == LIS2DW12_ID )
	{
		lis2dw12_full_scale_set ( &my_lis2dw12_ctx , LIS2DW12_2g ) ;
		lis2dw12_power_mode_set ( &my_lis2dw12_ctx , LIS2DW12_HIGH_PERFORMANCE ) ;
		/*Enable interrupt generation on Wake-Up INT1 pin */
		lis2dw12_pin_int1_route_get ( &my_lis2dw12_ctx , &int_route.ctrl4_int1_pad_ctrl ) ;
		int_route.ctrl4_int1_pad_ctrl.int1_wu = PROPERTY_ENABLE ;
		lis2dw12_pin_int1_route_set ( &my_lis2dw12_ctx , &int_route.ctrl4_int1_pad_ctrl ) ;

		return true ;
	}

	return false ;

}

uint8_t my_lis2dw12_get_id ( stmdev_ctx_t* ctx )
{
	uint8_t id = 0 ;
	lis2dw12_device_id_get ( ctx , &id ) ;
	return id ;
}

int32_t my_lis2dw12_platform_write ( void *handle , uint8_t reg , const uint8_t *bufp , uint16_t len )
{
	HAL_GPIO_WritePin	( LIS_SPI1_CS_GPIO_Port , LIS_SPI1_CS_Pin , GPIO_PIN_RESET ) ;
	HAL_Delay ( 20 ) ;
	HAL_SPI_Transmit	( handle , &reg , 1 , 1000 ) ;
	HAL_SPI_Transmit	( handle , (uint8_t*) bufp , len , 1000 ) ;
	HAL_GPIO_WritePin	( LIS_SPI1_CS_GPIO_Port , LIS_SPI1_CS_Pin , GPIO_PIN_SET) ;

	return 0;
}
int32_t my_lis2dw12_platform_read ( void *handle , uint8_t reg , uint8_t *bufp , uint16_t len )
{
	reg |= 0x80;
	HAL_GPIO_WritePin ( LIS_SPI1_CS_GPIO_Port , LIS_SPI1_CS_Pin , GPIO_PIN_RESET) ;
	HAL_Delay ( 20 ) ;
	HAL_SPI_Transmit ( handle , &reg , 1 , 1000 ) ;
	HAL_SPI_Receive ( handle , bufp , len , 1000 ) ;
	HAL_GPIO_WritePin ( LIS_SPI1_CS_GPIO_Port , LIS_SPI1_CS_Pin , GPIO_PIN_SET) ;

	return 0;
}

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
