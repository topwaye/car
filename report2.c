/*
 * report2.c
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

#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "match.h"

extern char * src_buf;
extern char * dst_buf;

/* if successful, returns 1. otherwise, returns 0 */

int strip_copy_file ( const char * src_filename, const char * dst_filename, char wildcard,
                	  struct filter_t * filter,
            		  char * pattern, char * replace, char * exclude )
{
	int src_fh, dst_fh;
	int bytes_read, bytes_copied, bytes_written;

    /* open file for input */
    if ( _sopen_s ( &src_fh, src_filename, _O_BINARY | _O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE) )
    {
        printf ( "open failed on input file\n" );
        return 0;
    }
	
	/* open file for output */
    if ( _sopen_s ( &dst_fh, dst_filename, _O_CREAT | _O_BINARY | _O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE) )
    {
        printf ( "open failed on output file\n" );
		_close( src_fh );
		return 0;
    }

	/* read in input */
	bytes_read = _read ( src_fh, src_buf, MAX_FILE_SIZE );
	if ( bytes_read == -1 )
	{
		printf ( "problem reading file\n" );
		_close ( dst_fh );
		_close ( src_fh );
		return 0;
	}

	bytes_copied = copy_and_replace_ex ( 1, wildcard, filter, src_buf, bytes_read, dst_buf, MAX_FILE_SIZE,
                                         pattern, replace, exclude );

	/* write out output */
	bytes_written = _write ( dst_fh, dst_buf, bytes_copied );
	if ( bytes_written == -1 )
	{
		printf ( "problem writing file\n" );
		_close ( dst_fh );
		_close ( src_fh );
		return 0;
	}

	printf ( "%d bytes read, ", bytes_read );
	printf ( "%d bytes copied, ", bytes_copied );
	printf ( "%d bytes written\n", bytes_written );

    _close ( dst_fh );
	_close ( src_fh );

	return 1;
}
