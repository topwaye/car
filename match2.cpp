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
 * escape character \a represents a runtime current matched string which can NOT be trimmed
 * escape character \b represents a runtime current matched string which can be trimmed (e.g. exclude somechar)
 * escape character \f represents a runtime placeholder which can be arbitrary string predefined
 */

#include <stdarg.h>

#include "config.h"

extern int hit_count;

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
	filter_operation_t filter_before_replace, filter_after_replace, filter_on_load, filter_on_custom;
	va_list args;

	if ( dst_size < 1 ) /* size >= len + 1 */
		return 0;

	hit_count = 0;

	filter_before_replace = filter ?  filter -> filter_before_replace : NULL;
	filter_after_replace = filter ?  filter -> filter_after_replace : NULL;
	filter_on_load = filter ?  filter -> filter_on_load : NULL;
	filter_on_custom = filter ?  filter -> filter_on_custom : NULL;

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

		hit_count ++;

		if ( filter_before_replace && ! filter_before_replace ( src, len, ii, & i, dst, dst_size, & h ) )
			return 0;

		va_start ( args, exclude );

		pos = replace;
	
		k = 0;
		while ( *( pos + k ) )
		{
			if ( h + 1 == dst_size )
				return 0;

			if ( '\b' == *( pos + k ) )
			{
				if ( filter_on_custom )
				{
					if ( ! filter_on_custom ( src, len, ii, & i, dst, dst_size, & h ) )
						return 0;
				}
				else
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
				}

				k ++;
				continue;
			}

			if ( '\a' == *( pos + k ) )
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

			if ( '\f' == *( pos + k ) )
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

		if ( filter_after_replace && ! filter_after_replace ( src, len, ii, & i, dst, dst_size, & h ) )
			return 0;
	}

	*( dst + h ) = 0;

	return h;
}

/*
 * this is a placeholder function
 * do anything you want to change when getting a runtime current matched string
 */
int filter_custom ( char * src, int src_len, int src_prior, int * src_index, char * dst, int dst_size, int * dst_index )
{
	int i, h, j;

	i = * src_index;
	h = * dst_index;

	j = src_prior;
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
	int i, h, j, r, t;

	i = * src_index;
	h = * dst_index;

	t = i; /* NOT src_prior */
	if ( ! seek_unknown_character ( KNOWN_ALPHABET_BLANK, src, src_len, & t ) )
		t = src_len;

	r = j = i;
	while ( seek_string ( '\n', src, t, & j ) ) /* NOT src_len */
		r = j ++;

	while ( r < t )
	{
		if ( h + 1 == dst_size )
			return 0;

		*( dst + h ++ ) = *( src + r ++ );
	}

	* src_index = i; /* NOT t */
	* dst_index = h;

	return 1; /* NOT 0 */
}

int filter_forward2 ( char * src, int src_len, int src_prior, int * src_index, char * dst, int dst_size, int * dst_index )
{
	int i, h, j, t;

	i = * src_index;
	h = * dst_index;

	t = i; /* NOT src_prior */
	if ( ! seek_unknown_character ( KNOWN_ALPHABET_BLANK, src, src_len, & t ) )
		t = src_len;

	j = i;
	while ( seek_string ( '\n', src, t, & j ) ) /* NOT src_len */
		i = j ++;

	* src_index = i; /* NOT t */
	* dst_index = h;

	return 1; /* NOT 0 */
}

int filter_forward3 ( char * src, int src_len, int src_prior, int * src_index, char * dst, int dst_size, int * dst_index )
{
	int i, h, t;

	i = * src_index;
	h = * dst_index;

	t = i; /* NOT src_prior */
	if ( ! seek_unknown_character ( KNOWN_ALPHABET_BLANK, src, src_len, & t ) )
		t = src_len;

	while ( seek_string ( '\n', src, t, & i ) ) /* NOT src_len */
		i ++;

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
