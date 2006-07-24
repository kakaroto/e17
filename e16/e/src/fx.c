/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "desktops.h"
#include "dialog.h"
#include "eimage.h"
#include "emodule.h"
#include "parse.h"
#include "timers.h"
#include "xwin.h"
#include <math.h>

/* FIXME - Needs cleaning up */

/* Someone may do this right one day, but for now - kill'em */
#define ENABLE_FX_INFO 0

#ifndef M_PI_2
#define M_PI_2 (3.141592654 / 2)
#endif

#define FX_OP_START  1
#define FX_OP_STOP   2
#define FX_OP_TOGGLE 3

typedef struct
{
   const char         *name;
   void                (*init_func) (const char *name);
   void                (*desk_func) (void);
   void                (*quit_func) (void);
   void                (*pause_func) (void);
   char                enabled;
   char                paused;
}
FXHandler;

/****************************** RIPPLES *************************************/

#define fx_ripple_waterh 64
static Pixmap       fx_ripple_above = None;
static Win          fx_ripple_win = NoWin;
static int          fx_ripple_count = 0;

static void
FX_ripple_info(void)
{
#if ENABLE_FX_INFO
   static char         before = 0;

   if (!before)
      DialogOK(_("Starting up Ripples FX..."),
	       _("\n" "You have just started the Ripples Effect.\n" "\n"
		 "If you look closely on your desktop background, and if it\n"
		 "doesn't have a solid colour (i.e. has a background texture or\n"
		 "image), you will see a pool of water at the bottom of your\n"
		 "screen that reflects everything above it and \"ripples\".\n"
		 "\n"
		 "To disable this effect just select this option again to toggle\n"
		 "it off.\n"));
   before = 1;
#endif
}

static void
FX_ripple_timeout(int val __UNUSED__, void *data __UNUSED__)
{
   static double       incv = 0, inch = 0;
   static GC           gc1 = 0, gc = 0;
   int                 y;

   if (fx_ripple_above == None)
     {
	XGCValues           gcv;

	fx_ripple_win = DeskGetBackgroundWin(DesksGetCurrent());

	fx_ripple_above =
	   ECreatePixmap(fx_ripple_win, VRoot.w, fx_ripple_waterh * 2, 0);
	if (gc)
	   EXFreeGC(gc);
	if (gc1)
	   EXFreeGC(gc1);
	gcv.subwindow_mode = IncludeInferiors;
	gc = EXCreateGC(Xwin(fx_ripple_win), GCSubwindowMode, &gcv);
	gc1 = EXCreateGC(Xwin(fx_ripple_win), 0L, &gcv);

	FX_ripple_info();
     }

   if (fx_ripple_count == 0)
      XCopyArea(disp, EoGetXwin(DesksGetCurrent()), fx_ripple_above, gc, 0,
		VRoot.h - (fx_ripple_waterh * 3), VRoot.w, fx_ripple_waterh * 2,
		0, 0);

   fx_ripple_count++;
   if (fx_ripple_count > 32)
      fx_ripple_count = 0;
   incv += 0.40;
   if (incv > (M_PI_2 * 4))
     {
	incv = 0;
     }
   inch += 0.32;
   if (inch > (M_PI_2 * 4))
     {
	inch = 0;
     }
   for (y = 0; y < fx_ripple_waterh; y++)
     {
	double              aa, a, p;
	int                 yoff, off, yy;

	p = (((double)(fx_ripple_waterh - y)) / ((double)fx_ripple_waterh));
	a = p * p * 48 + incv;
	yoff = y + (int)(sin(a) * 7) + 1;
	yy = (fx_ripple_waterh * 2) - yoff;
	aa = p * p * 64 + inch;
	off = (int)(sin(aa) * 10 * (1 - p));
	XCopyArea(disp, fx_ripple_above, Xwin(fx_ripple_win), gc1, 0, yy,
		  VRoot.w, 1, off, VRoot.h - fx_ripple_waterh + y);
     }
   DoIn("FX_RIPPLE_TIMEOUT", 0.066, FX_ripple_timeout, 0, NULL);
}

