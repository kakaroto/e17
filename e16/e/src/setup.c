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
#include <X11/keysym.h>

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
	XQueryTree(disp, root.win, &rt, &par, &wlist, &num);
	if (wlist)
	  {
	     for (i = 0; i < (int)num; i++)
	       {
		  if ((init_win_ext) && (init_win_ext == wlist[i]))
		    {
		       wlist[i] = 0;
		    }
		  else
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
				 if (init_win1)
				   {
				      XRaiseWindow(disp, init_win1);
				      XRaiseWindow(disp, init_win2);
				   }
				 if (init_win_ext)
				    XRaiseWindow(disp, init_win_ext);
				 ShowEdgeWindows();
				 RaiseProgressbars();
				 EMapWindow(disp, wlist[i]);
			      }
			    else
			      {
				 AddToFamily(wlist[i]);
			      }
			 }
		    }
	       }
	     XFree(wlist);
	  }
	break;
     default:
	break;
     }
   EDBUG_RETURN_;
}

void
SetupX()
{
   /* This function sets up all of our connections to X */

   EDBUG(6, "SetupX");

   /* In case we are going to fork, set up the master pid */
   master_pid = getpid();

   /* Open a connection to the diplay nominated by the DISPLAY variable */
   if (!dstr)
      dstr = getenv("DISPLAY");
   if (!dstr)
      dstr = ":0";
   disp = XOpenDisplay(dstr);
   /* if cannot connect to display */
   if (!disp)
     {
	Alert(_
	      ("Enlightenment cannot connect to the display nominated by\n"
	       "your shell's DISPLAY environment variable. You may set this\n"
	       "variable to indicate which display name Enlightenment is to\n"
	       "connect to. It may be that you do not have an Xserver already\n"
	       "running to serve that Display connection, or that you do not\n"
	       "have permission to connect to that display. Please make sure\n"
	       "all is correct before trying again. Run an Xserver by running\n"
	       "xdm or startx first, or contact your local system\n"
	       "administrator, or Xserver vendor, or read the X, xdm and\n"
	       "startx manual pages before proceeding.\n"));
	EExit((void *)1);
     }
   root.scr = DefaultScreen(disp);
   display_screens = ScreenCount(disp);
   master_screen = root.scr;
   /* Start up on multiple heads, if appropriate */
   if ((display_screens > 1) && (!single_screen_mode))
     {
	int                 i;
	char                subdisplay[255];
	char               *dispstr;

	dispstr = DisplayString(disp);

	strcpy(subdisplay, DisplayString(disp));

	for (i = 0; i < display_screens; i++)
	  {
	     pid_t               pid;

	     if (i != root.scr)
	       {
		  pid = fork();
		  if (pid)
		    {
		       child_count++;
		       e_children =
			  Erealloc(e_children, sizeof(pid_t) * child_count);
		       e_children[child_count - 1] = pid;
		    }
		  else
		    {
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
		       disp = XOpenDisplay(dstr);
		       root.scr = i;
		       /* Terminate the loop as I am the child process... */
		       break;
		    }
	       }
	  }
     }

   /* set up an error handler for then E would normally have fatal X errors */
   XSetErrorHandler((XErrorHandler) EHandleXError);
   /* set up a handler for when the X Connection goes down */
   XSetIOErrorHandler((XIOErrorHandler) HandleXIOError);

   /* initialise event handling */
   EventsInit();

   /* initialise imlib */
#if USE_IMLIB2
   root.win = DefaultRootWindow(disp);
   root.vis = DefaultVisual(disp, root.scr);
   root.depth = DefaultDepth(disp, root.scr);
   root.cmap = DefaultColormap(disp, root.scr);

   imlib_set_cache_size(2048 * 1024);
   imlib_set_font_cache_size(512 * 1024);
   imlib_set_color_usage(128);

   imlib_context_set_dither(1);

   imlib_context_set_display(disp);
   imlib_context_set_visual(root.vis);
   imlib_context_set_colormap(root.cmap);
   imlib_context_set_dither_mask(0);
#else
   pImlib_Context = Imlib_init(disp);
   IMLIB1_SET_CONTEXT(0);
   if (!pImlib_Context)
     {
	ASSIGN_ALERT(_("Imlib initialisation error"), "", "",
		     _("Quit Enlightenment"));
	Alert(_
	      ("FATAL ERROR:\n" "\n"
	       "Enlightenment is unable to initialise Imlib.\n" "\n"
	       "This is unusual. Unable to continue.\n" "Exiting.\n"));
	RESET_ALERT;
	EExit((void *)1);
     }
#if USE_FNLIB
   pFnlibData = Fnlib_init(pImlib_Context);
   if (!pFnlibData)
     {
	ASSIGN_ALERT(_("X server setup error"), "", "",
		     _("Quit Enlightenment"));
	Alert(_
	      ("FATAL ERROR:\n" "\n"
	       "Enlightenment is unable to initialise Fnlib.\n" "\n"
	       "This is unusual. Unable to continue.\n" "Exiting.\n"));
	RESET_ALERT;
	EExit((void *)1);
     }
#endif
   root.win = pImlib_Context->x.root;
   root.vis = Imlib_get_visual(pImlib_Context);
   root.depth = pImlib_Context->x.depth;
   root.cmap = Imlib_get_colormap(pImlib_Context);
   /* warn, if necessary about visual problems */
   if (DefaultVisual(disp, root.scr) != root.vis)
     {
	ImlibInitParams     p;

	p.flags = PARAMS_VISUALID;
	p.visualid = XVisualIDFromVisual(DefaultVisual(disp, root.scr));
	prImlib_Context = Imlib_init_with_params(disp, &p);
     }
   else
     {
	prImlib_Context = NULL;
     }
#endif
   root.w = DisplayWidth(disp, root.scr);
   root.h = DisplayHeight(disp, root.scr);

   /* just in case - set them up again */
   /* set up an error handler for then E would normally have fatal X errors */
   XSetErrorHandler((XErrorHandler) EHandleXError);
   /* set up a handler for when the X Connection goes down */
   XSetIOErrorHandler((XIOErrorHandler) HandleXIOError);

   /* select all the root window events to start managing */
   mode.xselect = 1;
   XSelectInput(disp, root.win,
		ButtonPressMask | ButtonReleaseMask | EnterWindowMask |
		LeaveWindowMask | ButtonMotionMask | PropertyChangeMask |
		SubstructureRedirectMask | KeyPressMask | KeyReleaseMask |
		PointerMotionMask | ResizeRedirectMask |
		SubstructureNotifyMask);
   XSync(disp, False);
   mode.xselect = 0;

   /* warn, if necessary about X version problems */
   if (ProtocolVersion(disp) != 11)
     {
	ASSIGN_ALERT(_("X server version error"), _("Ignore this error"), "",
		     _("Quit Enlightenment"));
	Alert(_
	      ("WARNING:\n"
	       "This is not an X11 Xserver. It in fact talks the X%i protocol.\n"
	       "This may mean Enlightenment will either not function, or\n"
	       "function incorrectly. If it is later than X11, then your\n"
	       "server is one the author of Enlightenment neither have\n"
	       "access to, nor have heard of.\n"), ProtocolVersion(disp));
	RESET_ALERT;
     }
   /* now we'll set the locale */
   setlocale(LC_ALL, "");
   if (!XSupportsLocale())
      setlocale(LC_ALL, "C");
   XSetLocaleModifiers("");
   setlocale(LC_ALL, NULL);

   /* I dont want any internationalisation of my numeric input & output */
   setlocale(LC_NUMERIC, "C");

   ICCCM_SetIconSizes();
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
		   numlock_mask = masks[i / mod->max_keypermod];
		else if ((sl) && (mod->modifiermap[i] == sl))
		   scrollock_mask = masks[i / mod->max_keypermod];
	     }
	}
      mask_mod_combos[0] = 0;
      mask_mod_combos[1] = LockMask;
      mask_mod_combos[2] = numlock_mask;
      mask_mod_combos[3] = scrollock_mask;
      mask_mod_combos[4] = numlock_mask | scrollock_mask;
      mask_mod_combos[5] = LockMask | numlock_mask;
      mask_mod_combos[6] = LockMask | scrollock_mask;
      mask_mod_combos[7] = LockMask | numlock_mask | scrollock_mask;

      if (mod)
	 XFreeModifiermap(mod);
   }

   /* Now we're going to set a bunch of default settings in E - in case we
    * don't ever get to load a config file for some odd reason. */
   memset(&conf, 0, sizeof(EConf));
   conf.areas.nx = 2;
   conf.areas.ny = 1;
   conf.areas.wraparound = 0;
   conf.autoraise.enable = 0;
   conf.autoraise.delay = 0.5;
   conf.backgrounds.hiquality = 1;
   conf.backgrounds.user = 1;
   conf.backgrounds.timeout = 240;
   conf.desks.num = 2;
   conf.desks.wraparound = 0;
   conf.desks.dragdir = 2;
   conf.desks.dragbar_width = 16;
   conf.desks.dragbar_ordering = 1;
   conf.desks.dragbar_length = 0;
   conf.desks.slidein = 1;
   conf.desks.slidespeed = 6000;
   conf.dialogs.headers = 0;
   conf.dock.dirmode = DOCK_DOWN;
   conf.dock.startx = 0;
   conf.dock.starty = 0;
   conf.focus.mode = MODE_FOCUS_SLOPPY;
   conf.focus.clickraises = 0;
   conf.focus.transientsfollowleader = 1;
   conf.focus.switchfortransientmap = 1;
   conf.focus.all_new_windows_get_focus = 0;
   conf.focus.new_transients_get_focus = 0;
   conf.focus.new_transients_get_focus_if_group_focused = 1;
   conf.focus.raise_on_next_focus = 1;
   conf.focus.raise_after_next_focus = 1;
   conf.focus.warp_on_next_focus = 0;
   conf.focus.warp_after_next_focus = 0;
   conf.group_config.iconify = 1;
   conf.group_config.kill = 0;
   conf.group_config.move = 1;
   conf.group_config.raise = 0;
   conf.group_config.set_border = 1;
   conf.group_config.stick = 1;
   conf.group_config.shade = 1;
   conf.group_config.mirror = 1;
   conf.pagers.enable = 1;
   conf.pagers.zoom = 1;
   conf.pagers.title = 1;
   conf.pagers.hiq = 1;
   conf.pagers.snap = 1;
   conf.pagers.scanspeed = 10;
   conf.group_swapmove = 1;
   conf.pagers.sel_button = 2;
   conf.pagers.win_button = 1;
   conf.pagers.menu_button = 3;
   conf.snap.enable = 1;
   conf.snap.edge_snap_dist = 8;
   conf.snap.screen_snap_dist = 32;
   conf.tooltips.enable = 1;
   conf.tooltips.delay = 0.5;
   conf.tooltips.showroottooltip = 1;
