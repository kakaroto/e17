#include <Ecore_X.h>
#include "elm_indicator.h"

#ifndef ELM_LIB_QUICKLAUNCH

static void _cb_win_del(void *data, Evas_Object *obj, void *event);
static void _cb_btn_home_clicked(void *data, Evas_Object *obj, void *event);
static void _cb_btn_dual_clicked(void *data, Evas_Object *obj, void *event);
//static void _cb_btn_kbd_clicked(void *data, Evas_Object *obj, void *event);
static void _cb_home_win_del(void *data, Evas_Object *obj, void *event);
static char *_desk_gl_label_get(const void *data, Evas_Object *obj, const char *part);
static Evas_Object *_desk_gl_icon_get(const void *data, Evas_Object *obj, const char *part);
static void _desk_gl_del(const void *data, Evas_Object *obj);

static Evas_Object *win = NULL;
static Evas_Object *hwin = NULL;
static Elm_Genlist_Item_Class it_desk;

EAPI int 
elm_main(int argc, char **argv) 
{
   Evas_Object *bg, *box, *btn, *icon;
   Evas_Object *clock;
   Ecore_X_Window xwin;
   char buff[PATH_MAX];

   win = elm_win_add(NULL, "elm_indicator", ELM_WIN_DOCK);
   elm_win_title_set(win, "Illume Indicator Window");
   evas_object_smart_callback_add(win, "delete-request", _cb_win_del, NULL);

   xwin = elm_win_xwindow_get(win);
   ecore_x_icccm_hints_set(xwin, 0, 0, 0, 0, 0, 0, 0);

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

   /*
   icon = elm_icon_add(win);
   snprintf(buff, sizeof(buff), "%s/images/kbd-off.png", PACKAGE_DATA_DIR);
   elm_icon_file_set(icon, buff, NULL);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

   btn = elm_button_add(win);
   elm_button_icon_set(btn, icon);
   evas_object_smart_callback_add(btn, "clicked", _cb_btn_kbd_clicked, win);
   evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(box, btn);
   evas_object_show(btn);
   evas_object_show(icon);
    */

   clock = elm_clock_add(win);
   printf("Scale: %2.2f\n", elm_object_scale_get(clock));
   elm_clock_show_seconds_set(clock, 0);
   elm_clock_show_am_pm_set(clock, 1);
   elm_object_scale_set(clock, 0.25);
   elm_box_pack_end(box, clock);
   evas_object_show(clock);
   printf("Scale: %2.2f\n", elm_object_scale_get(clock));

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
   Evas_Object *bg, *gl;
   Eina_List *desktops, *l;

   if (hwin) 
     {
        elm_win_activate(hwin);
        elm_win_raise(hwin);
        return;
     }

   hwin = elm_win_add(NULL, "elm_indicator_home", ELM_WIN_BASIC);
   elm_win_title_set(hwin, "Illume Indicator Home Window");
   evas_object_smart_callback_add(hwin, "delete-request", _cb_home_win_del, NULL);

   bg = elm_bg_add(hwin);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(hwin, bg);
   evas_object_show(bg);

   it_desk.item_style = "default";
   it_desk.func.label_get = _desk_gl_label_get;
   it_desk.func.icon_get = _desk_gl_icon_get;
   it_desk.func.state_get = NULL;
   it_desk.func.del = _desk_gl_del;

   gl = elm_genlist_add(hwin);
   evas_object_size_hint_weight_set(gl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(hwin, gl);
   evas_object_show(gl);

# ifdef ELM_EFREET
   elm_need_efreet();
   desktops = efreet_util_desktop_name_glob_list("*");
   if (desktops) 
     {
        Efreet_Desktop *d;

        EINA_LIST_FOREACH(desktops, l, d) 
          {
             elm_genlist_item_append(gl, &it_desk, d, NULL, ELM_GENLIST_ITEM_NONE, 
                                     NULL, NULL);
          }
        eina_list_free(desktops);
     }
# endif

   evas_object_resize(hwin, 200, 200);
   evas_object_show(hwin);
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
   /* we do a mode set on the xwindow so that illume module can read the current 
    * setting and adjust accordingly */
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

/*
static void 
_cb_btn_kbd_clicked(void *data, Evas_Object *obj, void *event) 
{
   Evas_Object *win;
   Ecore_X_Window xwin;
   Ecore_X_Virtual_Keyboard_State state;
   Evas_Object *icon;
   char buff[PATH_MAX];

   win = data;
   xwin = elm_win_xwindow_get(win);
   state = ecore_x_e_virtual_keyboard_state_get(xwin);
   if ((state == ECORE_X_VIRTUAL_KEYBOARD_STATE_OFF) || 
       (state == ECORE_X_VIRTUAL_KEYBOARD_STATE_UNKNOWN)) 
     {
        ecore_x_e_virtual_keyboard_state_set(xwin, 
                                             ECORE_X_VIRTUAL_KEYBOARD_STATE_ON);
        snprintf(buff, sizeof(buff), "%s/images/kbd-off.png", PACKAGE_DATA_DIR);
     }
   else 
     {
        ecore_x_e_virtual_keyboard_state_set(xwin, 
                                             ECORE_X_VIRTUAL_KEYBOARD_STATE_OFF);
        snprintf(buff, sizeof(buff), "%s/images/kbd-on.png", PACKAGE_DATA_DIR);
     }

   icon = elm_icon_add(win);
   elm_icon_file_set(icon, buff, NULL);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_button_icon_set(obj, icon);
}
*/

static void 
_cb_home_win_del(void *data, Evas_Object *obj, void *event) 
{
   if (hwin) evas_object_del(hwin);
   hwin = NULL;
}

static char *
_desk_gl_label_get(const void *data, Evas_Object *obj, const char *part) 
{
# ifdef ELM_EFREET
   Efreet_Desktop *d = (Efreet_Desktop *)data;
   return strdup(d->name);
#else
   return NULL;
#endif
}

static Evas_Object *
_desk_gl_icon_get(const void *data, Evas_Object *obj, const char *part) 
{
#ifdef ELM_EFREET
   Efreet_Desktop *d = (Efreet_Desktop *)data;
   char *path;
   Evas_Object *ic;

   ic = elm_icon_add(obj);
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   if (!(!strcmp(part, "elm.swallow.icon"))) return ic;
   if (!d->icon) return ic;
   path = efreet_icon_path_find("default", d->icon, 48);
   if (!path) 
     {
        path = efreet_icon_path_find("hicolor", d->icon, 48);
        if (!path) 
          {
             path = efreet_icon_path_find("gnome", d->icon, 48);
             if (!path)
               path = efreet_icon_path_find("Human", d->icon, 48);
          }
     }
   if (path) 
     {
        elm_icon_file_set(ic, path, NULL);
        free(path);
        return ic;
     }
   return ic;
#else
   return NULL;
#endif
}

static void 
_desk_gl_del(const void *data, Evas_Object *obj) 
{
#ifdef ELM_EFREET
   Efreet_Desktop *d = (Efreet_Desktop *)data;
   efreet_desktop_free(d);
#endif
}

#endif
ELM_MAIN();
