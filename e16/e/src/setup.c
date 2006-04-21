/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2006 Kim Woelders
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
#include "eimage.h"
#include "ewins.h"
#include "xwin.h"
#include <signal.h>
#include <X11/keysym.h>
#include <signal.h>

void
MapUnmap(int start)
{
   /* this function will map and unmap all the windows based on the progress
    * through the startup sequence
    */

   static Window      *wlist = NULL;
   static unsigned int num = 0;
   Window              par, rt;
   XWindowAttributes   attr;
   unsigned int        i;

#ifdef USE_EXT_INIT_WIN
   Window              init_win = ExtInitWinGet();
#endif

   switch (start)
     {
     case 0:
	EGrabServer();
	XQueryTree(disp, VRoot.win, &rt, &par, &wlist, &num);
	for (i = 0; i < num; i++)
	  {
#ifdef USE_EXT_INIT_WIN
	     if (init_win == wlist[i])
	       {
		  wlist[i] = None;
		  continue;
	       }
#endif
	     XGetWindowAttributes(disp, wlist[i], &attr);
	     if (attr.map_state == IsUnmapped)
		wlist[i] = None;
	     else
		XUnmapWindow(disp, wlist[i]);
	  }
	/* Flush (get rid of unmap events) */
	XSync(disp, True);
	EUngrabServer();
	break;

     case 1:
	if (!wlist)
	   break;

	for (i = 0; i < num; i++)
	  {
	     if (!wlist[i])
		continue;

	     if (!XGetWindowAttributes(disp, wlist[i], &attr))
		continue;

	     if (attr.override_redirect)
	       {
#ifdef USE_EXT_INIT_WIN
		  if (init_win)
		     XRaiseWindow(disp, init_win);
#endif
		  XMapWindow(disp, wlist[i]);
	       }
	     else
	       {
		  if (Mode.wm.exiting)
		     XMapWindow(disp, wlist[i]);
		  else
		     AddToFamily(NULL, wlist[i]);
	       }
	  }
	XFree(wlist);
	wlist = NULL;
	break;

     default:
	break;
     }
}

/*
 * This function sets up all of our connections to X
 */
