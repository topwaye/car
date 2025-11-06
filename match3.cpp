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
 * escape character \v represents anything, the behavior of a \v is defined by filters.
 */

#include <stdarg.h>

#include "config.h"

extern int hit_count;

extern int do_match_ex ( char wildcard, char * pattern, char * src, int src_len, int * next,
						 filter_initiate_t filter_initiate, filter_equal_t filter_equal );

int multiple_copy_and_replace_ex ( int argc, char wildcards [ ], struct filter_t * filters [ ], char * src, int src_len, char * dst, int dst_size,
								   char * patterns [ ], char * replaces [ ], char * excludes [ ],
								   ... )
{
	char * pos, * posx;
	int n, m, i, ii, j, h, k, s, t;
	int no_relay_initiate;
	filter_initiate_t filter_initiate;
	filter_equal_t filter_equal;
	filter_operation_t filter_before_replace, filter_after_replace, filter_on_load, filter_on_custom;
	va_list args;

	if ( dst_size < 1 ) /* size >= len + 1 */
		return 0;

	hit_count = 0;

	h = 0, i = 0; 
	while ( i < src_len )
	{
		m = 0;
		for ( n = 0; n < argc; n ++ )
		{
			no_relay_initiate = filters [ n ] -> no_relay_initiate;
			filter_initiate = filters [ n ] -> filter_initiate;
			filter_equal = filters [ n ] -> filter_equal;
			filter_before_replace = filters [ n ] -> filter_before_replace;
			filter_after_replace = filters [ n ] -> filter_after_replace;
			filter_on_load = filters [ n ] -> filter_on_load;
			filter_on_custom = filters [ n ] -> filter_on_custom;
	
			ii = i; /* save i = current index */

			if ( filter_initiate && filter_initiate ( src, src_len, & i ) )
			{
				j = ii;
				while ( j < i )
				{
					if ( h + 1 == dst_size )
						return 0;

					*( dst + h ++ ) = *( src + j ++ );
				}

				if ( i == src_len ) /* must test i < src_len now */
					goto quit;
			}

			if ( do_match_ex ( wildcards [ n ], patterns [ n ], src, src_len, & i, no_relay_initiate ? NULL : filter_initiate, filter_equal ) )
			{
				m = 1;
				
				hit_count ++;

				if ( filter_before_replace && ! filter_before_replace ( src, src_len, ii, & i, dst, dst_size, & h ) )
					return 0;

				va_start ( args, excludes );

				pos = replaces [ n ];

				k = 0;
				while ( *( pos + k ) )
				{
					if ( h + 1 == dst_size )
						return 0;

					if ( '\b' == *( pos + k ) )
					{
						if ( filter_on_custom )
						{
							if ( ! filter_on_custom ( src, src_len, ii, & i, dst, dst_size, & h ) )
								return 0;
						}
						else
						{
							j = ii;
							while ( j < i )
							{
								if ( h + 1 == dst_size )
									return 0;

								posx = excludes [ n ];

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

				break;
			}
		}
		if ( ! m )
		{
			if ( h + 1 == dst_size )
				return 0;

			*( dst + h ++ ) = *( src + i ++ );
		}
	}

quit:
	*( dst + h ) = 0;

	return h;
}
