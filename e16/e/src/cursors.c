/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
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
CreateECursor(char *name, char *image, int native_id, XColor * fg, XColor * bg)
{
   Cursor              curs;
   Pixmap              pmap, mask;
   int                 xh, yh;
   unsigned int        w, h, ww, hh;
   char               *img, msk[FILEPATH_LEN_MAX];
   ECursor            *ec;

   if ((!name) || (!image && native_id == -1))
      return NULL;

   if (image)
     {
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

	EAllocColor(fg);
	EAllocColor(bg);

	curs = 0;
	curs = XCreatePixmapCursor(disp, pmap, mask, fg, bg, xh, yh);
	EFreePixmap(disp, pmap);
	EFreePixmap(disp, mask);
	Efree(img);
     }
   else
     {
	curs = XCreateFontCursor(disp, native_id);
     }

   ec = Emalloc(sizeof(ECursor));
   ec->name = duplicate(name);
   ec->file = duplicate(image);
#if 0				/* Not used */
   ec->fg = *fg;
   ec->bg = *bg;
#endif
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
}

void
FreeECursor(ECursor * ec)
{
   if (!ec)
      return;

   if (ec->ref_count > 0)
     {
	DialogOK(_("ECursor Error!"), _("%u references remain\n"),
		 ec->ref_count);
	return;
     }

   while (RemoveItemByPtr(ec, LIST_TYPE_ECURSOR));

   if (ec->name)
      Efree(ec->name);
   if (ec->file)
      Efree(ec->file);
   Efree(ec);
}
