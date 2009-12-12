#include <Ecore_X.h>
#include "elm_illume.h"

#ifndef ELM_LIB_QUICKLAUNCH

static void _cb_win_del(void *data, Evas_Object *obj, void *event);
static void _cb_btn_home_clicked(void *data, Evas_Object *obj, void *event);
static void _cb_btn_close_clicked(void *data, Evas_Object *obj, void *event);

static Evas_Object *win = NULL;

EAPI int 
elm_main(int argc, char **argv) 
{
   Evas_Object *btn, *layout, *bx, *icon;
   Ecore_X_Window xwin;
   char buff[PATH_MAX];

   win = elm_win_add(NULL, "elm_illume", ELM_WIN_BASIC);
   elm_win_title_set(win, "Illume Conformant Window");
   evas_object_smart_callback_add(win, "delete-request", _cb_win_del, NULL);
   xwin = elm_win_xwindow_get(win);
   ecore_x_e_illume_conformant_set(xwin, 1);

   layout = elm_layout_add(win);
   snprintf(buff, sizeof(buff), "%s/themes/default.edj", PACKAGE_DATA_DIR);
   elm_layout_file_set(layout, buff, "layout");
   evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, layout);
   evas_object_show(layout);

   bx = edje_object_add(evas_object_evas_get(win));
   edje_object_file_set(bx, buff, "indicator");
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_layout_content_set(layout, "indicator", bx);
   evas_object_show(bx);

   icon = elm_icon_add(win);
   elm_icon_file_set(icon, buff, "home");
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

   btn = elm_button_add(win);
   elm_button_icon_set(btn, icon);
   evas_object_smart_callback_add(btn, "clicked", _cb_btn_home_clicked, win);
   edje_object_part_swallow(bx, "button", btn);
   evas_object_show(btn);
   evas_object_show(icon);

   btn = elm_label_add(win);
   elm_label_label_set(btn, "Window Content");
   elm_layout_content_set(layout, "content", btn);
   evas_object_show(btn);

   bx = edje_object_add(evas_object_evas_get(win));
   edje_object_file_set(bx, buff, "softkey");
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_layout_content_set(layout, "softkey", bx);
   evas_object_show(bx);

   icon = elm_icon_add(win);
   elm_icon_standard_set(icon, "close");
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

   btn = elm_button_add(win);
   elm_button_icon_set(btn, icon);
   evas_object_smart_callback_add(btn, "clicked", _cb_btn_close_clicked, win);
   edje_object_part_swallow(bx, "button", btn);
   evas_object_show(btn);
   evas_object_show(icon);

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
_cb_btn_home_clicked(void *data, Evas_Object *obj, void *event) 
{

}

static void 
_cb_btn_close_clicked(void *data, Evas_Object *obj, void *event) 
{

}

#endif
ELM_MAIN();
