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

ImageClass         *
CreateIclass()
{
   ImageClass         *i;

   EDBUG(5, "CreateIclass");

   i = Emalloc(sizeof(ImageClass));
   if (!i)
      EDBUG_RETURN(NULL);

   i->name = NULL;
   i->external = 0;
   i->norm.normal = i->norm.hilited = i->norm.clicked = i->norm.disabled = NULL;
   i->active.normal = i->active.hilited = i->active.clicked =
      i->active.disabled = NULL;
   i->sticky.normal = i->sticky.hilited = i->sticky.clicked =
      i->sticky.disabled = NULL;
   i->sticky_active.normal = i->sticky_active.hilited =
      i->sticky_active.clicked = i->sticky_active.disabled = NULL;
   i->padding.left = 0;
   i->padding.right = 0;
   i->padding.top = 0;
   i->padding.bottom = 0;
   i->colmod = NULL;
   i->ref_count = 0;

   EDBUG_RETURN(i);

}

static void
FreeImageState(ImageState * i)
{

   EDBUG(7, "FreeImageState");

   Efree(i->im_file);
   Efree(i->real_file);

   if (i->im)
     {
	imlib_context_set_image(i->im);
	imlib_free_image();
	i->im = NULL;
     }
   if (i->transp)
      Efree(i->transp);
   if (i->border)
      Efree(i->border);

   if (i->colmod)
      i->colmod->ref_count--;

   EDBUG_RETURN_;

}

static void
FreeImageStateArray(ImageStateArray * isa)
{

   EDBUG(6, "FreeImageStateArray");

   FreeImageState(isa->normal);
   Efree(isa->normal);
   FreeImageState(isa->hilited);
   Efree(isa->hilited);
   FreeImageState(isa->clicked);
   Efree(isa->clicked);
   FreeImageState(isa->disabled);
   Efree(isa->disabled);

   EDBUG_RETURN_;

}

void
FreeImageClass(ImageClass * i)
{

   EDBUG(5, "FreeImageClass");

   if (!i)
      EDBUG_RETURN_;

   if (i->ref_count > 0)
     {
	char                stuff[255];

	Esnprintf(stuff, sizeof(stuff), _("%u references remain\n"),
		  i->ref_count);
	DIALOG_OK(_("ImageClass Error!"), stuff);
	EDBUG_RETURN_;
     }
   while (RemoveItemByPtr(i, LIST_TYPE_ICLASS));

   if (i->name)
      Efree(i->name);

   FreeImageStateArray(&(i->norm));
   FreeImageStateArray(&(i->active));
   FreeImageStateArray(&(i->sticky));
   FreeImageStateArray(&(i->sticky_active));

   if (i->colmod)
      i->colmod->ref_count--;

   EDBUG_RETURN_;

}

ImageState         *
CreateImageState()
{
   ImageState         *is;

   EDBUG(6, "CreateImageState");

   is = Emalloc(sizeof(ImageState));
   if (!is)
      EDBUG_RETURN(NULL);

   is->im_file = NULL;
   is->real_file = NULL;
   is->unloadable = 0;
   is->im = NULL;
   is->transp = NULL;
   is->border = NULL;
   is->pixmapfillstyle = FILL_STRETCH;
   ESetColor(&(is->bg), 160, 160, 160);
   ESetColor(&(is->hi), 200, 200, 200);
   ESetColor(&(is->lo), 120, 120, 120);
   ESetColor(&(is->hihi), 255, 255, 255);
   ESetColor(&(is->lolo), 64, 64, 64);
   is->bevelstyle = BEVEL_NONE;
   is->colmod = NULL;

   EDBUG_RETURN(is);

}

void
ImageStatePopulate(ImageState * is)
{
   EDBUG(6, "ImageStatePopulate");

   if (!is)
      EDBUG_RETURN_;

   EAllocColor(&is->bg);
   EAllocColor(&is->hi);
   EAllocColor(&is->lo);
   EAllocColor(&is->hihi);
   EAllocColor(&is->lolo);

   EDBUG_RETURN_;
}