#ifdef WITH_TARTY_WARP
   conf.warplist.enable = 1;
#else
   conf.warplist.enable = 0;
#endif /* WITH_TARTY_WARP */
   conf.warplist.warpsticky = 1;
   conf.warplist.warpshaded = 1;
   conf.warplist.warpiconified = 0;
   conf.warplist.warpfocused = 1;

   conf.deskmode = MODE_NONE;
   conf.movemode = 0;
   conf.dockapp_support = 1;
   conf.primaryicondir = ICON_RIGHT;
   conf.resizemode = 1;
   conf.geominfomode = 1;
   conf.slidemode = 0;
   conf.cleanupslide = 1;
   conf.mapslide = 1;
   conf.slidespeedmap = 6000;
   conf.slidespeedcleanup = 8000;
   conf.shadespeed = 8000;
   conf.animate_shading = 1;
   conf.sound = 1;
   conf.button_move_resistance = 5;
   conf.autosave = 1;
   conf.memory_paranoia = 1;
   conf.save_under = 0;
   conf.menuslide = 0;
   conf.menusonscreen = 1;
   conf.warpmenus = 1;
   conf.manual_placement = 0;
   conf.edge_flip_resistance = 15;

   ScreenInit();

   MenusInit();

   EDBUG_RETURN_;
}

