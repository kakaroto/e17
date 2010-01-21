#include <Ecore_X.h>
#include <Ecore_X_Atoms.h>
#include "elm_quickpanel.h"

#ifndef ELM_LIB_QUICKLAUNCH

EAPI int 
elm_main(int argc, char **argv) 
{
   Ecore_X_Window *zones;
   int i = 0, zone_count = 0;

   zone_count = 
     ecore_x_window_prop_window_list_get(ecore_x_window_root_first_get(), 
                                         ECORE_X_ATOM_E_ILLUME_ZONE_LIST, &zones);

   for (i = 0; i < 3; i++) 
     {
        Evas_Object *win, *bg, *lbl;
        Ecore_X_Window xwin;
        Ecore_X_Window_State states[2];
        char buff[PATH_MAX];

        snprintf(buff, sizeof(buff), "elm_quickpanel:%d", i);

        win = elm_win_add(NULL, buff, ELM_WIN_BASIC);
        elm_win_title_set(win, "Illume Quickpanel Window");
        elm_win_autodel_set(win, EINA_TRUE);

        xwin = elm_win_xwindow_get(win);
        /* set hints to skip taskbar & pager */
        states[0] = ECORE_X_WINDOW_STATE_SKIP_TASKBAR;
        states[1] = ECORE_X_WINDOW_STATE_SKIP_PAGER;
        ecore_x_netwm_window_state_set(xwin, states, 2);
        /* set hints to not take focus */
        ecore_x_icccm_hints_set(xwin, 0, 0, 0, 0, 0, 0, 0);
        /* set this window as a quickpanel */
        ecore_x_e_illume_quickpanel_set(xwin, 1);

        /* set quickpanel priority */
        if (i == 1) 
          {
             ecore_x_e_illume_quickpanel_priority_major_set(xwin, 2);
             ecore_x_e_illume_quickpanel_priority_minor_set(xwin, 3);
          }
        else 
          {
             ecore_x_e_illume_quickpanel_priority_major_set(xwin, i);
             ecore_x_e_illume_quickpanel_priority_minor_set(xwin, i);
          }

        bg = elm_bg_add(win);
        evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_win_resize_object_add(win, bg);
        evas_object_show(bg);

        snprintf(buff, sizeof(buff), 
                 "This is quickpanel %d - Priority: Major %d Minor %d", i, 
                 ecore_x_e_illume_quickpanel_priority_major_get(xwin), 
                 ecore_x_e_illume_quickpanel_priority_minor_get(xwin));
        lbl = elm_label_add(win);
        elm_label_label_set(lbl, buff);
        evas_object_size_hint_weight_set(lbl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_win_resize_object_add(win, lbl);
        evas_object_show(lbl);

        evas_object_size_hint_min_set(win, 100, 32);
        evas_object_resize(win, 100, 32);
        evas_object_show(win);

        if (i == 0) 
          {
             if ((zones) && (zone_count > 1)) 
               {
                  xwin = elm_win_xwindow_get(win);
                  ecore_x_e_illume_quickpanel_zone_set(xwin, &zones[1]);
                  ecore_x_e_illume_quickpanel_zone_request_send(ecore_x_window_root_first_get(), xwin);
                  free(zones);
               }
          }
     }

   elm_run();
   elm_shutdown();
   return 0;
}

#endif
ELM_MAIN();
