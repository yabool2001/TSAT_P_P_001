/*
 * my_nmea.c
 *
 *  Created on: Oct 2, 2023
 *      Author: mzeml
 */

#include <inttypes.h>
#include "my_nmea.h"


int my_nmea_message ( uint8_t* c , uint8_t* m , uint8_t* i )
{
    if ( *c == '$' )
    {
        *i = 0 ;
        m[(*i)++] = *c ;
        m[*i] = '\0' ;
        return 0 ;
    }
    if ( ( *c >= ' ' && *c <= '~' && *i > 0 ) || *c == '\r' )
    {
        m[(*i)++] = *c ;
        m[*i] = '\0' ;
        return 1 ;
    }
    if ( *c == '\n' && *i > 1 )
    {
        if ( m[--(*i)] == '\r' )
        {
            m[*i] = '\0' ;
            return 2 ;
        }
    }
    return -1 ;
}

const char get_my_nmea_gngsa_fixed_mode_s ( const char* m )
{
	return m[9] ;
}

double get_my_nmea_gngsa_pdop_d ( const char* m )
{
	uint8_t pdop_position = my_find_char_position ( m , NMEA_DELIMETER , GSA_PDOP_POSITION ) + 1 ;
	uint8_t pdop_length = my_find_char_position ( m , NMEA_DELIMETER , GSA_PDOP_POSITION + 1 ) - pdop_position ;

	char* pdop_s = (char*) malloc ( ( pdop_length +1 ) * sizeof ( char ) ) ;
	strncpy ( pdop_s , m + pdop_position , pdop_length ) ; // Kopiowanie fragmentu łańcucha
	pdop_s[pdop_length] = '\0';
	double pdop = my_string2double_conv ( pdop_s ) ;
	free ( pdop_s ) ;
	return pdop ; // przed zwróceniem zaokrąglij do 2 miejsc po przecinku
}


bool is_my_nmea_checksum_ok ( const char* s )
{
    uint8_t cs = 0 ;
    uint8_t i = 1 ; // Start from index 1 to skip the '$' character
    while ( s[i] != '*' && s[i] != '\0' && i != 0 )
        cs ^= s[i++] ;
    //uint8_t* b = &s[++i] ;
    //uint8_t* c = strtol ( b , NULL, 16 ) ;
    return ( cs == strtol ( (char*) &s[++i] , NULL, 16 ) ) ? true : false ;
}
double nmea2decimal ( const char *coord , char dir )
{
    double deg , min ;
    sscanf ( coord , "%lf" , &deg ) ;
    min = deg / 100 ;
    deg = (int) min ;
    min = ( min - deg ) * 10 ;
    min = min / 6 ;
    //double c = deg + min ;
    if ( dir == 'S' || dir == 'W' )
    	return ( deg + min ) * -1 ;
    else
    	return deg + min ;
}

