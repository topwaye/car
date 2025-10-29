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

int traverse ( const char * directory, const char * specification, char wildcard, char * header, char * footer, char * pattern, char * replace, char * exclude );

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
 *     char replace [ ] = "#\n\t\terror_log(\"c:/apache/htdocs\".$_SERVER['PHP_SELF'].\">&>@\\n\", 3, \"c:/test/err.log\");";
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
 *     traverse ( path, "*.php", '*', header, footer, pattern, replace, exclude );
 *
 *     free ( buffer );
 * }
 *
 */