/*
 * Copyright (C) 2000-2003 Carsten Haitzler, Geoff Harrison and various contributors
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
   ImlibColor          icl;
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
   ic->name = duplicate("__FALLBACK_ICLASS");
   ic->norm.normal = CreateImageState();
   ic->norm.normal->hihi.r = 255;
   ic->norm.normal->hihi.g = 255;
   ic->norm.normal->hihi.b = 255;
   ic->norm.normal->hi.r = 255;
   ic->norm.normal->hi.g = 255;
   ic->norm.normal->hi.b = 255;
   ic->norm.normal->bg.r = 160;
   ic->norm.normal->bg.g = 160;
   ic->norm.normal->bg.b = 160;
   ic->norm.normal->lo.r = 0;
   ic->norm.normal->lo.g = 0;
   ic->norm.normal->lo.b = 0;
   ic->norm.normal->lolo.r = 0;
   ic->norm.normal->lolo.g = 0;
   ic->norm.normal->lolo.b = 0;
   ic->norm.normal->bevelstyle = BEVEL_AMIGA;

   ic->norm.hilited = CreateImageState();
   ic->norm.hilited->hihi.r = 255;
   ic->norm.hilited->hihi.g = 255;
   ic->norm.hilited->hihi.b = 255;
   ic->norm.hilited->hi.r = 255;
   ic->norm.hilited->hi.g = 255;
   ic->norm.hilited->hi.b = 255;
   ic->norm.hilited->bg.r = 192;
   ic->norm.hilited->bg.g = 192;
   ic->norm.hilited->bg.b = 192;
   ic->norm.hilited->lo.r = 0;
   ic->norm.hilited->lo.g = 0;
   ic->norm.hilited->lo.b = 0;
   ic->norm.hilited->lolo.r = 0;
   ic->norm.hilited->lolo.g = 0;
   ic->norm.hilited->lolo.b = 0;
   ic->norm.hilited->bevelstyle = BEVEL_AMIGA;

   ic->norm.clicked = CreateImageState();
   ic->norm.clicked->hihi.r = 0;
   ic->norm.clicked->hihi.g = 0;
   ic->norm.clicked->hihi.b = 0;
   ic->norm.clicked->hi.r = 0;
   ic->norm.clicked->hi.g = 0;
   ic->norm.clicked->hi.b = 0;
   ic->norm.clicked->bg.r = 192;
   ic->norm.clicked->bg.g = 192;
   ic->norm.clicked->bg.b = 192;
   ic->norm.clicked->lo.r = 255;
   ic->norm.clicked->lo.g = 255;
   ic->norm.clicked->lo.b = 255;
   ic->norm.clicked->lolo.r = 255;
   ic->norm.clicked->lolo.g = 255;
   ic->norm.clicked->lolo.b = 255;
   ic->norm.clicked->bevelstyle = BEVEL_AMIGA;

   ic->active.normal = CreateImageState();
   ic->active.normal->hihi.r = 255;
   ic->active.normal->hihi.g = 255;
   ic->active.normal->hihi.b = 255;
   ic->active.normal->hi.r = 255;
   ic->active.normal->hi.g = 255;
   ic->active.normal->hi.b = 255;
   ic->active.normal->bg.r = 180;
   ic->active.normal->bg.g = 140;
   ic->active.normal->bg.b = 160;
   ic->active.normal->lo.r = 0;
   ic->active.normal->lo.g = 0;
   ic->active.normal->lo.b = 0;
   ic->active.normal->lolo.r = 0;
   ic->active.normal->lolo.g = 0;
   ic->active.normal->lolo.b = 0;
   ic->active.normal->bevelstyle = BEVEL_AMIGA;

   ic->active.hilited = CreateImageState();
   ic->active.hilited->hihi.r = 255;
   ic->active.hilited->hihi.g = 255;
   ic->active.hilited->hihi.b = 255;
   ic->active.hilited->hi.r = 255;
   ic->active.hilited->hi.g = 255;
   ic->active.hilited->hi.b = 255;
   ic->active.hilited->bg.r = 230;
   ic->active.hilited->bg.g = 190;
   ic->active.hilited->bg.b = 210;
   ic->active.hilited->lo.r = 0;
   ic->active.hilited->lo.g = 0;
   ic->active.hilited->lo.b = 0;
   ic->active.hilited->lolo.r = 0;
   ic->active.hilited->lolo.g = 0;
   ic->active.hilited->lolo.b = 0;
   ic->active.hilited->bevelstyle = BEVEL_AMIGA;

   ic->active.clicked = CreateImageState();
   ic->active.clicked->hihi.r = 0;
   ic->active.clicked->hihi.g = 0;
   ic->active.clicked->hihi.b = 0;
   ic->active.clicked->hi.r = 0;
   ic->active.clicked->hi.g = 0;
   ic->active.clicked->hi.b = 0;
   ic->active.clicked->bg.r = 230;
   ic->active.clicked->bg.g = 190;
   ic->active.clicked->bg.b = 210;
   ic->active.clicked->lo.r = 255;
   ic->active.clicked->lo.g = 255;
   ic->active.clicked->lo.b = 255;
   ic->active.clicked->lolo.r = 255;
   ic->active.clicked->lolo.g = 255;
   ic->active.clicked->lolo.b = 255;
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
   bg = CreateDesktopBG("NONE", &icl, NULL, 0, 0, 0, 0, 0, 0, NULL, 0, 0, 0, 0,
			0);
   AddItem(bg, bg->name, 0, LIST_TYPE_BACKGROUND);

   /* create a fallback textclass in case no textclass is found */
   tc = CreateTclass();
   tc->name = "__FALLBACK_TCLASS";
   tc->norm.normal = CreateTextState();
   tc->norm.normal->fontname =
      duplicate("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-*-*");
   tc->norm.normal->fg_col.r = 0;
   tc->norm.normal->fg_col.g = 0;
   tc->norm.normal->fg_col.b = 0;
   AddItem(tc, tc->name, 0, LIST_TYPE_TCLASS);

   EDBUG_RETURN_;
}

void
SetupInit(void)
{
   EDBUG(5, "SetupInit");

   EDBUG_RETURN_;
}