void get_my_nmea_gngll_coordinates ( const char* m , char* latitude_s , char* longitude_s , int32_t* latitude_astro_geo_wr , int32_t* longitude_astro_geo_wr )
{
	char direction ;

	//Latitude part
	uint8_t coordinate_position = my_find_char_position ( m , NMEA_DELIMETER , GLL_LATITUDE_POSITION ) + 1 ;
	uint8_t coordinate_length = my_find_char_position ( m , NMEA_DELIMETER , GLL_LATITUDE_POSITION + 1 ) - coordinate_position ;

	char* latitude_s_temp = (char*) malloc ( ( coordinate_length +1 ) * sizeof ( char ) ) ;
	strncpy ( latitude_s_temp , m + coordinate_position , coordinate_length ) ; // Kopiowanie fragmentu łańcucha
	latitude_s_temp[coordinate_length] = '\0';
	direction = m[coordinate_position + coordinate_length + 1] ;
	double latitude_d = nmea2decimal ( latitude_s_temp , direction ) ;
	free ( latitude_s_temp ) ;
	latitude_d = round ( latitude_d * 1e6 ) / 1e6 ;
	snprintf ( latitude_s , 12 , "%.6lf" , latitude_d ) ;
	*latitude_astro_geo_wr = (int32_t) ( latitude_d * 10000000 ) ;

	//Longitude part
	coordinate_position = my_find_char_position ( m , NMEA_DELIMETER , GLL_LATITUDE_POSITION + 2) + 1 ;
	coordinate_length = my_find_char_position ( m , NMEA_DELIMETER , GLL_LATITUDE_POSITION + 2 + 1 ) - coordinate_position ;

	char* longitude_s_temp = (char*) malloc ( ( coordinate_length +1 ) * sizeof ( char ) ) ;
	strncpy ( longitude_s_temp , m + coordinate_position , coordinate_length ) ; // Kopiowanie fragmentu łańcucha
	longitude_s_temp[coordinate_length] = '\0';
	direction = m[coordinate_position + coordinate_length + 1] ;
	double longitude_d = nmea2decimal ( longitude_s_temp , direction ) ;
	free ( longitude_s_temp ) ;
	longitude_d = round ( longitude_d * 1e6 ) / 1e6 ;
	snprintf ( longitude_s , 12 , "%.6lf" , longitude_d ) ;
	*longitude_astro_geo_wr = (int32_t) ( longitude_d * 10000000 ) ;
}
uint16_t my_nmea_get_gsv_tns ( const char* m ) //tns = Total Number Satelites
{
	uint16_t tns = 0 ;
	uint8_t position = my_find_char_position ( m , NMEA_DELIMETER , GSV_TNS_POSITION ) + 1 ;
	char* s = (char*) malloc ( ( GSV_TNS_LENGTH +1 ) * sizeof ( uint8_t ) ) ;
	strncpy ( s , m + position , GSV_TNS_LENGTH ) ; // Kopiowanie fragmentu łańcucha
	s[GSV_TNS_LENGTH] = '\0';
	sscanf ( s , "%hu" , &tns ) ;
	free ( s ) ;
	return tns ;
}

