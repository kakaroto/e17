#ifndef  EYELIGHT_SMART_INC
#define  EYELIGHT_SMART_INC

#include "Eyelight.h"

EAPI Evas_Object *eyelight_object_add                   (Evas *evas);

EAPI void eyelight_object_presentation_file_set(Evas_Object *obj, const char* presentation);
EAPI void eyelight_object_theme_file_set(Evas_Object *obj, const char* theme);
EAPI const char* eyelight_object_presentation_file_get(Evas_Object *obj);
EAPI const char* eyelight_object_theme_file_get(Evas_Object *obj);

EAPI void eyelight_object_eye_file_set(Evas_Object *obj, const char *eye);
EAPI void eyelight_object_dump_file_set(Evas_Object *obj, const char *dump);

EAPI void eyelight_object_border_set(Evas_Object *obj, int border);
EAPI int eyelight_object_size_get(Evas_Object* obj);

EAPI void eyelight_object_focus_set(Evas_Object *obj, int focus);
EAPI void eyelight_object_event_set(Evas_Object *obj, int event);

EAPI void eyelight_object_slide_next(Evas_Object *obj);
EAPI void eyelight_object_slide_previous(Evas_Object *obj);
EAPI void eyelight_object_slide_goto(Evas_Object *obj, int id);

EAPI Eyelight_Viewer_State eyelight_object_state_get(Evas_Object *obj);

EAPI int eyelight_object_current_id_get(Evas_Object *obj);
EAPI void eyelight_object_clear_cache_set(Evas_Object *obj, int clear);

EAPI void eyelight_object_event_send(Evas_Object *obj, void *event_info);

EAPI void eyelight_object_expose_start(Evas_Object *obj, int select);
EAPI void eyelight_object_expose_stop(Evas_Object *obj);
EAPI void eyelight_object_expose_next(Evas_Object *obj);
EAPI void eyelight_object_expose_previous(Evas_Object *obj);
EAPI void eyelight_object_expose_window_next(Evas_Object *obj);
EAPI void eyelight_object_expose_window_previous(Evas_Object *obj);
EAPI void eyelight_object_expose_down(Evas_Object *obj);
EAPI void eyelight_object_expose_up(Evas_Object *obj);
EAPI void eyelight_object_expose_select(Evas_Object *obj);


EAPI void eyelight_object_slideshow_start(Evas_Object *obj,int select);
EAPI void eyelight_object_slideshow_stop(Evas_Object *obj);
EAPI void eyelight_object_slideshow_next(Evas_Object *obj);
EAPI void eyelight_object_slideshow_previous(Evas_Object *obj);
EAPI void eyelight_object_slideshow_select(Evas_Object *obj);

EAPI void eyelight_object_tableofcontents_start(Evas_Object *obj, int select);
EAPI void eyelight_object_tableofcontents_stop(Evas_Object *obj);
EAPI void eyelight_object_tableofcontents_next(Evas_Object *obj);
EAPI void eyelight_object_tableofcontents_previous(Evas_Object *obj);
EAPI void eyelight_object_tableofcontents_select(Evas_Object *obj);

EAPI void eyelight_object_gotoslide_start(Evas_Object *obj);
EAPI void eyelight_object_gotoslide_stop(Evas_Object *obj);
EAPI void eyelight_object_gotoslide_goto(Evas_Object *obj);
EAPI void eyelight_object_gotoslide_digit_add(Evas_Object *obj, int digit);
EAPI void eyelight_object_gotoslide_digit_last_remove(Evas_Object *obj);

EAPI void eyelight_object_thumbnails_size_set(Evas_Object *obj, int w, int h);

#endif   /* ----- #ifndef EYELIGHT_SMART_INC  ----- */

