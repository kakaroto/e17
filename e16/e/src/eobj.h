/*
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
#ifndef _EOBJ_H_
#define _EOBJ_H_

#include "etypes.h"

typedef struct _eobj EObj;

struct _eobj
{
   Window              win;	/* The top level window */
   short               type;	/* Ewin, button, other, ... */
   short               ilayer;	/* Internal stacking layer */
   short               layer;	/* Stacking layer */
   Desk               *desk;	/* Belongs on desk */
   int                 x, y;
   int                 w, h;
   signed char         stacked;
   signed char         shaped;
   char                sticky;
   char                floating;
   unsigned            external:1;
   unsigned            inputonly:1;
   unsigned            shown:1;
   unsigned            gone:1;
   unsigned            noredir:1;	/* Do not redirect */
   unsigned            shadow:1;	/* Enable shadows */
   unsigned            fade:1;
#if USE_COMPOSITE
   unsigned int        opacity;
   void               *cmhook;
#endif
   char               *name;
};

#define EOBJ_TYPE_EWIN      0
#define EOBJ_TYPE_BUTTON    1
#define EOBJ_TYPE_DESK      2
#define EOBJ_TYPE_MISC      3
#define EOBJ_TYPE_EVENT     4	/* Unredirected */
#define EOBJ_TYPE_EXT       5
#define EOBJ_TYPE_MISC_NR   6	/* Unredirected */
#define EOBJ_TYPE_ROOT_BG   7

#define EobjGetWin(eo)          ((eo)->win)
#define EobjGetXwin(eo)         ((eo)->win)
#define EobjGetDesk(eo)         ((eo)->desk)
#define EobjGetName(eo)         ((eo)->name)
#define EobjGetType(eo)         ((eo)->type)
#define EobjGetX(eo)            ((eo)->x)
#define EobjGetY(eo)            ((eo)->y)
#define EobjGetW(eo)            ((eo)->w)
#define EobjGetH(eo)            ((eo)->h)

#define EoObj(eo)               (&((eo)->o))
#define EoGetWin(eo)            EobjGetWin(EoObj(eo))
#define EoGetXwin(eo)           EobjGetXwin(EoObj(eo))
#define EoGetDesk(eo)           EobjGetDesk(EoObj(eo))
#define EoGetName(eo)           EobjGetName(EoObj(eo))
#define EoGetType(eo)           EobjGetType(EoObj(eo))
#define EoGetX(eo)              EobjGetX(EoObj(eo))
#define EoGetY(eo)              EobjGetY(EoObj(eo))
#define EoGetW(eo)              EobjGetW(EoObj(eo))
#define EoGetH(eo)              EobjGetH(EoObj(eo))
#define EoIsGone(eo)            (EoObj(eo)->gone)
#define EoIsSticky(eo)          (EoObj(eo)->sticky)
#define EoIsFloating(eo)        (EoObj(eo)->floating)
#define EoIsShown(eo)           (EoObj(eo)->shown)
#define EoGetDeskNum(eo)        (EoGetDesk(eo)->num)
#define EoGetLayer(eo)          (EoObj(eo)->layer)
#define EoGetPixmap(eo)         EobjGetPixmap(EoObj(eo))

#define EoSetName(eo, _x)       EoObj(eo)->name = (_x)
#define EoSetGone(eo)           EoObj(eo)->gone = 1
#define EoSetSticky(eo, _x)     EoObj(eo)->sticky = ((_x)?1:0)
#define EoSetFloating(eo, _f)   EobjSetFloating(EoObj(eo), (_f))
#define EoSetDesk(eo, _x)       EoObj(eo)->desk = (_x)
#define EoSetLayer(eo, _l)      EobjSetLayer(EoObj(eo), (_l))
#define EoChangeOpacity(eo, _o) EobjChangeOpacity(EoObj(eo), _o)
#define EoSetFade(eo, _x)       EoObj(eo)->fade = (_x)
#define EoSetNoRedirect(eo, _x) EoObj(eo)->noredir = (_x)
#define EoSetShadow(eo, _x)     EoObj(eo)->shadow = (_x)
#if USE_COMPOSITE
#define EoSetOpacity(eo, _o)    EoObj(eo)->opacity = (_o)
#define EoGetOpacity(eo)        (EoObj(eo)->opacity)
#define EoGetShadow(eo)         (EoObj(eo)->shadow)
#define EoGetNoRedirect(eo)     (EoObj(eo)->noredir)
#else
#define EoSetOpacity(eo, _o)
#endif

