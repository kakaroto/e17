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

static void         EdgeTimeout(int val, void *data);

static char        *userDir = NULL;
static char        *cacheDir = NULL;

static const char  *const bins[] = { "eesh", "epp" };
static const char  *const thms[] = { "themes/DEFAULT/epplets/epplets.cfg" };

#define N_BINS (sizeof(bins)/sizeof(char*))
#define N_THMS (sizeof(thms)/sizeof(char*))

void
BlumFlimFrub(void)
{
   unsigned int        i;
   char                s[1024];

   for (i = 0; i < N_BINS; i++)
     {
	Esnprintf(s, sizeof(s), "%s/%s", EDirBin(), bins[i]);
	if (!exists(s))
	  {
	     Alert(_("!!!!!!!! ERROR ERROR ERROR ERROR !!!!!!!!\n" "\n"
		     "Enlightenment's utility executable cannot be found at:\n"
		     "\n" "%s\n"
		     "This is a fatal error and Enlightenment will cease to run.\n"
		     "Please rectify this situation and ensure it is installed\n"
		     "correctly.\n" "\n"
		     "The reason this could be missing is due to badly created\n"
		     "packages, someone manually deleting that program or perhaps\n"
		     "an error in installing Enlightenment.\n"), s);
	     EExit(NULL);
	  }
	if (!canexec(s))
	  {
	     Alert(_("!!!!!!!! ERROR ERROR ERROR ERROR !!!!!!!!\n" "\n"
		     "Enlightenment's utility executable is not able to be executed:\n"
		     "\n" "%s\n"
		     "This is a fatal error and Enlightenment will cease to run.\n"
		     "Please rectify this situation and ensure it is installed\n"
		     "correctly.\n"), s);
	     EExit(NULL);
	  }
     }

   for (i = 0; i < N_THMS; i++)
     {
	Esnprintf(s, sizeof(s), "%s/%s", EDirRoot(), thms[i]);
	if (!exists(s))
	  {
	     Alert(_("!!!!!!!! ERROR ERROR ERROR ERROR !!!!!!!!\n" "\n"
		     "Enlightenment's DEFAULT installed theme is missing or inadequately\n"
		     "configured to be a useful DEFAULT theme.\n" "\n"
		     "This is a fatal error and Enlightenment will cease to run.\n"
		     "Please rectify this situation and ensure it is installed\n"
		     "correctly. The DEFAULT theme Enlightenment comes with normally\n"
		     "is BrushedMetal-Tigert and this theme is adequate for a DEFAULT\n"
		     "theme.\n"));
	     EExit(NULL);
	  }
     }
}

const char         *
EDirBin(void)
{
   return ENLIGHTENMENT_BIN;
}

const char         *
EDirRoot(void)
{
   return ENLIGHTENMENT_ROOT;
}

void
EDirUserSet(const char *d)
{
   userDir = Estrdup(d);
}

char               *
EDirUser(void)
{
   char               *home, buf[4096];

   if (userDir)
      return userDir;

   home = homedir(getuid());
   Esnprintf(buf, sizeof(buf), "%s/.enlightenment", home);
   Efree(home);
   userDir = Estrdup(buf);

   return userDir;
}

void
EDirUserCacheSet(const char *d)
{
   cacheDir = Estrdup(d);
}

char               *
EDirUserCache(void)
{
   if (!cacheDir)
      cacheDir = Estrdup(EDirUser());
   return cacheDir;
}

int
EExit(void *code)
{
   long                exitcode = 0;

   EDBUG(9, "EExit");

   SaveSession(1);

   if (disp)
     {
	UngrabX();
	UnGrabTheButtons();

	/* This mechanism is only needed when the SM is unavailable: */
	SetEInfoOnAll();

	/* XSetInputFocus(disp, None, RevertToParent, CurrentTime); */
	/* I think this is a better way to release the grabs: (felix) */
	XSetInputFocus(disp, PointerRoot, RevertToPointerRoot, CurrentTime);
	XSelectInput(disp, root.win, 0);
	XCloseDisplay(disp);
     }

   XSetErrorHandler((XErrorHandler) NULL);
   XSetIOErrorHandler((XIOErrorHandler) NULL);

   SignalsRestore();

   if (master_pid == getpid())
     {
	int                 i;

	exitcode = (long)code;
	SoundExit();
	ThemeCleanup();
	for (i = 0; i < child_count; i++)
	   kill(e_children[i], SIGINT);
     }
   Real_SaveSnapInfo(0, NULL);

   exit(exitcode);
   EDBUG_RETURN(exitcode);
}

