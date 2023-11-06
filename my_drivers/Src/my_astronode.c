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

	HAL_TIM_Base_Start_IT ( MY_TIMER ) ;
	while ( tim_seconds < MY_ASTRO_INIT_TIME && !cfg_wr )
	{
		cfg_wr = astronode_send_cfg_wr ( true , true , true , false , true , true , true , false ) ;
		my_astro_off () ;
		HAL_Delay ( 3000 ) ;
		my_astro_on () ;
		reset_astronode () ;
	}
	tim_seconds = 0 ;
	HAL_TIM_Base_Stop_IT ( MY_TIMER ) ;
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
bool my_astro_add_payload_2_queue ( char* payload )
{
	uint16_t id = 0 ;
	size_t l = strlen ( payload ) ;
	if ( l <= ASTRONODE_APP_PAYLOAD_MAX_LEN_BYTES )
	{
		if ( astronode_send_pld_er ( id , payload , l ) )
		{
			return true ;
		}
	}
	else
	{
		send_debug_logs ( "ERROR: Payload exceeded ASTRONODE_APP_PAYLOAD_MAX_LEN_BYTES value." ) ;
	}
	return false ;
}
bool my_astro_read_evt_reg ( void )
{
	send_debug_logs ( "my_astro: evt pin is high." ) ;
	astronode_send_evt_rr () ;
	if (is_sak_available () )
	{
	  astronode_send_sak_rr () ;
	  astronode_send_sak_cr () ;
	  send_debug_logs ( "my_astro: message has been acknowledged." ) ;
	  //astronode_send_per_rr () ;
	}
	if ( is_astronode_reset () )
	{
	  send_debug_logs ( "my_astro: terminal has been reset." ) ;
	  astronode_send_res_cr () ;
	}
	if ( is_command_available () )
	{
	  send_debug_logs ( "my_astro: unicast command is available" ) ;
	  astronode_send_cmd_rr () ;
	  astronode_send_cmd_cr () ;
	}
	return true ;
}
bool my_astro_log ( void )
{
	astronode_send_rtc_rr ();
	astronode_send_nco_rr () ;
	//astronode_send_lcd_rr () ;
	//astronode_send_end_rr () ;
	//astronode_send_per_rr () ;
	return true ;
}

void my_astro_write_coordinates ( int32_t astro_geo_wr_latitude , int32_t astro_geo_wr_longitude )
{
	astronode_send_geo_wr ( astro_geo_wr_latitude , astro_geo_wr_longitude ) ;
}


