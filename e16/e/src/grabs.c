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
	XGrabKey(disp, a->key, mod, VRoot.win, False, GrabModeAsync,
		 GrabModeAsync);
     }
   else
     {
	int                 i;

	/* grab the key even if locks are on or not */
	for (i = 0; i < 8; i++)
	   XGrabKey(disp, a->key, mod | mask_mod_combos[i], VRoot.win, False,
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
	XUngrabKey(disp, a->key, mod, VRoot.win);
     }
   else
     {
	int                 i;

	/* ungrab the key even if locks are on or not */
	for (i = 0; i < 8; i++)
	   XUngrabKey(disp, a->key, mod | mask_mod_combos[i], VRoot.win);
     }
   EDBUG_RETURN_;
}

void
GrabButtonsSet(Window win, unsigned int csr)
{
   if (Mode.grabs.pointer_grab_active)
      return;

   XGrabPointer(disp, win, True, ButtonPressMask | ButtonReleaseMask,
		GrabModeAsync, GrabModeAsync, None, ECsrGet(csr), CurrentTime);

   Mode.grabs.pointer_grab_window = win;
   Mode.grabs.pointer_grab_active = 1;
}

int
GrabPointerSet(Window win, unsigned int csr, int confine)
{
   int                 ret;
   Window              confine_to = (confine) ? win : None;

   if (Mode.grabs.pointer_grab_active)
      return 1;

   ret = XGrabPointer(disp, win, True,
		      ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
		      ButtonMotionMask | EnterWindowMask | LeaveWindowMask,
		      GrabModeAsync, GrabModeAsync, confine_to, ECsrGet(csr),
		      CurrentTime);

   Mode.grabs.pointer_grab_window = win;
   Mode.grabs.pointer_grab_active = 1;

   return ret;
}

void
GrabPointerRelease(void)
{
   if (!Mode.grabs.pointer_grab_active)
      return;

   XUngrabPointer(disp, CurrentTime);

   Mode.grabs.pointer_grab_active = 0;
   Mode.grabs.pointer_grab_window = None;
}

void
GrabButtonSet(unsigned int button, unsigned int modifiers, Window win,
	      unsigned int event_mask, unsigned int csr, int confine)
{
   Bool                owner_events = False;
   int                 pointer_mode = GrabModeSync;
   int                 keyboard_mode = GrabModeAsync;
   Window              confine_to = (confine) ? win : None;

   XGrabButton(disp, button, modifiers, win, owner_events, event_mask,
	       pointer_mode, keyboard_mode, confine_to, ECsrGet(csr));
}

void
GrabButtonRelease(unsigned int button, unsigned int modifiers, Window win)
{
   XUngrabButton(disp, button, modifiers, win);
}
