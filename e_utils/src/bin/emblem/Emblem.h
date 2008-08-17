#ifndef EMBLEM_H
#define EMBLEM_H

#include <sys/types.h>
#include <dirent.h>

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_Job.h>
#include <Ecore_File.h>
#include <Evas.h>
#include <Edje.h>
#include <Esmart/Esmart_Container.h>
#include <E_Lib.h>
#include <Epsilon.h>

#include "config.h"

#undef __UNUSED__
#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

typedef struct Emblem Emblem;
struct Emblem
{
    char *display;
    char *theme;
   
    struct {
        Ecore_Evas *ee;
        Evas *evas;

        Evas_Object *menu;
        Evas_Object *current;
        Evas_Object *edje;
    } gui;
};

Emblem *emblem_new(void);
void emblem_free(Emblem *em);

int emblem_ui_init(Emblem *em);
void emblem_ui_shutdown(void);

#endif

