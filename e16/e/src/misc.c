
#include "E.h"

void                EdgeTimeout(int val, void *data);

static char        *dir = NULL;

void
SetEDir(char *d)
{
   dir = duplicate(d);
}

char               *
UserEDir(void)
{
   if (dir)
      return dir;

   {
      char               *home, buf[4096];

      home = homedir(getuid());
      Esnprintf(buf, sizeof(buf), "%s/.enlightenment", home);
      Efree(home);
      dir = duplicate(buf);
   }
   return dir;
}

int
EExit(void *code)
{
   int                 exitcode = 0;

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
   signal(SIGHUP, SIG_DFL);
   signal(SIGINT, SIG_DFL);
   signal(SIGQUIT, SIG_DFL);
   signal(SIGILL, SIG_DFL);
   signal(SIGABRT, SIG_DFL);
   signal(SIGFPE, SIG_IGN);
   signal(SIGSEGV, SIG_IGN);
   signal(SIGPIPE, SIG_DFL);
   signal(SIGALRM, SIG_DFL);
   signal(SIGTERM, SIG_DFL);
   signal(SIGUSR1, SIG_DFL);
   signal(SIGUSR2, SIG_DFL);
   signal(SIGCHLD, SIG_DFL);
   signal(SIGTSTP, SIG_DFL);
   signal(SIGBUS, SIG_IGN);

   if (master_pid == getpid())
     {
	exitcode = (int)code;
	SoundExit();
	if (mustdel)
	  {
	     char                sss[FILEPATH_LEN_MAX];

	     Esnprintf(sss, sizeof(sss), "/bin/rm -rf %s", themepath);
	     system(sss);
	  }
     }
   SaveSnapInfo();

   exit(exitcode);
   EDBUG_RETURN(exitcode);
}

static Window       w1 = 0, w2 = 0, w3 = 0, w4 = 0;

void
ShowEdgeWindows(void)
{
   int                 ax, ay, cx, cy;

   if (mode.edge_flip_resistance <= 0)
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
	XSelectInput(disp, w1, EnterWindowMask | LeaveWindowMask |
		     PointerMotionMask | ButtonPressMask | ButtonReleaseMask);
	XSelectInput(disp, w2, EnterWindowMask | LeaveWindowMask |
		     PointerMotionMask | ButtonPressMask | ButtonReleaseMask);
	XSelectInput(disp, w3, EnterWindowMask | LeaveWindowMask |
		     PointerMotionMask | ButtonPressMask | ButtonReleaseMask);
	XSelectInput(disp, w4, EnterWindowMask | LeaveWindowMask |
		     PointerMotionMask | ButtonPressMask | ButtonReleaseMask);
     }
   GetCurrentArea(&cx, &cy);
   GetAreaSize(&ax, &ay);

   if (cx == 0)
      EUnmapWindow(disp, w1);
   else
      EMapRaised(disp, w1);
   if (cx == (ax - 1))
      EUnmapWindow(disp, w2);
   else
      EMapRaised(disp, w2);
   if (cy == 0)
      EUnmapWindow(disp, w3);
   else
      EMapRaised(disp, w3);
   if (cy == (ay - 1))
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
   DoIn("EDGE_TIMEOUT", ((double)mode.edge_flip_resistance) / 100.0,
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

   if (mode.mode != MODE_MOVE)
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

   if ((lastdir != dir) && (mode.edge_flip_resistance))
     {
	if (dir < 0)
	   RemoveTimerEvent("EDGE_TIMEOUT");
	else
	   DoIn("EDGE_TIMEOUT", ((double)mode.edge_flip_resistance) / 100.0,
		EdgeTimeout, dir, NULL);
	lastdir = dir;
     }
}

void
EdgeTimeout(int val, void *data)
{
   int                 ax, ay, aw, ah, dx, dy, dax, day;

   if (mode.cur_menu_mode > 0)
      return;
   if (!mode.edge_flip_resistance)
      return;
   GetCurrentArea(&ax, &ay);
   GetAreaSize(&aw, &ah);
   dx = 0;
   dy = 0;
   dax = 0;
   day = 0;
   switch (val)
     {
     case 0:
	if (ax == 0)
	   return;
	dx = root.w - 2;
	dax = -1;
	break;
     case 1:
	if (ax == (aw - 1))
	   return;
	dx = -(root.w - 2);
	dax = 1;
	break;
     case 2:
	if (ay == 0)
	   return;
	dy = root.h - 2;
	day = -1;
	break;
     case 3:
	if (ay == (ah - 1))
	   return;
	dy = -(root.h - 2);
	day = 1;
	break;
     default:
	break;
     }
   mode.flipp = 1;
   MoveCurrentAreaBy(dax, day);
   mode.flipp = 0;
   XWarpPointer(disp, None, None, 0, 0, 0, 0, dx, dy);
   data = NULL;
}

/* be paranoid and check for files being in theme */
char
SanitiseThemeDir(char *dir)
{
   char                s[4096];

   return 1;
   Esnprintf(s, sizeof(s), "%s/%s", dir, "borders.cfg");
   if (!isfile(s))
      return 0;
   Esnprintf(s, sizeof(s), "%s/%s", dir, "buttons.cfg");
   if (!isfile(s))
      return 0;
   Esnprintf(s, sizeof(s), "%s/%s", dir, "colormodifiers.cfg");
   if (!isfile(s))
      return 0;
   Esnprintf(s, sizeof(s), "%s/%s", dir, "cursors.cfg");
   if (!isfile(s))
      return 0;
   Esnprintf(s, sizeof(s), "%s/%s", dir, "desktops.cfg");
   if (!isfile(s))
      return 0;
   Esnprintf(s, sizeof(s), "%s/%s", dir, "imageclasses.cfg");
   if (!isfile(s))
      return 0;
   Esnprintf(s, sizeof(s), "%s/%s", dir, "init.cfg");
   if (!isfile(s))
      return 0;
   Esnprintf(s, sizeof(s), "%s/%s", dir, "menustyles.cfg");
   if (!isfile(s))
      return 0;
   Esnprintf(s, sizeof(s), "%s/%s", dir, "slideouts.cfg");
   if (!isfile(s))
      return 0;
   Esnprintf(s, sizeof(s), "%s/%s", dir, "sound.cfg");
   if (!isfile(s))
      return 0;
   Esnprintf(s, sizeof(s), "%s/%s", dir, "tooltips.cfg");
   if (!isfile(s))
      return 0;
   Esnprintf(s, sizeof(s), "%s/%s", dir, "windowmatches.cfg");
   if (!isfile(s))
      return 0;
   Esnprintf(s, sizeof(s), "%s/%s", dir, "menus.cfg");
   if (isfile(s))
      return 0;
   Esnprintf(s, sizeof(s), "%s/%s", dir, "control.cfg");
   if (isfile(s))
      return 0;
   Esnprintf(s, sizeof(s), "%s/%s", dir, "keybindings.cfg");
   if (isfile(s))
      return 0;
   return 1;
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
