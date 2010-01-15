#include <Ecore_X.h>
#include "elm_illume.h"

#ifndef ELM_LIB_QUICKLAUNCH

static void _cb_win_del(void *data, Evas_Object *obj, void *event);
static void _cb_btn_close_clicked(void *data, Evas_Object *obj, void *event);

static Evas_Object *win = NULL;

EAPI int 
elm_main(int argc, char **argv) 
{
   Evas_Object *bg, *conform, *btn;

   win = elm_win_add(NULL, "elm_illume", ELM_WIN_BASIC);
   elm_win_title_set(win, "Illume Conformant Window");
   elm_win_conformant_set(win, 1);
   evas_object_smart_callback_add(win, "delete-request", _cb_win_del, NULL);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   conform = elm_conformant_add(win);
   elm_win_resize_object_add(win, conform);
   evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(conform);

   btn = elm_button_add(win);
   elm_button_label_set(btn, "Click to Close");
   evas_object_smart_callback_add(btn, "clicked", _cb_btn_close_clicked, NULL);
   evas_object_show(btn);

   elm_conformant_content_set(conform, btn);

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
_cb_btn_close_clicked(void *data, Evas_Object *obj, void *event) 
{
   elm_exit();
}

#endif
ELM_MAIN();
