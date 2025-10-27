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

void do_command ( char * filename, char wildcard, char * header, char * footer, char * pattern, char * replace, char * exclude )
{
	int fh;
	int bytes_read, bytes_copied, bytes_written;
	int size;
	char * pos;

    /* open file for input */
    if ( _sopen_s ( &fh, filename, _O_BINARY | _O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE) )
    {
        printf ( "open failed on input file\n" );
        return;
    }

    /* read in input */
    bytes_read = _read ( fh, src_buf, MAX_FILE_SIZE );
    if ( bytes_read == -1 )
    {
        printf ( "problem reading file\n" );
        _close ( fh );
        return;
    }

    printf ( "%u bytes read, ", bytes_read );
	
	size = MAX_FILE_SIZE;
	pos = dst_buf;
	bytes_copied = copy_string ( header, pos, size, filename );
	pos += bytes_copied;
	size -= bytes_copied;
	bytes_copied = copy_and_replace_ex ( wildcard, src_buf, bytes_read, pos, size,
										 pattern, replace, exclude, filename );
	pos += bytes_copied;
	size -= bytes_copied;
	bytes_copied = copy_string ( footer, pos, size, filename );
	size -= bytes_copied;
	bytes_copied = MAX_FILE_SIZE - size;

	printf ( "%u bytes copied, ", bytes_copied );

	/* seek the beginning of the file */
	_lseek ( fh, 0L, SEEK_SET );
	
	/* change file size */
	if ( _chsize_s ( fh, bytes_copied ) )
	{
		printf ( "problem in changing the size\n" );
		_close ( fh );
		return;
	}
	
	/* write out output */
	bytes_written = _write ( fh, dst_buf, bytes_copied );
	if ( bytes_written == -1 )
    {
        printf ( "problem writing file\n" );
        _close ( fh );
        return;
    }

	printf ( "%u bytes written\n", bytes_written );

    _close ( fh );
}

void traverse ( char * directory, char wildcard, char * header, char * footer, char * pattern, char * replace, char * exclude )
{
    char path [ _MAX_PATH ];
    struct __finddata64_t info;
    intptr_t handle;

    copy_string ( directory, path, _MAX_PATH );
    concatenate_string ( "*.php", path, _MAX_PATH );

    handle = _findfirst64 ( path, &info );
    if ( handle != -1 )
    {
        do
        {
            printf ( info.attrib & _A_RDONLY ? " Y  " : " N  " );
            printf ( info.attrib & _A_SYSTEM ? " Y  " : " N  " );
            printf ( info.attrib & _A_HIDDEN ? " Y  " : " N  " );
            printf ( info.attrib & _A_ARCH ? " Y  " : " N  " );
            printf ( "%9lld %-36s ", info.size, info.name );

            copy_string ( directory, path, _MAX_PATH );
            concatenate_string ( info.name, path, _MAX_PATH );

            /* determine whether this is a subdirectory */
            if ( info.attrib & _A_SUBDIR )
            {
                printf ( "<DIR>\n" );

                if ( compare_string ( info.name, "." ) == 0 || compare_string ( info.name, ".." ) == 0 )
                    continue;

				concatenate_string ( "/", path, _MAX_PATH );
                traverse ( path, wildcard, header, footer, pattern, replace, exclude );
            }
            else
            {
				do_command ( path, wildcard, header, footer, pattern, replace, exclude );
            }

        } while ( _findnext64 ( handle, &info ) != -1 );

        _findclose ( handle );
    }
}
