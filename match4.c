/*
 * match4.c
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
extern int seek_string ( char c, char * src, int src_len, int * current );

/* wildcards are restricted to matching only predefined known characters */
int knowledge_based_do_match_ex ( int argc, const char * knowledge [ ], char wildcard,
								  char * pattern, char * src, int src_len, int * next,
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

				if ( argc > 0 )
				{
					while ( i < src_len )
					{
						if ( knowledge_based_do_match_ex ( argc - 1, knowledge, wildcard, pos + k, src, src_len, & i, filter_on_terminate, filter_on_equal ) )
							goto quit;

						if ( knowledge [ argc - 1 ] && ! is_known_character ( knowledge [ argc - 1 ], *( src + i ) ) )
							break;

						if ( filter_on_terminate && filter_on_terminate ( src, src_len, & i ) )
							continue; /* must continue to test i < src_len now */

						i ++;
					}
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
	{
		if ( argc > 0 )
		{
			while ( i < src_len )
			{
				if ( knowledge [ argc - 1 ] && ! is_known_character ( knowledge [ argc - 1 ], *( src + i ) ) )
					break;

				if ( filter_on_terminate && filter_on_terminate ( src, src_len, & i ) )
					continue; /* must continue to test i < src_len now */

				i ++;
			}
		}
	}
quit:
	* next = i;

	return a;
}

int knowledge_based_copy_and_replace_ex ( int argc, const char * knowledge [ ], char wildcard, struct filter_t * filter,
										  char * src, int src_len, char * dst, int dst_size,
										  char * pattern, char * replace, char * exclude,
										  ... )
{
	char * pos, * posx;
	int i, ii, j, h, k, s, t;
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

		if ( ! knowledge_based_do_match_ex ( argc, knowledge, wildcard, pattern, src, src_len, & i, filter_on_terminate, filter_on_equal ) )
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

				if ( filter_on_load && ! filter_on_load ( src, src_len, ii, & i, dst, dst_size, & h ) )
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

		if ( filter_after_replace && ! filter_after_replace ( src, src_len, ii, & i, dst, dst_size, & h ) )
			return 0;
	}

	*( dst + h ) = 0;

	return h;
}

/* prototype knowledge_based_do_match_ex function */
int knowledge_based_match_ex ( int argc, char * knowledge [ ], char wildcard, char * pattern, char * src, int src_len, int granularity )
{
	int i, ii;

	hit_count = 0;

	i = 0;
	while ( i < src_len )
	{
		ii = i; /* save i = current index */

		if ( ! knowledge_based_do_match_ex ( argc, knowledge, wildcard, pattern, src, src_len, & i, NULL, NULL ) )
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
