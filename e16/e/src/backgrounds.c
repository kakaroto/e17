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
#define DECLARE_STRUCT_BACKGROUND
#include "E.h"
#include <time.h>

char               *
BackgroundGetUniqueString(Background * bg)
{
   char                s[256];
   const char         *chmap =
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_";
   int                 r, g, b;
   int                 n1, n2, n3, n4, n5, f1, f2, f3, f4, f5, f6;

   EGetColor(&(bg->bg_solid), &r, &g, &b);
   n1 = (r << 24) | (g << 16) | (b << 8) | (bg->bg_tile << 7)
      | (bg->bg.keep_aspect << 6) | (bg->top.keep_aspect << 5);
   n2 = (bg->bg.xjust << 16) | (bg->bg.yjust);
   n3 = (bg->bg.xperc << 16) | (bg->bg.yperc);
   n4 = (bg->top.xjust << 16) | (bg->top.yjust);
   n5 = (bg->top.xperc << 16) | (bg->top.yperc);
   f1 = 0;
   f2 = 0;
   f3 = 0;
   f4 = 0;
   f5 = 0;
   f6 = 0;
   if (bg->bg.file)
     {
	char               *f;

	f = FindFile(bg->bg.file);
	if (f)
	  {
	     f1 = fileinode(f);
	     f2 = filedev(f);
	     f3 = (int)moddate(f);
	     Efree(f);
	  }
     }
   if (bg->top.file)
     {
	char               *f;

	f = FindFile(bg->top.file);
	if (f)
	  {
	     f4 = fileinode(f);
	     f5 = filedev(f);
	     f6 = (int)moddate(f);
	     Efree(f);
	  }
     }
   Esnprintf(s, sizeof(s),
	     "%c%c%c%c%c%c" "%c%c%c%c%c%c" "%c%c%c%c%c%c" "%c%c%c%c%c%c"
	     "%c%c%c%c%c%c" "%c%c%c%c%c%c" "%c%c%c%c%c%c" "%c%c%c%c%c%c"
	     "%c%c%c%c%c%c" "%c%c%c%c%c%c" "%c%c%c%c%c%c",
	     chmap[(n1 >> 0) & 0x3f], chmap[(n1 >> 6) & 0x3f],
	     chmap[(n1 >> 12) & 0x3f], chmap[(n1 >> 18) & 0x3f],
	     chmap[(n1 >> 24) & 0x3f], chmap[(n1 >> 28) & 0x3f],
	     chmap[(n2 >> 0) & 0x3f], chmap[(n2 >> 6) & 0x3f],
	     chmap[(n2 >> 12) & 0x3f], chmap[(n2 >> 18) & 0x3f],
	     chmap[(n2 >> 24) & 0x3f], chmap[(n2 >> 28) & 0x3f],
	     chmap[(n3 >> 0) & 0x3f], chmap[(n3 >> 6) & 0x3f],
	     chmap[(n3 >> 12) & 0x3f], chmap[(n3 >> 18) & 0x3f],
	     chmap[(n3 >> 24) & 0x3f], chmap[(n3 >> 28) & 0x3f],
	     chmap[(n4 >> 0) & 0x3f], chmap[(n4 >> 6) & 0x3f],
	     chmap[(n4 >> 12) & 0x3f], chmap[(n4 >> 18) & 0x3f],
	     chmap[(n4 >> 24) & 0x3f], chmap[(n4 >> 28) & 0x3f],
	     chmap[(n5 >> 0) & 0x3f], chmap[(n5 >> 6) & 0x3f],
	     chmap[(n5 >> 12) & 0x3f], chmap[(n5 >> 18) & 0x3f],
	     chmap[(n5 >> 24) & 0x3f], chmap[(n5 >> 28) & 0x3f],
	     chmap[(f1 >> 0) & 0x3f], chmap[(f1 >> 6) & 0x3f],
	     chmap[(f1 >> 12) & 0x3f], chmap[(f1 >> 18) & 0x3f],
	     chmap[(f1 >> 24) & 0x3f], chmap[(f1 >> 28) & 0x3f],
	     chmap[(f2 >> 0) & 0x3f], chmap[(f2 >> 6) & 0x3f],
	     chmap[(f2 >> 12) & 0x3f], chmap[(f2 >> 18) & 0x3f],
	     chmap[(f2 >> 24) & 0x3f], chmap[(f2 >> 28) & 0x3f],
	     chmap[(f3 >> 0) & 0x3f], chmap[(f3 >> 6) & 0x3f],
	     chmap[(f3 >> 12) & 0x3f], chmap[(f3 >> 18) & 0x3f],
	     chmap[(f3 >> 24) & 0x3f], chmap[(f3 >> 28) & 0x3f],
	     chmap[(f4 >> 0) & 0x3f], chmap[(f4 >> 6) & 0x3f],
	     chmap[(f4 >> 12) & 0x3f], chmap[(f4 >> 18) & 0x3f],
	     chmap[(f4 >> 24) & 0x3f], chmap[(f4 >> 28) & 0x3f],
	     chmap[(f5 >> 0) & 0x3f], chmap[(f5 >> 6) & 0x3f],
	     chmap[(f5 >> 12) & 0x3f], chmap[(f5 >> 18) & 0x3f],
	     chmap[(f5 >> 24) & 0x3f], chmap[(f5 >> 28) & 0x3f],
	     chmap[(f6 >> 0) & 0x3f], chmap[(f6 >> 6) & 0x3f],
	     chmap[(f6 >> 12) & 0x3f], chmap[(f6 >> 18) & 0x3f],
	     chmap[(f6 >> 24) & 0x3f], chmap[(f6 >> 28) & 0x3f]);
   return Estrdup(s);
}

