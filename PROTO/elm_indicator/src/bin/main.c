#include <Ecore_X.h>
#include "elm_indicator.h"

#ifndef ELM_LIB_QUICKLAUNCH

static void _cb_win_del(void *data, Evas_Object *obj, void *event);
static void _cb_btn_home_clicked(void *data, Evas_Object *obj, void *event);
static void _cb_btn_dual_clicked(void *data, Evas_Object *obj, void *event);

static Evas_Object *win = NULL;

EAPI int 
elm_main(int argc, char **argv) 
{
   Evas_Object *bg, *box, *btn, *icon;
   char buff[PATH_MAX];

   win = elm_win_add(NULL, "elm_indicator", ELM_WIN_DOCK);
   elm_win_title_set(win, "Illume Indicator Window");
   evas_object_smart_callback_add(win, "delete-request", _cb_win_del, NULL);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   box = elm_box_add(win);
   elm_box_horizontal_set(box, EINA_TRUE);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, box);
   evas_object_show(box);

   icon = elm_icon_add(win);
   snprintf(buff, sizeof(buff), "%s/images/home.png", PACKAGE_DATA_DIR);
   elm_icon_file_set(icon, buff, NULL);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

   btn = elm_button_add(win);
   elm_button_icon_set(btn, icon);
   evas_object_smart_callback_add(btn, "clicked", _cb_btn_home_clicked, NULL);
   evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(box, btn);
   evas_object_show(btn);
   evas_object_show(icon);

   icon = elm_icon_add(win);
   snprintf(buff, sizeof(buff), "%s/images/dual-mode.png", PACKAGE_DATA_DIR);
   elm_icon_file_set(icon, buff, NULL);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

   btn = elm_button_add(win);
   elm_button_icon_set(btn, icon);
   evas_object_smart_callback_add(btn, "clicked", _cb_btn_dual_clicked, win);
   evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(box, btn);
   evas_object_show(btn);
   evas_object_show(icon);

   evas_object_resize(win, 200, 32);
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
_cb_btn_dual_clicked(void *data, Evas_Object *obj, void *event) 
{
   Evas_Object *win;
   Ecore_X_Window xwin;
   Ecore_X_Illume_Mode mode;

   win = data;
   xwin = elm_win_xwindow_get(win);
   mode = ecore_x_e_illume_mode_get(xwin);
   printf("Ind Mode: %d\n", mode);
   if (mode == ECORE_X_ILLUME_MODE_SINGLE) 
     {
        ecore_x_e_illume_mode_set(xwin, ECORE_X_ILLUME_MODE_DUAL);
        ecore_x_e_illume_mode_send(xwin, ECORE_X_ILLUME_MODE_DUAL);
     }
   else 
     {
        ecore_x_e_illume_mode_set(xwin, ECORE_X_ILLUME_MODE_SINGLE);
        ecore_x_e_illume_mode_send(xwin, ECORE_X_ILLUME_MODE_SINGLE);
     }
}

#endif
ELM_MAIN();
