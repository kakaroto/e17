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
   Window              par;
   Window              rt;
   XWindowAttributes   attr;
   static unsigned int num = 0;
   int                 i;

   EDBUG(6, "MapUnmap");
   switch (start)
     {
     case 0:
	ecore_x_grab();
	XQueryTree(disp, VRoot.win, &rt, &par, &wlist, &num);
	if (wlist)
	  {
	     for (i = 0; i < (int)num; i++)
	       {
#ifdef USE_EXT_INIT_WIN
		  if ((init_win_ext) && (init_win_ext == wlist[i]))
		    {
		       wlist[i] = 0;
		    }
		  else
#endif
		    {
		       XGetWindowAttributes(disp, wlist[i], &attr);
		       if (attr.map_state == IsUnmapped)
			 {
			    wlist[i] = 0;
			 }
		       else
			 {
			    EUnmapWindow(disp, wlist[i]);
			 }
		    }
	       }
	  }
	ecore_x_ungrab();
	break;
     case 1:
	if (wlist)
	  {
	     for (i = 0; i < (int)num; i++)
	       {
		  if (wlist[i])
		    {
		       if (XGetWindowAttributes(disp, wlist[i], &attr))
			 {
			    if (attr.override_redirect)
			      {
#ifdef USE_EXT_INIT_WIN
				 if (init_win_ext)
				    XRaiseWindow(disp, init_win_ext);
#endif
				 StartupWindowsRaise();
				 ProgressbarsRaise();
				 EMapWindow(disp, wlist[i]);
			      }
			    else
			      {
				 if (Mode.wm.exiting)
				    EMapWindow(disp, wlist[i]);
				 else
				    AddToFamily(wlist[i]);
			      }
			 }
		    }
	       }
	     XFree(wlist);
	     wlist = NULL;
	  }
	break;
     default:
	break;
     }
   EDBUG_RETURN_;
}

/*
 * This function sets up all of our connections to X
 */
void
SetupX(const char *dstr)
{
   char                buf[128];

   EDBUG(6, "SetupX");

   /* In case we are going to fork, set up the master pid */
   Mode.wm.master = 1;
   Mode.wm.master_pid = getpid();

   /* Open a connection to the diplay nominated by the DISPLAY variable */
   disp = EDisplayOpen(dstr);
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

   VRoot.scr = DefaultScreen(disp);
   Mode.display.screens = ScreenCount(disp);

   Mode.wm.master_screen = VRoot.scr;

   /* Start up on multiple heads, if appropriate */
   if ((Mode.display.screens > 1) && (!Mode.wm.single))
     {
	int                 i;
	char                subdisplay[255];
	char               *dispstr;

	dispstr = DisplayString(disp);

	strcpy(subdisplay, DisplayString(disp));

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
		  VRoot.scr = i;
#ifdef SIGSTOP
		  kill(getpid(), SIGSTOP);
#endif
		  /* Find the point to concatenate the screen onto */
		  dispstr = strchr(subdisplay, ':');
		  if (NULL != dispstr)
		    {
		       dispstr = strchr(dispstr, '.');
		       if (NULL != dispstr)
			  *dispstr = '\0';
		    }
		  Esnprintf(subdisplay + strlen(subdisplay), 10, ".%d", i);
		  dstr = Estrdup(subdisplay);
		  disp = EDisplayOpen(dstr);
		  /* Terminate the loop as I am the child process... */
		  break;
	       }
	  }
     }

   Mode.display.name = Estrdup(DisplayString(disp));
   Esetenv("DISPLAY", Mode.display.name, 1);

   /* set up an error handler for then E would normally have fatal X errors */
   XSetErrorHandler((XErrorHandler) EHandleXError);
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
	XClassHint         *xch;
	XTextProperty       xtp;

	/* Running E in its own virtual root window */
	attr.backing_store = NotUseful;
	attr.override_redirect = False;
	attr.colormap = VRoot.cmap;
	attr.border_pixel = 0;
	attr.background_pixel = 0;
	attr.save_under = True;
	VRoot.win = XCreateWindow(disp, RRoot.win, 0, 0, VRoot.w, VRoot.h, 0,
				  VRoot.depth, InputOutput, VRoot.vis,
				  CWOverrideRedirect | CWSaveUnder |
				  CWBackingStore | CWColormap | CWBackPixel |
				  CWBorderPixel, &attr);
	ERegisterWindow(VRoot.win);

	xtp.encoding = XA_STRING;
	xtp.format = 8;
	xtp.value = (unsigned char *)("Enlightenment");
	xtp.nitems = strlen((char *)(xtp.value));
	XSetWMName(disp, VRoot.win, &xtp);
	xch = XAllocClassHint();
	xch->res_name = (char *)"Virtual-Root";
	xch->res_class = (char *)"Enlightenment";
	XSetClassHint(disp, VRoot.win, xch);
	XFree(xch);

	/* Enable eesh and edox to pix up the virtual root */
	Esnprintf(buf, sizeof(buf), "%#lx", VRoot.win);
	Esetenv("ENL_WM_ROOT", buf, 1);
     }
   else
     {
	/* Running E normally on the root window */
	VRoot.w = RRoot.w;
	VRoot.h = RRoot.h;
     }

   /* initialise imlib */
   imlib_set_cache_size(2048 * 1024);
   imlib_set_font_cache_size(512 * 1024);
   imlib_set_color_usage(128);

   imlib_context_set_dither(1);

   imlib_context_set_display(disp);
   imlib_context_set_visual(VRoot.vis);
   imlib_context_set_colormap(VRoot.cmap);
   imlib_context_set_dither_mask(0);

   /* Initialise event handling */
   EventsInit();

   /* just in case - set them up again */
   /* set up an error handler for then E would normally have fatal X errors */
   XSetErrorHandler((XErrorHandler) EHandleXError);
   /* set up a handler for when the X Connection goes down */
   XSetIOErrorHandler((XIOErrorHandler) HandleXIOError);

   /* select all the root window events to start managing */
   Mode.wm.xselect = 1;
   XSelectInput(disp, VRoot.win,
		ButtonPressMask | ButtonReleaseMask | EnterWindowMask |
		LeaveWindowMask | ButtonMotionMask | PropertyChangeMask |
		SubstructureRedirectMask | KeyPressMask | KeyReleaseMask |
		PointerMotionMask | ResizeRedirectMask |
		SubstructureNotifyMask);
   ecore_x_sync();
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

   ICCCM_Focus(NULL);
   MWM_SetInfo();

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

   EDBUG_RETURN_;
}
