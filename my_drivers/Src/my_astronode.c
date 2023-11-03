/*
 * my_astronode.c
 *
 *  Created on: Oct 23, 2023
 *      Author: mzeml
 */
#include "my_astronode.h"

bool my_astro_init ( void )
{
	bool cfg_wr = false ;
	tim_seconds = 0 ;
	HAL_TIM_Base_Start_IT ( &htim6 ) ;
	while ( tim_seconds < MY_ASTRO_INIT_TIME && !cfg_wr )
	{
		cfg_wr = astronode_send_cfg_wr ( true , true , true , false , true , true , true , false ) ;
		my_astro_off () ;
		HAL_Delay ( 3000 ) ;
		my_astro_on () ;
		reset_astronode () ;
	}
	HAL_TIM_Base_Stop_IT ( &htim6 ) ;
	if ( cfg_wr )
	{
		astronode_send_cfg_sr () ;
		astronode_send_mpn_rr () ;
		astronode_send_msn_rr () ;
		astronode_send_mgi_rr () ;
		astronode_send_pld_fr () ;
		return true ;
	}
	else
	{
		return false ;
	}
}

void my_astro_write_coordinates ( int32_t astro_geo_wr_latitude , int32_t astro_geo_wr_longitude )
{
	astronode_send_geo_wr ( astro_geo_wr_latitude , astro_geo_wr_longitude ) ;
}


