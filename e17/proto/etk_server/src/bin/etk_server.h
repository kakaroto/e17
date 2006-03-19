#ifndef _ETK_SERVER_H
#define _ETK_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>

#include <Etk.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_File.h>

#define ETK_SERVER_LIBETK "libetk.so"

void *etk_server_var_get(char *key);
char *etk_server_callback();
void  etk_server_shutdown();
void  etk_server_signal_connect(char *signal, Etk_Object *object, char *id);

#endif
