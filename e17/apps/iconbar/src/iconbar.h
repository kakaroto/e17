#ifndef ICONBAR_H
#define ICONBAR_H

#include <Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <Esmart/Esmart_Container.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <math.h>
#include <time.h>
#include <string.h>

typedef struct _Iconbar Iconbar;

#include "icon.h"

struct _Iconbar
{
  Evas_Object *obj;
  Evas_Object *cont; // icon container

  Evas_Object *clip;
  Evas_Object *grabber;

  char *path;
  
  Evas_Object *gui;

  int scroll;
  Ecore_Timer *scroll_timer;
  Ecore_Timer *clock_timer;
  
};


Evas_Object *iconbar_new(Evas *evas);
Evas_Object *iconbar_gui_get(Evas_Object *o);
Evas_Smart *iconbar_smart_get();
double iconbar_icons_length_get(Iconbar *ib);

void iconbar_path_set(Evas_Object *iconbar, char *path);
char *iconbar_path_get(Evas_Object *iconbar);

/* hack -- see the .c file */
void iconbar_fix(Evas_Object *obj);

#endif

