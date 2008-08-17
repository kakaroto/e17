/* vim: set ts=8 sw=3 sts=8 noexpandtab cino=>5n-3f0^-2{2: */
/*
 * Based on code from xmms (Copyright 1999 Håvard Kvålen <havardk@sol.no>)
 */

#ifndef _EX_HTTP_H
#define _EX_HTTP_H

int   ex_http_open_connection(char * server, int port);
void  ex_http_close_connection(int sock);
int   ex_http_read_line(int sock, char * buf, int size);
int   ex_http_read_first_line(int sock, char * buf, int size);
char *ex_http_get(char * url);

#endif