static void
FX_Ripple_Init(const char *name __UNUSED__)
{
   fx_ripple_count = 0;
   DoIn("FX_RIPPLE_TIMEOUT", 0.066, FX_ripple_timeout, 0, NULL);
}

static void
FX_Ripple_Desk(void)
{
   if (fx_ripple_above != None)
      EFreePixmap(fx_ripple_above);
   fx_ripple_count = 0;
   fx_ripple_above = None;
}

static void
FX_Ripple_Quit(void)
{
   RemoveTimerEvent("FX_RIPPLE_TIMEOUT");
   EClearArea(fx_ripple_win, 0, VRoot.h - fx_ripple_waterh, VRoot.w,
	      fx_ripple_waterh, False);
}

static void
FX_Ripple_Pause(void)
{
   static char         paused = 0;

   if (!paused)
     {
	FX_Ripple_Quit();
	paused = 1;
     }
   else
     {
	FX_Ripple_Init(NULL);
	paused = 0;
     }
}

#ifdef E_FX_RAINDROPS		/* FIXME - Requires eliminating use of PixImg */

/****************************** RAIN DROPS **********************************/

#define fx_raindrop_size 96
#define fx_raindrop_size2 (fx_raindrop_size / 2)
#define fx_raindrop_duration 32
#define fx_frequency 4
#define fx_amplitude 48
static Window       fx_raindrops_win = None;
static int          fx_raindrops_number = 4;
static PixImg      *fx_raindrops_draw = NULL;

typedef struct
{
   int                 x, y;
   int                 count;
   PixImg             *buf;
}
DropContext;

static DropContext  fx_raindrops[4];

static void
FX_raindrops_info(void)
{
#if ENABLE_FX_INFO
   static char         before = 0;

   if (!before)
      DialogOK(_("Starting up Raindrops FX..."),
	       _("\n" "You have just started the Raindrops Effect.\n"
		 "\n"
		 "If you look closely on your desktop background, and if it\n"
		 "doesn't have a solid colour (i.e. has a background texture or\n"
		 "image), you will see \"raindrops\" hit the background and\n"
		 "make little splashes. This Effect can be VERY CPU intensive.\n"
		 "\n"
		 "To disable this effect just select this option again to toggle\n"
		 "it off.\n"));
   before = 1;
#endif
}

