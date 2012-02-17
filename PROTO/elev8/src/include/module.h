#ifndef __MODULE_H__
#define __MODULE_H__

#include <v8.h>
#include <stdio.h>
#include <Eina.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#define MAX_LEN 128
using namespace v8;

#ifdef __cplusplus
extern "C"
{
#endif

typedef int (*module_init)(Handle<ObjectTemplate> global, void *data);
typedef int (*module_deinit)(void *data);

/* interfaces to be implemented by modules. */
struct module_info
{
   char name[MAX_LEN];
   module_init init;
   module_deinit deinit;
};

typedef void (*module_setup)(module_info *);

void load_modules();
void init_modules(Handle<ObjectTemplate> global);

#ifdef __cplusplus
}
#endif

#endif //__MODULE_H__
