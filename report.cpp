/*
 * report.c
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

#define MAX_THRESHOLD_VALUE		  1

extern char * src_buf;
extern char * dst_buf;

int nonredundancy_copy ( char * src, int src_len, char * dst, int dst_size )
{
	int i, j, h, ii;

	if ( dst_size < 1 ) /* size >= len + 1 */
		return 0;

	h = 0, i = ii = 0;
	while ( i < src_len )
	{
		if ( '\n' == *( src + i ) )
		{
			*( src + i ) = 0;

			j = ii;
			if ( ! match ( src + j, dst, h, 1 ) ) 
			{
				while ( *( src + j ) )
				{
					if ( h + 1 == dst_size )
						return 0;

					*( dst + h ++ ) = *( src + j ++ );
				}

				if ( h + 1 == dst_size )
					return 0;

				*( dst + h ++ ) = '\n';
			}

			ii = ++ i;
			continue;
		}

		if ( '\r' == *( src + i ) )
			*( src + i ) = 0;
		
		i ++;
	}

	*( dst + h ) = 0;

	return h;
}

int report_copy ( char * src, int src_len, char * dst, int dst_size )
{
	int i, h;
	int threshold;

	if ( dst_size < 1 ) /* size >= len + 1 */
		return 0;

	h = 0, i = 0; threshold = 0;
	while ( i < src_len )
	{
		if ( h + 1 == dst_size )
			return 0;

		if ( '\n' == *( src + i ) )
		{
			threshold = 0; /* reinit */

			i ++;
			continue;
		}

		if ( threshold > MAX_THRESHOLD_VALUE ) /* must be here, do NOT move this line */
		{
			i ++;
			continue;
		}

		if ( '>' == *( src + i ) )
		{
			threshold ++;

			*( dst + h ++ ) = '\n';

			i ++;
			continue;
		}

		*( dst + h ++ ) = *( src + i ++ );
	}

	*( dst + h ) = 0;

	return h;
}

/* if successful, returns 1. otherwise, returns 0 */

int report_copy_file ( const char * src_filename, const char * dst_filename )
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

	bytes_copied = report_copy ( src_buf, bytes_read, dst_buf, MAX_FILE_SIZE );

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

int nonredundancy_copy_file ( const char * src_filename, const char * dst_filename )
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

	bytes_copied = nonredundancy_copy ( src_buf, bytes_read, dst_buf, MAX_FILE_SIZE );

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
