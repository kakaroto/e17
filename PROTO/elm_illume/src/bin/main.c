#include "elm_illume.h"
#include <Ecore_X.h>

#ifndef ELM_LIB_QUICKLAUNCH

static void _cb_win_del(void *data, Evas_Object *obj, void *event);
static void _cb_btn_clicked(void *data, Evas_Object *obj, void *event);

static Evas_Object *win = NULL;

EAPI int 
elm_main(int argc, char **argv) 
{
   Evas_Object *bg, *box, *btn;
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

   box = elm_box_add(win);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, box);
   evas_object_show(box);

   btn = elm_button_add(win);
   elm_button_label_set(btn, "Click to close");
   elm_object_style_set(btn, "anchor");
   evas_object_smart_callback_add(btn, "clicked", _cb_btn_clicked, win);
   elm_box_pack_end(box, btn);
   evas_object_show(btn);

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

static void 
_cb_btn_clicked(void *data, Evas_Object *obj, void *event) 
{
   Evas_Object *win;

   if (!(win = data)) return;
   evas_object_del(win);
}

#endif
ELM_MAIN();
