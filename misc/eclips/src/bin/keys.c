#include <Evas.h>
#include <unistd.h>
#include <math.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include "keys.h"

char *pressed_key = NULL;

void cb_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{      
   Evas_Event_Key_Down *ev;
 
   ev = event_info;
   pressed_key = strdup(ev->keyname);
   
   if (!strcmp(ev->keyname, "Escape")) ecore_main_loop_quit();
   if (!strcmp(ev->keyname, "q")) ecore_main_loop_quit();
   if (!strcmp(ev->keyname, "Q")) ecore_main_loop_quit();
}

void cb_key_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{      
   Evas_Event_Key_Up *ev;
 
   ev = event_info;

   pressed_key = NULL;
}
