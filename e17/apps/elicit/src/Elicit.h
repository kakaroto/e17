#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <X11/Xlib.h>
#include <Imlib2.h>

#include "config.h"
#define DATADIR PACKAGE_DATA_DIR

#include "callbacks.h"
#include "action.h"
#include "util.h"

typedef struct _Elicit Elicit;

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



