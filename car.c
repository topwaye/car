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

#define DEFAULT_INIT_FILE				"c:/car/car.ini"

#define DEFAULT_BUFFER_SIZE				128
#define PARAM_ENTRY_WIDTH				2

extern const char * src_dir;
extern const char * dst_dir;
extern char * src_buf;
extern char * dst_buf;
extern int load_file ( const char * filename );

/*
 * make sure these directories and files are there
 * a directory must end with '/', distinguished from a file
 */
char param_list [ ] [ PARAM_ENTRY_WIDTH ] [ _MAX_PATH ] =
{
	{ "src",		"c:/apache24/htdocs/" },
	{ "dst",		"c:/car-workspace/"   },
	{ "ext",		".php"                }, /* do NOT include wildcard characters */
	{ "log",		"c:/car/e.log"        },
	{ "tmp",		"c:/car/e2.log"       },
	{ "obj",		"c:/car/e3.log"       },
	{ "inc",		"c:/car/e4.log"       },
	{ "xt_log",		"c:/car/xdebug.xt"    },
	{ "xt_tmp",		"c:/car/xdebug2.xt"   },
	{ "xt_obj",		"c:/car/xdebug3.xt"   },
	{ "xt_inc",		"c:/car/xdebug4.xt"   },
	{ "xt_time_tmp",	"c:/car/xdebug5.xt"   },
	{ "xt_time_obj",	"c:/car/xdebug6.xt"   },
	{ "xt_time_ndx",	"c:/car/xdebug7.xt"   },
	{ "xt_mem_tmp",		"c:/car/xdebug8.xt"   },
	{ "xt_mem_obj",		"c:/car/xdebug9.xt"   },
	{ "xt_mem_ndx",		"c:/car/xdebug10.xt"   },
	{ "dbg",		"c:/car/debug.php"    }
};

int param_list_len = sizeof ( param_list ) / sizeof ( param_list [ 0 ] );

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
	len = copy_and_replace_ex ( 0, '*', NULL, src, len, dst, DEFAULT_BUFFER_SIZE, pattern, replace, exclude, "placeholder_1", "placeholder_2" );
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
	filter.filter_on_equal = filter_blank;

	printf ( "%d:%s\n", len, src );
	len = copy_and_replace_ex ( 0, '*', & filter, src, len, dst, DEFAULT_BUFFER_SIZE, pattern, replace, exclude, "placeholder_1", "placeholder_2" );
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
	filter.filter_on_equal = filter_blank;

	printf ( "%d:%s\n", len, src );
	len = copy_and_replace_ex ( 0, '*', & filter, src, len, dst, DEFAULT_BUFFER_SIZE, pattern, replace, exclude, "placeholder_1", "placeholder_2" );
	printf ( "%d:%s\n", len, dst );

	return 1; /* NOT 0 */
}

int my_match5 ( )
{
	char pattern [ ] = "H*****M";
	char replace [ ] = "hello world";
	char exclude [ ] = "";
	/*
	 * another example in test.php file:
	 * x<H'T\'4ML\'44\'HTML'>x<'ML'>x<HTM>xSELECTx
	 */
	char src [ ] = "x<H\'T\\\'\'>x<\'ML\'>x<HTM>xSELECTx";
	char dst [ DEFAULT_BUFFER_SIZE ];
	int len = sizeof ( src ) / sizeof ( src [ 0 ] ) - 1;
	
	struct filter_t filter = { 0 }; /* init */
	/* filter_relay_initiate */
	filter.filter_on_initiate = filter_quote;
	filter.filter_on_terminate = filter_quote;

	printf ( "%d:%s\n", len, src );
	len = copy_and_replace_ex ( 0, '*', & filter, src, len, dst, DEFAULT_BUFFER_SIZE, pattern, replace, exclude, "placeholder_1", "placeholder_2" );
	printf ( "%d:%s\n", len, dst );

	return 1; /* NOT 0 */
}

