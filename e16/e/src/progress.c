/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "eobj.h"
#include "iclass.h"
#include "tclass.h"
#include "xwin.h"

struct _progressbar
{
   EObj               *win;
   EObj               *n_win;
   EObj               *p_win;
   int                 w, h;
   int                 value;
   ImageClass         *ic, *inc, *ipc;
   TextClass          *tc, *tnc;
};

static int          pnum = 0;
static Progressbar **plist = NULL;

Progressbar        *
ProgressbarCreate(const char *name, int w, int h)
{
   Progressbar        *p;
   int                 x, y;

   p = Ecalloc(1, sizeof(Progressbar));
   pnum++;
   plist = Erealloc(plist, pnum * sizeof(Progressbar *));
   plist[pnum - 1] = p;

   p->w = w;
   p->h = h;
   p->value = 0;

   x = (VRoot.w - w) / 2;
   y = 32 + (pnum * h * 2);

   p->win = EobjWindowCreate(EOBJ_TYPE_MISC, x, y, w - (h * 5), h, 1, name);
   p->n_win =
      EobjWindowCreate(EOBJ_TYPE_MISC, x + w - (h * 5), y, (h * 5), h, 1, "pn");
   p->p_win = EobjWindowCreate(EOBJ_TYPE_MISC, x, y + h, 1, h, 1, "pp");
   if (!p->win || !p->n_win || !p->p_win)
     {
	ProgressbarDestroy(p);
	return NULL;
     }
   p->win->fade = 0;
   p->n_win->fade = 0;
   p->p_win->fade = 0;

   p->ic = ImageclassFind("PROGRESS_BAR", 1);
   if (p->ic)
      ImageclassIncRefcount(p->ic);

   p->inc = ImageclassFind("PROGRESS_BAR", 1);
   if (p->inc)
      ImageclassIncRefcount(p->inc);

   p->ipc = ImageclassFind("PROGRESS_BAR", 1);
   if (p->ipc)
      ImageclassIncRefcount(p->ipc);

   p->tc = TextclassFind("PROGRESS_TEXT", 1);
   if (p->tc)
      TextclassIncRefcount(p->tc);

   p->tnc = TextclassFind("PROGRESS_TEXT_NUMBER", 1);
   if (p->tnc)
      TextclassIncRefcount(p->tnc);

   return p;
}

void
ProgressbarDestroy(Progressbar * p)
{
   int                 i, j, dy;

   ProgressbarHide(p);

   dy = 2 * p->h;
   EobjWindowDestroy(p->win);
   EobjWindowDestroy(p->n_win);
   EobjWindowDestroy(p->p_win);

   for (i = 0; i < pnum; i++)
     {
	if (plist[i] != p)
	   continue;

	for (j = i; j < pnum - 1; j++)
	  {
	     Progressbar        *pp;

	     pp = plist[j + 1];
	     plist[j] = pp;
	     EobjMove(pp->win, pp->win->x, pp->win->y - dy);
	     EobjMove(pp->n_win, pp->n_win->x, pp->n_win->y - dy);
	     EobjMove(pp->p_win, pp->p_win->x, pp->p_win->y - dy);
	  }
	break;
     }

   if (p->ic)
      ImageclassDecRefcount(p->ic);
   if (p->inc)
      ImageclassDecRefcount(p->inc);
   if (p->ipc)
      ImageclassDecRefcount(p->ipc);

   if (p->tc)
      TextclassDecRefcount(p->tc);
   if (p->tnc)
      TextclassDecRefcount(p->tnc);

   if (p)
      Efree(p);

   pnum--;
   if (pnum <= 0)
     {
	pnum = 0;
	_EFREE(plist);
     }
   else
     {
	plist = Erealloc(plist, pnum * sizeof(Progressbar *));
     }
}

void
ProgressbarSet(Progressbar * p, int progress)
{
   int                 w;
   char                s[64];
   EImageBorder       *pad;

   if (progress == p->value)
      return;

   p->value = progress;
   w = (p->value * p->w) / 100;
   if (w < 1)
      w = 1;
   if (w > p->w)
      w = p->w;
   Esnprintf(s, sizeof(s), "%i%%", p->value);

   EobjResize(p->p_win, w, p->h);
   ImageclassApply(p->inc, p->p_win->win, w, p->h, 1, 0, STATE_NORMAL,
		   ST_SOLID);
   EobjShapeUpdate(p->p_win, 0);

   pad = ImageclassGetPadding(p->inc);
   EClearWindow(p->n_win->win);
   TextDraw(p->tnc, Xwin(p->n_win->win), 0, 0, STATE_CLICKED, s,
	    pad->left, pad->top, p->h * 5 - (pad->left + pad->right),
	    p->h - (pad->top + pad->bottom), p->h - (pad->top + pad->bottom),
	    TextclassGetJustification(p->tnc));
   /* Hack - We may not be running in the event loop here */
   EobjDamage(p->n_win);

   EobjsRepaint();
}

void
ProgressbarShow(Progressbar * p)
{
   EImageBorder       *pad;

   ImageclassApply(p->ic, p->win->win, p->w - (p->h * 5), p->h, 0, 0,
		   STATE_NORMAL, ST_SOLID);
   ImageclassApply(p->inc, p->n_win->win, (p->h * 5), p->h, 0, 0, STATE_CLICKED,
		   ST_SOLID);
   ImageclassApply(p->ipc, p->p_win->win, 1, p->h, 1, 0, STATE_NORMAL,
		   ST_SOLID);

   EobjMap(p->win, 0);
   EobjMap(p->n_win, 0);
   EobjMap(p->p_win, 0);

   pad = ImageclassGetPadding(p->inc);
   TextDraw(p->tc, Xwin(p->win->win), 0, 0, STATE_NORMAL, p->win->name,
	    pad->left, pad->top, p->w - (p->h * 5) - (pad->left + pad->right),
	    p->h - (pad->top + pad->bottom), p->h - (pad->top + pad->bottom),
	    TextclassGetJustification(p->tnc));

   EobjsRepaint();
}

void
ProgressbarHide(Progressbar * p)
{
   EobjUnmap(p->win);
   EobjUnmap(p->n_win);
   EobjUnmap(p->p_win);
}
