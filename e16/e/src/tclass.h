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
#ifndef _TCLASS_H
#define _TCLASS_H

#include "xwin.h"

struct _imageclass;

typedef struct _textclass TextClass;
typedef struct _textstate TextState;

#define MODE_VERBATIM  0
#define MODE_WRAP_CHAR 1
#define MODE_WRAP_WORD 2

#define FONT_TO_RIGHT 0
#define FONT_TO_DOWN  1
#define FONT_TO_UP    2
#define FONT_TO_LEFT  3

struct _textstate
{
   char               *fontname;
   struct
   {
      char                mode;
      char                orientation;
   } style;
   XColor              fg_col;
   XColor              bg_col;
   int                 effect;
   Efont              *efont;
   XFontStruct        *xfont;
   XFontSet            xfontset;
   int                 xfontset_ascent;
   char                need_utf8;
};

struct _textclass
{
   char               *name;
   struct
   {
      TextState          *normal;
      TextState          *hilited;
      TextState          *clicked;
      TextState          *disabled;
   }
   norm               , active, sticky, sticky_active;
   int                 justification;
   unsigned int        ref_count;
};

/* tclass.c */
int                 TextclassConfigLoad(FILE * fs);
void                TextclassIncRefcount(TextClass * tc);
void                TextclassDecRefcount(TextClass * tc);
int                 TextclassGetJustification(TextClass * tc);
TextClass          *TextclassFind(const char *name, int fallback);
void                TextclassApply(struct _imageclass *ic, Window win, int w,
				   int h, int active, int sticky, int state,
				   char expose, TextClass * tclass,
				   const char *text);

/* text.c */
TextState          *TextclassGetTextState(TextClass * tclass, int state,
					  int active, int sticky);
void                TextstateDrawText(TextState * ts, Window win,
				      const char *text, int x, int y, int w,
				      int h, int fsize, int justification);
void                TextSize(TextClass * tclass, int active, int sticky,
			     int state, const char *text, int *width,
			     int *height, int fsize);
void                TextDraw(TextClass * tclass, Window win, int active,
			     int sticky, int state, const char *text, int x,
			     int y, int w, int h, int fsize, int justification);

#endif /* _TCLASS_H */
