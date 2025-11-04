/*
 * traverse.c
 *
 * Copyright (C) 2025.10.21 TOP WAYE topwaye@hotmail.com
 *
 * traverse all files in the current directory including subdirectories
 * 
 * copy and replace pattern words with predefined words in each file
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

extern int hit_count;

/* if successful, returns 1. otherwise, returns 0 */

int do_command ( char * filename, char wildcard,
                 struct filter_t * filter,
                 char * header, char * footer, char * pattern, char * replace, char * exclude )
{
	int fh;
	int bytes_read, bytes_copied, bytes_written;
	int size;
	char * pos;
    int dirty;

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
	
	size = MAX_FILE_SIZE;
	pos = dst_buf;
	bytes_copied = copy_string ( header, pos, size, filename );
    dirty = bytes_copied > 0;
	pos += bytes_copied;
	size -= bytes_copied;
	bytes_copied = copy_and_replace_ex ( wildcard, filter, src_buf, bytes_read, pos, size,
                                         pattern, replace, exclude, filename );
    dirty += hit_count;
	pos += bytes_copied;
	size -= bytes_copied;
	bytes_copied = copy_string ( footer, pos, size, filename );
    dirty += bytes_copied > 0;
	size -= bytes_copied;
	bytes_copied = MAX_FILE_SIZE - size;

    if ( ! dirty )
    {
        printf ( "%d bytes read, ", bytes_read );
	    printf ( "%d bytes copied\n", bytes_copied );

        _close ( fh );

        return 1;
    }

	/* seek the beginning of the file */
	_lseek ( fh, 0L, SEEK_SET );
	
	/* change file size */
	if ( _chsize_s ( fh, bytes_copied ) )
	{
		printf ( "problem in changing the size\n" );
		_close ( fh );
		return 0;
	}
	
	/* write out output */
	bytes_written = _write ( fh, dst_buf, bytes_copied );
	if ( bytes_written == -1 )
    {
        printf ( "problem writing file\n" );
        _close ( fh );
        return 0;
    }

	printf ( "%d bytes read, ", bytes_read );
	printf ( "%d bytes copied, ", bytes_copied );
	printf ( "%d bytes written\n", bytes_written );

    _close ( fh );

    return 1;
}

int traverse ( const char * directory, const char * extension, char wildcard,
               struct filter_t * filter,
               char * header, char * footer, char * pattern, char * replace, char * exclude )
{
    int ext_len;
    char path [ _MAX_PATH ];
    struct __finddata64_t info;
    intptr_t handle;

    ext_len = string_length ( extension );
    copy_string ( directory, path, _MAX_PATH );
    concatenate_string ( "*.*", path, _MAX_PATH );

    handle = _findfirst64 ( path, &info );
    if ( handle != -1 )
    {
        do
        {
            /* determine whether this is a subdirectory */
            if ( info.attrib & _A_SUBDIR )
            {
                if ( compare_string ( info.name, "." ) == 0 || compare_string ( info.name, ".." ) == 0 )
                    continue;

                printf ( info.attrib & _A_RDONLY ? " Y  " : " N  " );
                printf ( info.attrib & _A_SYSTEM ? " Y  " : " N  " );
                printf ( info.attrib & _A_HIDDEN ? " Y  " : " N  " );
                printf ( info.attrib & _A_ARCH ? " Y  " : " N  " );
                printf ( "%9lld %-35s ", info.size, info.name );

                printf ( "<DIR>\n" );

                copy_string ( directory, path, _MAX_PATH );
                concatenate_string ( info.name, path, _MAX_PATH );
                concatenate_string ( "/", path, _MAX_PATH );

                if ( ! traverse ( path, extension, wildcard, filter, header, footer, pattern, replace, exclude ) )
                    return 0;
            }
            else
            {
                if ( compare_string_ex ( -1, ext_len, info.name, extension ) != 0 )
                    continue;

                printf ( info.attrib & _A_RDONLY ? " Y  " : " N  " );
                printf ( info.attrib & _A_SYSTEM ? " Y  " : " N  " );
                printf ( info.attrib & _A_HIDDEN ? " Y  " : " N  " );
                printf ( info.attrib & _A_ARCH ? " Y  " : " N  " );
                printf ( "%9lld %-35s ", info.size, info.name );

                copy_string ( directory, path, _MAX_PATH );
                concatenate_string ( info.name, path, _MAX_PATH );

                if ( ! do_command ( path, wildcard, filter, header, footer, pattern, replace, exclude ) )
                    return 0;
            }

        } while ( _findnext64 ( handle, &info ) != -1 );

        _findclose ( handle );
    }

    return 1;
}
