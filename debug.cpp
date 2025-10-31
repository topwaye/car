/*
 * directory.c
 *
 * Copyright (C) 2025.10.31 TOP WAYE topwaye@hotmail.com
 * 
 * print each character of a file
 */

#include <stdio.h>

extern char * src_buf;

extern int load_file ( const char * filename );
extern int is_known_character ( char c, const char * known );

/* if successful, returns 1. otherwise, returns 0 */

int debug ( const char * known, const char * filename )
{
	int i;
	char * src;
	int src_len;

	src_len = load_file ( filename );
	if ( ! src_len )
	{
		printf ( "failed to load %s\n", filename );
		return 0;
	}

	src = src_buf;

	i = 0;
	while ( i < src_len )
	{
		if ( ! is_known_character ( *( src + i ), known ) )
			printf ( "\\x%x", *( src + i ) );
		else
			printf ( "%c", *( src + i ) );

		i ++;
	}

	printf ( "\n");

	return 1;
}
