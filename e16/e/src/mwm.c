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

/* Motif window hints */
#define MWM_HINTS_FUNCTIONS           (1L << 0)
#define MWM_HINTS_DECORATIONS         (1L << 1)
#define MWM_HINTS_INPUT_MODE          (1L << 2)
#define MWM_HINTS_STATUS              (1L << 3)

/* bit definitions for MwmHints.functions */
#define MWM_FUNC_ALL            (1L << 0)
#define MWM_FUNC_RESIZE         (1L << 1)
#define MWM_FUNC_MOVE           (1L << 2)
#define MWM_FUNC_MINIMIZE       (1L << 3)
#define MWM_FUNC_MAXIMIZE       (1L << 4)
#define MWM_FUNC_CLOSE          (1L << 5)

/* bit definitions for MwmHints.decorations */
#define MWM_DECOR_ALL                 (1L << 0)
#define MWM_DECOR_BORDER              (1L << 1)
#define MWM_DECOR_RESIZEH             (1L << 2)
#define MWM_DECOR_TITLE               (1L << 3)
#define MWM_DECOR_MENU                (1L << 4)
#define MWM_DECOR_MINIMIZE            (1L << 5)
#define MWM_DECOR_MAXIMIZE            (1L << 6)

/* bit definitions for MwmHints.inputMode */
#define MWM_INPUT_MODELESS                  0
#define MWM_INPUT_PRIMARY_APPLICATION_MODAL 1
#define MWM_INPUT_SYSTEM_MODAL              2
#define MWM_INPUT_FULL_APPLICATION_MODAL    3

#define PROP_MWM_HINTS_ELEMENTS             5

/* Motif window hints */
typedef struct _mwmhints
{
   CARD32              flags;
   CARD32              functions;
   CARD32              decorations;
   INT32               inputMode;
   CARD32              status;
}
MWMHints;

void
MWM_GetHints(EWin * ewin, Atom atom_change)
{
   int                 fmt;
   Atom                a1, a2;
   unsigned long       num, end;
   MWMHints           *mwmhints;
   unsigned char      *puc;

   EDBUG(6, "MWM_GetHints");
   if (ewin->internal)
      EDBUG_RETURN_;
   if (atom_change)
     {
	static Atom         atom = 0;

	if (!atom)
	   atom = XInternAtom(disp, "_MOTIF_WM_HINTS", False);
	if (atom_change != atom)
	   EDBUG_RETURN_;
     }
   ewin->client.mwm_decor_border = 1;
   ewin->client.mwm_decor_resizeh = 1;
   ewin->client.mwm_decor_title = 1;
   ewin->client.mwm_decor_menu = 1;
   ewin->client.mwm_decor_minimize = 1;
   ewin->client.mwm_decor_maximize = 1;
   ewin->client.mwm_func_resize = 1;
   ewin->client.mwm_func_move = 1;
   ewin->client.mwm_func_minimize = 1;
   ewin->client.mwm_func_maximize = 1;
   ewin->client.mwm_func_close = 1;
   a1 = XInternAtom(disp, "_MOTIF_WM_HINTS", False);
   puc = NULL;
   XGetWindowProperty(disp, ewin->client.win, a1, 0, 20, False, a1, &a2, &fmt,
		      &num, &end, &puc);
   mwmhints = (MWMHints *) puc;
   if (mwmhints)
     {
	if (num < PROP_MWM_HINTS_ELEMENTS)
	  {
	     XFree(mwmhints);
	     EDBUG_RETURN_;
	  }
	if (mwmhints->flags & MWM_HINTS_DECORATIONS)
	  {
	     ewin->client.mwm_decor_border = 0;
	     ewin->client.mwm_decor_resizeh = 0;
	     ewin->client.mwm_decor_title = 0;
	     ewin->client.mwm_decor_menu = 0;
	     ewin->client.mwm_decor_minimize = 0;
	     ewin->client.mwm_decor_maximize = 0;
	     if (mwmhints->decorations & MWM_DECOR_ALL)
	       {
		  ewin->client.mwm_decor_border = 1;
		  ewin->client.mwm_decor_resizeh = 1;
		  ewin->client.mwm_decor_title = 1;
		  ewin->client.mwm_decor_menu = 1;
		  ewin->client.mwm_decor_minimize = 1;
		  ewin->client.mwm_decor_maximize = 1;
	       }
	     if (mwmhints->decorations & MWM_DECOR_BORDER)
		ewin->client.mwm_decor_border = 1;
	     if (mwmhints->decorations & MWM_DECOR_RESIZEH)
		ewin->client.mwm_decor_resizeh = 1;
	     if (mwmhints->decorations & MWM_DECOR_TITLE)
		ewin->client.mwm_decor_title = 1;
	     if (mwmhints->decorations & MWM_DECOR_MENU)
		ewin->client.mwm_decor_menu = 1;
	     if (mwmhints->decorations & MWM_DECOR_MINIMIZE)
		ewin->client.mwm_decor_minimize = 1;
	     if (mwmhints->decorations & MWM_DECOR_MAXIMIZE)
		ewin->client.mwm_decor_maximize = 1;
	  }
	if (mwmhints->flags & MWM_HINTS_FUNCTIONS)
	  {
	     ewin->client.mwm_func_resize = 0;
	     ewin->client.mwm_func_move = 0;
	     ewin->client.mwm_func_minimize = 0;
	     ewin->client.mwm_func_maximize = 0;
	     ewin->client.mwm_func_close = 0;
	     if (mwmhints->functions & MWM_FUNC_ALL)
	       {
		  ewin->client.mwm_func_resize = 1;
		  ewin->client.mwm_func_move = 1;
		  ewin->client.mwm_func_minimize = 1;
		  ewin->client.mwm_func_maximize = 1;
		  ewin->client.mwm_func_close = 1;
	       }
	     if (mwmhints->functions & MWM_FUNC_RESIZE)
		ewin->client.mwm_func_resize = 1;
	     if (mwmhints->functions & MWM_FUNC_MOVE)
		ewin->client.mwm_func_move = 1;
	     if (mwmhints->functions & MWM_FUNC_MINIMIZE)
		ewin->client.mwm_func_minimize = 1;
	     if (mwmhints->functions & MWM_FUNC_MAXIMIZE)
		ewin->client.mwm_func_maximize = 1;
	     if (mwmhints->functions & MWM_FUNC_CLOSE)
		ewin->client.mwm_func_close = 1;
	  }
	XFree(mwmhints);
     }
   EDBUG_RETURN_;
}

void
MWM_SetInfo()
{
   Atom                a1;
   struct _mwminfo
   {
      long                flags;
      Window              win;
   }
   mwminfo;

   EDBUG(6, "MWM_SetInfo");
   a1 = XInternAtom(disp, "_MOTIF_WM_INFO", False);
   mwminfo.flags = 2;
   mwminfo.win = root.win;
   XChangeProperty(disp, root.win, a1, a1, 32, PropModeReplace,
		   (unsigned char *)&mwminfo, 2);
   EDBUG_RETURN_;
}
