#ifndef ICONBAR_H
#define ICONBAR_H

#include <Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <Esmart/container.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

typedef struct _Iconbar Iconbar;
typedef struct _Icon Icon;

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
    
  Evas_Object *clock;
  
};

struct _Icon
{
  Iconbar *iconbar;
  Evas_Object *image;

  char *file;

  pid_t launch_pid;
  int launch_id;
  void *launch_id_cb;
};

Evas_Object *iconbar_new(Evas *evas);
Evas_Object *iconbar_gui_get(Evas_Object *o);
Evas_Smart *iconbar_smart_get();
double iconbar_icons_length_get(Iconbar *ib);

void iconbar_path_set(Evas_Object *iconbar, char *path);
char *iconbar_path_get(Evas_Object *iconbar);

#endif

