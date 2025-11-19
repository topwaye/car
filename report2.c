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

int float_index_copy ( char * src, int src_len, int src_size, char * dst, int dst_size )
{
	int i, ii, j, h, t;
	int tmp;
	double current;
	double * values;
	int * indexes;
	char index_in_chars [ _CVTBUFSIZE ];

	if ( dst_size < 1 ) /* size >= len + 1 */
		return 0;

	// 1. write numbers to dst_buf

	h = 0, ii = i = 0;
	while ( i < src_len )
	{
		if ( ! seek_string ( '\n', src, src_len, & i ) ) /* start a new line hopefully */
			break;

		*( src + i ) = 0;

		if ( string_length ( src + ii ) > 0 ) /* NOT i */
		{
			current = atof ( src + ii );
			
			if ( sizeof ( double ) * ( h + 1 ) > dst_size ) /* NOT ==, NOT >= */
				return 0;

			*( ( double * ) dst + h ++ ) = current;
		}

		ii = ++ i; /* NOT i++ */
	}

	// 2. write indexes to src_buf

	for ( i = 0; i < h; i ++ )
	{
		if ( sizeof ( int ) * ( i + 1 ) > src_size ) /* NOT ==, NOT >= */
			return 0;

		*( ( int * ) src + i ) = i;	
	}

	// 3. sort indexes in src_buf

	values = ( double * ) dst;
	indexes = ( int * ) src;
	
	if ( h < 1 )
		return 0;

	for ( i = 0; i < h - 1; i ++ )
	{
		t = i;
		for ( j = i + 1; j < h; j ++ )
			if ( *( values + *( indexes + t ) ) < *( values + *( indexes + j ) ) )
				t = j;

		tmp = *( indexes + t );
		*( indexes + t ) = *( indexes + i );
		*( indexes + i ) = tmp;
	}

	// 4. write indexes to dst_buf
	t = h; h = 0;
	for ( i = 0; i < t; i ++ )
	{
		_itoa_s ( *( indexes + i ), index_in_chars, _CVTBUFSIZE, 10 );

		j = 0;
		while ( *( index_in_chars + j ) )
		{
			if ( h + 1 == dst_size )
				return 0;

			*( dst + h ++ ) = *( index_in_chars + j ++ );
		}
			
		if ( h + 1 == dst_size )
			return 0;
		
		*( dst + h ++ ) = '\n';
	}

	*( dst + h ) = 0;

	return h;
}

int integer_index_copy ( char * src, int src_len, int src_size, char * dst, int dst_size )
{
	int i, ii, j, h, t;
	int tmp;
	int current;
	int * values;
	int * indexes;
	char index_in_chars [ _CVTBUFSIZE ];

	if ( dst_size < 1 ) /* size >= len + 1 */
		return 0;

	// 1. write numbers to dst_buf

	h = 0, ii = i = 0;
	while ( i < src_len )
	{
		if ( ! seek_string ( '\n', src, src_len, & i ) ) /* start a new line hopefully */
			break;

		*( src + i ) = 0;

		if ( string_length ( src + ii ) > 0 ) /* NOT i */
		{
			current = atoi ( src + ii );
			
			if ( sizeof ( int ) * ( h + 1 ) > dst_size ) /* NOT ==, NOT >= */
				return 0;

			*( ( int * ) dst + h ++ ) = current;
		}

		ii = ++ i; /* NOT i++ */
	}

	// 2. write indexes to src_buf

	for ( i = 0; i < h; i ++ )
	{
		if ( sizeof ( int ) * ( i + 1 ) > src_size ) /* NOT ==, NOT >= */
			return 0;

		*( ( int * ) src + i ) = i;	
	}

	// 3. sort indexes in src_buf

	values = ( int * ) dst;
	indexes = ( int * ) src;
	
	if ( h < 1 )
		return 0;

	for ( i = 0; i < h - 1; i ++ )
	{
		t = i;
		for ( j = i + 1; j < h; j ++ )
			if ( *( values + *( indexes + t ) ) < *( values + *( indexes + j ) ) )
				t = j;

		tmp = *( indexes + t );
		*( indexes + t ) = *( indexes + i );
		*( indexes + i ) = tmp;
	}

	// 4. write indexes to dst_buf
	t = h; h = 0;
	for ( i = 0; i < t; i ++ )
	{
		_itoa_s ( *( indexes + i ), index_in_chars, _CVTBUFSIZE, 10 );

		j = 0;
		while ( *( index_in_chars + j ) )
		{
			if ( h + 1 == dst_size )
				return 0;

			*( dst + h ++ ) = *( index_in_chars + j ++ );
		}
			
		if ( h + 1 == dst_size )
			return 0;
		
		*( dst + h ++ ) = '\n';
	}

	*( dst + h ) = 0;

	return h;
}

