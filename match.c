/*
 * match.c
 *
 * Copyright (C) 2025.10.25 TOP WAYE topwaye@hotmail.com
 * 
 * copy and replace pattern words with predefined words
 * a wildcard (*) matches zero or more characters
 * 
 * as a result:
 * escape character \a represents a runtime current matched string which can NOT be trimmed
 * escape character \b represents a runtime current matched string which can be trimmed (e.g. exclude somechar)
 * escape character \f represents a runtime placeholder which can be arbitrary string predefined
 * escape character \v represents anything, the behavior of a \v is defined by filters
 *
 * the most fundamental feature of this algorithm is
 * wildcards cannot ensure the order of matching
 * only fixed characters can guarantee the matching order
 */

#include <stdarg.h>

#include "config.h"

extern int hit_count;
extern int is_known_character ( const char * known, char c );

int string_length ( const char * src )
{
	int i;

	i = 0;
	while ( *( src + i ) )
		i ++;

	return i;
}

int calculate_string ( int src_index, int src_delta, const char * src )
{
	int n;

	if ( src_delta < 0 ) /* NOT < 1 */
		return -1;

	n = string_length ( src );

	if ( src_index < 0 )
	{
		src_index = -src_index;

		if ( src_index > n )
			return -1;

		if ( src_index + src_delta > n + 1 )
			return -1;

		src_index = n + 1 - src_index - src_delta;
	}
	else
	{
		if ( src_index >= n )
			return -1;

		if ( src_index + src_delta > n )
			return -1;
	}

	return src_index;
}

int substring ( int src_index, int src_delta, char ** src )
{
	int i;

	i = calculate_string ( src_index, src_delta, * src );
	if ( i == -1 )
		return 0;

	* src += i;
	*( * src + src_delta ) = 0;

	return 1;
}

int trim_string_tail ( int src_delta, char * src )
{
	int i;

	i = calculate_string ( -1, src_delta, src );
	if ( i == -1 )
		return 0;

	*( src + i ) = 0;

	return 1;
}

int compare_string_ex ( int src_index, int src_delta, const char * src, const char * dst )
{
	int i, h;

	i = calculate_string ( src_index, src_delta, src );
	if ( i == -1 )
		return -1;

	h = 0;
	while ( src_delta -- )
		if ( *( src + i ++ ) != *( dst + h ++ ) )
			return 1;

	return 0; /* matched */
}

int compare_string ( const char * src, const char * dst )
{
	int i, h, ii;

	i = 0, h = 0;
	do
	{
		ii = i;
		if ( *( src + i ++ ) != *( dst + h ++ ) )
			return 1;

	} while ( *( src + ii ) );

	return 0; /* matched */
}

int seek_string ( char c, char * src, int src_len, int * current )
{
	int i;

	i = * current;

	while ( i < src_len )
	{
		if ( c == *( src + i ) )
		{
			* current = i;
			return 1;	
		}
		
		i ++; /* must be here, do NOT move this line */
	}

	return 0;
}

int concatenate_string ( const char * src, char * dst, int dst_size, ... )
{
	char * placeholder;
	int i, h, k, t;
	va_list args;

	if ( dst_size < 1 ) /* size >= len + 1 */
		return 0;

	va_start ( args, dst_size );

	i = 0;
	if ( ! seek_string ( 0, dst, dst_size, & i ) )
		return 0;

	h = i, k = 0;
	while ( *( src + k ) )
	{
		if ( h + 1 == dst_size )
		{
			*( dst + i ) = 0; /* roll back */
			return 0; /* NOT i */
		}

		if ( '\f' == *( src + k ) )
		{
			placeholder = va_arg ( args, char * );

			t = 0;
			while ( *( placeholder + t ) )
			{
				if ( h + 1 == dst_size )
				{
					*( dst + i ) = 0; /* roll back */
					return 0; /* NOT i */
				}
				*( dst + h ++ ) = *( placeholder + t ++ );
			}

			k ++;
			continue;
		}

		*( dst + h ++ ) = *( src + k ++ );
	}

	va_end ( args );

	*( dst + h ) = 0;

	return h;
}