static void
FX_raindrops_timeout(int val __UNUSED__, void *data __UNUSED__)
{
   static GC           gc1 = 0, gc = 0;
   int                 i, x, y, xx, yy;
   int                 percent_done;
   static char         first = 1;
   static char         sintab[256];
   static unsigned char disttab[fx_raindrop_size][fx_raindrop_size];

   if (fx_raindrops_win == None)
     {
	XGCValues           gcv;

	FX_raindrops_info();

	if (first)
	  {
	     int                 j;

	     first = 0;
	     for (i = 0; i < 256; i++)
		sintab[i] =
		   (char)(sin(((double)i) * M_PI_2 * 4 / 256) * fx_amplitude);
	     for (j = 0; j < fx_raindrop_size; j++)
	       {
		  for (i = 0; i < fx_raindrop_size; i++)
		    {
		       xx = i - fx_raindrop_size2;
		       yy = j - fx_raindrop_size2;
		       disttab[i][j] =
			  (unsigned char)sqrt((double)((xx * xx) + (yy * yy)));
		    }
	       }
	  }

	fx_raindrops_win = DeskGetBackgroundWin(DesksGetCurrent());

	if (gc)
	   EXFreeGC(gc);
	if (gc1)
	   EXFreeGC(gc1);

	gcv.subwindow_mode = IncludeInferiors;
	gc = EXCreateGC(fx_raindrops_win, GCSubwindowMode, &gcv);
	gc1 = EXCreateGC(fx_raindrops_win, 0L, &gcv);

	fx_raindrops_draw =
	   ECreatePixImg(fx_raindrops_win, fx_raindrop_size, fx_raindrop_size);

	if (!fx_raindrops_draw)
	   return;

	for (i = 0; i < fx_raindrops_number; i++)
	  {
	     fx_raindrops[i].buf =
		ECreatePixImg(fx_raindrops_win, fx_raindrop_size,
			      fx_raindrop_size);
	     if (fx_raindrops[i].buf)
		XShmGetImage(disp, fx_raindrops_win, fx_raindrops[i].buf->xim,
			     fx_raindrops[i].x, fx_raindrops[i].y, 0xffffffff);
	     if (!fx_raindrops[i].buf)
		return;
	  }
     }

   for (i = 0; i < fx_raindrops_number; i++)
     {
	fx_raindrops[i].count++;
	if (fx_raindrops[i].count == fx_raindrop_duration)
	  {
	     int                 j, count = 0;
	     char                intersect = 1;

	     XClearArea(disp, fx_raindrops_win, fx_raindrops[i].x,
			fx_raindrops[i].y, fx_raindrop_size, fx_raindrop_size,
			False);
	     fx_raindrops[i].count = 0;
	     while (intersect)
	       {
		  count++;
		  if (count > 10240)
		     break;
		  intersect = 0;
		  for (j = 0; j < fx_raindrops_number; j++)
		    {
		       fx_raindrops[i].x =
			  rand() % (VRoot.w - fx_raindrop_size);
		       fx_raindrops[i].y =
			  rand() % (VRoot.h - fx_raindrop_size);
		       if (fx_raindrops[i].x < 0)
			  fx_raindrops[i].x = 0;
		       else if (fx_raindrops[i].x >
				(VRoot.w - fx_raindrop_size))
			  fx_raindrops[i].x = VRoot.w - fx_raindrop_size;
		       if (fx_raindrops[i].y < 0)
			  fx_raindrops[i].y = 0;
		       else if (fx_raindrops[i].y >
				(VRoot.h - fx_raindrop_size))
			  fx_raindrops[i].y = VRoot.h - fx_raindrop_size;
		       if (i != j)
			 {
			    if (((fx_raindrops[i].x >= fx_raindrops[j].x)
				 && (fx_raindrops[i].x <
				     fx_raindrops[j].x + fx_raindrop_size)
				 && (fx_raindrops[i].y >= fx_raindrops[j].y)
				 && (fx_raindrops[i].y <
				     fx_raindrops[j].y + fx_raindrop_size))
				||
				((fx_raindrops
				  [i].x + fx_raindrop_size >= fx_raindrops[j].x)
				 && (fx_raindrops[i].x + fx_raindrop_size <
				     fx_raindrops[j].x + fx_raindrop_size)
				 && (fx_raindrops[i].y >= fx_raindrops[j].y)
				 && (fx_raindrops[i].y <
				     fx_raindrops[j].y + fx_raindrop_size))
				|| ((fx_raindrops[i].x >= fx_raindrops[j].x)
				    && (fx_raindrops[i].x <
					fx_raindrops[j].x + fx_raindrop_size)
				    && (fx_raindrops[i].y + fx_raindrop_size >=
					fx_raindrops[j].y)
				    && (fx_raindrops[i].y + fx_raindrop_size <
					fx_raindrops[j].y + fx_raindrop_size))
				||
				((fx_raindrops
				  [i].x + fx_raindrop_size >= fx_raindrops[j].x)
				 && (fx_raindrops[i].x + fx_raindrop_size <
				     fx_raindrops[j].x + fx_raindrop_size)
				 && (fx_raindrops[i].y + fx_raindrop_size >=
				     fx_raindrops[j].y)
				 && (fx_raindrops[i].y + fx_raindrop_size <
				     fx_raindrops[j].y + fx_raindrop_size)))
			       intersect = 1;
			 }
		    }
	       }
	     XShmGetImage(disp, fx_raindrops_win, fx_raindrops[i].buf->xim,
			  fx_raindrops[i].x, fx_raindrops[i].y, 0xffffffff);
	  }
	percent_done =
	   1 + ((fx_raindrops[i].count << 8) / fx_raindrop_duration);
	for (y = 0; y < fx_raindrop_size; y++)
	  {
	     for (x = 0; x < fx_raindrop_size; x++)
	       {
		  int                 dist;

		  dist = disttab[x][y];
		  if (dist > fx_raindrop_size2)
		     XPutPixel(fx_raindrops_draw->xim, x, y,
			       XGetPixel(fx_raindrops[i].buf->xim, x, y));
		  else
		    {
		       int                 percent;

		       percent = 1 + ((dist << 8) / fx_raindrop_size2);
		       if (percent > percent_done)
			  XPutPixel(fx_raindrops_draw->xim, x, y,
				    XGetPixel(fx_raindrops[i].buf->xim, x, y));
		       else
			 {
			    int                 varx, vary;
			    int                 phase, divisor, multiplier;

			    phase =
			       ((percent - percent_done) * fx_frequency) & 0xff;
			    xx = x - fx_raindrop_size2;
			    yy = y - fx_raindrop_size2;
			    divisor = 1 + (dist << 8);
			    multiplier =
			       (int)sintab[phase] * (256 - percent_done);
			    varx = ((-xx) * multiplier) / divisor;
			    vary = ((-yy) * multiplier) / divisor;
			    xx = x + varx;
			    yy = y + vary;
			    if (xx < 0)
			       xx = 0;
			    else if (xx >= fx_raindrop_size)
			       xx = fx_raindrop_size - 1;
			    if (yy < 0)
			       yy = 0;
			    else if (yy >= fx_raindrop_size)
			       yy = fx_raindrop_size - 1;
			    XPutPixel(fx_raindrops_draw->xim, x, y,
				      XGetPixel(fx_raindrops[i].buf->xim, xx,
						yy));
			 }
		    }
	       }
	  }
	XShmPutImage(disp, fx_raindrops_win, gc1, fx_raindrops_draw->xim, 0, 0,
		     fx_raindrops[i].x, fx_raindrops[i].y, fx_raindrop_size,
		     fx_raindrop_size, False);
	ESync();
     }

   DoIn("FX_RAINDROPS_TIMEOUT", (0.066 /*/ (float)fx_raindrops_number */ ),
	FX_raindrops_timeout, 0, NULL);
}

