#include "geist.h"
#include "geist_object.h"

struct __geist_image {
   geist_object object;
   char *name;
   char *filename;
   int w;
   int h;
   Imlib_Image im;
   int alias;
};

geist_object *geist_image_new(void);
geist_object *geist_image_new_from_file(int x, int y, char *filename);
void geist_image_init(geist_image *img);
void geist_image_free(geist_object *obj);
void geist_image_render(geist_object *obj, Imlib_Image dest);
int geist_image_load_file(geist_image *img, char *filename);
