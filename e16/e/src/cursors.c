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
#include "X11/cursorfont.h"

struct _ecursor
{
   char               *name;
#if 0				/* Not used */
   Imlib_Color         fg, bg;
#endif
   char               *file;
   Cursor              cursor;
   unsigned int        ref_count;
#if 0				/* Not used */
   char                inroot;
#endif
};

ECursor            *
ECursorCreate(const char *name, const char *image, int native_id, XColor * fg,
	      XColor * bg)
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
	XReadBitmapFile(disp, VRoot.win, msk, &w, &h, &mask, &xh, &yh);
	XReadBitmapFile(disp, VRoot.win, img, &w, &h, &pmap, &xh, &yh);
	XQueryBestCursor(disp, VRoot.win, w, h, &ww, &hh);
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
	curs = (native_id == 999) ? None : XCreateFontCursor(disp, native_id);
     }

   ec = Emalloc(sizeof(ECursor));
   ec->name = Estrdup(name);
   ec->file = Estrdup(image);
#if 0				/* Not used */
   ec->fg = *fg;
   ec->bg = *bg;
#endif
   ec->cursor = curs;
   ec->ref_count = 0;
#if 0				/* Not used */
   ec->inroot = 0;
#endif

   AddItem(ec, ec->name, 0, LIST_TYPE_ECURSOR);

   return ec;
}

void
ECursorApply(ECursor * ec, Window win)
{
   if (!ec)
      return;
   XDefineCursor(disp, win, ec->cursor);
#if 0				/* Not used */
   if (win == VRoot.win)
      ec->inroot = 1;
#endif
}

static              Cursor
ECursorGetByName(const char *name, unsigned int fallback)
{
   ECursor            *ec;

   ec = FindItem(name, 0, LIST_FINDBY_NAME, LIST_TYPE_ECURSOR);
   if (!ec)
      return XCreateFontCursor(disp, fallback);

   ECursorIncRefcount(ec);

   return ec->cursor;
}

void
ECursorDestroy(ECursor * ec)
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

void
ECursorIncRefcount(ECursor * ec)
{
   if (ec)
      ec->ref_count++;
}

void
ECursorDecRefcount(ECursor * ec)
{
   if (ec)
      ec->ref_count--;
}

const char         *
ECursorGetName(ECursor * ec)
{
   return (ec) ? ec->name : 0;
}

int
ECursorGetRefcount(ECursor * ec)
{
   return (ec) ? ec->ref_count : 0;
}

static Cursor       ECsrs[ECSR_COUNT];

Cursor
ECsrGet(int which)
{
   return (which >= 0 && which < ECSR_COUNT) ? ECsrs[which] : None;
}

void
ECsrApply(int which, Window win)
{
   XDefineCursor(disp, win, ECsrGet(which));
}

/*
 * Set up some basic cursors
 */
void
ECursorsInit(void)
{
   ECsrs[ECSR_NONE] = None;
   ECsrs[ECSR_ROOT] = ECursorGetByName("DEFAULT", XC_left_ptr);
   ECsrs[ECSR_GRAB] = ECursorGetByName("GRAB", XC_crosshair);
   ECsrs[ECSR_ACT_MOVE] = ECursorGetByName("GRAB_MOVE", XC_fleur);
   ECsrs[ECSR_ACT_RESIZE] = ECursorGetByName("GRAB_RESIZE", XC_sizing);
}