void my_nmea_get_rmc_date_yy ( const char* m , uint8_t* yy )
{
	uint16_t temp ;
	uint8_t position = my_find_char_position ( m , NMEA_DELIMETER , RMC_DATE_POSITION ) + 1 + RMC_DATE_YY_OFFSET ;
	char* s = (char*) malloc ( ( RMC_DATE_PART_LENGTH +1 ) * sizeof ( uint8_t ) ) ;
	strncpy ( s , m + position , RMC_DATE_PART_LENGTH ) ; // Kopiowanie fragmentu łańcucha
	s[RMC_DATE_PART_LENGTH] = '\0';
	//sscanf ( s , SCNu8 , yy ) ; // Version requires Properties: C build/Settings/MCU Settings/Standard C
	// Version for Properties: C build/Settings/MCU Settings/Reduced C (--specs=nano.specs)
	sscanf ( s , "%hu" , &temp ) ;
	free ( s ) ;
	*yy = (uint8_t) temp ;

}
void my_nmea_get_rmc_date_mm ( const char* m , uint8_t* mm )
{
	uint16_t temp ;
	uint8_t position = my_find_char_position ( m , NMEA_DELIMETER , RMC_DATE_POSITION ) + 1 + RMC_DATE_MM_OFFSET ;
	char* s = (char*) malloc ( ( RMC_DATE_PART_LENGTH +1 ) * sizeof ( uint8_t ) ) ;
	strncpy ( s , m + position , RMC_DATE_PART_LENGTH ) ; // Kopiowanie fragmentu łańcucha
	s[RMC_DATE_PART_LENGTH] = '\0';
	//sscanf ( s , SCNu8 , mm ) ; // Version requires Properties: C build/Settings/MCU Settings/Standard C
	// Version for Properties: C build/Settings/MCU Settings/Reduced C (--specs=nano.specs)
	sscanf ( s , "%hu" , &temp ) ;
	free ( s ) ;
	*mm = (uint8_t) temp ;

}
void my_nmea_get_rmc_date_dd ( const char* m , uint8_t* dd )
{
	uint16_t temp ;
	uint8_t position = my_find_char_position ( m , NMEA_DELIMETER , RMC_DATE_POSITION ) + 1 + RMC_DATE_DD_OFFSET ;
	char* s = (char*) malloc ( ( RMC_DATE_PART_LENGTH +1 ) * sizeof ( uint8_t ) ) ;
	strncpy ( s , m + position , RMC_DATE_PART_LENGTH ) ; // Kopiowanie fragmentu łańcucha
	s[RMC_DATE_PART_LENGTH] = '\0';
	//sscanf ( s , SCNu8 , dd ) ; // Version requires Properties: C build/Settings/MCU Settings/Standard C
	// Version for Properties: C build/Settings/MCU Settings/Reduced C (--specs=nano.specs)
	sscanf ( s , "%hu" , &temp ) ;
	free ( s ) ;
	*dd = (uint8_t) temp ;

}
void my_nmea_get_rmc_utc_hh ( const char* m , uint8_t* hh )
{
	uint16_t temp ;
	uint8_t position = my_find_char_position ( m , NMEA_DELIMETER , RMC_UTC_POSITION ) + 1 + RMC_UTC_HH_OFFSET ;
	char* s = (char*) malloc ( ( RMC_UTC_PART_LENGTH +1 ) * sizeof ( uint8_t ) ) ;
	strncpy ( s , m + position , RMC_UTC_PART_LENGTH ) ; // Kopiowanie fragmentu łańcucha
	s[RMC_UTC_PART_LENGTH] = '\0';
	//sscanf ( s , SCNu8 , hh ) ; // Version requires Properties: C build/Settings/MCU Settings/Standard C
	// Version for Properties: C build/Settings/MCU Settings/Reduced C (--specs=nano.specs)
	sscanf ( s , "%hu" , &temp ) ;
	free ( s ) ;
	*hh = (uint8_t) temp ;

}
void my_nmea_get_rmc_utc_mm ( const char* m , uint8_t* mm )
{
	uint16_t temp ;
	uint8_t position = my_find_char_position ( m , NMEA_DELIMETER , RMC_UTC_POSITION ) + 1 + RMC_UTC_MM_OFFSET ;
	char* s = (char*) malloc ( ( RMC_UTC_PART_LENGTH +1 ) * sizeof ( uint8_t ) ) ;
	strncpy ( s , m + position , RMC_UTC_PART_LENGTH ) ; // Kopiowanie fragmentu łańcucha
	s[RMC_UTC_PART_LENGTH] = '\0';
	//sscanf ( s , SCNu8 , mm ) ; // Version requires Properties: C build/Settings/MCU Settings/Standard C
	// Version for Properties: C build/Settings/MCU Settings/Reduced C (--specs=nano.specs)
	sscanf ( s , "%hu" , &temp ) ;
	free ( s ) ;
	*mm = (uint8_t) temp ;

}
void my_nmea_get_rmc_utc_ss ( const char* m , uint8_t* ss )
{
	uint16_t temp ;
	uint8_t position = my_find_char_position ( m , NMEA_DELIMETER , RMC_UTC_POSITION ) + 1 + RMC_UTC_SS_OFFSET ;
	char* s = (char*) malloc ( ( RMC_UTC_PART_LENGTH +1 ) * sizeof ( uint8_t ) ) ;
	strncpy ( s , m + position , RMC_UTC_PART_LENGTH ) ; // Kopiowanie fragmentu łańcucha
	s[RMC_UTC_PART_LENGTH] = '\0';
	//sscanf ( s , SCNu8 , ss ) ; // Version requires Properties: C build/Settings/MCU Settings/Standard C
	// Version for Properties: C build/Settings/MCU Settings/Reduced C (--specs=nano.specs)
	sscanf ( s , "%hu" , &temp ) ;
	free ( s ) ;
	*ss = (uint8_t) temp ;

}
void my_nmea_get_rmc_utc_sss ( const char* m , uint32_t* sss )
{
	uint8_t position = my_find_char_position ( m , NMEA_DELIMETER , RMC_UTC_POSITION ) + 1 + RMC_UTC_SSS_OFFSET ;
	char* s = (char*) malloc ( ( RMC_UTC_SSS_LENGTH +1 ) * sizeof ( uint8_t ) ) ;
	strncpy ( s , m + position , RMC_UTC_SSS_LENGTH ) ; // Kopiowanie fragmentu łańcucha
	s[RMC_UTC_SSS_LENGTH] = '\0';
	sscanf ( s , "%lu" , sss ) ;
	free ( s ) ;
}

/*
void get_my_nmea_longitude_s ( const char* m , char* coor )
{
	__NOP () ;
}
*/