static void
FX_Raindrops_Init(const char *name __UNUSED__)
{
   int                 i;

   fx_raindrops_win = None;
   for (i = 0; i < fx_raindrops_number; i++)
     {
	fx_raindrops[i].count = rand() % fx_raindrop_duration;
	fx_raindrops[i].x = rand() % (VRoot.w - fx_raindrop_size);
	fx_raindrops[i].y = rand() % (VRoot.h - fx_raindrop_size);
     }
   DoIn("FX_RAINDROPS_TIMEOUT", 0.066, FX_raindrops_timeout, 0, NULL);
}

static void
FX_Raindrops_Desk(void)
{
   fx_raindrops_win = None;
}

static void
FX_Raindrops_Quit(void)
{
   int                 i;

   RemoveTimerEvent("FX_RAINDROPS_TIMEOUT");
   for (i = 0; i < fx_raindrops_number; i++)
     {
	XClearArea(disp, fx_raindrops_win, fx_raindrops[i].x, fx_raindrops[i].y,
		   fx_raindrop_size, fx_raindrop_size, False);
	if (fx_raindrops[i].buf)
	   EDestroyPixImg(fx_raindrops[i].buf);
	fx_raindrops[i].buf = NULL;
     }
   if (fx_raindrops_draw)
      EDestroyPixImg(fx_raindrops_draw);
   fx_raindrops_draw = NULL;
   fx_raindrops_win = None;
}

static void
FX_Raindrops_Pause(void)
{
   static char         paused = 0;

   if (!paused)
     {
	FX_Raindrops_Quit();
	paused = 1;
     }
   else
     {
	FX_Raindrops_Init(NULL);
	paused = 0;
     }
}

#endif /* E_FX_RAINDROPS */

/****************************** WAVES ***************************************/
/* by tsade :)                                                              */
/****************************************************************************/

#define FX_WAVE_WATERH 64
#define FX_WAVE_WATERW 64
#define FX_WAVE_DEPTH  10
#define FX_WAVE_GRABH  (FX_WAVE_WATERH + FX_WAVE_DEPTH)
#define FX_WAVE_CROSSPERIOD 0.42
static Pixmap       fx_wave_above = None;
static Win          fx_wave_win = NoWin;
static int          fx_wave_count = 0;

