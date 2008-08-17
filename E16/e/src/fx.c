/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "ecompmgr.h"
#include "eimage.h"
#include "emodule.h"
#include "settings.h"
#include "timers.h"
#include "xwin.h"
#include <math.h>

#define E_FX_RAINDROPS          0
#define E_FX_IMAGESPINNER       0

/* FIXME - Needs cleaning up */

#ifndef M_PI_2
#define M_PI_2 (3.141592654 / 2)
#endif

#define FX_OP_START  1
#define FX_OP_STOP   2
#define FX_OP_TOGGLE 3

typedef struct {
   const char         *name;
   void                (*init_func) (const char *name);
   void                (*desk_func) (void);
   void                (*quit_func) (void);
   void                (*pause_func) (void);
   char                enabled;
   char                paused;
} FXHandler;

#if USE_COMPOSITE
/* As of composite 0.4 we need to set the clip region */
#define SET_GC_CLIP(eo, gc) ECompMgrWinClipToGC(eo, gc)
#else
#define SET_GC_CLIP(eo, gc)
#endif

/****************************** RIPPLES *************************************/

#define fx_ripple_waterh 64
static Pixmap       fx_ripple_above = None;
static Win          fx_ripple_win = NULL;
static int          fx_ripple_count = 0;
static Timer       *fx_ripple_timer = NULL;

static int
FX_ripple_timeout(void *data __UNUSED__)
{
   static double       incv = 0, inch = 0;
   static GC           gc1 = 0, gc = 0;
   int                 y;
   EObj               *bgeo;

   bgeo = DeskGetBackgroundObj(DesksGetCurrent());

   if (fx_ripple_above == None)
     {
	XGCValues           gcv;

	fx_ripple_win = EobjGetWin(bgeo);

	fx_ripple_above =
	   ECreatePixmap(fx_ripple_win, WinGetW(VROOT),
			 fx_ripple_waterh * 2, 0);
	if (gc)
	   EXFreeGC(gc);
	if (gc1)
	   EXFreeGC(gc1);
	gcv.subwindow_mode = IncludeInferiors;
	gc = EXCreateGC(WinGetXwin(fx_ripple_win), GCSubwindowMode, &gcv);
	gc1 = EXCreateGC(WinGetXwin(fx_ripple_win), 0L, &gcv);
     }

   if (fx_ripple_count == 0)
      XCopyArea(disp, WinGetXwin(fx_ripple_win), fx_ripple_above, gc, 0,
		WinGetH(VROOT) - (fx_ripple_waterh * 3), WinGetW(VROOT),
		fx_ripple_waterh * 2, 0, 0);

   fx_ripple_count++;
   if (fx_ripple_count > 32)
      fx_ripple_count = 0;

   incv += 0.40;
   if (incv > (M_PI_2 * 4))
      incv = 0;
   inch += 0.32;
   if (inch > (M_PI_2 * 4))
      inch = 0;

   SET_GC_CLIP(bgeo, gc1);

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
	XCopyArea(disp, fx_ripple_above, WinGetXwin(fx_ripple_win), gc1, 0, yy,
		  WinGetW(VROOT), 1, off,
		  WinGetH(VROOT) - fx_ripple_waterh + y);
     }

   return 1;
}

