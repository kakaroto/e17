/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison
 *                         and various contributors
 * Copyright (C) 2004-2005 Kim Woelders
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
#ifndef _ICLASS_H
#define _ICLASS_H

#include "xwin.h"

struct _textclass;
struct _textstate;

typedef struct _imageclass ImageClass;
typedef struct _imagestate ImageState;

/* Selective Transparency item types */
#define ST_UNKNWN	0
#define ST_BORDER	1
#define ST_WIDGET	2
#define ST_ICONBOX	3
#define ST_MENU		4
#define ST_MENU_ITEM	5
#define ST_TOOLTIP	6
#define ST_DIALOG	7
#define ST_HILIGHT	8
#define ST_PAGER	9
#define ST_WARPLIST	10
#define ST_BUTTON	11

/* iclass.c */
int                 ImageclassConfigLoad(FILE * fs);

#ifdef ENABLE_THEME_TRANSPARENCY
void                TransparencySet(int transparency);
int                 TransparencyEnabled(void);
int                 TransparencyUpdateNeeded(void);
int                 ImageclassIsTransparent(ImageClass * ic);
#else
#define TransparencyEnabled() 0
#define TransparencyUpdateNeeded() 0
#define ImageclassIsTransparent(ic) 0
#endif
void                ImageclassIncRefcount(ImageClass * ic);
void                ImageclassDecRefcount(ImageClass * ic);
const char         *ImageclassGetName(ImageClass * ic);
Imlib_Border       *ImageclassGetPadding(ImageClass * ic);
ImageState         *ImageclassGetImageState(ImageClass * ic, int state,
					    int active, int sticky);
ImageClass         *ImageclassCreateSimple(const char *name, const char *image);
ImageClass         *ImageclassFind(const char *name, int fallback);
Imlib_Image        *ImageclassGetImage(ImageClass * ic, int active, int sticky,
				       int state);
Pixmap              ImageclassApplySimple(ImageClass * ic, Window win,
					  Drawable draw, int state, int x,
					  int y, int w, int h);
void                ImageclassApply(ImageClass * ic, Window win, int w, int h,
				    int active, int sticky, int state,
				    char expose, int image_type);
void                ImageclassApplyCopy(ImageClass * ic, Window win, int w,
					int h, int active, int sticky,
					int state, PmapMask * pmm,
					int make_mask, int image_type);
void                ITApply(Window win, ImageClass * ic, ImageState * is, int w,
			    int h, int state, int active, int sticky,
			    char expose, int image_type, struct _textclass *tc,
			    struct _textstate *ts, const char *text);

#endif /* _ICLASS_H */