static void
FX_Wave_info(void)
{
#if ENABLE_FX_INFO
   static char         before = 0;

   if (!before)
      DialogOK(_("Starting up Waves FX..."),
	       _("\n" "You have just started the Waves Effect.\n" "\n"
		 "If you look closely on your desktop background, and if it\n"
		 "doesn't have a solid colour (i.e. has a background texture or\n"
		 "image), you will see a pool of water at the bottom of your\n"
		 "screen that reflects everything above it and \"waves\".\n"
		 "\n"
		 "To disable this effect just select this option again to toggle\n"
		 "it off.\n"));
   before = 1;
#endif
}

static void
FX_Wave_timeout(int val __UNUSED__, void *data __UNUSED__)
{
   /* Variables */
   static double       incv = 0, inch = 0;
   static double       incx = 0;
   double              incx2;
   static GC           gc1 = 0, gc = 0;
   int                 y;

   /* Check to see if we need to create stuff */
   if (!fx_wave_above)
     {
	XGCValues           gcv;

	fx_wave_win = DeskGetBackgroundWin(DesksGetCurrent());

	fx_wave_above =
	   ECreatePixmap(fx_wave_win, VRoot.w, FX_WAVE_WATERH * 2, 0);
	if (gc)
	   EXFreeGC(gc);
	if (gc1)
	   EXFreeGC(gc1);
	gcv.subwindow_mode = IncludeInferiors;
	gc = EXCreateGC(Xwin(fx_wave_win), GCSubwindowMode, &gcv);
	gc1 = EXCreateGC(Xwin(fx_wave_win), 0L, &gcv);

	FX_Wave_info();
     }

   /* On the zero, grab the desktop again. */
   if (fx_wave_count == 0)
     {
	XCopyArea(disp, EoGetXwin(DesksGetCurrent()), fx_wave_above, gc, 0,
		  VRoot.h - (FX_WAVE_WATERH * 3), VRoot.w, FX_WAVE_WATERH * 2,
		  0, 0);
     }

   /* Increment and roll the counter */
   fx_wave_count++;
   if (fx_wave_count > 32)
      fx_wave_count = 0;

   /* Increment and roll some other variables */
   incv += 0.40;
   if (incv > (M_PI_2 * 4))
      incv = 0;

   inch += 0.32;
   if (inch > (M_PI_2 * 4))
      inch = 0;

   incx += 0.32;
   if (incx > (M_PI_2 * 4))
      incx = 0;

   /* Copy the area to correct bugs */
   if (fx_wave_count == 0)
     {
	XCopyArea(disp, fx_wave_above, Xwin(fx_wave_win), gc1, 0,
		  VRoot.h - FX_WAVE_GRABH, VRoot.w, FX_WAVE_DEPTH * 2, 0,
		  VRoot.h - FX_WAVE_GRABH);
     }

   /* Go through the bottom couple (FX_WAVE_WATERH) lines of the window */
   for (y = 0; y < FX_WAVE_WATERH; y++)
     {
	/* Variables */
	double              aa, a, p;
	int                 yoff, off, yy;
	int                 x;

	/* Figure out the side-to-side movement */
	p = (((double)(FX_WAVE_WATERH - y)) / ((double)FX_WAVE_WATERH));
	a = p * p * 48 + incv;
	yoff = y + (int)(sin(a) * 7) + 1;
	yy = (FX_WAVE_WATERH * 2) - yoff;
	aa = p * p * FX_WAVE_WATERH + inch;
	off = (int)(sin(aa) * 10 * (1 - p));

	/* Set up the next part */
	incx2 = incx;

	/* Go through the width of the screen, in block sizes */
	for (x = 0; x < VRoot.w; x += FX_WAVE_WATERW)
	  {
	     /* Variables */
	     int                 sx;

	     /* Add something to incx2 and roll it */
	     incx2 += FX_WAVE_CROSSPERIOD;

	     if (incx2 > (M_PI_2 * 4))
		incx2 = 0;

	     /* Figure it out */
	     sx = (int)(sin(incx2) * FX_WAVE_DEPTH);

	     /* Display this block */
	     XCopyArea(disp, fx_wave_above, Xwin(fx_wave_win), gc1, x, yy,	/* x, y */
		       FX_WAVE_WATERW, 1,	/* w, h */
		       off + x, VRoot.h - FX_WAVE_WATERH + y + sx	/* dx, dy */
		);
	  }
     }

   /* Make noise */
   DoIn("FX_WAVE_TIMEOUT", 0.066, FX_Wave_timeout, 0, NULL);
}