static void         ChkDir(char *d);

static void
ChkDir(char *d)
{
   if (!isdir(d))
     {
	Alert(_
	      ("The directory %s is apparently not a directory\n"
	       "This is a fatal condition.\n" "Please remove this file\n"), d);
	EExit((void *)1);
     }
   if (!canexec(d))
     {
	Alert(_
	      ("Do not have execute access to %s\n"
	       "This is a fatal condition.\n"
	       "Please check the ownership and permissions of this\n"
	       "directory and take steps to rectify this.\n"), d);
	EExit((void *)1);
     }
   if (!canread(d))
     {
	Alert(_
	      ("Do not have read access to %s\n" "This is a fatal condition.\n"
	       "Please check the ownership and permissions of this\n"
	       "directory and take steps to rectify this.\n"), d);
	EExit((void *)1);
     }
   if (!canwrite(d))
     {
	Alert(_
	      ("Do not have write access to %s\n" "This is a fatal condition.\n"
	       "Please check the ownership and permissions of this\n"
	       "directory and take steps to rectify this.\n"), d);
	EExit((void *)1);
     }
}

void
SetupDirs()
{
   char                s[1024], ss[1024], *home;

   EDBUG(6, "SetupDirs");
   Esnprintf(s, sizeof(s), "%s", EDirUser());
   home = homedir(getuid());
   if (home)
     {
	ChkDir(home);
	Efree(home);
     }
   if (exists(s))
     {
	if (!isdir(s))
	  {
	     Esnprintf(ss, sizeof(ss), "%s.old", EDirUser());
	     mv(s, ss);
	     md(s);
	  }
	else
	   ChkDir(EDirUser());
     }
   else
      md(s);
   Esnprintf(s, sizeof(s), "%s/themes", EDirUser());
   if (!exists(s))
      md(s);
   else
      ChkDir(s);
   Esnprintf(s, sizeof(s), "%s/backgrounds", EDirUser());
   if (!exists(s))
      md(s);
   else
      ChkDir(s);
   Esnprintf(s, sizeof(s), "%s/cached", EDirUserCache());
   if (!exists(s))
      md(s);
   else
      ChkDir(s);
   Esnprintf(s, sizeof(s), "%s/cached/img", EDirUserCache());
   if (!exists(s))
      md(s);
   else
      ChkDir(s);
   Esnprintf(s, sizeof(s), "%s/cached/cfg", EDirUserCache());
   if (!exists(s))
      md(s);
   else
      ChkDir(s);
   Esnprintf(s, sizeof(s), "%s/cached/bgsel", EDirUserCache());
   if (!exists(s))
      md(s);
   else
      ChkDir(s);
   Esnprintf(s, sizeof(s), "%s/cached/pager", EDirUserCache());
   if (!exists(s))
      md(s);
   else
      ChkDir(s);
   EDBUG_RETURN_;
}

