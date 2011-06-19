#include "main.h"

Enlil_Win *
enlil_win_new()
{
   Evas_Object *win, *bg, *bx0;
   Enlil_Win *enlil_win;

   enlil_win = calloc(1, sizeof(Enlil_Win));

   win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_title_set(win, D_("Enki (Photo Manager)"));

   bg = elm_bg_add(win);
   enlil_win->bg = bg;
   evas_object_size_hint_weight_set(bg, 1.0, 1.0);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   bx0 = elm_box_add(win);
   evas_object_size_hint_weight_set(bx0, 1.0, 1.0);
   elm_win_resize_object_add(win, bx0);
   evas_object_show(bx0);

   enlil_win->win = win;

   return enlil_win;
}

void
enlil_win_bg_set(Enlil_Win *win, const char *file)
{
   Evas_Object *bg;

   ASSERT_RETURN_VOID(win != NULL);

   bg = elm_bg_add(win->win);
   evas_object_lower(bg);
   if (file) elm_bg_file_set(bg, file, NULL);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win->win, bg);
   evas_object_show(bg);

   evas_object_del(win->bg);
   win->bg = bg;
}

