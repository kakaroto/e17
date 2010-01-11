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
      s = NULL;
      free(s);
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
}
