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

static struct
{
   char                enable;
   char                showroottooltip;
   int                 delay;	/* milliseconds */
   unsigned int        opacity;
} Conf_tooltips;

struct _tooltip
{
   char               *name;

   ImageClass         *iclass;
   ImageClass         *s_iclass[4];
   TextClass          *tclass;
   int                 dist;
   Window              win;
   Window              iwin;
   Window              s_win[4];
   char                visible;
   ImageClass         *tooltippic;
   unsigned int        ref_count;
};

static ToolTip     *
TooltipCreate(const char *name, ImageClass * ic0, ImageClass * ic1,
	      ImageClass * ic2, ImageClass * ic3, ImageClass * ic4,
	      TextClass * tclass, int dist, ImageClass * tooltippic)
{
   int                 i;
   ToolTip            *tt;

#if USE_COMPOSITE
   EObj               *eo;
#endif

   EDBUG(5, "TooltipCreate");

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
   tt->win = ECreateWindow(VRoot.win, -10, -100, 1, 1, 1);
   tt->iwin = ECreateWindow(tt->win, -10, -100, 1, 1, 1);
#if USE_COMPOSITE
   eo = EobjRegister(tt->win);
   eo->opacity = OpacityExt(Conf_tooltips.opacity);
#endif

   for (i = 0; i < 4; i++)
     {
	Window              win;

	win = 0;
	if (tt->s_iclass[i])
	  {
	     int                 wh = (i + 1) * 8;

	     win = ECreateWindow(VRoot.win, -10, -100, wh, wh, 1);
	     tt->s_iclass[i]->ref_count++;
#if USE_COMPOSITE
	     eo = EobjRegister(win);
	     eo->opacity = OpacityExt(Conf_tooltips.opacity);
#endif
	  }
	tt->s_win[i] = win;
     }

   tt->visible = 0;
   tt->ref_count = 0;

   AddItem(tt, tt->name, 0, LIST_TYPE_TOOLTIP);

   EDBUG_RETURN(tt);
}

#if 0				/* Not used */
static void
TooltipDestroy(ToolTip * tt)
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
#endif

int
TooltipConfigLoad(FILE * ConfigFile)
{
   int                 err = 0;
   ToolTip            *tt;
   char               *name = 0;
   ImageClass         *drawiclass = 0;
   ImageClass         *bubble1 = 0, *bubble2 = 0, *bubble3 = 0, *bubble4 = 0;
   TextClass          *tclass = 0;
   ImageClass         *tooltiphelppic = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   int                 distance = 0;
   int                 fields;

   tt = NULL;
   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	   i1 = CONFIG_INVALID;
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     if ((drawiclass) && (tclass) && (name))
		tt = TooltipCreate(name, drawiclass, bubble1, bubble2,
				   bubble3, bubble4, tclass, distance,
				   tooltiphelppic);
	     if (name)
		Efree(name);
	     goto done;

	  case CONFIG_CLASSNAME:
	     if (ConfigSkipIfExists(ConfigFile, s2, LIST_TYPE_TOOLTIP))
		goto done;
	     name = Estrdup(s2);
	     break;
	  case TOOLTIP_DRAWICLASS:
	  case CONFIG_IMAGECLASS:
	     drawiclass = ImageclassFind(s2, 0);
	     break;
	  case TOOLTIP_BUBBLE1:
	     bubble1 = ImageclassFind(s2, 0);
	     break;
	  case TOOLTIP_BUBBLE2:
	     bubble2 = ImageclassFind(s2, 0);
	     break;
	  case TOOLTIP_BUBBLE3:
	     bubble3 = ImageclassFind(s2, 0);
	     break;
	  case TOOLTIP_BUBBLE4:
	     bubble4 = ImageclassFind(s2, 0);
	     break;
	  case CONFIG_TEXT:
	     tclass = TextclassFind(s2, 1);
	     break;
	  case TOOLTIP_DISTANCE:
	     distance = atoi(s2);
	     break;
	  case TOOLTIP_HELP_PIC:
	     tooltiphelppic = ImageclassFind(s2, 0);
	     break;
	  default:
	     Alert(_("Warning: unable to determine what to do with\n"
		     "the following text in the middle of current "
		     "ToolTip definition:\n"
		     "%s\nWill ignore and continue...\n"), s);
	     break;
	  }
     }
   err = -1;

 done:
   return err;
}