int my_match6 ( )
{
	char pattern [ ] = "H*****M";
	char replace [ ] = "hello world";
	char exclude [ ] = "";
	/*
	 * another example in test.php file:
	 * x<H'T\'4ML\'44\'HTML'>x<'ML'>x<HTM>xSELECTx
	 */
	char src [ ] = "x<H\'T\\\'\'>x<\'ML\'>x<HTM>xSELECTx";
	char dst [ DEFAULT_BUFFER_SIZE ];
	int len = sizeof ( src ) / sizeof ( src [ 0 ] ) - 1;

	struct filter_t filter = { 0 }; /* init */
	/* filter_no_relay_initiate */
	filter.filter_on_initiate = filter_quote;

	printf ( "%d:%s\n", len, src );
	len = copy_and_replace_ex ( 0, '*', & filter, src, len, dst, DEFAULT_BUFFER_SIZE, pattern, replace, exclude, "placeholder_1", "placeholder_2" );
	printf ( "%d:%s\n", len, dst );

	return 1; /* NOT 0 */
}

int my_match7 ( )
{
	char pattern [ ] = "*8*****3***";
	char replace [ ] = "hello world";
	char exclude [ ] = "";

	const char * knowledge [ ] =
	{
		KNOWN_ALPHABET_NUM,	/* wildcard segment 2 */
		KNOWN_ALPHABET_NUM,	/* wildcard segment 1 */
		KNOWN_ALPHABET_NUM	/* wildcard segment 0 */
	};

	char src [ ] = "x987654x83x678x";
	char dst [ DEFAULT_BUFFER_SIZE ];
	int len = sizeof ( src ) / sizeof ( src [ 0 ] ) - 1;

	printf ( "%d:%s\n", len, src );
	len = knowledge_based_copy_and_replace_ex ( 0, sizeof ( knowledge ) / sizeof ( knowledge [ 0 ] ), knowledge, '*', NULL, src, len, dst, DEFAULT_BUFFER_SIZE, pattern, replace, exclude, "placeholder_1", "placeholder_2" );
	printf ( "%d:%s\n", len, dst );

	return 1; /* NOT 0 */
}

int my_match8 ( )
{
	char pattern [ ] = "*8*****3***";
	char replace [ ] = "hello world";
	char exclude [ ] = "";

	char src [ ] = "x98765432x83x678x";
	char dst [ DEFAULT_BUFFER_SIZE ];
	int len = sizeof ( src ) / sizeof ( src [ 0 ] ) - 1;

	printf ( "%d:%s\n", len, src );
	len = copy_and_replace_ex2 ( 0, KNOWN_ALPHABET_NUM, '*', NULL, src, len, dst, DEFAULT_BUFFER_SIZE, pattern, replace, exclude, "placeholder_1", "placeholder_2" );
	printf ( "%d:%s\n", len, dst );

	return 1; /* NOT 0 */
}

int my_traverse1 ( const char * directory, const char * extension )
{
	char header [ ] = "";
	char footer [ ] = "";

	/* erase format 1 comment lines */

	char pattern1 [ ] = "//*\n";
	char replace1 [ ] = "\n";
	char exclude1 [ ] = "";

	struct filter_t filter1 = { 0 }; /* init */
	/* filter1_no_relay_initiate */
	filter1.filter_on_initiate = filter_quote;
	filter1.filter_before_replace = filter_forward3;

	/* erase format 2 comment lines */

	char pattern2 [ ] = "/*?*/";
	char replace2 [ ] = "";
	char exclude2 [ ] = "";

	struct filter_t filter2 = { 0 }; /* init */
	/* filter2_no_relay_initiate */
	filter2.filter_on_initiate = filter_quote;
	filter2.filter_before_replace = filter_forward2;

	/* ready to go */

	char wildcards [ ] = { '*', '?' };
	struct filter_t * filters [  ] = { & filter1, & filter2 };
	char * patterns [ ] = { pattern1, pattern2 };
	char * replaces [ ] = { replace1, replace2 };
	char * excludes [ ] = { exclude1, exclude2 };

	printf ( "listing %s*%s\n", directory, extension );
	printf ( "RDO HID SYS ARC      SIZE FILE %30c COMMAND\n", ' ' );
	printf ( "--- --- --- ---      ---- ---- %30c -------\n", ' ' );

	return traverse3 ( directory, extension, sizeof ( patterns ) / sizeof ( patterns [ 0 ] ), wildcards, filters, header, footer, patterns, replaces, excludes, "" );
}