void
IclassPopulate(ImageClass * iclass)
{
   ColorModifierClass *cm;

   EDBUG(6, "IclassPopulate");
   if ((!iclass) || (iclass->external))
      EDBUG_RETURN_;

   if (!iclass->norm.normal)
      EDBUG_RETURN_;

   ImageStatePopulate(iclass->norm.normal);
   if (!iclass->norm.hilited)
     {
	iclass->norm.hilited = iclass->norm.normal;
     }
   else
     {
	ImageStatePopulate(iclass->norm.hilited);
     }
   if (!iclass->norm.clicked)
     {
	iclass->norm.clicked = iclass->norm.normal;
     }
   else
     {
	ImageStatePopulate(iclass->norm.clicked);
     }
   if (!iclass->norm.disabled)
     {
	iclass->norm.disabled = iclass->norm.normal;
     }
   else
     {
	ImageStatePopulate(iclass->norm.disabled);
     }

   if (!iclass->active.normal)
     {
	iclass->active.normal = iclass->norm.normal;
     }
   else
     {
	ImageStatePopulate(iclass->active.normal);
     }
   if (!iclass->active.hilited)
     {
	iclass->active.hilited = iclass->active.normal;
     }
   else
     {
	ImageStatePopulate(iclass->active.hilited);
     }
   if (!iclass->active.clicked)
     {
	iclass->active.clicked = iclass->active.normal;
     }
   else
     {
	ImageStatePopulate(iclass->active.clicked);
     }
   if (!iclass->active.disabled)
     {
	iclass->active.disabled = iclass->active.normal;
     }
   else
     {
	ImageStatePopulate(iclass->active.disabled);
     }

   if (!iclass->sticky.normal)
     {
	iclass->sticky.normal = iclass->norm.normal;
     }
   else
     {
	ImageStatePopulate(iclass->sticky.normal);
     }
   if (!iclass->sticky.hilited)
     {
	iclass->sticky.hilited = iclass->sticky.normal;
     }
   else
     {
	ImageStatePopulate(iclass->sticky.hilited);
     }
   if (!iclass->sticky.clicked)
     {
	iclass->sticky.clicked = iclass->sticky.normal;
     }
   else
     {
	ImageStatePopulate(iclass->sticky.clicked);
     }
   if (!iclass->sticky.disabled)
     {
	iclass->sticky.disabled = iclass->sticky.normal;
     }
   else
     {
	ImageStatePopulate(iclass->sticky.disabled);
     }

   if (!iclass->sticky_active.normal)
     {
	iclass->sticky_active.normal = iclass->norm.normal;
     }
   else
     {
	ImageStatePopulate(iclass->sticky_active.normal);
     }
   if (!iclass->sticky_active.hilited)
     {
	iclass->sticky_active.hilited = iclass->sticky_active.normal;
     }
   else
     {
	ImageStatePopulate(iclass->sticky_active.hilited);
     }
   if (!iclass->sticky_active.clicked)
     {
	iclass->sticky_active.clicked = iclass->sticky_active.normal;
     }
   else
     {
	ImageStatePopulate(iclass->sticky_active.clicked);
     }
   if (!iclass->sticky_active.disabled)
     {
	iclass->sticky_active.disabled = iclass->sticky_active.normal;
     }
   else
     {
	ImageStatePopulate(iclass->sticky_active.disabled);
     }

   if (!iclass->colmod)
     {
	cm = (ColorModifierClass *) FindItem("ICLASS", 0, LIST_FINDBY_NAME,
					     LIST_TYPE_COLORMODIFIER);
	if (!cm)
	   cm = (ColorModifierClass *) FindItem("DEFAULT", 0, LIST_FINDBY_NAME,
						LIST_TYPE_COLORMODIFIER);
	iclass->colmod = cm;
     }
   cm = (ColorModifierClass *) FindItem("NORMAL", 0, LIST_FINDBY_NAME,
					LIST_TYPE_COLORMODIFIER);
   if (!cm)
      cm = iclass->colmod;
   if (!iclass->norm.normal->colmod)
     {
	iclass->norm.normal->colmod = cm;
	if (cm)
	  {
	     cm->ref_count++;
	  }
     }
   if (!iclass->norm.hilited->colmod)
     {
	iclass->norm.hilited->colmod = cm;
	if (cm)
	  {
	     cm->ref_count++;
	  }
     }
   if (!iclass->norm.clicked->colmod)
     {
	iclass->norm.clicked->colmod = cm;
	if (cm)
	  {
	     cm->ref_count++;
	  }
     }
   if (!iclass->norm.disabled->colmod)
     {
	iclass->norm.disabled->colmod = cm;
	if (cm)
	  {
	     cm->ref_count++;
	  }
     }
   cm = (ColorModifierClass *) FindItem("ACTIVE", 0, LIST_FINDBY_NAME,
					LIST_TYPE_COLORMODIFIER);
   if (!cm)
      cm = iclass->colmod;
   if (!iclass->active.normal->colmod)
     {
	iclass->active.normal->colmod = cm;
	if (cm)
	  {
	     cm->ref_count++;
	  }
     }
   if (!iclass->active.hilited->colmod)
     {
	iclass->active.hilited->colmod = cm;
	if (cm)
	  {
	     cm->ref_count++;
	  }
     }
   if (!iclass->active.clicked->colmod)
     {
	iclass->active.clicked->colmod = cm;
	if (cm)
	  {
	     cm->ref_count++;
	  }
     }
   if (!iclass->active.disabled->colmod)
     {
	iclass->active.disabled->colmod = cm;
	if (cm)
	  {
	     cm->ref_count++;
	  }
     }
   cm = (ColorModifierClass *) FindItem("STICKY", 0, LIST_FINDBY_NAME,
					LIST_TYPE_COLORMODIFIER);
   if (!cm)
      cm = iclass->colmod;
   if (!iclass->sticky.normal->colmod)
     {
	iclass->sticky.normal->colmod = cm;
	if (cm)
	  {
	     cm->ref_count++;
	  }
     }
   if (!iclass->sticky.hilited->colmod)
     {
	iclass->sticky.hilited->colmod = cm;
	if (cm)
	  {
	     cm->ref_count++;
	  }
     }
   if (!iclass->sticky.clicked->colmod)
     {
	iclass->sticky.clicked->colmod = cm;
	if (cm)
	  {
	     cm->ref_count++;
	  }
     }
   if (!iclass->sticky.disabled->colmod)
     {
	iclass->sticky.disabled->colmod = cm;
	if (cm)
	  {
	     cm->ref_count++;
	  }
     }
   cm = (ColorModifierClass *) FindItem("STICKY_ACTIVE", 0, LIST_FINDBY_NAME,
					LIST_TYPE_COLORMODIFIER);
   if (!cm)
      cm = iclass->colmod;
   if (!iclass->sticky_active.normal->colmod)
     {
	iclass->sticky_active.normal->colmod = cm;
	if (cm)
	  {
	     cm->ref_count++;
	  }
     }
   if (!iclass->sticky_active.hilited->colmod)
     {
	iclass->sticky_active.hilited->colmod = cm;
	if (cm)
	  {
	     cm->ref_count++;
	  }
     }
   if (!iclass->sticky_active.clicked->colmod)
     {
	iclass->sticky_active.clicked->colmod = cm;
	if (cm)
	  {
	     cm->ref_count++;
	  }
     }
   if (!iclass->sticky_active.disabled->colmod)
     {
	iclass->sticky_active.disabled->colmod = cm;
	if (cm)
	  {
	     cm->ref_count++;
	  }
     }
   EDBUG_RETURN_;

}

