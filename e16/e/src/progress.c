
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

static int          pnum = 0;
static Progressbar **plist = NULL;

Progressbar        *
CreateProgressbar(char *name, int width, int height)
{
   Progressbar        *p;

   EDBUG(5, "CreateProgressbar");

   p = Emalloc(sizeof(Progressbar));
   pnum++;
   plist = Erealloc(plist, pnum * sizeof(Progressbar *));
   plist[pnum - 1] = p;
   p->name = Estrdup(name);
   p->x = (root.w - width) / 2;
   p->y = 32 + (pnum * height * 2);
   p->w = width;
   p->h = height;
   p->win = ECreateWindow(root.win, p->x, p->y, p->w - (p->h * 5), p->h, 1);
   p->n_win =
      ECreateWindow(root.win, p->x + p->w - (p->h * 5), p->y, (p->h * 5),
		    p->h, 1);
   p->p_win = ECreateWindow(root.win, p->x, p->y + p->h, 1, p->h, 1);
   /* FIXME: need to use other image and textclasses */

   p->ic = FindItem("PROGRESS_BAR", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (p->ic)
      p->ic->ref_count++;

   p->inc = FindItem("PROGRESS_BAR", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (p->inc)
      p->inc->ref_count++;

   p->ipc = FindItem("PROGRESS_BAR", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (p->ipc)
      p->ipc->ref_count++;

   p->tc = FindItem("PROGRESS_TEXT", 0, LIST_FINDBY_NAME, LIST_TYPE_TCLASS);
   if (p->tc)
      p->tc->ref_count++;

   p->tnc =
      FindItem("PROGRESS_TEXT_NUMBER", 0, LIST_FINDBY_NAME, LIST_TYPE_TCLASS);
   if (p->tnc)
      p->tnc->ref_count++;

   p->value = 0;

   EDBUG_RETURN(p);
}

void
SetProgressbar(Progressbar * p, int progress)
{
   int                 w;
   char                s[64], pq;

   EDBUG(5, "SetProgressbar");

   if (progress == p->value)
      EDBUG_RETURN_;

   p->value = progress;
   w = (p->value * p->w) / 100;
   if (w < 1)
      w = 1;
   if (w > p->w)
      w = p->w;
   Esnprintf(s, sizeof(s), "%i%%", p->value);
   pq = Mode.queue_up;
   Mode.queue_up = 0;
   TclassApply(p->inc, p->n_win, p->h * 5, p->h, 0, 0, STATE_CLICKED, 0,
	       p->tnc, s);
   IclassApply(p->inc, p->p_win, w, p->h, 1, 0, STATE_NORMAL, 0);
   EResizeWindow(disp, p->p_win, w, p->h);
   Mode.queue_up = pq;
   XFlush(disp);

   EDBUG_RETURN_;
}

void
ShowProgressbar(Progressbar * p)
{
   int                 w;
   char                pq;

   EDBUG(5, "ShowProgressbar");
   w = (p->value * p->w) / 100;
   if (w < 1)
      w = 1;
   if (w > p->w)
      w = p->w;
   pq = Mode.queue_up;
   Mode.queue_up = 0;
   IclassApply(p->ic, p->win, p->w - (p->h * 5), p->h, 0, 0, STATE_NORMAL, 0);
   IclassApply(p->inc, p->n_win, (p->h * 5), p->h, 0, 0, STATE_CLICKED, 0);
   IclassApply(p->ipc, p->p_win, w, p->h, 1, 0, STATE_NORMAL, 0);
   EMapRaised(disp, p->win);
   EMapRaised(disp, p->n_win);
   EMapRaised(disp, p->p_win);
   XSync(disp, False);
   TclassApply(p->ic, p->win, p->w - (p->h * 5), p->h, 0, 0, STATE_NORMAL, 0,
	       p->tc, p->name);
   Mode.queue_up = pq;
   EDBUG_RETURN_;
}

void
HideProgressbar(Progressbar * p)
{
   EDBUG(5, "HideProgressbar");
   EUnmapWindow(disp, p->win);
   EUnmapWindow(disp, p->n_win);
   EUnmapWindow(disp, p->p_win);
   EDBUG_RETURN_;
}

void
FreeProgressbar(Progressbar * p)
{
   int                 i, j;

   EDBUG(5, "FreeProgressbar");

   if (p->name)
      Efree(p->name);
   if (p->win)
      EDestroyWindow(disp, p->win);
   if (p->win)
      EDestroyWindow(disp, p->n_win);
   if (p->win)
      EDestroyWindow(disp, p->p_win);

   for (i = 0; i < pnum; i++)
     {
	if (plist[i] == p)
	  {
	     for (j = i; j < (pnum - 1); j++)
	       {
		  plist[j] = plist[j + 1];
		  plist[j]->y -= p->h;
		  EMoveWindow(disp, p->win, plist[j]->x, plist[j]->y);
		  EMoveWindow(disp, p->n_win,
			      plist[j]->x + plist[j]->w - (plist[j]->h * 5),
			      plist[j]->y);
		  EMoveWindow(disp, p->p_win, plist[j]->x,
			      plist[j]->y + plist[j]->h);
	       }
	     i = pnum;
	  }
     }

   if (p->ic)
      p->ic->ref_count--;
   if (p->inc)
      p->inc->ref_count--;
   if (p->ipc)
      p->ipc->ref_count--;

   if (p->tc)
      p->tc->ref_count--;
   if (p->tnc)
      p->tnc->ref_count--;

   if (p)
      Efree(p);

   pnum--;
   if (pnum <= 0)
     {
	pnum = 0;
	if (plist)
	   Efree(plist);
	plist = NULL;
     }
   else
     {
	plist = Erealloc(plist, pnum * sizeof(Progressbar *));
     }

   EDBUG_RETURN_;
}

Window             *
ListProgressWindows(int *num)
{
   int                 i, j;
   Window             *wl;

   *num = pnum * 3;
   if (pnum > 0)
     {
	j = 0;
	wl = Emalloc(sizeof(Window) * pnum * 3);
	for (i = 0; i < pnum; i++)
	  {
	     wl[j++] = plist[i]->win;
	     wl[j++] = plist[i]->n_win;
	     wl[j++] = plist[i]->p_win;
	  }
	return wl;
     }

   return NULL;
}

void
RaiseProgressbars(void)
{
   int                 i;

   for (i = 0; i < pnum; i++)
     {
	XRaiseWindow(disp, plist[i]->win);
	XRaiseWindow(disp, plist[i]->n_win);
	XRaiseWindow(disp, plist[i]->p_win);
     }

   return;
}
