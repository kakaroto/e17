#include "E.h"

ECursor            *
CreateECursor(char *name, char *image, ImlibColor * fg, ImlibColor * bg)
{
   Cursor              curs;
   XColor              xfg, xbg;
   Pixmap              pmap, mask;
   int                 xh, yh;
   unsigned int        w, h, ww, hh;
   char               *img, msk[FILEPATH_LEN_MAX];
   ECursor            *ec;
   int                 r, g, b;

   if ((!name) || (!image))
      return NULL;
   img = FindFile(image);
   if (!img)
      return NULL;

   Esnprintf(msk, sizeof(msk), "%s.mask", img);
   pmap = 0;
   mask = 0;
   xh = 0;
   yh = 0;
   XReadBitmapFile(disp, root.win, msk, &w, &h, &mask, &xh, &yh);
   XReadBitmapFile(disp, root.win, img, &w, &h, &pmap, &xh, &yh);
   XQueryBestCursor(disp, root.win, w, h, &ww, &hh);
   if ((w > ww) || (h > hh))
     {
	EFreePixmap(disp, pmap);
	EFreePixmap(disp, mask);
	Efree(img);
	return NULL;
     }
   r = fg->r;
   g = fg->g;
   b = fg->b;
   xfg.red = (fg->r << 8) | (fg->r);
   xfg.green = (fg->g << 8) | (fg->g);
   xfg.blue = (fg->b << 8) | (fg->b);
   xfg.pixel = Imlib_best_color_match(id, &r, &g, &b);
   r = bg->r;
   g = bg->g;
   b = bg->b;
   xbg.red = (bg->r << 8) | (bg->r);
   xbg.green = (bg->g << 8) | (bg->g);
   xbg.blue = (bg->b << 8) | (bg->b);
   xbg.pixel = Imlib_best_color_match(id, &r, &g, &b);
   curs = 0;
   curs = XCreatePixmapCursor(disp, pmap, mask, &xfg, &xbg, xh, yh);
   EFreePixmap(disp, pmap);
   EFreePixmap(disp, mask);
   Efree(img);
   ec = Emalloc(sizeof(ECursor));
   ec->name = duplicate(name);
   ec->file = duplicate(image);
   ec->fg = *fg;
   ec->bg = *bg;
   ec->cursor = curs;
   ec->ref_count = 0;
   ec->inroot = 0;

   return ec;
}

void
ApplyECursor(Window win, ECursor * ec)
{
   if (!ec)
      return;
   XDefineCursor(disp, win, ec->cursor);

   return;
}

void
FreeECursor(ECursor * ec)
{

   if (!ec)
      return;

   if (ec->ref_count > 0)
     {
	char                stuff[255];

	Esnprintf(stuff, sizeof(stuff), "Error: %u references remain\n",
		  ec->ref_count);
	DIALOG_OK("ECursor Error", stuff);

	return;
     }

   while (RemoveItemByPtr(ec, LIST_TYPE_ECURSOR));

   if (ec->name)
      Efree(ec->name);
   if (ec->file)
      Efree(ec->file);
   Efree(ec);

   return;
}
