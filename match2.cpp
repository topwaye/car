/*
 * match.c
 *
 * Copyright (C) 2025.10.30 TOP WAYE topwaye@hotmail.com
 * 
 * copy and replace pattern words with predefined words
 * a wildcard (*) matches zero or more characters
 * only matches any one of the known characters from predefined character set (e.g. 1234567890)
 * 
 * as a result:
 * sign # represents a runtime current matched string which can NOT be trimmed
 * sign @ represents a runtime current matched string which can be trimmed (e.g. exclude somechar)
 * sign & represents a runtime placeholder which can be arbitrary string predefined
 */

#include <stdarg.h>

#include "config.h"

extern int seek_string ( char c, char * src, int src_len, int * current );
extern int do_match_ex ( char wildcard, char * pattern, char * src, int src_len, int * next );

int is_known_character ( char c, const char * known )
{
	int i;

	i = 0;
	while ( *( known + i ) )
	{
		if ( c == *( known + i ) )
			return 1;
		
		i ++; /* must be here, do NOT move this line */
	}

	return 0;
}

int seek_unknown_character ( const char * known, char * src, int src_len, int * current )
{
	int i, k;
	int a;

	i = * current;

	while ( i < src_len )
	{
		a = 0; k = 0;
		while ( *( known + k ) )
		{
			if ( *( src + i ) == *( known + k ) )
			{
				a = 1;
				break;
			}

			k++;
		}
		if ( ! a )
		{
			* current = i;
			return 1;
		}

		i ++; /* must be here, do NOT move this line */
	}

	return 0;
}

int copy_and_replace_ex2 ( const char * known, char wildcard, char * src, int src_len, char * dst, int dst_size,
						   struct filter_t * filter,
						   char * pattern, char * replace, char * exclude,
						   ... )
{
	char * pos, * posx;
	int i, ii, iii, j, h, k, s, t;
	int len;
	int ( * filter_on_replace ) ( char *, int, int, int *, char *, int, int * );
	int ( * filter_on_load ) ( char *, int, int, int *, char *, int, int * );
	va_list args;

	if ( dst_size < 1 ) /* size >= len + 1 */
		return 0;

	filter_on_replace = filter ?  filter -> filter_on_replace : NULL;
	filter_on_load = filter ?  filter -> filter_on_load : NULL;

	h = 0, i = 0; 
	while ( i < src_len )
	{
		if ( ! is_known_character ( *( src + i ), known ) )
		{
			if ( h + 1 == dst_size )
				return 0;

			*( dst + h ++ ) = *( src + i ++ );

			continue;
		}

		iii = ii = i; /* save i = current index */

		len = ! seek_unknown_character ( known, src, src_len, & iii ) ? src_len : iii; /* NOT iii - ii */

		if ( ! do_match_ex ( wildcard, pattern, src, len, & i ) )
		{
			if ( h + 1 == dst_size )
				return 0;

			*( dst + h ++ ) = *( src + i ++ );
			
			continue;
		}

		va_start ( args, exclude );

		pos = replace;
	
		k = 0;
		while ( *( pos + k ) )
		{
			if ( h + 1 == dst_size )
				return 0;

			if ( '@' == *( pos + k ) )
			{
				j = ii;
				while ( j < i )
				{
					if ( h + 1 == dst_size )
						return 0;

					posx = exclude;

					s = 0, t = 0;
					while ( *( posx + t ) )
					{
						if ( *( src + j ) == *( posx + t ) )
						{
							s = 1;
							break;
						}
						t ++;
					}
					if ( s )
					{
						j ++;
						continue;
					}

					*( dst + h ++ ) = *( src + j ++ );
				}

				k ++;
				continue;
			}

			if ( '#' == *( pos + k ) )
			{
				j = ii;
				while ( j < i )
				{
					if ( h + 1 == dst_size )
						return 0;

					*( dst + h ++ ) = *( src + j ++ );
				}

				if ( filter_on_load && ! filter_on_load ( src, len, ii, & i, dst, dst_size, & h ) )
					return 0;

				k ++;
				continue;
			}

			if ( '&' == *( pos + k ) )
			{
				posx = va_arg ( args, char * );

				t = 0;
				while ( *( posx + t ) )
				{
					if ( h + 1 == dst_size )
						return 0;

					*( dst + h ++ ) = *( posx + t ++ );
				}

				k ++;
				continue;
			}

			*( dst + h ++ ) = *( pos + k ++ );
		}

		va_end ( args );

		if ( filter_on_replace && ! filter_on_replace ( src, len, ii, & i, dst, dst_size, & h ) )
			return 0;
	}

	*( dst + h ) = 0;

	return h;
}

int filter_backward ( char * src, int src_len, int src_prior, int * src_index, char * dst, int dst_size, int * dst_index )
{
	int i, h, j;

	i = * src_index;
	h = * dst_index;

	j = src_prior;
	while ( seek_string ( '\n', src, src_len, & j ) )
		j ++;

	while ( j < i )
	{
		if ( h + 1 == dst_size )
			return 0;

		*( dst + h ++ ) = *( src + j ++ );
	}

	* src_index = i;
	* dst_index = h;

	return 1;
}

int filter_forward ( char * src, int src_len, int src_prior, int * src_index, char * dst, int dst_size, int * dst_index )
{
	int i, h, j, t;

	i = * src_index;
	h = * dst_index;

	t = j = i; /* NOT src_prior */
	if ( ! seek_unknown_character ( KNOWN_ALPHABET_BLANK, src, src_len, & t ) )
		t = src_len;

	while ( j < t )
	{
		if ( h + 1 == dst_size )
			return 0;

		*( dst + h ++ ) = *( src + j ++ );
	}

	* src_index = i; /* NOT t */
	* dst_index = h;

	return 1; /* NOT 0 */
}

int match_ex2 ( char * known, char wildcard, char * pattern, char * src, int src_len, int granularity )
{
	int i, ii, iii; 
	int len;

	i = 0;
	while ( i < src_len )
	{
		if ( ! is_known_character ( *( src + i ), known ) )
		{
			i ++;
			continue;
		}

		iii = ii = i; /* save i = current index */

		len = ! seek_unknown_character ( known, src, src_len, & iii ) ? src_len : iii; /* NOT iii - ii */

		if ( ! do_match_ex ( wildcard, pattern, src, len, & i ) )
		{
			/* match failed when i = current index, i is not changed */

			if ( granularity && ! seek_string ( '\n', src, src_len, & i ) ) /* start a new line hopefully */
				break;

			i ++; /* add 1 manually */

			continue;
		}

		/* match succeeded when i = current index, i is changed to next index automatically */

		return i - ii; /* matched string length */
	}

	return 0;
}
