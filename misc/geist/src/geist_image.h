#ifndef GEIST_IMAGE_H
#define GEIST_IMAGE_H

#include "geist.h"
#include "geist_object.h"

#define GEIST_IMAGE(O) ((geist_image *) O)

struct __geist_image
{
   geist_object object;
   char *filename;
   int w;
   int h;
   Imlib_Image im;
   int alias;
};

geist_object *geist_image_new(void);
geist_object *geist_image_new_from_file(int x, int y, char *filename);
void geist_image_init(geist_image * img);
void geist_image_free(geist_object * obj);
void geist_image_render(geist_object * obj, Imlib_Image dest);
void geist_image_render_partial(geist_object * obj, Imlib_Image dest, int x,
                                int y, int w, int h);
int geist_image_load_file(geist_image * img, char *filename);
Imlib_Image geist_image_get_rendered_image(geist_object * obj);
geist_object *geist_image_duplicate(geist_object * obj);
void geist_image_resize(geist_object * obj, int x, int y);
GtkWidget * geist_image_display_props(geist_object *obj);


#endif
