#include "geist_text.h"


geist_object *
geist_text_new(void)
{
   geist_text *txt;

   D_ENTER(5);

   txt = emalloc(sizeof(geist_text));
   geist_text_init(txt);

   geist_object_set_state(GEIST_OBJECT(txt), VISIBLE);

   D_RETURN(5, GEIST_OBJECT(txt));
}

void
geist_text_init(geist_text * txt)
{
   geist_object *obj;

   D_ENTER(5);
   memset(txt, 0, sizeof(geist_text));
   obj = GEIST_OBJECT(txt);
   geist_object_init(obj);
   obj->free = geist_text_free;
   obj->render = geist_text_render;
   obj->render_partial = geist_text_render_partial;
   obj->get_rendered_image = geist_text_get_rendered_image;
   obj->duplicate = geist_text_duplicate;
   obj->resize_event = geist_text_resize;
   obj->sizemode = SIZEMODE_NONE;
   obj->alignment = ALIGN_CENTER;
   geist_object_set_type(obj, GEIST_TYPE_TEXT);

   D_RETURN_(5);
}

geist_object *
geist_text_new_with_text(int x, int y, char *fontname, char *text, int a,
                         int r, int g, int b)
{
   geist_text *txt;
   geist_object *obj;
   Imlib_Font fn;

   D_ENTER(5);

   fn = imlib_load_font(fontname);

   if (!fn)
   {
      weprintf("load font %s failed.", fontname);
      D_RETURN(5, (NULL));
   }

   obj = geist_text_new();
   txt = GEIST_TEXT(obj);

   txt->text = estrdup(text);
   txt->fontname = estrdup(fontname);

   txt->fn = fn;
   txt->a = a;
   txt->b = b;
   txt->g = g;
   txt->r = r;

   obj->x = x;
   obj->y = y;
   obj->rendered_x = 0;
   obj->rendered_y = 0;

   txt->im = geist_text_create_image(txt, &obj->w, &obj->h);

   D_RETURN(5, GEIST_OBJECT(txt));
}

void
geist_text_free(geist_object * obj)
{
   geist_text *txt;

   D_ENTER(5);

   txt = (geist_text *) obj;

   if (!txt)
      D_RETURN_(5);

   if (txt->name)
      efree(txt->name);
   if (txt->fontname)
      efree(txt->fontname);
   if (txt->text)
      efree(txt->text);
   if (txt->fn)
      geist_imlib_free_font(txt->fn);
   if (txt->im)
      geist_imlib_free_image(txt->im);

   efree(txt);

   D_RETURN_(5);
}

void
geist_text_render(geist_object * obj, Imlib_Image dest)
{
   geist_text *im;
   int sw, sh, dw, dh;

   D_ENTER(5);

   if (!geist_object_get_state(obj, VISIBLE))
      D_RETURN_(5);

   im = GEIST_TEXT(obj);
   if (!im->im)
      D_RETURN_(5);

   dw = geist_imlib_image_get_width(dest);
   dh = geist_imlib_image_get_height(dest);
   sw = geist_imlib_image_get_width(im->im);
   sh = geist_imlib_image_get_height(im->im);

   D(3, ("Rendering text %p with text %s\n", obj, im->text));
   geist_imlib_blend_image_onto_image(dest, im->im, 0, 0, 0, sw, sh,
                                      obj->x + obj->rendered_x,
                                      obj->y + obj->rendered_y, sw, sh, 1, 1,
                                      im->alias);

   D_RETURN_(5);
}

