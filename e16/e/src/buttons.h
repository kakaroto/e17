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
#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#include "eobj.h"
#include "etypes.h"

typedef void        (ButtonCbFunc) (EObj * eo, XEvent * ev, ActionClass * ac);

/* buttons.c */
Button             *ButtonCreate(const char *name, int id,
				 ImageClass * ic,
				 ActionClass * ac, TextClass * tc,
				 const char *label, char ontop, int flags,
				 int minw, int maxw, int minh, int maxh, int xo,
				 int yo, int xa, int xr, int ya, int yr,
				 int xsr, int xsa, int ysr, int ysa, char simg,
				 int desk, char sticky);
void                ButtonDestroy(Button * b);
Button             *ButtonFind(const char *name);
void                ButtonShow(Button * b);
void                ButtonHide(Button * b);
void                ButtonMoveToCoord(Button * b, int x, int y);
void                ButtonMoveRelative(Button * b, int dx, int dy);
void                ButtonIncRefcount(Button * b);
void                ButtonDecRefcount(Button * b);
void                ButtonSwallowInto(Button * b, EObj * eo);
void                ButtonSetCallback(Button * b, ButtonCbFunc * func,
				      EObj * eo);
int                 ButtonGetInfo(const Button * b, RectBox * r, Desk * d);
int                 ButtonDoShowDefault(const Button * b);
int                 ButtonEmbedWindow(Button * ButtonToUse,
				      Window WindowToEmbed);

Button            **ButtonsGetList(int *pnum);
void                ButtonsForeach(int id, Desk * dsk,
				   void (*func) (Button * b));
void                ButtonsMoveStickyToDesk(Desk * d);
int                 ButtonsConfigLoad(FILE * fs);

#endif /* _BUTTONS_H_ */
