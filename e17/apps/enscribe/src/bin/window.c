#include "enscribe.h"

/* init our window, set title and class */
Ens_Window
*ens_window_init(Ecore_Evas *ee) {
   Evas_Object *o;
   Evas        *evas;
   Ens_Window *ens_win;
   
   evas = ecore_evas_get(ee);   
   ecore_evas_title_set(ee, "Enscribe");
   ecore_evas_name_class_set(ee, "Enscribe", "Enscribe");
   
   ens_win = malloc(sizeof(Ens_Window));
   ens_win->ee = ee;
   ens_win->menus = edje_object_add(evas);
   evas_font_path_append(ecore_evas_get(ens_win->ee), DATADIR);   
   evas_object_text_font_set(ecore_evas_get(ens_win->ee), "VeraMono", 10);
   edje_object_file_set(ens_win->menus, DATADIR"menus.edj", "menus");
   evas_object_move(ens_win->menus, 0, 0);
   evas_object_resize(ens_win->menus, 640, 30);
   evas_object_layer_set(ens_win->menus, 3);
   edje_object_signal_callback_add(ens_win->menus, "ens,quit", "*", ens_cb_menu_quit, NULL);
   evas_object_show(ens_win->menus);
 
   return ens_win;
}

/* add a new tab with its text object */
/* TODO: split this into 2 function that add an empty tab, and later
 * we can add whatever we want to the tab, how about a terminal? (=
 */
void
ens_window_tab_add(Ens_Window *ens_win) {
   Evas_Object *ta;
  
   ta = esmart_textarea_add(ecore_evas_get(ens_win->ee));
   evas_object_show(ta);
   evas_object_move(ta, 0, 30);
   evas_object_resize(ta, 640, 480 - 30);
   ens_win = evas_list_append(ens_win->tabs, ta);

}
