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
#include <sys/time.h>

#ifndef __EMX__
int
AddEToFile(char *file)
{
   FILE               *f1, *f2;
   char                tmp[2048], s[2048];
   char               *s1, *s2;
   char                hase = 0;
   char                foundwm = 0;
   char               *wms[] = {
      "wmaker", "afterstep", "fvwm", "fvwm2", "twm", "mwm", "vtwm", "ctwm",
      "gwm",
      "mlvwm", "kwm", "olwm", "wm2", "wmx", "olvwm", "9wm", "blackbox", "awm",
      "uwm",
      "amiwm", "dtwm", "4dwm", "scwm", "fvwm95", "fvwm95-2", "tvtwm",
      "bowman",
      "qwm", "icewm", "qvwm", "gnome-session", "xsm", "startkde"
   };
   int                 wmnum = 33;
   int                 i;

   EDBUG(8, "AddEToFile");
   if (!exists(file))
      EDBUG_RETURN(0);

   Esnprintf(tmp, 2048, "%s/estrt_%i",
	     (getenv("TMPDIR") == NULL) ? "/tmp" : getenv("TMPDIR"),
	     (unsigned)time(NULL));
   f1 = fopen(file, "r");
   if (!f1)
      EDBUG_RETURN(0);
   f2 = fopen(tmp, "w");
   if (!f2)
     {
	fclose(f1);
	EDBUG_RETURN(0);
     }
   while (fgets(s, 2048, f1))
     {
	s1 = strstr(s, "enlightenment");
	if (s1)
	  {
	     s2 = strstr(s, "#");
	     if (((!s2) || (s1 < s2)) && (!foundwm))
		hase = 1;
	  }
	for (i = 0; i < wmnum; i++)
	  {
	     s1 = strstr(s, wms[i]);
	     if (s1)
	       {
		  s2 = strstr(s, "#");
		  if ((!s2) || (s1 < s2))
		    {
		       fprintf(f2, "#%s", s);
		       fprintf(f2,
			       "\n# Enlightenment inserted Execution string here\n");
		       fprintf(f2, "exec %s/enlightenment\n\n",
			       ENLIGHTENMENT_BIN);
		       foundwm = 1;
		       i = wmnum + 1;
		    }
	       }
	  }
	if (i <= wmnum)
	   fprintf(f2, "%s", s);
     }
   fclose(f1);
   fclose(f2);
   if (!foundwm)
     {
	f1 = fopen(file, "r");
	if (!f1)
	   EDBUG_RETURN(0);
	f2 = fopen(tmp, "w");
	if (!f2)
	  {
	     fclose(f1);
	     EDBUG_RETURN(0);
	  }
	fprintf(f2, "\n# Enlightenment inserted Execution string here\n");
	fprintf(f2, "exec %s/enlightenment\n\n", ENLIGHTENMENT_BIN);
	while (fgets(s, 2048, f1))
	   fprintf(f2, "%s", s);
	fclose(f1);
	fclose(f2);
     }
   if (!hase)
      cp(tmp, file);
   rm(tmp);
   EDBUG_RETURN(1);
}

int
CreateEFile(char *file)
{
   FILE               *f;

   EDBUG(8, "CreateEFile");
   f = fopen(file, "w");
   if (!f)
      EDBUG_RETURN(0);
   fprintf(f, "# Enlightenment inserted Execution string here\n");
   fprintf(f, "exec %s/enlightenment\n", ENLIGHTENMENT_BIN);
   fclose(f);
   EDBUG_RETURN(1);
}
#endif

void
AddE()
{
#ifndef __EMX__
   char               *h;
   char                s[1024];
   int                 val;

   EDBUG(6, "AddE");
   val = 0;
   h = homedir(getuid());
   Esnprintf(s, 1024, "%s/.xsession", h);
   val |= AddEToFile(s);
   Esnprintf(s, 1024, "%s/.xinitrc", h);
   val |= AddEToFile(s);
   Esnprintf(s, 1024, "%s/.Xclients", h);
   val |= AddEToFile(s);
   if (!val)
     {
	Esnprintf(s, 1024, "%s/.xsession", h);
	CreateEFile(s);
	Esnprintf(s, 1024, "%s/.xinitrc", h);
	CreateEFile(s);
	Esnprintf(s, 1024, "%s/.Xclients", h);
	CreateEFile(s);
     }
   if (h)
      Efree(h);
   EDBUG_RETURN_;
#endif
}

