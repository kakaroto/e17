#include <Evas.h>
#include <unistd.h>
#include <math.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include "eclipse.h"

void cb_resize(Ecore_Evas *ee);
void cb_move(Ecore_Evas *ee);
Eclipse_Options *setup_window(Ecore_Evas *ee, Eclipse_Options *options, int argc, char **argv);
Evas_Object *draw_background(Eclipse_Options *eo,Ecore_Evas *ee, Evas *evas);
    
