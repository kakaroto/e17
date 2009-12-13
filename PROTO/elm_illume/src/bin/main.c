#include <Ecore_X.h>
#include "elm_illume.h"

#ifndef ELM_LIB_QUICKLAUNCH

static void _cb_win_del(void *data, Evas_Object *obj, void *event);

static Evas_Object *win = NULL;

EAPI int 
elm_main(int argc, char **argv) 
{
   Evas_Object *bg;
   Ecore_X_Window xwin;

   win = elm_win_add(NULL, "elm_illume", ELM_WIN_BASIC);
   elm_win_title_set(win, "Illume Conformant Window");
   evas_object_smart_callback_add(win, "delete-request", _cb_win_del, NULL);
   xwin = elm_win_xwindow_get(win);
   ecore_x_e_illume_conformant_set(xwin, 1);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   evas_object_resize(win, 200, 200);
   evas_object_show(win);

   elm_run();
   elm_shutdown();
   return 0;
}

static void 
_cb_win_del(void *data, Evas_Object *obj, void *event) 
{
   elm_exit();
}

#endif
ELM_MAIN();
