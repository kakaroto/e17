#ifndef ELICIT_H
#define ELICIT_H

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Ecore_Config.h>
#include <Edje.h>
#include <X11/Xlib.h>
#include <Imlib2.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <sys/stat.h>

#include "config.h"
#define DATADIR PACKAGE_DATA_DIR"/data"

typedef struct _Elicit Elicit;

#include "callbacks.h"
#include "util.h"
#include "conf.h"


struct _Elicit
{
  Ecore_Evas *ee;
  Evas *evas;
  Evas_Object *gui;

  Evas_Object *shot, *swatch;
  
  struct {
    int r, g, b;
    double h, s, v;
    char *hex;
  } color;
  double zoom;
  
  /* flags */
  struct {
    int picking;
    int shooting;
    int changed;
    int changing;
  } flags;

  Ecore_Timer *change_timer;
  char *change_sig;
};


#endif

