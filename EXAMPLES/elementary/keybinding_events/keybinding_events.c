#include <Elementary.h>

static Eina_Bool
_input_event_cb(void *data, int type, void *event)
{
   printf("_input_event_cb: ");
   if (type == ECORE_EVENT_KEY_DOWN) {
        printf("EVAS_CALLBACK_KEY_DOWN");
   } else if (type == ECORE_EVENT_KEY_UP) {
        printf("EVAS_CALLBACK_KEY_UP");
   } else {
        printf("...");
   }
   printf("\n");
   return ECORE_CALLBACK_RENEW;
}

EAPI int
elm_main(int argc, char **argv)
{
   Evas_Object *win, *o, *bg, *box;
   Ecore_Event_Handler *handle_key_up, *handle_key_down;

   win = elm_win_add(NULL, "elm_event", ELM_WIN_BASIC);
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
   elm_win_autodel_set(win, EINA_TRUE);

   o = elm_bg_add(win);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, o);
   evas_object_show(o);

   box = elm_box_add(win);
   elm_box_horizontal_set(box, EINA_FALSE);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, box);
   evas_object_show(box);

   o = elm_entry_add(win);
   elm_entry_single_line_set(o, EINA_TRUE);
   elm_entry_entry_set(o, "123ab456");
   elm_box_pack_end(box, o);
   evas_object_show(o);

   o = elm_button_add(win);
   elm_object_text_set(o, "Test");
   elm_box_pack_end(box, o);
   evas_object_show(o);

   handle_key_up = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN,
                                           _input_event_cb, NULL);
   handle_key_down = ecore_event_handler_add(ECORE_EVENT_KEY_UP,
                                             _input_event_cb, NULL);

   evas_object_resize(win, 300, 300);
   evas_object_show(win);

   elm_run();

   if (handle_key_up)
     ecore_event_handler_del(handle_key_up);
   if (handle_key_down)
     ecore_event_handler_del(handle_key_down);

   elm_shutdown();

   return 0;
}
ELM_MAIN()
