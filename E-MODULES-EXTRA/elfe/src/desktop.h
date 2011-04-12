#ifndef _DESKTOP_H_
#define _DESKTOP_H_

#include <Elementary.h>

Evas_Object *elfe_desktop_add(Evas_Object *parent, E_Zone *zone);
void elfe_desktop_app_add(Evas_Object *obj, Efreet_Menu *menu, Evas_Coord x, Evas_Coord y);
void elfe_desktop_gadget_add(Evas_Object *obj, const char *name, Evas_Coord x, Evas_Coord y);
void elfe_desktop_edit_mode_set(Evas_Object *obj, Eina_Bool mode);

#endif /* _DESKTOP_H_ */
