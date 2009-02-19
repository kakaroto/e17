#ifndef _ENNA_MODULE_H_
#define _ENNA_MODULE_H_

#include "enna.h"

#define ENNA_MODULE_VERSION 1

typedef struct _Enna_Module Enna_Module;
typedef struct _Enna_Module_Api Enna_Module_Api;

typedef enum {
    ENNA_MODULE_ACTIVITY,
    ENNA_MODULE_BACKEND,
    ENNA_MODULE_BROWSER,
    ENNA_MODULE_METADATA,
    ENNA_MODULE_VOLUME,
    ENNA_MODULE_INPUT,
} _Enna_Module_Type;

struct _Enna_Module
{
    const char *name;
    struct
    {
        void * (*init)(Enna_Module *m);
        int (*shutdown)(Enna_Module *m);
    } func;

    Enna_Module_Api *api;
    unsigned char enabled;
    Ecore_Plugin *plugin;
    Evas *evas;
    void *mod;
};

struct _Enna_Module_Api
{
    int version;
    _Enna_Module_Type type;
    const char *name;
};

int enna_module_init(void);
int enna_module_shutdown(void);
Enna_Module *enna_module_open(const char *name, _Enna_Module_Type type, Evas *evas);
int enna_module_enable(Enna_Module *m);
int enna_module_disable(Enna_Module *m);

#endif
