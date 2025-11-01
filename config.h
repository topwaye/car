/*
 * config.h
 *
 * Copyright (C) 2025.10.26 TOP WAYE topwaye@hotmail.com
 */

#ifndef CONFIG_H
#define CONFIG_H

#define MAX_BUFFER_SIZE         4096    /* 4KB */
#define MAX_FILE_SIZE		    4194304 /* 4MB */

#define KNOWN_ALPHABET_NUM		"1234567890"
#define KNOWN_ALPHABET_BLANK	" \t\r\n"
#define KNOWN_ALPHABET_DEBUG	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890_$?!&\"\'<>(){}[]+-*/.,;="

struct filter_t
{
    int ( * filter_on_replace ) ( char *, int, int, int *, char *, int, int * );
	int ( * filter_on_load ) ( char *, int, int, int *, char *, int, int * );
};

#endif
