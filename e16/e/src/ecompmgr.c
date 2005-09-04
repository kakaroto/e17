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
/*
 * This code was originally derived from xcompmgr.c,  see original copyright
 * notice at end.
 * It has been mostly rewritten since, only the shadow code is more or less
 * intact.
 */

#include "E.h"
#if USE_COMPOSITE
#include "desktops.h"
#include "ecompmgr.h"
#include "emodule.h"
#include "xwin.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xrender.h>

#define ENABLE_SHADOWS 1

#define ENABLE_DEBUG   1
#if ENABLE_DEBUG
#define EDBUG_TYPE_COMPMGR  161
#define EDBUG_TYPE_COMPMGR2 162
#define EDBUG_TYPE_COMPMGR3 163
#define EDBUG_TYPE_COMPMGR4 164
#define D1printf(fmt...) if(EventDebug(EDBUG_TYPE_COMPMGR))Eprintf(fmt)
#define D2printf(fmt...) if(EventDebug(EDBUG_TYPE_COMPMGR2))Eprintf(fmt)
#define D3printf(fmt...) if(EventDebug(EDBUG_TYPE_COMPMGR3))Eprintf(fmt)
#define D4printf(fmt...) if(EventDebug(EDBUG_TYPE_COMPMGR4))Eprintf(fmt)
#else
#define D1printf(fmt...)
#define D2printf(fmt...)
#endif /* ENABLE_DEBUG */

#define INV_POS     0x01
#define INV_SIZE    0x02
#define INV_OPACITY 0x04
#define INV_SHADOW  0x08
#define INV_PIXMAP  0x10
#define INV_GEOM    (INV_POS | INV_SIZE)
#define INV_ALL     (INV_POS | INV_SIZE | INV_OPACITY | INV_SHADOW | INV_PIXMAP)

typedef struct
{
   EObj               *next;	/* Paint order */
   EObj               *prev;	/* Paint order */
   Pixmap              pixmap;
   struct
   {
      int                 class;	/* FIXME - Remove? */
      int                 depth;	/* FIXME - Remove? */
      Visual             *visual;	/* FIXME - Remove? */
      int                 border_width;
   } a;
   int                 rcx, rcy, rcw, rch;
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
} ECmWinInfo;

/*
 * Configuration
 */
#if ENABLE_SHADOWS
#define ECM_SHADOWS_OFF      0
#define ECM_SHADOWS_SHARP    1	/* use window alpha for shadow; sharp, but precise */
#define ECM_SHADOWS_BLURRED  2	/* use window extents for shadow, blurred */
#endif

#define ECM_OR_UNMANAGED     0
#define ECM_OR_ON_MAP        1
#define ECM_OR_ON_MAPUNMAP   2
#define ECM_OR_ON_CREATE     3

static struct
{
   char                enable;
   char                resize_fix_enable;
   char                use_name_pixmap;
   int                 mode;
   int                 shadow;
   int                 shadow_radius;
   struct
   {
      int                 enable;
      int                 dt_us;	/* us between updates */
      unsigned int        step;
   } fading;
   struct
   {
      int                 mode;
      int                 opacity;
   } override_redirect;
} Conf_compmgr;

/*
 * State
 */
#define ECM_MODE_OFF    0
#define ECM_MODE_ROOT   1
#define ECM_MODE_WINDOW 2
#define ECM_MODE_AUTO   3

static struct
{
   char                active;
   char                use_pixmap;
   EObj               *eo_first;
   EObj               *eo_last;
   XserverRegion       rgn_screen;
} Mode_compmgr;

static Picture      rootPicture;
static Picture      rootBuffer;

static XserverRegion allDamage;

#define OPAQUE          0xffffffff

#define WINDOW_UNREDIR  0
#define WINDOW_SOLID    1
#define WINDOW_TRANS    2
#define WINDOW_ARGB     3

static void         ECompMgrWinSetPicts(EObj * eo);
static void         ECompMgrDamageAll(void);
static void         ECompMgrHandleRootEvent(XEvent * ev, void *prm);
static void         ECompMgrHandleWindowEvent(XEvent * ev, void *prm);

/*
 * Visuals
 */

int
EVisualIsARGB(Visual * vis)
{
   XRenderPictFormat  *pictfmt;

   if (!Mode_compmgr.active)
      return 0;

   pictfmt = XRenderFindVisualFormat(disp, vis);
   if (!pictfmt)
      return 0;

#if 0
   Eprintf("Visual ID=%#lx Type=%d, alphamask=%d\n", vis->visualid,
	   pictfmt->type, pictfmt->direct.alphaMask);
#endif
   return pictfmt->type == PictTypeDirect && pictfmt->direct.alphaMask;
}

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
ERegionLimit(XserverRegion rgn)
{
   XserverRegion       screen;

   screen = Mode_compmgr.rgn_screen;
   if (screen == None)
      Mode_compmgr.rgn_screen = screen = ERegionCreate(0, 0, VRoot.w, VRoot.h);

   XFixesIntersectRegion(disp, rgn, rgn, screen);
}

