#ifndef  GRID_OBJECT_INC
#define  GRID_OBJECT_INC

#include <Evas.h>
#include <Edje.h>
#include <Ecore_Evas.h>
#include <stdio.h>

typedef void (*Grid_Object_Region_CB) (Evas_Object *obj, int *x, int *y, int *w, int *h, void *data);
typedef void (*Grid_Object_Move_CB) (Evas_Object *obj, int id, int id_after, void *data);

Evas_Object *grid_object_add(Evas *evas);

void grid_object_freeze(Evas_Object *obj, int freeze);
void grid_object_clear(Evas_Object *obj);

/**
 * This callback is called by the object to retrieve the region which it should display
 * This avoid to show all the items if the whole object is not displayed
 */
void grid_object_region_cb_set(Evas_Object *obj, Grid_Object_Region_CB cb, void *data);

void grid_object_move_cb_set(Evas_Object *obj, Grid_Object_Move_CB cb, void *data);

void grid_object_items_size_set(Evas_Object *obj, int w, int h);

void grid_object_item_append(Evas_Object *obj, Evas_Object *subobj);
void grid_object_item_append_relative(Evas_Object *obj, Evas_Object *subobj, Evas_Object *relative);
void grid_object_item_prepend(Evas_Object *obj, Evas_Object *subobj);
void grid_object_item_replace(Evas_Object *obj, int id, Evas_Object *subobj);
void grid_object_item_remove(Evas_Object *obj, Evas_Object *subobj);

#endif   /* ----- #ifndef GRID_OBJECT_INC  ----- */

