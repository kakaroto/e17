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
#include "conf.h"

ToolTip            *
CreateToolTip(char *name, ImageClass * ic0, ImageClass * ic1,
	      ImageClass * ic2, ImageClass * ic3, ImageClass * ic4,
	      TextClass * tclass, int dist, ImageClass * tooltippic)
{
   int                 i;
   ToolTip            *tt;

   EDBUG(5, "CreateToolTip");

   if (ic0 == NULL || ic1 == NULL || ic2 == NULL || ic3 == NULL || ic4 == NULL
       || tclass == NULL)
      EDBUG_RETURN((ToolTip *) NULL);

   tt = Emalloc(sizeof(ToolTip));
   tt->name = Estrdup(name);
   tt->iclass = ic0;
   if (ic0)
      ic0->ref_count++;
   tt->s_iclass[0] = ic1;
   tt->s_iclass[1] = ic2;
   tt->s_iclass[2] = ic3;
   tt->s_iclass[3] = ic4;
   tt->tclass = tclass;
   if (tclass)
      tclass->ref_count++;
   tt->tooltippic = tooltippic;
   if (tooltippic)
      tooltippic->ref_count++;

   tt->dist = dist;
   tt->win = ECreateWindow(root.win, -10, -100, 1, 1, 1);
   tt->iwin = ECreateWindow(tt->win, -10, -100, 1, 1, 1);

   for (i = 0; i < 4; i++)
     {
	Window              win;

	win = 0;
	if (tt->s_iclass[i])
	  {
	     int                 wh = (i + 1) * 8;

	     win = ECreateWindow(root.win, -10, -100, wh, wh, 1);
	     tt->s_iclass[i]->ref_count++;
	  }
	tt->s_win[i] = win;
     }

   tt->visible = 0;
   tt->ref_count = 0;

   EDBUG_RETURN(tt);
}