static Window       w1 = 0, w2 = 0, w3 = 0, w4 = 0;

void
ShowEdgeWindows(void)
{
   int                 ax, ay, cx, cy;

   if (Conf.edge_flip_resistance <= 0)
     {
	HideEdgeWindows();
	return;
     }
   if (!w1)
     {
	w1 = ECreateEventWindow(root.win, 0, 0, 1, root.h);
	w2 = ECreateEventWindow(root.win, root.w - 1, 0, 1, root.h);
	w3 = ECreateEventWindow(root.win, 0, 0, root.w, 1);
	w4 = ECreateEventWindow(root.win, 0, root.h - 1, root.w, 1);
	XSelectInput(disp, w1,
		     EnterWindowMask | LeaveWindowMask | PointerMotionMask |
		     ButtonPressMask | ButtonReleaseMask);
	XSelectInput(disp, w2,
		     EnterWindowMask | LeaveWindowMask | PointerMotionMask |
		     ButtonPressMask | ButtonReleaseMask);
	XSelectInput(disp, w3,
		     EnterWindowMask | LeaveWindowMask | PointerMotionMask |
		     ButtonPressMask | ButtonReleaseMask);
	XSelectInput(disp, w4,
		     EnterWindowMask | LeaveWindowMask | PointerMotionMask |
		     ButtonPressMask | ButtonReleaseMask);
     }
   GetCurrentArea(&cx, &cy);
   GetAreaSize(&ax, &ay);

   if (cx == 0 && !Conf.areas.wraparound)
      EUnmapWindow(disp, w1);
   else
      EMapRaised(disp, w1);
   if (cx == (ax - 1) && !Conf.areas.wraparound)
      EUnmapWindow(disp, w2);
   else
      EMapRaised(disp, w2);
   if (cy == 0 && !Conf.areas.wraparound)
      EUnmapWindow(disp, w3);
   else
      EMapRaised(disp, w3);
   if (cy == (ay - 1) && !Conf.areas.wraparound)
      EUnmapWindow(disp, w4);
   else
      EMapRaised(disp, w4);
}

void
HideEdgeWindows(void)
{
   if (w1)
     {
	EUnmapWindow(disp, w1);
	EUnmapWindow(disp, w2);
	EUnmapWindow(disp, w3);
	EUnmapWindow(disp, w4);
     }
}

int
IsEdgeWin(Window win)
{
   if (!w1)
      return -1;
   if (win == w1)
      return 0;
   else if (win == w2)
      return 1;
   else if (win == w3)
      return 2;
   else if (win == w4)
      return 3;
   return -1;
}

void
EdgeHandleEnter(XEvent * ev)
{
   int                 dir;

   dir = IsEdgeWin(ev->xcrossing.window);
   if (dir < 0)
      return;
   DoIn("EDGE_TIMEOUT", ((double)Conf.edge_flip_resistance) / 100.0,
	EdgeTimeout, dir, NULL);
}

void
EdgeHandleLeave(XEvent * ev)
{
   int                 dir;

   dir = IsEdgeWin(ev->xcrossing.window);
   if (dir < 0)
      return;
   RemoveTimerEvent("EDGE_TIMEOUT");
}

void
EdgeHandleMotion(XEvent * ev)
{
   static int          lastdir = -1;
   int                 dir;

   if (Mode.mode != MODE_MOVE_PENDING && Mode.mode != MODE_MOVE)
      return;

   dir = -1;
   if (ev->xmotion.x_root == 0)
      dir = 0;
   else if (ev->xmotion.x_root == (root.w - 1))
      dir = 1;
   else if (ev->xmotion.y_root == 0)
      dir = 2;
   else if (ev->xmotion.y_root == (root.h - 1))
      dir = 3;

   if ((lastdir != dir) && (Conf.edge_flip_resistance))
     {
	if (dir < 0)
	   RemoveTimerEvent("EDGE_TIMEOUT");
	else
	   DoIn("EDGE_TIMEOUT", ((double)Conf.edge_flip_resistance) / 100.0,
		EdgeTimeout, dir, NULL);
	lastdir = dir;
     }
}