static ImageClass  *
TooltipCreateIclass(const char *name, const char *file, int *pw, int *ph)
{
   ImageClass         *ic;

   ic = ImageclassFind(name, 0);
   if (!ic)
      ic = ImageclassCreateSimple(name, file);

   if (ic->norm.normal && ic->norm.normal->im)
     {
	imlib_context_set_image(ic->norm.normal->im);
	if (*pw < imlib_image_get_width())
	   *pw = imlib_image_get_width();
	if (*ph < imlib_image_get_height())
	   *ph = imlib_image_get_height();
     }

   return ic;
}

static void
TooltipIclassPaste(ToolTip * tt, const char *ic_name, int x, int y, int *px)
{
   ImageClass         *ic;

   ic = ImageclassFind(ic_name, 0);
   if (!ic || !ic->norm.normal->im)
      return;

   imlib_context_set_image(ic->norm.normal->im);
   imlib_context_set_drawable(tt->win);
   imlib_context_set_blend(1);
   imlib_render_image_on_drawable(x, y);
   imlib_context_set_blend(0);

   *px = x + imlib_image_get_width();
}

void
TooltipShow(ToolTip * tt, const char *text, ActionClass * ac, int x, int y)
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
   int                 num, modifiers;
   Action             *aa;
   const char         *tts;

   EDBUG(5, "TooltipShow");

   if (!tt || Mode.mode != MODE_NONE)
      EDBUG_RETURN_;

   pq = Mode.queue_up;
   Mode.queue_up = 0;

   /* if we get an actionclass, look for tooltip action texts */
   if (ac)
     {
	num = ActionclassGetActionCount(ac);
	heights = Emalloc(num * sizeof(int));

	for (i = 0; i < num; i++)
	  {
	     temp_w = 0;
	     temp_h = 0;

	     aa = ActionclassGetAction(ac, i);
	     if (!aa)
		continue;

	     tts = ActionGetTooltipString(aa);
	     if (!tts)
		continue;

	     TextSize(tt->tclass, 0, 0, STATE_NORMAL, tts, &temp_w, &temp_h,
		      17);
	     if (temp_w > labels_width)
		labels_width = temp_w;
	     temp_w = 0;

	     if (ActionGetEvent(aa) == EVENT_DOUBLE_DOWN)
	       {
		  TextSize(tt->tclass, 0, 0, STATE_NORMAL, "2x", &double_w,
			   &temp_h, 17);
		  if (cols[0] < double_w)
		     cols[0] = double_w;
	       }

	     if (ActionGetAnybutton(aa))
	       {
		  TooltipCreateIclass("TOOLTIP_MOUSEBUTTON_ANY",
				      "pix/mouse_any.png", &cols[1], &temp_h);
	       }
	     else
		switch (ActionGetButton(aa))
		  {
		  case 1:
		     ic = TooltipCreateIclass("TOOLTIP_MOUSEBUTTON_1",
					      "pix/mouse_1.png", &cols[1],
					      &temp_h);
		     break;
		  case 2:
		     ic = TooltipCreateIclass("TOOLTIP_MOUSEBUTTON_2",
					      "pix/mouse_2.png", &cols[1],
					      &temp_h);
		     break;
		  case 3:
		     ic = TooltipCreateIclass("TOOLTIP_MOUSEBUTTON_3",
					      "pix/mouse_3.png", &cols[1],
					      &temp_h);
		     break;
		  case 4:
		     ic = TooltipCreateIclass("TOOLTIP_MOUSEBUTTON_4",
					      "pix/mouse_4.png", &cols[1],
					      &temp_h);
		     break;
		  case 5:
		     ic = TooltipCreateIclass("TOOLTIP_MOUSEBUTTON_5",
					      "pix/mouse_5.png", &cols[1],
					      &temp_h);
		     break;
		  case 0:
		  default:
		     break;
		  }

	     modifiers = ActionGetModifiers(aa);
	     if (modifiers)
	       {
		  if (modifiers & ShiftMask)
		     ic = TooltipCreateIclass("TOOLTIP_KEY_SHIFT",
					      "pix/key_shift.png",
					      &cols[2], &temp_h);
		  if (modifiers & LockMask)
		     ic = TooltipCreateIclass("TOOLTIP_KEY_LOCK",
					      "pix/key_lock.png",
					      &cols[3], &temp_h);
		  if (modifiers & ControlMask)
		     ic = TooltipCreateIclass("TOOLTIP_KEY_CTRL",
					      "pix/key_ctrl.png",
					      &cols[4], &temp_h);
		  if (modifiers & Mod1Mask)
		     ic = TooltipCreateIclass("TOOLTIP_KEY_MOD1",
					      "pix/key_mod1.png",
					      &cols[5], &temp_h);
		  if (modifiers & Mod2Mask)
		     ic = TooltipCreateIclass("TOOLTIP_KEY_MOD2",
					      "pix/key_mod2.png",
					      &cols[6], &temp_h);
		  if (modifiers & Mod3Mask)
		     ic = TooltipCreateIclass("TOOLTIP_KEY_MOD3",
					      "pix/key_mod3.png",
					      &cols[7], &temp_h);
		  if (modifiers & Mod4Mask)
		     ic = TooltipCreateIclass("TOOLTIP_KEY_MOD4",
					      "pix/key_mod4.png",
					      &cols[8], &temp_h);
		  if (modifiers & Mod5Mask)
		     ic = TooltipCreateIclass("TOOLTIP_KEY_MOD5",
					      "pix/key_mod5.png",
					      &cols[9], &temp_h);
	       }

	     temp_w = cols[0] + cols[1] + cols[2] + cols[3] + cols[4] +
		cols[5] + cols[6] + cols[7] + cols[8] + cols[9];

	     if (temp_w > icons_width)
		icons_width = temp_w;
	     heights[i] = temp_h;
	     h += temp_h;
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
	ImageclassApply(tt->tooltippic, tt->iwin, iw, ih, 0, 0, STATE_NORMAL, 0,
			ST_TOOLTIP);
     }
   else
      EUnmapWindow(disp, tt->iwin);

   dx = x - VRoot.w / 2;
   dy = y - VRoot.h / 2;

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
	xx = x - (dist * 10 * dx) / (100 * VRoot.w / 2);
	EMoveWindow(disp, tt->s_win[0], xx - 4, yy - 4);

	yy = y - ((ady * 30 * dist) / 100);
	xx = x - (dist * 30 * dx) / (100 * VRoot.w / 2);
	EMoveWindow(disp, tt->s_win[1], xx - 8, yy - 8);

	yy = y - ((ady * 50 * dist) / 100);
	xx = x - (dist * 50 * dx) / (100 * VRoot.w / 2);
	EMoveWindow(disp, tt->s_win[2], xx - 12, yy - 12);

	yy = y - ((ady * 80 * dist) / 100);
	xx = x - (dist * 80 * dx) / (100 * VRoot.w / 2);
	EMoveWindow(disp, tt->s_win[3], xx - 16, yy - 16);

	yy = y - ((ady * 100 * dist) / 100);
	xx = x - (dist * 100 * dx) / (100 * VRoot.w / 2);
	if (ady < 0)
	  {
	     hh = 0;
	  }
	else
	  {
	     hh = h;
	  }
	ww = (w / 2) + ((dx * w) / (VRoot.w / 2));
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
	yy = y - (dist * 10 * dy) / (100 * VRoot.h / 2);
	EMoveWindow(disp, tt->s_win[0], xx - 4, yy - 4);
	xx = x - ((adx * 30 * dist) / 100);
	yy = y - (dist * 30 * dy) / (100 * VRoot.h / 2);
	EMoveWindow(disp, tt->s_win[1], xx - 8, yy - 8);
	xx = x - ((adx * 50 * dist) / 100);
	yy = y - (dist * 50 * dy) / (100 * VRoot.h / 2);
	EMoveWindow(disp, tt->s_win[2], xx - 12, yy - 12);
	xx = x - ((adx * 80 * dist) / 100);
	yy = y - (dist * 80 * dy) / (100 * VRoot.h / 2);
	EMoveWindow(disp, tt->s_win[3], xx - 16, yy - 16);
	xx = x - ((adx * 100 * dist) / 100);
	yy = y - (dist * 100 * dy) / (100 * VRoot.h / 2);
	if (adx < 0)
	  {
	     ww = 0;
	  }
	else
	  {
	     ww = w;
	  }
	hh = (h / 2) + ((dy * h) / (VRoot.h / 2));
     }

   EMoveResizeWindow(disp, tt->win, xx - ww, yy - hh, w, h);

   ImageclassApply(tt->s_iclass[0], tt->s_win[0], 8, 8, 0, 0, STATE_NORMAL, 0,
		   ST_TOOLTIP);
   ImageclassApply(tt->s_iclass[1], tt->s_win[1], 16, 16, 0, 0, STATE_NORMAL, 0,
		   ST_TOOLTIP);
   ImageclassApply(tt->s_iclass[2], tt->s_win[2], 24, 24, 0, 0, STATE_NORMAL, 0,
		   ST_TOOLTIP);
   ImageclassApply(tt->s_iclass[3], tt->s_win[3], 32, 32, 0, 0, STATE_NORMAL, 0,
		   ST_TOOLTIP);
   ImageclassApply(tt->iclass, tt->win, w, h, 0, 0, STATE_NORMAL, 0,
		   ST_TOOLTIP);
   EMapRaised(disp, tt->s_win[0]);
   EMapRaised(disp, tt->s_win[1]);
   EMapRaised(disp, tt->s_win[2]);
   EMapRaised(disp, tt->s_win[3]);
   EMapRaised(disp, tt->win);
   ecore_x_sync();

   xx = tt->iclass->padding.left + iw;

   /* draw the ordinary tooltip text */
   TextDraw(tt->tclass, tt->win, 0, 0, STATE_NORMAL, text, xx,
	    tt->iclass->padding.top, headline_w, headline_h, 17, 512);

   /* draw the icons and labels, if any */
   if (ac)
     {
	num = ActionclassGetActionCount(ac);
	y = tt->iclass->padding.top + headline_h;
	xx = tt->iclass->padding.left + double_w;

	for (i = 0; i < num; i++)
	  {
	     x = xx + iw;

	     aa = ActionclassGetAction(ac, i);
	     if (!aa)
		continue;

	     tts = ActionGetTooltipString(aa);
	     if (!tts)
		continue;

	     if (ActionGetEvent(aa) == EVENT_DOUBLE_DOWN)
	       {
		  TextDraw(tt->tclass, tt->win, 0, 0, STATE_NORMAL, "2x",
			   xx + iw - double_w, y, double_w, heights[i], 17, 0);
	       }

	     if (ActionGetAnybutton(aa))
	       {
		  TooltipIclassPaste(tt, "TOOLTIP_MOUSEBUTTON_ANY", x, y, &x);
	       }
	     else
		switch (ActionGetButton(aa))
		  {
		  case 1:
		     TooltipIclassPaste(tt, "TOOLTIP_MOUSEBUTTON_1", x, y, &x);
		     break;
		  case 2:
		     TooltipIclassPaste(tt, "TOOLTIP_MOUSEBUTTON_2", x, y, &x);
		     break;
		  case 3:
		     TooltipIclassPaste(tt, "TOOLTIP_MOUSEBUTTON_3", x, y, &x);
		     break;
		  case 4:
		     TooltipIclassPaste(tt, "TOOLTIP_MOUSEBUTTON_4", x, y, &x);
		     break;
		  case 5:
		     TooltipIclassPaste(tt, "TOOLTIP_MOUSEBUTTON_5", x, y, &x);
		     break;
		  default:
		     break;
		  }

	     modifiers = ActionGetModifiers(aa);
	     if (modifiers)
	       {
		  if (modifiers & ShiftMask)
		     TooltipIclassPaste(tt, "TOOLTIP_KEY_SHIFT", x, y, &x);
		  if (modifiers & LockMask)
		     TooltipIclassPaste(tt, "TOOLTIP_KEY_LOCK", x, y, &x);
		  if (modifiers & ControlMask)
		     TooltipIclassPaste(tt, "TOOLTIP_KEY_CTRL", x, y, &x);
		  if (modifiers & Mod1Mask)
		     TooltipIclassPaste(tt, "TOOLTIP_KEY_MOD1", x, y, &x);
		  if (modifiers & Mod2Mask)
		     TooltipIclassPaste(tt, "TOOLTIP_KEY_MOD2", x, y, &x);
		  if (modifiers & Mod3Mask)
		     TooltipIclassPaste(tt, "TOOLTIP_KEY_MOD3", x, y, &x);
		  if (modifiers & Mod4Mask)
		     TooltipIclassPaste(tt, "TOOLTIP_KEY_MOD4", x, y, &x);
		  if (modifiers & Mod5Mask)
		     TooltipIclassPaste(tt, "TOOLTIP_KEY_MOD5", x, y, &x);
	       }

	     TextDraw(tt->tclass, tt->win, 0, 0, STATE_NORMAL, tts,
		      tt->iclass->padding.left + icons_width + iw, y,
		      labels_width, heights[i], 17, 0);
	     y += heights[i];

	  }
     }

   Mode.queue_up = pq;
   tt->visible = 1;
   if (heights)
      Efree(heights);
   EDBUG_RETURN_;
}

