#include "geist.h"
#include "geist_object.h"
#include "geist_list.h"

#ifndef GEIST_LAYER_H
#define GEIST_LAYER_H

typedef struct __geist_layer geist_layer;

struct __geist_layer
{
   geist_list *objects;
   int transparency;
   int x_offset;
   int y_offset;
   int visible;
   char *name;
};

geist_layer *geist_layer_new(void);
void geist_layer_free(geist_layer *l);
void geist_layer_render(geist_layer * layer, Imlib_Image dest);
void geist_layer_add_object(geist_layer * layer, geist_object *obj);
geist_object *geist_layer_find_clicked_object(geist_layer *layer, int x, int y);

#endif 
