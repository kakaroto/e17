#include <Elementary.h>

static void
on_win_del_req(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

// sms main window - setup
static void
create_main_win(void)
{
   Evas_Object *win, *bg;

   win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_title_set(win, "SMS");
   evas_object_smart_callback_add(win, "delete-request", on_win_del_req, NULL);
   
   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, 1.0, 1.0);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);
   
   evas_object_show(win);
}

int
main(int argc, char **argv)
{
   int i;
   
   elm_init(argc, argv);
   for (i = 1; i < argc; i++)
     {
	// parse args
     }
   create_main_win();
   elm_run();
   elm_shutdown();
   return 0; 
}