int float_delta_copy ( char * src, int src_len, char * dst, int dst_size )
{
	int i, ii, iii, j, h;
	int prior_len, current_len;
	double prior, current;
	char delta_in_chars [ _CVTBUFSIZE ];

	if ( dst_size < 1 ) /* size >= len + 1 */
		return 0;

	h = 0, iii = ii = i = 0; prior_len = current_len = 0;
	while ( i < src_len )
	{
		if ( ! seek_string ( '\n', src, src_len, & i ) ) /* start a new line hopefully */
			break;

		*( src + i ) = 0;

		prior_len = current_len;
		current_len = string_length ( src + ii ); /* NOT i, must be here, do NOT move this line */

		if ( prior_len > 0 && current_len > 0 )
		{
			prior = atof ( src + iii );
			current = atof ( src + ii );
			sprintf_s ( delta_in_chars, _CVTBUFSIZE, "%.4f", current - prior ); /* Convert float to string with 4 decimal */

			j = 0;
			while ( *( delta_in_chars + j ) )
			{
				if ( h + 1 == dst_size )
					return 0;

				*( dst + h ++ ) = *( delta_in_chars + j ++ );
			}
			
			if ( h + 1 == dst_size )
				return 0;
		
			*( dst + h ++ ) = '\n';
		}

		iii = ii;
		ii = ++ i; /* NOT i++ */
	}

	*( dst + h ) = 0;

	return h;
}

int integer_delta_copy ( char * src, int src_len, char * dst, int dst_size )
{
	int i, ii, iii, j, h;
	int prior_len, current_len;
	int prior, current;
	char delta_in_chars [ _CVTBUFSIZE ];

	if ( dst_size < 1 ) /* size >= len + 1 */
		return 0;

	h = 0, iii = ii = i = 0; prior_len = current_len = 0;
	while ( i < src_len )
	{
		if ( ! seek_string ( '\n', src, src_len, & i ) ) /* start a new line hopefully */
			break;

		*( src + i ) = 0;

		prior_len = current_len;
		current_len = string_length ( src + ii ); /* NOT i, must be here, do NOT move this line */

		if ( prior_len > 0 && current_len > 0 )
		{
			prior = atoi ( src + iii );
			current = atoi ( src + ii );
			_itoa_s ( current - prior, delta_in_chars, _CVTBUFSIZE, 10 );

			j = 0;
			while ( *( delta_in_chars + j ) )
			{
				if ( h + 1 == dst_size )
					return 0;

				*( dst + h ++ ) = *( delta_in_chars + j ++ );
			}
			
			if ( h + 1 == dst_size )
				return 0;
		
			*( dst + h ++ ) = '\n';
		}

		iii = ii;
		ii = ++ i; /* NOT i++ */
	}

	*( dst + h ) = 0;

	return h;
}

int field_copy ( int threshold, char * src, int src_len, char * dst, int dst_size )
{
	int i, h;
	int n;
	int begin, end;

	if ( dst_size < 1 ) /* size >= len + 1 */
		return 0;

	h = 0, i = 0;
	while ( i < src_len )
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

		i = begin;
		while ( i < end )
		{
			if ( h + 1 == dst_size )
				return 0;

			*( dst + h ++ ) = *( src + i ++ );
		}

		if ( begin < end )
		{
			if ( h + 1 == dst_size )
				return 0;
		
			*( dst + h ++ ) = '\n';
		}

		if ( ! seek_string ( '\n', src, src_len, & i ) ) /* start a new line hopefully */
			break;

		i ++; /* add 1 manually */
	}

	*( dst + h ) = 0;

	return h;
}

/* if successful, returns 1. otherwise, returns 0 */

int field_copy_file ( int threshold, const char * src_filename, const char * dst_filename )
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

	bytes_copied = field_copy ( threshold, src_buf, bytes_read, dst_buf, MAX_FILE_SIZE );

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

int float_index_copy_file ( const char * src_filename, const char * dst_filename )
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

	bytes_copied = float_index_copy ( src_buf, bytes_read, MAX_FILE_SIZE, dst_buf, MAX_FILE_SIZE );

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

int float_delta_copy_file ( const char * src_filename, const char * dst_filename )
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

	bytes_copied = float_delta_copy ( src_buf, bytes_read, dst_buf, MAX_FILE_SIZE );

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

int integer_index_copy_file ( const char * src_filename, const char * dst_filename )
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

	bytes_copied = integer_index_copy ( src_buf, bytes_read, MAX_FILE_SIZE, dst_buf, MAX_FILE_SIZE );

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

int integer_delta_copy_file ( const char * src_filename, const char * dst_filename )
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

	bytes_copied = integer_delta_copy ( src_buf, bytes_read, dst_buf, MAX_FILE_SIZE );

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
