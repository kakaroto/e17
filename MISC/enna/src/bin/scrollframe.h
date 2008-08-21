#ifndef ENNA_SCROLLFRAME_H
#define ENNA_SCROLLFRAME_H

#include "enna.h"


typedef enum _Enna_Scrollframe_Policy
{
    ENNA_SCROLLFRAME_POLICY_OFF,
    ENNA_SCROLLFRAME_POLICY_ON,
    ENNA_SCROLLFRAME_POLICY_AUTO
}
Enna_Scrollframe_Policy;

EAPI Evas_Object *enna_scrollframe_add             (Evas *evas);
EAPI void enna_scrollframe_child_set               (Evas_Object *obj, Evas_Object *child);
EAPI void enna_scrollframe_extern_pan_set          (Evas_Object *obj, Evas_Object *pan, void (*pan_set) (Evas_Object *obj, Evas_Coord x, Evas_Coord y), void (*pan_get) (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y), void (*pan_max_get) (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y), void (*pan_child_size_get) (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y));
EAPI void enna_scrollframe_custom_theme_set        (Evas_Object *obj, char *custom_category, char *custom_group);
EAPI void enna_scrollframe_custom_edje_file_set    (Evas_Object *obj, char *file, char *group);
EAPI void enna_scrollframe_child_pos_set           (Evas_Object *obj, Evas_Coord x, Evas_Coord y);
EAPI void enna_scrollframe_child_pos_get           (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y);
EAPI void enna_scrollframe_child_region_show       (Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h);
EAPI void enna_scrollframe_child_viewport_size_get (Evas_Object *obj, Evas_Coord *w, Evas_Coord *h);
EAPI void enna_scrollframe_step_size_set           (Evas_Object *obj, Evas_Coord x, Evas_Coord y);
EAPI void enna_scrollframe_step_size_get           (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y);
EAPI void enna_scrollframe_page_size_set           (Evas_Object *obj, Evas_Coord x, Evas_Coord y);
EAPI void enna_scrollframe_page_size_get           (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y);
EAPI void enna_scrollframe_policy_set              (Evas_Object *obj, Enna_Scrollframe_Policy hbar, Enna_Scrollframe_Policy vbar);
EAPI void enna_scrollframe_policy_get              (Evas_Object *obj, Enna_Scrollframe_Policy *hbar, Enna_Scrollframe_Policy *vbar);
EAPI Evas_Object *enna_scrollframe_edje_object_get (Evas_Object *obj);
EAPI void enna_scrollframe_event_key_down          (Evas_Object *obj, void *event_info);

#endif
