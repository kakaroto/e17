#ifndef ELICIT_H
#define ELICIT_H

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Ecore_Config.h>
#include <Ecore_File.h>
#include <Edje.h>
#include <Esmart/Esmart_Draggies.h>
#include <Esmart/Esmart_Container.h>
#include <Esmart/Esmart_Resize.h>
#include <Eet.h>
#include <X11/Xlib.h>
#include <Imlib2.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <sys/stat.h>
#include <fnmatch.h>

#include "config.h"

#ifdef HAVE_EDB
#include <Edb.h>
#endif

#define DATADIR PACKAGE_DATA_DIR
#define DEFAULT_THEME "winter"

typedef struct _Elicit Elicit;
typedef struct _Elicit_Ruler Elicit_Ruler;
typedef struct _Elicit_Swatch Elicit_Swatch;
typedef struct _Elicit_Shot Elicit_Shot;

#include "callbacks.h"
#include "util.h"
#include "conf.h"
#include "ruler.h"
#include "swatches.h"
#include "shots.h"
#include "themes.h"
#include "spectrum.h"
#include "zoom.h"

typedef enum _Elicit_Ruler_Orient
{
  ELICIT_RULER_ORIENT_TOP,
  ELICIT_RULER_ORIENT_RIGHT,
  ELICIT_RULER_ORIENT_BOTTOM,
  ELICIT_RULER_ORIENT_LEFT
} Elicit_Ruler_Orient;

typedef enum _Elicit_Ruler_Units
{
  ELICIT_RULER_UNITS_PX,
  ELICIT_RULER_UNITS_CM,
  ELICIT_RULER_UNITS_IN,
  ELICIT_RULER_UNITS_PC,
  ELICIT_RULER_UNITS_PT
} Elicit_Ruler_Units;

struct _Elicit
{
  Ecore_Evas *ee;
  Evas *evas;
  Evas_Object *gui;

  Evas_Object *draggie;
  Evas_Object *shot, *swatch;

  char *app_name;

  struct {
    int r, g, b;
    double h, s, v;
    char *hex;
  } color;
  double zoom;
  double zoom_max;
  
  /* flags */
  struct {
    int picking;
    int shooting;
    int changed;
    int changing;
    int swatch_scrolling;
    int shot_scrolling;
    int shot_taken;
    int ruler;
  } flags;

  struct {
    Evas_Object *cont;
    double length;
    int scrolling;
    int dir;
  } swatches, shots, themes;

  Ecore_Timer *change_timer;
  char *change_sig;

  char *tmpdir;
};

struct _Elicit_Ruler
{
  Ecore_Evas *ee;
  Ecore_Evas *popup_ee;
  Evas *evas;
  Evas_Object *ruler;
  Evas_Object *draggie;
  Evas_Object *resize;
  Evas_Object *smart;
  Evas_Object *popup_o;

  Elicit_Ruler_Orient orient;
  Elicit_Ruler_Units units;

  Elicit *el;

  char *theme;
  int r, g, b, a;
  int popup : 1;
};

struct _Elicit_Swatch
{
  Evas_Object *obj;
  Evas_Object *rect;

  char *name;
  int r, g, b;
};

struct _Elicit_Shot
{
  Evas_Object *obj;
  Evas_Object *shot;

  char *name;
};

void elicit_ui_theme_set(Elicit *el, char *name, char *group);

void elicit_ui_update(Elicit *el); /* this does the next two */
void elicit_ui_update_text(Elicit *el);
void elicit_ui_update_sliders(Elicit *el);

#endif

