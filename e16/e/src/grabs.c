
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
	XGrabKey(disp, a->key, mod, root.win, False, GrabModeAsync, GrabModeAsync);
     }
   else
     {
	int                 i;

	/* grab the key even if locks are on or not */
	for (i = 0; i < 8; i++)
	   XGrabKey(disp, a->key, mod | mask_mod_combos[i], root.win, False, GrabModeAsync, GrabModeAsync);
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
   ret = XGrabPointer(disp, win, True, ButtonPressMask | ButtonReleaseMask |
		      PointerMotionMask | ButtonMotionMask |
		      EnterWindowMask | LeaveWindowMask, GrabModeAsync,
		      GrabModeAsync, None, None, CurrentTime);
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
