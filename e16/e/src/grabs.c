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

static Window       grab_window;

void
GrabActionKey(Action * a)
{
   int                 mod;

   EDBUG(4, "GrabActionKey");
   if (!a->key)
      EDBUG_RETURN_;
   mod = a->modifiers;
   if (a->anymodifier)
     {
	mod = AnyModifier;
	XGrabKey(disp, a->key, mod, root.win, False, GrabModeAsync,
		 GrabModeAsync);
     }
   else
     {
	int                 i;

	/* grab the key even if locks are on or not */
	for (i = 0; i < 8; i++)
	   XGrabKey(disp, a->key, mod | mask_mod_combos[i], root.win, False,
		    GrabModeAsync, GrabModeAsync);
     }
   EDBUG_RETURN_;
}

void
UnGrabActionKey(Action * a)
{
   int                 mod;

   EDBUG(4, "UnGrabActionKey");
   if (!a->key)
      EDBUG_RETURN_;
   mod = a->modifiers;
   if (a->anymodifier)
     {
	mod = AnyModifier;
	XUngrabKey(disp, a->key, mod, root.win);
     }
   else
     {
	int                 i;

	/* ungrab the key even if locks are on or not */
	for (i = 0; i < 8; i++)
	   XUngrabKey(disp, a->key, mod | mask_mod_combos[i], root.win);
     }
   EDBUG_RETURN_;
}

void
GrabTheButtons(Window win)
{
   EDBUG(4, "GrabTheButtons");
   if (mode.click_focus_grabbed)
      EDBUG_RETURN_;
   XGrabPointer(disp, win, True, ButtonPressMask | ButtonReleaseMask,
		GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
   grab_window = win;
   mode.click_focus_grabbed = 1;
   EDBUG_RETURN_;
}

int
GrabThePointer(Window win)
{
   int                 ret;

   EDBUG(4, "GrabThePointer");
   if (mode.click_focus_grabbed)
      EDBUG_RETURN(1);
   ret =
      XGrabPointer(disp, win, True,
		   ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
		   ButtonMotionMask | EnterWindowMask | LeaveWindowMask,
		   GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
   grab_window = win;
   mode.click_focus_grabbed = 1;
   EDBUG_RETURN(ret);
}

int
GrabConfineThePointer(Window win)
{
   int                 ret;

   EDBUG(4, "GrabThePointer");
   if (mode.click_focus_grabbed)
      EDBUG_RETURN(1);
   ret =
      XGrabPointer(disp, win, True,
		   ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
		   ButtonMotionMask | EnterWindowMask | LeaveWindowMask,
		   GrabModeAsync, GrabModeAsync, win, None, CurrentTime);
   grab_window = win;
   mode.click_focus_grabbed = 1;
   EDBUG_RETURN(ret);
}

void
UnGrabTheButtons()
{
   EDBUG(4, "UnGrabTheButtons");
   if (!mode.click_focus_grabbed)
      EDBUG_RETURN_;
   XUngrabPointer(disp, CurrentTime);
   mode.click_focus_grabbed = 0;
   grab_window = 0;
   EDBUG_RETURN_;
}
