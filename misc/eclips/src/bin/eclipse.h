#ifndef ECLIPSE_H
#define ECLIPSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <getopt.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <X11/Xlib.h>
#include <Imlib2.h>

#include "config.h"
#include "help.h"

#define DATADIR PACKAGE_DATA_DIR"/"

typedef struct _Eclipse_Options Eclipse_Options;

struct _Eclipse_Options
{
   int       delay;
   char      *mode;
   int       trans;
   int       alpha_increment;
   double    transition_timer;
   int       geom_w;
   int       geom_h;
   int       pseudo_trans;
   int       menu_trans;
   int       shadows;
   int       random;
   int       borderless;
   Evas_List *images;
};

typedef struct _Eclipse_Image Eclipse_Image;

struct _Eclipse_Image
{
   Evas_Object *img;
   int         x,y,w,h;
   int         layer;
   Evas_List   *list;
};

typedef struct _Eclipse_View Eclipse_View;

struct _Eclipse_View
{
   int               x,y,w,h;
   int               a_old,display,t;
   int               mode; /* 1 = pnp, 2 = multi, 3 = pcn */   
   int               layer;
   Eclipse_Options  *options; /* do we really need this? */
   Evas_List        *images;
   Evas_List        *imfirst;
   Evas_Object      *shadow;
   Eclipse_Image    *curimg;   
};

#endif