static void
FX_Ripple_Init(const char *name __UNUSED__)
{
   fx_ripple_count = 0;
   TIMER_ADD(fx_ripple_timer, 0.066, FX_ripple_timeout, NULL);
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
   TIMER_DEL(fx_ripple_timer);
   EClearArea(fx_ripple_win, 0, WinGetH(VROOT) - fx_ripple_waterh,
	      WinGetW(VROOT), fx_ripple_waterh);
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

#if E_FX_RAINDROPS
#include "piximg.h"

/****************************** RAIN DROPS **********************************/

#define fx_raindrop_size 96
#define fx_raindrop_size2 (fx_raindrop_size / 2)
#define fx_raindrop_duration 32
#define fx_frequency 4
#define fx_amplitude 48
static Win          fx_raindrops_win = NULL;
static int          fx_raindrops_number = 4;
static PixImg      *fx_raindrops_draw = NULL;
static Timer       *fx_raindrops_timer = NULL;

typedef struct {
   int                 x, y;
   int                 count;
   PixImg             *buf;
} DropContext;

static DropContext  fx_raindrops[4];

static int
FX_raindrops_timeout(void *data __UNUSED__)
{
   static GC           gc1 = 0, gc = 0;
   int                 i, x, y, xx, yy;
   int                 percent_done;
   static char         first = 1;
   static char         sintab[256];
   static unsigned char disttab[fx_raindrop_size][fx_raindrop_size];
   EObj               *bgeo;

   bgeo = DeskGetBackgroundObj(DesksGetCurrent());

   if (fx_raindrops_win == None)
     {
	XGCValues           gcv;

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

	fx_raindrops_win = EobjGetWin(bgeo);

	if (gc)
	   EXFreeGC(gc);
	if (gc1)
	   EXFreeGC(gc1);

	gcv.subwindow_mode = IncludeInferiors;
	gc = EXCreateGC(WinGetXwin(fx_raindrops_win), GCSubwindowMode, &gcv);
	gc1 = EXCreateGC(WinGetXwin(fx_raindrops_win), 0L, &gcv);

	fx_raindrops_draw =
	   ECreatePixImg(WinGetXwin(fx_raindrops_win), fx_raindrop_size,
			 fx_raindrop_size);
	if (!fx_raindrops_draw)
	   return 0;

	for (i = 0; i < fx_raindrops_number; i++)
	  {
	     fx_raindrops[i].buf =
		ECreatePixImg(WinGetXwin(fx_raindrops_win), fx_raindrop_size,
			      fx_raindrop_size);
	     if (fx_raindrops[i].buf)
		XShmGetImage(disp, WinGetXwin(fx_raindrops_win),
			     fx_raindrops[i].buf->xim, fx_raindrops[i].x,
			     fx_raindrops[i].y, 0xffffffff);
	     if (!fx_raindrops[i].buf)
		return 0;
	  }
     }

   SET_GC_CLIP(bgeo, gc1);

   for (i = 0; i < fx_raindrops_number; i++)
     {
	fx_raindrops[i].count++;
	if (fx_raindrops[i].count == fx_raindrop_duration)
	  {
	     int                 j, count = 0;
	     char                intersect = 1;

	     EClearArea(fx_raindrops_win, fx_raindrops[i].x,
			fx_raindrops[i].y, fx_raindrop_size, fx_raindrop_size);
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
			  rand() % (WinGetW(VROOT) - fx_raindrop_size);
		       fx_raindrops[i].y =
			  rand() % (WinGetH(VROOT) - fx_raindrop_size);
		       if (fx_raindrops[i].x < 0)
			  fx_raindrops[i].x = 0;
		       else if (fx_raindrops[i].x >
				(WinGetW(VROOT) - fx_raindrop_size))
			  fx_raindrops[i].x = WinGetW(VROOT) - fx_raindrop_size;
		       if (fx_raindrops[i].y < 0)
			  fx_raindrops[i].y = 0;
		       else if (fx_raindrops[i].y >
				(WinGetH(VROOT) - fx_raindrop_size))
			  fx_raindrops[i].y = WinGetH(VROOT) - fx_raindrop_size;
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
	     XShmGetImage(disp, WinGetXwin(fx_raindrops_win),
			  fx_raindrops[i].buf->xim,
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
	XShmPutImage(disp, WinGetXwin(fx_raindrops_win), gc1,
		     fx_raindrops_draw->xim, 0, 0,
		     fx_raindrops[i].x, fx_raindrops[i].y, fx_raindrop_size,
		     fx_raindrop_size, False);
	ESync(0);
     }

   return 1;
}

static void
FX_Raindrops_Init(const char *name __UNUSED__)
{
   int                 i;

   fx_raindrops_win = None;
   for (i = 0; i < fx_raindrops_number; i++)
     {
	fx_raindrops[i].count = rand() % fx_raindrop_duration;
	fx_raindrops[i].x = rand() % (WinGetW(VROOT) - fx_raindrop_size);
	fx_raindrops[i].y = rand() % (WinGetH(VROOT) - fx_raindrop_size);
     }
   TIMER_ADD(fx_raindrops_timer, 0.066, FX_raindrops_timeout, NULL);
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

   TIMER_DEL(fx_raindrops_timer);
   for (i = 0; i < fx_raindrops_number; i++)
     {
	EClearArea(fx_raindrops_win, fx_raindrops[i].x, fx_raindrops[i].y,
		   fx_raindrop_size, fx_raindrop_size);
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
static Win          fx_wave_win = NULL;
static int          fx_wave_count = 0;
static Timer       *fx_wave_timer = NULL;

static int
FX_Wave_timeout(void *data __UNUSED__)
{
   /* Variables */
   static double       incv = 0, inch = 0;
   static double       incx = 0;
   double              incx2;
   static GC           gc1 = 0, gc = 0;
   int                 y;
   EObj               *bgeo;

   bgeo = DeskGetBackgroundObj(DesksGetCurrent());

   /* Check to see if we need to create stuff */
   if (!fx_wave_above)
     {
	XGCValues           gcv;

	fx_wave_win = EobjGetWin(bgeo);

	fx_wave_above =
	   ECreatePixmap(fx_wave_win, WinGetW(VROOT), FX_WAVE_WATERH * 2, 0);
	if (gc)
	   EXFreeGC(gc);
	if (gc1)
	   EXFreeGC(gc1);
	gcv.subwindow_mode = IncludeInferiors;
	gc = EXCreateGC(WinGetXwin(fx_wave_win), GCSubwindowMode, &gcv);
	gc1 = EXCreateGC(WinGetXwin(fx_wave_win), 0L, &gcv);
     }

   /* On the zero, grab the desktop again. */
   if (fx_wave_count == 0)
     {
	XCopyArea(disp, WinGetXwin(fx_wave_win), fx_wave_above, gc, 0,
		  WinGetH(VROOT) - (FX_WAVE_WATERH * 3), WinGetW(VROOT),
		  FX_WAVE_WATERH * 2, 0, 0);
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

   SET_GC_CLIP(bgeo, gc1);

   /* Copy the area to correct bugs */
   if (fx_wave_count == 0)
     {
	XCopyArea(disp, fx_wave_above, WinGetXwin(fx_wave_win), gc1, 0,
		  WinGetH(VROOT) - FX_WAVE_GRABH, WinGetW(VROOT),
		  FX_WAVE_DEPTH * 2, 0, WinGetH(VROOT) - FX_WAVE_GRABH);
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
	for (x = 0; x < WinGetW(VROOT); x += FX_WAVE_WATERW)
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
	     XCopyArea(disp, fx_wave_above, WinGetXwin(fx_wave_win), gc1, x, yy,	/* x, y */
		       FX_WAVE_WATERW, 1,	/* w, h */
		       off + x, WinGetH(VROOT) - FX_WAVE_WATERH + y + sx	/* dx, dy */
		);
	  }
     }

   return 1;
}

static void
FX_Waves_Init(const char *name __UNUSED__)
{
   fx_wave_count = 0;
   TIMER_ADD(fx_wave_timer, 0.066, FX_Wave_timeout, NULL);
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
   TIMER_DEL(fx_wave_timer);
   EClearArea(fx_wave_win, 0, WinGetH(VROOT) - FX_WAVE_WATERH,
	      WinGetW(VROOT), FX_WAVE_WATERH);
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

#if E_FX_IMAGESPINNER

/****************************** IMAGESPINNER ********************************/

static Win          fx_imagespinner_win = NULL;
static int          fx_imagespinner_count = 3;
static char        *fx_imagespinner_params = NULL;
static Timer       *fx_imagespinner_timer = NULL;

static int
FX_imagespinner_timeout(void *data __UNUSED__)
{
   char               *string = NULL;
   EObj               *bgeo;

   if (fx_imagespinner_win == None)
     {
	bgeo = DeskGetBackgroundObj(DesksGetCurrent());
	fx_imagespinner_win = EobjGetWin(bgeo);
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

	im = ThemeImageLoad(string);
	if (im)
	  {
	     int                 x, y, w, h;

	     EImageGetSize(im, &w, &h);
	     sscanf(fx_imagespinner_params, "%*s %i %i ", &x, &y);
	     x = ((WinGetW(VROOT) * x) >> 10) - ((w * x) >> 10);
	     y = ((WinGetH(VROOT) * y) >> 10) - ((h * y) >> 10);
	     EImageRenderOnDrawable(im, fx_imagespinner_win, None, 0,
				    x, y, w, h);
	     EImageFree(im);
	  }
	Efree(string);
     }

   return 1;
}

static void
FX_ImageSpinner_Init(const char *name)
{
   fx_imagespinner_count = 3;
   TIMER_ADD(fx_imagespinner_timer, 0.066, FX_imagespinner_timeout, NULL);
   fx_imagespinner_params = Estrdup(name);
}

static void
FX_ImageSpinner_Desk(void)
{
   EObj               *bgeo;

   bgeo = DeskGetBackgroundObj(DesksGetCurrent());
   fx_imagespinner_win = EobjGetWin(bgeo);
}

static void
FX_ImageSpinner_Quit(void)
{
   TIMER_DEL(fx_imagespinner_timer);
   EClearArea(fx_imagespinner_win, 0, 0, WinGetW(VROOT), WinGetH(VROOT));
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
#if E_FX_RAINDROPS		/* FIXME */
   {"raindrops",
    FX_Raindrops_Init, FX_Raindrops_Desk, FX_Raindrops_Quit,
    FX_Raindrops_Pause,
    0, 0},
#endif
#if E_FX_IMAGESPINNER
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
_DlgFillFx(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   DItem              *di;

   tmp_effect_raindrops = FX_IsOn("raindrops");
   tmp_effect_ripples = FX_IsOn("ripples");
   tmp_effect_waves = FX_IsOn("waves");

   DialogItemTableSetOptions(table, 1, 0, 0, 0);

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
}

const DialogDef     DlgFx = {
   "CONFIGURE_FX",
   N_("FX"),
   N_("Special FX Settings"),
   "SOUND_SETTINGS_FX",
   "pix/fx.png",
   N_("Enlightenment Special Effects\n" "Settings Dialog\n"),
   _DlgFillFx,
   DLG_OAC, CB_ConfigureFX,
};

static void
FxIpc(const char *params)
{
   char                word1[1024];
   char                word2[1024];

   if (!params)
      return;

   word1[0] = word2[0] = '\0';
   sscanf(params, "%1000s %1000s", word1, word2);

   if (!strcmp(word1, "raindrops") || !strcmp(word1, "ripples") ||
       !strcmp(word1, "waves"))
     {
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
#if E_FX_RAINDROPS		/* FIXME */
   CFR_ITEM_BOOL(fx_handlers[2].enabled, raindrops.enabled, 0),
#endif
};
#define N_CFG_ITEMS (sizeof(FxCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
extern const EModule ModEffects;
const EModule       ModEffects = {
   "effects", "efx",
   FxSighan,
   {N_IPC_FUNCS, FxIpcArray},
   {N_CFG_ITEMS, FxCfgItems}
};
