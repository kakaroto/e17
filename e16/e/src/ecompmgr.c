/*
 * Copyright (C) 2004-2005 Kim Woelders
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

/* >>> Original copyright notice begin <<< */
/*
 * $Id: xcompmgr.c,v 1.26 2004/08/14 21:39:51 keithp Exp $
 *
 * Copyright © 2003 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
/*
 * Modified by Matthew Hawn. I don't know what to say here so follow what it 
 * says above. Not that I can really do anything about it
 */
/* >>> Original copyright notice end <<< */

/*
 * This code is derived from xcompmgr.c, see original copyright notice above.
 * Initially adapted for E16 by Kim Woelders.
 */
#include "E.h"
#if USE_COMPOSITE
#include "ecompmgr.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xrender.h>

#if COMPOSITE_MAJOR > 0 || COMPOSITE_MINOR >= 2
#define HAS_NAME_WINDOW_PIXMAP 1
#endif

#define CAN_DO_USABLE 0

#define ENABLE_SHADOWS 1
#if HAS_NAME_WINDOW_PIXMAP
#define ENABLE_FADING  0
#else
#define ENABLE_FADING  0
#endif

#define ENABLE_DEBUG   1
#if ENABLE_DEBUG
#define EDBUG_TYPE_COMPMGR  161
#define EDBUG_TYPE_COMPMGR2 162
#define EDBUG_TYPE_COMPMGR3 163
#define D1printf(fmt...) if(EventDebug(EDBUG_TYPE_COMPMGR))Eprintf(fmt)
#define D2printf(fmt...) if(EventDebug(EDBUG_TYPE_COMPMGR2))Eprintf(fmt)
#else
#define D1printf(fmt...)
#define D2printf(fmt...)
#endif /* ENABLE_DEBUG */

#define INV_POS     0x01
#define INV_SIZE    0x02
#define INV_OPACITY 0x04
#define INV_SHADOW  0x08
#define INV_GEOM    (INV_POS | INV_SIZE)
#define INV_ALL     (INV_POS | INV_SIZE | INV_OPACITY | INV_SHADOW)

typedef struct _ecmwininfo ECmWinInfo;	/* TBD */

struct _ecmwininfo
{
   EObj               *next;	/* Paint order */
   EObj               *prev;	/* Paint order */
#if HAS_NAME_WINDOW_PIXMAP
   Pixmap              pixmap;
#endif
   XWindowAttributes   a;
   int                 rcx, rcy, rcw, rch;
#if CAN_DO_USABLE
   Bool                usable;	/* mapped and all damaged at one point */
   XRectangle          damage_bounds;	/* bounds of damage */
#endif
   int                 mode;
   char                visible;
   char                damaged;
   Damage              damage;
   Picture             picture;
   Picture             alphaPict;
   XserverRegion       borderSize;
   XserverRegion       extents;
   XserverRegion       clip;
#if ENABLE_SHADOWS
   Picture             shadowPict;
   Picture             shadow;
   int                 shadow_dx;
   int                 shadow_dy;
   int                 shadow_width;
   int                 shadow_height;
#endif
   unsigned int        opacity;

   unsigned long       damage_sequence;	/* sequence when damage was created */
};

#if ENABLE_SHADOWS
#define ECM_SHADOWS_OFF      0
#define ECM_SHADOWS_SHARP    1	/* use window alpha for shadow; sharp, but precise */
#define ECM_SHADOWS_BLURRED  2	/* use window extents for shadow, blurred */
#endif

static struct
{
   char                enable;
   char                resize_fix_enable;
   char                use_name_pixmap;
   int                 mode;
   int                 shadow;
   int                 shadow_radius;
} Conf_compmgr;

#define ECM_MODE_OFF    0
#define ECM_MODE_ROOT   1
#define ECM_MODE_WINDOW 2
#define ECM_MODE_AUTO   3

static struct
{
   char               *args;
   char                active;
#if HAS_NAME_WINDOW_PIXMAP
   char                use_pixmap;
#endif
   EObj               *eo_first;
   EObj               *eo_last;
} Mode_compmgr;

static Picture      rootPicture;
static Picture      rootBuffer;

static XserverRegion allDamage;

#define OPAQUE          0xffffffff

#define WINDOW_SOLID    0
#define WINDOW_TRANS    1
#define WINDOW_ARGB     2

static void         ECompMgrDamageAll(void);
static void         ECompMgrHandleRootEvent(XEvent * ev, void *prm);
static void         ECompMgrHandleWindowEvent(XEvent * ev, void *prm);

/*
 * Regions
 */

static              XserverRegion
ERegionCreate(int x, int y, int w, int h)
{
   XserverRegion       rgn;
   XRectangle          rct;

   rct.x = x;
   rct.y = y;
   rct.width = w;
   rct.height = h;
   rgn = XFixesCreateRegion(disp, &rct, 1);

   return rgn;
}

static void
ERegionShow(const char *txt, XserverRegion rgn)
{
   int                 i, nr;
   XRectangle         *pr;

   pr = XFixesFetchRegion(disp, rgn, &nr);
   if (!pr)
      return;

   Eprintf("ERegionShow: %s %#lx:\n", txt, rgn);
   for (i = 0; i < nr; i++)
      Eprintf("%4d: %4d+%4d %4dx%4d\n", i, pr[i].x, pr[i].y, pr[i].width,
	      pr[i].height);

   XFree(pr);
}

/*
 * Pictures
 */

static              Picture
EPictureCreateSolid(Bool argb, double a, double r, double g, double b)
{
   Display            *dpy = disp;
   Pixmap              pmap;
   Picture             pict;
   XRenderPictFormat  *pictfmt;
   XRenderPictureAttributes pa;
   XRenderColor        c;

   pmap = XCreatePixmap(dpy, VRoot.win, 1, 1, argb ? 32 : 8);
   pictfmt = XRenderFindStandardFormat(dpy,
				       argb ? PictStandardARGB32 :
				       PictStandardA8);
   pa.repeat = True;
   pict = XRenderCreatePicture(dpy, pmap, pictfmt, CPRepeat, &pa);

   c.alpha = a * 0xffff;
   c.red = r * 0xffff;
   c.green = g * 0xffff;
   c.blue = b * 0xffff;
   XRenderFillRectangle(dpy, PictOpSrc, pict, &c, 0, 0, 1, 1);

   XFreePixmap(dpy, pmap);

   return pict;
}

static              Picture
EPictureCreateBuffer(Window win, int w, int h, int depth, Visual * vis)
{
   Picture             pict;
   Pixmap              pmap;
   XRenderPictFormat  *pictfmt;

   pmap = XCreatePixmap(disp, win, w, h, depth);
   pictfmt = XRenderFindVisualFormat(disp, vis);
   pict = XRenderCreatePicture(disp, pmap, pictfmt, 0, 0);
   XFreePixmap(disp, pmap);

   return pict;
}

/* Hack to fix redirected window resize bug(?) */
void
ECompMgrMoveResizeFix(EObj * eo, int x, int y, int w, int h)
{
   Picture             pict;
   int                 wo, ho;
   ECmWinInfo         *c = eo->cmhook;

   if (!c || !Conf_compmgr.resize_fix_enable)
     {
	EMoveResizeWindow(eo->win, x, y, w, h);
	return;
     }

   wo = ho = 0;
   EGetGeometry(eo->win, NULL, NULL, NULL, &wo, &ho, NULL, NULL);
   if (wo <= 0 || ho <= 0 || (wo == w && ho == h))
     {
	EMoveResizeWindow(eo->win, x, y, w, h);
	return;
     }

   /* Resizing - grab old contents */
   pict = EPictureCreateBuffer(eo->win, wo, ho, c->a.depth, c->a.visual);
   XRenderComposite(disp, PictOpSrc, c->picture, None, pict,
		    0, 0, 0, 0, 0, 0, wo, ho);

   /* Resize (+move) */
   EMoveResizeWindow(eo->win, x, y, w, h);

   /* Paste old contents back in */
   if (w < wo)
      w = wo;
   if (h < ho)
      h = ho;
   XRenderComposite(disp, PictOpSrc, pict, None, c->picture,
		    0, 0, 0, 0, 0, 0, w, h);
   XRenderFreePicture(disp, pict);

#if 0				/* FIXME - the screen update seems to be delayed? */
   if (c->extents != None)
      ECompMgrDamageMerge(c->extents, 0);
#endif
}

