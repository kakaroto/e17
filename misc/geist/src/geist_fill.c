#include "geist.h"
#include "geist_fill.h"

geist_fill *
geist_fill_new(void)
{
   geist_fill *f = NULL;

   D_ENTER(3);

   f = emalloc(sizeof(geist_fill));

   memset(f, 0, sizeof(geist_fill));

   D_RETURN(3, f);
}

void
geist_fill_free(geist_fill * f)
{
   D_ENTER(3);

   efree(f);

   D_RETURN_(3);
}

geist_fill *
geist_fill_new_coloured(int r, int g, int b, int a)
{
   geist_fill *f;
   D_ENTER(3);

   f = geist_fill_new();
   f->r = r;
   f->g = g;
   f->b = b;
   f->a = a;
   
   D_RETURN(3, f);
}

void
geist_fill_render(geist_fill * fill, Imlib_Image dest, int x, int y, int w,
                  int h)
{
   D_ENTER(3);

   geist_imlib_image_fill_rectangle(dest, x, y, w, h, fill->r, fill->g, fill->b,
                              fill->a);

   D_RETURN_(3);
}