static void
ERegionShow(const char *txt, XserverRegion rgn)
{
   int                 i, nr;
   XRectangle         *pr;

   pr = XFixesFetchRegion(disp, rgn, &nr);
   if (!pr || nr <= 0)
     {
	Eprintf(" - region: %s %#lx is empty\n", txt, rgn);
	goto done;
     }

   Eprintf(" - region: %s %#lx:\n", txt, rgn);
   for (i = 0; i < nr; i++)
      Eprintf("%4d: %4d+%4d %4dx%4d\n", i, pr[i].x, pr[i].y, pr[i].width,
	      pr[i].height);

 done:
   if (pr)
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

#if 0
static              Picture
EPictureCreate(Window win, int depth, Visual * vis)
{
   Picture             pict;
   XRenderPictFormat  *pictfmt;

   pictfmt = XRenderFindVisualFormat(disp, vis);
   pict = XRenderCreatePicture(disp, win, pictfmt, 0, 0);

   return pict;
}
#endif

/* Hack to fix redirected window resize bug(?) */
void
ECompMgrMoveResizeFix(EObj * eo, int x, int y, int w, int h)
{
   Picture             pict;
   int                 wo, ho;
   ECmWinInfo         *cw = eo->cmhook;

   if (!cw || !Conf_compmgr.resize_fix_enable)
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
   pict = EPictureCreateBuffer(eo->win, wo, ho, cw->a.depth, cw->a.visual);
   XRenderComposite(disp, PictOpSrc, cw->picture, None, pict,
		    0, 0, 0, 0, 0, 0, wo, ho);

   /* Resize (+move) */
   EMoveResizeWindow(eo->win, x, y, w, h);

   /* Paste old contents back in */
   if (w < wo)
      w = wo;
   if (h < ho)
      h = ho;
   XRenderComposite(disp, PictOpSrc, pict, None, cw->picture,
		    0, 0, 0, 0, 0, 0, w, h);
   XRenderFreePicture(disp, pict);
}

/*
 * Desktops (move to desktops.c?)
 */

static              Picture
DeskBackgroundPictureGet(Desk * dsk)
{
   ECmWinInfo         *cw = dsk->o.cmhook;
   Picture             pict;
   Pixmap              pmap;
   Bool                fill;
   XRenderPictFormat  *pictfmt;
   XRenderPictureAttributes pa;

   if (!cw)
     {
	ECompMgrWinNew(&dsk->o);
	cw = dsk->o.cmhook;
	if (!cw)
	   return None;
     }

   fill = False;
   pmap = BackgroundGetPixmap(DeskGetBackground(dsk));
   if (pmap == None)
     {
	if (cw->pixmap && cw->picture)
	   return cw->picture;
	pmap = XCreatePixmap(disp, VRoot.win, 1, 1, VRoot.depth);
	fill = True;
     }
   else if (pmap == cw->pixmap)
     {
	if (cw->picture != None)
	   return cw->picture;
     }
   D1printf
      ("DeskBackgroundPictureGet: Desk %d: using pixmap %#lx (%#lx %#lx)\n",
       dsk->num, pmap, cw->pixmap, cw->picture);

   if (cw->picture)
      XRenderFreePicture(disp, cw->picture);

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
	XFreePixmap(disp, pmap);
     }

#if 0				/* FIXME - Not in window mode? */
   /* New background, all must be repainted */
   ECompMgrDamageAll();
#endif

   cw->pixmap = pmap;
   cw->picture = pict;

   return pict;
}

static void
DeskBackgroundPictureFree(Desk * dsk)
{
   ECmWinInfo         *cw = dsk->o.cmhook;
   Picture             pict;

   if (!cw)
      return;

   pict = cw->picture;
   if (pict == None)
      return;

   D1printf("DeskBackgroundPictureFree: Desk %d: pict=%#lx\n", dsk->num, pict);

   XRenderFreePicture(disp, pict);

   cw->picture = None;
}

/*
 * Root (?)
 */

