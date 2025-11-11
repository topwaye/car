/*
 * match.h
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

#ifndef MATCH_H
#define MATCH_H

extern int string_length ( const char * src );
extern int substring ( int src_index, int src_delta, char ** src );
extern int trim_string_tail ( int src_delta, char * src );

extern int compare_string ( const char * src, const char * dst );
extern int compare_string_ex ( int src_index, int src_delta, const char * src, const char * dst );

extern int seek_string ( char c, char * src, int src_len, int * current );

extern int concatenate_string ( const char * src, char * dst, int dst_size, ... );
extern int copy_string ( const char * src, char * dst, int dst_size, ... );

extern int filter_quote ( char * src, int src_len, int * src_index );
extern int filter_blank ( char * pattern, int * pattern_index, char * src, int src_len, int * src_index );
extern int filter_alphabet ( char * pattern, int * pattern_index, char * src, int src_len, int * src_index );
extern int filter_escape ( char * src, int src_len, int src_prior, int * src_index, char * dst, int dst_size, int * dst_index, char * exclude );

extern int filter_custom ( char * src, int src_len, int src_prior, int * src_index, char * dst, int dst_size, int * dst_index );
extern int filter_forward3 ( char * src, int src_len, int src_prior, int * src_index, char * dst, int dst_size, int * dst_index );
extern int filter_forward2 ( char * src, int src_len, int src_prior, int * src_index, char * dst, int dst_size, int * dst_index );
extern int filter_forward ( char * src, int src_len, int src_prior, int * src_index, char * dst, int dst_size, int * dst_index );
extern int filter_backward ( char * src, int src_len, int src_prior, int * src_index, char * dst, int dst_size, int * dst_index );

/* wildcards are restricted to matching only predefined known characters */
extern int knowledge_based_copy_and_replace_ex ( int argc, const char * knowledge [ ], char wildcard, struct filter_t * filter,
												 char * src, int src_len, char * dst, int dst_size,
												 char * pattern, char * replace, char * exclude,
												 ... );

extern int multiple_copy_and_replace_ex ( int argc, char wildcards [ ], struct filter_t * filters [ ], char * src, int src_len, char * dst, int dst_size,
										  char * patterns [ ], char * replaces [ ], char * excludes [ ],
										  ... );

extern int copy_and_replace_ex2 ( const char * known, char wildcard, struct filter_t * filter, char * src, int src_len, char * dst, int dst_size,
								  char * pattern, char * replace, char * exclude,
								  ... );

extern int copy_and_replace_ex ( char wildcard, struct filter_t * filter, char * src, int src_len, char * dst, int dst_size,
								 char * pattern, char * replace, char * exclude,
								 ... );

/* prototype knowledge_based_do_match_ex function */
extern int knowledge_based_match_ex ( int argc, const char * knowledge [ ], char wildcard, char * pattern, char * src, int src_len, int granularity );

extern int match_ex2 ( char * known, char wildcard, char * pattern, char * src, int src_len, int granularity );

/* prototype do_match_ex function */
extern int match_ex ( char wildcard, char * pattern, char * src, int src_len, int granularity );

/* prototype do_match function */
extern int match ( char * target, char * src, int src_len, int granularity );

extern int copy_and_replace ( char * src, int src_len, char * dst, int dst_size,
							  char * target, char * replace );

#endif

/*
 * #define MAX_TEST_SIZE	128
 *
 * void test ( )
 * {
 *     char target [ ] = "HTM";
 *
 *     char pattern [ ] = "*H*****L***";
 *     char replace [ ] = "hello world";
 *
 *  // char replace [ ] = "\ahello world";
 *  // char replace [ ] = "\bhello world";
 *  // char replace [ ] = "\fhello world";
 *  // char replace [ ] = "\fhello\fworld";
 *
 *     char exclude [ ] = "\r\n"; // what characters a matched @string excludes
 *
 *     char unknown_chars_1 [ ] = "x<HTML>x<HTM>xSELECTx";
 *     char unknown_chars_2 [ MAX_TEST_SIZE ];
 *     int len;
 *
 *     len = sizeof ( unknown_chars_1 ) / sizeof ( unknown_chars_1 [ 0 ] ) - 1;
 *
 *     printf ( "%d:%s\n", len, unknown_chars_1 );
 *     len = copy_and_replace ( unknown_chars_1, len, unknown_chars_2, MAX_TEST_SIZE, target, replace );
 *     printf ( "%d:%s\n", len, unknown_chars_2 );
 *
 *     len = sizeof ( unknown_chars_1 ) / sizeof ( unknown_chars_1 [ 0 ] ) - 1;
 *
 *     printf ( "%d:%s\n", len, unknown_chars_1 );
 *     len = copy_and_replace_ex ( '*', unknown_chars_1, len, unknown_chars_2, MAX_TEST_SIZE, NULL, pattern, replace, exclude, "placeholder_1", "placeholder_2" );
 *     printf ( "%d:%s\n", len, unknown_chars_2 );
 * }
 * 
 */
