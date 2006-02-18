/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2006 Kim Woelders
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
#ifndef _BORDERS_H_
#define _BORDERS_H_

typedef struct _winpoint
{
   int                 originbox;
   struct
   {
      int                 percent;
      int                 absolute;
   }
   x                  , y;
}
WinPoint;

typedef struct _geometry
{
   Constraints         width, height;
   WinPoint            topleft, bottomright;
}
Geometry;

typedef struct _winpart
{
   Geometry            geom;
   struct _imageclass *iclass;
   struct _actionclass *aclass;
   struct _textclass  *tclass;
   ECursor            *ec;
   signed char         ontop;
   int                 flags;
   char                keep_for_shade;
}
WinPart;

typedef struct _border
{
   char               *name;
   char               *group_border_name;
   Imlib_Border        border;
   int                 num_winparts;
   WinPart            *part;
   char                changes_shape;
   char                shadedir;
   char                throwaway;
   unsigned int        ref_count;
   struct _actionclass *aclass;
}
Border;

typedef struct _ewinbit
{
   EWin               *ewin;	/* Belongs to */
   Window              win;
   int                 x, y, w, h;
   int                 cx, cy, cw, ch;
   int                 state;
   char                expose;
   char                no_expose;
   char                left;
   struct _imagestate *is;
   struct _textstate  *ts;
}
EWinBit;

/* borders.c */
Border             *BorderFind(const char *name);
void                BorderIncRefcount(const Border * b);
void                BorderDecRefcount(const Border * b);
const char         *BorderGetName(const Border * b);
int                 BorderConfigLoad(FILE * fs);
void                EwinBorderSelect(EWin * ewin);
void                EwinBorderDetach(EWin * ewin);
void                EwinBorderSetTo(EWin * ewin, const Border * b);
void                EwinBorderDraw(EWin * ewin, int do_shape, int do_paint);
void                EwinBorderCalcSizes(EWin * ewin, int propagate);
void                EwinBorderMinShadeSize(EWin * ewin, int *mw, int *mh);
void                EwinBorderUpdateInfo(EWin * ewin);
void                EwinBorderEventsConfigure(EWin * ewin, int mode);
void                EwinSetBorder(EWin * ewin, const Border * b, int apply);
void                EwinSetBorderByName(EWin * ewin, const char *name);
int                 BorderWinpartIndex(EWin * ewin, Window win);
Border             *BorderCreateFiller(int left, int right, int top,
				       int bottom);
void                BordersForeach(void (*func) (Border * b, void *data),
				   void *data);
Border            **BordersGetList(int *pnum);
void                BordersSetupFallback(void);

#endif /* _BORDERS_H_ */
