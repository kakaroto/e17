#ifndef _DESKTOP_PAGE_H_
#define _DESKTOP_PAGE_H_

Evas_Object *elfe_desktop_page_add(Evas_Object *parent, E_Zone *zone,
                                   int page, const char *desktop_name);
Eina_Bool elfe_desktop_page_pos_is_free(Evas_Object *obj, int row, int col);
void elfe_desktop_page_item_gadget_add(Evas_Object *obj, const char *name,
                                       Evas_Coord x, Evas_Coord y);
void elfe_desktop_page_item_app_add(Evas_Object *obj, Efreet_Menu *menu,
                                    Evas_Coord x, Evas_Coord y);
void elfe_desktop_page_edit_mode_set(Evas_Object *obj, Eina_Bool mode);
#endif /* _DESKTOP_PAGE_H_ */
