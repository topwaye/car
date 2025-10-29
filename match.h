/*
 * match.h
 *
 * Copyright (C) 2025.10.25 TOP WAYE topwaye@hotmail.com
 * 
 * copy and replace pattern words with predefined words
 * a wildcard (*) matches zero or more characters
 * 
 * as a result:
 * sign # represents a runtime current matched string which can NOT be trimmed
 * sign @ represents a runtime current matched string which can be trimmed (e.g. exclude somechar)
 * sign & represents a runtime placeholder which can be arbitrary string predefined
 */

#ifndef MATCH_H
#define MATCH_H

int compare_string ( const char * src, const char * dst );

int seek_string ( char c, char * src, int src_len, int * current );

int concatenate_string ( const char * src, char * dst, int dst_size, ... );

int copy_string ( const char * src, char * dst, int dst_size, ... );

int copy_and_replace_ex ( char wildcard, char * src, int src_len, char * dst, int dst_size,
						  char * pattern, char * replace, char * exclude,
						  ... );

/* prototype do_match_ex function */
int match_ex ( char wildcard, char * pattern, char * src, int src_len, int granularity );

/* prototype do_match function */
int match ( char * target, char * src, int src_len, int granularity );

int copy_and_replace ( char * src, int src_len, char * dst, int dst_size,
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
 *  // char replace [ ] = "#hello world";
 *  // char replace [ ] = "@hello world";
 *  // char replace [ ] = "&hello world";
 *  // char replace [ ] = "&hello&world";
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
 *     len = copy_and_replace_ex ( '*', unknown_chars_1, len, unknown_chars_2, MAX_TEST_SIZE, pattern, replace, exclude, "placeholder_1", "placeholder_2" );
 *     printf ( "%d:%s\n", len, unknown_chars_2 );
 * }
 * 
 */