void
SetupEnv()
{
   char                s[1024];

   if (master_pid != getpid())
      Esetenv("DISPLAY", DisplayString(disp), 1);
   Esetenv("EVERSION", ENLIGHTENMENT_VERSION, 1);
   Esetenv("EROOT", EDirRoot(), 1);
   Esetenv("EBIN", EDirBin(), 1);
   Esnprintf(s, sizeof(s), "%i", getpid());
   Esetenv("EPID", s, 1);
   Esetenv("ETHEME", themepath, 1);
   Esetenv("ECONFDIR", EDirUser(), 1);
   Esetenv("ECACHEDIR", EDirUserCache(), 1);
}

Window
MakeExtInitWin(void)
{
   Display            *d2;
   Window              win;
   XGCValues           gcv;
   GC                  gc;
   Pixmap              pmap;
   Atom                a;
   CARD32              val;
   int                 i;
   Window             *retval;
   XSetWindowAttributes attr;

   a = XInternAtom(disp, "ENLIGHTENMENT_RESTART_SCREEN", False);
   XSync(disp, False);
   if (fork())
     {
	UngrabX();
	for (;;)
	  {
	     Atom                aa;
	     int                 format_ret;
	     unsigned long       bytes_after, num_ret;
	     unsigned char      *puc;

	     puc = NULL;
	     XGetWindowProperty(disp, root.win, a, 0, 0x7fffffff, True,
				XA_CARDINAL, &aa, &format_ret, &num_ret,
				&bytes_after, &puc);
	     retval = (Window *) puc;
	     XSync(disp, False);
	     if (retval)
		break;
	  }
	win = *retval;
	fflush(stdout);
	XFree(retval);

	return win;
     }

   /* on solairs connection stays up - close */
   XSetErrorHandler((XErrorHandler) NULL);
   XSetIOErrorHandler((XIOErrorHandler) NULL);

   SignalsRestore();

   d2 = XOpenDisplay(DisplayString(disp));
   close(ConnectionNumber(disp));
   XGrabServer(d2);
#if USE_IMLIB2
   imlib_set_cache_size(2048 * 1024);
   imlib_set_font_cache_size(512 * 1024);
   imlib_set_color_usage(128);

   imlib_context_set_display(d2);
   imlib_context_set_visual(DefaultVisual(d2, DefaultScreen(d2)));
   imlib_context_set_colormap(DefaultColormap(d2, DefaultScreen(d2)));
#else
   pImlib_Context = Imlib_init(d2);
#endif
   IMLIB1_SET_CONTEXT(0);
   attr.backing_store = NotUseful;
   attr.override_redirect = True;
   attr.colormap = root.cmap;
   attr.border_pixel = 0;
   attr.background_pixel = 0;
   attr.save_under = True;
   win =
      XCreateWindow(d2, root.win, 0, 0, root.w, root.h, 0, root.depth,
		    InputOutput, root.vis,
		    CWOverrideRedirect | CWSaveUnder | CWBackingStore |
		    CWColormap | CWBackPixel | CWBorderPixel, &attr);
   pmap = ECreatePixmap(d2, win, root.w, root.h, root.depth);
   gcv.subwindow_mode = IncludeInferiors;
   gc = XCreateGC(d2, win, GCSubwindowMode, &gcv);
   XCopyArea(d2, root.win, pmap, gc, 0, 0, root.w, root.h, 0, 0);
   ESetWindowBackgroundPixmap(d2, win, pmap);
   EMapRaised(d2, win);
   EFreePixmap(d2, pmap);
   XFreeGC(d2, gc);
   val = win;
   a = XInternAtom(d2, "ENLIGHTENMENT_RESTART_SCREEN", False);
   XChangeProperty(d2, root.win, a, XA_CARDINAL, 32, PropModeReplace,
		   (unsigned char *)&val, 1);
   XSelectInput(d2, win, StructureNotifyMask);
   XUngrabServer(d2);
   XSync(d2, False);

#if !USE_IMLIB2
   if (!pImlib_Context)
     {
	i = 0;
	for (;;)
	  {
	     Cursor              cs = 0;
	     struct timeval      tv;

	     cs = XCreateFontCursor(d2, i);
	     XDefineCursor(d2, win, cs);
	     XSync(d2, False);
	     tv.tv_sec = 0;
	     tv.tv_usec = 50000;
	     select(0, NULL, NULL, NULL, &tv);
	     XFreeCursor(d2, cs);
	     i += 2;
	     if (i >= XC_num_glyphs)
		i = 0;
	  }
     }
   else
#endif
     {
	Window              w2, ww;
	char               *f, s[1024];
	Pixmap              pmap, mask;
	Imlib_Image        *im;
	struct timeval      tv;
	int                 dd, x, y;
	unsigned int        mm;
	Cursor              cs = 0;
	XColor              cl;
	GC                  gc;
	XGCValues           gcv;

	w2 = XCreateWindow(d2, win, 0, 0, 32, 32, 0, root.depth, InputOutput,
			   root.vis,
			   CWOverrideRedirect | CWBackingStore | CWColormap |
			   CWBackPixel | CWBorderPixel, &attr);
	pmap = ECreatePixmap(d2, w2, 16, 16, 1);
	gc = XCreateGC(d2, pmap, 0, &gcv);
	XSetForeground(d2, gc, 0);
	XFillRectangle(d2, pmap, gc, 0, 0, 16, 16);
	mask = ECreatePixmap(d2, w2, 16, 16, 1);
	gc = XCreateGC(d2, mask, 0, &gcv);
	XSetForeground(d2, gc, 0);
	XFillRectangle(d2, mask, gc, 0, 0, 16, 16);
	cs = XCreatePixmapCursor(d2, pmap, mask, &cl, &cl, 0, 0);
	XDefineCursor(d2, win, cs);
	XDefineCursor(d2, w2, cs);
	i = 1;
	for (;;)
	  {

	     i++;
	     if (i > 12)
		i = 1;

	     Esnprintf(s, sizeof(s), "pix/wait%i.png", i);

	     f = FindFile(s);
	     im = NULL;
	     if (f)
	       {
		  im = imlib_load_image(f);
		  Efree(f);
	       }

	     if (im)
	       {
		  imlib_context_set_image(im);
		  imlib_context_set_drawable(w2);
		  imlib_render_pixmaps_for_whole_image(&pmap, &mask);
		  EShapeCombineMask(d2, w2, ShapeBounding, 0, 0, mask,
				    ShapeSet);
		  ESetWindowBackgroundPixmap(d2, w2, pmap);
		  imlib_free_pixmap_and_mask(pmap);
		  XClearWindow(d2, w2);
		  XQueryPointer(d2, win, &ww, &ww, &dd, &dd, &x, &y, &mm);
		  EMoveResizeWindow(d2, w2,
				    x - imlib_image_get_width() / 2,
				    y - imlib_image_get_height() / 2,
				    imlib_image_get_width(),
				    imlib_image_get_height());
		  EMapWindow(d2, w2);
		  imlib_free_image();
	       }
	     tv.tv_sec = 0;
	     tv.tv_usec = 50000;
	     select(0, NULL, NULL, NULL, &tv);
	     XSync(d2, False);
	  }
     }
/*    {
 * XEvent              ev;
 * 
 * XSync(d2, False);
 * for (;;)
 * {
 * XNextEvent(d2, &ev);
 * }
 * } */
   exit(0);
}

void
SetupUserInitialization(void)
{

   FILE               *f;
   char                file[FILEPATH_LEN_MAX];

   EDBUG(3, "SetupUserInitialization");

   Esnprintf(file, sizeof(file), "%s/.initialized", EDirUser());
   if (isfile(file))
     {
	mode.firsttime = 0;
     }
   else
     {
	mode.firsttime = 1;
	f = fopen(file, "w");
	fprintf(f, "Congratulations, you have run enlightenment before.\n");
	fprintf(f, "Removing this file and the *.menu files in this\n");
	fprintf(f, "directory will cause enlightenment to regenerate them\n");
	fprintf(f, "and run the documentation browser.\n");
	fclose(f);
	if (fork())
	   EDBUG_RETURN_;
	Esnprintf(file, sizeof(file), "exec %s/scripts/e_gen_menu", EDirRoot());
	execl(usershell(getuid()), usershell(getuid()), "-c", (char *)file,
	      NULL);
	exit(0);

     }

   EDBUG_RETURN_;

}
