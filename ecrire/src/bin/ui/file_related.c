#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Elementary.h>

Evas_Object *inwin;

static void
_cleaning_cb(void *data, Evas_Object *obj, void *event_info)
{
   void (*func) (void *, Evas_Object *, void *) = data;
   func(NULL, obj, event_info);
   evas_object_del(inwin);  /* delete the test window */
}

void
ui_file_open_save_dialog_open(Evas_Object *parent, Evas_Smart_Cb func,
      Eina_Bool save)
{
   Evas_Object *fs;
   inwin = elm_win_inwin_add(parent);
   evas_object_show(inwin);

   fs = elm_fileselector_add(inwin);
   elm_fileselector_is_save_set(fs, save);
   elm_fileselector_expandable_set(fs, EINA_FALSE);
   elm_fileselector_path_set(fs, getenv("HOME"));
   elm_win_inwin_content_set(inwin, fs);
   evas_object_show(fs);

   evas_object_smart_callback_add(fs, "done", _cleaning_cb, func);
}