static ImageState  *
IclassGetImageState1(ImageStateArray * pisa, int state)
{
   ImageState         *is;

   switch (state)
     {
     case STATE_NORMAL:
	is = pisa->normal;
	break;
     case STATE_HILITED:
	is = pisa->hilited;
	break;
     case STATE_CLICKED:
	is = pisa->clicked;
	break;
     case STATE_DISABLED:
	is = pisa->disabled;
	break;
     default:
	is = NULL;
	break;
     }

   return is;
}

static ImageState  *
IclassGetImageState2(ImageClass * iclass, int state, int active, int sticky)
{
   ImageState         *is;

   if (active)
     {
	if (sticky)
	   is = IclassGetImageState1(&iclass->sticky_active, state);
	else
	   is = IclassGetImageState1(&iclass->active, state);
     }
   else
     {
	if (sticky)
	   is = IclassGetImageState1(&iclass->sticky, state);
	else
	   is = IclassGetImageState1(&iclass->norm, state);
     }

   return is;
}

void
IclassApply(ImageClass * iclass, Window win, int w, int h, int active,
	    int sticky, int state, char expose)
{
   ImageState         *is;
   XGCValues           gcv;
   GC                  gc;
   Pixmap              pmap, mask;

   EDBUG(4, "IclassApply");

   if ((!iclass) || (!win))
      EDBUG_RETURN_;

   if (w < 0)
      GetWinWH(win, (unsigned int *)&w, (unsigned int *)&h);
   if ((w < 0) || (h < 0))
      EDBUG_RETURN_;

   if (queue_up)
     {
	DrawQueue          *dq;

	dq = Emalloc(sizeof(DrawQueue));
	dq->win = win;
	dq->iclass = iclass;
	if (dq->iclass)
	   dq->iclass->ref_count++;
	dq->w = w;
	dq->h = h;
	dq->active = active;
	dq->sticky = sticky;
	dq->state = state;
	dq->expose = expose;
	dq->tclass = NULL;
	dq->text = NULL;
	dq->shape_propagate = 0;
	dq->pager = NULL;
	dq->redraw_pager = NULL;
	dq->d = NULL;
	dq->di = NULL;
	dq->x = 0;
	dq->y = 0;
	AddItem(dq, "DRAW", dq->win, LIST_TYPE_DRAW);
	EDBUG_RETURN_;
     }

   if (iclass->external)
      EDBUG_RETURN_;

   is = IclassGetImageState2(iclass, state, active, sticky);
   if (!is)
      EDBUG_RETURN_;

   imlib_context_set_drawable(win);
   if (!expose)
     {
	if (is->im_file)
	  {
	     /* has bg pixmap */
	     if (!is->im)
		ImageStateRealize(is);

	     if (is->im)
	       {
		  imlib_context_set_image(is->im);

		  /* if image, render */
		  if (is->pixmapfillstyle == FILL_STRETCH)
		    {
		       imlib_render_pixmaps_for_whole_image_at_size(&pmap,
								    &mask, w,
								    h);
		       if (pmap)
			 {
			    ESetWindowBackgroundPixmap(disp, win, pmap);
			    EShapeCombineMask(disp, win, ShapeBounding, 0,
					      0, mask, ShapeSet);
			    imlib_free_pixmap_and_mask(pmap);
			 }
		    }
		  else
		    {
		       int                 cw, ch, pw, ph;
		       Pixmap              tm = 0;
		       GC                  gc;
		       XGCValues           gcv;

		       pw = w;
		       ph = h;
		       if (is->pixmapfillstyle & FILL_TILE_H)
			  pw = imlib_image_get_width();
		       if (is->pixmapfillstyle & FILL_TILE_V)
			  ph = imlib_image_get_height();
		       if (is->pixmapfillstyle & FILL_INT_TILE_H)
			 {
			    cw = w / imlib_image_get_width();
			    if (cw * imlib_image_get_width() < w)
			       cw++;
			    if (cw < 1)
			       cw = 1;
			    pw = w / cw;
			 }
		       if (is->pixmapfillstyle & FILL_INT_TILE_V)
			 {
			    ch = h / imlib_image_get_height();
			    if (ch * imlib_image_get_height() < h)
			       ch++;
			    if (ch < 1)
			       ch = 1;
			    ph = h / ch;
			 }
		       imlib_render_pixmaps_for_whole_image_at_size(&pmap,
								    &mask, pw,
								    ph);
		       if (mask)
			 {
			    gcv.fill_style = FillTiled;
			    gcv.tile = mask;
			    gcv.ts_x_origin = 0;
			    gcv.ts_y_origin = 0;
			    tm = ECreatePixmap(disp, win, w, h, 1);
			    gc = XCreateGC(disp, tm,
					   GCFillStyle | GCTile |
					   GCTileStipXOrigin |
					   GCTileStipYOrigin, &gcv);
			    XFillRectangle(disp, tm, gc, 0, 0, w, h);
			    XFreeGC(disp, gc);
			    EShapeCombineMask(disp, win, ShapeBounding, 0,
					      0, tm, ShapeSet);
			    EFreePixmap(disp, tm);
			 }
		       ESetWindowBackgroundPixmap(disp, win, pmap);
		       imlib_free_pixmap_and_mask(pmap);
		    }
	       }
	  }
	if (!is->im)
	   /* bg color */
	   ESetWindowBackground(disp, win, is->bg.pixel);
	else if (is->im_file)
	  {
	     /* if unloadable - then unload */
	     if ((is->unloadable) || (mode.memory_paranoia))
	       {
		  imlib_free_image();
		  is->im = NULL;
	       }
	  }
     }
   XClearWindow(disp, win);
   /* if there is a bevel to draw, draw it */
   if (is->bevelstyle != BEVEL_NONE)
     {
	gc = XCreateGC(disp, win, 0, &gcv);
	switch (is->bevelstyle)
	  {
	  case BEVEL_AMIGA:
	     XSetForeground(disp, gc, is->hihi.pixel);
	     XDrawLine(disp, win, gc, 0, 0, w - 2, 0);
	     XDrawLine(disp, win, gc, 0, 0, 0, h - 2);
	     XSetForeground(disp, gc, is->lolo.pixel);
	     XDrawLine(disp, win, gc, 1, h - 1, w - 1, h - 1);
	     XDrawLine(disp, win, gc, w - 1, 1, w - 1, h - 1);
	     break;
	  case BEVEL_MOTIF:
	     XSetForeground(disp, gc, is->hi.pixel);
	     XDrawLine(disp, win, gc, 0, 0, w - 1, 0);
	     XDrawLine(disp, win, gc, 0, 0, 0, h - 1);
	     XDrawLine(disp, win, gc, 1, 1, w - 2, 1);
	     XDrawLine(disp, win, gc, 1, 1, 1, h - 2);
	     XSetForeground(disp, gc, is->lo.pixel);
	     XDrawLine(disp, win, gc, 0, h - 1, w - 1, h - 1);
	     XDrawLine(disp, win, gc, w - 1, 1, w - 1, h - 1);
	     XDrawLine(disp, win, gc, 1, h - 2, w - 2, h - 2);
	     XDrawLine(disp, win, gc, w - 2, 2, w - 2, h - 2);
	     break;
	  case BEVEL_NEXT:
	     XSetForeground(disp, gc, is->hihi.pixel);
	     XDrawLine(disp, win, gc, 0, 0, w - 1, 0);
	     XDrawLine(disp, win, gc, 0, 0, 0, h - 1);
	     XSetForeground(disp, gc, is->hi.pixel);
	     XDrawLine(disp, win, gc, 1, 1, w - 2, 1);
	     XDrawLine(disp, win, gc, 1, 1, 1, h - 2);
	     XSetForeground(disp, gc, is->lolo.pixel);
	     XDrawLine(disp, win, gc, 1, h - 1, w - 1, h - 1);
	     XDrawLine(disp, win, gc, w - 1, 1, w - 1, h - 1);
	     XSetForeground(disp, gc, is->lo.pixel);
	     XDrawLine(disp, win, gc, 2, h - 2, w - 2, h - 2);
	     XDrawLine(disp, win, gc, w - 2, 2, w - 2, h - 2);
	     break;
	  case BEVEL_DOUBLE:
	     XSetForeground(disp, gc, is->hi.pixel);
	     XDrawLine(disp, win, gc, 0, 0, w - 2, 0);
	     XDrawLine(disp, win, gc, 0, 0, 0, h - 2);
	     XSetForeground(disp, gc, is->lo.pixel);
	     XDrawLine(disp, win, gc, 1, 1, w - 3, 1);
	     XDrawLine(disp, win, gc, 1, 1, 1, h - 3);
	     XSetForeground(disp, gc, is->lo.pixel);
	     XDrawLine(disp, win, gc, 1, h - 1, w - 1, h - 1);
	     XDrawLine(disp, win, gc, w - 1, 1, w - 1, h - 1);
	     XSetForeground(disp, gc, is->hi.pixel);
	     XDrawLine(disp, win, gc, 2, h - 2, w - 2, h - 2);
	     XDrawLine(disp, win, gc, w - 2, 2, w - 2, h - 2);
	     break;
	  case BEVEL_WIDEDOUBLE:
	     XSetForeground(disp, gc, is->hihi.pixel);
	     XDrawLine(disp, win, gc, 0, 0, w - 1, 0);
	     XDrawLine(disp, win, gc, 0, 0, 0, h - 1);
	     XSetForeground(disp, gc, is->hi.pixel);
	     XDrawLine(disp, win, gc, 1, 1, w - 2, 1);
	     XDrawLine(disp, win, gc, 1, 1, 1, h - 2);
	     XDrawLine(disp, win, gc, 3, h - 4, w - 4, h - 4);
	     XDrawLine(disp, win, gc, w - 4, 3, w - 4, h - 4);
	     XSetForeground(disp, gc, is->lolo.pixel);
	     XDrawLine(disp, win, gc, 1, h - 1, w - 1, h - 1);
	     XDrawLine(disp, win, gc, w - 1, 1, w - 1, h - 1);
	     XSetForeground(disp, gc, is->lo.pixel);
	     XDrawLine(disp, win, gc, 2, h - 2, w - 2, h - 2);
	     XDrawLine(disp, win, gc, w - 2, 2, w - 2, h - 2);
	     XDrawLine(disp, win, gc, 3, 3, w - 4, 3);
	     XDrawLine(disp, win, gc, 3, 3, 3, h - 4);
	     break;
	  case BEVEL_THINPOINT:
	     XSetForeground(disp, gc, is->hi.pixel);
	     XDrawLine(disp, win, gc, 0, 0, w - 2, 0);
	     XDrawLine(disp, win, gc, 0, 0, 0, h - 2);
	     XSetForeground(disp, gc, is->lo.pixel);
	     XDrawLine(disp, win, gc, 1, h - 1, w - 1, h - 1);
	     XDrawLine(disp, win, gc, w - 1, 1, w - 1, h - 1);
	     XSetForeground(disp, gc, is->hihi.pixel);
	     XDrawLine(disp, win, gc, 0, 0, 1, 0);
	     XDrawLine(disp, win, gc, 0, 0, 0, 1);
	     XSetForeground(disp, gc, is->lolo.pixel);
	     XDrawLine(disp, win, gc, w - 2, h - 1, w - 1, h - 1);
	     XDrawLine(disp, win, gc, w - 1, h - 2, w - 1, h - 1);
	     XSync(disp, False);
	     break;
	  case BEVEL_THICKPOINT:
	     XSetForeground(disp, gc, is->hi.pixel);
	     XDrawRectangle(disp, win, gc, 0, 0, w - 1, h - 1);
	     break;
	  default:
	     break;
	  }
	XFreeGC(disp, gc);
     }
   EDBUG_RETURN_;
}

