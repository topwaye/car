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

extern const char * src_dir;
extern const char * dst_dir;
extern char * src_buf;

/* if successful, returns 1. otherwise, returns 0 */

int make_directory ( const char * dirname )
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
			return 0;
		}
	}

	return 1;
}

int copy_file ( const char * src_filename, const char * dst_filename )
{
	char file_buf [ MAX_BUFFER_SIZE ];
	int src_fh, dst_fh;
	int bytes_read, bytes_written;

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

	while ( 1 )
	{
		/* read in input */
		bytes_read = _read ( src_fh, file_buf, MAX_BUFFER_SIZE );
		if ( bytes_read == -1 )
		{
			printf ( "problem reading file\n" );
			_close ( dst_fh );
			_close ( src_fh );
			return 0;
		}

		if ( bytes_read == 0 ) /* EOF */
			break;

		/* write out output */
		bytes_written = _write ( dst_fh, file_buf, bytes_read );
		if ( bytes_written == -1 )
		{
			printf ( "problem writing file\n" );
			_close ( dst_fh );
			_close ( src_fh );
			return 0;
		}
	}

    _close ( dst_fh );
	_close ( src_fh );

	return 1;
}

/* returns the number of bytes read */
int load_file ( const char * filename )
{
	int fh;
	int bytes_read;

	printf ( "loading %s\n", filename );

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

    _close ( fh );

	return bytes_read;
}

int copy_listed_files ( const char * listname )
{
	int i, f, g, h, k;
	int a;
	const char * src_path;
	const char * dst_path;
	char src_entry [ _MAX_PATH ];
	char dst_entry [ _MAX_PATH ];
	char * list;
	int list_len;

	list_len = load_file ( listname );
	if ( ! list_len )
	{
		printf ( "failed to load %s\n", listname );
		return 0;
	}

	list = src_buf;
	src_path = src_dir;
	dst_path = dst_dir;

	g = 0, f = 0;
	while ( *( src_path + f ) )
	{
		if ( g + 1 == _MAX_PATH )
			return 0;

		*( src_entry + g ++ ) = *( src_path + f ++ );
	}

	h = 0, k = 0;
	while ( *( dst_path + k ) )
	{
		if ( h + 1 == _MAX_PATH )
			return 0;

		*( dst_entry + h ++ ) = *( dst_path + k ++ );
	}

	a = 0;
	i = 0;
	while ( i < list_len )
	{
		if ( h + 1 == _MAX_PATH || g + 1 == _MAX_PATH )
			return 0;
		
		if ( '\r' == *( list + i ) )
		{
			i ++;
			continue;
		}

		if ( '\n' == *( list + i ) )
		{
			*( src_entry + g ) = 0;
			*( dst_entry + h ) = 0;

			printf ( "%s > ", src_entry );
			printf ( "%s\n", dst_entry );

			if ( ! copy_file ( src_entry, dst_entry ) )
				return 0;

			a = 0;
			g = f;
			h = k;
			i ++;
			continue;
		}
		
		if ( '/' == *( list + i ) )
		{
			if ( a == 0 )
			{
				a = 1;
				i ++;
				continue;
			}

			*( dst_entry + h ) = 0;

			if ( ! make_directory ( dst_entry ) )
				return 0;
		}

		*( src_entry + g ++ ) = *( list + i );
		*( dst_entry + h ++ ) = *( list + i );
		i ++;
	}

	return 1;
}
