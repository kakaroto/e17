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

TextClass          *
CreateTclass()
{
   TextClass          *t;

   EDBUG(5, "CreateTclass");
   t = Emalloc(sizeof(TextClass));
   if (!t)
      EDBUG_RETURN(NULL);
   t->name = NULL;
   t->norm.normal = t->norm.hilited = t->norm.clicked = t->norm.disabled = NULL;
   t->active.normal = t->active.hilited = t->active.clicked =
      t->active.disabled = NULL;
   t->sticky.normal = t->sticky.hilited = t->sticky.clicked =
      t->sticky.disabled = NULL;
   t->sticky_active.normal = t->sticky_active.hilited =
      t->sticky_active.clicked = t->sticky_active.disabled = NULL;
   t->justification = 512;
   t->ref_count = 0;
   EDBUG_RETURN(t);
}

void
FreeTextState(TextState * ts)
{
   if (ts->fontname)
      Efree(ts->fontname);
#if USE_FNLIB
   if (ts->font)
      Fnlib_free_font(pFnlibData, ts->font);
#endif
   if (ts->xfont)
      XFreeFont(disp, ts->xfont);
   if (ts->efont)
      Efont_free(ts->efont);
   Efree(ts);
}

void
DeleteTclass(TextClass * t)
{

   if (t->ref_count > 0)
     {
	DialogOK(_("TextClass Error!"), _("%u references remain\n"),
		 t->ref_count);
	EDBUG_RETURN_;
     }
   if (t->name)
      Efree(t->name);
   if (t->norm.normal)
      FreeTextState(t->norm.normal);
   if (t->norm.hilited)
      FreeTextState(t->norm.hilited);
   if (t->norm.clicked)
      FreeTextState(t->norm.clicked);
   if (t->norm.disabled)
      FreeTextState(t->norm.disabled);
   if (t->active.normal)
      FreeTextState(t->active.normal);
   if (t->active.hilited)
      FreeTextState(t->active.hilited);
   if (t->active.clicked)
      FreeTextState(t->active.clicked);
   if (t->active.disabled)
      FreeTextState(t->active.disabled);
   if (t->sticky.normal)
      FreeTextState(t->sticky.normal);
   if (t->sticky.hilited)
      FreeTextState(t->sticky.hilited);
   if (t->sticky.clicked)
      FreeTextState(t->sticky.clicked);
   if (t->sticky.disabled)
      FreeTextState(t->sticky.disabled);
   if (t->sticky_active.normal)
      FreeTextState(t->sticky_active.normal);
   if (t->sticky_active.hilited)
      FreeTextState(t->sticky_active.hilited);
   if (t->sticky_active.clicked)
      FreeTextState(t->sticky_active.clicked);
   if (t->sticky_active.disabled)
      FreeTextState(t->sticky_active.disabled);
   Efree(t);
}

TextState          *
CreateTextState()
{

   TextState          *ts;

   EDBUG(6, "CreateTextState");

   ts = Emalloc(sizeof(TextState));

   if (!ts)
      EDBUG_RETURN(NULL);

   ts->fontname = NULL;
   ts->style.mode = MODE_WRAP_CHAR;
   ts->style.orientation = FONT_TO_RIGHT;
#if USE_FNLIB
   ts->style.justification = 0;
   ts->style.spacing = 0;
   ts->font = NULL;
#endif
   ts->efont = NULL;
   ts->xfont = NULL;
   ts->xfontset = 0;

   EDBUG_RETURN(ts);

}

void
TclassPopulate(TextClass * tclass)
{

   EDBUG(6, "TclassPopulate");

   if (!tclass)
      EDBUG_RETURN_;

   if (!tclass->norm.normal)
      EDBUG_RETURN_;

   if (!tclass->norm.hilited)
      tclass->norm.hilited = tclass->norm.normal;
   if (!tclass->norm.clicked)
      tclass->norm.clicked = tclass->norm.normal;
   if (!tclass->norm.disabled)
      tclass->norm.disabled = tclass->norm.normal;

   if (!tclass->active.normal)
      tclass->active.normal = tclass->norm.normal;
   if (!tclass->active.hilited)
      tclass->active.hilited = tclass->active.normal;
   if (!tclass->active.clicked)
      tclass->active.clicked = tclass->active.normal;
   if (!tclass->active.disabled)
      tclass->active.disabled = tclass->active.normal;

   if (!tclass->sticky.normal)
      tclass->sticky.normal = tclass->norm.normal;
   if (!tclass->sticky.hilited)
      tclass->sticky.hilited = tclass->sticky.normal;
   if (!tclass->sticky.clicked)
      tclass->sticky.clicked = tclass->sticky.normal;
   if (!tclass->sticky.disabled)
      tclass->sticky.disabled = tclass->sticky.normal;

   if (!tclass->sticky_active.normal)
      tclass->sticky_active.normal = tclass->norm.normal;
   if (!tclass->sticky_active.hilited)
      tclass->sticky_active.hilited = tclass->sticky_active.normal;
   if (!tclass->sticky_active.clicked)
      tclass->sticky_active.clicked = tclass->sticky_active.normal;
   if (!tclass->sticky_active.disabled)
      tclass->sticky_active.disabled = tclass->sticky_active.normal;

   EDBUG_RETURN_;

}

void
TclassApply(ImageClass * iclass, Window win, int w, int h, int active,
	    int sticky, int state, char expose, TextClass * tclass,
	    const char *text)
{

   EDBUG(4, "TclassApply");

   if ((!iclass) || (!tclass) || (!win) || (w < 1) || (h < 1))
      EDBUG_RETURN_;

   if (queue_up)
     {
	DrawQueue          *dq;

	dq = Emalloc(sizeof(DrawQueue));
	dq->win = win;
	dq->iclass = iclass;
	if (dq->iclass)
	   dq->iclass->ref_count++;
	dq->w = w;
	dq->h = h;
	dq->active = active;
	dq->sticky = sticky;
	dq->state = state;
	dq->expose = expose;
	dq->tclass = tclass;
	if (dq->tclass)
	   dq->tclass->ref_count++;
	if (text)
	   dq->text = Estrdup(text);
	else
	   dq->text = NULL;
	dq->w = w;
	dq->shape_propagate = 0;
	dq->pager = NULL;
	dq->redraw_pager = NULL;
	dq->d = NULL;
	dq->di = NULL;
	dq->x = 0;
	dq->y = 0;
	AddItem(dq, "DRAW", dq->win, LIST_TYPE_DRAW);
	EDBUG_RETURN_;
     }
   XClearWindow(disp, win);

   TextDraw(tclass, win, active, sticky, state, text, iclass->padding.left,
	    iclass->padding.top,
	    w - (iclass->padding.left + iclass->padding.right),
	    h - (iclass->padding.top + iclass->padding.bottom),
	    h - (iclass->padding.top + iclass->padding.bottom),
	    tclass->justification);

   EDBUG_RETURN_;

}