int copy_string ( const char * src, char * dst, int dst_size, ... )
{
	char * placeholder;
	int h, k, t;
	va_list args;

	if ( dst_size < 1 ) /* size >= len + 1 */
		return 0;

	va_start ( args, dst_size );

	h = 0, k = 0;
	while ( *( src + k ) )
	{
		if ( h + 1 == dst_size )
			return 0;

		if ( '\f' == *( src + k ) )
		{
			placeholder = va_arg ( args, char * );

			t = 0;
			while ( *( placeholder + t ) )
			{
				if ( h + 1 == dst_size )
					return 0;

				*( dst + h ++ ) = *( placeholder + t ++ );
			}

			k ++;
			continue;
		}

		*( dst + h ++ ) = *( src + k ++ );
	}

	va_end ( args );

	*( dst + h ) = 0;

	return h;
}

int do_match_ex ( char wildcard, char * pattern, char * src, int src_len, int * next,
				  filter_terminate_t filter_on_terminate, filter_equal_t filter_on_equal )
{
	char * pos;
	int i, k;
	int a, b, ii;

	i = * next;

	a = 0, b = 0;
	pos = pattern;
	k = 0;
	while ( *( pos + k ) )
	{
		if ( ! a && ! b )
		{
			b = wildcard == *( pos + k );

			if ( ! b )
			{
				if ( '\v' == *( pos + k ) && filter_on_equal )
				{
					ii = i; /* save i = current index */
					if ( ! filter_on_equal ( pos, & k, src, src_len, & i ) )
						break;
				}
				else
				{
					if ( *( src + i ) != *( pos + k ) )
						break;
				
					ii = i ++;
				}
			}
			else
			{
				ii = i;
			}

			a = 1;
		}
		else if ( a && b )
		{
			b = wildcard == *( pos + k );

			if ( ! b )
			{
				/* a new block in pattern was found, therefore, reenter */

				while ( i < src_len )
				{
					if ( do_match_ex ( wildcard, pos + k, src, src_len, & i, filter_on_terminate, filter_on_equal ) )
						goto quit;

					if ( filter_on_terminate && filter_on_terminate ( src, src_len, & i ) )
						continue; /* must continue to test i < src_len now */						

					i ++;
				}

				a = 0;
				i = ii;
				break;
			}
		}
		else if ( a && ! b )
		{
			b = wildcard == *( pos + k );

			if ( ! b )
			{
				if ( i == src_len ) /* must be here, do NOT move this line */
				{
					a = 0;
					i = ii;
					break;
				}

				if ( '\v' == *( pos + k ) && filter_on_equal )
				{
					if ( ! filter_on_equal ( pos, & k, src, src_len, & i ) )
					{
						a = 0;
						i = ii;
						break;
					}
				}
				else
				{
					if ( *( src + i ) != *( pos + k ) )
					{
						a = 0;
						i = ii;
						break;
					}

					i ++;
				}
			}
		}
		else /* ! a && b */
		{
			/* never be here */
		}

		k ++;
	} /* end while */

	if ( a && b ) /* b must be after a, do NOT move this line */
		i = src_len;
quit:
	* next = i;

	return a;
}

int copy_and_replace_ex ( char wildcard, struct filter_t * filter, char * src, int src_len, char * dst, int dst_size,
						  char * pattern, char * replace, char * exclude,
						  ... )
{
	char * pos, * posx;
	int i, ii, j, h, k;
	filter_initiate_t filter_on_initiate;
	filter_terminate_t filter_on_terminate;
	filter_equal_t filter_on_equal;
	filter_exclude_t filter_on_exclude;
	filter_operation_t filter_before_replace, filter_after_replace, filter_on_load;
	va_list args;

	if ( dst_size < 1 ) /* size >= len + 1 */
		return 0;

	hit_count = 0;

	filter_on_initiate = filter ? filter -> filter_on_initiate : NULL;
	filter_on_terminate = filter ? filter -> filter_on_terminate : NULL;
	filter_on_equal = filter ? filter -> filter_on_equal : NULL;
	filter_on_exclude = filter ? filter -> filter_on_exclude : NULL;
	filter_before_replace = filter ? filter -> filter_before_replace : NULL;
	filter_after_replace = filter ? filter -> filter_after_replace : NULL;
	filter_on_load = filter ? filter -> filter_on_load : NULL;

	h = 0, i = 0; 
	while ( i < src_len )
	{
		ii = i; /* save i = current index */

		if ( filter_on_initiate && filter_on_initiate ( src, src_len, & i ) )
		{
			j = ii;
			while ( j < i )
			{
				if ( h + 1 == dst_size )
					return 0;

				*( dst + h ++ ) = *( src + j ++ );
			}

			continue; /* must continue to test i < src_len now */
		}

		if ( ! do_match_ex ( wildcard, pattern, src, src_len, & i, filter_on_terminate, filter_on_equal ) )
		{
			if ( h + 1 == dst_size )
				return 0;

			*( dst + h ++ ) = *( src + i ++ );
			
			continue;
		}
	
		hit_count ++;

		if ( filter_before_replace && ! filter_before_replace ( src, src_len, ii, & i, dst, dst_size, & h ) )
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
				if ( filter_on_exclude )
				{
					if ( ! filter_on_exclude ( src, src_len, ii, & i, dst, dst_size, & h, exclude ) )
						return 0;
				}
				else
				{
					j = ii;
					while ( j < i )
					{
						if ( h + 1 == dst_size )
							return 0;

						if ( is_known_character ( exclude, *( src + j ) ) )
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

				if ( filter_on_load && ! filter_on_load ( src, src_len, ii, & i, dst, dst_size, & h ) )
					return 0;

				k ++;
				continue;
			}

			if ( '\f' == *( pos + k ) )
			{
				posx = va_arg ( args, char * );

				j = 0;
				while ( *( posx + j ) )
				{
					if ( h + 1 == dst_size )
						return 0;

					*( dst + h ++ ) = *( posx + j ++ );
				}

				k ++;
				continue;
			}

			*( dst + h ++ ) = *( pos + k ++ );
		}

		va_end ( args );

		if ( filter_after_replace && ! filter_after_replace ( src, src_len, ii, & i, dst, dst_size, & h ) )
			return 0;
	}

	*( dst + h ) = 0;

	return h;
}

