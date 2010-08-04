#ifndef  EYELIGHT_EDIT_INC
#define  EYELIGHT_EDIT_INC

typedef struct Eyelight_Edit Eyelight_Edit;
typedef struct eyelight_node Eyelight_Node;
typedef enum eyelight_node_name Eyelight_Node_Name;

#include "Eyelight.h"

enum eyelight_node_name
{
    EYELIGHT_NAME_NONE,
    EYELIGHT_NAME_ROOT,
    EYELIGHT_NAME_SLIDE,
    EYELIGHT_NAME_IGNORE_AREA,
    EYELIGHT_NAME_ITEMS,
    EYELIGHT_NAME_ITEM,
    EYELIGHT_NAME_AREA,
    EYELIGHT_NAME_EDJ,
    EYELIGHT_NAME_VIDEO,
    EYELIGHT_NAME_TITLE,
    EYELIGHT_NAME_SUBTITLE,
    EYELIGHT_NAME_HEADER_IMAGE,
    EYELIGHT_NAME_FOOT_IMAGE,
    EYELIGHT_NAME_FOOT_TEXT,
    EYELIGHT_NAME_LAYOUT,
    EYELIGHT_NAME_NAME,
    EYELIGHT_NAME_TEXT,
    EYELIGHT_NAME_IMAGE,
    EYELIGHT_NAME_ASPECT,
    EYELIGHT_NAME_KEEP_ASPECT,
    EYELIGHT_NAME_FILE,
    EYELIGHT_NAME_GROUP,
    EYELIGHT_NAME_TRANSITION,
    EYELIGHT_NAME_TRANSITION_NEXT,
    EYELIGHT_NAME_TRANSITION_PREVIOUS,
    EYELIGHT_NAME_CUSTOM_AREA,
    EYELIGHT_NAME_THEME_AREA,
    EYELIGHT_NAME_BORDER,
    EYELIGHT_NAME_NUMBERING,
    EYELIGHT_NAME_RELATIVE,
    EYELIGHT_NAME_ALPHA,
    EYELIGHT_NAME_AUTOPLAY,
    EYELIGHT_NAME_REPLAY,
    EYELIGHT_NAME_SHADOW,
    EYELIGHT_NAME_PRESENTATION,
    EYELIGHT_NAME_THEME,
    EYELIGHT_NAME_CODE
};



/**
 * This file describes an API which allows to work directly in the presentation (set item ...) <br>
 * <b>WARNING: We can only work on a slide which is display ! Do not try to edit a slide which is not display.</b>
 */

/**
 * @brief Extension of the eyelight smart object. Returns The Eyelight_Viewer* object.
 * @param obj the eyelight smart object
 * @return Returns the Eyelgiht_Viewer* object
 */

EAPI Eyelight_Viewer *eyelight_object_pres_get(Evas_Object *obj);

EAPI void eyelight_edit_edit_mode_set(Eyelight_Viewer *pres, Edje_Signal_Cb cb, void *data);
EAPI int eyelight_edit_save(Eyelight_Viewer *pres, const char *file);

EAPI const char* eyelight_edit_slide_title_get(Eyelight_Viewer *pres, int id_slide);
EAPI void eyelight_edit_slide_insert(Eyelight_Viewer *pres, int after);
EAPI void eyelight_edit_slide_delete(Eyelight_Viewer *pres, int id_slide);
EAPI void eyelight_edit_slide_move(Eyelight_Viewer *pres, int id_slide, int id_after);
EAPI void eyelight_edit_slide_default_areas_reinit(Eyelight_Viewer *pres, int id_slide);

EAPI Eyelight_Node_Name eyelight_edit_name_get(Eyelight_Edit *edit);
EAPI void eyelight_edit_obj_unselect(Eyelight_Edit *edit);
EAPI void eyelight_edit_object_down(Eyelight_Viewer *pres, Eyelight_Edit *edit);
EAPI void eyelight_edit_object_up(Eyelight_Viewer *pres, Eyelight_Edit *edit);
EAPI void eyelight_edit_object_delete(Eyelight_Edit *edit);


EAPI void eyelight_edit_area_geometry_get(Eyelight_Edit *edit, double *rel1_x, double *rel1_y, double *rel2_x, double *rel2_y);
EAPI void eyelight_edit_area_move(Eyelight_Edit *edit, double rel1_x, double rel1_y, double rel2_x, double rel2_y);
EAPI void eyelight_edit_area_up(Eyelight_Edit *edit);
EAPI void eyelight_edit_area_down(Eyelight_Edit *edit);
EAPI void eyelight_edit_area_add(Eyelight_Viewer *pres, int id_slide);
EAPI void eyelight_edit_area_delete(Eyelight_Edit *edit);
EAPI const char* eyelight_edit_area_layout_get(Eyelight_Edit *edit);
EAPI void eyelight_edit_area_layout_set(Eyelight_Edit *edit, const char *layout);

EAPI void eyelight_edit_area_image_add(Eyelight_Viewer *pres, Eyelight_Edit *edit, const char* image);
EAPI void eyelight_edit_image_properties_get(Eyelight_Edit *edit, char **file, int *border, int *shadow, int *keep_aspect);
EAPI void eyelight_edit_image_properties_set(Eyelight_Viewer *pres, Eyelight_Edit *edit, char *file, int border, int shadow, int keep_aspect);

#endif   /* ----- #ifndef EYELIGHT_EDIT_INC  ----- */

