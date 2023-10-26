/*
 * my_conversions.c
 *
 *  Created on: Oct 7, 2023
 *      Author: mzeml
 */

#include "my_conversions.h"

double my_string2double_conv ( const char* s )
{
    double d = strtod ( s , NULL ) ;
    return d ;
}

float my_string2float_conv ( const char* s )
{
    float f = strtof ( s , NULL ) ;
    return f ;
}

//Find position in string of n occurance of the comma
uint8_t my_find_char_position ( const char* m , const char c , uint8_t n )
{
	uint8_t i = 0 ;
	uint8_t p = 0 ;

	while ( m[i] != '\0' )
	{
		if ( m[i] == c )
			p++ ;
		if ( p == n )
			break ;
		i++ ;
	}
	return i ;
}
