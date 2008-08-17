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
#include <Ecore_Con.h>
#include <Ecore_Job.h>
#include <Ecore_Data.h>

#define ETK_SERVER_LIBETK "libetk.so"
#define E_FREE(ptr) if(ptr) { free(ptr); ptr = NULL; }

typedef struct _Etk_Server_Func Etk_Server_Func;
typedef struct _Etk_Server_Signal Etk_Server_Signal;

struct _Etk_Server_Func
{
      char *name;
      char *return_type;
      Evas_List *arg_names;
      Evas_List *arg_types;
};

struct _Etk_Server_Signal
{
   char *name;
   char *marshaller;
};

void *etk_server_var_get(char *key);
char *etk_server_callback();
void  etk_server_shutdown();
void  etk_server_signal_connect(char *signal, Etk_Object *object, char *id);
void  _etk_server_valid_funcs_populate();
void  _etk_server_valid_signals_populate();
#endif
