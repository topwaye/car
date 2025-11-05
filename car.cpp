/*
 * car.c
 *
 * Copyright (C) 2025.10.28 TOP WAYE topwaye@hotmail.com
 * 
 * CAR : Computer Aided Read
 */

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "match.h"
#include "traverse.h"
#include "report.h"
#include "directory.h"
#include "debug.h"

/*
 * make sure these directories and files are there
 * a directory must end with '/', distinguished from a file
 */

#define SOURCE_PATH				"c:/apache24/htdocs/"
#define DESTINATION_PATH		"c:/test_bak/"

#define FILE_EXTENSION			".php" /* do NOT include wildcard characters */

#define REPORT_ORG_FILE			"c:/test/err.log"
#define REPORT_TMP_FILE			"c:/test/err2.log"
#define REPORT_OBJ_FILE			"c:/test/err3.log"

#define DEBUG_FILE				"c:/test/debug.php"

#define DEFAULT_BUFFER_SIZE		128

extern char * src_buf;
extern char * dst_buf;

int my_match1 ( )
{
	char target [ ] = "HTM";
	char replace [ ] = "hello world";

	char src [ ] = "x<HTML>x<HTM>xSELECTx";
	char dst [ DEFAULT_BUFFER_SIZE ];
	int len = sizeof ( src ) / sizeof ( src [ 0 ] ) - 1;

	printf ( "%d:%s\n", len, src );
	len = copy_and_replace ( src, len, dst, DEFAULT_BUFFER_SIZE, target, replace );
	printf ( "%d:%s\n", len, dst );

	return 1; /* NOT 0 */
}

int my_match2 ( )
{
	char pattern [ ] = "*H*****L***";
	char replace [ ] = "hello world";
	/*
	char replace [ ] = "\ahello world";
	char replace [ ] = "\bhello world";
	char replace [ ] = "\fhello world";
	char replace [ ] = "\fhello\fworld";
	*/
	char exclude [ ] = "";

	char src [ ] = "x<HTML>x<HTM>xSELECTx";
	char dst [ DEFAULT_BUFFER_SIZE ];
	int len = sizeof ( src ) / sizeof ( src [ 0 ] ) - 1;

	printf ( "%d:%s\n", len, src );
	len = copy_and_replace_ex ( '*', NULL, src, len, dst, DEFAULT_BUFFER_SIZE, pattern, replace, exclude, "placeholder_1", "placeholder_2" );
	printf ( "%d:%s\n", len, dst );

	return 1; /* NOT 0 */
}

int my_match3 ( )
{
	char pattern [ ] = "\v1H*****M";
	char replace [ ] = "hello world";
	char exclude [ ] = "";

	char src [ ] = "x<  HTML>x<HTM>x<		HTM>xSELECTx";
	char dst [ DEFAULT_BUFFER_SIZE ];
	int len = sizeof ( src ) / sizeof ( src [ 0 ] ) - 1;
	
	struct filter_t filter = { 0 }; /* init */
	filter.filter_equal = filter_blank;

	printf ( "%d:%s\n", len, src );
	len = copy_and_replace_ex ( '*', & filter, src, len, dst, DEFAULT_BUFFER_SIZE, pattern, replace, exclude, "placeholder_1", "placeholder_2" );
	printf ( "%d:%s\n", len, dst );

	return 1; /* NOT 0 */
}

int my_match4 ( )
{
	char pattern [ ] = "\v2H*****M";
	char replace [ ] = "hello world";
	char exclude [ ] = "";

	char src [ ] = "x<  HTML>x<HTM>x<		HTM>xSELECTx";
	char dst [ DEFAULT_BUFFER_SIZE ];
	int len = sizeof ( src ) / sizeof ( src [ 0 ] ) - 1;
	
	struct filter_t filter = { 0 }; /* init */
	filter.filter_equal = filter_blank;

	printf ( "%d:%s\n", len, src );
	len = copy_and_replace_ex ( '*', & filter, src, len, dst, DEFAULT_BUFFER_SIZE, pattern, replace, exclude, "placeholder_1", "placeholder_2" );
	printf ( "%d:%s\n", len, dst );

	return 1; /* NOT 0 */
}