static void
ECompMgrDamageMerge(XserverRegion damage, int destroy)
{
   if (allDamage != None)
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
ECompMgrDamageMergeObject(EObj * eo, XserverRegion damage, int destroy)
{
   Desk               *dsk = eo->desk;

   if (dsk->num > 0 && !dsk->viewable && eo->ilayer < 512)
     {
	if (destroy)
	   XFixesDestroyRegion(disp, damage);
	return;
     }

   if (dsk->num > 0)
     {
	if (EoGetX(dsk) != 0 || EoGetY(dsk) != 0)
	  {
	     if (!destroy)
	       {
		  XserverRegion       region;

		  region = XFixesCreateRegion(disp, 0, 0);
		  XFixesCopyRegion(disp, region, damage);
		  damage = region;
	       }
	     XFixesTranslateRegion(disp, damage, EoGetX(dsk), EoGetY(dsk));
	  }
     }

   ECompMgrDamageMerge(damage, destroy);
}

static void
ECompMgrDamageAll(void)
{
   ECompMgrDamageMerge(ERegionCreate(0, 0, VRoot.w, VRoot.h), 1);
}

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
   ECmWinInfo         *cw = eo->cmhook;
   XRectangle          r;

   if (Mode_compmgr.use_pixmap)
     {
	cw->rcx = eo->x;
	cw->rcy = eo->y;
	cw->rcw = eo->w + cw->a.border_width * 2;
	cw->rch = eo->h + cw->a.border_width * 2;
     }
   else
     {
	cw->rcx = eo->x + cw->a.border_width;
	cw->rcy = eo->y + cw->a.border_width;
	cw->rcw = eo->w;
	cw->rch = eo->h;
     }

   r.x = cw->rcx;
   r.y = cw->rcy;
   r.width = cw->rcw;
   r.height = cw->rch;

#if ENABLE_SHADOWS
   if (eo->shadow && Conf_compmgr.shadow != ECM_SHADOWS_OFF &&
       (Conf_compmgr.shadow == ECM_SHADOWS_SHARP || cw->mode != WINDOW_ARGB) &&
       (Conf_compmgr.shadow != ECM_SHADOWS_BLURRED || !EobjIsShaped(eo)))
     {
	XRectangle          sr;

	if (Conf_compmgr.shadow == ECM_SHADOWS_SHARP)
	  {
	     cw->shadow_dx = 2;
	     cw->shadow_dy = 7;
	     cw->shadow_width = cw->rcw;
	     cw->shadow_height = cw->rch;
	  }
	else
	  {
	     cw->shadow_dx = -Conf_compmgr.shadow_radius * 5 / 4;
	     cw->shadow_dy = -Conf_compmgr.shadow_radius * 5 / 4;
	     if (!cw->shadow)
	       {
		  double              opacity = SHADOW_OPACITY;

		  if (cw->mode == WINDOW_TRANS)
		     opacity *= ((double)cw->opacity) / OPAQUE;
		  cw->shadow = shadow_picture(dpy, opacity, cw->rcw, cw->rch,
					      &cw->shadow_width,
					      &cw->shadow_height);
	       }
	  }
	sr.x = cw->rcx + cw->shadow_dx;
	sr.y = cw->rcy + cw->shadow_dy;
	sr.width = cw->shadow_width;
	sr.height = cw->shadow_height;
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
   ECmWinInfo         *cw = eo->cmhook;
   XserverRegion       border;
   int                 x, y;

   border = XFixesCreateRegionFromWindow(disp, eo->win, WindowRegionBounding);

   /* translate this */
   x = eo->x + cw->a.border_width;
   y = eo->y + cw->a.border_width;
   XFixesTranslateRegion(disp, border, x, y);

   D2printf("border_size %#lx: %d %d\n", eo->win, x, y);
   if (EventDebug(EDBUG_TYPE_COMPMGR3))
      ERegionShow("borderSize", border);

   return border;
}

Pixmap
ECompMgrWinGetPixmap(const EObj * eo)
{
   ECmWinInfo         *cw = eo->cmhook;

   if (!cw)
      return None;

   if (cw->pixmap != None)
      return cw->pixmap;

   cw->pixmap = XCompositeNameWindowPixmap(disp, eo->win);

   return cw->pixmap;
}

static void
ECompMgrWinInvalidate(EObj * eo, int what)
{
   ECmWinInfo         *cw = eo->cmhook;
   Display            *dpy = disp;

   if (!cw)
      return;

   if ((what & (INV_SIZE | INV_PIXMAP)) && cw->pixmap != None)
     {
	XFreePixmap(dpy, cw->pixmap);
	cw->pixmap = None;
	if (cw->picture != None && Mode_compmgr.use_pixmap)
	  {
	     XRenderFreePicture(dpy, cw->picture);
	     cw->picture = None;
	  }
     }

   if ((what & INV_GEOM) && cw->borderSize != None)
     {
	XFixesDestroyRegion(dpy, cw->borderSize);
	cw->borderSize = None;
     }

   if ((what & INV_OPACITY) && cw->alphaPict != None)
     {
	XRenderFreePicture(dpy, cw->alphaPict);
	cw->alphaPict = None;
     }

#if 0				/* Recalculating clip every repaint for now. */
   if ((what & INV_SIZE) && cw->clip != None)
     {
	XFixesDestroyRegion(dpy, cw->clip);
	cw->clip = None;
     }
#endif

#if ENABLE_SHADOWS
   if ((what & (INV_SIZE | INV_OPACITY | INV_SHADOW)) && cw->shadow != None)
     {
	XRenderFreePicture(dpy, cw->shadow);
	cw->shadow = None;
	what |= INV_GEOM;
     }
   if ((what & (INV_SIZE | INV_OPACITY | INV_SHADOW)) && cw->shadowPict != None)
     {
	XRenderFreePicture(dpy, cw->shadowPict);
	cw->shadowPict = None;
     }
#endif

   if ((what & (INV_GEOM | INV_SHADOW)) && cw->extents != None)
     {
#if 0				/* FIXME - Not necessary? */
	ECompMgrDamageMergeObject(eo, cw->extents, 1);
#else
	XFixesDestroyRegion(dpy, cw->extents);
#endif
	cw->extents = None;
     }
}

void
ECompMgrWinChangeOpacity(EObj * eo, unsigned int opacity)
{
   ECmWinInfo         *cw = eo->cmhook;
   int                 mode;

   if (!cw || cw->opacity == opacity)
      return;

   cw->opacity = opacity;

   D1printf("ECompMgrWinChangeOpacity: %#lx opacity=%#x\n", eo->win,
	    cw->opacity);

   if (eo->shown)		/* FIXME - ??? */
      /* Extents may be unchanged, however, we must repaint */
      if (cw->extents != None)
	 ECompMgrDamageMergeObject(eo, cw->extents, 0);

   /* Invalidate stuff changed by opacity */
   ECompMgrWinInvalidate(eo, INV_OPACITY);

   if (eo->noredir)
      mode = WINDOW_UNREDIR;
   else if (EVisualIsARGB(cw->a.visual))
      mode = WINDOW_ARGB;
   else if (cw->opacity != OPAQUE)
      mode = WINDOW_TRANS;
   else
      mode = WINDOW_SOLID;
   cw->mode = mode;
}

static void
doECompMgrWinFade(int val, void *data)
{
   EObj               *eo = data;
   ECmWinInfo         *cw;
   unsigned int        op = (unsigned int)val;

   /* May be gone */
   if (!EobjListStackFind(eo->win))
      return;

   cw = eo->cmhook;
   if (cw->opacity == op)
      return;

   if (op > cw->opacity)
     {
	if (op - cw->opacity > Conf_compmgr.fading.step)
	  {
	     DoIn("Fade", 1e-6 * Conf_compmgr.fading.dt_us, doECompMgrWinFade,
		  op, eo);
	     op = cw->opacity + Conf_compmgr.fading.step;
	  }
     }
   else
     {
	if (cw->opacity - op > Conf_compmgr.fading.step)
	  {
	     DoIn("Fade", 1e-6 * Conf_compmgr.fading.dt_us, doECompMgrWinFade,
		  op, eo);
	     op = cw->opacity - Conf_compmgr.fading.step;
	  }
     }

#if 0
   Eprintf("doECompMgrWinFade %#x\n", op);
#endif
   ECompMgrWinChangeOpacity(eo, op);
}

static void
ECompMgrWinFadeIn(EObj * eo)
{
   DoIn("Fade", 1e-6 * Conf_compmgr.fading.dt_us, doECompMgrWinFade,
	eo->opacity, eo);
   ECompMgrWinChangeOpacity(eo, 0x10000000);
}

static void
ECompMgrWinFadeOut(EObj * eo)
{
   DoIn("Fade", 1e-6 * Conf_compmgr.fading.dt_us, doECompMgrWinFade,
	0x10000000, eo);
   ECompMgrWinChangeOpacity(eo, eo->opacity);
}

void
ECompMgrWinMap(EObj * eo)
{
   ECmWinInfo         *cw = eo->cmhook;

   if (!cw)
     {
	ECompMgrWinNew(eo);
	cw = eo->cmhook;
	if (!cw)
	   return;
     }

   D1printf("ECompMgrWinMap %#lx\n", eo->win);

   if (cw->extents == None)
      cw->extents = win_extents(disp, eo);
   ECompMgrDamageMergeObject(eo, cw->extents, 0);

   ECompMgrWinSetPicts(eo);
   if (Conf_compmgr.fading.enable && eo->fade)
      ECompMgrWinFadeIn(eo);
}

void
ECompMgrWinUnmap(EObj * eo)
{
   ECmWinInfo         *cw = eo->cmhook;

   D1printf("ECompMgrWinUnmap %#lx\n", eo->win);

   if (cw->extents != None)
      ECompMgrDamageMergeObject(eo, cw->extents, 0);

   if (Conf_compmgr.fading.enable && eo->fade)
      ECompMgrWinFadeOut(eo);
   else
      ECompMgrWinInvalidate(eo, INV_PIXMAP);
}

static void
ECompMgrWinSetPicts(EObj * eo)
{
   ECmWinInfo         *cw = eo->cmhook;

   if (eo->type == EOBJ_TYPE_DESK)
     {
#if 0				/* FIXME - Get this right */
	if (cw->picture == None)
#endif
	  {
	     cw->picture = DeskBackgroundPictureGet((Desk *) eo);
	     cw->damaged = 1;	/* FIXME - ??? */
	  }
	return;
     }

   if (cw->pixmap == None && Mode_compmgr.use_pixmap)
      cw->pixmap = XCompositeNameWindowPixmap(disp, eo->win);

   if (cw->picture == None)
     {
	XRenderPictFormat  *pictfmt;
	XRenderPictureAttributes pa;
	Drawable            draw = eo->win;

	if (cw->pixmap && Mode_compmgr.use_pixmap)
	   draw = cw->pixmap;

	pictfmt = XRenderFindVisualFormat(disp, cw->a.visual);
	pa.subwindow_mode = IncludeInferiors;
	cw->picture = XRenderCreatePicture(disp, draw,
					   pictfmt, CPSubwindowMode, &pa);
	D2printf("New picture %#lx\n", cw->picture);
     }
}

void
ECompMgrWinNew(EObj * eo)
{
   ECmWinInfo         *cw;
   XWindowAttributes   attr;

   if (!Mode_compmgr.active)	/* FIXME - Here? */
      return;

   if (!XGetWindowAttributes(disp, eo->win, &attr))
      return;
   if (attr.class == InputOnly)
      return;

   cw = Ecalloc(1, sizeof(ECmWinInfo));
   if (!cw)
      return;

   D1printf("ECompMgrWinNew %#lx\n", eo->win);

   eo->cmhook = cw;

   cw->damaged = 0;

   cw->a.class = attr.class;	/* FIXME - remove */
   cw->a.depth = attr.depth;
   cw->a.visual = attr.visual;
   cw->a.border_width = attr.border_width;

   if (eo->type == EOBJ_TYPE_DESK)
      eo->noredir = 1;

   if (!eo->noredir)
     {
	if (Conf_compmgr.mode == ECM_MODE_WINDOW)
	   XCompositeRedirectWindow(disp, eo->win, CompositeRedirectManual);
	cw->damage_sequence = NextRequest(disp);
	cw->damage = XDamageCreate(disp, eo->win, XDamageReportNonEmpty);
     }

   if (eo->type == EOBJ_TYPE_EXT)
      eo->opacity =
	 (unsigned int)(Conf_compmgr.override_redirect.opacity << 24);
   if (eo->opacity == 0)
      eo->opacity = 0xFFFFFFFF;

   eo->shadow = 1;

   cw->picture = None;
   cw->pixmap = None;
#if 0				/* FIXME - Remove? */
   ECompMgrWinSetPicts(eo);
#endif

   cw->alphaPict = None;
   cw->borderSize = None;
   cw->extents = None;
   cw->clip = None;
#if ENABLE_SHADOWS
   cw->shadowPict = None;
   cw->shadow = None;
   cw->shadow_dx = 0;
   cw->shadow_dy = 0;
   cw->shadow_width = 0;
   cw->shadow_height = 0;
#endif

#if 0				/* FIXME - Do we need this? */
   if (Conf_compmgr.mode == ECM_MODE_WINDOW)
      ESelectInputAdd(eo->win, StructureNotifyMask);
#endif

   cw->opacity = 0xdeadbeef;
   ECompMgrWinChangeOpacity(eo, eo->opacity);

   EventCallbackRegister(eo->win, 0, ECompMgrHandleWindowEvent, eo);
}

void
ECompMgrWinMoveResize(EObj * eo, int change_xy, int change_wh, int change_bw)
{
   ECmWinInfo         *cw = eo->cmhook;
   XserverRegion       damage = None;
   int                 invalidate;

   D1printf("ECompMgrWinMoveResize %#lx xy=%d wh=%d bw=%d\n",
	    eo->win, change_xy, change_wh, change_bw);

   invalidate = 0;
   if (change_xy || change_bw)
      invalidate |= INV_POS;
   if (change_wh || change_bw)
      invalidate |= INV_SIZE;

   if (!invalidate)
      return;

   if (!eo->shown)
     {
	ECompMgrWinInvalidate(eo, invalidate);
	return;
     }

   /* Invalidate old window region */
   damage = XFixesCreateRegion(disp, 0, 0);
   if (cw->extents != None)
      XFixesCopyRegion(disp, damage, cw->extents);
   if (EventDebug(EDBUG_TYPE_COMPMGR3))
      ERegionShow("old-extents:", damage);

   ECompMgrWinInvalidate(eo, invalidate);

   if (invalidate & INV_SIZE)	/* FIXME - ??? */
      ECompMgrWinSetPicts(eo);

   /* Find new window region */
   cw->extents = win_extents(disp, eo);

#if 1
   /* Hmmm. Why if not changed? - To get shadows painted. */
   /* Invalidate new window region */
   XFixesUnionRegion(disp, damage, damage, cw->extents);
#endif

   if (damage != None)
      ECompMgrDamageMergeObject(eo, damage, 1);
}

static void
ECompMgrWinConfigure(EObj * eo, XEvent * ev)
{
   ECmWinInfo         *cw = eo->cmhook;
   int                 x, y, w, h, bw;
   int                 change_xy, change_wh, change_bw;

   x = ev->xconfigure.x;
   y = ev->xconfigure.y;
   w = ev->xconfigure.width;
   h = ev->xconfigure.height;
   bw = ev->xconfigure.border_width;

   change_xy = eo->x != x || eo->y != y;
   change_wh = eo->w != w || eo->h != h;
   change_bw = cw->a.border_width != bw;

   eo->x = x;
   eo->y = y;
   eo->w = w;
   eo->h = h;
   cw->a.border_width = bw;

   ECompMgrWinMoveResize(eo, change_xy, change_wh, change_bw);
}

void
ECompMgrWinReparent(EObj * eo, int desk, int change_xy)
{
   ECmWinInfo         *cw = eo->cmhook;

   D1printf("ECompMgrWinReparent %#lx %#lx d=%d->%d x,y=%d,%d %d\n",
	    eo->win, cw->extents, eo->desk->num, desk, eo->x, eo->y, change_xy);

   /* Invalidate old window region */
   if (EventDebug(EDBUG_TYPE_COMPMGR3))
      ERegionShow("old-extents:", cw->extents);
   ECompMgrDamageMergeObject(eo, cw->extents, change_xy);

   if (change_xy)
     {
	cw->extents = None;
	ECompMgrWinInvalidate(eo, INV_POS);

	/* Find new window region */
	cw->extents = win_extents(disp, eo);
	ECompMgrDamageMergeObject(eo, cw->extents, 0);
     }
}

static void
ECompMgrWinCirculate(EObj * eo, XEvent * ev)
{
   D1printf("ECompMgrWinCirculate %#lx %#lx\n", ev->xany.window, eo->win);

   /* FIXME - Check if stacking was changed */
}

void
ECompMgrWinChangeShape(EObj * eo)
{
   ECmWinInfo         *cw = eo->cmhook;

   if (cw->extents != None)
     {
	ECompMgrDamageMergeObject(eo, cw->extents, 1);
	cw->extents = None;
     }

   ECompMgrWinInvalidate(eo, INV_SIZE);
}

void
ECompMgrWinChangeStacking(EObj * eo)
{
   ECmWinInfo         *cw = eo->cmhook;

   if (Conf_compmgr.shadow == ECM_SHADOWS_OFF)
      return;

   if (cw->extents != None)
      ECompMgrDamageMergeObject(eo, cw->extents, 0);
}

void
ECompMgrWinDel(EObj * eo)
{
   ECmWinInfo         *cw = eo->cmhook;

   if (!cw)
      return;

   D1printf("ECompMgrWinDel %#lx\n", eo->win);

   EventCallbackUnregister(eo->win, 0, ECompMgrHandleWindowEvent, eo);

   if (eo->type == EOBJ_TYPE_DESK)
     {
	DeskBackgroundPictureFree((Desk *) eo);
	cw->pixmap = None;
     }

   if (!eo->noredir)
     {
	if (!eo->gone && Conf_compmgr.mode == ECM_MODE_WINDOW)
	   XCompositeUnredirectWindow(disp, eo->win, CompositeRedirectManual);
     }

   ECompMgrWinInvalidate(eo, INV_ALL);

   if (!eo->gone)
     {
	if (cw->picture != None)
	   XRenderFreePicture(disp, cw->picture);

	if (cw->damage != None)
	   XDamageDestroy(disp, cw->damage);
     }

   _EFREE(eo->cmhook);
}

static void
ECompMgrWinDamage(EObj * eo, XEvent * ev __UNUSED__)
{
   ECmWinInfo         *cw = eo->cmhook;
   Display            *dpy = disp;
   XDamageNotifyEvent *de = (XDamageNotifyEvent *) ev;
   XserverRegion       parts;

   D1printf("ECompMgrWinDamage %#lx %#lx damaged=%d %d,%d %dx%d\n",
	    ev->xany.window, eo->win, cw->damaged,
	    de->area.x, de->area.y, de->area.width, de->area.height);

   if (!cw->damaged)
     {
	parts = win_extents(dpy, eo);
	XDamageSubtract(dpy, cw->damage, None, None);
	cw->damaged = 1;
     }
   else
     {
	parts = XFixesCreateRegion(dpy, 0, 0);
	XDamageSubtract(dpy, cw->damage, None, parts);
	XFixesTranslateRegion(dpy, parts,
			      eo->x + cw->a.border_width,
			      eo->y + cw->a.border_width);
#if ENABLE_SHADOWS
	if (Conf_compmgr.shadow == ECM_SHADOWS_SHARP)
	  {
	     XserverRegion       o;

	     o = XFixesCreateRegion(dpy, 0, 0);
	     XFixesCopyRegion(dpy, o, parts);
	     XFixesTranslateRegion(dpy, o, cw->shadow_dx, cw->shadow_dy);
	     XFixesUnionRegion(dpy, parts, parts, o);
	     XFixesDestroyRegion(dpy, o);
	  }
#endif
     }
   ECompMgrDamageMergeObject(eo, parts, 1);
}

/* Ensure that the blend mask is up to date */
static void
ECompMgrCheckAlphaMask(ECmWinInfo * cw)
{
   if (cw->opacity != OPAQUE && !cw->alphaPict)
      cw->alphaPict = EPictureCreateSolid(False, (double)cw->opacity / OPAQUE,
					  0, 0, 0);
}

static int
ECompMgrRepaintDetermineOrder(EObj * const *lst, int num, EObj ** first,
			      EObj ** last, Desk * dsk)
{
   EObj               *eo, *eo_prev, *eo_first;
   int                 i, stop;
   ECmWinInfo         *cw;

   D4printf("ECompMgrRepaintDetermineOrder %d\n", dsk->num);
   if (!lst)
      lst = EobjListStackGet(&num);

   /* Determine overall paint order, top to bottom */
   stop = 0;
   eo_first = eo_prev = NULL;

   for (i = 0; i < num; i++)
     {
	eo = lst[i];

	if (!eo->shown || eo->desk != dsk)
	   continue;

	D4printf(" - %#lx desk=%d shown=%d\n", eo->win, eo->desk->num,
		 eo->shown);

	if (eo->type == EOBJ_TYPE_DESK)
	  {
	     EObj               *eo1, *eo2;
	     ECmWinInfo         *ec1;
	     Desk               *d = (Desk *) eo;

	     if (!d->viewable)
		continue;

	     stop = ECompMgrRepaintDetermineOrder(lst, num, &eo1, &eo2, d);
	     if (eo1)
	       {
		  ec1 = eo1->cmhook;
		  if (!eo_first)
		     eo_first = eo1;
		  if (eo_prev)
		     ((ECmWinInfo *) (eo_prev->cmhook))->next = eo1;
		  ((ECmWinInfo *) (eo1->cmhook))->prev = eo_prev;
		  eo_prev = eo2;
	       }
	     ECompMgrWinSetPicts(&d->o);
	  }

	cw = eo->cmhook;

	if (!cw)
	   continue;

	D4printf(" - %#lx desk=%d shown=%d dam=%d pict=%#lx\n",
		 eo->win, eo->desk->num, eo->shown, cw->damaged, cw->picture);

#if 0				/* FIXME - Need this? */
	if (!cw->damaged)
	   continue;
#endif
	if (!cw->picture)
	   continue;

	D4printf
	   ("ECompMgrRepaintDetermineOrder hook in %d - %#lx desk=%d shown=%d\n",
	    dsk->num, eo->win, eo->desk->num, eo->shown);

	if (!eo_first)
	   eo_first = eo;
	cw->prev = eo_prev;
	if (eo_prev)
	   ((ECmWinInfo *) (eo_prev->cmhook))->next = eo;
	eo_prev = eo;

	/*  FIXME - We should break when the repaint region (cw->clip) becomes empty */
	if (eo->type == EOBJ_TYPE_DESK && eo->x == 0 && eo->y == 0)
	   stop = 1;
	if (stop)
	   break;
     }
   if (eo_prev)
      ((ECmWinInfo *) (eo_prev->cmhook))->next = NULL;

   *first = eo_first;
   *last = eo_prev;

   return stop;
}

static void
ERegionSubtractOffset(XserverRegion dst, int dx, int dy, XserverRegion src)
{
   Display            *dpy = disp;
   XserverRegion       reg;

   if (dx == 0 && dy == 0)
     {
	reg = src;
     }
   else
     {
	reg = XFixesCreateRegion(dpy, 0, 0);
	XFixesCopyRegion(dpy, reg, src);
	XFixesTranslateRegion(dpy, reg, dx, dy);
     }
   XFixesSubtractRegion(dpy, dst, dst, reg);
   if (reg != src)
      XFixesDestroyRegion(dpy, reg);
}

static void
ECompMgrRepaintObj(Picture pbuf, XserverRegion region, EObj * eo, int mode)
{
   Display            *dpy = disp;
   ECmWinInfo         *cw;
   Desk               *dsk = eo->desk;
   int                 x, y;

   cw = eo->cmhook;

#if 0
   ECompMgrWinSetPicts(eo);
#endif

   D2printf("ECompMgrRepaintObj mode=%d %#lx %s\n", mode, eo->win, eo->name);

   /* Region of shaped window in screen coordinates */
   if (!cw->borderSize)
      cw->borderSize = border_size(eo);
   if (EventDebug(EDBUG_TYPE_COMPMGR3))
      ERegionShow("borderSize", cw->borderSize);

   /* Region of window in screen coordinates, including shadows */
   if (!cw->extents)
      cw->extents = win_extents(dpy, eo);
   if (EventDebug(EDBUG_TYPE_COMPMGR3))
      ERegionShow("extents", cw->extents);

   x = EoGetX(dsk);
   y = EoGetY(dsk);

   if (mode == 0)
     {
	/* Painting opaque windows top down, updating clip regions. */

	cw->clip = XFixesCreateRegion(dpy, 0, 0);
	XFixesCopyRegion(dpy, cw->clip, region);

	switch (cw->mode)
	  {
	  case WINDOW_UNREDIR:
	  case WINDOW_SOLID:
	     D2printf(" * solid pict=%#lx d=%d l=%d\n",
		      cw->picture, eo->desk->num, eo->ilayer);
	     ERegionLimit(region);
	     XFixesSetPictureClipRegion(dpy, pbuf, 0, 0, region);
	     XRenderComposite(dpy, PictOpSrc, cw->picture, None, pbuf,
			      0, 0, 0, 0, x + cw->rcx, y + cw->rcy, cw->rcw,
			      cw->rch);
	     ERegionSubtractOffset(region, x, y, cw->borderSize);
	     break;
	  }
     }
   else
     {
	/* Painting trans stuff bottom up. */

	switch (cw->mode)
	  {
	  case WINDOW_TRANS:
	  case WINDOW_ARGB:
	     D2printf(" * trans pict=%#lx d=%d l=%d\n",
		      cw->picture, eo->desk->num, eo->ilayer);
	     ERegionLimit(cw->clip);
	     XFixesSetPictureClipRegion(dpy, pbuf, 0, 0, cw->clip);
	     ECompMgrCheckAlphaMask(cw);
	     XRenderComposite(dpy, PictOpOver, cw->picture, cw->alphaPict, pbuf,
			      0, 0, 0, 0, x + cw->rcx, y + cw->rcy, cw->rcw,
			      cw->rch);
	     break;
	  }

#if ENABLE_SHADOWS
	switch (Conf_compmgr.shadow)
	  {
	  case ECM_SHADOWS_OFF:
	     break;
	  case ECM_SHADOWS_SHARP:
	     if (cw->opacity != OPAQUE && !cw->shadowPict)
		cw->shadowPict = EPictureCreateSolid(True,
						     (double)cw->opacity /
						     OPAQUE * 0.3, 0, 0, 0);
	     ERegionSubtractOffset(cw->clip, x, y, cw->borderSize);
	     ERegionLimit(cw->clip);
	     XFixesSetPictureClipRegion(dpy, pbuf, 0, 0, cw->clip);
	     XRenderComposite(dpy, PictOpOver,
			      cw->shadowPict ? cw->
			      shadowPict : transBlackPicture, cw->picture, pbuf,
			      0, 0, 0, 0,
			      x + cw->rcx + cw->shadow_dx,
			      y + cw->rcy + cw->shadow_dy,
			      cw->shadow_width, cw->shadow_height);
	     break;
	  case ECM_SHADOWS_BLURRED:
	     if (cw->shadow == None)
		break;

	     ERegionSubtractOffset(cw->clip, x, y, cw->borderSize);
	     ERegionLimit(cw->clip);
	     XFixesSetPictureClipRegion(dpy, pbuf, 0, 0, cw->clip);
	     XRenderComposite(dpy, PictOpOver, blackPicture, cw->shadow, pbuf,
			      0, 0, 0, 0,
			      x + cw->rcx + cw->shadow_dx,
			      y + cw->rcy + cw->shadow_dy,
			      cw->shadow_width, cw->shadow_height);
	     break;
	  }
#endif

	XFixesDestroyRegion(dpy, cw->clip);
	cw->clip = None;
     }
}

void
ECompMgrRepaint(void)
{
   Display            *dpy = disp;
   XserverRegion       region;
   EObj               *eo;
   Picture             pict, pbuf;
   Desk               *dsk = DeskGet(0);

   if (!Mode_compmgr.active || allDamage == None)
      return;

   region = XFixesCreateRegion(disp, 0, 0);
   XFixesCopyRegion(disp, region, allDamage);

   D2printf("ECompMgrRepaint rootBuffer=%#lx rootPicture=%#lx\n",
	    rootBuffer, rootPicture);
   if (EventDebug(EDBUG_TYPE_COMPMGR3))
      ERegionShow("allDamage", region);

   if (!rootBuffer)
      rootBuffer = EPictureCreateBuffer(VRoot.win, VRoot.w, VRoot.h,
					VRoot.depth, VRoot.vis);
   pbuf = rootBuffer;

   if (!dsk)
      return;

   /* Do paint order list linking */
   ECompMgrRepaintDetermineOrder(NULL, 0, &Mode_compmgr.eo_first,
				 &Mode_compmgr.eo_last, dsk);

   /* Paint opaque windows top down, adjusting clip regions */
   for (eo = Mode_compmgr.eo_first; eo;
	eo = ((ECmWinInfo *) (eo->cmhook))->next)
      ECompMgrRepaintObj(pbuf, region, eo, 0);

   if (EventDebug(EDBUG_TYPE_COMPMGR2))
      ERegionShow("after opaque", region);

   /* Repaint background, clipped by damage region and opaque windows */
   pict = DeskBackgroundPictureGet(dsk);
   D1printf("ECompMgrRepaint desk picture=%#lx\n", pict);
   ERegionLimit(region);
   XFixesSetPictureClipRegion(dpy, pbuf, 0, 0, region);
   XRenderComposite(dpy, PictOpSrc, pict, None, pbuf,
		    0, 0, 0, 0, 0, 0, VRoot.w, VRoot.h);

   /* Paint trans windows and shadows bottom up */
   for (eo = Mode_compmgr.eo_last; eo; eo = ((ECmWinInfo *) (eo->cmhook))->prev)
      ECompMgrRepaintObj(pbuf, None, eo, 1);

   if (pbuf != rootPicture)
     {
	ERegionLimit(allDamage);
	XFixesSetPictureClipRegion(dpy, pbuf, 0, 0, allDamage);
	XRenderComposite(dpy, PictOpSrc, pbuf, None, rootPicture,
			 0, 0, 0, 0, 0, 0, VRoot.w, VRoot.h);
     }

   XFixesDestroyRegion(dpy, region);
   XFixesDestroyRegion(dpy, allDamage);
   allDamage = None;
}

static void
ECompMgrRootConfigure(void *prm __UNUSED__, XEvent * ev)
{
   Display            *dpy = disp;

   D1printf("ECompMgrRootConfigure root\n");
   if (ev->xconfigure.window == VRoot.win)
     {
	if (rootBuffer != None)
	  {
	     XRenderFreePicture(dpy, rootBuffer);
	     rootBuffer = None;
	  }

	if (Mode_compmgr.rgn_screen != None)
	   XFixesDestroyRegion(disp, Mode_compmgr.rgn_screen);
	Mode_compmgr.rgn_screen = None;
     }
   return;
}

#if 1				/* FIXME - Need this? */
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
#endif

static void
ECompMgrDeskChanged(Desk * dsk)
{
   if (!dsk || !dsk->o.cmhook)
      return;

   D1printf("ECompMgrDeskChanged: desk=%d\n", dsk->num);

   DeskBackgroundPictureFree(dsk);
   ECompMgrDamageAll();
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
   EObj               *const *lst;
   int                 i, num;
   XRenderPictFormat  *pictfmt;
   XRenderPictureAttributes pa;

   if (Mode_compmgr.active || Conf_compmgr.mode == ECM_MODE_OFF)
      return;
   Conf_compmgr.enable = Mode_compmgr.active = 1;

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
     case ECM_MODE_WINDOW:
	ESelectInputAdd(VRoot.win,
			SubstructureNotifyMask |
			ExposureMask | StructureNotifyMask);
	break;
     case ECM_MODE_AUTO:
	XCompositeRedirectSubwindows(disp, VRoot.win,
				     CompositeRedirectAutomatic);
	break;
     }

   allDamage = None;
   if (Conf_compmgr.mode != ECM_MODE_AUTO)
     {
#if 0				/* FIXME - Remove? */
	ECompMgrDamageAll();
	ECompMgrRepaint();
#endif
     }

   EventCallbackRegister(VRoot.win, 0, ECompMgrHandleRootEvent, NULL);

   lst = EobjListStackGet(&num);
   for (i = 0; i < num; i++)
     {
	ECompMgrWinNew(lst[i]);
	if (lst[i]->shown)
	   ECompMgrWinMap(lst[i]);
     }
}

