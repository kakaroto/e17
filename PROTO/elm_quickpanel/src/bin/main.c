#include "elm_quickpanel.h"

#ifndef ELM_LIB_QUICKLAUNCH

EAPI int 
elm_main(int argc, char **argv) 
{
   int i = 0;

   for (i = 0; i < 3; i++) 
     {
        Evas_Object *win, *bg, *lbl, *box;
        Ecore_X_Window xwin;
        Ecore_X_Window_State states[2];
        char buff[PATH_MAX];

        snprintf(buff, sizeof(buff), "elm_quickpanel:%d", i);

        win = elm_win_add(NULL, buff, ELM_WIN_BASIC);
        elm_win_title_set(win, "Illume Quickpanel Window");
        elm_win_autodel_set(win, EINA_TRUE);
        elm_win_quickpanel_set(win, 1);

        /* set quickpanel priority */
        if (i == 1) 
          {
             elm_win_quickpanel_priority_major_set(win, 2);
             elm_win_quickpanel_priority_minor_set(win, 3);
          }
        else 
          {
             elm_win_quickpanel_priority_major_set(win, i);
             elm_win_quickpanel_priority_minor_set(win, i);
          }

        bg = elm_bg_add(win);
        evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_win_resize_object_add(win, bg);
        evas_object_show(bg);

        box = elm_box_add(win);
        evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_win_resize_object_add(win, box);
        evas_object_show(box);

        if (i == 1) 
          {
             snprintf(buff, sizeof(buff), 
                      "This is quickpanel %d - Priority: Major %d Minor %d", 
                      i, 2, 3);
          }
        else 
          {
             snprintf(buff, sizeof(buff), 
                      "This is quickpanel %d - Priority: Major %d Minor %d", 
                      i, i, i);
          }

        lbl = elm_button_add(win);
        elm_button_label_set(lbl, buff);
        elm_box_pack_end(box, lbl);
        evas_object_show(lbl);

        evas_object_size_hint_min_set(win, 100, 32);
        evas_object_resize(win, 100, 32);
        evas_object_show(win);

        if (i == 0) elm_win_quickpanel_zone_set(win, 1);
        else elm_win_quickpanel_zone_set(win, 0);
     }

   elm_run();
   elm_shutdown();
   return 0;
}

#endif
ELM_MAIN();
