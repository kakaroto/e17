#ifndef GEIST_IMAGE_H
#define GEIST_IMAGE_H

#include "geist.h"
#include "geist_object.h"

#define GEIST_IMAGE(O) ((geist_image *) O)
#define FULL_OPACITY 100
#define FULL_COLOR 100

#define R 0
#define G 1
#define B 2
#define A 3

struct __geist_image
{
   geist_object object;
   char *filename;
   Imlib_Image im, orig_im;
   struct {
      int image_mods[4];
   } last;
   int image_mods[4];
};

geist_object *geist_image_new(void);
geist_object *geist_image_new_from_file(int x, int y, char *filename);
void geist_image_init(geist_image * img);
void geist_image_free(geist_object * obj);
void geist_image_render(geist_object * obj, Imlib_Image dest);
void geist_image_render_partial(geist_object * obj, Imlib_Image dest, int x,
                                int y, int w, int h);
void refresh_image_opacity_cb(GtkWidget * widget, gpointer * obj);
int geist_image_load_file(geist_image * img, char *filename);
Imlib_Image geist_image_get_rendered_image(geist_object * obj);
geist_object *geist_image_duplicate(geist_object * obj);
void geist_image_resize(geist_object * obj, int x, int y);
GtkWidget * geist_image_display_props(geist_object *obj);
void geist_image_change_image_mods(geist_object * obj, int p[]);
void geist_image_apply_image_mods(geist_object * obj);
void geist_image_update_sizemode(geist_object * obj);
void geist_image_update_positioning(geist_object *obj);
char *geist_image_get_object_list_entry(geist_object * obj, int column);
unsigned char geist_image_has_transparency(geist_object * obj);

#endif
