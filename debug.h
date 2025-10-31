/*
 * directory.h
 *
 * Copyright (C) 2025.10.31 TOP WAYE topwaye@hotmail.com
 * 
 * print known and unknown characters of a file
 */

#ifndef DEBUG_H
#define DEBUG_H

/* if successful, returns 1. otherwise, returns 0 */

int debug ( const char * known, const char * filename );

#endif

/*
 * #define KNOWN_TEST_ALPHABET		"ABCDEFabcdef1234567890"
 *
 * void test ( )
 * {
 *     // make sure these directories and files are there
 *
 *     char filename [ _MAX_PATH ] = "c:/test/test.txt";
 *
 *     char * buffer;
 *
 *     buffer = ( char * ) malloc ( MAX_FILE_SIZE );
 *
 *     debug ( KNOWN_TEST_ALPHABET, filename );
 *
 *     free ( buffer );
 *
 *     return 0;
 * }
 * 
 */