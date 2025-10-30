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
			break;

		i ++; /* must be here, do NOT move this line */
	}

	* current = i;

	return ! a;
}

int copy_and_replace_ex2 ( const char * known, char wildcard, char * src, int src_len, char * dst, int dst_size,
						   char * pattern, char * replace, char * exclude,
						   ... )
{
	char * pos, * posx;
	int i, ii, iii, j, h, k, s, t;
	int len;
	va_list args;

	if ( dst_size < 1 ) /* size >= len + 1 */
		return 0;

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
	}

	*( dst + h ) = 0;

	return h;
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