int my_match5 ( )
{
	char pattern [ ] = "H*****M";
	char replace [ ] = "hello world";
	char exclude [ ] = "";

	char src [ ] = "x<\'HTM\\\'L\'>x<HTM>x<HTM>xSELECTx";
	char dst [ DEFAULT_BUFFER_SIZE ];
	int len = sizeof ( src ) / sizeof ( src [ 0 ] ) - 1;
	
	struct filter_t filter = { 0 }; /* init */
	filter.filter_initiate = filter_quote;

	printf ( "%d:%s\n", len, src );
	len = copy_and_replace_ex ( '*', & filter, src, len, dst, DEFAULT_BUFFER_SIZE, pattern, replace, exclude, "placeholder_1", "placeholder_2" );
	printf ( "%d:%s\n", len, dst );

	return 1; /* NOT 0 */
}

int my_match6 ( )
{
	char pattern [ ] = "*8*****3***";
	char replace [ ] = "hello world";
	char exclude [ ] = "";

	char src [ ] = "x98765432x83x678x";
	char dst [ DEFAULT_BUFFER_SIZE ];
	int len = sizeof ( src ) / sizeof ( src [ 0 ] ) - 1;

	printf ( "%d:%s\n", len, src );
	len = copy_and_replace_ex2 ( KNOWN_ALPHABET_NUM, '*', NULL, src, len, dst, DEFAULT_BUFFER_SIZE, pattern, replace, exclude, "placeholder_1", "placeholder_2" );
	printf ( "%d:%s\n", len, dst );

	return 1; /* NOT 0 */
}

int my_traverse1 ( )
{
	char pattern [ ] = "/*?*/";
	char replace [ ] = "";
	char exclude [ ] = "";
	char header [ ] = "";
	char footer [ ] = "";

	struct filter_t filter = { 0 }; /* init */
	filter.filter_initiate = filter_quote;
	filter.filter_before_replace = filter_forward2;

	printf ( "listing %s*%s\n", SOURCE_PATH, FILE_EXTENSION);
	printf ( "RDO HID SYS ARC      SIZE FILE %30c COMMAND\n", ' ' );
	printf ( "--- --- --- ---      ---- ---- %30c -------\n", ' ' );
	
	return traverse ( SOURCE_PATH, FILE_EXTENSION, '?', & filter, header, footer, pattern, replace, exclude );
}

int my_traverse2 ( )
{
	char pattern [ ] = "//*\n";
	char replace [ ] = "\n";
	char exclude [ ] = "";
	char header [ ] = "";
	char footer [ ] = "";

	struct filter_t filter = { 0 }; /* init */
	filter.filter_initiate = filter_quote;
	filter.filter_before_replace = filter_forward3;

	printf ( "listing %s*%s\n", SOURCE_PATH, FILE_EXTENSION );
	printf ( "RDO HID SYS ARC      SIZE FILE %30c COMMAND\n", ' ' );
	printf ( "--- --- --- ---      ---- ---- %30c -------\n", ' ' );

	return traverse ( SOURCE_PATH, FILE_EXTENSION, '*', & filter, header, footer, pattern, replace, exclude );
}

int my_traverse3 ( )
{
	char pattern [ ] = "*\n*\n*\n*";
	char replace [ ] = "\n\n";
	char exclude [ ] = "";
	char header [ ] = "";
	char footer [ ] = "";

	struct filter_t filter = { 0 }; /* init */
	filter.filter_after_replace = filter_backward;

	printf ( "listing %s*%s\n", SOURCE_PATH, FILE_EXTENSION );
	printf ( "RDO HID SYS ARC      SIZE FILE %30c COMMAND\n", ' ' );
	printf ( "--- --- --- ---      ---- ---- %30c -------\n", ' ' );

	return traverse2 ( SOURCE_PATH, FILE_EXTENSION, KNOWN_ALPHABET_BLANK, '*', & filter, header, footer, pattern, replace, exclude );
}

