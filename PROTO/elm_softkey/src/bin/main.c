#include "elm_softkey.h"

#ifndef ELM_LIB_QUICKLAUNCH

/* local function prototypes */
static void _cb_win_del(void *data, Evas_Object *obj, void *event);
static void _cb_btn_close_clicked(void *data, Evas_Object *obj, void *event);
static void _cb_btn_back_clicked(void *data, Evas_Object *obj, void *event);

/* local variables */

EAPI int 
elm_main(int argc, char **argv) 
{
   Ecore_X_Window *roots = NULL;
   int num = 0, i = 0;

   roots = ecore_x_window_root_list(&num);
   if ((!roots) || (num <= 0)) return EXIT_FAILURE;

   for (i = 0; i < num; i++) 
     {
        Ecore_X_Window *zones;
        int x = 0, count = 0;

        count = 
          ecore_x_window_prop_window_list_get(roots[i], 
                                         ECORE_X_ATOM_E_ILLUME_ZONE_LIST, 
                                         &zones);
        if (!zones) continue;
        for (x = 0; x < count; x++) 
          {
             Evas_Object *win;
             Evas_Object *bg, *box, *btn, *icon;
             Ecore_X_Window xwin;
             Ecore_X_Window_State states[2];
             char buff[PATH_MAX];
             int zx, zy, zw, zh;

             /* create new window */
             win = elm_win_add(NULL, "Illume-Softkey", ELM_WIN_DOCK);
             elm_win_title_set(win, "Illume Softkey");
             evas_object_smart_callback_add(win, "delete-request", 
                                            _cb_win_del, NULL);
             evas_object_data_set(win, "zone", (const void *)zones[x]);

             xwin = elm_win_xwindow_get(win);
             ecore_x_icccm_hints_set(xwin, 0, 0, 0, 0, 0, 0, 0);
             states[0] = ECORE_X_WINDOW_STATE_SKIP_TASKBAR;
             states[1] = ECORE_X_WINDOW_STATE_SKIP_PAGER;
             ecore_x_netwm_window_state_set(xwin, states, 2);

             bg = elm_bg_add(win);
             evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, 
                                              EVAS_HINT_EXPAND);
             elm_win_resize_object_add(win, bg);
             evas_object_show(bg);

             box = elm_box_add(win);
             elm_box_horizontal_set(box, EINA_TRUE);
             evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, 
                                              EVAS_HINT_EXPAND);
             elm_win_resize_object_add(win, box);
             evas_object_show(box);

             icon = elm_icon_add(win);
             snprintf(buff, sizeof(buff), "%s/images/back.png", 
                      PACKAGE_DATA_DIR);
             elm_icon_file_set(icon, buff, NULL);
             evas_object_size_hint_aspect_set(icon, 
                                              EVAS_ASPECT_CONTROL_VERTICAL, 
                                              1, 1);

             btn = elm_button_add(win);
             elm_button_icon_set(btn, icon);
             evas_object_smart_callback_add(btn, "clicked", 
                                            _cb_btn_back_clicked, win);
             evas_object_size_hint_align_set(btn, 1.0, 0.5);
             elm_box_pack_end(box, btn);
             evas_object_show(btn);
             evas_object_show(icon);

             icon = elm_icon_add(win);
             snprintf(buff, sizeof(buff), "%s/images/close.png", 
                      PACKAGE_DATA_DIR);
             elm_icon_file_set(icon, buff, NULL);
             evas_object_size_hint_aspect_set(icon, 
                                              EVAS_ASPECT_CONTROL_VERTICAL, 
                                              1, 1);

             btn = elm_button_add(win);
             elm_button_icon_set(btn, icon);
             evas_object_smart_callback_add(btn, "clicked", 
                                            _cb_btn_close_clicked, win);
             evas_object_size_hint_align_set(btn, 1.0, 0.5);
             elm_box_pack_end(box, btn);
             evas_object_show(btn);
             evas_object_show(icon);

             ecore_x_window_geometry_get(zones[x], &zx, &zy, &zw, &zh);
             ecore_x_e_illume_softkey_geometry_set(zones[x], zx, 
                                                   (zy + zh - 32), zw, 32);

             evas_object_move(win, zx, (zy + zh - 32));
             evas_object_resize(win, zw, 32);
             evas_object_show(win);
          }
        free(zones);
     }
   free(roots);

   elm_run();

   elm_shutdown();
   return EXIT_SUCCESS;
}

static void 
_cb_win_del(void *data, Evas_Object *obj, void *event) 
{
   Ecore_X_Window zone;

   zone = (Ecore_X_Window)evas_object_data_get(obj, "zone");
   ecore_x_e_illume_softkey_geometry_set(zone, 0, 0, 0, 0);

   elm_exit();
}

static void 
_cb_btn_close_clicked(void *data, Evas_Object *obj, void *event) 
{
   Evas_Object *win;
   Ecore_X_Window xwin;

   if (!(win = data)) return;
   xwin = elm_win_xwindow_get(win);
   ecore_x_e_illume_close_send(xwin);
}

static void 
_cb_btn_back_clicked(void *data, Evas_Object *obj, void *event) 
{
   Evas_Object *win;
   Ecore_X_Window zone;

   if (!(win = data)) return;
   zone = (Ecore_X_Window)evas_object_data_get(win, "zone");
   ecore_x_e_illume_focus_back_send(zone);
}

#endif
ELM_MAIN();
