
#ifndef GEIST_LAYER_H
#define GEIST_LAYER_H

#include "geist.h"
#include "geist_object.h"
#include "geist_document.h"
#include "geist_list.h"

#define GEIST_LAYER(O) ((geist_layer *)O)

struct __geist_layer
{
   geist_list *objects;
   geist_document *doc;
   int transparency;
   int x_offset;
   int y_offset;
   int visible;
   char *name;
};

geist_layer *geist_layer_new(void);
void geist_layer_free(geist_layer * l);
void geist_layer_render(geist_layer * layer, Imlib_Image dest);
void geist_layer_render_partial(geist_layer * layer, Imlib_Image dest, int x,
                                int y, int w, int h);

void geist_layer_add_object(geist_layer * layer, geist_object * obj);
void geist_layer_raise_object(geist_object * obj);
geist_object *geist_layer_find_clicked_object(geist_layer * layer, int x,

                                              int y);
unsigned char geist_layer_remove_object(geist_layer *lay, geist_object *obj);

#endif