void
TooltipHide(ToolTip * tt)
{
   EDBUG(5, "TooltipHide");

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
   ecore_x_sync();

   EDBUG_RETURN_;
}

/*
 * Tooltips
 */

void
TooltipsHide(void)
{
   ToolTip           **lst;
   int                 i, j;

   lst = (ToolTip **) ListItemType(&j, LIST_TYPE_TOOLTIP);
   if (lst)
     {
	for (i = 0; i < j; i++)
	  {
	     TooltipHide(lst[i]);
	  }
	Efree(lst);
     }
}

static ToolTip     *ttip = NULL;

static void
ToolTipTimeout(int val __UNUSED__, void *data __UNUSED__)
{
   int                 x, y, dum;
   unsigned int        mask;
   Window              win, rt, ch;
   ActionClass        *ac;
   const char         *tts;

   EDBUG(5, "ToolTipTimeout");

   /* In the case of multiple screens, check to make sure
    * the root window is still where the mouse is... */
   if (False ==
       XQueryPointer(disp, VRoot.win, &rt, &ch, &dum, &dum, &x, &y, &mask))
      EDBUG_RETURN_;

   /* In case this is a virtual root */
   if (x < 0 || y < 0 || x >= VRoot.w || y >= VRoot.h)
      EDBUG_RETURN_;

   /* dont pop up tooltip is mouse button down */
   if (mask &
       (Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask))
      EDBUG_RETURN_;

   win = WindowAtXY(x, y);
   ac = FindActionClass(win);
   if (!ac)
      EDBUG_RETURN_;

   if (!ttip)
      ttip = FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_TOOLTIP);

   tts = ActionclassGetTooltipString(ac);
   if (tts)
     {
	if (Conf_tooltips.showroottooltip)
	  {
	     TooltipShow(ttip, tts, ac, x, y);
	  }
	else
	  {
	     int                 i;
	     int                 show = 1;

	     for (i = 0; i < Conf.desks.num; i++)
	       {
		  if (win == DeskGetWin(i))
		     show = 0;
	       }
	     if (show)
		TooltipShow(ttip, tts, ac, x, y);
	  }
     }

   EDBUG_RETURN_;
}

