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

#define MAX_TEST_SIZE	128

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
	char unknown_chars_2 [ MAX_TEST_SIZE ];
	int len;

	len = sizeof ( unknown_chars_1 ) / sizeof ( unknown_chars_1 [ 0 ] ) - 1;

	printf ( "%d:%s\n", len, unknown_chars_1 );
	len = copy_and_replace ( unknown_chars_1, len, unknown_chars_2, MAX_TEST_SIZE, target, replace );
	printf ( "%d:%s\n", len, unknown_chars_2 );

	len = sizeof ( unknown_chars_1 ) / sizeof ( unknown_chars_1 [ 0 ] ) - 1;

	printf ( "%d:%s\n", len, unknown_chars_1 );
	len = copy_and_replace_ex ( '*', unknown_chars_1, len, unknown_chars_2, MAX_TEST_SIZE, pattern, replace, exclude, "placeholder_1", "placeholder_2" );
	printf ( "%d:%s\n", len, unknown_chars_2 );

	return 0;
}

int test_traversing_phase_1 ( )
{
	/* make sure this directory is there */

	/* a directory must end with '/', distinguished from a file */
	char path [ _MAX_PATH ] = "c:/test2/";

	char pattern [ ] = "/*?*/";
	char replace [ ] = "";
	char exclude [ ] = "\r\n"; /* what characters a matched @string excludes */
	char header [ ] = "";
	char footer [ ] = "";

	/* list the files... */
	printf ( "listing of files in the directory %s\n\n", path );
	printf ( "RDO HID SYS ARC      SIZE FILE %31c COMMAND\n", ' ' );
	printf ( "--- --- --- ---      ---- ---- %31c -------\n", ' ' );

	traverse ( path, "*.php", '?', header, footer, pattern, replace, exclude );

	return 0;
}

int test_traversing_phase_2 ( )
{
	/* make sure this directory is there */

	/* a directory must end with '/', distinguished from a file */
	char path [ _MAX_PATH ] = "c:/test2/";

	char pattern [ ] = "//*\n";
	char replace [ ] = "\n";
	char exclude [ ] = "\r\n"; /* what characters a matched @string excludes */
	char header [ ] = "";
	char footer [ ] = "";

	/* list the files... */
	printf ( "listing of files in the directory %s\n\n", path );
	printf ( "RDO HID SYS ARC      SIZE FILE %31c COMMAND\n", ' ' );
	printf ( "--- --- --- ---      ---- ---- %31c -------\n", ' ' );

	traverse ( path, "*.php", '*', header, footer, pattern, replace, exclude );

	return 0;
}

int test_traversing_phase_3 ( )
{
	/* make sure this directory is there */

	/* a directory must end with '/', distinguished from a file */
	char path [ _MAX_PATH ] = "c:/test2/";

	char pattern [ ] = "function *(*)*{";
	char replace [ ] = "#\n\t\terror_log(\"c:/apache/htdocs\".$_SERVER['PHP_SELF'].\">&>@\\n\", 3, \"c:/test/err.log\");";
	char exclude [ ] = "\r\n${"; /* what characters a matched @string excludes */
	char header [ ] = "<?php error_log(\"header<&>\\n\", 3, \"c:/test/err.log\"); ?>\n";
	char footer [ ] = "\n<?php error_log(\"footer<&>\\n\", 3, \"c:/test/err.log\"); ?>";

	/* list the files... */
	printf ( "listing of files in the directory %s\n\n", path );
	printf ( "RDO HID SYS ARC      SIZE FILE %31c COMMAND\n", ' ' );
	printf ( "--- --- --- ---      ---- ---- %31c -------\n", ' ' );

	traverse ( path, "*.php", '*', header, footer, pattern, replace, exclude );

	return 0;
}

int test_report_phase_1 ( )
{
	/* make sure these directories and files are there */

	char filename_1 [ _MAX_PATH ] = "c:/test/err.log";
	char filename_2 [ _MAX_PATH ] = "c:/test/err2.log";

	report_copy_file ( filename_1, filename_2 );

	return 0;
}

int test_report_phase_2 ( )
{
	/* make sure these directories and files are there */

	char filename_1 [ _MAX_PATH ] = "c:/test/err2.log";
	char filename_2 [ _MAX_PATH ] = "c:/test/err3.log";

	nonredundancy_copy_file ( filename_1, filename_2 );

	return 0;
}

int test_directory ( )
{
	/* make sure these directories and files are there */

	/*
	 * each line is a list entry which ends with '\n' implicitly
	 *
	 * list.txt:
	 *
	 * c:/test/hello.php
	 * c:/test/test2/hello_world.php
	 *
	 */
	char file_list_name [ _MAX_PATH ] = "c:/test/err3.log";

	/* a directory must end with '/', distinguished from a file */
	char src_path [ _MAX_PATH ] = "c:/test/";
	char dst_path [ _MAX_PATH ] = "c:/test_bak/";

	int list_size;

	list_size = load_file ( file_list_name );
	if ( ! list_size )
	{
		printf ( "load_file failed\n" );
		return 1;
	}

	copy_listed_files ( src_path, dst_path, src_buf, list_size );

	return 0;
}

void do_test ( )
{
	test_match ( );
	test_traversing_phase_1();
	test_traversing_phase_2 ( );
	test_traversing_phase_3 ( );
	test_report_phase_1 ( );
	test_report_phase_2 ( );
	test_directory ( );
}

int main ( )
{
	char * buffer;

	buffer = ( char * ) malloc ( MAX_FILE_SIZE + MAX_FILE_SIZE );
	if ( ! buffer )
	{
		printf ( "insufficient memory available\n" );
		return 1;
	}

	src_buf = buffer;
	dst_buf = buffer + MAX_FILE_SIZE;

	do_test ( );

	free ( buffer );

	return 0;
}