#include "geist_text.h"


geist_object *
geist_text_new(void)
{
   geist_text *txt;

   D_ENTER(5);

   txt = emalloc(sizeof(geist_text));
   geist_text_init(txt);

   GEIST_OBJECT(txt)->visible = TRUE;

   D_RETURN(5, GEIST_OBJECT(txt));
}

geist_object *
geist_text_new_with_text(int x, int y, char *fontname, char *text)
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

   obj->x = x;
   obj->y = y;

   txt->im = geist_text_create_image(txt, &obj->w, &obj->h);

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
   obj->render_selected = geist_object_int_render_selected;
   obj->render_partial = geist_text_render_partial;

   D_RETURN_(5);
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
      free(txt->name);
   if (txt->fontname)
      free(txt->fontname);
   if (txt->text)
      free(txt->text);
   if (txt->fn)
      geist_imlib_free_font(txt->fn);
   if (txt->im)
      geist_imlib_free_image(txt->im);

   free(txt);

   D_RETURN_(5);
}

void
geist_text_render(geist_object * obj, Imlib_Image dest)
{
   geist_text *im;
   int sw, sh, dw, dh;

   D_ENTER(5);

   if (!obj->visible)
      D_RETURN_(5);

   im = GEIST_TEXT(obj);
   if (!im->im)
      D_RETURN_(5);

   dw = geist_imlib_image_get_width(dest);
   dh = geist_imlib_image_get_height(dest);
   sw = geist_imlib_image_get_width(im->im);
   sh = geist_imlib_image_get_height(im->im);

   D(3, ("Rendering text %p with text %s\n", obj, im->text));
   geist_imlib_blend_image_onto_image(dest, im->im, 0, 0, 0, sw, sh, obj->x,
                                      obj->y, sw, sh, 1, 1, im->alias);

   D_RETURN_(5);
}

void
geist_text_render_partial(geist_object * obj, Imlib_Image dest, int x, int y,
                          int w, int h)
{
   geist_text *im;
   int sw, sh, dw, dh, sx, sy, dx, dy;

   D_ENTER(5);

   if (!obj->visible)
      D_RETURN_(5);

   im = GEIST_TEXT(obj);
   if (!im->im)
      D_RETURN_(5);
   
   sx = x - obj->x;
   sy = y - obj->y;
   sw = sx + w;
   sh = sy + h;

   if (sx < 0)
      sx = 0;
   if (sy < 0)
      sy = 0;
   if (sw > geist_imlib_image_get_width(im->im))
      sw = geist_imlib_image_get_width(im->im);
   if (sh > geist_imlib_image_get_height(im->im))
      sh = geist_imlib_image_get_height(im->im);

   dx = obj->x + sx;
   dy = obj->y + sy;
   dw = sw;
   dh = sh;

   D(3,
     ("Rendering text area:\nsx: %d\tsy: %d\nsw: %d\tsh: %d\ndx: %d\tdy: %d\ndw: %d\tdh: %d\n",
      sx, sy, sw, sh, dx, dy, dw, dh));

   D(3, ("Rendering partial text %s\n", im->text));
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
      free(txt->text);
   if (txt->im)
      geist_imlib_free_image_and_decache(txt->im);
   txt->im = geist_text_create_image(txt, &obj->w, &obj->h);

   D_RETURN_(3);
}

Imlib_Image
geist_text_create_image(geist_text * txt, int *w, int *h)
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
   geist_imlib_text_draw(im, txt->fn, 0, 0, txt->text, IMLIB_TEXT_TO_RIGHT, 0,
                         0, 0, 255);

   D_RETURN(3, im);
}