void
CreateStartupDisplay(char start)
{
   static Window       w1, w2, win1, win2, b1, b2;
   static Background  *bg = NULL;
   static Background  *bg_sideways = NULL;	/* currently used to determine if the startup screen should slide sideways */
   static ImageClass  *ic = NULL;
   char                pq;

   EDBUG(6, "CreateStartupDisplay");
   if (init_win_ext)
      EDBUG_RETURN_;
   if (start)
     {
	bg_sideways =
	   (Background *) FindItem("STARTUP_BACKGROUND_SIDEWAYS", 0,
				   LIST_FINDBY_NAME, LIST_TYPE_BACKGROUND);
	ic = (ImageClass *) FindItem("STARTUP_BAR", 0, LIST_FINDBY_NAME,
				     LIST_TYPE_ICLASS);
	if (!ic)
	   ic = (ImageClass *) FindItem("DESKTOP_DRAGBUTTON_HORIZ", 0,
					LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	bg = (Background *) FindItem("STARTUP_BACKGROUND", 0, LIST_FINDBY_NAME,
				     LIST_TYPE_BACKGROUND);
	if ((!ic) || (!bg))
	   EDBUG_RETURN_;

	if (bg_sideways)
	  {
	     w1 = ECreateWindow(root.win, (root.w / 2), 0, root.w, root.h, 1);
	     w2 = ECreateWindow(root.win, -(root.w / 2), 0, root.w, root.h, 1);
	     win1 = ECreateWindow(w1, -(root.w / 2), 0, root.w, root.h, 0);
	     win2 = ECreateWindow(w2, (root.w / 2), 0, root.w, root.h, 0);
	  }
	else
	  {
	     w1 = ECreateWindow(root.win, 0, -(root.h / 2), root.w, root.h, 1);
	     w2 = ECreateWindow(root.win, 0, (root.h / 2), root.w, root.h, 1);
	     win1 = ECreateWindow(w1, 0, (root.h / 2), root.w, root.h, 0);
	     win2 = ECreateWindow(w2, 0, -(root.h / 2), root.w, root.h, 0);
	  }

	EMapWindow(disp, win1);
	EMapWindow(disp, win2);

	b1 = ECreateWindow(w1, 0, root.h - desks.dragbar_width, root.w,
			   desks.dragbar_width, 0);
	b2 = ECreateWindow(w2, 0, 0, root.w, desks.dragbar_width, 0);
	EMapRaised(disp, b1);
	EMapRaised(disp, b2);

	pq = queue_up;
	queue_up = 0;
	IclassApply(ic, b1, root.w, desks.dragbar_width, 0, 0, 0, 0);
	IclassApply(ic, b2, root.w, desks.dragbar_width, 0, 0, 0, 0);
	queue_up = pq;
	SetBackgroundTo(win1, bg, 1);
	SetBackgroundTo(win2, bg, 1);
	if (bg->pmap)
	   imlib_free_pixmap_and_mask(bg->pmap);
	bg->pmap = 0;
	init_win1 = w1;
	init_win2 = w2;
	EMapRaised(disp, w1);
	EMapRaised(disp, w2);
     }
   else
     {
	int                 k, spd, x, y, xOffset, yOffset, ty, fy;
	int                 min, speed;

	/* we have this many so that we save on lines o code - eAndroid */
	struct timeval      timev1, timev2;
	int                 dsec, dusec;
	double              tm;

	if ((!ic) || (!bg))
	   EDBUG_RETURN_;

	speed = mode.slidespeedcleanup / 2;
	spd = 16;
	min = 2;
	fy = 0;

	for (k = 0; k <= 1024; k += spd)
	  {
	     if (bg_sideways)
	       {		/* so we can have two different slide methods */
		  ty = (root.w / 2);
		  xOffset = ((fy * (1024 - k)) + (ty * k)) >> 10;
		  x = ty;
		  yOffset = 0;
		  y = 0;
	       }
	     else
	       {
		  ty = (root.h / 2);
		  xOffset = 0;
		  x = 0;
		  yOffset = ((fy * (1024 - k)) + (ty * k)) >> 10;
		  y = ty;
	       }
	     gettimeofday(&timev1, NULL);
	     EMoveWindow(disp, w1, x + xOffset, -y - yOffset);
	     EMoveWindow(disp, w2, -x - xOffset, y + yOffset);
	     XSync(disp, False);
	     gettimeofday(&timev2, NULL);
	     dsec = timev2.tv_sec - timev1.tv_sec;
	     dusec = timev2.tv_usec - timev1.tv_usec;
	     if (dusec < 0)
	       {
		  dsec--;
		  dusec += 1000000;
	       }
	     tm = (double)dsec + (((double)dusec) / 1000000);
	     spd = (int)((double)speed * tm);
	     if (spd < min)
		spd = min;
	  }

	EDestroyWindow(disp, w1);
	EDestroyWindow(disp, w2);
	init_win1 = 0;
	init_win2 = 0;
     }
   EDBUG_RETURN_;
}
