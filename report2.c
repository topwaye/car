/*
 * report2.c
 *
 * Copyright (C) 2025.11.16 TOP WAYE topwaye@hotmail.com
 * 
 * read an xdebug file and write a file in predefined format
 * each line is a list entry which ends with '\n' implicitly
 * 
 * input:
 * 0.4121     403456   -> {main}() c:\test\index.php:0
 * 0.4152     416208     -> require_once(c:\test\hello.php) c:\test\world.php:13
 *
 * output:
 * c:\test\index.php
 * c:\test\world.php
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

extern int seek_unknown_character ( const char * known, char * src, int src_len, int * current );

int x_report_copy ( int threshold, char * src, int src_len, char * dst, int dst_size )
{
	int i, h;
	int a, n;
	int begin, end;

	if ( dst_size < 1 ) /* size >= len + 1 */
		return 0;

	h = 0, i = 0; a = 0;
	while ( i < src_len )
	{
		if ( h + 1 == dst_size )
			return 0;

		if ( ! a )
		{
			begin = end = i;
			for ( n = 0; n < threshold; n ++ )
			{	
				begin = end;
				if ( ! seek_unknown_character ( KNOWN_ALPHABET_BLANK, src, src_len, & begin ) )
					begin = src_len;

				end = begin;
				if ( ! seek_unknown_character ( KNOWN_ALPHABET_NUM, src, src_len, & end ) )
					end = src_len;
			}

			a = 1;
		}

		if ( '\n' == *( src + i ) )
		{
			a = 0; /* reinit */

			*( dst + h ++ ) = '\n';
			i ++;
			continue;
		}

		if ( i >= begin && i < end ) /* must be here, do NOT move this line */
		{
			*( dst + h ++ ) = *( src + i ++ );
			continue;
		}

		i ++;
	}

	*( dst + h ) = 0;

	return h;
}

/* if successful, returns 1. otherwise, returns 0 */

int x_report_copy_file ( int threshold, const char * src_filename, const char * dst_filename )
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

	bytes_copied = x_report_copy ( threshold, src_buf, bytes_read, dst_buf, MAX_FILE_SIZE );

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
