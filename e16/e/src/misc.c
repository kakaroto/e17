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

   if (Mode.wm.master)
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

void
Eprintf(const char *fmt, ...)
{
   va_list             args;

   fprintf(stdout, "[%d] ", getpid());
   va_start(args, fmt);
   vfprintf(stdout, fmt, args);
   va_end(args);
}
