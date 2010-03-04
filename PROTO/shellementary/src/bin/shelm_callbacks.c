#include "shelm.h"

SH_API void
destroy(void *data, Evas_Object *obj, void *event_info)
{
  elm_exit();
}

SH_API void
cancel_callback(void *data, Evas_Object *obj, void *event_info)
{
  arguments->quit_cancel_bool = EINA_TRUE;
  elm_exit();
}

SH_API void
slider_callback(void *data, Evas_Object *obj, void *event_info)
{
  Evas_Object *slider = data;
  double value;

  value = elm_slider_value_get(slider);

  printf("%f\n", value);
  elm_exit();
}

SH_API void
clock_callback(void *data, Evas_Object *obj, void *event_info)
{
  Evas_Object *clock = data;
  char value[PATH_MAX];
  int h, m, s;
  elm_clock_time_get(clock, &h, &m, &s);
  snprintf(value, sizeof(value), "%i:%i:%i", h, m, s);

  printf("%s\n", value);
  elm_exit();
}

SH_API void
entry_callback(void *data, Evas_Object *obj, void *event_info)
{
  Evas_Object *entry = data;

  if (elm_entry_entry_get(entry))
    {
      char *s;
      s = elm_entry_markup_to_utf8(elm_entry_entry_get(entry));
      printf(s);
      E_FREE(s);
    }
  elm_exit();
}

SH_API void
list_dialog_callback(void *data, Evas_Object *obj, void *event_info)
{
  Evas_Object *list = data;
  Elm_List_Item *listitem;
  const char *item_selected;

  listitem = elm_list_selected_item_get(list);
  item_selected = elm_list_item_label_get(listitem);
  
  printf("%s\n", item_selected);
  elm_exit();
}

static Eina_Bool _stdin_prepare(Ecore_Fd_Handler *fd_handler)
{
   int flags;

   if (ecore_main_fd_handler_active_get(fd_handler, ECORE_FD_ERROR))
     {
       perror(_("fd error, exit."));
       ecore_main_loop_quit();
       return EINA_FALSE;
     }
       else if (!ecore_main_fd_handler_active_get(fd_handler, ECORE_FD_READ)) {
       perror(_("nothing to read?"));
       return EINA_FALSE;
     }
   // nonblocking :)
   flags = fcntl(0, F_GETFL, 0);
   flags |= O_NONBLOCK;
   fcntl(0, F_SETFL, flags);

   return EINA_TRUE;
}

SH_API int _read_stdin_entry(void *data, Ecore_Fd_Handler *fd_handler)
{
   char *buffer;
   char *s;
   char c;
   int i = 0;
   Evas_Object *obj = data;

   if (!_stdin_prepare(fd_handler)) return 0;

   // allocate initial buffer
   buffer = malloc(sizeof(char));

   // get the buffer
   do {
     c = getc(stdin);
     buffer[i] = c;
     buffer = realloc(buffer, i + sizeof(buffer));
     i++;
   } while (c != EOF);

   // terminate the string.
   buffer[i - 1] = '\0';

   // add into entry, but before convert to html markup
   s = elm_entry_utf8_to_markup(buffer);
   elm_entry_entry_set(obj, s);

   // free the the pointers, delete handler when we don't need it
   E_FREE(buffer);
   E_FREE(s);
   ecore_main_fd_handler_del(fd_handler);
   return 0;
}

SH_API int _read_stdin_list(void *data, Ecore_Fd_Handler *fd_handler)
{
   char **splitted;
   char *buffer;
   char c;
   int i = 0;
   int len = 0;
   Evas_Object *obj = data;

   if (!_stdin_prepare(fd_handler)) return 0;

   // allocate initial buffer
   buffer = malloc(sizeof(char));

   // get the buffer
   do {
     c = getc(stdin);
     buffer[i] = c;
     buffer = realloc(buffer, i + sizeof(buffer));
     i++;
   } while (c != EOF);

   // terminate the string.
   buffer[i - 1] = '\0';

   // split and append
   splitted = eina_str_split_full(buffer, "\n", 0, &len);

   for (i = 0; i < len; i++)
     elm_list_item_append(obj, splitted[i], NULL, NULL, NULL, NULL);

   elm_list_item_del(eina_list_data_get(eina_list_last(elm_list_items_get(obj))));

   elm_list_go(obj);

   // free the the pointers, delete handler when we don't need it
   E_FREE(splitted);
   E_FREE(buffer);
   ecore_main_fd_handler_del(fd_handler);
   return 0;

}
