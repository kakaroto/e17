#include "main.h"

Evas_Object *main_win;

static void
es_window_delete(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

int
elm_main(int argc, char **argv)
{
   main_win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_title_set(main_win, basename(argv[0]));
   evas_object_focus_set(main_win, 1);
   evas_object_smart_callback_add(main_win, "delete,request", es_window_delete, NULL);
   evas_object_resize(main_win, 320, 480);
   Evas_Object *bg = elm_bg_add(main_win);
   elm_win_resize_object_add(main_win, bg);
   evas_object_show(bg);

   evas_object_show(main_win);
   elm_run();

   return 0;
}

ELM_MAIN()