/*
 * We want this on
 * ButtonPress, ButtonRelease, MotionNotify, EnterNotify, LeaveNotify
 */
void
TooltipsHandleEvent(void)
{
   if (ttip)
      TooltipHide(ttip);
   RemoveTimerEvent("TOOLTIP_TIMEOUT");
   if (Conf_tooltips.enable)
      DoIn("TOOLTIP_TIMEOUT", 0.001 * Conf_tooltips.delay, ToolTipTimeout, 0,
	   NULL);
}

/*
 * Tooltips Module
 */

static void
TooltipsSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_AREA_SWITCH_START:
     case ESIGNAL_DESK_SWITCH_START:
	TooltipsHide();
	break;
     }
}

/*
 * Configuration dialog
 */
static char         tmp_tooltips;
static int          tmp_tooltiptime;
static char         tmp_roottip;

static void
CB_ConfigureTooltips(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf_tooltips.enable = tmp_tooltips;
	Conf_tooltips.delay = tmp_tooltiptime * 10;
	Conf_tooltips.showroottooltip = tmp_roottip;
     }
   autosave();
}

static void
SettingsTooltips(void)
{
   Dialog             *d;
   DItem              *table, *di;

   if ((d =
	FindItem("CONFIGURE_TOOLTIPS", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_TOOLTIPS");

   tmp_tooltips = Conf_tooltips.enable;
   tmp_tooltiptime = Conf_tooltips.delay / 10;
   tmp_roottip = Conf_tooltips.showroottooltip;

   d = DialogCreate("CONFIGURE_TOOLTIPS");
   DialogSetTitle(d, _("Tooltip Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/tips.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Tooltip\n" "Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Display Tooltips"));
   DialogItemCheckButtonSetState(di, tmp_tooltips);
   DialogItemCheckButtonSetPtr(di, &tmp_tooltips);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Display Root Window Tips"));
   DialogItemCheckButtonSetState(di, tmp_roottip);
   DialogItemCheckButtonSetPtr(di, &tmp_roottip);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Tooltip Delay:\n"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 300);
   DialogItemSliderSetUnits(di, 10);
   DialogItemSliderSetJump(di, 25);
   DialogItemSliderSetVal(di, tmp_tooltiptime);
   DialogItemSliderSetValPtr(di, &tmp_tooltiptime);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureTooltips, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureTooltips, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureTooltips, 1);
   DialogSetExitFunction(d, CB_ConfigureTooltips, 2);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
   DialogBindKey(d, "Return", CB_ConfigureTooltips, 0);
   ShowDialog(d);
}

static void
TooltipsIpc(const char *params, Client * c __UNUSED__)
{
   if (params && !strncmp(params, "cfg", 3))
     {
	SettingsTooltips();
     }
}

IpcItem             TooltipsIpcArray[] = {
   {
    TooltipsIpc,
    "tooltips", "tt",
    "Tooltip functions",
    "  tooltips cfg          Configure tooltips\n"}
};
#define N_IPC_FUNCS (sizeof(TooltipsIpcArray)/sizeof(IpcItem))

static const CfgItem TooltipsCfgItems[] = {
   CFG_ITEM_BOOL(Conf_tooltips, enable, 1),
   CFG_ITEM_BOOL(Conf_tooltips, showroottooltip, 1),
   CFG_ITEM_INT(Conf_tooltips, delay, 1500),
   CFG_ITEM_INT(Conf_tooltips, opacity, 200),
};
#define N_CFG_ITEMS (sizeof(TooltipsCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
EModule             ModTooltips = {
   "tooltips", "tt",
   TooltipsSighan,
   {N_IPC_FUNCS, TooltipsIpcArray},
   {N_CFG_ITEMS, TooltipsCfgItems}
};
