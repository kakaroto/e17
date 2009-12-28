#include <Ecore_X.h>
#include "elm_quickpanel.h"

#ifndef ELM_LIB_QUICKLAUNCH

/* local function prototypes */
static void _cb_win_del(void *data, Evas_Object *obj, void *event);

EAPI int 
elm_main(int argc, char **argv) 
{
   int i = 0;

   for (i = 0; i < 2; i++) 
     {
        Evas_Object *win, *bg, *lbl;
        Ecore_X_Window xwin;
        Ecore_X_Window_State states[2];
        char buff[PATH_MAX];

        win = elm_win_add(NULL, "elm_quickpanel", ELM_WIN_BASIC);
        elm_win_title_set(win, "Illume Quickpanel Window");
        elm_win_autodel_set(win, EINA_TRUE);
//        elm_win_borderless_set(win, EINA_TRUE);

        xwin = elm_win_xwindow_get(win);
        /* set hints to skip taskbar & pager */
        states[0] = ECORE_X_WINDOW_STATE_SKIP_TASKBAR;
        states[1] = ECORE_X_WINDOW_STATE_SKIP_PAGER;
        ecore_x_netwm_window_state_set(xwin, states, 2);
        /* set hints to not take focus */
        ecore_x_icccm_hints_set(xwin, 0, 0, 0, 0, 0, 0, 0);
        /* set this window as a quickpanel */
        ecore_x_e_illume_quickpanel_set(xwin, 1);

        bg = elm_bg_add(win);
        evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_win_resize_object_add(win, bg);
        evas_object_show(bg);

        snprintf(buff, sizeof(buff), "This is quickpanel %d", i);
        lbl = elm_label_add(win);
        elm_label_label_set(lbl, buff);
        evas_object_size_hint_weight_set(lbl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_win_resize_object_add(win, lbl);
        evas_object_show(lbl);

        evas_object_size_hint_min_set(win, 100, 32);
        evas_object_resize(win, 100, 32);
        evas_object_show(win);
     }

   elm_run();
   elm_shutdown();
   return 0;
}

#endif
ELM_MAIN();
