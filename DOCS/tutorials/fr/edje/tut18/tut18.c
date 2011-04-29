/* gcc -o test tut13.c `pkg-config elementary --cflags --libs` */

#include <Elementary.h>

static void
_win_del(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

int main(int argc, char **argv)
{
   Evas_Object *win;
   Evas_Object *layout;
   Evas_Object *table;

   elm_init(argc, argv);

   win = elm_win_add(NULL, "tuto", ELM_WIN_BASIC);
   elm_win_title_set(win, "Edje Tutorial");
   evas_object_smart_callback_add(win, "delete,request", _win_del, NULL);


   layout = elm_layout_add(win);
   elm_layout_file_set(layout, "tut19.edj", "interface");
   evas_object_show(layout);
   elm_win_resize_object_add(win, layout);

   table = elm_layout_add(layout);
   elm_layout_file_set(table, "tut19.edj", "table");
   evas_object_show(table);

   evas_object_size_hint_weight_set(layout, 1, 1);

   elm_layout_content_set(layout, "table_swallow", table);
   evas_object_size_hint_min_set(layout, 64, 64);

   evas_object_resize(win, 800, 480);
   evas_object_show(win);

   elm_run();
   elm_shutdown();
}