static void
FX_Waves_Init(const char *name __UNUSED__)
{
   fx_wave_count = 0;
   DoIn("FX_WAVE_TIMEOUT", 0.066, FX_Wave_timeout, 0, NULL);
}

static void
FX_Waves_Desk(void)
{
   EFreePixmap(fx_wave_above);
   fx_wave_count = 0;
   fx_wave_above = 0;
}

static void
FX_Waves_Quit(void)
{
   RemoveTimerEvent("FX_WAVE_TIMEOUT");
   EClearArea(fx_wave_win, 0, VRoot.h - FX_WAVE_WATERH, VRoot.w,
	      FX_WAVE_WATERH, False);
}

static void
FX_Waves_Pause(void)
{
   static char         paused = 0;

   if (!paused)
     {
	FX_Waves_Quit();
	paused = 1;
     }
   else
     {
	FX_Waves_Init(NULL);
	paused = 0;
     }
}

#ifdef E_FX_IMAGESPINNER

/****************************** IMAGESPINNER ********************************/

static Window       fx_imagespinner_win = None;
static int          fx_imagespinner_count = 3;
static char        *fx_imagespinner_params = NULL;

static void
FX_imagespinner_info(void)
{
#if ENABLE_FX_INFO
   static char         before = 0;

   if (!before)
      DialogOK(_("Starting up imagespinners FX..."),
	       _("\n" "You have just started the imagespinners Effect.\n"
		 "\n"
		 "To disable this effect just select this option again to toggle\n"
		 "it off.\n"));
   before = 1;
#endif
}

static void
FX_imagespinner_timeout(int val __UNUSED__, void *data __UNUSED__)
{
   char               *string = NULL;

   if (fx_imagespinner_win == None)
     {
	fx_imagespinner_win = DeskGetBackgroundWin(DesksGetCurrent());
	FX_imagespinner_info();
     }

#if 0				/* Don't use getword */
/* do stuff here */
   string = getword(fx_imagespinner_params, fx_imagespinner_count);
   if (!string)
     {
	fx_imagespinner_count = 3;
	string = getword(fx_imagespinner_params, fx_imagespinner_count);
     }
#endif

   fx_imagespinner_count++;
   if (string)
     {
	EImage             *im;

	im = EImageLoad(string);
	if (im)
	  {
	     int                 x, y, w, h;

	     EImageGetSize(im, &w, &h);
	     sscanf(fx_imagespinner_params, "%*s %i %i ", &x, &y);
	     x = ((VRoot.w * x) >> 10) - ((w * x) >> 10);
	     y = ((VRoot.h * y) >> 10) - ((h * y) >> 10);
	     EImageRenderOnDrawable(im, fx_imagespinner_win, x, y, w, h, 0);
	     EImageFree(im);
	  }
	Efree(string);
     }

   DoIn("FX_IMAGESPINNER_TIMEOUT", 0.066, FX_imagespinner_timeout, 0, NULL);
}

static void
FX_ImageSpinner_Init(const char *name)
{
   fx_imagespinner_count = 3;
   DoIn("FX_IMAGESPINNER_TIMEOUT", 0.066, FX_imagespinner_timeout, 0, NULL);
   fx_imagespinner_params = Estrdup(name);
}

static void
FX_ImageSpinner_Desk(void)
{
   fx_imagespinner_win = DeskGetBackgroundWin(DesksGetCurrent());
}

static void
FX_ImageSpinner_Quit(void)
{
   RemoveTimerEvent("FX_IMAGESPINNER_TIMEOUT");
   XClearArea(disp, fx_imagespinner_win, 0, 0, VRoot.w, VRoot.h, False);
   if (fx_imagespinner_params)
      Efree(fx_imagespinner_params);
   fx_imagespinner_params = NULL;
   fx_imagespinner_win = None;
}

static void
FX_ImageSpinner_Pause(void)
{
   static char         paused = 0;

   if (!paused)
     {
	FX_ImageSpinner_Quit();
	paused = 1;
     }
   else
     {
	FX_ImageSpinner_Init(NULL);
	paused = 0;
     }
}

