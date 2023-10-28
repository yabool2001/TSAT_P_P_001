/*
 * my_nmea.c
 *
 *  Created on: Oct 2, 2023
 *      Author: mzeml
 */

#include "my_nmea.h"

/* Nie chce działać
int my_nmea_message ( uint8_t* c , uint8_t* m , uint8_t* i )
{
    if ( *c == '$' )
    {
        *i = 0 ;
        m[(*i)++] = *c ;
        m[*i] = '\0' ;
        return 0 ;
    }
    if ( *c >= ' ' && *i > 0 )
    {
        m[(*i)++] = *c ;
        m[*i] = '\0' ;
        return 1 ;
    }
    else if ( *c == '\r' && *i > 0)
    {
    	*i = 0 ;
    	return 2 ;
    }
    return -1 ;
}
*/
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
/*
double nmea2decimal ( const char *coord , char dir )
{
    double deg , min ;
    sscanf ( coord , "%lf" , &deg ) ;
    min = deg / 100 ;
    deg = (int) min ;
    min = ( min - deg ) / 0.6 ;
    double c = deg + min ;
    if ( dir == 'S' || dir == 'W' )
    	c = c * -1 ;
    return c ;
    //double c = ( deg + min / 0.6 ) * ( ( dir == 'S' || dir == 'W' ) ? -1 : 1 ) ;
    //return ( deg + min / 0.6 ) * ( ( dir == 'S' || dir == 'W' ) ? -1 : 1 ) ;
}
*/
void get_my_nmea_gngll_coordinates_s ( const char* m , char* latitude , char* longitude )
{
	char direction ;

	//Latitude part
	uint8_t coordinate_position = my_find_char_position ( m , NMEA_DELIMETER , GLL_LATITUDE_POSITION ) + 1 ;
	uint8_t coordinate_length = my_find_char_position ( m , NMEA_DELIMETER , GLL_LATITUDE_POSITION + 1 ) - coordinate_position ;

	char* latitude_s = (char*) malloc ( ( coordinate_length +1 ) * sizeof ( char ) ) ;
	strncpy ( latitude_s , m + coordinate_position , coordinate_length ) ; // Kopiowanie fragmentu łańcucha
	latitude_s[coordinate_length] = '\0';
	direction = m[coordinate_position + coordinate_length + 1] ;
	double latitude_d = nmea2decimal ( latitude_s , direction ) ;
	free ( latitude_s ) ;
	latitude_d = round ( latitude_d * 1e6 ) / 1e6 ;
	snprintf ( latitude , 12 , "%.6lf" , latitude_d ) ;

	//Longitude part
	coordinate_position = my_find_char_position ( m , NMEA_DELIMETER , GLL_LATITUDE_POSITION + 2) + 1 ;
	//coordinate_length = my_find_char_position ( m , NMEA_DELIMETER , GLL_LATITUDE_POSITION + 1 ) - coordinate_position ;
	coordinate_length = my_find_char_position ( m , NMEA_DELIMETER , GLL_LATITUDE_POSITION + 2 + 1 ) - coordinate_position ;

	char* longitude_s = (char*) malloc ( ( coordinate_length +1 ) * sizeof ( char ) ) ;
	strncpy ( longitude_s , m + coordinate_position , coordinate_length ) ; // Kopiowanie fragmentu łańcucha
	longitude_s[coordinate_length] = '\0';
	direction = m[coordinate_position + coordinate_length + 1] ;
	double longitude_d = nmea2decimal ( longitude_s , direction ) ;
	free ( longitude_s ) ;
	longitude_d = round ( longitude_d * 1e6 ) / 1e6 ;
	snprintf ( longitude , 12 , "%.6lf" , longitude_d ) ;
}

/*
void get_my_nmea_longitude_s ( const char* m , char* coor )
{
	__NOP () ;
}
*/
