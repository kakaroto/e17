
#ifndef  EYELIGHT_EDIT_INC
#define  EYELIGHT_EDIT_INC

#include "Eyelight.h"

/**
 * This file describes an API which allows to work directly in the presentation (set item ...)
 */

EAPI const char* eyelight_edit_slide_title_get(Eyelight_Viewer *pres, int id_slide);
EAPI void eyelight_edit_slide_insert(Eyelight_Viewer *pres, int after);
EAPI void eyelight_edit_slide_delete(Eyelight_Viewer *pres, int id_slide);
EAPI void eyelight_edit_slide_move(Eyelight_Viewer *pres, int id_slide, int id_after);

EAPI Eyelight_Viewer *eyelight_object_pres_get(Evas_Object *obj);

#endif   /* ----- #ifndef EYELIGHT_EDIT_INC  ----- */