void
IclassApplyCopy(ImageClass * iclass, Window win, int w, int h, int active,
		int sticky, int state, PmapMask * pmm, int make_mask)
{
   ImageState         *is;
   XGCValues           gcv;
   GC                  gc;

   EDBUG(4, "IclassApplyCopy");
   if ((!iclass) || (!win) || (w < 1) || (h < 1) || (!pmm))
      EDBUG_RETURN_;

   pmm->pmap = 0;
   pmm->mask = 0;

   if (iclass->external)
      EDBUG_RETURN_;

   is = IclassGetImageState2(iclass, state, active, sticky);
   if (!is)
      EDBUG_RETURN_;

   imlib_context_set_drawable(win);

   if (is->im_file)
     {
	/* has bg pixmap */
	if (!is->im)
	  {
	     ImageStateRealize(is);
	  }
	if (is->im)
	  {
	     /* if image, render */
	     if (is->pixmapfillstyle == FILL_STRETCH)
	       {
		  pmm->type = 1;
		  imlib_context_set_image(is->im);
		  imlib_render_pixmaps_for_whole_image_at_size(&pmm->pmap,
							       &pmm->mask,
							       w, h);
		  /* if unloadable - then unload */
		  if ((is->unloadable) || (mode.memory_paranoia))
		    {
		       imlib_free_image();
		       is->im = NULL;
		    }
		  EDBUG_RETURN_;
	       }
	     else
	       {
		  int                 cw, ch, pw, ph;
		  Pixmap              pmap, mask, tp = 0, tm = 0;
		  GC                  gc;
		  XGCValues           gcv;

		  imlib_context_set_image(is->im);
		  pw = w;
		  ph = h;
		  if (is->pixmapfillstyle & FILL_TILE_H)
		     pw = imlib_image_get_width();
		  if (is->pixmapfillstyle & FILL_TILE_V)
		     ph = imlib_image_get_height();
		  if (is->pixmapfillstyle & FILL_INT_TILE_H)
		    {
		       cw = w / imlib_image_get_width();
		       if (cw * imlib_image_get_width() < w)
			  cw++;
		       if (cw < 1)
			  cw = 1;
		       pw = w / cw;
		    }
		  if (is->pixmapfillstyle & FILL_INT_TILE_V)
		    {
		       ch = h / imlib_image_get_height();
		       if (ch * imlib_image_get_height() < h)
			  ch++;
		       if (ch < 1)
			  ch = 1;
		       ph = h / ch;
		    }
		  imlib_render_pixmaps_for_whole_image_at_size(&pmap, &mask, pw,
							       ph);
		  tp = ECreatePixmap(disp, win, w, h, root.depth);
		  if ((make_mask) && (mask))
		     tm = ECreatePixmap(disp, win, w, h, 1);
		  gcv.fill_style = FillTiled;
		  gcv.tile = pmap;
		  gcv.ts_x_origin = 0;
		  gcv.ts_y_origin = 0;
		  gc = XCreateGC(disp, tp,
				 GCFillStyle | GCTile | GCTileStipXOrigin |
				 GCTileStipYOrigin, &gcv);
		  XFillRectangle(disp, tp, gc, 0, 0, w, h);
		  XFreeGC(disp, gc);
		  if (tm)
		    {
		       gcv.fill_style = FillTiled;
		       gcv.tile = mask;
		       gcv.ts_x_origin = 0;
		       gcv.ts_y_origin = 0;
		       gc = XCreateGC(disp, tm,
				      GCFillStyle | GCTile |
				      GCTileStipXOrigin |
				      GCTileStipYOrigin, &gcv);
		       XFillRectangle(disp, tm, gc, 0, 0, w, h);
		       XFreeGC(disp, gc);
		    }
		  pmm->type = 0;
		  pmm->pmap = tp;
		  pmm->mask = tm;
		  IMLIB_FREE_PIXMAP_AND_MASK(pmap, mask);
		  /* if unloadable - then unload */
		  if ((is->unloadable) || (mode.memory_paranoia))
		    {
		       imlib_free_image();
		       is->im = NULL;
		    }
		  EDBUG_RETURN_;
	       }
	  }
     }

   /* TBD should we return here ? */

   /* if there is a bevel to draw, draw it */
   if (is->bevelstyle != BEVEL_NONE)
     {
	Pixmap              pmap;

	if (pmm->pmap)
	   printf("IclassApplyCopy: Hmm... pmm->pmap already set\n");

	pmap = ECreatePixmap(disp, win, w, h, root.depth);
	pmm->type = 0;
	pmm->pmap = pmap;
	pmm->mask = 0;

	gc = XCreateGC(disp, pmap, 0, &gcv);
	/* bg color */
	XSetForeground(disp, gc, is->bg.pixel);
	XFillRectangle(disp, pmap, gc, 0, 0, w, h);
	switch (is->bevelstyle)
	  {
	  case BEVEL_AMIGA:
	     XSetForeground(disp, gc, is->hihi.pixel);
	     XDrawLine(disp, pmap, gc, 0, 0, w - 2, 0);
	     XDrawLine(disp, pmap, gc, 0, 0, 0, h - 2);
	     XSetForeground(disp, gc, is->lolo.pixel);
	     XDrawLine(disp, pmap, gc, 1, h - 1, w - 1, h - 1);
	     XDrawLine(disp, pmap, gc, w - 1, 1, w - 1, h - 1);
	     break;
	  case BEVEL_MOTIF:
	     XSetForeground(disp, gc, is->hi.pixel);
	     XDrawLine(disp, pmap, gc, 0, 0, w - 1, 0);
	     XDrawLine(disp, pmap, gc, 0, 0, 0, h - 1);
	     XDrawLine(disp, pmap, gc, 1, 1, w - 2, 1);
	     XDrawLine(disp, pmap, gc, 1, 1, 1, h - 2);
	     XSetForeground(disp, gc, is->lo.pixel);
	     XDrawLine(disp, pmap, gc, 0, h - 1, w - 1, h - 1);
	     XDrawLine(disp, pmap, gc, w - 1, 1, w - 1, h - 1);
	     XDrawLine(disp, pmap, gc, 1, h - 2, w - 2, h - 2);
	     XDrawLine(disp, pmap, gc, w - 2, 2, w - 2, h - 2);
	     break;
	  case BEVEL_NEXT:
	     XSetForeground(disp, gc, is->hihi.pixel);
	     XDrawLine(disp, pmap, gc, 0, 0, w - 1, 0);
	     XDrawLine(disp, pmap, gc, 0, 0, 0, h - 1);
	     XSetForeground(disp, gc, is->hi.pixel);
	     XDrawLine(disp, pmap, gc, 1, 1, w - 2, 1);
	     XDrawLine(disp, pmap, gc, 1, 1, 1, h - 2);
	     XSetForeground(disp, gc, is->lolo.pixel);
	     XDrawLine(disp, pmap, gc, 1, h - 1, w - 1, h - 1);
	     XDrawLine(disp, pmap, gc, w - 1, 1, w - 1, h - 1);
	     XSetForeground(disp, gc, is->lo.pixel);
	     XDrawLine(disp, pmap, gc, 2, h - 2, w - 2, h - 2);
	     XDrawLine(disp, pmap, gc, w - 2, 2, w - 2, h - 2);
	     break;
	  case BEVEL_DOUBLE:
	     XSetForeground(disp, gc, is->hi.pixel);
	     XDrawLine(disp, pmap, gc, 0, 0, w - 2, 0);
	     XDrawLine(disp, pmap, gc, 0, 0, 0, h - 2);
	     XSetForeground(disp, gc, is->lo.pixel);
	     XDrawLine(disp, pmap, gc, 1, 1, w - 3, 1);
	     XDrawLine(disp, pmap, gc, 1, 1, 1, h - 3);
	     XSetForeground(disp, gc, is->lo.pixel);
	     XDrawLine(disp, pmap, gc, 1, h - 1, w - 1, h - 1);
	     XDrawLine(disp, pmap, gc, w - 1, 1, w - 1, h - 1);
	     XSetForeground(disp, gc, is->hi.pixel);
	     XDrawLine(disp, pmap, gc, 2, h - 2, w - 2, h - 2);
	     XDrawLine(disp, pmap, gc, w - 2, 2, w - 2, h - 2);
	     break;
	  case BEVEL_WIDEDOUBLE:
	     XSetForeground(disp, gc, is->hihi.pixel);
	     XDrawLine(disp, pmap, gc, 0, 0, w - 1, 0);
	     XDrawLine(disp, pmap, gc, 0, 0, 0, h - 1);
	     XSetForeground(disp, gc, is->hi.pixel);
	     XDrawLine(disp, pmap, gc, 1, 1, w - 2, 1);
	     XDrawLine(disp, pmap, gc, 1, 1, 1, h - 2);
	     XDrawLine(disp, pmap, gc, 3, h - 4, w - 4, h - 4);
	     XDrawLine(disp, pmap, gc, w - 4, 3, w - 4, h - 4);
	     XSetForeground(disp, gc, is->lolo.pixel);
	     XDrawLine(disp, pmap, gc, 1, h - 1, w - 1, h - 1);
	     XDrawLine(disp, pmap, gc, w - 1, 1, w - 1, h - 1);
	     XSetForeground(disp, gc, is->lo.pixel);
	     XDrawLine(disp, pmap, gc, 2, h - 2, w - 2, h - 2);
	     XDrawLine(disp, pmap, gc, w - 2, 2, w - 2, h - 2);
	     XDrawLine(disp, pmap, gc, 3, 3, w - 4, 3);
	     XDrawLine(disp, pmap, gc, 3, 3, 3, h - 4);
	     break;
	  case BEVEL_THINPOINT:
	     XSetForeground(disp, gc, is->hi.pixel);
	     XDrawLine(disp, pmap, gc, 0, 0, w - 2, 0);
	     XDrawLine(disp, pmap, gc, 0, 0, 0, h - 2);
	     XSetForeground(disp, gc, is->lo.pixel);
	     XDrawLine(disp, pmap, gc, 1, h - 1, w - 1, h - 1);
	     XDrawLine(disp, pmap, gc, w - 1, 1, w - 1, h - 1);
	     XSetForeground(disp, gc, is->hihi.pixel);
	     XDrawLine(disp, pmap, gc, 0, 0, 1, 0);
	     XDrawLine(disp, pmap, gc, 0, 0, 0, 1);
	     XSetForeground(disp, gc, is->lolo.pixel);
	     XDrawLine(disp, pmap, gc, w - 2, h - 1, w - 1, h - 1);
	     XDrawLine(disp, pmap, gc, w - 1, h - 2, w - 1, h - 1);
	     XSync(disp, False);
	     break;
	  case BEVEL_THICKPOINT:
	     XSetForeground(disp, gc, is->hi.pixel);
	     XDrawRectangle(disp, pmap, gc, 0, 0, w - 1, h - 1);
	     break;
	  default:
	     break;
	  }
	XFreeGC(disp, gc);
     }

   EDBUG_RETURN_;
}

