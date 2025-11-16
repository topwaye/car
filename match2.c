/*
 * match2.c
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
 * escape character \v represents anything, the behavior of a \v is defined by filters
 *
 * the most fundamental feature of this algorithm is
 * wildcards cannot ensure the order of matching
 * only fixed characters can guarantee the matching order
 */

#include <stdarg.h>

#include "config.h"

extern const char * src_dir;
extern int hit_count;

extern int string_length ( const char * src );
extern int seek_string ( char c, char * src, int src_len, int * current );
extern int do_match_ex ( char wildcard, char * pattern, char * src, int src_len, int * next,
						 filter_terminate_t filter_on_terminate, filter_equal_t filter_on_equal );

int is_known_character ( const char * known, char c )
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

int copy_and_replace_ex2 ( int match_only, const char * known, char wildcard, struct filter_t * filter, char * src, int src_len, char * dst, int dst_size,
						   char * pattern, char * replace, char * exclude,
						   ... )
{
	char * pos, * posx;
	int i, ii, iii, j, h, k;
	int len;
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
		if ( ! is_known_character ( known, *( src + i ) ) )
		{
			if ( match_only )
			{
				i ++;
				continue;
			}

			if ( h + 1 == dst_size )
				return 0;

			*( dst + h ++ ) = *( src + i ++ );

			continue;
		}

		iii = ii = i; /* save i = current index */

		len = ! seek_unknown_character ( known, src, src_len, & iii ) ? src_len : iii; /* NOT iii - ii */

		if ( filter_on_initiate && filter_on_initiate ( src, len, & i ) )
		{
			if ( match_only )
				continue;

			j = ii;
			while ( j < i )
			{
				if ( h + 1 == dst_size )
					return 0;

				*( dst + h ++ ) = *( src + j ++ );
			}

			continue; /* must continue to test i < src_len now */
		}

		if ( ! do_match_ex ( wildcard, pattern, src, len, & i, filter_on_terminate, filter_on_equal ) )
		{
			if ( match_only )
			{
				i ++;
				continue;
			}

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
				if ( filter_on_exclude )
				{
					if ( ! filter_on_exclude ( src, len, ii, & i, dst, dst_size, & h, exclude ) )
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

				if ( filter_on_load && ! filter_on_load ( src, len, ii, & i, dst, dst_size, & h ) )
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

		if ( filter_after_replace && ! filter_after_replace ( src, len, ii, & i, dst, dst_size, & h ) )
			return 0;
	}

	*( dst + h ) = 0;

	return h;
}

/*
 * this is a placeholder function, change anything you want
 * it shows what to do when getting a runtime current matched string
 * do NOT forget a replace[] string must include escape character \b
 * and use parameter &filter (.filter_custom = filter_custom)
 * to call copy_and_replace like functions
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

int filter_escape ( char * src, int src_len, int src_prior, int * src_index, char * dst, int dst_size, int * dst_index, char * exclude )
{
	int i, h, j, f, g;
	int a;
	int threshold;
	int skip;

	i = * src_index;
	h = * dst_index;

	j = src_prior, f = g = -1; threshold = 0; skip = string_length ( "function" );

	while ( skip -- )
		j ++;

	while ( j < i )
	{
		if ( h + 1 == dst_size )
			return 0;

		if ( is_known_character ( exclude, *( src + j ) ) )
		{
			j ++;
			continue;
		}

		/* remove the characters in parentheses */
		if ( '\'' == *( src + j ) )
		{
			a = 0;
			while ( ++ j < i ) 
			{
				if ( a )
				{
					a = 0;
					continue;
				}

				if ( '\'' == *( src + j ) )
				{
					j ++; /* must be here, do NOT move this line */
					break;
				}

				if ( '\\' == *( src + j ) )
					a = 1;
			}

			continue;
		}
		else if ( '\"' == *( src + j ) )
		{
			a = 0;
			while ( ++ j < i ) 
			{
				if ( a )
				{
					a = 0;
					continue;
				}

				if ( '\"' == *( src + j ) )
				{
					j ++; /* must be here, do NOT move this line */
					break;
				}

				if ( '\\' == *( src + j ) )
					a = 1;
			}

			continue;
		}
		else if( '(' == *( src + j ) )
		{
			if ( threshold ++ > 0 )
			{
				j ++;
				continue;
			}
		}
		else if( ')' == *( src + j ) )
		{
			if ( -- threshold > 0 )
			{
				j ++;
				continue;
			}
		}
		else
		{
			if ( threshold > 0 ) /* NOT 1 */
			{
				j ++;
				continue;
			}
		}

		/* remove leading and trailing spaces */
		if ( f == -1 ) /* must be here, do NOT move this line */
			f = j;

		if ( is_known_character ( KNOWN_ALPHABET_BLANK, *( src + j ) ) )
		{
			if ( g == -1 )
				g = j;

			j ++;
			continue;
		}
		if ( g == f )
		{
			g = -1;
		}
		else if ( g > f )
		{
			*( dst + h ++ ) = *( src + g );
			g = -1;

			if ( h + 1 == dst_size ) /* must be here, do NOT move this line */
				return 0;
		}
		else
		{
			/* -1: nothing to do */
		}

		/* escape characters */
		if ( '$' == *( src + j ) || '\'' == *( src + j ) || '\"' == *( src + j ) )
		{
			*( dst + h ++ ) = '\\';

			if ( h + 1 == dst_size ) /* must be here, do NOT move this line */
				return 0;	
		}

		*( dst + h ++ ) = *( src + j ++ );
	}

	* src_index = i;
	* dst_index = h;

	return 1;
}