#endif /* E_FX_IMAGESPINNER */

/****************************************************************************/

static FXHandler    fx_handlers[] = {
   {"ripples",
    FX_Ripple_Init, FX_Ripple_Desk, FX_Ripple_Quit, FX_Ripple_Pause,
    0, 0},
   {"waves",
    FX_Waves_Init, FX_Waves_Desk, FX_Waves_Quit, FX_Waves_Pause,
    0, 0},
#ifdef E_FX_RAINDROPS		/* FIXME */
   {"raindrops",
    FX_Raindrops_Init, FX_Raindrops_Desk, FX_Raindrops_Quit,
    FX_Raindrops_Pause,
    0, 0},
#endif
#ifdef E_FX_IMAGESPINNER
   {"imagespinner",
    FX_ImageSpinner_Init, FX_ImageSpinner_Desk, FX_ImageSpinner_Quit,
    FX_ImageSpinner_Pause,
    0, 0},
#endif
};
#define N_FX_HANDLERS (sizeof(fx_handlers)/sizeof(FXHandler))

/****************************** Effect handlers *****************************/

static FXHandler   *
FX_Find(const char *name)
{
   unsigned int        i;

   for (i = 0; i < N_FX_HANDLERS; i++)
      if (!strcmp(fx_handlers[i].name, name))
	 return &fx_handlers[i];

   return NULL;
}

static void
FX_Op(const char *name, int fx_op)
{
   FXHandler          *fxh;

   fxh = FX_Find(name);
   if (fxh == NULL)
      return;

   switch (fx_op)
     {
     case FX_OP_START:
	if (fxh->enabled)
	   break;
      do_start:
	if (fxh->init_func)
	   fxh->init_func(name);
	fxh->enabled = 1;
	break;

     case FX_OP_STOP:
	if (!fxh->enabled)
	   break;
      do_stop:
	if (fxh->quit_func)
	   fxh->quit_func();
	fxh->enabled = 0;
	break;

     case FX_OP_TOGGLE:
	if (fxh->enabled)
	   goto do_stop;
	else
	   goto do_start;
     }
}

static void
FX_DeskChange(void)
{
   unsigned int        i;

   for (i = 0; i < N_FX_HANDLERS; i++)
     {
	if (fx_handlers[i].enabled)
	  {
	     if (fx_handlers[i].desk_func)
		fx_handlers[i].desk_func();
	  }
     }
}

static void
FX_Pause(void)
{
   unsigned int        i;

   for (i = 0; i < N_FX_HANDLERS; i++)
     {
	if (fx_handlers[i].enabled)
	  {
	     if (fx_handlers[i].paused)
	       {
		  if (fx_handlers[i].pause_func)
		     fx_handlers[i].pause_func();
		  fx_handlers[i].paused = 1;
	       }
	     else
	       {
		  if (fx_handlers[i].pause_func)
		     fx_handlers[i].pause_func();
		  fx_handlers[i].paused = 0;
	       }
	  }
     }
}

static void
FX_StartAll(void)
{
   unsigned int        i;
   FXHandler          *fxh;

   for (i = 0; i < N_FX_HANDLERS; i++)
     {
	fxh = &fx_handlers[i];
	if (fxh->enabled && fxh->init_func)
	   fxh->init_func(fxh->name);
     }
}

static int
FX_IsOn(const char *effect)
{
   unsigned int        i;

   for (i = 0; i < N_FX_HANDLERS; i++)
     {
	if (!strcmp(fx_handlers[i].name, effect))
	  {
	     return fx_handlers[i].enabled;
	  }
     }
   return 0;
}

/****************************************************************************/

/*
 * Fx Module
 */

static void
FxSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_START:
	FX_StartAll();
	break;
     case ESIGNAL_AREA_SWITCH_START:
     case ESIGNAL_DESK_SWITCH_START:
	break;
     case ESIGNAL_AREA_SWITCH_DONE:
     case ESIGNAL_DESK_SWITCH_DONE:
	FX_DeskChange();
	break;
     case ESIGNAL_ANIMATION_SUSPEND:
     case ESIGNAL_ANIMATION_RESUME:
	FX_Pause();
	break;
     }
}

