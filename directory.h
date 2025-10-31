/*
 * directory.h
 *
 * Copyright (C) 2025.10.22 TOP WAYE topwaye@hotmail.com
 * 
 * copy files, according to a filename list, including subdirectories
 */

#ifndef DIRECTORY_H
#define DIRECTORY_H

/* if successful, returns 1. otherwise, returns 0 */

int copy_listed_files ( const char * listname, const char * src_path, const char * dst_path );

#endif

/*
 * void test ( )
 * {
 *     // make sure these directories and files are there
 *
 *     // each line is a list entry which ends with '\n' implicitly
 *     //
 *     // list.txt:
 *     //
 *     // c:/test/hello.php
 *     // c:/test/test2/hello_world.php
 *
 *     char file_list_name [ _MAX_PATH ] = "c:/test/list.txt";
 *
 *     // a directory must end with '/', distinguished from a file
 *     char src_path [ _MAX_PATH ] = "c:/test/";
 *     char dst_path [ _MAX_PATH ] = "c:/test_bak/";
 *
 *     char * buffer;
 *
 *     buffer = ( char * ) malloc ( MAX_FILE_SIZE );
 *
 *     copy_listed_files ( file_list_name, src_path, dst_path );
 *
 *     free ( buffer );
 *
 *     return 0;
 * }
 * 
 */