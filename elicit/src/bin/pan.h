/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef PAN_H
#define PAN_H

#include <Evas.h> 

Evas_Object *pan_add            (Evas *evas);
void         pan_child_set      (Evas_Object *obj, Evas_Object *child);
Evas_Object *pan_child_get      (Evas_Object *obj);
void         pan_set            (Evas_Object *obj, Evas_Coord x, Evas_Coord y);
void         pan_get            (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y);
void         pan_max_get        (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y);
void         pan_child_size_get (Evas_Object *obj, Evas_Coord *w, Evas_Coord *h);
void         pan_fill_policy_set(Evas_Object *obj, int fill_x, int fill_y);
    
#endif