void
BackgroundPixmapFree(Background * bg)
{
   if (bg && bg->pmap)
     {
	imlib_free_pixmap_and_mask(bg->pmap);
	bg->pmap = 0;
     }
}

void
BackgroundImagesFree(Background * bg, int free_pmap)
{
   if (bg->bg.im)
     {
	imlib_context_set_image(bg->bg.im);
	imlib_free_image();
	bg->bg.im = NULL;
     }
   if (bg->top.im)
     {
	imlib_context_set_image(bg->top.im);
	imlib_free_image();
	bg->top.im = NULL;
     }
   if (free_pmap && bg->pmap)
      BackgroundPixmapFree(bg);
}

void
BackgroundImagesKeep(Background * bg, int onoff)
{
   if (onoff)
     {
	bg->keepim = 1;
     }
   else
     {
	bg->keepim = 0;
	BackgroundImagesFree(bg, 0);
     }
}

void
BackgroundImagesRemove(Background * bg)
{
   if (bg->bg.file)
      Efree(bg->bg.file);
   bg->bg.file = NULL;

   if (bg->bg.real_file)
      Efree(bg->bg.real_file);
   bg->bg.real_file = NULL;

   if (bg->top.file)
      Efree(bg->top.file);
   bg->top.file = NULL;

   if (bg->top.real_file)
      Efree(bg->top.real_file);
   bg->top.real_file = NULL;

   BackgroundImagesFree(bg, 1);

   bg->keepim = 0;
}

int
BackgroundDestroy(Background * bg)
{
   EDBUG(6, "BackgroundDestroy");

   if (!bg)
      EDBUG_RETURN(-1);

   if (bg->ref_count > 0)
     {
	DialogOK(_("Background Error!"), _("%u references remain\n"),
		 bg->ref_count);
	EDBUG_RETURN(-1);
     }

   RemoveItem((char *)bg, 0, LIST_FINDBY_POINTER, LIST_TYPE_BACKGROUND);
   BackgroundImagesRemove(bg);

   if (bg->name)
      Efree(bg->name);

   Efree(bg);

   EDBUG_RETURN(0);
}

void
BackgroundDelete(Background * bg)
{
   char               *f;

   EDBUG(6, "BackgroundDelete");

   if (BackgroundDestroy(bg))
      EDBUG_RETURN_;

   /* And delete the actual image files */
   if (bg->bg.file)
     {
	f = FindFile(bg->bg.file);
	if (f)
	  {
	     E_rm(f);
	     Efree(f);
	  }
     }
   if (bg->top.file)
     {
	f = FindFile(bg->top.file);
	if (f)
	  {
	     E_rm(f);
	     Efree(f);
	  }
     }

   EDBUG_RETURN_;
}

