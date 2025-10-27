﻿/*
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

void report_copy_file ( char * src_filename, char * dst_filename );

void nonredundancy_copy_file ( char * src_filename, char * dst_filename );

#endif

/*
 * int test ( )
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
 *     if ( ! buffer )
 *     {
 *         printf ( "insufficient memory available\n" );
 *         return 1;
 *     }
 *
 *     src_buf = buffer;
 *     dst_buf = buffer + MAX_FILE_SIZE;
 *
 *     report_copy_file ( filename_1, filename_2 );
 *     nonredundancy_copy_file ( filename_2, filename_3 );
 *
 *     free ( buffer );
 *
 *     return 0;
 * }
 *
 */
