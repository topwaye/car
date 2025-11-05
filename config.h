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

typedef int ( * filter_equal_t ) ( char *, int *, char *, int, int * );
typedef int ( * filter_initiate_t ) ( char *, int, int * );
typedef int ( * filter_operation_t ) ( char *, int, int, int *, char *, int, int * );

struct filter_t
{
    filter_equal_t filter_equal;
    filter_initiate_t filter_initiate;
    filter_operation_t filter_before_replace;
    filter_operation_t filter_after_replace;
	filter_operation_t filter_on_load;
    filter_operation_t filter_on_custom;
};

#endif