Background         *
BackgroundCreate(const char *name, XColor * solid, const char *bgn, char tile,
		 char keep_aspect, int xjust, int yjust, int xperc,
		 int yperc, const char *top, char tkeep_aspect, int txjust,
		 int tyjust, int txperc, int typerc)
{
   Background         *bg;

   EDBUG(6, "BackgroundCreate");

   bg = Emalloc(sizeof(Background));
   if (!bg)
      EDBUG_RETURN(NULL);
   bg->name = Estrdup(name);
   bg->pmap = 0;
   bg->last_viewed = 0;

   ESetColor(&(bg->bg_solid), 160, 160, 160);
   if (solid)
      bg->bg_solid = *solid;
   bg->bg.file = NULL;
   if (bgn)
      bg->bg.file = Estrdup(bgn);
   bg->bg.real_file = NULL;
   bg->bg.im = NULL;
   bg->bg_tile = tile;
   bg->bg.keep_aspect = keep_aspect;
   bg->bg.xjust = xjust;
   bg->bg.yjust = yjust;
   bg->bg.xperc = xperc;
   bg->bg.yperc = yperc;

   bg->top.file = NULL;
   if (top)
      bg->top.file = Estrdup(top);
   bg->top.real_file = NULL;
   bg->top.im = NULL;
   bg->top.keep_aspect = tkeep_aspect;
   bg->top.xjust = txjust;
   bg->top.yjust = tyjust;
   bg->top.xperc = txperc;
   bg->top.yperc = typerc;

   bg->cmclass = NULL;
   bg->keepim = 0;
   bg->ref_count = 0;

   AddItem(bg, bg->name, 0, LIST_TYPE_BACKGROUND);

   EDBUG_RETURN(bg);
}

void
BackgroundDestroyByName(const char *name)
{
   BackgroundDestroy(FindItem(name, 0, LIST_FINDBY_NAME, LIST_TYPE_BACKGROUND));
}

static void
BgFindImageSize(BgPart * bgp, int rw, int rh, int *pw, int *ph, int setbg)
{
   int                 w, h;

   if (bgp->xperc > 0)
     {
	w = (rw * bgp->xperc) >> 10;
     }
   else
     {
	if (!setbg)
	   w = (imlib_image_get_width() * rw) / root.w;
	else
	   w = imlib_image_get_width();
     }

   if (bgp->yperc > 0)
     {
	h = (rh * bgp->yperc) >> 10;
     }
   else
     {
	if (!setbg)
	  {
	     h = (imlib_image_get_height() * rh) / root.h;
	  }
	else
	  {
	     h = imlib_image_get_height();
	  }
     }

   if (w <= 0)
      w = 1;
   if (h <= 0)
      h = 1;

   if (bgp->keep_aspect)
     {
	if (bgp->yperc <= 0)
	  {
	     if (((w << 10) / h) !=
		 ((imlib_image_get_width() << 10) / imlib_image_get_height()))
		h = ((w * imlib_image_get_height()) / imlib_image_get_width());
	  }
	else
	  {
	     if (((h << 10) / w) !=
		 ((imlib_image_get_height() << 10) / imlib_image_get_width()))
		w = ((h * imlib_image_get_width()) / imlib_image_get_height());
	  }
     }

   *pw = w;
   *ph = h;
}

