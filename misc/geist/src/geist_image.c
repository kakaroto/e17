#include "geist.h"
#include "geist_imlib.h"
#include "geist_image.h"


geist_object *
geist_image_new(void)
{
   geist_image *img;

   D_ENTER(5);

   img = emalloc(sizeof(geist_image));
   geist_image_init(img);

   ((geist_object *) img)->visible = TRUE;

   D_RETURN(5, (geist_object *) img);
}

geist_object *
geist_image_new_from_file(int x, int y, char *filename)
{
   geist_image *img;
   geist_object *obj;

   D_ENTER(5);

   obj = geist_image_new();
   img = (geist_image *) obj;

   obj->x = x;
   obj->y = y;
   
   if(!(geist_image_load_file(img, filename)))
   {
      geist_image_free(obj);
      D_RETURN(5, NULL);
   }

   D_RETURN(5, (geist_object *) img);
}

void
geist_image_init(geist_image * img)
{
   geist_object *obj;

   D_ENTER(5);
   memset(img, 0, sizeof(geist_image));
   obj = (geist_object *) img;
   geist_object_init(obj);
   obj->free = geist_image_free;
   obj->render = geist_image_render;

   D_RETURN_(5);
}

void
geist_image_free(geist_object * obj)
{
   geist_image *img;

   D_ENTER(5);

   img = (geist_image *) obj;

   if (!img)
      D_RETURN_(5);

   if (img->name)
      free(img->name);
   if (img->filename)
      free(img->filename);
   if (img->im)
      geist_imlib_free_image(img->im);

   free(img);

   D_RETURN_(5);
}

void
geist_image_render(geist_object * obj, Imlib_Image dest)
{
   geist_image *im;
   int sw, sh, dw, dh;

   D_ENTER(5);

   if (!obj->visible)
      D_RETURN_(5);

   im = (geist_image *) obj;
   if (!im->im)
      D_RETURN_(5);

   
   dw = geist_imlib_image_get_width(dest);
   dh = geist_imlib_image_get_height(dest);
   sw = geist_imlib_image_get_width(im->im);
   sh = geist_imlib_image_get_height(im->im);

   D(3, ("Rendering image %p\n", obj));
   geist_imlib_blend_image_onto_image(dest, im->im, 0, 0, 0, sw, sh, obj->x,
                                      obj->y, sw, sh, 1,
                                      geist_imlib_image_has_alpha(im->im),
                                      im->alias);

   D_RETURN_(5);
}


int
geist_image_load_file(geist_image * img, char *filename)
{
   D_ENTER(5);

   if (img->im)
      geist_imlib_free_image(img->im);

   D_RETURN(5, geist_imlib_load_image(&img->im, filename));
}