/*
 * Desktops (move to desktops.c?)
 */
typedef struct
{
   Picture             bgpict;	/* The background picture */
} EcmDeskInfo;

static EcmDeskInfo  desks[32];

static              Picture
DeskBackgroundPictureGet(int desk)
{
   Picture             pict;
   Pixmap              pmap;
   Bool                fill;
   XRenderPictFormat  *pictfmt;
   XRenderPictureAttributes pa;

   pict = desks[desk].bgpict;
   if (pict != None)
      return pict;

   fill = False;
   pmap = BackgroundGetPixmap(DeskGetBackground(desk));
   D1printf("DeskBackgroundPictureGet: Desk %d: using pixmap %#lx\n", desk,
	    pmap);
   if (!pmap)
     {
	pmap = XCreatePixmap(disp, VRoot.win, 1, 1, VRoot.depth);
	fill = True;
     }

   pa.repeat = True;
   pictfmt = XRenderFindVisualFormat(disp, VRoot.vis);
   pict = XRenderCreatePicture(disp, pmap, pictfmt, CPRepeat, &pa);

   if (fill)
     {
	/* FIXME - use desk bg color */
	XRenderColor        c;

	c.red = c.green = c.blue = 0x8080;
	c.alpha = 0xffff;
	XRenderFillRectangle(disp, PictOpSrc, pict, &c, 0, 0, 1, 1);
     }

   /* New background, all must be repainted */
   ECompMgrDamageAll();

   desks[desk].bgpict = pict;

   return pict;
}

static void
DeskBackgroundPictureFree(int desk)
{
   Picture             pict;

   pict = desks[desk].bgpict;
   if (pict == None)
      return;

   XClearArea(disp, DeskGetWin(desk), 0, 0, 0, 0, True);
   XRenderFreePicture(disp, pict);

   desks[desk].bgpict = None;
}

/*
 * Root (?)
 */

static void
ECompMgrDamageMerge(XserverRegion damage, int destroy)
{
   if (allDamage)
     {
	if (EventDebug(EDBUG_TYPE_COMPMGR3))
	   ERegionShow("ECompMgrDamageMerge add:", damage);

	XFixesUnionRegion(disp, allDamage, allDamage, damage);
	if (destroy)
	   XFixesDestroyRegion(disp, damage);
     }
   else if (!destroy)
     {
	allDamage = XFixesCreateRegion(disp, 0, 0);
	XFixesCopyRegion(disp, allDamage, damage);
     }
   else
     {
	allDamage = damage;
     }

   if (EventDebug(EDBUG_TYPE_COMPMGR3))
      ERegionShow("ECompMgrDamageMerge all:", allDamage);
}

static void
ECompMgrDamageAll(void)
{
   ECompMgrDamageMerge(ERegionCreate(0, 0, VRoot.w, VRoot.h), 1);
}

#if ENABLE_FADING

typedef struct _fade
{
   struct _fade       *next;
   EObj               *eo;
   double              cur;
   double              step;
   void                (*callback) (Display * dpy, EObj * eo, Bool gone);
   Display            *dpy;
   Bool                gone;
} fade;

static fade        *fades;
double              fade_step = 0.05;
int                 fade_delta = 10;
int                 fade_time = 0;

Bool                fadeWindows;

