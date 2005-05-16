#ifndef ENTANGLE_H
#define ENTANGLE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include <Ecore.h>
#include <Ecore_File.h>
#include <Ecore_Evas.h>
#include <Evas.h>
#include <Edje.h>
#include <Eet.h>
#include <Esmart/Esmart_Container.h>
#include <Esmart/Esmart_Text_Entry.h>

#undef __UNUSED__
#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

#undef FREE
#define FREE(x) { free(x); x = NULL; }

#undef IF_FREE
#define IF_FREE(x) { if (x) FREE(x) }

typedef struct Entangle_Eapp Entangle_Eapp;
struct Entangle_Eapp
{
    char *path;
    char *eapp_name;
    char *name;
    char *exe;
    char *class;
};

typedef struct Entangle_App Entangle_App;
struct Entangle_App
{
    Entangle_Eapp *eapp;

    Ecore_List *children;
};

int entangle_eapps_init(void);
void entangle_eapps_shutdown(void);
Entangle_Eapp *entangle_eapps_get(const char *key);
Ecore_List *entangle_eapps_key_list_get(void);

int entangle_apps_init(void);
void entangle_apps_shutdown(void);
void entangle_apps_save(void);
void entangle_apps_dump(void);
Ecore_List *entangle_apps_list_get(const char *list);

int entangle_ui_init(const char *display, const char *theme);
void entangle_ui_shutdown(void);

#endif

