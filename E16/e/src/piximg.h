/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
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
#ifndef _PIXIMG_H_
#define _PIXIMG_H_

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

typedef struct _PixImg {
   XImage             *xim;
   XShmSegmentInfo    *shminfo;
   Pixmap              pmap;
   GC                  gc;
} PixImg;

void                EFillPixmap(Window win, Pixmap pmap, int x, int y, int w,
				int h);
void                EPastePixmap(Window win, Pixmap pmap, int x, int y, int w,
				 int h);

PixImg             *ECreatePixImg(Window win, int w, int h);
void                EDestroyPixImg(PixImg * pi);
void                EBlendRemoveShape(Win win, Pixmap pmap, int x, int y);
void                EBlendPixImg(Win win, PixImg * s1, PixImg * s2,
				 PixImg * dst, int x, int y, int w, int h);

#endif /* _PIXIMG_H_ */