static void
ECompMgrStop(void)
{
   EObj               *const *lst1, **lst;
   int                 i, num;

   if (!Mode_compmgr.active)
      return;
   Conf_compmgr.enable = Mode_compmgr.active = 0;

   if (rootPicture)
      XRenderFreePicture(disp, rootPicture);
   rootPicture = None;

   if (rootBuffer)
      XRenderFreePicture(disp, rootBuffer);
   rootBuffer = None;

   DeskBackgroundPictureFree(DeskGet(0));

   ECompMgrShadowsInit(ECM_SHADOWS_OFF, 0);

   lst1 = EobjListStackGet(&num);
   if (num > 0)
     {
	lst = Emalloc(num * sizeof(EObj *));
	memcpy(lst, lst1, num * sizeof(EObj *));
	for (i = 0; i < num; i++)
	  {
	     if (lst[i]->type == EOBJ_TYPE_EXT)
		EobjUnregister(lst[i]);	/* Modifies the object stack! */
	     else
		ECompMgrWinDel(lst[i]);
	  }
	Efree(lst);
     }

   if (allDamage != None)
      XFixesDestroyRegion(disp, allDamage);
   allDamage = None;

   if (Mode_compmgr.rgn_screen != None)
      XFixesDestroyRegion(disp, Mode_compmgr.rgn_screen);
   Mode_compmgr.rgn_screen = None;

   if (Conf_compmgr.mode == ECM_MODE_ROOT)
      XCompositeUnredirectSubwindows(disp, VRoot.win, CompositeRedirectManual);

   EventCallbackUnregister(VRoot.win, 0, ECompMgrHandleRootEvent, NULL);

   if (Conf_compmgr.shadow != ECM_SHADOWS_OFF)
      DesksClear();
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

   autosave();
}

