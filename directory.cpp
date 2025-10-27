/*
 * directory.c
 *
 * Copyright (C) 2025.10.22 TOP WAYE topwaye@hotmail.com
 * 
 * copy files, according to a filename list, including subdirectories
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

#include <direct.h>
#include <errno.h>

#include "config.h"

extern char * src_buf;

void make_directory ( char * dirname )
{
	if ( _mkdir ( dirname ) == -1 )
	{
		/*
		 * errno is a global variable in errno.h that _mkdir will set to a
		 * preprocessor constant to identify the specific error that has occurred
		 */
		if ( errno == ENOENT )
		{
			printf ( "path not found.\n" );
			return;
		}
	}
}

void copy_file ( char * src_filename, char * dst_filename )
{
	char file_buf [ MAX_BUFFER_SIZE ];
	int src_fh, dst_fh;
	int bytes_read, bytes_written;
	int total_bytes_read, total_bytes_written;

    /* open file for input */
    if ( _sopen_s ( &src_fh, src_filename, _O_BINARY | _O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE) )
    {
        printf ( "open failed on input file\n" );
        return;
    }
	
	/* open file for output */
    if ( _sopen_s ( &dst_fh, dst_filename, _O_CREAT | _O_BINARY | _O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE) )
    {
        printf ( "open failed on output file\n" );
		_close( src_fh );
		return;
    }

	total_bytes_read = total_bytes_written = 0;
	while ( 1 )
	{
		/* read in input */
		bytes_read = _read ( src_fh, file_buf, MAX_BUFFER_SIZE );
		if ( bytes_read == -1 )
		{
			printf ( "problem reading file\n" );
			_close ( dst_fh );
			_close ( src_fh );
			return;
		}

		if ( bytes_read == 0 ) /* EOF */
			break;

		total_bytes_read += bytes_read;

		/* write out output */
		bytes_written = _write ( dst_fh, file_buf, bytes_read );
		if ( bytes_written == -1 )
		{
			printf ( "problem writing file\n" );
			_close ( dst_fh );
			_close ( src_fh );
			return;
		}

		total_bytes_written += bytes_written;
	}
	
	printf ( "%u bytes read, ", total_bytes_read );
	printf ( "%u bytes written\n", total_bytes_written );

    _close ( dst_fh );
	_close ( src_fh );
}

int load_file ( char * filename )
{
	int fh;
	int bytes_read;

    /* open file for input */
    if ( _sopen_s ( &fh, filename, _O_BINARY | _O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE) )
    {
        printf ( "open failed on input file\n" );
        return 0;
    }

    /* read in input */
    bytes_read = _read ( fh, src_buf, MAX_FILE_SIZE );
    if ( bytes_read == -1 )
    {
        printf ( "problem reading file\n" );
        _close ( fh );
        return 0;
    }

    printf ( "%u bytes read\n", bytes_read );

    _close ( fh );

	return bytes_read;
}

void copy_listed_files ( char * src_path, char * dst_path, char * list, int list_size )
{
	int i, j, h, k, ii;
	char list_entry [ _MAX_PATH ] = { 0 };

	h = 0, k = 0;
	while ( *( dst_path + k ) )
	{
		if ( h + 1 == _MAX_PATH )
			return;

		*( list_entry + h ++ ) = *( dst_path + k ++ );
	}

	i = ii = 0, j = 0;
	while ( i < list_size )
	{
		if ( j == 0 )
		{
			while ( *( src_path + j ) )
			{
				if ( *( src_path + j ++ ) != *( list + i ++ ) )
				{
					printf ( "bad src_path or bad list\n" );
					return;
				}
			}
		}

		if ( h + 1 == _MAX_PATH )
			return;
		
		if ( '\r' == *( list + i ) )
		{
			*( list + i ) = 0;

			printf ( "%s\n", list + ii );

			i ++;
			continue;
		}

		if ( '\n' == *( list + i ) )
		{
			*( list + i ) = 0;
			*( list_entry + h ) = 0;

			printf ( "%s\n", list + ii );
			printf ( "%s\n", list_entry );

			copy_file ( list + ii, list_entry );
			
			ii = ++ i;
			j = 0;
			h = k;
			continue;
		}
		
		if ( '/' == *( list + i ) )
		{
			*( list_entry + h ) = 0;

			make_directory ( list_entry );
		}

		*( list_entry + h ++ ) = *( list + i ++ );
	}
}
