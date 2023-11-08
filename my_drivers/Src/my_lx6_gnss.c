/*
 * my_lx6_gnss.c
 *
 *  Created on: Oct 27, 2023
 *      Author: mzeml
 */

#include "my_lx6_gnss.h"


bool my_lx6_get_coordinates ( uint16_t active_time_ths , double nmea_pdop_ths , double* nmea_fixed_pdop_d , int32_t* astro_geo_wr_latitude , int32_t* astro_geo_wr_longitude )
{
	bool		r = false ;
	uint8_t		rxd_byte = 0 ;
	bool 		received_nmea_rmc_flag = false ;
	uint8_t		nmea_message[NMEA_MESSAGE_SIZE] = {0} ;
	uint8_t		gngll_message[NMEA_MESSAGE_SIZE] = {0} ;
	uint8_t		rmc_message[NMEA_MESSAGE_SIZE] = {0} ;
	uint8_t		i_nmea = 0 ;
	char 		nmea_latitude_s[MY_GNSS_NMEA_MAX_SIZE] = {0} ; // 10 + ew. znak minus + '\0'
	char 		nmea_longitude_s[MY_GNSS_NMEA_MAX_SIZE] = {0} ; // 10 + ew. znak minus + '\0'
	char 		nmea_coordinates_log[52] ; // Nagłowek + 12 + ew. znak minus + '\0'
	char* 		nmea_gngsa_label = "GNGSA" ;
	char* 		nmea_gngll_label = "GNGLL" ;
	char* 		nmea_rmc_label = "RMC" ;
	char		nmea_fixed_mode_s = '\0' ;

	tim_seconds = 0 ;
	HAL_TIM_Base_Start_IT ( &htim6 ) ;
	my_lx6_on () ;
	while ( tim_seconds < active_time_ths  ) // 1200 = 10 min.
	{
		HAL_UART_Receive ( HUART_Lx6 , &rxd_byte , 1 , UART_TIMEOUT ) ;
		HAL_UART_Transmit ( HUART_DBG , &rxd_byte , 1 , UART_TIMEOUT ) ; // Transmit all nmea to DBG
		if ( rxd_byte )
		{
			if ( my_nmea_message ( &rxd_byte , nmea_message , &i_nmea ) == 2 )
			{
				if ( is_my_nmea_checksum_ok ( (char*) nmea_message ) )
				{
					if ( strstr ( (char*) nmea_message , nmea_rmc_label ) && !received_nmea_rmc_flag )
					{
						set_my_rtc_from_nmea_rmc ( (char*) nmea_message ) ;
						received_nmea_rmc_flag = true ;
					}
					/*
					if ( strstr ( (char*) nmea_message , nmea_rmc_label ) && !received_nmea_rmc_flag )
					{
						set_my_rtc_from_nmea_rmc ( (char*) nmea_message ) ;
						received_nmea_rmc_flag = true ;
					}
					*/
					if ( strstr ( (char*) nmea_message , nmea_gngsa_label ) )
					{
						nmea_fixed_mode_s = get_my_nmea_gngsa_fixed_mode_s ( (char*) nmea_message ) ;
						*nmea_fixed_pdop_d = get_my_nmea_gngsa_pdop_d ( (char*) nmea_message ) ;
					}
					if ( strstr ( (char*) nmea_message , nmea_gngll_label ) /*&& nmea_fixed_pdop_d <= nmea_pdop_ths */)
					{
						if ( *nmea_fixed_pdop_d <= nmea_pdop_ths )
						{
							get_my_nmea_gngll_coordinates ( (char*) nmea_message , nmea_latitude_s , nmea_longitude_s , astro_geo_wr_latitude , astro_geo_wr_longitude ) ; // Nie musze nic kombinować z przenoszeniem tej operacji, bo po niej nie będzie już dalej odbierania wiadomości tylko wyjście
						}
						else
						{
							memcpy ( gngll_message , nmea_message , NMEA_MESSAGE_SIZE ) ; // Zapisuję, żeby potem, jak nie osiągnę jakości nmea_pdop_ths to wykorzystać coordinates do payload
						}
					}
				}
			}
		}
		if ( tim_seconds > MY_GNSS_RMC_TIME_THS && !received_nmea_rmc_flag )
		{
			break ;
		}
		if ( *nmea_fixed_pdop_d <= nmea_pdop_ths )
		{
			if ( nmea_latitude_s[0] != 0 )
			{
				if ( nmea_fixed_mode_s == NMEA_3D_FIX )
				{
					if ( received_nmea_rmc_flag )
					{
						r = true ;
						break ;
					}
				}
			}
		}
	}
	HAL_TIM_Base_Stop_IT ( &htim6 ) ;
	my_lx6_off () ;
	if ( nmea_latitude_s[0] == 0 && gngll_message[0] != 0 ) // Jeśli nie masz współrzędnych pdop to wykorzystaja gorsze i zrób ich backup
	{
		get_my_nmea_gngll_coordinates ( (char*) gngll_message , nmea_latitude_s , nmea_longitude_s , astro_geo_wr_latitude , astro_geo_wr_longitude ) ;
		r = true ;
	}
	sprintf ( nmea_coordinates_log , "NMEA coordinates: %s,%s" , nmea_latitude_s , nmea_longitude_s ) ;
	send_debug_logs ( nmea_coordinates_log ) ;
	return r ;
}
