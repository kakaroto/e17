#include "private.h"

Evas_Object *
nowplaying_add(Evas_Object *parent)
{
   Evas_Object *layout = elm_layout_add(parent);
   if (!layout) return NULL;

   if (!elm_layout_theme_set(layout, "layout", "enjoy", "nowplaying"))
     {
        if (!elm_layout_file_set(layout,
                                 PACKAGE_DATA_DIR "/default.edj", "nowplaying"))
          {
             CRITICAL("no theme for 'nowplaying' at %s",
                      PACKAGE_DATA_DIR "/default.edj");
             goto error;
          }
     }

   return layout;
error:
   evas_object_del(layout);
   return NULL;
}