static char         tmp_effect_raindrops;
static char         tmp_effect_ripples;
static char         tmp_effect_waves;

static void
CB_ConfigureFX(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	FX_Op("raindrops", tmp_effect_raindrops ? FX_OP_START : FX_OP_STOP);
	FX_Op("ripples", tmp_effect_ripples ? FX_OP_START : FX_OP_STOP);
	FX_Op("waves", tmp_effect_waves ? FX_OP_START : FX_OP_STOP);
     }
   autosave();
}

static void
_DlgFillFx(Dialog * d, DItem * table, void *data __UNUSED__)
{
   DItem              *di;

   tmp_effect_raindrops = FX_IsOn("raindrops");
   tmp_effect_ripples = FX_IsOn("ripples");
   tmp_effect_waves = FX_IsOn("waves");

   DialogItemTableSetOptions(table, 1, 0, 0, 0);

   if (Conf.dialogs.headers)
      DialogAddHeader(d, "pix/fx.png",
		      _("Enlightenment Special Effects\n" "Settings Dialog\n"));

   /* Effects */
   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetText(di, _("Effects"));
#if 0				/* Disabled */
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetText(di, _("Enable Effect: Raindrops"));
   DialogItemCheckButtonSetPtr(di, &tmp_effect_raindrops);
#endif
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetText(di, _("Ripples"));
   DialogItemCheckButtonSetPtr(di, &tmp_effect_ripples);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetText(di, _("Waves"));
   DialogItemCheckButtonSetPtr(di, &tmp_effect_waves);

   DialogAddFooter(d, DLG_OAC, CB_ConfigureFX);
}

const DialogDef     DlgFx = {
   "CONFIGURE_FX",
   N_("FX"),
   N_("Special FX Settings"),
   "SOUND_SETTINGS_FX",
   _DlgFillFx
};

static void
FxIpc(const char *params, Client * c __UNUSED__)
{
   char                word1[FILEPATH_LEN_MAX];
   char                word2[FILEPATH_LEN_MAX];

   if (!params)
      return;

   word1[0] = '\0';
   word2[0] = '\0';

   word(params, 1, word1);

   if (!strcmp(word1, "raindrops") || !strcmp(word1, "ripples") ||
       !strcmp(word1, "waves"))
     {
	word(params, 2, word2);
	if (!strcmp(word2, ""))
	   FX_Op(word1, FX_OP_TOGGLE);
	else if (!strcmp(word2, "on"))
	   FX_Op(word1, FX_OP_START);
	else if (!strcmp(word2, "off"))
	   FX_Op(word1, FX_OP_STOP);
	else if (!strcmp(word2, "?"))
	   IpcPrintf("%s: %s", word1, FX_IsOn(word1) ? "on" : "off");
	else
	   IpcPrintf("Error: unknown mode specified");
     }
   else if (!strncmp(word1, "cfg", 2))
     {
	DialogShowSimple(&DlgFx, NULL);
     }
}

static const IpcItem FxIpcArray[] = {
   {
    FxIpc,
    "fx", "fx",
    "Toggle various effects on/off",
    "  fx cfg               Configure effects\n"
    "  fx <effect> <mode>   Set the mode of a particular effect\n"
    "  fx <effect> ?\"      Get the current mode\n"
    "the following effects are available\n"
    " ripples <on/off> (ripples that act as a water effect on the screen)\n"
    " waves <on/off> (waves that act as a water effect on the screen)\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(FxIpcArray)/sizeof(IpcItem))

static const CfgItem FxCfgItems[] = {
   CFR_ITEM_BOOL(fx_handlers[0].enabled, ripples.enabled, 0),
   CFR_ITEM_BOOL(fx_handlers[1].enabled, waves.enabled, 0),
#ifdef E_FX_RAINDROPS		/* FIXME */
   CFR_ITEM_BOOL(fx_handlers[2].enabled, raindrops.enabled, 0),
#endif
};
#define N_CFG_ITEMS (sizeof(FxCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
const EModule       ModEffects = {
   "effects", "efx",
   FxSighan,
   {N_IPC_FUNCS, FxIpcArray},
   {N_CFG_ITEMS, FxCfgItems}
};