int do_match ( char * target, char * src, int src_len, int * next )
{
	char * pos;
	int i, k;
	int a, ii;

	i = * next;

	a = 0;
	pos = target;
	k = 0; 
	while ( *( pos + k ) )
	{
		if ( ! a )
		{
			if ( *( src + i ) != *( pos + k ) )
				break;

			a = 1;
			ii = i ++;
		}
		else
		{
			if ( i == src_len )
			{
				a = 0;
				i = ii;
				break;
			}
					
			if ( *( src + i ) != *( pos + k ) )
			{
				a = 0;
				i = ii;
				break;
			}

			i ++;
		}

		k++;
	} /* end while */

	* next = i;

	return a;
}

/* prototype do_match_ex function */
int match_ex ( char wildcard, char * pattern, char * src, int src_len, int granularity )
{
	int i, ii; 

	hit_count = 0;

	i = 0;
	while ( i < src_len )
	{
		ii = i; /* save i = current index */

		if ( ! do_match_ex ( wildcard, pattern, src, src_len, & i, NULL, NULL ) )
		{
			/* match failed when i = current index, i is not changed */

			if ( granularity && ! seek_string ( '\n', src, src_len, & i ) ) /* start a new line hopefully */
				break;

			i ++; /* add 1 manually */

			continue;
		}

		/* match succeeded when i = current index, i is changed to next index automatically */
		hit_count ++;

		return i - ii; /* matched string length */
	}

	return 0;
}

/* prototype do_match function */
int match ( char * target, char * src, int src_len, int granularity )
{
	int i, ii; 

	hit_count = 0;

	i = 0;
	while ( i < src_len )
	{
		ii = i; /* save i = current index */

		if ( ! do_match ( target, src, src_len, & i ) )
		{
			/* match failed when i = current index, i is not changed */

			if ( granularity && ! seek_string ( '\n', src, src_len, & i ) ) /* start a new line hopefully */
				break;

			i ++; /* add 1 manually */

			continue;
		}

		/* match succeeded when i = current index, i is changed to next index automatically */
		hit_count ++;

		return i - ii; /* matched string length */
	}

	return 0;
}

int copy_and_replace ( char * src, int src_len, char * dst, int dst_size,
					   char * target, char * replace )
{
	int i, h, k;

	if ( dst_size < 1 ) /* size >= len + 1 */
		return 0;

	hit_count = 0;

	h = 0, i = 0; 
	while ( i < src_len )
	{
		if ( ! do_match ( target, src, src_len, & i ) )
		{
			if ( h + 1 == dst_size )
				return 0;

			*( dst + h ++ ) = *( src + i ++ );
			
			continue;
		}

		hit_count ++;

		k = 0;
		while ( *( replace + k ) )
		{
			if ( h + 1 == dst_size )
				return 0;

			*( dst + h ++ ) = *( replace + k ++ );
		}
	}

	*( dst + h ) = 0;

	return h;
}
