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

void
SetupFallbackClasses(void)
{

   /*
    * This function creates simple internal data members to be used in 
    * emergencies - ie when all else fails - ie a button is told to use an
    * imageclass that doesn't exist, or no DEFAULT border is defined... at 
    * least E won't barf on us then.
    */

   Border             *b;
   ImageClass         *ic;
   ActionClass        *ac;
   Action             *a;
   Background         *bg;
   TextClass          *tc;

   EDBUG(5, "SetupFallbackClasses");

   /* create a default fallback actionclass for the fallback border */
   ac = CreateAclass("__FALLBACK_ACTION");
   AddItem(ac, ac->name, 0, LIST_TYPE_ACLASS);
   a = CreateAction(EVENT_MOUSE_DOWN, 1, 0, 0, 1, 0, NULL, NULL);
   AddAction(ac, a);
   AddToAction(a, ACTION_MOVE, NULL);
   a = CreateAction(EVENT_MOUSE_DOWN, 1, 0, 0, 2, 0, NULL, NULL);
   AddAction(ac, a);
   AddToAction(a, ACTION_KILL, NULL);
   a = CreateAction(EVENT_MOUSE_DOWN, 1, 0, 0, 3, 0, NULL, NULL);
   AddAction(ac, a);
   AddToAction(a, ACTION_RESIZE, NULL);

   /* create a fallback imageclass in case no imageclass can be found */
   ic = CreateIclass();
   ic->name = Estrdup("__FALLBACK_ICLASS");
   ic->norm.normal = CreateImageState();
   ESetColor(&(ic->norm.normal->hihi), 255, 255, 255);
   ESetColor(&(ic->norm.normal->hi), 255, 255, 255);
   ESetColor(&(ic->norm.normal->bg), 160, 160, 160);
   ESetColor(&(ic->norm.normal->lo), 0, 0, 0);
   ESetColor(&(ic->norm.normal->lolo), 0, 0, 0);
   ic->norm.normal->bevelstyle = BEVEL_AMIGA;

   ic->norm.hilited = CreateImageState();
   ESetColor(&(ic->norm.hilited->hihi), 255, 255, 255);
   ESetColor(&(ic->norm.hilited->hi), 255, 255, 255);
   ESetColor(&(ic->norm.hilited->bg), 192, 192, 192);
   ESetColor(&(ic->norm.hilited->lo), 0, 0, 0);
   ESetColor(&(ic->norm.hilited->lolo), 0, 0, 0);
   ic->norm.hilited->bevelstyle = BEVEL_AMIGA;

   ic->norm.clicked = CreateImageState();
   ESetColor(&(ic->norm.clicked->hihi), 0, 0, 0);
   ESetColor(&(ic->norm.clicked->hi), 0, 0, 0);
   ESetColor(&(ic->norm.clicked->bg), 192, 192, 192);
   ESetColor(&(ic->norm.clicked->lo), 255, 255, 255);
   ESetColor(&(ic->norm.clicked->lolo), 255, 255, 255);
   ic->norm.clicked->bevelstyle = BEVEL_AMIGA;

   ic->active.normal = CreateImageState();
   ESetColor(&(ic->active.normal->hihi), 255, 255, 255);
   ESetColor(&(ic->active.normal->hi), 255, 255, 255);
   ESetColor(&(ic->active.normal->bg), 180, 140, 160);
   ESetColor(&(ic->active.normal->lo), 0, 0, 0);
   ESetColor(&(ic->active.normal->lolo), 0, 0, 0);
   ic->active.normal->bevelstyle = BEVEL_AMIGA;

   ic->active.hilited = CreateImageState();
   ESetColor(&(ic->active.hilited->hihi), 255, 255, 255);
   ESetColor(&(ic->active.hilited->hi), 255, 255, 255);
   ESetColor(&(ic->active.hilited->bg), 230, 190, 210);
   ESetColor(&(ic->active.hilited->lo), 0, 0, 0);
   ESetColor(&(ic->active.hilited->lolo), 0, 0, 0);
   ic->active.hilited->bevelstyle = BEVEL_AMIGA;

   ic->active.clicked = CreateImageState();
   ESetColor(&(ic->active.clicked->hihi), 0, 0, 0);
   ESetColor(&(ic->active.clicked->hi), 0, 0, 0);
   ESetColor(&(ic->active.clicked->bg), 230, 190, 210);
   ESetColor(&(ic->active.clicked->lo), 255, 255, 255);
   ESetColor(&(ic->active.clicked->lolo), 255, 255, 255);
   ic->active.clicked->bevelstyle = BEVEL_AMIGA;

   IclassPopulate(ic);
   AddItem(ic, ic->name, 0, LIST_TYPE_ICLASS);

   /* create a fallback border in case no border is found */
   b = CreateBorder("__FALLBACK_BORDER");
   b->border.left = 8;
   b->border.right = 8;
   b->border.top = 8;
   b->border.bottom = 8;
   AddBorderPart(b, ic, ac, NULL, NULL, 1, FLAG_BUTTON, 0, 8, 99999, 8, 99999,
		 -1, 0, 0, 0, 0, -1, 1024, -1, 0, 7, 1);
   AddBorderPart(b, ic, ac, NULL, NULL, 1, FLAG_BUTTON, 0, 8, 99999, 8, 99999,
		 -1, 0, 0, 1024, -8, -1, 1024, -1, 1024, -1, 1);
   AddBorderPart(b, ic, ac, NULL, NULL, 1, FLAG_BUTTON, 0, 8, 99999, 8, 99999,
		 -1, 0, 0, 0, 8, -1, 0, 7, 1024, -9, 1);

   AddBorderPart(b, ic, ac, NULL, NULL, 1, FLAG_BUTTON, 0, 8, 99999, 8, 99999,
		 -1, 1024, -8, 0, 8, -1, 1024, -1, 1024, -9, 1);
   AddItem(b, b->name, 0, LIST_TYPE_BORDER);

   /* create a fallback background in case no background is found */
   bg = CreateDesktopBG("NONE", NULL, NULL, 0, 0, 0, 0, 0, 0, NULL, 0, 0, 0, 0,
			0);
   AddItem(bg, bg->name, 0, LIST_TYPE_BACKGROUND);

   /* create a fallback textclass in case no textclass is found */
   tc = CreateTclass();
   tc->name = Estrdup("__FALLBACK_TCLASS");
   tc->norm.normal = CreateTextState();
   tc->norm.normal->fontname =
      Estrdup("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-*-*");
   ESetColor(&(tc->norm.normal->fg_col), 0, 0, 0);
   AddItem(tc, tc->name, 0, LIST_TYPE_TCLASS);

   EDBUG_RETURN_;
}
