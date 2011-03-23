#ifndef _DOCK_H_
#define _DOCK_H_


Evas_Object *elfe_dock_add(Evas_Object *parent);
void elfe_dock_item_app_add(Evas_Object *obj, Efreet_Menu *menu, Evas_Coord x, Evas_Coord y);
void elfe_dock_edit_mode_set(Evas_Object *obj, Eina_Bool mode);

#endif /* _DOCK_H_ */
