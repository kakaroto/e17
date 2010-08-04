#ifndef  SLIDES_GRID_INC
#define  SLIDES_GRID_INC

#include "main.h"
#include "grid_object.h"
#include "slides_list.h"

Evas_Object *slides_grid_create();
void slides_grid_update();
void slides_grid_thumb_done_cb(int id_slide);
void slides_grid_append_relative(List_Item *item, List_Item *previous);
void slides_grid_remove(List_Item *item);

#endif   /* ----- #ifndef SLIDES_GRID_INC  ----- */

