#include "geist_object.h"

geist_object *
geist_object_new(void)
{
   geist_object *obj = NULL;

   D_ENTER(5);

   obj = emalloc(sizeof(geist_object));

   D_RETURN(5, obj);
}

void
geist_object_init(geist_object * obj)
{
   D_ENTER(5);

   if (!obj)
      eprintf("bug. no object to init.");

   memset(obj, 0, sizeof(geist_object));
   obj->free = geist_object_int_free;
   obj->render = geist_object_int_render;
   obj->render_selected = geist_object_int_render_selected;
   obj->render_partial = geist_object_int_render_partial;
   obj->sizemode = SIZEMODE_ZOOM;

   D_RETURN_(5);
}

void
geist_object_int_free(geist_object * obj)
{
   D_ENTER(5);

   if (!obj)
      D_RETURN_(5);

   if (obj->name)
      free(obj->name);

   free(obj);

   D_RETURN_(5);
}

void
geist_object_free(geist_object * obj)
{
   D_ENTER(5);

   if (!obj)
      D_RETURN_(5);

   obj->free(obj);

   free(obj);

   D_RETURN_(5);
}

void
geist_object_int_render(geist_object * obj, Imlib_Image dest)
{
   D_ENTER(5);

   printf("IMPLEMENT\n");

   D_RETURN_(5);
}

void
geist_object_int_render_partial(geist_object * obj, Imlib_Image dest, int x,
                                int y, int w, int h)
{
   D_ENTER(5);

   printf("IMPLEMENT\n");

   D_RETURN_(5);
}


void
geist_object_render(geist_object * obj, Imlib_Image dest)
{
   D_ENTER(5);

   if (!obj)
      D_RETURN_(5);

   obj->render(obj, dest);

   D_RETURN_(5);
}

void
geist_object_render_partial(geist_object * obj, Imlib_Image dest, int x,
                            int y, int w, int h)
{
   D_ENTER(5);

   D(5, ("rendering area %d,%d %dx%d\n", x, y, w, h));
   obj->render_partial(obj, dest, x, y, w, h);

   D_RETURN_(5);
}


void
geist_object_show(geist_object * obj)
{
   D_ENTER(3);

   obj->visible = TRUE;

   D_RETURN_(3);
}

void
geist_object_raise(geist_document * doc, geist_object * obj)
{
   D_ENTER(3);

   if (!obj)
      D_RETURN_(3);

   geist_layer_raise_object(doc, obj);

   D_RETURN_(3);
}

void
geist_object_render_selected(geist_object * obj, Imlib_Image dest,
                             unsigned char multiple)
{
   D_ENTER(3);

   obj->render_selected(obj, dest, multiple);

   D_RETURN_(3);
}

void
geist_object_int_render_selected(geist_object * obj, Imlib_Image dest,
                                 unsigned char multiple)
{
   D_ENTER(3);

   if (multiple)
   {
      geist_imlib_image_draw_rectangle(dest, obj->x - HALF_SEL_WIDTH,
                                       obj->y - HALF_SEL_HEIGHT,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_draw_rectangle(dest, obj->x - HALF_SEL_WIDTH + obj->w,
                                       obj->y - HALF_SEL_HEIGHT,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_draw_rectangle(dest, obj->x - HALF_SEL_WIDTH,
                                       obj->y - HALF_SEL_HEIGHT + obj->h,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_draw_rectangle(dest, obj->x - HALF_SEL_WIDTH + obj->w,
                                       obj->y - HALF_SEL_HEIGHT + obj->h,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
   }
   else
   {
      geist_imlib_image_fill_rectangle(dest, obj->x - HALF_SEL_WIDTH,
                                       obj->y - HALF_SEL_HEIGHT,
                                       2 * HALF_SEL_WIDTH, 4, 0, 0, 0, 255);
      geist_imlib_image_fill_rectangle(dest, obj->x - HALF_SEL_WIDTH + obj->w,
                                       obj->y - HALF_SEL_HEIGHT,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_fill_rectangle(dest, obj->x - HALF_SEL_WIDTH,
                                       obj->y - HALF_SEL_HEIGHT + obj->h,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_fill_rectangle(dest, obj->x - HALF_SEL_WIDTH + obj->w,
                                       obj->y - HALF_SEL_HEIGHT + obj->h,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
   }

   D_RETURN_(3);
}
