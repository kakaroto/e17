#include <Evas.h>
#include <unistd.h>
#include <math.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include "eclipse.h"

typedef struct _Eclips_Menu Eclips_Menu;

struct _Eclips_Menu
{
   Ecore_Evas  *window;
   Evas_Object *bg;   
   Evas_Object *text;
};



void root_menu_show();

void root_menu_hide();

void show_menu();

/* show image info in menu */
void show_image_info_menu(Evas_Event_Mouse_Down * ev, Eclipse_View *view);