static int
get_time_in_milliseconds(void)
{
   struct timeval      tv;

   gettimeofday(&tv, NULL);
   return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static fade        *
find_fade(EObj * eo)
{
   fade               *f;

   for (f = fades; f; f = f->next)
     {
	if (f->eo == eo)
	   return f;
     }
   return NULL;
}

static void
dequeue_fade(Display * dpy, fade * f)
{
   fade              **prev;

   for (prev = &fades; *prev; prev = &(*prev)->next)
      if (*prev == f)
	{
	   *prev = f->next;
	   if (f->callback)
	      (*f->callback) (dpy, f->eo, f->gone);
	   free(f);
	   break;
	}
}

static void
cleanup_fade(Display * dpy, EObj * eo)
{
   fade               *f = find_fade(eo);

   if (f)
      dequeue_fade(dpy, f);
}

static void
enqueue_fade(Display * dpy __UNUSED__, fade * f)
{
   if (!fades)
      fade_time = get_time_in_milliseconds() + fade_delta;
   f->next = fades;
   fades = f;
}

static void
set_fade(Display * dpy, EObj * eo, Bool in,
	 void (*callback) (Display * dpy, EObj * eo, Bool gone), Bool gone)
{
   fade               *f;

   f = find_fade(eo);
   if (!f)
     {
	f = malloc(sizeof(fade));
	f->next = 0;
	f->eo = eo;
	if (in)
	   f->cur = 0;
	else
	   f->cur = 1;
	enqueue_fade(dpy, f);
     }
   if (in)
      f->step = fade_step;
   else
      f->step = -fade_step;
   f->callback = callback;
   f->gone = gone;

   ECompMgrWinChangeOpacity(eo, f->cur * OPAQUE);
}

static int
fade_timeout(void)
{
   int                 now;
   int                 delta;

   if (!fades)
      return -1;
   now = get_time_in_milliseconds();
   delta = fade_time - now;
   if (delta < 0)
      delta = 0;
/*    printf ("timeout %d\n", delta); */
   return delta;
}

static void
run_fades(Display * dpy)
{
   int                 now = get_time_in_milliseconds();
   fade               *f, *next;
   int                 steps;

#if 0
   printf("run fades\n");
#endif
   if (fade_time - now > 0)
      return;
   steps = 1 + (now - fade_time) / fade_delta;
   for (next = fades; (f = next);)
     {
	next = f->next;
	f->cur += f->step * steps;
	if (f->cur >= 1)
	   f->cur = 1;
	else if (f->cur < 0)
	   f->cur = 0;

	if (f->step > 0)
	  {
	     if (f->cur >= 1)
		dequeue_fade(dpy, f);
	  }
	else
	  {
	     if (f->cur <= 0)
		dequeue_fade(dpy, f);
	  }

	ECompMgrWinChangeOpacity(f->eo, f->cur * OPAQUE);
     }
   fade_time = now + fade_delta;
}

#endif /* ENABLE_FADING */

#if ENABLE_SHADOWS

#define SHADOW_OPACITY	0.75

static Picture      blackPicture;
static Picture      transBlackPicture;

typedef struct _conv
{
   int                 size;
   double             *data;
} conv;

static conv        *gaussianMap;

static double
gaussian(double r, double x, double y)
{
   return ((1 / (sqrt(2 * M_PI * r))) * exp((-(x * x + y * y)) / (2 * r * r)));
}

static conv        *
make_gaussian_map(Display * dpy __UNUSED__, double r)
{
   conv               *c;
   int                 size = ((int)ceil((r * 3)) + 1) & ~1;
   int                 center = size / 2;
   int                 x, y;
   double              t;
   double              g;

   c = malloc(sizeof(conv) + size * size * sizeof(double));
   c->size = size;
   c->data = (double *)(c + 1);
   t = 0.0;
   for (y = 0; y < size; y++)
      for (x = 0; x < size; x++)
	{
	   g = gaussian(r, (double)(x - center), (double)(y - center));
	   t += g;
	   c->data[y * size + x] = g;
	}
/*    printf ("gaussian total %f\n", t); */
   for (y = 0; y < size; y++)
      for (x = 0; x < size; x++)
	{
	   c->data[y * size + x] /= t;
	}
   return c;
}

/*
 * A picture will help
 *
 *	-center   0                width  width+center
 *  -center +-----+-------------------+-----+
 *	    |     |                   |     |
 *	    |     |                   |     |
 *        0 +-----+-------------------+-----+
 *	    |     |                   |     |
 *	    |     |                   |     |
 *	    |     |                   |     |
 *   height +-----+-------------------+-----+
 *	    |     |                   |     |
 * height+  |     |                   |     |
 *  center  +-----+-------------------+-----+
 */

static unsigned char
sum_gaussian(conv * map, double opacity, int x, int y, int width, int height)
{
   int                 fx, fy;
   double             *g_data;
   double             *g_line = map->data;
   int                 g_size = map->size;
   int                 center = g_size / 2;
   int                 fx_start, fx_end;
   int                 fy_start, fy_end;
   double              v;

   /*
    * Compute set of filter values which are "in range",
    * that's the set with:
    *  0 <= x + (fx-center) && x + (fx-center) < width &&
    *  0 <= y + (fy-center) && y + (fy-center) < height
    *
    *  0 <= x + (fx - center)  x + fx - center < width
    *  center - x <= fx        fx < width + center - x
    */

   fx_start = center - x;
   if (fx_start < 0)
      fx_start = 0;
   fx_end = width + center - x;
   if (fx_end > g_size)
      fx_end = g_size;

   fy_start = center - y;
   if (fy_start < 0)
      fy_start = 0;
   fy_end = height + center - y;
   if (fy_end > g_size)
      fy_end = g_size;

   g_line = g_line + fy_start * g_size + fx_start;

   v = 0;
   for (fy = fy_start; fy < fy_end; fy++)
     {
	g_data = g_line;
	g_line += g_size;

	for (fx = fx_start; fx < fx_end; fx++)
	   v += *g_data++;
     }
   if (v > 1)
      v = 1;

   return ((unsigned char)(v * opacity * 255.0));
}

static XImage      *
make_shadow(Display * dpy, double opacity, int width, int height)
{
   XImage             *ximage;
   unsigned char      *data;
   int                 gsize = gaussianMap->size;
   int                 ylimit, xlimit;
   int                 swidth = width + gsize;
   int                 sheight = height + gsize;
   int                 center = gsize / 2;
   int                 x, y;
   unsigned char       d;
   int                 x_diff;

   data = calloc(swidth * sheight, sizeof(unsigned char));
   if (!data)
      return NULL;

   ximage = XCreateImage(dpy, DefaultVisual(dpy, DefaultScreen(dpy)),
			 8, ZPixmap, 0,
			 (char *)data,
			 swidth, sheight, 8, swidth * sizeof(unsigned char));
   if (!ximage)
     {
	free(data);
	return NULL;
     }

   /*
    * Build the gaussian in sections
    */

#if 0				/* Never used */
   /*
    * center (fill the complete data array)
    */

   d = sum_gaussian(gaussianMap, opacity, center, center, width, height);
   memset(data, d, sheight * swidth);
#endif

   /*
    * corners
    */
   ylimit = gsize;
   if (ylimit > sheight / 2)
      ylimit = (sheight + 1) / 2;
   xlimit = gsize;
   if (xlimit > swidth / 2)
      xlimit = (swidth + 1) / 2;

   for (y = 0; y < ylimit; y++)
      for (x = 0; x < xlimit; x++)
	{
	   d = sum_gaussian(gaussianMap, opacity, x - center, y - center, width,
			    height);
	   data[y * swidth + x] = d;
	   data[(sheight - y - 1) * swidth + x] = d;
	   data[(sheight - y - 1) * swidth + (swidth - x - 1)] = d;
	   data[y * swidth + (swidth - x - 1)] = d;
	}

   /*
    * top/bottom
    */
   x_diff = swidth - (gsize * 2);
   if (x_diff > 0 && ylimit > 0)
     {
	for (y = 0; y < ylimit; y++)
	  {
	     d = sum_gaussian(gaussianMap, opacity, center, y - center, width,
			      height);
	     memset(&data[y * swidth + gsize], d, x_diff);
	     memset(&data[(sheight - y - 1) * swidth + gsize], d, x_diff);
	  }
     }

   /*
    * sides
    */

   for (x = 0; x < xlimit; x++)
     {
	d = sum_gaussian(gaussianMap, opacity, x - center, center, width,
			 height);
	for (y = gsize; y < sheight - gsize; y++)
	  {
	     data[y * swidth + x] = d;
	     data[y * swidth + (swidth - x - 1)] = d;
	  }
     }

   return ximage;
}

static              Picture
shadow_picture(Display * dpy, double opacity, int width, int height, int *wp,
	       int *hp)
{
   XImage             *shadowImage;
   Pixmap              shadowPixmap;
   Picture             shadowPicture;
   GC                  gc;

   shadowImage = make_shadow(dpy, opacity, width, height);
   if (!shadowImage)
      return None;

   shadowPixmap = XCreatePixmap(dpy, VRoot.win,
				shadowImage->width, shadowImage->height, 8);
   shadowPicture = XRenderCreatePicture(dpy, shadowPixmap,
					XRenderFindStandardFormat(dpy,
								  PictStandardA8),
					0, 0);
   gc = XCreateGC(dpy, shadowPixmap, 0, 0);

   XPutImage(dpy, shadowPixmap, gc, shadowImage, 0, 0, 0, 0,
	     shadowImage->width, shadowImage->height);
   *wp = shadowImage->width;
   *hp = shadowImage->height;
   XFreeGC(dpy, gc);
   XDestroyImage(shadowImage);
   XFreePixmap(dpy, shadowPixmap);

   return shadowPicture;
}

#endif /* ENABLE_SHADOWS */

/*
 * Window ops
 */

static              XserverRegion
win_extents(Display * dpy, EObj * eo)
{
   ECmWinInfo         *w = eo->cmhook;
   XRectangle          r;

#if HAS_NAME_WINDOW_PIXMAP
   w->rcx = w->a.x;
   w->rcy = w->a.y;
   w->rcw = w->a.width + w->a.border_width * 2;
   w->rch = w->a.height + w->a.border_width * 2;
#else
   w->rcx = w->a.x + w->a.border_width;
   w->rcy = w->a.y + w->a.border_width;
   w->rcw = w->a.width;
   w->rch = w->a.height;
#endif

   r.x = w->a.x;
   r.y = w->a.y;
   r.width = w->a.width + w->a.border_width * 2;
   r.height = w->a.height + w->a.border_width * 2;

#if ENABLE_SHADOWS
   if (eo->shadow && Conf_compmgr.shadow != ECM_SHADOWS_OFF &&
       (Conf_compmgr.shadow == ECM_SHADOWS_SHARP || w->mode != WINDOW_ARGB) &&
       (Conf_compmgr.shadow != ECM_SHADOWS_BLURRED || !EobjIsShaped(eo)))
     {
	XRectangle          sr;

	if (Conf_compmgr.shadow == ECM_SHADOWS_SHARP)
	  {
	     w->shadow_dx = 2;
	     w->shadow_dy = 7;
	     w->shadow_width = w->a.width;
	     w->shadow_height = w->a.height;
	  }
	else
	  {
	     w->shadow_dx = -Conf_compmgr.shadow_radius * 5 / 4;
	     w->shadow_dy = -Conf_compmgr.shadow_radius * 5 / 4;
	     if (!w->shadow)
	       {
		  double              opacity = SHADOW_OPACITY;

		  if (w->mode == WINDOW_TRANS)
		     opacity *= ((double)w->opacity) / OPAQUE;
		  w->shadow = shadow_picture(dpy, opacity,
					     w->a.width +
					     w->a.border_width * 2,
					     w->a.height +
					     w->a.border_width * 2,
					     &w->shadow_width,
					     &w->shadow_height);
	       }
	  }
	sr.x = w->a.x + w->shadow_dx;
	sr.y = w->a.y + w->shadow_dy;
	sr.width = w->shadow_width;
	sr.height = w->shadow_height;
	if (sr.x < r.x)
	  {
	     r.width = (r.x + r.width) - sr.x;
	     r.x = sr.x;
	  }
	if (sr.y < r.y)
	  {
	     r.height = (r.y + r.height) - sr.y;
	     r.y = sr.y;
	  }
	if (sr.x + sr.width > r.x + r.width)
	   r.width = sr.x + sr.width - r.x;
	if (sr.y + sr.height > r.y + r.height)
	   r.height = sr.y + sr.height - r.y;
     }
#endif

   D2printf("win_extents %#lx %d %d %d %d\n", eo->win, r.x, r.y, r.width,
	    r.height);

   return XFixesCreateRegion(dpy, &r, 1);
}

static              XserverRegion
border_size(EObj * eo)
{
   ECmWinInfo         *w = eo->cmhook;
   XserverRegion       border;

   border = XFixesCreateRegionFromWindow(disp, eo->win, WindowRegionBounding);

   /* translate this */
   XFixesTranslateRegion(disp, border,
			 w->a.x + w->a.border_width,
			 w->a.y + w->a.border_width);

   D2printf("border_size %#lx: %d %d\n",
	    eo->win, w->a.x + w->a.border_width, w->a.y + w->a.border_width);

   return border;
}

Pixmap
ECompMgrWinGetPixmap(const EObj * eo)
{
#if HAS_NAME_WINDOW_PIXMAP
   ECmWinInfo         *w = eo->cmhook;

   return (w) ? w->pixmap : None;
#else
   eo = NULL;
   return None;
#endif
}

static void
ECompMgrWinInvalidate(EObj * eo, int what)
{
   ECmWinInfo         *w = eo->cmhook;
   Display            *dpy = disp;

   if (!w)
      return;

#if HAS_NAME_WINDOW_PIXMAP
   if ((what & INV_SIZE) && w->pixmap != None)
     {
	XFreePixmap(dpy, w->pixmap);
	w->pixmap = None;
	if (w->picture != None)
	  {
	     XRenderFreePicture(dpy, w->picture);
	     w->picture = None;
	  }
     }
#endif

   if ((what & INV_GEOM) && w->borderSize != None)
     {
	XFixesDestroyRegion(dpy, w->borderSize);
	w->borderSize = None;
     }

   if ((what & INV_OPACITY) && w->alphaPict != None)
     {
	XRenderFreePicture(dpy, w->alphaPict);
	w->alphaPict = None;
     }

#if 0				/* Recalculating clip every repaint for now. */
   if ((what & INV_SIZE) && w->clip != None)
     {
	XFixesDestroyRegion(dpy, w->clip);
	w->clip = None;
     }
#endif

#if ENABLE_SHADOWS
   if ((what & (INV_SIZE | INV_OPACITY | INV_SHADOW)) && w->shadow != None)
     {
	XRenderFreePicture(dpy, w->shadow);
	w->shadow = None;
	what |= INV_GEOM;
     }
   if ((what & (INV_SIZE | INV_OPACITY | INV_SHADOW)) && w->shadowPict != None)
     {
	XRenderFreePicture(dpy, w->shadowPict);
	w->shadowPict = None;
     }
#endif

   if ((what & (INV_GEOM | INV_SHADOW)) && w->extents != None)
     {
	ECompMgrDamageMerge(w->extents, 1);
	w->extents = None;
     }
}

void
ECompMgrWinChangeOpacity(EObj * eo, unsigned int opacity)
{
   ECmWinInfo         *w = eo->cmhook;
   Display            *dpy = disp;
   int                 mode;
   XRenderPictFormat  *pictfmt;

   if (!w || w->opacity == opacity)
      return;

   w->opacity = opacity;

   D1printf("ECompMgrWinChangeOpacity: %#lx opacity=%#x\n", eo->win,
	    w->opacity);

   /* Invalidate stuff changed by opacity */
   ECompMgrWinInvalidate(eo, INV_OPACITY);

   /* Extents may be unchanged, however, we must repaint */
   if (w->extents != None)
      ECompMgrDamageMerge(w->extents, 0);

   if (w->a.class == InputOnly)
      pictfmt = NULL;
   else
      pictfmt = XRenderFindVisualFormat(dpy, w->a.visual);

   if (pictfmt && pictfmt->type == PictTypeDirect && pictfmt->direct.alphaMask)
      mode = WINDOW_ARGB;
   else if (w->opacity != OPAQUE)
      mode = WINDOW_TRANS;
   else
      mode = WINDOW_SOLID;
   w->mode = mode;
}

static void
ECompMgrWinMap(EObj * eo, unsigned long sequence __UNUSED__,
	       Bool do_fade __UNUSED__)
{
   ECmWinInfo         *w = eo->cmhook;

   if (!w)
     {
	ECompMgrWinNew(eo);
	w = eo->cmhook;
	if (!w)
	   return;
     }

   w->a.map_state = IsViewable;
   w->visible = 1;

   D1printf("ECompMgrWinMap %#lx\n", eo->win);
#if CAN_DO_USABLE
   w->damage_bounds.x = w->damage_bounds.y = 0;
   w->damage_bounds.width = w->damage_bounds.height = 0;
#endif
   ECompMgrDamageMerge(w->extents, 0);
}

static void
finish_unmap_win(EObj * eo)
{
   ECmWinInfo         *w = eo->cmhook;

   w->visible = 0;

#if 0
   w->damaged = 0;

#if CAN_DO_USABLE
   w->usable = False;
#endif

   ECompMgrWinInvalidate(eo, INV_SIZE);

   if (w->extents != None)
     {
	ECompMgrDamageMerge(w->extents, 1);
	w->extents = None;
     }
#else
   if (w->extents != None)
      ECompMgrDamageMerge(w->extents, 0);
#endif
}

#if ENABLE_FADING
static void
unmap_callback(Display * dpy, ECmWinInfo * w, Bool gone __UNUSED__)
{
   finish_unmap_win(dpy, w);
}
#endif

static void
ECompMgrWinUnmap(EObj * eo, Bool do_fade __UNUSED__)
{
   D1printf("ECompMgrWinUnmap %#lx\n", eo->win);

#if ENABLE_FADING
   ECmWinInfo         *w = eo->cmhook;

   if (w->pixmap && do_fade && fadeWindows)
      set_fade(dpy, w, False, unmap_callback, False);
   else
#endif
      finish_unmap_win(eo);
}

static void
ECompMgrWinSetPicts(EObj * eo)
{
   ECmWinInfo         *w = eo->cmhook;

   if (w->a.class == InputOnly)
      return;

#if HAS_NAME_WINDOW_PIXMAP
   if (w->pixmap != None)
     {
	XFreePixmap(disp, w->pixmap);
	w->pixmap = None;
	if (w->picture != None)
	  {
	     XRenderFreePicture(disp, w->picture);
	     w->picture = None;
	  }
     }
   if (Mode_compmgr.use_pixmap)
      w->pixmap = XCompositeNameWindowPixmap(disp, eo->win);
#endif

#if 0
   if (w->picture != None)
     {
	XRenderFreePicture(disp, w->picture);
	w->picture = None;
     }
#endif
   if (w->picture == None)
     {
	XRenderPictFormat  *pictfmt;
	XRenderPictureAttributes pa;
	Drawable            draw = eo->win;

#if HAS_NAME_WINDOW_PIXMAP
	if (w->pixmap)
	   draw = w->pixmap;
#endif
	pictfmt = XRenderFindVisualFormat(disp, w->a.visual);
	pa.subwindow_mode = IncludeInferiors;
	w->picture = XRenderCreatePicture(disp, draw,
					  pictfmt, CPSubwindowMode, &pa);
	D2printf("New picture %#lx\n", w->picture);
     }
}

void
ECompMgrWinNew(EObj * eo)
{
   ECmWinInfo         *w;

   w = Ecalloc(1, sizeof(ECmWinInfo));
   if (!w)
      return;

   D1printf("ECompMgrWinNew %#lx\n", eo->win);

   if (!XGetWindowAttributes(disp, eo->win, &w->a))
     {
	free(w);
	return;
     }

   eo->cmhook = w;

   w->damaged = 0;
#if CAN_DO_USABLE
   w->usable = False;
#endif

   if (w->a.class == InputOnly)
     {
	w->damage_sequence = 0;
	w->damage = None;
     }
   else
     {
#if 0
	XCompositeRedirectWindow(disp, eo->win, CompositeRedirectManual);
#endif
	w->damage_sequence = NextRequest(disp);
	w->damage = XDamageCreate(disp, eo->win, XDamageReportNonEmpty);
     }

   w->picture = None;
#if HAS_NAME_WINDOW_PIXMAP
   w->pixmap = None;
#endif
   ECompMgrWinSetPicts(eo);

   w->alphaPict = None;
   w->borderSize = None;
   w->extents = None;
   w->clip = None;
#if ENABLE_SHADOWS
   w->shadowPict = None;
   w->shadow = None;
   w->shadow_dx = 0;
   w->shadow_dy = 0;
   w->shadow_width = 0;
   w->shadow_height = 0;
#endif

#if 0
   /* moved mode setting to one place */
   ESelectInputAdd(disp, eo->win,
		   PropertyChangeMask /* | StructureNotifyMask */ );
#endif

   w->opacity = 0xdeadbeef;
   ECompMgrWinChangeOpacity(eo, eo->opacity);

   /* Find new window region */
   w->extents = win_extents(disp, eo);

   EventCallbackRegister(eo->win, 0, ECompMgrHandleWindowEvent, eo);

#if 0
   if (w->a.map_state == IsViewable)
      ECompMgrWinMap(eo, w->damage_sequence - 1, False);
#endif
}

static void
ECompMgrWinConfigure(EObj * eo, XEvent * ev)
{
   ECmWinInfo         *w = eo->cmhook;
   XserverRegion       damage = None;
   int                 change_xy, change_wh, change_bw, invalidate;

   D1printf("ECompMgrWinConfigure %#lx %#lx\n", eo->win, w->extents);

   /* Invalidate old window region */
#if CAN_DO_USABLE
   if (w->usable)
#endif
     {
	damage = XFixesCreateRegion(disp, 0, 0);
	if (w->extents != None)
	   XFixesCopyRegion(disp, damage, w->extents);
	if (EventDebug(EDBUG_TYPE_COMPMGR3))
	   ERegionShow("ECompMgrWinConfigure old-extents:", damage);
     }

   /* Can this change ?!? */
   w->a.override_redirect = ev->xconfigure.override_redirect;

   change_xy = w->a.x != ev->xconfigure.x || w->a.y != ev->xconfigure.y;
   change_wh = w->a.width != ev->xconfigure.width ||
      w->a.height != ev->xconfigure.height;
   change_bw = w->a.border_width != ev->xconfigure.border_width;

   invalidate = 0;
   if (change_xy || change_bw)
      invalidate |= INV_POS;
   if (change_wh || change_bw)
      invalidate |= INV_SIZE;

   if (invalidate)
     {
	ECompMgrWinInvalidate(eo, invalidate);
	if (invalidate & INV_SIZE)
	   ECompMgrWinSetPicts(eo);

	w->a.x = ev->xconfigure.x;
	w->a.y = ev->xconfigure.y;
	w->a.width = ev->xconfigure.width;
	w->a.height = ev->xconfigure.height;
	w->a.border_width = ev->xconfigure.border_width;

	/* Find new window region */
	w->extents = win_extents(disp, eo);
     }

   /* Hmmm. Why if not changed? */
   /* Invalidate new window region */
   XFixesUnionRegion(disp, damage, damage, w->extents);
   ECompMgrDamageMerge(damage, 1);
}

static void
ECompMgrWinCirculate(EObj * eo, XEvent * ev)
{
   D1printf("ECompMgrWinCirculate %#lx %#lx\n", ev->xany.window, eo->win);

   /* FIXME - Check if stacking was changed */
}

static void
finish_destroy_win(EObj * eo, Bool gone)
{
   ECmWinInfo         *w = eo->cmhook;

   if (!gone)
      finish_unmap_win(eo);

   ECompMgrWinInvalidate(eo, INV_ALL);

   if (w->picture != None)
     {
	XRenderFreePicture(disp, w->picture);
	w->picture = None;
     }

   if (w->damage != None)
     {
	XDamageDestroy(disp, w->damage);
	w->damage = None;
     }

#if ENABLE_FADING
   cleanup_fade(disp, eo);
#endif

   _EFREE(eo->cmhook);
}

#if ENABLE_FADING
static void
destroy_callback(EObj * eo, Bool gone)
{
   finish_destroy_win(eo, gone);
}
#endif

void
ECompMgrWinDel(EObj * eo, Bool gone, Bool do_fade)
{
   ECmWinInfo         *w = eo->cmhook;

   if (!w)
      return;

   D1printf("ECompMgrWinDel %#lx\n", eo->win);

#if ENABLE_FADING
   if (w && w->pixmap && do_fade && fadeWindows)
      set_fade(disp, w, False, destroy_callback, gone);
   else
#endif
      finish_destroy_win(eo, gone);
   do_fade = False;

#if 0
   XCompositeUnredirectWindow(disp, eo->win, CompositeRedirectManual);
#endif
   EventCallbackUnregister(eo->win, 0, ECompMgrHandleWindowEvent, eo);
}

static void
ECompMgrWinDamage(EObj * eo, XEvent * ev __UNUSED__)
{
   ECmWinInfo         *w = eo->cmhook;
   Display            *dpy = disp;
   XDamageNotifyEvent *de = (XDamageNotifyEvent *) ev;
   XserverRegion       parts;

   D1printf("ECompMgrWinDamage %#lx %#lx damaged=%d %d,%d %dx%d\n",
	    ev->xany.window, eo->win, w->damaged,
	    de->area.x, de->area.y, de->area.width, de->area.height);

#if CAN_DO_USABLE
   if (!w->usable)
     {
	if (w->damage_bounds.width == 0 || w->damage_bounds.height == 0)
	  {
	     w->damage_bounds = de->area;
	  }
	else
	  {
	     if (de->area.x < w->damage_bounds.x)
	       {
		  w->damage_bounds.width += (w->damage_bounds.x - de->area.x);
		  w->damage_bounds.x = de->area.x;
	       }
	     if (de->area.y < w->damage_bounds.y)
	       {
		  w->damage_bounds.height += (w->damage_bounds.y - de->area.y);
		  w->damage_bounds.y = de->area.y;
	       }
	     if (de->area.x + de->area.width >
		 w->damage_bounds.x + w->damage_bounds.width)
		w->damage_bounds.width =
		   de->area.x + de->area.width - w->damage_bounds.x;
	     if (de->area.y + de->area.height >
		 w->damage_bounds.y + w->damage_bounds.height)
		w->damage_bounds.height =
		   de->area.y + de->area.height - w->damage_bounds.y;
	  }
#if 0
	printf("unusable damage %d, %d: %d x %d bounds %d, %d: %d x %d\n",
	       de->area.x,
	       de->area.y,
	       de->area.width,
	       de->area.height,
	       w->damage_bounds.x,
	       w->damage_bounds.y,
	       w->damage_bounds.width, w->damage_bounds.height);
#endif
	if (w->damage_bounds.x <= 0 &&
	    w->damage_bounds.y <= 0 &&
	    w->a.width <= w->damage_bounds.x + w->damage_bounds.width &&
	    w->a.height <= w->damage_bounds.y + w->damage_bounds.height)
	  {
#if ENABLE_FADING
	     if (fadeWindows)
		set_fade(dpy, w, True, 0, False);
#endif
	     w->usable = True;
	  }
     }
   if (!w->usable)
      return;
#else
   de = NULL;
#endif

   if (!w->damaged)
     {
	parts = win_extents(dpy, eo);
	XDamageSubtract(dpy, w->damage, None, None);
     }
   else
     {
	parts = XFixesCreateRegion(dpy, 0, 0);
	XDamageSubtract(dpy, w->damage, None, parts);
	XFixesTranslateRegion(dpy, parts,
			      w->a.x + w->a.border_width,
			      w->a.y + w->a.border_width);
#if ENABLE_SHADOWS
	if (Conf_compmgr.shadow == ECM_SHADOWS_SHARP)
	  {
	     XserverRegion       o;

	     o = XFixesCreateRegion(dpy, 0, 0);
	     XFixesCopyRegion(dpy, o, parts);
	     XFixesTranslateRegion(dpy, o, w->shadow_dx, w->shadow_dy);
	     XFixesUnionRegion(dpy, parts, parts, o);
	     XFixesDestroyRegion(dpy, o);
	  }
#endif
     }
   ECompMgrDamageMerge(parts, 1);
   w->damaged = 1;
}

/* Ensure that the blend mask is up to date */
static void
ECompMgrCheckAlphaMask(ECmWinInfo * w)
{
   if (w->opacity != OPAQUE && !w->alphaPict)
      w->alphaPict = EPictureCreateSolid(False, (double)w->opacity / OPAQUE,
					 0, 0, 0);
}

static void
ECompMgrRepaintDetermineOrder(void)
{
   EObj               *const *lst, *eo, *eo_prev, *eo_first;
   int                 i, num;
   ECmWinInfo         *w;

#if 0
   lst = EobjListStackGet(&num);
#else
   lst = EobjListStackGetForDesk(&num, 0 /*desk */ );
#endif

   /* Determine overall paint order, top to bottom */
   eo_first = eo_prev = NULL;

   for (i = 0; i < num; i++)
     {
	eo = lst[i];
	if (!eo->cmhook)
	   continue;
	w = eo->cmhook;

	if (!w->visible)
	   continue;
#if CAN_DO_USABLE
	if (!w->usable)
	   continue;
#endif
	if (!w->damaged)
	   continue;
	if (!w->picture)
	   continue;

	if (!eo_first)
	   eo_first = eo;
	w->prev = eo_prev;
	if (eo_prev)
	   ((ECmWinInfo *) (eo_prev->cmhook))->next = eo;
	eo_prev = eo;

#if 0
	/*  FIXME - We should break when the repaint region (w->clip) becomes empty */
	if (eo->type == EOBJ_TYPE_DESK && eo->x == 0 && eo->y == 0)
	   break;
#endif
     }
   if (eo_prev)
      ((ECmWinInfo *) (eo_prev->cmhook))->next = NULL;

   Mode_compmgr.eo_first = eo_first;
   Mode_compmgr.eo_last = eo_prev;
}

static void
ECompMgrRepaintObj(Picture pbuf, XserverRegion region, EObj * eo, int mode)
{
   Display            *dpy = disp;
   ECmWinInfo         *w;

   w = eo->cmhook;

#if 0
   ECompMgrWinSetPicts(eo);
#endif

   D2printf("ECompMgrRepaintObj %d %#lx %d %#lx\n", mode, eo->win, w->mode,
	    w->picture);

   /* Region of shaped window in screen coordinates */
   if (!w->borderSize)
      w->borderSize = border_size(eo);
   if (EventDebug(EDBUG_TYPE_COMPMGR3))
      ERegionShow("Window borderSize", w->borderSize);

   /* Region of window in screen coordinates, including shadows */
   if (!w->extents)
      w->extents = win_extents(dpy, eo);
   if (EventDebug(EDBUG_TYPE_COMPMGR3))
      ERegionShow("Window extents", w->extents);

   if (mode == 0)
     {
	/* Painting opaque windows top down, updating clip regions. */

	w->clip = XFixesCreateRegion(dpy, 0, 0);
	XFixesCopyRegion(dpy, w->clip, region);

	switch (w->mode)
	  {
	  case WINDOW_SOLID:
	     XFixesSetPictureClipRegion(dpy, pbuf, 0, 0, region);
	     XFixesSubtractRegion(dpy, region, region, w->borderSize);
	     XRenderComposite(dpy, PictOpSrc, w->picture, None, pbuf,
			      0, 0, 0, 0, w->rcx, w->rcy, w->rcw, w->rch);
	     break;
	  }
     }
   else
     {
	/* Painting trans stuff bottom up. */

	switch (w->mode)
	  {
	  case WINDOW_TRANS:
	  case WINDOW_ARGB:
	     XFixesSetPictureClipRegion(dpy, pbuf, 0, 0, w->clip);
	     ECompMgrCheckAlphaMask(w);
	     XRenderComposite(dpy, PictOpOver, w->picture, w->alphaPict, pbuf,
			      0, 0, 0, 0, w->rcx, w->rcy, w->rcw, w->rch);
	     break;
	  }

#if ENABLE_SHADOWS
	switch (Conf_compmgr.shadow)
	  {
	  case ECM_SHADOWS_OFF:
	     break;
	  case ECM_SHADOWS_SHARP:
	     if (w->opacity != OPAQUE && !w->shadowPict)
		w->shadowPict = EPictureCreateSolid(True,
						    (double)w->opacity /
						    OPAQUE * 0.3, 0, 0, 0);
	     XFixesSubtractRegion(dpy, w->clip, w->clip, w->borderSize);
	     XFixesSetPictureClipRegion(dpy, pbuf, 0, 0, w->clip);
	     XRenderComposite(dpy, PictOpOver,
			      w->shadowPict ? w->shadowPict : transBlackPicture,
			      w->picture, pbuf, 0, 0, 0, 0,
			      w->a.x + w->shadow_dx, w->a.y + w->shadow_dy,
			      w->shadow_width, w->shadow_height);
	     break;
	  case ECM_SHADOWS_BLURRED:
	     if (w->shadow)
	       {
		  XFixesSubtractRegion(dpy, w->clip, w->clip, w->borderSize);
		  XFixesSetPictureClipRegion(dpy, pbuf, 0, 0, w->clip);
		  XRenderComposite(dpy, PictOpOver, blackPicture, w->shadow,
				   pbuf, 0, 0, 0, 0,
				   w->a.x + w->shadow_dx, w->a.y + w->shadow_dy,
				   w->shadow_width, w->shadow_height);
	       }
	     break;
	  }
#endif

	XFixesDestroyRegion(dpy, w->clip);
	w->clip = None;
     }
}

static void
ECompMgrRepaint(void)
{
   Display            *dpy = disp;
   XserverRegion       region = allDamage;
   EObj               *eo;
   Picture             pict, pbuf;

   D2printf("ECompMgrRepaint rootBuffer=%#lx rootPicture=%#lx\n",
	    rootBuffer, rootPicture);
   if (EventDebug(EDBUG_TYPE_COMPMGR3))
      ERegionShow("ECompMgrRepaint", region);

   if (!rootBuffer)
      rootBuffer = EPictureCreateBuffer(VRoot.win, VRoot.w, VRoot.h,
					VRoot.depth, VRoot.vis);
   pbuf = rootBuffer;

   /* Draw desktop background picture */
   pict = DeskBackgroundPictureGet(0);
   D1printf("ECompMgrRepaint desk picture=%#lx\n", pict);
   XFixesSetPictureClipRegion(dpy, pbuf, 0, 0, region);
   XRenderComposite(dpy, PictOpSrc, pict, None, pbuf,
		    0, 0, 0, 0, 0, 0, VRoot.w, VRoot.h);

   /* Do paint order list linking */
   ECompMgrRepaintDetermineOrder();

   /* Paint opaque windows top down, adjusting clip regions */
   for (eo = Mode_compmgr.eo_first; eo;
	eo = ((ECmWinInfo *) (eo->cmhook))->next)
      ECompMgrRepaintObj(pbuf, region, eo, 0);

   /* Paint trans windows and shadows bottom up */
   for (eo = Mode_compmgr.eo_last; eo; eo = ((ECmWinInfo *) (eo->cmhook))->prev)
      ECompMgrRepaintObj(pbuf, None, eo, 1);

   if (pbuf != rootPicture)
     {
	XFixesSetPictureClipRegion(dpy, pbuf, 0, 0, None);
	XRenderComposite(dpy, PictOpSrc, pbuf, None, rootPicture,
			 0, 0, 0, 0, 0, 0, VRoot.w, VRoot.h);
     }

   XFixesDestroyRegion(dpy, region);
   allDamage = None;
}

static void
ECompMgrRootConfigure(void *prm __UNUSED__, XEvent * ev)
{
   Display            *dpy = disp;

   D1printf("ECompMgrRootConfigure root\n");
   if (ev->xconfigure.window == VRoot.win)
     {
	if (rootBuffer)
	  {
	     XRenderFreePicture(dpy, rootBuffer);
	     rootBuffer = None;
	  }
#if 0				/* Should be handled elsewhere */
	VRoot.w = ev->xconfigure.width;
	VRoot.h = ev->xconfigure.height;
#endif
     }
   return;
}

static void
ECompMgrRootExpose(void *prm __UNUSED__, XEvent * ev)
{
   Display            *dpy = disp;
   static XRectangle  *expose_rects = 0;
   static int          size_expose = 0;
   static int          n_expose = 0;
   int                 more = ev->xexpose.count + 1;

   if (ev->xexpose.window != VRoot.win)
      return;

   D1printf("ECompMgrRootExpose %d %d %d\n", n_expose, size_expose,
	    ev->xexpose.count);

   if (n_expose == size_expose)
     {
	if (expose_rects)
	  {
	     expose_rects = realloc(expose_rects,
				    (size_expose + more) * sizeof(XRectangle));
	     size_expose += more;
	  }
	else
	  {
	     expose_rects = malloc(more * sizeof(XRectangle));
	     size_expose = more;
	  }
     }
   expose_rects[n_expose].x = ev->xexpose.x;
   expose_rects[n_expose].y = ev->xexpose.y;
   expose_rects[n_expose].width = ev->xexpose.width;
   expose_rects[n_expose].height = ev->xexpose.height;
   n_expose++;
   if (ev->xexpose.count == 0)
     {
	XserverRegion       region;

	region = XFixesCreateRegion(dpy, expose_rects, n_expose);

	ECompMgrDamageMerge(region, 1);
	n_expose = 0;
     }
}

static void
ECompMgrDeskChanged(int desk)
{
   D1printf("ECompMgrDeskChanged: desk=%d\n", desk);

   DeskBackgroundPictureFree(desk);
}

#if ENABLE_SHADOWS
static void
ECompMgrShadowsInit(int mode, int cleanup)
{
   if (mode == ECM_SHADOWS_BLURRED)
      gaussianMap = make_gaussian_map(disp, Conf_compmgr.shadow_radius);
   else
     {
	if (gaussianMap)
	   free(gaussianMap);
	gaussianMap = NULL;
     }

   if (mode != ECM_SHADOWS_OFF)
      blackPicture = EPictureCreateSolid(True, 1, 0, 0, 0);
   else
     {
	if (blackPicture)
	   XRenderFreePicture(disp, blackPicture);
	blackPicture = None;
     }

   if (mode == ECM_SHADOWS_SHARP)
      transBlackPicture = EPictureCreateSolid(True, 0.3, 0, 0, 0);
   else
     {
	if (transBlackPicture)
	   XRenderFreePicture(disp, transBlackPicture);
	transBlackPicture = None;
     }

   if (cleanup)
     {
	EObj               *const *lst;
	int                 i, num;

	lst = EobjListStackGet(&num);
	for (i = 0; i < num; i++)
	   ECompMgrWinInvalidate(lst[i], INV_SHADOW);
     }
}
#else
#define ECompMgrShadowsInit(mode, cleanup)
#endif

static void
ECompMgrStart(void)
{
   XRenderPictFormat  *pictfmt;
   XRenderPictureAttributes pa;

   if (!Conf_compmgr.enable || Mode_compmgr.active)
      return;
   Mode_compmgr.active = 1;

   pa.subwindow_mode = IncludeInferiors;
   pictfmt = XRenderFindVisualFormat(disp, VRoot.vis);
   rootPicture =
      XRenderCreatePicture(disp, VRoot.win, pictfmt, CPSubwindowMode, &pa);

   ECompMgrShadowsInit(Conf_compmgr.shadow, 0);

   switch (Conf_compmgr.mode)
     {
     case ECM_MODE_ROOT:
	XCompositeRedirectSubwindows(disp, VRoot.win, CompositeRedirectManual);
	ESelectInputAdd(VRoot.win,
			SubstructureNotifyMask |
			ExposureMask | StructureNotifyMask);
	break;
     case ECM_MODE_AUTO:
	XCompositeRedirectSubwindows(disp, VRoot.win,
				     CompositeRedirectAutomatic);
	break;
     case ECM_MODE_WINDOW:
	break;
     }

   allDamage = None;
   if (Conf_compmgr.mode != ECM_MODE_AUTO)
     {
	ECompMgrDamageAll();
	ECompMgrRepaint();
     }

   EventCallbackRegister(VRoot.win, 0, ECompMgrHandleRootEvent, NULL);
}

static void
ECompMgrStop(void)
{
   EObj               *const *lst;
   int                 i, num;

   if (!Mode_compmgr.active)
      return;
   Mode_compmgr.active = 0;

   if (rootPicture)
      XRenderFreePicture(disp, rootPicture);
   rootPicture = None;

   if (rootBuffer)
      XRenderFreePicture(disp, rootBuffer);
   rootBuffer = None;

   DeskBackgroundPictureFree(0);

   ECompMgrShadowsInit(ECM_SHADOWS_OFF, 0);

   lst = EobjListStackGet(&num);
   for (i = 0; i < num; i++)
      ECompMgrWinDel(lst[i], False, False);

   XCompositeUnredirectSubwindows(disp, VRoot.win, CompositeRedirectManual);

   EventCallbackUnregister(VRoot.win, 0, ECompMgrHandleRootEvent, NULL);
}

void
ECompMgrConfigGet(cfg_composite * cfg)
{
   cfg->enable = Conf_compmgr.enable;
   cfg->shadow = Conf_compmgr.shadow;
}

void
ECompMgrConfigSet(const cfg_composite * cfg)
{
   Conf_compmgr.mode = ECM_MODE_OFF;
   if (Conf_compmgr.mode == ECM_MODE_OFF)
     {
	if (cfg->enable)
	   DialogOK("Enable Composite Error",
		    _("Cannot enable Composite Manager.\n"
		      "Use xdpyinfo to check that\n"
		      "Composite, Damage, Fixes, and Render\n"
		      "extensions are loaded."));
	return;
     }

   if (cfg->enable != Conf_compmgr.enable)
     {
	Conf_compmgr.enable = cfg->enable;
	Conf_compmgr.shadow = cfg->shadow;
	if (cfg->enable)
	   ECompMgrStart();
	else
	   ECompMgrStop();
     }
   else
     {
	if (cfg->shadow != Conf_compmgr.shadow)
	  {
	     Conf_compmgr.shadow = cfg->shadow;
	     if (Conf_compmgr.enable)
	       {
		  ECompMgrShadowsInit(Conf_compmgr.shadow, 1);
		  ECompMgrDamageAll();
	       }
	  }
     }
}

int
ECompMgrActive(void)		/* FIXME - Remove */
{
   return Mode_compmgr.active;
}

/*
 * Event handlers
 */

static void
ECompMgrHandleWindowEvent(XEvent * ev, void *prm)
{
   EObj               *eo = prm;

   switch (ev->type)
     {
#if 0
     case ConfigureNotify:
	ECompMgrWinConfigure(eo, ev);
	break;

     case MapNotify:
	ECompMgrWinMap(eo, ev->xmap.serial, True);
	break;
     case UnmapNotify:
	ECompMgrWinUnmap(eo, True);
	break;

     case CirculateNotify:
	ECompMgrWinCirculate(eo, ev);
	break;
#endif

     case EX_EVENT_DAMAGE_NOTIFY:
	ECompMgrWinDamage(eo, ev);
	break;
     }
}

static void
ECompMgrHandleRootEvent(XEvent * ev, void *prm)
{
   EObj               *eo;

   switch (ev->type)
     {
     case CreateNotify:
#if 0
      case_CreateNotify:
	eo = EobjListStackFind(ev->xcreatewindow.window);
	if (eo)
	   ECompMgrWinNew(eo);
#endif
	break;
     case DestroyNotify:
	eo = EobjListStackFind(ev->xdestroywindow.window);
	if (eo && eo->cmhook)
	   ECompMgrWinDel(eo, True, True);
	EobjUnregister(ev->xdestroywindow.window);
	break;

     case ReparentNotify:
	eo = EobjListStackFind(ev->xreparent.window);
	if (eo && eo->cmhook)
	   ECompMgrWinUnmap(eo, True);
#if 0
	if (ev->xreparent.parent == VRoot.win)
	   goto case_CreateNotify;
	eo = EobjListStackFind(ev->xreparent.window);
	if (eo)
	   ECompMgrWinDel(eo, False, True);
#endif
	break;

     case ConfigureNotify:
	if (ev->xconfigure.window == VRoot.win)
	  {
	     ECompMgrRootConfigure(prm, ev);
	  }
	else
	  {
	     eo = EobjListStackFind(ev->xconfigure.window);
	     if (eo && eo->cmhook)
		ECompMgrWinConfigure(eo, ev);
	  }
	break;

     case MapNotify:
	eo = EobjListStackFind(ev->xmap.window);
	if (!eo)
	   eo = EobjRegister(ev->xmap.window, EOBJ_TYPE_OVERR);
	else
	   EobjListStackRaise(eo);	/* FIXME - Use Configure/CirculateNotify */
	if (eo)
	   ECompMgrWinMap(eo, ev->xmap.serial, True);
	break;
     case UnmapNotify:
	eo = EobjListStackFind(ev->xunmap.window);
	if (eo && eo->cmhook)
	   ECompMgrWinUnmap(eo, True);
	break;

     case CirculateNotify:
	eo = EobjListStackFind(ev->xcirculate.window);
	if (eo && eo->cmhook)
	   ECompMgrWinCirculate(eo, ev);
	break;

     case Expose:
	ECompMgrRootExpose(prm, ev);
	break;
     }
}

void
ECompMgrParseArgs(const char *args)
{
   Mode_compmgr.args = Estrdup(args);
}

/*
 * Module functions
 */

static void
ECompMgrInit(void)
{
   int                 events, errors, major, minor;

   if (!XCompositeQueryExtension(disp, &events, &errors) ||
       !XDamageQueryExtension(disp, &events, &errors) ||
       !XFixesQueryExtension(disp, &events, &errors) ||
       !XRenderQueryExtension(disp, &events, &errors))
     {
	Conf_compmgr.mode = ECM_MODE_OFF;
	goto done;
     }

   if (!XDamageQueryVersion(disp, &major, &minor))
     {
	Conf_compmgr.mode = ECM_MODE_OFF;
	goto done;
     }
#if HAS_NAME_WINDOW_PIXMAP
   Mode_compmgr.use_pixmap =
      (major > 0 || minor >= 2) && Conf_compmgr.use_name_pixmap;
#endif

   if (Conf_compmgr.mode == ECM_MODE_OFF)
      Conf_compmgr.mode = ECM_MODE_ROOT;

   if (Mode_compmgr.args)
     {
	switch (Mode_compmgr.args[0])
	  {
	  case '0':
	     Conf_compmgr.mode = ECM_MODE_OFF;
	     break;
	  case '1':
	     Conf_compmgr.mode = ECM_MODE_ROOT;
	     break;
	  case '2':
	     Conf_compmgr.mode = ECM_MODE_WINDOW;
	     break;
	  case '3':
	     Conf_compmgr.mode = ECM_MODE_AUTO;
	     break;
	  }
	_EFREE(Mode_compmgr.args);
     }

 done:
   if (Conf_compmgr.mode == ECM_MODE_OFF)
      Conf_compmgr.enable = 0;
   D1printf("ECompMgrInit: enable=%d mode=%d\n", Conf_compmgr.enable,
	    Conf_compmgr.mode);
}

static void
ECompMgrSighan(int sig, void *prm)
{
   if (sig != ESIGNAL_INIT && Conf_compmgr.mode == ECM_MODE_OFF)
      return;

   switch (sig)
     {
     case ESIGNAL_INIT:
	ECompMgrInit();
	ECompMgrStart();
	break;

     case ESIGNAL_BACKGROUND_CHANGE:
	ECompMgrDeskChanged((int)prm);
	break;

     case ESIGNAL_IDLE:
	/* Do we get here on auto? */
	if (!allDamage /* || Conf_compmgr.mode == ECM_MODE_AUTO */ )
	   return;
	ECompMgrRepaint();
	XSync(disp, False);
	break;
     }
}

static void
CompMgrIpc(const char *params, Client * c __UNUSED__)
{
   const char         *p;
   char                cmd[128], prm[4096];
   int                 len;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %4000s %n", cmd, prm, &len);
	p += len;
     }

   if (!p || cmd[0] == '?')
     {
	IpcPrintf("CompMgr - on=%d\n", Mode_compmgr.active);
     }
   else if (!strcmp(cmd, "cfg"))
     {
	SettingsComposite();
     }
   else if (!strcmp(cmd, "start"))
     {
	ECompMgrStart();
     }
   else if (!strcmp(cmd, "stop"))
     {
	ECompMgrStop();
     }
   else if (!strncmp(cmd, "list", 2))
     {
	/* TBD */
     }
}

IpcItem             CompMgrIpcArray[] = {
   {
    CompMgrIpc,
    "compmgr", "cm",
    "Composite manager functions",
    "  cm ?                     Show info\n"
    "  cm cfg                   Configure\n"
    "  cm start                 Start composite manager\n"
    "  cm stop                  Stop composite manager\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(CompMgrIpcArray)/sizeof(IpcItem))

static const CfgItem CompMgrCfgItems[] = {
   CFG_ITEM_BOOL(Conf_compmgr, enable, 0),
   CFG_ITEM_INT(Conf_compmgr, mode, 1),
   CFG_ITEM_INT(Conf_compmgr, shadow, 0),
   CFG_ITEM_INT(Conf_compmgr, shadow_radius, 12),
   CFG_ITEM_BOOL(Conf_compmgr, resize_fix_enable, 0),
   CFG_ITEM_BOOL(Conf_compmgr, use_name_pixmap, 0),
};
#define N_CFG_ITEMS (sizeof(CompMgrCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
EModule             ModCompMgr = {
   "compmgr", "cm",
   ECompMgrSighan,
   {N_IPC_FUNCS, CompMgrIpcArray},
   {N_CFG_ITEMS, CompMgrCfgItems}
};

#endif /* USE_COMPOSITE */