#define EoInit(eo, type, win, x, y, w, h, su, name) \
					EobjInit(EoObj(eo), type, win, x, y, w, h, su, name)
#define EoFini(eo)			EobjFini(EoObj(eo));
#define EoSync(eo)                      EobjSync(EoObj(eo))
#define EoMap(eo, raise)                EobjMap(EoObj(eo), raise)
#define EoUnmap(eo)                     EobjUnmap(EoObj(eo))
#define EoMove(eo, x, y)                EobjMove(EoObj(eo), x, y)
#define EoResize(eo, w, h)              EobjResize(EoObj(eo), w, h)
#define EoMoveResize(eo, x, y, w, h)    EobjMoveResize(EoObj(eo), x, y, w, h)
#define EoReparent(eo, d, x, y)         EobjReparent(EoObj(eo), d, x, y)
#define EoRaise(eo)                     EobjRaise(EoObj(eo))
#define EoLower(eo)                     EobjLower(EoObj(eo))
#define EoShapeUpdate(eo, p)            EobjShapeUpdate(EoObj(eo), p)

/* eobj.c */
void                EobjInit(EObj * eo, int type, Window win, int x, int y,
			     int w, int h, int su, const char *name);
void                EobjFini(EObj * eo);
void                EobjDestroy(EObj * eo);
void                EobjSync(EObj * eo);

EObj               *EobjWindowCreate(int type, int x, int y, int w, int h,
				     int su, const char *name);
void                EobjWindowDestroy(EObj * eo);

EObj               *EobjRegister(Window win, int type);
void                EobjUnregister(EObj * eo);

void                EobjMap(EObj * eo, int raise);
void                EobjUnmap(EObj * eo);
void                EobjMove(EObj * eo, int x, int y);
void                EobjResize(EObj * eo, int w, int h);
void                EobjMoveResize(EObj * eo, int x, int y, int w, int h);
void                EobjDamage(EObj * eo);
void                EobjReparent(EObj * eo, EObj * dst, int x, int y);
int                 EobjRaise(EObj * eo);
int                 EobjLower(EObj * eo);
void                EobjShapeUpdate(EObj * eo, int propagate);
void                EobjsRepaint(void);
Pixmap              EobjGetPixmap(const EObj * eo);
void                EobjChangeOpacity(EObj * eo, unsigned int opacity);
void                EobjSetLayer(EObj * eo, int layer);
void                EobjSetFloating(EObj * eo, int floating);
int                 EobjIsShaped(const EObj * eo);
void                EobjSlideTo(EObj * eo, int fx, int fy, int tx, int ty,
				int speed);
void                EobjsSlideBy(EObj ** peo, int num, int dx, int dy,
				 int speed);
void                EobjSlideSizeTo(EObj * eo, int fx, int fy, int tx, int ty,
				    int fw, int fh, int tw, int th, int speed);

/* stacking.c */
void                EobjListStackAdd(EObj * eo, int ontop);
void                EobjListStackDel(EObj * eo);
int                 EobjListStackRaise(EObj * eo);
int                 EobjListStackLower(EObj * eo);
int                 EobjListStackCheck(EObj * eo);
EObj               *EobjListStackFind(Window win);
EObj               *const *EobjListStackGet(int *num);
EObj               *const *EobjListStackGetForDesk(int *num, Desk * dsk);
void                EobjListFocusAdd(EObj * eo, int ontop);
void                EobjListFocusDel(EObj * eo);
int                 EobjListFocusRaise(EObj * eo);
int                 EobjListFocusLower(EObj * eo);
void                EobjListOrderAdd(EObj * eo);
void                EobjListOrderDel(EObj * eo);

/* Hmmm. */
int                 OpacityFix(int op);
unsigned int        OpacityFromPercent(int op);
int                 OpacityToPercent(unsigned int opacity);

#endif /* _EOBJ_H_ */