/*
 * Event handlers
 */
#define USE_WINDOW_EVENTS 0

static void
ECompMgrHandleWindowEvent(XEvent * ev, void *prm)
{
   EObj               *eo = prm;

   D2printf("ECompMgrHandleWindowEvent: type=%d\n", ev->type);

   switch (ev->type)
     {
#if USE_WINDOW_EVENTS
     case ConfigureNotify:
	ECompMgrWinConfigure(eo, ev);
	break;

     case MapNotify:
	ECompMgrWinMap(eo);
	break;
     case UnmapNotify:
	if (eo->type == EOBJ_TYPE_EXT && eo->cmhook)
	  {
	     ECompMgrWinUnmap(eo);
	     eo->shown = 0;
	  }
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
   Window              xwin;
   EObj               *eo;

   D2printf("ECompMgrHandleRootEvent: type=%d\n", ev->type);

   switch (ev->type)
     {
     case CreateNotify:
	xwin = ev->xcreatewindow.window;
      case_CreateNotify:
	if (!Conf_compmgr.override_redirect.mode != ECM_OR_ON_CREATE)
	   break;
	eo = EobjListStackFind(xwin);
	if (!eo)
	   EobjRegister(xwin, EOBJ_TYPE_EXT);
	break;

     case DestroyNotify:
	xwin = ev->xdestroywindow.window;
      case_DestroyNotify:
	eo = EobjListStackFind(xwin);
	if (eo && eo->type == EOBJ_TYPE_EXT)
	   EobjUnregister(eo);
	break;

     case ReparentNotify:
	xwin = ev->xreparent.window;
	if (ev->xreparent.parent == VRoot.win)
	   goto case_CreateNotify;
	else
	   goto case_DestroyNotify;
	break;

     case ConfigureNotify:
	if (ev->xconfigure.window == VRoot.win)
	  {
	     ECompMgrRootConfigure(prm, ev);
	  }
	else
	  {
	     eo = EobjListStackFind(ev->xconfigure.window);
	     if (eo && eo->type == EOBJ_TYPE_EXT && eo->cmhook)
	       {
		  ECompMgrWinConfigure(eo, ev);
	       }
	  }
	break;

     case MapNotify:
	eo = EobjListStackFind(ev->xmap.window);
	if (!eo && Conf_compmgr.override_redirect.mode)
	   eo = EobjRegister(ev->xmap.window, EOBJ_TYPE_EXT);
	if (eo && eo->type == EOBJ_TYPE_EXT && eo->cmhook)
	  {
	     eo->shown = 1;
	     eo->fade = 1;
	     EobjListStackRaise(eo);
	     ECompMgrWinMap(eo);
	  }
	break;

     case UnmapNotify:
	eo = EobjListStackFind(ev->xunmap.window);
	if (eo && eo->type == EOBJ_TYPE_EXT && eo->cmhook)
	  {
#if 0
	     /* No. Unredirection seems to cause map/unmap => loop */
	     if (Conf_compmgr.override_redirect.mode == ECM_OR_ON_MAPUNMAP)
	       {
		  EobjUnregister(eo);
	       }
	     else
#endif
	       {
		  ECompMgrWinUnmap(eo);
		  eo->shown = 0;
	       }
	  }
	break;

     case CirculateNotify:
	eo = EobjListStackFind(ev->xcirculate.window);
	if (eo && eo->cmhook)
	   ECompMgrWinCirculate(eo, ev);
	break;

     case Expose:
#if 1				/* FIXME - Need this? */
	if (Conf_compmgr.shadow != ECM_SHADOWS_OFF)
	   ECompMgrRootExpose(prm, ev);
#endif
	break;
     }
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

   if (!XCompositeQueryVersion(disp, &major, &minor) ||
       (major == 0 && minor < 2))
     {
	Conf_compmgr.mode = ECM_MODE_OFF;
	goto done;
     }

   Mode_compmgr.use_pixmap = Conf_compmgr.use_name_pixmap;

   if (Conf_compmgr.mode == ECM_MODE_OFF)
      Conf_compmgr.mode = ECM_MODE_ROOT;

   /* FIXME - Hardcode for now. */
   Conf_compmgr.mode = ECM_MODE_WINDOW;

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
	if (Conf_compmgr.enable)
	   ECompMgrStart();
	break;

     case ESIGNAL_BACKGROUND_CHANGE:
	ECompMgrDeskChanged((Desk *) prm);
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
	autosave();
     }
   else if (!strcmp(cmd, "stop"))
     {
	ECompMgrStop();
	autosave();
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
   CFG_ITEM_BOOL(Conf_compmgr, fading.enable, 0),
   CFG_ITEM_INT(Conf_compmgr, fading.dt_us, 10000),
   CFG_ITEM_INT(Conf_compmgr, fading.step, 0x10000000),
   CFG_ITEM_INT(Conf_compmgr, override_redirect.mode, 1),
   CFG_ITEM_INT(Conf_compmgr, override_redirect.opacity, 240),
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