void
geist_text_render_partial(geist_object * obj, Imlib_Image dest, int x, int y,
                          int w, int h)
{
   geist_text *im;
   int sw, sh, dw, dh, sx, sy, dx, dy;

   D_ENTER(5);

   if (!geist_object_get_state(obj, VISIBLE))
      D_RETURN_(5);

   im = GEIST_TEXT(obj);
   if (!im->im)
      D_RETURN_(5);

   if (obj->rendered_x < 0)
      sx = x - obj->x;
   else
      sx = x - (obj->x + obj->rendered_x);
   if (obj->rendered_y < 0)
      sy = y - obj->y;
   else
      sy = y - (obj->y + obj->rendered_y);

   if (sx < 0)
      sx = 0;
   if (sy < 0)
      sy = 0;

   if (obj->rendered_w > obj->w)
      sw = obj->w - sx;
   else
      sw = obj->rendered_w - sx;

   if (obj->rendered_h > obj->h)
      sh = obj->h - sy;
   else
      sh = obj->rendered_h - sy;

   if (sw > w)
      sw = w;
   if (sh > h)
      sh = h;

   if (obj->rendered_x < 0)
      dx = obj->x + sx;
   else
      dx = (obj->x + obj->rendered_x) + sx;
   if (obj->rendered_y < 0)
      dy = obj->y + sy;
   else
      dy = (obj->y + obj->rendered_y) + sy;
   dw = sw;
   dh = sh;

   D(5, ("Rendering partial text %s\n", im->text));
   D(5,
     ("Rendering text area:\nsx: %d\tsy: %d\nsw: %d\tsh: %d\ndx: %d\tdy: %d\ndw: %d\tdh: %d\n",
      sx, sy, sw, sh, dx, dy, dw, dh));

   geist_imlib_blend_image_onto_image(dest, im->im, 0, sx, sy, sw, sh, dx, dy,
                                      dw, dh, 1, 1, im->alias);

   D_RETURN_(5);
}

void
geist_text_change_text(geist_text * txt, char *newtext)
{
   geist_object *obj;

   D_ENTER(3);

   obj = GEIST_OBJECT(txt);

   if (txt->text)
      efree(txt->text);
   if (txt->im)
      geist_imlib_free_image_and_decache(txt->im);
   txt->text = estrdup(newtext);
   txt->im = geist_text_create_image(txt, &obj->w, &obj->h);

   D_RETURN_(3);
}

Imlib_Image geist_text_create_image(geist_text * txt, int *w, int *h)
{
   DATA8 atab[256];
   Imlib_Image im;
   geist_object *obj;

   D_ENTER(3);

   if (!txt->fn)
   {
      weprintf("no font for text.");
      D_RETURN(3, NULL);
   }
   if (!txt->text)
   {
      weprintf("no text in text object.");
      D_RETURN(3, NULL);
   }

   obj = GEIST_OBJECT(txt);
   geist_imlib_get_text_size(txt->fn, txt->text, w, h, IMLIB_TEXT_TO_RIGHT);
   obj->rendered_w = *w;
   obj->rendered_h = *h;

   im = imlib_create_image(*w, *h);
   if (!im)
   {
      weprintf("couldn't create imlib image for text area.");
      D_RETURN(3, NULL);
   }

   /* make image transparent (HACK - imlib2 should do this nicely) */
   geist_imlib_image_set_has_alpha(im, 1);
   memset(atab, 0, sizeof(atab));
   geist_imlib_apply_color_modifier_to_rectangle(im, 0, 0, obj->w, obj->h,
                                                 NULL, NULL, NULL, atab);


   /* todo - put offset, margin, color etc into txt struct */
   geist_imlib_text_draw(im, txt->fn, 0, 0, txt->text, IMLIB_TEXT_TO_RIGHT,
                         txt->r, txt->g, txt->b, txt->a);

   D_RETURN(3, im);
}

Imlib_Image
geist_text_get_rendered_image(geist_object * obj)
{
   D_ENTER(3);

   D_RETURN(3, GEIST_TEXT(obj)->im);
}

geist_object *
geist_text_duplicate(geist_object * obj)
{
   geist_object *ret;
   geist_text *txt;

   D_ENTER(3);

   txt = GEIST_TEXT(obj);

   ret =
      geist_text_new_with_text(obj->x, obj->y, txt->fontname, txt->text,
                               txt->a, txt->r, txt->g, txt->b);
   ret->rendered_x = obj->rendered_x;
   ret->rendered_y = obj->rendered_y;
   ret->w = obj->w;
   ret->h = obj->h;
   if (ret)
   {
      ret->state = obj->state;
      GEIST_IMAGE(ret)->alias = txt->alias;
      ret->name =
         g_strjoin(" ", "Copy of", obj->name ? obj->name : "Untitled object",
                   NULL);
   }

   D_RETURN(3, ret);
}

void
geist_text_resize(geist_object * obj, int x, int y)
{
   geist_text *txt;

   D_ENTER(5);

   txt = GEIST_TEXT(obj);

   D(5, ("resize to %d,%d\n", x, y));
   geist_object_resize_object(obj, x, y);

   D_RETURN_(5);
}
