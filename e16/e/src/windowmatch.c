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

WindowMatch        *
CreateWindowMatch(char *name)
{
   WindowMatch        *b;

   EDBUG(5, "CreateWindowMatch");
   b = Emalloc(sizeof(WindowMatch));
   if (!b)
      EDBUG_RETURN(NULL);
   b->name = duplicate(name);
   b->win_title = NULL;
   b->win_name = NULL;
   b->win_class = NULL;
   b->width.min = 0;
   b->width.max = 99999;
   b->height.min = 0;
   b->height.max = 99999;
   b->transient = -1;
   b->no_resize_h = -1;
   b->no_resize_v = -1;
   b->shaped = -1;
   b->border = NULL;
   b->icon = NULL;
   b->desk = 0;
   b->make_sticky = 0;
   EDBUG_RETURN(b);
}

char
TestWindowMatch(EWin * ewin, WindowMatch * b)
{

   EDBUG(5, "MatchEwinBorder");

   if ((b->win_title) && (ewin->client.title)
       && (!matchregexp(b->win_title, ewin->client.title)))
      EDBUG_RETURN(0);

   if ((b->win_name) && (ewin->client.name)
       && (!matchregexp(b->win_name, ewin->client.name)))
      EDBUG_RETURN(0);

   if ((b->win_class) && (ewin->client.class)
       && (!matchregexp(b->win_class, ewin->client.class)))
      EDBUG_RETURN(0);

   if ((ewin->client.w > b->width.max) || (ewin->client.w < b->width.min))
      EDBUG_RETURN(0);

   if ((ewin->client.h > b->height.max) || (ewin->client.h < b->height.min))
      EDBUG_RETURN(0);

   if ((b->transient >= 0) && (b->transient != ewin->client.transient))
      EDBUG_RETURN(0);

   if ((b->no_resize_h >= 0) && (b->no_resize_h != ewin->client.no_resize_h))
      EDBUG_RETURN(0);

   if ((b->no_resize_v >= 0) && (b->no_resize_v != ewin->client.no_resize_v))
      EDBUG_RETURN(0);

   if ((b->shaped >= 0) && (b->shaped != ewin->client.shaped))
      EDBUG_RETURN(0);

   EDBUG_RETURN(1);
}

Border             *
MatchEwinBorder(EWin * ewin, WindowMatch * b)
{
   EDBUG(4, "MatchEwinBorder");

   if (TestWindowMatch(ewin, b))
     {
	if (b->make_sticky)
	   ewin->sticky = 1;

	EDBUG_RETURN(b->border);
     }
   else
     {
	EDBUG_RETURN(0);
     }
}

ImageClass         *
MatchEwinIcon(EWin * ewin, WindowMatch * b)
{
   EDBUG(4, "MatchEwinIcon");
   if (TestWindowMatch(ewin, b))
     {
	if (b->make_sticky)
	   ewin->sticky = 1;

	EDBUG_RETURN(b->icon);
     }
   else
     {
	EDBUG_RETURN(0);
     }
}

int
MatchEwinDesktop(EWin * ewin, WindowMatch * b)
{
   EDBUG(4, "MatchEwinDesktop");
   if (TestWindowMatch(ewin, b))
     {
	if (b->make_sticky)
	   ewin->sticky = 1;

	EDBUG_RETURN(b->desk);
     }
   else
     {
	EDBUG_RETURN(0);
     }
}

void               *
MatchEwinByFunction(EWin * ewin,
		    void *(*FunctionToTest) (EWin *, WindowMatch *))
{
   WindowMatch       **lst;
   int                 i, num;
   void               *retval;

   EDBUG(4, "MatchEwinByFunction");
   retval = 0;

   lst = (WindowMatch **) ListItemType(&num, LIST_TYPE_WINDOWMATCH);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     if ((retval = (*FunctionToTest) (ewin, lst[i])))
	       {
		  i = num;
	       }
	  }
	Efree(lst);
     }

   EDBUG_RETURN(retval);

}

void
RemoveWindowMatch(WindowMatch * wm)
{

   EDBUG(4, "RemoveWindowMatch");

   if (!wm)
      EDBUG_RETURN_;

   while (RemoveItemByPtr(wm, LIST_TYPE_WINDOWMATCH));

   if (wm->icon)
      wm->icon->ref_count--;
   if (wm->border)
      wm->border->ref_count--;
   if (wm->name)
      Efree(wm->name);
   if (wm->win_title)
      Efree(wm->win_title);
   if (wm->win_name)
      Efree(wm->win_name);
   if (wm->win_class)
      Efree(wm->win_class);

   EDBUG_RETURN_;

}