int my_traverse3 ( const char * directory, const char * extension )
{
	char header [ ] = "";
	char footer [ ] = "";

	char pattern [ ] = "*\n*\n*\n*";
	char replace [ ] = "\n\n";
	char exclude [ ] = "";

	struct filter_t filter = { 0 }; /* init */
	/* filter_no_relay_initiate */
	filter.filter_on_initiate = filter_quote;
	filter.filter_after_replace = filter_backward;

	printf ( "listing %s*%s\n", directory, extension );
	printf ( "RDO HID SYS ARC      SIZE FILE %30c COMMAND\n", ' ' );
	printf ( "--- --- --- ---      ---- ---- %30c -------\n", ' ' );

	return traverse2 ( directory, extension, KNOWN_ALPHABET_BLANK, '*', & filter, header, footer, pattern, replace, exclude, "" );
}

int my_traverse4 ( const char * directory, const char * extension, const char * log )
{
	char header [ ] = "";
	char footer [ ] = "";

	char pattern [ ] = "function\v2\v8*(*)*{";
					/* "\aerror_log(\"c:/apache24/htdocs\".$_SERVER['PHP_SELF'].\">\f>\b\\n\", 3, \"c:/test/err.log\");" */
	char replace [ ] = "\aerror_log($_SERVER['PHP_SELF'].\">\f>\b\".PHP_EOL, 3, \"\f\");";
	char exclude [ ] = "\r\n{"; /* what characters a matched @string excludes */

	const char * knowledge [ ] =
	{
		":?"					/* , */
		KNOWN_ALPHABET_BLANK	/* , */
		KNOWN_ALPHABET_NAME,	/* wildcard segment 2, a return type for php 7, e.g. function name ( ) : type { } */
		NULL,					/* wildcard segment 1, function parameters are arbitrary characters with no limit */
		KNOWN_ALPHABET_BLANK	/* wildcard segment 0, a function name has already been defined at \v9 */
	};

	struct filter_t filter = { 0 }; /* init */
	/* filter_relay_initiate */
	filter.filter_on_initiate = filter_quote;
	filter.filter_on_terminate = filter_quote;
	filter.filter_on_equal = filter_alphabet; /* filter_blank */
	filter.filter_on_exclude = filter_escape; /* "\"\'$" */
	filter.filter_on_load = filter_forward;

	printf ( "listing %s*%s\n", directory, extension );
	printf ( "RDO HID SYS ARC      SIZE FILE %30c COMMAND\n", ' ' );
	printf ( "--- --- --- ---      ---- ---- %30c -------\n", ' ' );

	return traverse4 ( directory, extension, sizeof ( knowledge ) / sizeof ( knowledge [ 0 ] ), knowledge, '*', & filter, header, footer, pattern, replace, exclude, log );
}

int my_report1 ( const char * log, const char * tmp )
{
	printf ( "parsing %s\n", log );

	return report_copy_file ( log, tmp );
}

int my_report2 ( const char * tmp, const char * obj )
{
	printf ( "parsing %s\n", tmp );

	return nonredundancy_copy_file ( tmp, obj );
}

int my_report3 ( const char * obj, const char * inc )
{
	char lead [ ] = "require_once __DIR__ . '";
	char trail [ ] = "';";

	printf ( "parsing %s\n", obj );

	return rich_copy_file ( obj, inc, lead, trail );
}

int my_report4 ( const char * log, const char * tmp )
{
	char pattern [ ] = "\v1\v9:\v6";
	char replace [ ] = "\b\n";
	char exclude [ ] = ""; /* what characters a matched @string excludes */

	struct filter_t filter = { 0 }; /* init */
	/* filter_no_relay_initiate */
	filter.filter_on_initiate = filter_quote;
	filter.filter_on_equal = filter_alphabet2; /* filter_blank */
	filter.filter_on_exclude = filter_escape2; /* "\"\'$" */

	printf ( "parsing %s\n", log );

	return strip_copy_file ( log, tmp, '*', & filter, pattern, replace, exclude );
}

