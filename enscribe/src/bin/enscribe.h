#include <stdio.h>
#include <stdlib.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <Esmart/Esmart_Textarea.h>

#include "config.h"

#define DATADIR PACKAGE_DATA_DIR"/"

struct _Ens_Window {                     /* app window to hold tabs */
   Ecore_Evas  *ee;
   Evas_Object *menus;
   Evas_List   *tabs;
};

typedef struct _Ens_Window Ens_Window;
   
/* edje callback functions */
void            ens_cb_menu_quit(void *data, Evas_Object * o, const char *emission, const char *source);

/* main window and init functions */
Ens_Window     *ens_window_init(Ecore_Evas *ee);
void            ens_window_tab_add(Ens_Window *ens_win);