void
BackgroundApply(Background * bg, Window win, int setbg)
{
   unsigned int        rw, rh;
   Pixmap              dpmap;
   GC                  gc;
   XGCValues           gcv;
   int                 rt, depth;

   EDBUG(4, "BackgroundApply");

   if (!WinExists(win))
      EDBUG_RETURN_;

   IMLIB1_SET_CONTEXT(win == root.win);

   GetWinWH(win, &rw, &rh);
   depth = GetWinDepth(win);
   imlib_context_set_drawable(win);

   EAllocColor(&bg->bg_solid);
   gc = 0;
   rt = imlib_context_get_dither();

   if (Conf.backgrounds.hiquality)
     {
	imlib_context_set_dither(1);
#if 0				/* ??? */
	imlib_context_set_anti_alias(1);
#endif
     }

   dpmap = bg->pmap;
   if (!setbg && dpmap)
     {
	/* Always regenerate if setting non-desktop window (?) */
	imlib_free_pixmap_and_mask(dpmap);
	dpmap = 0;
     }

   if (!dpmap)
     {
	unsigned int        w, h, x, y;
	char                hasbg, hasfg;
	Pixmap              pmap, mask;
	ColorModifierClass *cm;

	if (bg->bg.file && !bg->bg.im)
	  {
	     if (!bg->bg.real_file)
		bg->bg.real_file = FindFile(bg->bg.file);
	     bg->bg.im = ELoadImage(bg->bg.real_file);
	  }

	if (bg->top.file && !bg->top.im)
	  {
	     if (!bg->top.real_file)
		bg->top.real_file = FindFile(bg->top.file);
	     bg->top.im = ELoadImage(bg->top.real_file);
	  }

	cm = bg->cmclass;
	if (cm)
	   cm->ref_count--;
	else
	   cm = (ColorModifierClass *) FindItem("BACKGROUND", 0,
						LIST_FINDBY_NAME,
						LIST_TYPE_COLORMODIFIER);

	if (cm)
	  {
	     cm->ref_count++;
#if !USE_IMLIB2
	     if (bg->top.im)
	       {
		  Imlib_set_image_red_curve(pImlib_Context, bg->top.im,
					    cm->red.map);
		  Imlib_set_image_green_curve(pImlib_Context, bg->top.im,
					      cm->green.map);
		  Imlib_set_image_blue_curve(pImlib_Context, bg->top.im,
					     cm->blue.map);
	       }
	     if (bg->bg.im)
	       {
		  Imlib_set_image_red_curve(pImlib_Context, bg->bg.im,
					    cm->red.map);
		  Imlib_set_image_green_curve(pImlib_Context, bg->bg.im,
					      cm->green.map);
		  Imlib_set_image_blue_curve(pImlib_Context, bg->bg.im,
					     cm->blue.map);
	       }
#endif
	  }

	hasbg = hasfg = 0;
	if (bg->top.im)
	   hasfg = 1;
	if (bg->bg.im)
	   hasbg = 1;

	w = h = x = y = 0;

	if (hasbg)
	  {
	     imlib_context_set_image(bg->bg.im);

	     BgFindImageSize(&(bg->bg), rw, rh, &w, &h, setbg);
	     x = ((rw - w) * bg->bg.xjust) >> 10;
	     y = ((rh - h) * bg->bg.yjust) >> 10;

	     imlib_render_pixmaps_for_whole_image_at_size(&pmap, &mask, w, h);
	  }

	if (hasbg && !hasfg && setbg && x == 0 && y == 0 && w == rw && h == rh)
	  {
	     /* Put image 1:1 onto the current root window */
	     dpmap = pmap;
	  }
	else if (hasbg && !hasfg && bg->bg_tile && !Conf.theme.transparency)
	  {
	     /* BG only, tiled */
	     dpmap = ECreatePixmap(disp, win, w, h, depth);
	     gc = XCreateGC(disp, dpmap, 0, &gcv);
	  }
	else
	  {
	     /* The rest that require some more work */
	     dpmap = ECreatePixmap(disp, win, rw, rh, depth);
	     gc = XCreateGC(disp, dpmap, 0, &gcv);
	     if (!bg->bg_tile)
	       {
		  XSetForeground(disp, gc, bg->bg_solid.pixel);
		  XFillRectangle(disp, dpmap, gc, 0, 0, rw, rh);
	       }
	  }

	if (hasbg && dpmap != pmap)
	  {
	     XSetTile(disp, gc, pmap);
	     XSetTSOrigin(disp, gc, x, y);
	     XSetFillStyle(disp, gc, FillTiled);
	     if (bg->bg_tile)
		XFillRectangle(disp, dpmap, gc, 0, 0, rw, rh);
	     else
		XFillRectangle(disp, dpmap, gc, x, y, w, h);
	     IMLIB_FREE_PIXMAP_AND_MASK(pmap, mask);
	  }

	if (hasfg)
	  {
	     int                 ww, hh;

	     imlib_context_set_image(bg->top.im);

	     BgFindImageSize(&(bg->top), rw, rh, &ww, &hh, setbg);
	     x = ((rw - ww) * bg->top.xjust) >> 10;
	     y = ((rh - hh) * bg->top.yjust) >> 10;

	     imlib_render_pixmaps_for_whole_image_at_size(&pmap, &mask, ww, hh);
	     XSetTile(disp, gc, pmap);
	     XSetTSOrigin(disp, gc, x, y);
	     XSetFillStyle(disp, gc, FillTiled);
	     if (mask)
	       {
		  XSetClipMask(disp, gc, mask);
		  XSetClipOrigin(disp, gc, x, y);
	       }
	     XFillRectangle(disp, dpmap, gc, x, y, ww, hh);
	     IMLIB_FREE_PIXMAP_AND_MASK(pmap, mask);
	  }

	if (!bg->keepim)
	   BackgroundImagesFree(bg, 0);
     }

   if (setbg)
     {
	if (dpmap)
	  {
	     HintsSetRootInfo(win, dpmap, 0);
	     XSetWindowBackgroundPixmap(disp, win, dpmap);
	  }
	else
	  {
	     HintsSetRootInfo(win, 0, bg->bg_solid.pixel);
	     XSetWindowBackground(disp, win, bg->bg_solid.pixel);
	  }
	XClearWindow(disp, win);
     }
   else
     {
	if (dpmap)
	  {
	     if (!gc)
		gc = XCreateGC(disp, dpmap, 0, &gcv);
	     XSetClipMask(disp, gc, 0);
	     XSetTile(disp, gc, dpmap);
	     XSetTSOrigin(disp, gc, 0, 0);
	     XSetFillStyle(disp, gc, FillTiled);
	     XFillRectangle(disp, win, gc, 0, 0, rw, rh);
	     imlib_free_pixmap_and_mask(dpmap);
	     dpmap = 0;
	  }
	else
	  {
	     if (!gc)
		gc = XCreateGC(disp, win, 0, &gcv);
	     XSetClipMask(disp, gc, 0);
	     XSetFillStyle(disp, gc, FillSolid);
	     XSetForeground(disp, gc, bg->bg_solid.pixel);
	     XFillRectangle(disp, win, gc, 0, 0, rw, rh);
	  }
	XSync(disp, False);
     }
   bg->pmap = dpmap;

   if (gc)
      XFreeGC(disp, gc);

   imlib_context_set_dither(rt);

   IMLIB1_SET_CONTEXT(0);

   EDBUG_RETURN_;
}