int my_report5 ( int threshold, const char * log, const char * tmp )
{
	printf ( "parsing %s\n", log );

	return field_copy_file ( threshold, log, tmp );
}

int my_report6 ( const char * tmp, const char * obj )
{
	printf ( "parsing %s\n", tmp );

	return float_delta_copy_file ( tmp, obj );
}

int my_report7 ( const char * obj, const char * ndx )
{
	printf ( "parsing %s\n", obj );

	return float_index_copy_file ( obj, ndx );
}

int my_report8 ( const char * tmp, const char * obj )
{
	printf ( "parsing %s\n", tmp );

	return integer_delta_copy_file ( tmp, obj );
}

int my_report9 ( const char * obj, const char * ndx )
{
	printf ( "parsing %s\n", obj );

	return integer_index_copy_file ( obj, ndx );
}

int my_directory ( const char * obj )
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
	printf ( "copying from %s\n", obj );

	return copy_listed_files ( obj );
}

int my_debug ( const char * dbg )
{
	printf ( "debugging %s\n", dbg );

	return debug ( KNOWN_ALPHABET_DEBUG, dbg );
}

int load_params ( const char * listname )
{
	int i, j, h;
	char list_entry [ _MAX_PATH ];
	char * list;
	int list_len;
	char * var;
	int num;

	list_len = load_file ( listname );
	if ( ! list_len )
	{
		printf ( "failed to load %s\n", listname );
		return 0;
	}

	list = src_buf;

	num = -1;
	h = 0, i = 0;
	while ( i < list_len )
	{
		if ( h + 1 == _MAX_PATH )
			return 0;
		
		if ( '\r' == *( list + i ) )
		{
			i ++;
			continue;
		}

		if ( '\n' == *( list + i ) )
		{
			*( list_entry + h ) = 0;

			printf ( "%s\n", list_entry );

			if ( num == -1 )
			{
				printf ( "bad format\n" );
				return 0;
			}

			var = param_list [ num ] [ 1 ];
			copy_string ( list_entry, var, _MAX_PATH );

			i ++;
			h = 0;
			num = -1;
			continue;
		}
		
		if ( '=' == *( list + i ) )
		{
			*( list_entry + h ) = 0;
	
			printf ( "%s=", list_entry );
			
			for ( j = 0; j < param_list_len; j ++ )
			{
				var = param_list [ j ] [ 0 ];
				if ( compare_string ( list_entry, var ) == 0 )
				{
					num = j;
					break;
				}
			}
			if ( num == -1 )
			{
				printf ( "bad variable\n" );
				return 0;
			}

			i ++;
			h = 0;
			continue;
		}

		*( list_entry + h ++ ) = *( list + i ++ );
	}

	return 1;
}