int my_traverse4 ( )
{
	char pattern [ ] = "function\v2*(*)*{";
	char replace [ ] = "\aerror_log(\"c:/apache24/htdocs\".$_SERVER['PHP_SELF'].\">\f>\b\\n\", 3, \"c:/test/err.log\");";
	char exclude [ ] = "\r\n\"\'${"; /* what characters a matched @string excludes */
	char header [ ] = "";
	char footer [ ] = "";
	
	struct filter_t filter = { 0 }; /* init */
	filter.filter_equal = filter_blank;
	filter.filter_on_load = filter_forward;

	printf ( "listing %s*%s\n", SOURCE_PATH, FILE_EXTENSION );
	printf ( "RDO HID SYS ARC      SIZE FILE %30c COMMAND\n", ' ' );
	printf ( "--- --- --- ---      ---- ---- %30c -------\n", ' ' );

	return traverse ( SOURCE_PATH, FILE_EXTENSION, '*', & filter, header, footer, pattern, replace, exclude );
}

int my_report1 ( )
{
	printf ( "parsing %s\n", REPORT_ORG_FILE );

	return report_copy_file ( REPORT_ORG_FILE, REPORT_TMP_FILE );
}

int my_report2 ( )
{
	printf ( "parsing %s\n", REPORT_TMP_FILE );

	return nonredundancy_copy_file ( REPORT_TMP_FILE, REPORT_OBJ_FILE );
}

int my_directory ( )
{
	/*
	 * each line is a list entry which ends with '\n' implicitly
	 *
	 * list.txt:
	 *
	 * c:/test/hello.php
	 * c:/test/test2/hello_world.php
	 *
	 */

	printf ( "copying from %s\n", REPORT_OBJ_FILE );

	return copy_listed_files ( REPORT_OBJ_FILE, SOURCE_PATH, DESTINATION_PATH );
}

int my_debug ( )
{
	printf ( "debugging %s\n", DEBUG_FILE );

	return debug ( KNOWN_ALPHABET_DEBUG, DEBUG_FILE );
}

int run ( int operation )
{
	switch ( operation )
	{
		case 1: return my_match1 ( ) && my_match2 ( ) && my_match3 ( )
					   && my_match4 ( ) && my_match5 ( ) && my_match6 ( );
		case 2: return my_traverse1 ( ) && my_traverse2 ( ) && my_traverse3 ( )
					   && my_traverse4 ( );
		case 3: return my_report1 ( ) && my_report2 ( ) && my_directory ( );
		case 4: return my_debug ( );
	}

	return 0;
}

/* if successful, returns 1. otherwise, returns 0 */

int main ( int argc, char * argv [ ] )
{
	char * buffer;
	int operation;
	int c;

	printf ( "CAR : Computer Aided Read\n" );
	printf ( "copyright (C) 2025.10.28 TOP WAYE topwaye@hotmail.com\n" );

	if ( argc == 1 )
	{
		printf ( "[ 1 ] match a pattern string\n" );
		printf ( "[ 2 ] traverse a directory tree and insert a line of source code into all files\n" );
		printf ( "[ 3 ] make a new directory tree on the disk according to a tracing report\n" );
		printf ( "[ 4 ] debug a file to print each known and unknown character\n" );
		printf ( "enter an operation number: " );

		c = _getch ( );
		_putch ( c );
		_putch ( '\n' ); /* line feed */

		switch ( c )
		{
			case '1': operation = 1; break;
			case '2': operation = 2; break;
			case '3': operation = 3; break;
			case '4': operation = 4; break;
			default: operation = 0;
		}
	}
	else if ( argc == 2 )
	{
		if ( compare_string ( argv [ 1 ], "1" ) == 0 )
			operation = 1;
		else if ( compare_string ( argv [ 1 ], "2" ) == 0 )
			operation = 2;
		else if ( compare_string ( argv [ 1 ], "3" ) == 0 )
			operation = 3;
		else if ( compare_string ( argv [ 1 ], "4" ) == 0 )
			operation = 4;
		else
			operation = 0;
	}
	else
	{
		operation = 0;
	}

	if ( ! operation )
	{
		printf ( "invalid operation number\n" );
		return 0;
	}

	buffer = ( char * ) malloc ( MAX_FILE_SIZE + MAX_FILE_SIZE );
	if ( ! buffer )
	{
		printf ( "insufficient memory available\n" );
		return 0;
	}

	src_buf = buffer;
	dst_buf = buffer + MAX_FILE_SIZE;

	if ( ! run ( operation ) )
	{
		printf ( "operation failed\n" );
		
		free ( buffer );
		return 0;
	}

	free ( buffer );

	return 1;
}