#include "geist_rect.h"


geist_object *
geist_rect_new(void)
{
   geist_rect *rec;

   D_ENTER(5);

   rec = emalloc(sizeof(geist_rect));
   geist_rect_init(rec);

   geist_object_set_state(GEIST_OBJECT(rec), VISIBLE);

   D_RETURN(5, GEIST_OBJECT(rec));
}

void
geist_rect_init(geist_rect * rec)
{
   geist_object *obj;

   D_ENTER(5);
   memset(rec, 0, sizeof(geist_rect));
   obj = GEIST_OBJECT(rec);
   geist_object_init(obj);
   obj->free = geist_rect_free;
   obj->render = geist_rect_render;
   obj->render_partial = geist_rect_render_partial;
   obj->duplicate = geist_rect_duplicate;
   obj->part_is_transparent = geist_text_part_is_transparent;
   obj->resize_event = geist_rect_resize;
   geist_object_set_type(obj, GEIST_TYPE_RECT);

   D_RETURN_(5);
}

geist_object *
geist_rect_new_of_size(int x, int y, int w, int h, int a, int r, int g, int b)
{
   geist_rect *rec;
   geist_object *obj;

   D_ENTER(5);

   obj = geist_rect_new();
   rec = GEIST_RECT(obj);

   rec->a = a;
   rec->b = b;
   rec->g = g;
   rec->r = r;

   obj->x = x;
   obj->y = y;
   obj->w = obj->rendered_w = w;
   obj->h = obj->rendered_h = h;
   obj->rendered_x = 0;
   obj->rendered_y = 0;

   D_RETURN(5, GEIST_OBJECT(rec));
}

void
geist_rect_free(geist_object * obj)
{
   geist_rect *rec;

   D_ENTER(5);

   rec = (geist_rect *) obj;

   if (!rec)
      D_RETURN_(5);

   efree(rec);

   D_RETURN_(5);
}

void
geist_rect_render(geist_object * obj, Imlib_Image dest)
{
   geist_rect *rec;

   D_ENTER(5);

   if (!geist_object_get_state(obj, VISIBLE))
      D_RETURN_(5);

   rec = GEIST_RECT(obj);
   D(5,
     ("rendering %d,%d %dx%d with %d,%d,%d,%d\n", obj->x, obj->y, obj->w,
      obj->h, rec->r, rec->g, rec->b, rec->a));

   geist_imlib_image_fill_rectangle(dest, obj->x, obj->y, obj->w, obj->h,
                                    rec->r, rec->g, rec->b, rec->a);

   D_RETURN_(5);
}

void
geist_rect_render_partial(geist_object * obj, Imlib_Image dest, int x, int y,
                          int w, int h)
{
   geist_rect *rec;
   int sw, sh, dw, dh, sx, sy, dx, dy;

   D_ENTER(5);

   if (!geist_object_get_state(obj, VISIBLE))
      D_RETURN_(5);

   rec = GEIST_RECT(obj);

   sx = x - obj->x;
   sy = y - obj->y;

   if (sx < 0)
      sx = 0;
   if (sy < 0)
      sy = 0;

   sw = obj->w - sx;
   sh = obj->h - sy;

   if (sw > w)
      sw = w;
   if (sh > h)
      sh = h;

   dx = obj->x + sx;
   dy = obj->y + sy;
   dw = sw;
   dh = sh;

   D(5,
     ("partial rendering %d,%d %dx%d with %d,%d,%d,%d\n", dx, dy, dw, dh,
      rec->r, rec->g, rec->b, rec->a));

   geist_imlib_image_fill_rectangle(dest, dx, dy, dw, dh, rec->r, rec->g,
                                    rec->b, rec->a);

   D_RETURN_(5);
}

geist_object *
geist_rect_duplicate(geist_object * obj)
{
   geist_object *ret;
   geist_rect *rec;

   D_ENTER(3);

   rec = GEIST_RECT(obj);

   ret =
      geist_rect_new_of_size(obj->x, obj->y,
                             obj->rendered_w, obj->rendered_h, rec->a, rec->r,
                             rec->g, rec->b);
   ret->rendered_x = obj->rendered_x;
   ret->rendered_y = obj->rendered_y;
   ret->w = obj->w;
   ret->h = obj->h;
   if (ret)
   {
      ret->state = obj->state;
      ret->name =
         g_strjoin(" ", "Copy of", obj->name ? obj->name : "Untitled object",
                   NULL);
   }

   D_RETURN(3, ret);
}

unsigned char
geist_text_part_is_transparent(geist_object * obj, int x, int y)
{
   D_ENTER(3);

   D_RETURN(3, GEIST_RECT(obj)->a == 0 ? 1 : 0);
}

void
geist_rect_resize(geist_object * obj, int x, int y)
{
   D_ENTER(5);

   printf("resize to %d,%d\n", x, y);

   D_RETURN_(5);
}
