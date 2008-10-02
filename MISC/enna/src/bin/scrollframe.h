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

Evas_Object *enna_scrollframe_add             (Evas *evas);
void enna_scrollframe_child_set               (Evas_Object *obj, Evas_Object *child);
void enna_scrollframe_extern_pan_set          (Evas_Object *obj, Evas_Object *pan, void (*pan_set) (Evas_Object *obj, Evas_Coord x, Evas_Coord y), void (*pan_get) (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y), void (*pan_max_get) (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y), void (*pan_child_size_get) (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y));
void enna_scrollframe_custom_edje_file_set    (Evas_Object *obj, char *file, char *group);
void enna_scrollframe_child_pos_set           (Evas_Object *obj, Evas_Coord x, Evas_Coord y);
void enna_scrollframe_child_pos_get           (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y);
void enna_scrollframe_child_region_show       (Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h);
void enna_scrollframe_child_viewport_size_get (Evas_Object *obj, Evas_Coord *w, Evas_Coord *h);
void enna_scrollframe_step_size_set           (Evas_Object *obj, Evas_Coord x, Evas_Coord y);
void enna_scrollframe_step_size_get           (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y);
void enna_scrollframe_page_size_set           (Evas_Object *obj, Evas_Coord x, Evas_Coord y);
void enna_scrollframe_page_size_get           (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y);
void enna_scrollframe_policy_set              (Evas_Object *obj, Enna_Scrollframe_Policy hbar, Enna_Scrollframe_Policy vbar);
void enna_scrollframe_policy_get              (Evas_Object *obj, Enna_Scrollframe_Policy *hbar, Enna_Scrollframe_Policy *vbar);
Evas_Object *enna_scrollframe_edje_object_get (Evas_Object *obj);
void enna_scrollframe_single_dir_set          (Evas_Object *obj, Evas_Bool single_dir);
Evas_Bool enna_scrollframe_single_dir_get     (Evas_Object *obj);

#endif