void
ShowToolTip(ToolTip * tt, char *text, ActionClass * ac, int x, int y)
{
   int                 i, w = 0, h = 0, ix, iy, iw, ih, dx, dy, xx, yy;
   int                 ww, hh, adx, ady, dist;
   int                 headline_h = 0, headline_w = 0, icons_width =
      0, labels_width = 0, double_w = 0, temp_w, temp_h;
   Imlib_Image        *im;
   char                pq;
   int                *heights = NULL;
   ImageClass         *ic = NULL;
   int                 cols[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
   Pixmap              pmap, mask;

   EDBUG(5, "ShowToolTip");
   if (!tt)
      EDBUG_RETURN_;

   pq = queue_up;
   queue_up = 0;

   /* if we get an actionclass, look for tooltip action texts */
   if (ac)
     {
	heights = Emalloc(ac->num * sizeof(int));

	for (i = 0; i < ac->num; i++)
	  {
	     temp_w = 0;
	     temp_h = 0;

	     if (ac->list[i]->tooltipstring)
	       {
		  TextSize(tt->tclass, 0, 0, STATE_NORMAL,
			   ac->list[i]->tooltipstring, &temp_w, &temp_h, 17);
		  if (temp_w > labels_width)
		     labels_width = temp_w;
		  temp_w = 0;

		  if (ac->list[i]->event == EVENT_DOUBLE_DOWN)
		    {
		       TextSize(tt->tclass, 0, 0, STATE_NORMAL, "2x", &double_w,
				&temp_h, 17);
		       if (cols[0] < double_w)
			  cols[0] = double_w;
		    }

		  if (ac->list[i]->anybutton)
		    {
		       ic = FindItem("TOOLTIP_MOUSEBUTTON_ANY", 0,
				     LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		       if (!ic)
			 {
			    ic = CreateIclass();
			    ic->norm.normal = CreateImageState();
			    ic->norm.normal->im_file = "pix/mouse_any.png";
			    IclassPopulate(ic);
			    AddItem(ic, "TOOLTIP_MOUSEBUTTON_ANY", 0,
				    LIST_TYPE_ICLASS);
			 }
		       if (ic->norm.normal)
			  ImageStateRealize(ic->norm.normal);
		    }
		  else
		     switch (ac->list[i]->button)
		       {
		       case 1:
			  ic = FindItem("TOOLTIP_MOUSEBUTTON_1", 0,
					LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
			  if (!ic)
			    {
			       ic = CreateIclass();
			       ic->norm.normal = CreateImageState();
			       ic->norm.normal->im_file = "pix/mouse_1.png";
			       IclassPopulate(ic);
			       AddItem(ic, "TOOLTIP_MOUSEBUTTON_1", 0,
				       LIST_TYPE_ICLASS);
			    }
			  if (ic->norm.normal)
			     ImageStateRealize(ic->norm.normal);
			  break;
		       case 2:
			  ic = FindItem("TOOLTIP_MOUSEBUTTON_2", 0,
					LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
			  if (!ic)
			    {
			       ic = CreateIclass();
			       ic->norm.normal = CreateImageState();
			       ic->norm.normal->im_file = "pix/mouse_2.png";
			       IclassPopulate(ic);
			       AddItem(ic, "TOOLTIP_MOUSEBUTTON_2", 0,
				       LIST_TYPE_ICLASS);
			    }
			  if (ic->norm.normal)
			     ImageStateRealize(ic->norm.normal);
			  break;
		       case 3:
			  ic = FindItem("TOOLTIP_MOUSEBUTTON_3", 0,
					LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
			  if (!ic)
			    {
			       ic = CreateIclass();
			       ic->norm.normal = CreateImageState();
			       ic->norm.normal->im_file = "pix/mouse_3.png";
			       IclassPopulate(ic);
			       AddItem(ic, "TOOLTIP_MOUSEBUTTON_3", 0,
				       LIST_TYPE_ICLASS);
			    }
			  if (ic->norm.normal)
			     ImageStateRealize(ic->norm.normal);
			  break;
		       case 4:
			  ic = FindItem("TOOLTIP_MOUSEBUTTON_4", 0,
					LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
			  if (!ic)
			    {
			       ic = CreateIclass();
			       ic->norm.normal = CreateImageState();
			       ic->norm.normal->im_file = "pix/mouse_4.png";
			       IclassPopulate(ic);
			       AddItem(ic, "TOOLTIP_MOUSEBUTTON_4", 0,
				       LIST_TYPE_ICLASS);
			    }
			  if (ic->norm.normal)
			     ImageStateRealize(ic->norm.normal);
			  break;
		       case 5:
			  ic = FindItem("TOOLTIP_MOUSEBUTTON_5", 0,
					LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
			  if (!ic)
			    {
			       ic = CreateIclass();
			       ic->norm.normal = CreateImageState();
			       ic->norm.normal->im_file = "pix/mouse_5.png";
			       IclassPopulate(ic);
			       AddItem(ic, "TOOLTIP_MOUSEBUTTON_5", 0,
				       LIST_TYPE_ICLASS);
			    }
			  if (ic->norm.normal)
			     ImageStateRealize(ic->norm.normal);
			  break;
		       case 0:
		       default:
			  break;
		       }

		  if (ic)
		    {
		       if (ic->norm.normal->im)
			 {
			    imlib_context_set_image(ic->norm.normal->im);
			    if (cols[1] < imlib_image_get_width())
			       cols[1] = imlib_image_get_width();
			    if (imlib_image_get_height() > temp_h)
			       temp_h = imlib_image_get_height();
			 }
		    }
		  if (ac->list[i]->modifiers)
		    {
		       if (ac->list[i]->modifiers & ShiftMask)
			 {
			    ic = FindItem("TOOLTIP_KEY_SHIFT", 0,
					  LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
			    if (!ic)
			      {
				 ic = CreateIclass();
				 ic->norm.normal = CreateImageState();
				 ic->norm.normal->im_file = "pix/key_shift.png";
				 IclassPopulate(ic);
				 AddItem(ic, "TOOLTIP_KEY_SHIFT", 0,
					 LIST_TYPE_ICLASS);
			      }
			    if (ic->norm.normal)
			       ImageStateRealize(ic->norm.normal);
			    if (ic->norm.normal->im)
			      {
				 imlib_context_set_image(ic->norm.normal->im);
				 if (cols[2] < imlib_image_get_width())
				    cols[2] = imlib_image_get_width();
				 if (imlib_image_get_height() > temp_h)
				    temp_h = imlib_image_get_height();
			      }
			 }
		       if (ac->list[i]->modifiers & LockMask)
			 {
			    ic = FindItem("TOOLTIP_KEY_LOCK", 0,
					  LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
			    if (!ic)
			      {
				 ic = CreateIclass();
				 ic->norm.normal = CreateImageState();
				 ic->norm.normal->im_file = "pix/key_lock.png";
				 IclassPopulate(ic);
				 AddItem(ic, "TOOLTIP_KEY_LOCK", 0,
					 LIST_TYPE_ICLASS);
			      }
			    if (ic->norm.normal)
			       ImageStateRealize(ic->norm.normal);
			    if (ic->norm.normal->im)
			      {
				 imlib_context_set_image(ic->norm.normal->im);
				 if (cols[3] < imlib_image_get_width())
				    cols[3] = imlib_image_get_width();
				 if (imlib_image_get_height() > temp_h)
				    temp_h = imlib_image_get_height();
			      }
			 }
		       if (ac->list[i]->modifiers & ControlMask)
			 {
			    ic = FindItem("TOOLTIP_KEY_CTRL", 0,
					  LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
			    if (!ic)
			      {
				 ic = CreateIclass();
				 ic->norm.normal = CreateImageState();
				 ic->norm.normal->im_file = "pix/key_ctrl.png";
				 IclassPopulate(ic);
				 AddItem(ic, "TOOLTIP_KEY_CTRL", 0,
					 LIST_TYPE_ICLASS);
			      }
			    if (ic->norm.normal)
			       ImageStateRealize(ic->norm.normal);
			    if (ic->norm.normal->im)
			      {
				 imlib_context_set_image(ic->norm.normal->im);
				 if (cols[4] < imlib_image_get_width())
				    cols[4] = imlib_image_get_width();
				 if (imlib_image_get_height() > temp_h)
				    temp_h = imlib_image_get_height();
			      }
			 }
		       if (ac->list[i]->modifiers & Mod1Mask)
			 {
			    ic = FindItem("TOOLTIP_KEY_MOD1", 0,
					  LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
			    if (!ic)
			      {
				 ic = CreateIclass();
				 ic->norm.normal = CreateImageState();
				 ic->norm.normal->im_file = "pix/key_mod1.png";
				 IclassPopulate(ic);
				 AddItem(ic, "TOOLTIP_KEY_MOD1", 0,
					 LIST_TYPE_ICLASS);
			      }
			    if (ic->norm.normal)
			       ImageStateRealize(ic->norm.normal);
			    if (ic->norm.normal->im)
			      {
				 imlib_context_set_image(ic->norm.normal->im);
				 if (cols[5] < imlib_image_get_width())
				    cols[5] = imlib_image_get_width();
				 if (imlib_image_get_height() > temp_h)
				    temp_h = imlib_image_get_height();
			      }
			 }
		       if (ac->list[i]->modifiers & Mod2Mask)
			 {
			    ic = FindItem("TOOLTIP_KEY_MOD2", 0,
					  LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
			    if (!ic)
			      {
				 ic = CreateIclass();
				 ic->norm.normal = CreateImageState();
				 ic->norm.normal->im_file = "pix/key_mod2.png";
				 IclassPopulate(ic);
				 AddItem(ic, "TOOLTIP_KEY_MOD2", 0,
					 LIST_TYPE_ICLASS);
			      }
			    if (ic->norm.normal)
			       ImageStateRealize(ic->norm.normal);
			    if (ic->norm.normal->im)
			      {
				 imlib_context_set_image(ic->norm.normal->im);
				 if (cols[6] < imlib_image_get_width())
				    cols[6] = imlib_image_get_width();
				 if (imlib_image_get_height() > temp_h)
				    temp_h = imlib_image_get_height();
			      }
			 }
		       if (ac->list[i]->modifiers & Mod3Mask)
			 {
			    ic = FindItem("TOOLTIP_KEY_MOD3", 0,
					  LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
			    if (!ic)
			      {
				 ic = CreateIclass();
				 ic->norm.normal = CreateImageState();
				 ic->norm.normal->im_file = "pix/key_mod3.png";
				 IclassPopulate(ic);
				 AddItem(ic, "TOOLTIP_KEY_MOD3", 0,
					 LIST_TYPE_ICLASS);
			      }
			    if (ic->norm.normal)
			       ImageStateRealize(ic->norm.normal);
			    if (ic->norm.normal->im)
			      {
				 imlib_context_set_image(ic->norm.normal->im);
				 if (cols[7] < imlib_image_get_width())
				    cols[7] = imlib_image_get_width();
				 if (imlib_image_get_height() > temp_h)
				    temp_h = imlib_image_get_height();
			      }
			 }
		       if (ac->list[i]->modifiers & Mod4Mask)
			 {
			    ic = FindItem("TOOLTIP_KEY_MOD4", 0,
					  LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
			    if (!ic)
			      {
				 ic = CreateIclass();
				 ic->norm.normal = CreateImageState();
				 ic->norm.normal->im_file = "pix/key_mod4.png";
				 IclassPopulate(ic);
				 AddItem(ic, "TOOLTIP_KEY_MOD4", 0,
					 LIST_TYPE_ICLASS);
			      }
			    if (ic->norm.normal)
			       ImageStateRealize(ic->norm.normal);
			    if (ic->norm.normal->im)
			      {
				 imlib_context_set_image(ic->norm.normal->im);
				 if (cols[8] < imlib_image_get_width())
				    cols[8] = imlib_image_get_width();
				 if (imlib_image_get_height() > temp_h)
				    temp_h = imlib_image_get_height();
			      }
			 }
		       if (ac->list[i]->modifiers & Mod5Mask)
			 {
			    ic = FindItem("TOOLTIP_KEY_MOD5", 0,
					  LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
			    if (!ic)
			      {
				 ic = CreateIclass();
				 ic->norm.normal = CreateImageState();
				 ic->norm.normal->im_file = "pix/key_mod5.png";
				 IclassPopulate(ic);
				 AddItem(ic, "TOOLTIP_KEY_MOD5", 0,
					 LIST_TYPE_ICLASS);
			      }
			    if (ic->norm.normal)
			       ImageStateRealize(ic->norm.normal);
			    if (ic->norm.normal->im)
			      {
				 imlib_context_set_image(ic->norm.normal->im);
				 if (cols[9] < imlib_image_get_width())
				    cols[9] = imlib_image_get_width();
				 if (imlib_image_get_height() > temp_h)
				    temp_h = imlib_image_get_height();
			      }
			 }
		    }

		  temp_w =
		     cols[0] + cols[1] + cols[2] + cols[3] + cols[4] +
		     cols[5] + cols[6] + cols[7] + cols[8] + cols[9];

		  if (temp_w > icons_width)
		     icons_width = temp_w;
		  heights[i] = temp_h;
		  h += temp_h;
	       }
	  }
     }

   TextSize(tt->tclass, 0, 0, STATE_NORMAL, text, &headline_w, &headline_h, 17);
   if (headline_w < icons_width + labels_width)
      w = icons_width + labels_width;
   else
      w = headline_w;
   h += headline_h;
   iw = 0;
   ih = 0;
   if (tt->tooltippic)
     {
	im = ELoadImage(tt->tooltippic->norm.normal->im_file);
	ix = 0;
	iy = 0;
	if (im)
	  {
	     imlib_context_set_image(im);
	     iw = imlib_image_get_width();
	     ih = imlib_image_get_height();
	     imlib_free_image();
	  }
	w += iw;
	if (h < ih)
	   h = ih;
     }
   w += tt->iclass->padding.left + tt->iclass->padding.right;
   h += tt->iclass->padding.top + tt->iclass->padding.bottom;

   if ((tt->tooltippic) && (iw > 0) && (ih > 0))
     {
	ix = tt->iclass->padding.left;
	iy = (h - ih) / 2;
	EMoveResizeWindow(disp, tt->iwin, ix, iy, iw, ih);
	EMapWindow(disp, tt->iwin);
	IclassApply(tt->tooltippic, tt->iwin, iw, ih, 0, 0, STATE_NORMAL, 0);
     }
   else
      EUnmapWindow(disp, tt->iwin);

   dx = x - root.w / 2;
   dy = y - root.h / 2;

   if ((dy == 0) && (dx == 0))
      dy = -1;

   adx = dx;
   if (adx < 0)
      adx = -adx;
   ady = dy;
   if (ady < 0)
      ady = -ady;
   if (adx < ady)
      /*   +-------+   */
      /*   |\#####/|   */
      /*   | \###/ |   */
      /*   |  \#/  |   */
      /*   |  /#\  |   */
      /*   | /###\ |   */
      /*   |/#####\|   */
      /*   +-------+   */
     {
	dist = tt->dist;
	ady = ady / dy;

	yy = y - ((ady * 10 * dist) / 100);
	xx = x - (dist * 10 * dx) / (100 * root.w / 2);
	EMoveWindow(disp, tt->s_win[0], xx - 4, yy - 4);

	yy = y - ((ady * 30 * dist) / 100);
	xx = x - (dist * 30 * dx) / (100 * root.w / 2);
	EMoveWindow(disp, tt->s_win[1], xx - 8, yy - 8);

	yy = y - ((ady * 50 * dist) / 100);
	xx = x - (dist * 50 * dx) / (100 * root.w / 2);
	EMoveWindow(disp, tt->s_win[2], xx - 12, yy - 12);

	yy = y - ((ady * 80 * dist) / 100);
	xx = x - (dist * 80 * dx) / (100 * root.w / 2);
	EMoveWindow(disp, tt->s_win[3], xx - 16, yy - 16);

	yy = y - ((ady * 100 * dist) / 100);
	xx = x - (dist * 100 * dx) / (100 * root.w / 2);
	if (ady < 0)
	  {
	     hh = 0;
	  }
	else
	  {
	     hh = h;
	  }
	ww = (w / 2) + ((dx * w) / (root.w / 2));
     }
   else
      /*   +-------+   */
      /*   |\     /|   */
      /*   |#\   /#|   */
      /*   |##\ /##|   */
      /*   |##/ \##|   */
      /*   |#/   \#|   */
      /*   |/     \|   */
      /*   +-------+   */
     {
	if (dx == 0)
	  {
	     dx = 1;
	     adx = 1;
	  }
	dist = tt->dist;
	adx = adx / dx;
	xx = x - ((adx * 10 * dist) / 100);
	yy = y - (dist * 10 * dy) / (100 * root.h / 2);
	EMoveWindow(disp, tt->s_win[0], xx - 4, yy - 4);
	xx = x - ((adx * 30 * dist) / 100);
	yy = y - (dist * 30 * dy) / (100 * root.h / 2);
	EMoveWindow(disp, tt->s_win[1], xx - 8, yy - 8);
	xx = x - ((adx * 50 * dist) / 100);
	yy = y - (dist * 50 * dy) / (100 * root.h / 2);
	EMoveWindow(disp, tt->s_win[2], xx - 12, yy - 12);
	xx = x - ((adx * 80 * dist) / 100);
	yy = y - (dist * 80 * dy) / (100 * root.h / 2);
	EMoveWindow(disp, tt->s_win[3], xx - 16, yy - 16);
	xx = x - ((adx * 100 * dist) / 100);
	yy = y - (dist * 100 * dy) / (100 * root.h / 2);
	if (adx < 0)
	  {
	     ww = 0;
	  }
	else
	  {
	     ww = w;
	  }
	hh = (h / 2) + ((dy * h) / (root.h / 2));
     }

   EMoveResizeWindow(disp, tt->win, xx - ww, yy - hh, w, h);

   IclassApply(tt->s_iclass[0], tt->s_win[0], 8, 8, 0, 0, STATE_NORMAL, 0);
   IclassApply(tt->s_iclass[1], tt->s_win[1], 16, 16, 0, 0, STATE_NORMAL, 0);
   IclassApply(tt->s_iclass[2], tt->s_win[2], 24, 24, 0, 0, STATE_NORMAL, 0);
   IclassApply(tt->s_iclass[3], tt->s_win[3], 32, 32, 0, 0, STATE_NORMAL, 0);
   if (conf.theme.transparency && tt->iclass->norm.normal->transparent == 0)
      tt->iclass->norm.normal->transparent = 2;
   IclassApply(tt->iclass, tt->win, w, h, 0, 0, STATE_NORMAL, 0);
   EMapRaised(disp, tt->s_win[0]);
   EMapRaised(disp, tt->s_win[1]);
   EMapRaised(disp, tt->s_win[2]);
   EMapRaised(disp, tt->s_win[3]);
   EMapRaised(disp, tt->win);
   XSync(disp, False);

   xx = tt->iclass->padding.left + iw;

   /* draw the ordinary tooltip text */
   TextDraw(tt->tclass, tt->win, 0, 0, STATE_NORMAL, text, xx,
	    tt->iclass->padding.top, headline_w, headline_h, 17, 512);

   /* draw the icons and labels, if any */
   if (ac)
     {
	y = tt->iclass->padding.top + headline_h;
	xx = tt->iclass->padding.left + double_w;

	for (i = 0; i < ac->num; i++)
	  {
	     x = xx + iw;
	     if (!ac->list[i]->tooltipstring)
		continue;

	     if (ac->list[i]->event == EVENT_DOUBLE_DOWN)
	       {
		  TextDraw(tt->tclass, tt->win, 0, 0, STATE_NORMAL, "2x",
			   xx + iw - double_w, y, double_w, heights[i], 17, 0);
	       }

	     if (ac->list[i]->anybutton)
	       {
		  ic = FindItem("TOOLTIP_MOUSEBUTTON_ANY", 0,
				LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		  if (ic)
		     if (ic->norm.normal->im)
		       {
			  imlib_context_set_image(ic->norm.normal->im);
			  imlib_render_pixmaps_for_whole_image(&pmap, &mask);
			  PastePixmap(disp, tt->win, pmap, mask, x, y);
			  x += imlib_image_get_width();
		       }
	       }
	     else
		switch (ac->list[i]->button)
		  {
		  case 1:
		     ic = FindItem("TOOLTIP_MOUSEBUTTON_1", 0,
				   LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		     if (ic)
			if (ic->norm.normal->im)
			  {
			     imlib_context_set_image(ic->norm.normal->im);
			     imlib_render_pixmaps_for_whole_image(&pmap, &mask);
			     PastePixmap(disp, tt->win, pmap, mask, x, y);
			     x += imlib_image_get_width();
			  }
		     break;
		  case 2:
		     ic = FindItem("TOOLTIP_MOUSEBUTTON_2", 0,
				   LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		     if (ic)
			if (ic->norm.normal->im)
			  {
			     imlib_context_set_image(ic->norm.normal->im);
			     imlib_render_pixmaps_for_whole_image(&pmap, &mask);
			     PastePixmap(disp, tt->win, pmap, mask, x, y);
			     x += imlib_image_get_width();
			  }
		     break;
		  case 3:
		     ic = FindItem("TOOLTIP_MOUSEBUTTON_3", 0,
				   LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		     if (ic)
			if (ic->norm.normal->im)
			  {
			     imlib_context_set_image(ic->norm.normal->im);
			     imlib_render_pixmaps_for_whole_image(&pmap, &mask);
			     PastePixmap(disp, tt->win, pmap, mask, x, y);
			     x += imlib_image_get_width();
			  }
		     break;
		  case 4:
		     ic = FindItem("TOOLTIP_MOUSEBUTTON_4", 0,
				   LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		     if (ic)
			if (ic->norm.normal->im)
			  {
			     imlib_context_set_image(ic->norm.normal->im);
			     imlib_render_pixmaps_for_whole_image(&pmap, &mask);
			     PastePixmap(disp, tt->win, pmap, mask, x, y);
			     x += imlib_image_get_width();
			  }
		     break;
		  case 5:
		     ic = FindItem("TOOLTIP_MOUSEBUTTON_5", 0,
				   LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		     if (ic)
			if (ic->norm.normal->im)
			  {
			     imlib_context_set_image(ic->norm.normal->im);
			     imlib_render_pixmaps_for_whole_image(&pmap, &mask);
			     PastePixmap(disp, tt->win, pmap, mask, x, y);
			     x += imlib_image_get_width();
			  }
		     break;
		  default:
		     break;
		  }

	     if (ac->list[i]->modifiers)
	       {
		  if (ac->list[i]->modifiers & ShiftMask)
		    {
		       ic = FindItem("TOOLTIP_KEY_SHIFT", 0,
				     LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		       if (ic)
			  if (ic->norm.normal->im)
			    {
			       imlib_context_set_image(ic->norm.normal->im);
			       imlib_render_pixmaps_for_whole_image(&pmap,
								    &mask);
			       PastePixmap(disp, tt->win, pmap, mask, x, y);
			       x += imlib_image_get_width();
			    }
		    }
		  if (ac->list[i]->modifiers & LockMask)
		    {
		       ic = FindItem("TOOLTIP_KEY_LOCK", 0,
				     LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		       if (ic)
			  if (ic->norm.normal->im)
			    {
			       imlib_context_set_image(ic->norm.normal->im);
			       imlib_render_pixmaps_for_whole_image(&pmap,
								    &mask);
			       PastePixmap(disp, tt->win, pmap, mask, x, y);
			       x += imlib_image_get_width();
			    }
		    }
		  if (ac->list[i]->modifiers & ControlMask)
		    {
		       ic = FindItem("TOOLTIP_KEY_CTRL", 0,
				     LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		       if (ic)
			  if (ic->norm.normal->im)
			    {
			       imlib_context_set_image(ic->norm.normal->im);
			       imlib_render_pixmaps_for_whole_image(&pmap,
								    &mask);
			       PastePixmap(disp, tt->win, pmap, mask, x, y);
			       x += imlib_image_get_width();
			    }
		    }
		  if (ac->list[i]->modifiers & Mod1Mask)
		    {
		       ic = FindItem("TOOLTIP_KEY_MOD1", 0,
				     LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		       if (ic)
			  if (ic->norm.normal->im)
			    {
			       imlib_context_set_image(ic->norm.normal->im);
			       imlib_render_pixmaps_for_whole_image(&pmap,
								    &mask);
			       PastePixmap(disp, tt->win, pmap, mask, x, y);
			       x += imlib_image_get_width();
			    }
		    }
		  if (ac->list[i]->modifiers & Mod2Mask)
		    {
		       ic = FindItem("TOOLTIP_KEY_MOD2", 0,
				     LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		       if (ic)
			  if (ic->norm.normal->im)
			    {
			       imlib_context_set_image(ic->norm.normal->im);
			       imlib_render_pixmaps_for_whole_image(&pmap,
								    &mask);
			       PastePixmap(disp, tt->win, pmap, mask, x, y);
			       x += imlib_image_get_width();
			    }
		    }
		  if (ac->list[i]->modifiers & Mod3Mask)
		    {
		       ic = FindItem("TOOLTIP_KEY_MOD3", 0,
				     LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		       if (ic)
			  if (ic->norm.normal->im)
			    {
			       imlib_context_set_image(ic->norm.normal->im);
			       imlib_render_pixmaps_for_whole_image(&pmap,
								    &mask);
			       PastePixmap(disp, tt->win, pmap, mask, x, y);
			       x += imlib_image_get_width();
			    }
		    }
		  if (ac->list[i]->modifiers & Mod4Mask)
		    {
		       ic = FindItem("TOOLTIP_KEY_MOD4", 0,
				     LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		       if (ic)
			  if (ic->norm.normal->im)
			    {
			       imlib_context_set_image(ic->norm.normal->im);
			       imlib_render_pixmaps_for_whole_image(&pmap,
								    &mask);
			       PastePixmap(disp, tt->win, pmap, mask, x, y);
			       x += imlib_image_get_width();
			    }
		    }
		  if (ac->list[i]->modifiers & Mod5Mask)
		    {
		       ic = FindItem("TOOLTIP_KEY_MOD5", 0,
				     LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		       if (ic)
			  if (ic->norm.normal->im)
			    {
			       imlib_context_set_image(ic->norm.normal->im);
			       imlib_render_pixmaps_for_whole_image(&pmap,
								    &mask);
			       PastePixmap(disp, tt->win, pmap, mask, x, y);
			       x += imlib_image_get_width();
			    }
		    }
	       }

	     TextDraw(tt->tclass, tt->win, 0, 0, STATE_NORMAL,
		      ac->list[i]->tooltipstring,
		      tt->iclass->padding.left + icons_width + iw, y,
		      labels_width, heights[i], 17, 0);
	     y += heights[i];

	  }
     }

   queue_up = pq;
   tt->visible = 1;
   if (heights)
      Efree(heights);
   EDBUG_RETURN_;
}

void
HideToolTip(ToolTip * tt)
{
   EDBUG(5, "HideToolTip");

   if (!tt)
      EDBUG_RETURN_;

   if (!tt->visible)
      EDBUG_RETURN_;

   tt->visible = 0;
   EUnmapWindow(disp, tt->win);
   EUnmapWindow(disp, tt->s_win[0]);
   EUnmapWindow(disp, tt->s_win[1]);
   EUnmapWindow(disp, tt->s_win[2]);
   EUnmapWindow(disp, tt->s_win[3]);
   XSync(disp, False);

   EDBUG_RETURN_;
}

void
FreeToolTip(ToolTip * tt)
{
   EDBUG(5, "FreeToolTip");

   if (!tt)
      EDBUG_RETURN_;

   if (tt->ref_count > 0)
     {
	DialogOK(_("ToolTip Error!"), _("%u references remain\n"),
		 tt->ref_count);
     }

   EDBUG_RETURN_;
}
