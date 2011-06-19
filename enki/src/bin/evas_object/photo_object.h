#ifndef  PHOTO_OBJECT_INC
#define  PHOTO_OBJECT_INC

#include <Evas.h>
#include <Edje.h>
#include <Ecore_Evas.h>
#include <stdio.h>
#include <Elementary.h>

Evas_Object *
photo_object_add(Evas_Object *obj);
void
photo_object_theme_file_set(Evas_Object *obj, const char *theme,
                            const char *theme_group);
void
photo_object_file_set(Evas_Object *obj, const char *image,
                      const char *photo_group);
void
photo_object_size_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
void
photo_object_progressbar_set(Evas_Object *obj, Eina_Bool b);
void
photo_object_radio_set(Evas_Object *obj, Eina_Bool b);
void
photo_object_camera_set(Evas_Object *obj, Eina_Bool b);
void
photo_object_text_set(Evas_Object *obj, const char *s);
void
photo_object_done_set(Evas_Object *obj, Eina_Bool b);
void
photo_object_fill_set(Evas_Object *obj, Eina_Bool fill);
void
photo_object_gpx_set(Evas_Object *obj);

Evas_Object *
photo_object_netsync_state_set(Evas_Object *obj, const char *state);
void
photo_object_icon_menu_show(Evas_Object *obj, Eina_Bool show);

#endif   /* ----- #ifndef PHOTO_OBJECT_INC  ----- */

