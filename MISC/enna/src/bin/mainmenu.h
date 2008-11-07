#ifndef _ENNA_MAINMENU_H_
#define _ENNA_MAINMENU_H_

#include "enna.h"

Evas_Object *enna_mainmenu_add(Evas * evas);

void enna_mainmenu_append(Evas_Object *obj, Evas_Object *icon,
        const char *label, Enna_Class_Activity *act, void (*func) (void *data), void *data);
Enna_Class_Activity *enna_mainmenu_selected_activity_get(Evas_Object *obj);
void enna_mainmenu_load_from_activities(Evas_Object *obj);
void enna_mainmenu_select_next(Evas_Object *obj);
void enna_mainmenu_select_prev(Evas_Object *obj);
void enna_mainmenu_select_nth(Evas_Object *obj, int nth);
void enna_mainmenu_show(Evas_Object *obj);
void enna_mainmenu_hide(Evas_Object *obj);
unsigned char enna_mainmenu_visible(Evas_Object *obj);
void enna_mainmenu_activate_nth(Evas_Object *obj, int nth);
int enna_mainmenu_selected_get(Evas_Object *obj);
#endif