extern char         throw_move_events_away;

static void
EdgeTimeout(int val, void *data)
{
   int                 ax, ay, aw, ah, dx, dy, dax, day;

   if (Mode.cur_menu_mode > 0)
      return;
   if (!Conf.edge_flip_resistance)
      return;
   throw_move_events_away = 1;
   GetCurrentArea(&ax, &ay);
   GetAreaSize(&aw, &ah);
   dx = 0;
   dy = 0;
   dax = 0;
   day = 0;
   switch (val)
     {
     case 0:
	if (ax == 0 && !Conf.areas.wraparound)
	   return;
	dx = root.w - 2;
	dax = -1;
	break;
     case 1:
	if (ax == (aw - 1) && !Conf.areas.wraparound)
	   return;
	dx = -(root.w - 2);
	dax = 1;
	break;
     case 2:
	if (ay == 0 && !Conf.areas.wraparound)
	   return;
	dy = root.h - 2;
	day = -1;
	break;
     case 3:
	if (ay == (ah - 1) && !Conf.areas.wraparound)
	   return;
	dy = -(root.h - 2);
	day = 1;
	break;
     default:
	break;
     }
   Mode.flipp = 1;
   MoveCurrentAreaBy(dax, day);
   Mode.flipp = 0;
   if (aw == 1)
      dx = 0;
   if (ah == 1)
      dy = 0;
   XWarpPointer(disp, None, None, 0, 0, 0, 0, dx, dy);
   data = NULL;
}

/* This is a general quicksort algorithm, using median-of-three strategy.
 * 
 * Parameters:
 * ===========
 * a:            array of items to be sorted (list of void pointers).
 * l:            left edge of sub-array to be sorted. Toplevel call has 0 here.
 * r:            right edge of sub-array to be sorted. Toplevel call has |a| - 1 here.
 * CompareFunc:  Pointer to a function that accepts two general items d1 and d2
 * and returns values as follows:
 * 
 * < 0  --> d1 "smaller" than d2
 * > 0  --> d1 "larger"  than d2
 * 0    --> d1 "==" d2.
 * 
 * See sample application in ipc.c's IPC_Help.
 */
void
Quicksort(void **a, int l, int r, int (*CompareFunc) (void *d1, void *d2))
{

   int                 i, j, m;
   void               *v, *t;

   if (r > l)
     {

	m = (r + l) / 2 + 1;
	if (CompareFunc(a[l], a[r]) > 0)
	  {
	     t = a[l];
	     a[l] = a[r];
	     a[r] = t;
	  }
	if (CompareFunc(a[l], a[m]) > 0)
	  {
	     t = a[l];
	     a[l] = a[m];
	     a[m] = t;
	  }
	if (CompareFunc(a[r], a[m]) > 0)
	  {
	     t = a[r];
	     a[r] = a[m];
	     a[m] = t;
	  }

	v = a[r];
	i = l - 1;
	j = r;

	for (;;)
	  {
	     while (CompareFunc(a[++i], v) < 0);
	     while (CompareFunc(a[--j], v) > 0);
	     if (i >= j)
		break;
	     t = a[i];
	     a[i] = a[j];
	     a[j] = t;
	  }
	t = a[i];
	a[i] = a[r];
	a[r] = t;
	Quicksort(a, l, i - 1, CompareFunc);
	Quicksort(a, i + 1, r, CompareFunc);
     }
}

#if !USE_LIBC_SETENV
int
Esetenv(const char *name, const char *value, int overwrite __UNUSED__)
{
   char                envvar[FILEPATH_LEN_MAX];

   Esnprintf(envvar, FILEPATH_LEN_MAX, "%s=%s", name, value);
   return putenv(Estrdup(envvar));
}
#endif
