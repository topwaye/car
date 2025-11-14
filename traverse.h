/*
 * traverse.h
 *
 * Copyright (C) 2025.10.21 TOP WAYE topwaye@hotmail.com
 *
 * traverse all files in the current directory including subdirectories
 * 
 * copy and replace pattern words with predefined words in each file
 */

#ifndef TRAVERSE_H
#define TRAVERSE_H

/* if successful, returns 1. otherwise, returns 0 */

extern int traverse ( const char * directory, const char * extension, char wildcard,
                      struct filter_t * filter,
                      const char * header, const char * footer, char * pattern, char * replace, char * exclude,
                      const char * log );

extern int traverse2 ( const char * directory, const char * extension, const char * known, char wildcard,
                       struct filter_t * filter,
                       const char * header, const char * footer, char * pattern, char * replace, char * exclude,
                       const char * log );

extern int traverse3 ( const char * directory, const char * extension, int argc, char wildcards [ ],
                       struct filter_t * filters [ ],
                       const char * header, const char * footer, char * patterns [ ], char * replaces [ ], char * excludes [ ],
                       const char * log );

extern int traverse4 ( const char * directory, const char * extension, int argc, const char * knowledge [ ], char wildcard,
                       struct filter_t * filter,
                       const char * header, const char * footer, char * pattern, char * replace, char * exclude,
                       const char * log );

#endif

/*
 * void test ( )
 * {
 *     // make sure this directory is there
 *
 *     // a directory must end with '/', distinguished from a file
 *     char path [ _MAX_PATH ] = "c:/test2/";
 *
 *     char pattern [ ] = "function *(*)*{";
 *     char replace [ ] = "#error_log(\"c:/apache/htdocs\".$_SERVER['PHP_SELF'].\">&>@\\n\", 3, \"c:/test/err.log\");";
 *     char exclude [ ] = "\r\n${"; // what characters a matched @string excludes
 *     char header [ ] = "<?php error_log(\"header<&>\\n\", 3, \"c:/test/err.log\"); ?>\n";
 *     char footer [ ] = "\n<?php error_log(\"footer<&>\\n\", 3, \"c:/test/err.log\"); ?>";
 *
 *     char * buffer = NULL;
 *
 *     buffer = ( char * ) malloc ( MAX_FILE_SIZE + MAX_FILE_SIZE );
 *
 *     src_buf = buffer;
 *     dst_buf = buffer + MAX_FILE_SIZE;
 *
 *     // list the files...
 *     printf ( "listing of files in the directory %s\n\n", path );
 *     printf ( "RDO HID SYS ARC      SIZE FILE %31c COMMAND\n", ' ' );
 *     printf ( "--- --- --- ---      ---- ---- %31c -------\n", ' ' );
 *
 *     traverse ( path, ".php", '*', NULL, header, footer, pattern, replace, exclude );
 *
 *     free ( buffer );
 * }
 *
 */