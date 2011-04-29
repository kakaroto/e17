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

   elm_init(argc, argv);

   win = elm_win_add(NULL, "tuto", ELM_WIN_BASIC);
   elm_win_title_set(win, "Edje Tutorial");
   evas_object_smart_callback_add(win, "delete,request", _win_del, NULL);

   evas_object_resize(win, 800, 480);

   evas_object_show(win);

   elm_run();
   elm_shutdown();
}
