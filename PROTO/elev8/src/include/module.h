#ifndef __MODULE_H__
#define __MODULE_H__

#include <v8.h>
#include <stdio.h>
#include <Eina.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#define  MODULE_DEFAULT_PATH	"/usr/local/share/elev8/data/config/module.ini"
#define MAX_LEN 128

#ifdef __cplusplus
extern "C"
{
#endif

/* interfaces to be implemented by modules. */
typedef int (*module_init)(v8::Handle<v8::ObjectTemplate> global, void *data);
typedef int (*module_shutdown)(void *data);

typedef struct
{
   EINA_INLIST;
   void *handle;
   char name[MAX_LEN];
   char path[MAX_LEN];
   module_init init;
   module_shutdown shutdown;
} module_info;

typedef int (*setup_module)(module_info *mi);
void load_modules(v8::Handle<v8::ObjectTemplate> global);

#ifdef __cplusplus
}
#endif

#endif //__MODULE_H__
