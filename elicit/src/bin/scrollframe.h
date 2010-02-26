/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef SCROLLFRAME_H
#define SCROLLFRAME_H

#include <Evas.h>

typedef enum _E_Scrollframe_Policy
{
  SCROLLFRAME_POLICY_OFF,
  SCROLLFRAME_POLICY_ON,
  SCROLLFRAME_POLICY_AUTO
} Scrollframe_Policy;

Evas_Object *scrollframe_add             (Evas *evas);
void scrollframe_theme_set(Evas_Object *obj, const char *file, const char *group);
void scrollframe_child_set               (Evas_Object *obj, Evas_Object *child);
void scrollframe_extern_pan_set          (Evas_Object *obj, Evas_Object *pan, void (*pan_set) (Evas_Object *obj, Evas_Coord x, Evas_Coord y), void (*pan_get) (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y), void (*pan_max_get) (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y), void (*pan_child_size_get) (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y));
void scrollframe_custom_theme_set        (Evas_Object *obj, char *custom_category, char *custom_group);
void scrollframe_custom_edje_file_set    (Evas_Object *obj, char *file, char *group);
void scrollframe_child_pos_set           (Evas_Object *obj, Evas_Coord x, Evas_Coord y);
void scrollframe_child_pos_get           (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y);
void scrollframe_child_region_show       (Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h);
void scrollframe_child_viewport_size_get (Evas_Object *obj, Evas_Coord *w, Evas_Coord *h);
void scrollframe_step_size_set           (Evas_Object *obj, Evas_Coord x, Evas_Coord y);
void scrollframe_step_size_get           (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y);
void scrollframe_page_size_set           (Evas_Object *obj, Evas_Coord x, Evas_Coord y);
void scrollframe_page_size_get           (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y);
void scrollframe_policy_set              (Evas_Object *obj, Scrollframe_Policy hbar, Scrollframe_Policy vbar);
void scrollframe_policy_get              (Evas_Object *obj, Scrollframe_Policy *hbar, Scrollframe_Policy *vbar);
Evas_Object *scrollframe_edje_object_get (Evas_Object *obj);
void scrollframe_fill_policy_set(Evas_Object *obj, int fill_x, int fill_y);
void scrollframe_scroll_pos_set(Evas_Object *obj, float vx, float vy);
    
#endif