int run ( int operation )
{
	char * src = param_list [ 0 ] [ 1 ];
	char * dst = param_list [ 1 ] [ 1 ];
	char * ext = param_list [ 2 ] [ 1 ];

	char * log = param_list [ 3 ] [ 1 ];
	char * tmp = param_list [ 4 ] [ 1 ];
	char * obj = param_list [ 5 ] [ 1 ];
	char * inc = param_list [ 6 ] [ 1 ];

	char * xt_log = param_list [ 7 ] [ 1 ];
	char * xt_tmp = param_list [ 8 ] [ 1 ];
	char * xt_obj = param_list [ 9 ] [ 1 ];
	char * xt_inc = param_list [ 10 ] [ 1 ];
	char * xt_time_tmp = param_list [ 11 ] [ 1 ];
	char * xt_time_obj = param_list [ 12 ] [ 1 ];
	char * xt_time_ndx = param_list [ 13 ] [ 1 ];
	char * xt_mem_tmp = param_list [ 14 ] [ 1 ];
	char * xt_mem_obj = param_list [ 15 ] [ 1 ];
	char * xt_mem_ndx = param_list [ 16 ] [ 1 ];

	char * dbg = param_list [ 17 ] [ 1 ];

	src_dir = src;
	dst_dir = dst;

	switch ( operation )
	{
		case 1: return my_match1 ( ) && my_match2 ( ) && my_match3 ( )
					   && my_match4 ( ) && my_match5 ( ) && my_match6 ( )
					   && my_match7 ( ) && my_match8 ( );
		case 2: return my_traverse1 ( src, ext ) && my_traverse3 ( src, ext ) && my_traverse4 ( src, ext, log );
		case 3: return my_report1 ( log, tmp ) && my_report2 ( tmp, obj )  && my_report3 ( obj, inc )
					   && my_directory ( obj );
		case 4: return my_report4 ( xt_log, xt_tmp ) && my_report2 ( xt_tmp, xt_obj )  && my_report3 ( xt_obj, xt_inc )
					   && my_report5 ( TIME_THRESHOLD_VALUE, xt_log, xt_time_tmp ) && my_report6 ( xt_time_tmp, xt_time_obj ) && my_report7 ( xt_time_obj, xt_time_ndx )
					   && my_report5 ( MEM_THRESHOLD_VALUE, xt_log, xt_mem_tmp ) && my_report8 ( xt_mem_tmp, xt_mem_obj ) && my_report9 ( xt_mem_obj, xt_mem_ndx )
					   && my_directory ( xt_obj );
		case 5: return my_debug ( dbg );
	}

	return 0;
}

/* if successful, returns 1. otherwise, returns 0 */

int main ( int argc, char * argv [ ] )
{
	char * buffer;
	int operation;
	int c;

	buffer = ( char * ) malloc ( MAX_FILE_SIZE + MAX_FILE_SIZE );
	if ( ! buffer )
	{
		printf ( "insufficient memory available\n" );
		return 0;
	}

	src_buf = buffer;
	dst_buf = buffer + MAX_FILE_SIZE;

	printf ( "***********************************************************************************\n" );
	printf ( "CAR: Computer Aided Read\n" );
	printf ( "copyright (C) 2025.10.28 TOP WAYE topwaye@hotmail.com\n" );
	printf ( "usage:\n" );
	printf ( "car\n" );
	printf ( "car 1\n" );
	printf ( "car 2 c:/car/car.ini\n" );
	printf ( "car 3 c:/car/car.ini\n");
	printf ( "car 4 c:/car/car.ini\n" );
	printf ( "***********************************************************************************\n" );

	if ( ! load_params ( argc == 3 ? argv [ 2 ] : DEFAULT_INIT_FILE ) )
		printf ( "warning: bad car.ini, use default parameters instead\n" );

	printf ( "***********************************************************************************\n" );
	
	switch ( argc )
	{
		case 3:
		case 2:
			if ( compare_string ( argv [ 1 ], "1" ) == 0 )
				operation = 1;
			else if ( compare_string ( argv [ 1 ], "2" ) == 0 )
				operation = 2;
			else if ( compare_string ( argv [ 1 ], "3" ) == 0 )
				operation = 3;
			else if ( compare_string ( argv [ 1 ], "4" ) == 0 )
				operation = 4;
			else if ( compare_string ( argv [ 1 ], "5" ) == 0 )
				operation = 5;
			else
				operation = 0;
			break;
		case 1:
			printf ( "[ 1 ] match a pattern string\n" );
			printf ( "[ 2 ] traverse a directory tree and insert a line of source code into all files\n" );
			printf ( "[ 3 ] make a new directory tree on the disk according to a tracing report\n" );
			printf ( "[ 4 ] make a new directory tree on the disk according to an xdebug tracing report\n" );
			printf ( "[ 5 ] debug a file to print each known and unknown character\n" );
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
				case '5': operation = 5; break;
				default: operation = 0;
			}
			break;
		default:
			operation = 0;
	}

	if ( ! run ( operation ) )
	{
		printf ( "operation failed\n" );
		
		free ( buffer );
		return 0;
	}

	free ( buffer );

	return 1;
}