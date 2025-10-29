/*
 * car.c
 *
 * Copyright (C) 2025.10.28 TOP WAYE topwaye@hotmail.com
 * 
 * CAR : Computer Aided Read
 */

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "match.h"
#include "traverse.h"
#include "report.h"
#include "directory.h"

/*
 * make sure these directories and files are there
 * a directory must end with '/', distinguished from a file
 */

#define SOURCE_PATH				"c:/test/"
#define DESTINATION_PATH		"c:/test_bak/"

#define FILE_EXTENSION			".php" /* do NOT include wildcard characters */

#define REPORT_ORG_FILE			"c:/test/err.log"
#define REPORT_TMP_FILE			"c:/test/err2.log"
#define REPORT_OBJ_FILE			"c:/test/err3.log"

#define DEFAULT_BUFFER_SIZE		128

extern char * src_buf;
extern char * dst_buf;

int test_match ( )
{
	char target [ ] = "HTM";

	char pattern [ ] = "*H*****L***";
	char replace [ ] = "hello world";
	/*
	char replace [ ] = "#hello world";
	char replace [ ] = "@hello world";
	char replace [ ] = "&hello world";
	char replace [ ] = "&hello&world";
	*/
	char exclude [ ] = "\r\n"; /* what characters a matched @string excludes */

	char unknown_chars_1 [ ] = "x<HTML>x<HTM>xSELECTx";
	char unknown_chars_2 [ DEFAULT_BUFFER_SIZE ];
	int len;

	len = sizeof ( unknown_chars_1 ) / sizeof ( unknown_chars_1 [ 0 ] ) - 1;

	printf ( "%d:%s\n", len, unknown_chars_1 );
	len = copy_and_replace ( unknown_chars_1, len, unknown_chars_2, DEFAULT_BUFFER_SIZE, target, replace );
	printf ( "%d:%s\n", len, unknown_chars_2 );

	len = sizeof ( unknown_chars_1 ) / sizeof ( unknown_chars_1 [ 0 ] ) - 1;

	printf ( "%d:%s\n", len, unknown_chars_1 );
	len = copy_and_replace_ex ( '*', unknown_chars_1, len, unknown_chars_2, DEFAULT_BUFFER_SIZE, pattern, replace, exclude, "placeholder_1", "placeholder_2" );
	printf ( "%d:%s\n", len, unknown_chars_2 );

	return 1; /* NOT 0 */
}

int test_traversing_phase_1 ( )
{
	char pattern [ ] = "/*?*/";
	char replace [ ] = "";
	char exclude [ ] = "\r\n"; /* what characters a matched @string excludes */
	char header [ ] = "";
	char footer [ ] = "";

	printf ( "listing %s*%s\n", SOURCE_PATH, FILE_EXTENSION);
	printf ( "RDO HID SYS ARC      SIZE FILE %31c COMMAND\n", ' ' );
	printf ( "--- --- --- ---      ---- ---- %31c -------\n", ' ' );
	
	return traverse ( SOURCE_PATH, FILE_EXTENSION, '?', header, footer, pattern, replace, exclude );
}

int test_traversing_phase_2 ( )
{
	char pattern [ ] = "//*\n";
	char replace [ ] = "\n";
	char exclude [ ] = "\r\n"; /* what characters a matched @string excludes */
	char header [ ] = "";
	char footer [ ] = "";

	printf ( "listing %s*%s\n", SOURCE_PATH, FILE_EXTENSION );
	printf ( "RDO HID SYS ARC      SIZE FILE %31c COMMAND\n", ' ' );
	printf ( "--- --- --- ---      ---- ---- %31c -------\n", ' ' );

	return traverse ( SOURCE_PATH, FILE_EXTENSION, '*', header, footer, pattern, replace, exclude );
}

int test_traversing_phase_3 ( )
{
	char pattern [ ] = "function *(*)*{";
	char replace [ ] = "#\n\t\terror_log(\"c:/apache/htdocs\".$_SERVER['PHP_SELF'].\">&>@\\n\", 3, \"c:/test/err.log\");";
	char exclude [ ] = "\r\n${"; /* what characters a matched @string excludes */
	char header [ ] = "";
	char footer [ ] = "";

	printf ( "listing %s*%s\n", SOURCE_PATH, FILE_EXTENSION );
	printf ( "RDO HID SYS ARC      SIZE FILE %31c COMMAND\n", ' ' );
	printf ( "--- --- --- ---      ---- ---- %31c -------\n", ' ' );

	return traverse ( SOURCE_PATH, FILE_EXTENSION, '*', header, footer, pattern, replace, exclude );
}

int test_report_phase_1 ( )
{
	printf ( "parsing %s\n", REPORT_ORG_FILE );

	return report_copy_file ( REPORT_ORG_FILE, REPORT_TMP_FILE );
}

int test_report_phase_2 ( )
{
	printf ( "parsing %s\n", REPORT_TMP_FILE );

	return nonredundancy_copy_file ( REPORT_TMP_FILE, REPORT_OBJ_FILE );
}

int test_directory ( )
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

int do_test ( )
{
	return test_match ( )
		&& test_traversing_phase_1 ( )
		&& test_traversing_phase_2 ( )
		&& test_traversing_phase_3 ( )
		&& test_report_phase_1 ( )
		&& test_report_phase_2 ( )
		&& test_directory ( );
}

/* if successful, returns 1. otherwise, returns 0 */

int main ( )
{
	char * buffer;

	buffer = ( char * ) malloc ( MAX_FILE_SIZE + MAX_FILE_SIZE );
	if ( ! buffer )
	{
		printf ( "insufficient memory available\n" );
		return 0;
	}

	src_buf = buffer;
	dst_buf = buffer + MAX_FILE_SIZE;

	if ( ! do_test ( ) )
	{
		printf ( "do_test failed\n" );
		
		free ( buffer );
		return 0;
	}

	free ( buffer );

	return 1;
}