void
SetupX(const char *dstr)
{
   char                buf[128];
   long                mask;

   if (!dstr)
      dstr = getenv("DISPLAY");
   if (!dstr)
      dstr = ":0";

   /* Open a connection to the diplay nominated by the DISPLAY variable */
   disp = EDisplayOpen(dstr, VRoot.scr);
   if (!disp)
     {
	Alert(_("Enlightenment cannot connect to the display nominated by\n"
		"your shell's DISPLAY environment variable. You may set this\n"
		"variable to indicate which display name Enlightenment is to\n"
		"connect to. It may be that you do not have an Xserver already\n"
		"running to serve that Display connection, or that you do not\n"
		"have permission to connect to that display. Please make sure\n"
		"all is correct before trying again. Run an Xserver by running\n"
		"xdm or startx first, or contact your local system\n"
		"administrator, or Xserver vendor, or read the X, xdm and\n"
		"startx manual pages before proceeding.\n"));
	EExit(1);
     }

   if (getenv("ESYNCHRONIZE"))
      XSynchronize(disp, True);

   VRoot.scr = DefaultScreen(disp);
   Mode.display.screens = ScreenCount(disp);

   if (Mode.wm.master ||
       Mode.wm.master_screen < 0 ||
       Mode.wm.master_screen >= Mode.display.screens)
      Mode.wm.master_screen = VRoot.scr;

   /* Start up on multiple heads, if appropriate */
   if (Mode.display.screens > 1 && !Mode.wm.single && !Mode.wm.restart)
     {
	int                 i;

	for (i = 0; i < Mode.display.screens; i++)
	  {
	     pid_t               pid;

	     if (i == VRoot.scr)
		continue;

	     pid = fork();
	     if (pid)
	       {
		  /* We are the master */
		  Mode.wm.child_count++;
		  Mode.wm.children =
		     Erealloc(Mode.wm.children,
			      sizeof(pid_t) * Mode.wm.child_count);
		  Mode.wm.children[Mode.wm.child_count - 1] = pid;
	       }
	     else
	       {
		  /* We are a slave */
		  EDisplayDisconnect();
		  Mode.wm.master = 0;
		  Mode.wm.pid = getpid();
		  VRoot.scr = i;
		  ExtInitWinSet(None);
#ifdef SIGSTOP
		  kill(getpid(), SIGSTOP);
#endif
		  disp = EDisplayOpen(dstr, i);
		  /* Terminate the loop as I am the child process... */
		  break;
	       }
	  }
     }

   Mode.display.name = Estrdup(DisplayString(disp));
   Esetenv("DISPLAY", Mode.display.name);

   /* set up an error handler for then E would normally have fatal X errors */
   XSetErrorHandler((XErrorHandler) HandleXError);
   /* set up a handler for when the X Connection goes down */
   XSetIOErrorHandler((XIOErrorHandler) HandleXIOError);

   /* Root defaults */
   RRoot.scr = DefaultScreen(disp);
   RRoot.win = DefaultRootWindow(disp);
   RRoot.vis = DefaultVisual(disp, RRoot.scr);
   RRoot.depth = DefaultDepth(disp, RRoot.scr);
   RRoot.cmap = DefaultColormap(disp, RRoot.scr);
   RRoot.w = DisplayWidth(disp, RRoot.scr);
   RRoot.h = DisplayHeight(disp, RRoot.scr);

   VRoot.win = RRoot.win;
   VRoot.vis = RRoot.vis;
   VRoot.depth = RRoot.depth;
   VRoot.cmap = RRoot.cmap;

   ERegisterWindow(RRoot.win);

   if (Mode.wm.window)
     {
	XSetWindowAttributes attr;

	/* Running E in its own virtual root window */
	attr.backing_store = NotUseful;
	attr.override_redirect = False;
	attr.colormap = VRoot.cmap;
	attr.border_pixel = 0;
	attr.background_pixel = 0;
	attr.save_under = True;
	VRoot.win = XCreateWindow(disp, RRoot.win, 0, 0, VRoot.w, VRoot.h, 0,
				  CopyFromParent, InputOutput, CopyFromParent,
				  CWOverrideRedirect | CWSaveUnder |
				  CWBackingStore | CWColormap | CWBackPixel |
				  CWBorderPixel, &attr);
	ERegisterWindow(VRoot.win);

	/* Enable eesh and edox to pix up the virtual root */
	Esnprintf(buf, sizeof(buf), "%#lx", VRoot.win);
	Esetenv("ENL_WM_ROOT", buf);
     }
   else
     {
	/* Running E normally on the root window */
	VRoot.w = RRoot.w;
	VRoot.h = RRoot.h;
     }

   /* Initialise image handling */
   EImageInit(disp);

   /* Initialise event handling */
   EventsInit();

   /* just in case - set them up again */
   /* set up an error handler for then E would normally have fatal X errors */
   XSetErrorHandler((XErrorHandler) HandleXError);
   /* set up a handler for when the X Connection goes down */
   XSetIOErrorHandler((XIOErrorHandler) HandleXIOError);

   /* select all the root window events to start managing */
   Mode.wm.xselect = 1;
   mask =
      ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask |
      ButtonMotionMask | PropertyChangeMask | SubstructureRedirectMask |
      PointerMotionMask | StructureNotifyMask | SubstructureNotifyMask;
   XSelectInput(disp, VRoot.win, mask);

   ESync();
   Mode.wm.xselect = 0;

   /* warn, if necessary about X version problems */
   if (ProtocolVersion(disp) != 11)
     {
	AlertX(_("X server version error"), _("Ignore this error"), "",
	       _("Quit Enlightenment"),
	       _("WARNING:\n"
		 "This is not an X11 Xserver. It in fact talks the X%i protocol.\n"
		 "This may mean Enlightenment will either not function, or\n"
		 "function incorrectly. If it is later than X11, then your\n"
		 "server is one the author of Enlightenment neither have\n"
		 "access to, nor have heard of.\n"), ProtocolVersion(disp));
     }

   /* damn that bloody numlock stuff - ok I'd rather XFree got fixed to not */
   /* have it as a modifier and everyone have to write specific code to mask */
   /* it out - but well.... */
   /* ok under Xfree Numlock and Scollock are lock modifiers and we need */
   /* to hunt them down to mask them out - EVIL EVIL EVIL hack but needed */
   {
      XModifierKeymap    *mod;
      KeyCode             nl, sl;
      int                 i;
      int                 masks[8] = {
	 ShiftMask, LockMask, ControlMask, Mod1Mask, Mod2Mask, Mod3Mask,
	 Mod4Mask, Mod5Mask
      };

      mod = XGetModifierMapping(disp);
      nl = XKeysymToKeycode(disp, XK_Num_Lock);
      sl = XKeysymToKeycode(disp, XK_Scroll_Lock);
      if ((mod) && (mod->max_keypermod > 0))
	{
	   for (i = 0; i < (8 * mod->max_keypermod); i++)
	     {
		if ((nl) && (mod->modifiermap[i] == nl))
		   Mode.masks.numlock = masks[i / mod->max_keypermod];
		else if ((sl) && (mod->modifiermap[i] == sl))
		   Mode.masks.scrollock = masks[i / mod->max_keypermod];
	     }
	}
      Mode.masks.mod_combos[0] = 0;
      Mode.masks.mod_combos[1] = LockMask;
      Mode.masks.mod_combos[2] = Mode.masks.numlock;
      Mode.masks.mod_combos[3] = Mode.masks.scrollock;
      Mode.masks.mod_combos[4] = Mode.masks.numlock | Mode.masks.scrollock;
      Mode.masks.mod_combos[5] = LockMask | Mode.masks.numlock;
      Mode.masks.mod_combos[6] = LockMask | Mode.masks.scrollock;
      Mode.masks.mod_combos[7] = LockMask |
	 Mode.masks.numlock | Mode.masks.scrollock;

      if (mod)
	 XFreeModifiermap(mod);
   }

   ScreenInit();
   ZoomInit();
}
