/*
 * Copyright (C) 2004 Kim Woelders
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

#define ENABLE_DEBUG_STACKING 1

struct _ewinlist
{
   const char         *name;
   int                 nalloc;
   int                 nwins;
   EWin              **list;
};

#if ENABLE_DEBUG_STACKING
static void
EwinListShow(const char *txt, EWinList * ewl)
{
   int                 i;
   EWin               *ewin;

   if (!EventDebug(EDBUG_TYPE_STACKING))
      return;

   printf("%s-%s:\n", ewl->name, txt);
   for (i = 0; i < ewl->nwins; i++)
     {
	ewin = ewl->list[i];
	printf(" %2d: %#10lx %#10lx %d %d %s\n", i, ewin->win, ewin->client.win,
	       ewin->desktop, (ewin->floating) ? 999 : ewin->layer,
	       EwinGetTitle(ewin));
     }
}
#else
#define EwinListShow(txt, ewl)
#endif

int
EwinListGetIndex(EWinList * ewl, EWin * ewin)
{
   int                 i;

   for (i = 0; i < ewl->nwins; i++)
      if (ewl->list[i] == ewin)
	 return i;

   return -1;
}

void
EwinListAdd(EWinList * ewl, EWin * ewin)
{
   int                 i;

   /* Quit if already in list */
   i = EwinListGetIndex(ewl, ewin);
   if (i >= 0)
      return;

   if (ewl->nwins >= ewl->nalloc)
     {
	ewl->nalloc += 16;
	ewl->list = (EWin **) Erealloc(ewl->list, ewl->nalloc * sizeof(EWin *));
     }

   /* Add to end */
   ewl->list[ewl->nwins] = ewin;
   ewl->nwins++;

   EwinListShow("EwinListAdd", ewl);
}

void
EwinListDelete(EWinList * ewl, EWin * ewin)
{
   int                 i, n;

   /* Quit if not in list */
   i = EwinListGetIndex(ewl, ewin);
   if (i < 0)
      return;

   ewl->nwins--;
   n = ewl->nwins - i;
   if (n > 0)
     {
	memmove(ewl->list + i, ewl->list + i + 1, n * sizeof(EWin *));
     }
   else if (ewl->nwins <= 0)
     {
	/* Enables autocleanup at shutdown, if ever implemented */
	Efree(ewl->list);
	ewl->list = NULL;
     }

   EwinListShow("EwinListDelete", ewl);
}

int
EwinListLower(EWinList * ewl, EWin * ewin, int mode)
{
   int                 i, j, n;

   /* Quit if not in list */
   i = EwinListGetIndex(ewl, ewin);
   if (i < 0)
      return 0;

   j = ewl->nwins - 1;
   if (mode)
     {
	/* Take the layer into account */
	for (; j > i; j--)
	   if (ewin->layer <= ewl->list[j]->layer)
	      break;
     }

   n = j - i;
   if (n > 0)
     {
	memmove(ewl->list + i, ewl->list + i + 1, n * sizeof(EWin *));
	ewl->list[j] = ewin;
     }

   EwinListShow("EwinListLower", ewl);
   return n;
}

int
EwinListRaise(EWinList * ewl, EWin * ewin, int mode)
{
   int                 i, j, n;

   /* Quit if not in list */
   i = EwinListGetIndex(ewl, ewin);
   if (i < 0)
      return 0;

   j = 0;
   if (mode)
     {
	/* Take the layer into account */
	for (; j < i; j++)
	   if (ewin->layer >= ewl->list[j]->layer)
	      break;
     }

   n = i - j;
   if (n > 0)
     {
	memmove(ewl->list + j + 1, ewl->list + j, n * sizeof(EWin *));
	ewl->list[j] = ewin;
     }

   EwinListShow("EwinListRaise", ewl);
   return n;
}

/*
 * The global stacking and focus lists
 */
EWinList            EwinListFocus = { "Focus", 0, 0, NULL };
EWinList            EwinListStack = { "Stack", 0, 0, NULL };

EWin               *const *
EwinListGet(EWinList * ewl, int *num)
{
   *num = ewl->nwins;
   return ewl->list;
}

EWin               *const *
EwinListGetForDesktop(int desk, int *num)
{
   static EWin       **lst = NULL;
   static int          nalloc;
   int                 i, n, nwins;
   EWin               *ewin;

   /* TBD: Maintain per desktop lists? Not sure it's worth while */
   nwins = EwinListStack.nwins;
   if (nalloc < nwins)
     {
	nalloc += 16;
	lst = Erealloc(lst, nalloc * sizeof(EWin *));
     }

   n = 0;
   for (i = 0; i < nwins; i++)
     {
	ewin = EwinListStack.list[i];
	if (EwinGetDesk(ewin) == desk)
	   lst[n++] = ewin;
     }
   *num = n;

   return lst;
}

int
EwinListStackingRaise(EWin * ewin)
{
   return EwinListRaise(&EwinListStack, ewin, 1);
}

int
EwinListStackingLower(EWin * ewin)
{
   return EwinListLower(&EwinListStack, ewin, 1);
}

int
EwinListFocusRaise(EWin * ewin)
{
   return EwinListRaise(&EwinListFocus, ewin, 0);
}
