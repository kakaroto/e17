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
#include "conf.h"

static WindowMatch *
WindowMatchCreate(const char *name)
{
   WindowMatch        *b;

   b = Emalloc(sizeof(WindowMatch));
   if (!b)
      return NULL;

   b->name = Estrdup(name);
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

   return b;
}

#if 0
static void
WindowMatchDestroy(WindowMatch * wm)
{
   if (!wm)
      return;

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
}
#endif

int
WindowMatchConfigLoad(FILE * fs)
{
   int                 err = 0;
   WindowMatch        *bm = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   int                 fields;

   while (GetLine(s, sizeof(s), fs))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	   i1 = CONFIG_INVALID;
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     AddItem(bm, bm->name, 0, LIST_TYPE_WINDOWMATCH);
	     goto done;
	  case CONFIG_CLASSNAME:
	     bm = WindowMatchCreate(s2);
	     break;
	  case CONFIG_BORDER:
	  case WINDOWMATCH_USEBORDER:
	     bm->border = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);
	     if (bm->border)
		bm->border->ref_count++;
	     break;
	  case WINDOWMATCH_MATCHNAME:
	     bm->win_name = Estrdup(atword(s, 2));
	     break;
	  case WINDOWMATCH_MATCHCLASS:
	     bm->win_class = Estrdup(atword(s, 2));
	     break;
	  case WINDOWMATCH_MATCHTITLE:
	     bm->win_title = Estrdup(atword(s, 2));
	     break;
	  case WINDOWMATCH_DESKTOP:
	  case CONFIG_DESKTOP:
	     bm->desk = atoi(s2);
	     break;
	  case WINDOWMATCH_ICON:
	  case CONFIG_ICONBOX:
	     bm->icon = ImageclassFind(s2, 0);
	     if (bm->icon)
		bm->icon->ref_count++;
	     break;
	  case WINDOWMATCH_WIDTH:
	     {
		char                s3[FILEPATH_LEN_MAX];

		sscanf(s, "%*s %4000s %4000s", s2, s3);
		bm->width.min = atoi(s2);
		bm->width.max = atoi(s3);
	     }
	     break;
	  case WINDOWMATCH_HEIGHT:
	     {
		char                s3[FILEPATH_LEN_MAX];

		sscanf(s, "%*s %4000s %4000s", s2, s3);
		bm->height.min = atoi(s2);
		bm->height.max = atoi(s3);
	     }
	     break;
	  case WINDOWMATCH_TRANSIENT:
	     bm->transient = atoi(s2);
	     break;
	  case WINDOWMATCH_NO_RESIZE_H:
	     bm->no_resize_h = atoi(s2);
	     break;
	  case WINDOWMATCH_NO_RESIZE_V:
	     bm->no_resize_v = atoi(s2);
	     break;
	  case WINDOWMATCH_SHAPED:
	     bm->shaped = atoi(s2);
	     break;
	  case WINDOWMATCH_MAKESTICKY:
	     bm->make_sticky = atoi(s2);
	     break;
	  default:
	     Alert(_("Warning: unable to determine what to do with\n"
		     "the following text in the middle of current "
		     "WindowMatch definition:\n"
		     "%s\nWill ignore and continue...\n"), s);
	     break;
	  }
     }
   err = -1;

 done:
   return err;
}

static int
WindowMatchTest(EWin * ewin, WindowMatch * b)
{

   if ((b->win_title) && (ewin->icccm.wm_name)
       && (!matchregexp(b->win_title, ewin->icccm.wm_name)))
      return 0;

   if ((b->win_name) && (ewin->icccm.wm_res_name)
       && (!matchregexp(b->win_name, ewin->icccm.wm_res_name)))
      return 0;

   if ((b->win_class) && (ewin->icccm.wm_res_class)
       && (!matchregexp(b->win_class, ewin->icccm.wm_res_class)))
      return 0;

   if ((ewin->client.w > b->width.max) || (ewin->client.w < b->width.min))
      return 0;

   if ((ewin->client.h > b->height.max) || (ewin->client.h < b->height.min))
      return 0;

   if ((b->transient >= 0) && (b->transient != ewin->client.transient))
      return 0;

   if ((b->no_resize_h >= 0) && (b->no_resize_h != ewin->client.no_resize_h))
      return 0;

   if ((b->no_resize_v >= 0) && (b->no_resize_v != ewin->client.no_resize_v))
      return 0;

   if ((b->shaped >= 0) && (b->shaped != ewin->client.shaped))
      return 0;

   return 1;
}

Border             *
MatchEwinBorder(EWin * ewin, WindowMatch * b)
{
   if (WindowMatchTest(ewin, b))
     {
	if (b->make_sticky)
	   EoSetSticky(ewin, 1);

	return b->border;
     }
   else
     {
	return 0;
     }
}

ImageClass         *
MatchEwinIcon(EWin * ewin, WindowMatch * b)
{
   if (WindowMatchTest(ewin, b))
     {
	if (b->make_sticky)
	   EoSetSticky(ewin, 1);

	return b->icon;
     }
   else
     {
	return 0;
     }
}

int
MatchEwinDesktop(EWin * ewin, WindowMatch * b)
{
   if (WindowMatchTest(ewin, b))
     {
	if (b->make_sticky)
	   EoSetSticky(ewin, 1);

	return b->desk;
     }
   else
     {
	return 0;
     }
}

void               *
MatchEwinByFunction(EWin * ewin,
		    void *(*FunctionToTest) (EWin *, WindowMatch *))
{
   WindowMatch       **lst;
   int                 i, num;
   void               *retval;

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

   return retval;
}
