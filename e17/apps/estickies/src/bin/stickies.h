#ifndef _STICKIES_H
#define _STICKIES_H
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <Etk.h>
#include <Eet.h>
#include <Ecore_File.h>
#include <Ecore_X.h>

#include "config.h"

#define ERROR(string) \
     fprintf(stderr,"[estickies error]: "#string"\n");
#define DEBUG(string) \
   fprintf(stdout,"[estickies debug]: "#string"\n");

#define E_CONTAINS(x, y, w, h, xx, yy, ww, hh) (((xx) >= (x)) && (((x) + (w)) >= ((xx) + (ww))) && ((yy) >= (y)) && (((y) + (h)) >= ((yy) + (hh))))
#define E_INTERSECTS(x, y, w, h, xx, yy, ww, hh) (((x) < ((xx) + (ww))) && ((y) < ((yy) + (hh))) && (((x) + (w)) > (xx)) && (((y) + (h)) > (yy)))

#ifdef E_FREE
#undef E_FREE
#endif
#define E_FREE(ptr) if(ptr) { free (ptr); ptr = NULL;}

#ifdef E_NEW
#undef E_NEW
#endif
#define E_NEW(count, type) calloc(count, sizeof(type))

typedef struct _E_Config_Sticky E_Config_Sticky;
typedef struct _E_Config_Stickies E_Config_Stickies;
typedef struct _E_Config_General E_Config_General;
typedef struct _E_Config_Version E_Config_Version;
typedef struct _E_Sticky E_Sticky;
typedef struct _E_Stickies E_Stickies;

#include "conf.h"

struct _E_Config_Sticky
{
   int x, y, w, h;
   int r, g, b, a;
   char *text;
};

struct _E_Config_Stickies
{
   int test;
   Evas_List *stickies;
};

struct _E_Config_General
{
   int nothing;
};

struct _E_Config_Version
{
   int major;
   int minor;
   int patch;
};

struct _E_Sticky
{
   int x, y, w, h;
   int r, g, b, a;
   char *text;
   
   /* non-savable fields */
   Etk_Widget *win;
   Etk_Widget *textview;
   Etk_Widget *buttonbox;
   Etk_Widget *stick_toggle;
};

struct _E_Stickies
{
   Evas_List *stickies;
   E_Config_Version *version;
};

#endif
