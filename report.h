/*
 * report.h
 *
 * Copyright (C) 2025.10.26 TOP WAYE topwaye@hotmail.com
 * 
 * read a file and write a file in predefined format
 * each line is a list entry which ends with '\n' implicitly
 * 
 * input:
 * c:/test/test/test.php>c:/hello/hello.php>function hello(argc, argv)
 * c:/test/test/test.php>c:/hello/world.php>function world(argc, argv)
 * 
 * output:
 * c:/test/test/test.php
 * c:/hello/hello.php
 * c:/hello/world.php
 */

#ifndef REPORT_H
#define REPORT_H

#define TIME_THRESHOLD_VALUE		1
#define MEM_THRESHOLD_VALUE			2

/* if successful, returns 1. otherwise, returns 0 */

extern int strip_copy_file ( const char * src_filename, const char * dst_filename, char wildcard,
                             struct filter_t * filter,
            	             char * pattern, char * replace, char * exclude );

extern int field_copy_file ( int threshold, const char * src_filename, const char * dst_filename );

extern int float_index_copy_file ( const char * src_filename, const char * dst_filename );

extern int float_delta_copy_file ( const char * src_filename, const char * dst_filename );

extern int integer_index_copy_file ( const char * src_filename, const char * dst_filename );

extern int integer_delta_copy_file ( const char * src_filename, const char * dst_filename );

extern int report_copy_file ( const char * src_filename, const char * dst_filename );

extern int nonredundancy_copy_file ( const char * src_filename, const char * dst_filename );

extern int rich_copy_file ( const char * src_filename, const char * dst_filename, const char * lead, const char * trail );

#endif

/*
 * void test ( )
 * {
 *     // make sure these directories and files are there
 *
 *     char filename_1 [ _MAX_PATH ] = "c:/test/err1.log";
 *     char filename_2 [ _MAX_PATH ] = "c:/test/err2.log";
 *     char filename_3 [ _MAX_PATH ] = "c:/test/err3.log";
 *
 *     char * buffer;
 *
 *     buffer = ( char * ) malloc ( MAX_FILE_SIZE + MAX_FILE_SIZE );
 *
 *     src_buf = buffer;
 *     dst_buf = buffer + MAX_FILE_SIZE;
 *
 *     report_copy_file ( filename_1, filename_2 );
 *     nonredundancy_copy_file ( filename_2, filename_3 );
 *
 *     free ( buffer );
 * }
 *
 */
