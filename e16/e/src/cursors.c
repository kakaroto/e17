/*
 * Copyright (C) 2000 Carsten Haitzler, Geoff Harrison and various contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
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

        Esnprintf(stuff, sizeof(stuff), _("%u references remain\n"),
                  ec->ref_count);
        DIALOG_OK(_("ECursor Error!"), stuff);

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