void
ImageStateRealize(ImageState * is)
{
   if (is == NULL || is->im_file == NULL)
      return;

   /* has bg pixmap */
   if (is->im)
      return;

   /* not loaded, load and setup */
   if (!is->real_file)
      is->real_file = FindFile(is->im_file);

   is->im = ELoadImage(is->real_file);
   imlib_context_set_image(is->im);
   if (is->im == NULL)
      printf
	 ("ImageStateRealize: Hmmm... is->im is NULL (im_file=%s real_file=%s\n",
	  is->im_file, is->real_file);

   if (is->border)
      imlib_image_set_border(is->border);

#if !USE_IMLIB2
   if (is->transp)
      Imlib_set_image_shape(pImlib_Context, is->im, is->transp);

   if (is->colmod)
     {
	Imlib_set_image_red_curve(pImlib_Context, is->im, is->colmod->red.map);
	Imlib_set_image_green_curve(pImlib_Context, is->im,
				    is->colmod->green.map);
	Imlib_set_image_blue_curve(pImlib_Context, is->im,
				   is->colmod->blue.map);
     }
#endif
}

/*
 */
void
FreePmapMask(PmapMask * pmm)
{

   /* type !=0: Created by imlib_render_pixmaps_for_whole_image... */
   if (pmm->pmap)
     {
	if (pmm->type == 0)
	   EFreePixmap(disp, pmm->pmap);
	else
	   imlib_free_pixmap_and_mask(pmm->pmap);
	pmm->pmap = 0;
     }

   if (pmm->mask)
     {
	if (pmm->type == 0)
	   EFreePixmap(disp, pmm->mask);
#if !USE_IMLIB2
	else
	   imlib_free_pixmap_and_mask(pmm->mask);
#endif
	pmm->mask = 0;
     }
}
