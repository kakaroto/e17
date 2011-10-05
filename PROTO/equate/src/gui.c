#include "Equate.h"

int inited = 0;

Evas_Object *win;
Evas_Object *ly;

void
equate_init_gui(Equate * equate, int argc, char **argv)
{
   char buf[1024];
   char *pkgdatadir;

   if (inited == 1)
     return;
   if (equate)
     {
        win = elm_win_add(NULL, "equate", ELM_WIN_BASIC);
        elm_win_title_set(win, "Equate");
        elm_win_autodel_set(win, EINA_TRUE);
        elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);


        ly = elm_layout_add(win);
        pkgdatadir = getenv("EQUATE_PKGDATA_DIR");
        if (!pkgdatadir)
          pkgdatadir = PACKAGE_PKGDATA_DIR;
        snprintf(buf, sizeof(buf), "%s/themes/equate.edj", pkgdatadir);
        elm_layout_file_set(ly, buf, "Main");
        evas_object_size_hint_weight_set(ly, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(ly, EVAS_HINT_FILL, EVAS_HINT_FILL);
        elm_win_resize_object_add(win, ly);
        evas_object_show(ly);

        evas_object_show(win);
        evas_object_resize(win, 200, 296);

        equate_edje_callback_define(elm_layout_edje_get(ly));

        inited = 1;
     }
}