int filter_escape2 ( char * src, int src_len, int src_prior, int * src_index, char * dst, int dst_size, int * dst_index, char * exclude )
{
	int i, h, j;
	int skip;

	i = * src_index;
	h = * dst_index;

	j = src_prior; skip = string_length ( src_dir );

	while ( skip -- ) /* a blank + c:/test/ => c:/test */
		j ++;

	while ( j < i )
	{
		if ( h + 1 == dst_size )
			return 0;

		if ( is_known_character ( exclude, *( src + j ) ) )
		{
			j ++;
			continue;
		}

		/* escape characters */
		if ( '\\' == *( src + j ) )
		{
			*( dst + h ++ ) = '/';
			j ++; /* patch */
			continue;
		}

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

	i = r; /* i moves forward */
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

int filter_quote ( char * src, int src_len, int * src_index )
{
	int i;
	int a;

	i = * src_index;

	/* 
	 * do_match_ex ( ) has already checked current 1 char *( src + i ),
	 * do NOT check if ( i == src_len ) return 0 again here
	 */
	if ( '\'' == *( src + i ) )
	{
		a = 0;
		while ( ++ i < src_len ) 
		{
			if ( a )
			{
				a = 0;
				continue;
			}

			if ( '\'' == *( src + i ) )
			{
				i ++; /* ++i must be here */
				break;	
			}

			if ( '\\' == *( src + i ) )
				a = 1;
		}
	}
	else if ( '\"' == *( src + i ) )
	{
		a = 0;
		while ( ++ i < src_len ) 
		{
			if ( a )
			{
				a = 0;
				continue;
			}

			if ( '\"' == *( src + i ) )
			{
				i ++; /* ++i must be here */
				break;	
			}

			if ( '\\' == *( src + i ) )
				a = 1;
		}
	}
	else
	{
		return 0;
	}

	* src_index = i; /* i == src_len, normally or abnormally, that is what we expect */

	return 1;
}

int filter_blank ( char * pattern, int * pattern_index, char * src, int src_len, int * src_index )
{
	char * pos;
	int i, j, k;

	i = * src_index;

	pos = pattern;
	k = * pattern_index;

	k ++;

	if ( *( pos + k ) )
	{
		if ( *( pos + k ) == '1' ) /* \v1: call number == 1 */
		{
			/* 
			 * do_match_ex ( ) has already checked current 1 char *( src + i ),
			 * do NOT check if ( i == src_len ) return 0 again here
			 */
			if ( is_known_character ( KNOWN_ALPHABET_BLANK, *( src + i ) ) )
			{
				* src_index = ++ i; /* ++i must be here */
				* pattern_index = k;

				return 1;
			}
		}
		else if ( *( pos + k ) == '2' ) /* \v2: call number == 2 */
		{
			j = i;
						
			/* 
			 * do_match_ex ( ) has already checked current 1 char *( src + i ),
			 * do NOT check if ( i == src_len ) return 0 again here
			 * meaning the 1st round comparing is safe to go
			 */
			while ( is_known_character ( KNOWN_ALPHABET_BLANK, *( src + i ) ) )
				if ( ++ i == src_len ) /* must be here, do NOT move this line */
					break; /* NOT return 0 */

			if ( j < i )
			{
				* src_index = i; /* NOT ++i */
				* pattern_index = k;

				return 1;
			}
		}
		else
		{
			/* illegal call number */
		}
	}

	return 0;
}

int filter_alphabet ( char * pattern, int * pattern_index, char * src, int src_len, int * src_index )
{
	char * pos;
	int i, j, k;

	i = * src_index;

	pos = pattern;
	k = * pattern_index;

	k ++;

	if ( *( pos + k ) )
	{
		if ( *( pos + k ) == '1' ) /* \v1: call number == 1 */
		{
			/* 
			 * do_match_ex ( ) has already checked current 1 char *( src + i ),
			 * do NOT check if ( i == src_len ) return 0 again here
			 */
			if ( is_known_character ( KNOWN_ALPHABET_BLANK, *( src + i ) ) )
			{
				* src_index = ++ i; /* ++i must be here */
				* pattern_index = k;

				return 1;
			}
		}
		else if ( *( pos + k ) == '2' ) /* \v2: call number == 2 */
		{
			j = i;
						
			/* 
			 * do_match_ex ( ) has already checked current 1 char *( src + i ),
			 * do NOT check if ( i == src_len ) return 0 again here
			 * meaning the 1st round comparing is safe to go
			 */
			while ( is_known_character ( KNOWN_ALPHABET_BLANK, *( src + i ) ) )
				if ( ++ i == src_len ) /* must be here, do NOT move this line */
					break; /* NOT return 0 */

			if ( j < i )
			{
				* src_index = i; /* NOT ++i */
				* pattern_index = k;

				return 1;
			}
		}
		else if ( *( pos + k ) == '7' ) /* \v7: call number == 7 */
		{
			/* 
			 * do_match_ex ( ) has already checked current 1 char *( src + i ),
			 * do NOT check if ( i == src_len ) return 0 again here
			 */
			if ( is_known_character ( KNOWN_ALPHABET_NAME, *( src + i ) ) )
			{
				* src_index = ++ i; /* ++i must be here */
				* pattern_index = k;

				return 1;
			}
		}
		else if ( *( pos + k ) == '8' ) /* \v8: call number == 8 */
		{
			j = i;
						
			/* 
			 * do_match_ex ( ) has already checked current 1 char *( src + i ),
			 * do NOT check if ( i == src_len ) return 0 again here
			 * meaning the 1st round comparing is safe to go
			 */
			while ( is_known_character ( KNOWN_ALPHABET_NAME, *( src + i ) ) )
				if ( ++ i == src_len ) /* must be here, do NOT move this line */
					break; /* NOT return 0 */

			if ( j < i )
			{
				* src_index = i; /* NOT ++i */
				* pattern_index = k;

				return 1;
			}
		}
		else
		{
			/* illegal call number */
		}
	}

	return 0;
}

int filter_alphabet2 ( char * pattern, int * pattern_index, char * src, int src_len, int * src_index )
{
	char * pos;
	int i, j, k;

	i = * src_index;

	pos = pattern;
	k = * pattern_index;

	k ++;

	if ( *( pos + k ) )
	{
		if ( *( pos + k ) == '1' ) /* \v1: call number == 1 */
		{
			/* 
			 * do_match_ex ( ) has already checked current 1 char *( src + i ),
			 * do NOT check if ( i == src_len ) return 0 again here
			 */
			if ( is_known_character ( KNOWN_ALPHABET_BLANK, *( src + i ) ) )
			{
				* src_index = ++ i; /* ++i must be here */
				* pattern_index = k;

				return 1;
			}
		}
		else if ( *( pos + k ) == '2' ) /* \v2: call number == 2 */
		{
			j = i;
						
			/* 
			 * do_match_ex ( ) has already checked current 1 char *( src + i ),
			 * do NOT check if ( i == src_len ) return 0 again here
			 * meaning the 1st round comparing is safe to go
			 */
			while ( is_known_character ( KNOWN_ALPHABET_BLANK, *( src + i ) ) )
				if ( ++ i == src_len ) /* must be here, do NOT move this line */
					break; /* NOT return 0 */

			if ( j < i )
			{
				* src_index = i; /* NOT ++i */
				* pattern_index = k;

				return 1;
			}
		}
		else if ( *( pos + k ) == '3' ) /* \v3: call number == 3 */
		{
			/* 
			 * do_match_ex ( ) has already checked current 1 char *( src + i ),
			 * do NOT check if ( i == src_len ) return 0 again here
			 */
			if ( is_known_character ( KNOWN_ALPHABET_NUM, *( src + i ) ) )
			{
				* src_index = ++ i; /* ++i must be here */
				* pattern_index = k;

				return 1;
			}
		}
		else if ( *( pos + k ) == '4' ) /* \v4: call number == 4 */
		{
			j = i;
						
			/* 
			 * do_match_ex ( ) has already checked current 1 char *( src + i ),
			 * do NOT check if ( i == src_len ) return 0 again here
			 * meaning the 1st round comparing is safe to go
			 */
			while ( is_known_character ( KNOWN_ALPHABET_NUM, *( src + i ) ) )
				if ( ++ i == src_len ) /* must be here, do NOT move this line */
					break; /* NOT return 0 */

			if ( j < i )
			{
				* src_index = i; /* NOT ++i */
				* pattern_index = k;

				return 1;
			}
		}
		else if ( *( pos + k ) == '5' ) /* \v5: call number == 5 */
		{
			/* 
			 * do_match_ex ( ) has already checked current 1 char *( src + i ),
			 * do NOT check if ( i == src_len ) return 0 again here
			 */
			if ( is_known_character ( KNOWN_ALPHABET_DIR, *( src + i ) ) )
			{
				* src_index = ++ i; /* ++i must be here */
				* pattern_index = k;

				return 1;
			}
		}
		else if ( *( pos + k ) == '6' ) /* \v6: call number == 6 */
		{
			j = i;
						
			/* 
			 * do_match_ex ( ) has already checked current 1 char *( src + i ),
			 * do NOT check if ( i == src_len ) return 0 again here
			 * meaning the 1st round comparing is safe to go
			 */
			while ( is_known_character ( KNOWN_ALPHABET_DIR, *( src + i ) ) )
				if ( ++ i == src_len ) /* must be here, do NOT move this line */
					break; /* NOT return 0 */

			if ( j < i )
			{
				* src_index = i; /* NOT ++i */
				* pattern_index = k;

				return 1;
			}
		}
		else if ( *( pos + k ) == '9' ) /* \v9: call number == 9 */
		{
			/* 
			 * do_match_ex ( ) has already checked current 1 char *( src + i ),
			 * do NOT check if ( i == src_len ) return 0 again here
			 */
			if ( is_known_character ( KNOWN_ALPHABET_DRV, *( src + i ) ) )
			{
				* src_index = ++ i; /* ++i must be here */
				* pattern_index = k;

				return 1;
			}
		}
		else
		{
			/* illegal call number */
		}
	}

	return 0;
}

int match_ex2 ( char * known, char wildcard, char * pattern, char * src, int src_len, int granularity )
{
	int i, ii, iii; 
	int len;

	i = 0;
	while ( i < src_len )
	{
		if ( ! is_known_character ( known, *( src + i ) ) )
		{
			i ++;
			continue;
		}

		iii = ii = i; /* save i = current index */

		len = ! seek_unknown_character ( known, src, src_len, & iii ) ? src_len : iii; /* NOT iii - ii */

		if ( ! do_match_ex ( wildcard, pattern, src, len, & i, NULL, NULL ) )
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