void
BackgroundIncRefcount(Background * bg)
{
   if (bg == NULL)
      return;
   bg->ref_count++;
}

void
BackgroundDecRefcount(Background * bg)
{
   if (bg == NULL)
      return;
   bg->ref_count--;
   if (bg->ref_count <= 0)
      bg->last_viewed = 0;	/* Clean out asap */
}

void
BackgroundTouch(Background * bg)
{
   if (bg == NULL)
      return;
   bg->last_viewed = time(NULL);
}

const char         *
BackgroundGetName(const Background * bg)
{
   return (bg) ? bg->name : NULL;
}

Pixmap
BackgroundGetPixmap(const Background * bg)
{
   return (bg) ? bg->pmap : None;
}

void
BackgroundsAccounting(void)
{
   time_t              now;
   int                 i, j, num;
   Background        **lst;
   Window              win;

   EDBUG(3, "BackgroundsAccounting");

   now = time(NULL);

   for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
     {
	if ((desks.desk[i].bg) && (desks.desk[i].viewable))
	   desks.desk[i].bg->last_viewed = now;
     }

   lst = (Background **) ListItemType(&num, LIST_TYPE_BACKGROUND);
   for (i = 0; i < num; i++)
     {
	/* Skip if no pixmap or not timed out */
	if ((lst[i]->pmap == 0) ||
	    ((now - lst[i]->last_viewed) <= Conf.backgrounds.timeout))
	   continue;

	/* Skip if associated with any viewable desktop */
	for (j = 0; j < ENLIGHTENMENT_CONF_NUM_DESKTOPS; j++)
	   if (lst[i] == desks.desk[j].bg && desks.desk[j].viewable)
	      goto next;

	for (j = 0; j < ENLIGHTENMENT_CONF_NUM_DESKTOPS; j++)
	  {
	     if (lst[i] != desks.desk[j].bg || desks.desk[j].viewable)
		continue;

	     /* Unviewable desktop - update the virtual root hints */
	     win = desks.desk[j].win;
	     HintsSetRootInfo(win, 0, 0);
	     XSetWindowBackground(disp, win, 0);
	     XClearWindow(disp, win);
	  }

	IMLIB1_SET_CONTEXT(lst[i] == desks.desk[0].bg);
	BackgroundPixmapFree(lst[i]);

      next:
	;
     }
   if (lst)
      Efree(lst);
   IMLIB1_SET_CONTEXT(0);

   EDBUG_RETURN_;
}

static void
BackgroundsTimeout(int val __UNUSED__, void *data __UNUSED__)
{
   EDBUG(5, "BackgroundsTimeout");
   BackgroundsAccounting();
/* RemoveTimerEvent("BACKGROUND_ACCOUNTING_TIMEOUT"); */
   DoIn("BACKGROUND_ACCOUNTING_TIMEOUT", 30.0, BackgroundsTimeout, 0, NULL);
   EDBUG_RETURN_;
}

void
BackgroundsInit(void)
{
   DoIn("BACKGROUND_ACCOUNTING_TIMEOUT", 30.0, BackgroundsTimeout, 0, NULL);
}
