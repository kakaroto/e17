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
   char c;
   char *s;
   int i = 0;
   Evas_Object *obj = data;
   Eina_List *in = NULL;

   if (!_stdin_prepare(fd_handler)) return 0;

   // get needed things
   do {
     c = getc(stdin);
     in = eina_list_append(in, c);
   } while (c != EOF);

   // free the variable for now
   E_FREE(c);
   eina_list_remove_list(in, eina_list_last(in));

   // initialize buffer of size we need
   char buf[eina_list_count(in)];

   // load from list.
   EINA_LIST_FREE(in, c)
     {
        buf[i] = c;
        i++;
     }

   s = elm_entry_utf8_to_markup(buf);
   elm_entry_entry_set(obj, s);
   E_FREE(s);
   E_FREE(c);

   // as we already read stdin, we dont need it anymore
   eina_list_free(in);
   ecore_main_fd_handler_del(fd_handler);
   return 0;
}

SH_API int _read_stdin_list(void *data, Ecore_Fd_Handler *fd_handler)
{
   char c;
   char *s;
   int i = 0;
   Evas_Object *obj = data;
   Eina_List *in = NULL;
 
   do {
     c = getc(stdin);
     in = eina_list_append(in, c);
   } while (c != EOF);

   E_FREE(c);

   char buf[eina_list_count(in)];

   EINA_LIST_FREE(in, c)
     {
       buf[i] = c;
       i++;
     }

   s = strtok(buf, "\n");
   while (s != NULL)
     {
       elm_list_item_append(obj, s, NULL, NULL, NULL, NULL);
       s = strtok(NULL, "\n");
     }
   elm_list_item_del(eina_list_data_get(eina_list_last(elm_list_items_get(obj))));
   elm_list_go(obj);
   E_FREE(s);
   E_FREE(c);

   eina_list_free(in);
   ecore_main_fd_handler_del(fd_handler);
   return 0;
}
