#include "E.h"

static void         IcondefChecker(int val, void *data);

#define IB_ANIM_TIME 0.25

void
IB_Animate(char iconify, EWin * from, EWin * to)
{
   double              t1, t2, t, i, spd, ii;
   int                 x, y, w, h, fx, fy, fw, fh, dx, dy, dw, dh;
   GC                  gc;
   XGCValues           gcv;

   GrabX();
   spd = 0.01;
   gcv.subwindow_mode = IncludeInferiors;
   gcv.function = GXxor;
   gcv.foreground = WhitePixel(disp, root.scr);
   if (gcv.foreground == 0)
      gcv.foreground = BlackPixel(disp, root.scr);
   gc = XCreateGC(disp, root.win, GCFunction | GCForeground | GCSubwindowMode,
		  &gcv);
   t1 = GetTime();
   if (iconify)
     {
	fw = from->w;
	fh = from->h;
	fx = from->x + desks.desk[from->desktop].x;
	fy = from->y + desks.desk[from->desktop].y;
	dw = 0;
	dh = 0;
	dx = to->x + desks.desk[to->desktop].x + (to->w / 2);
	dy = to->y + desks.desk[to->desktop].y + (to->h / 2);
	for (i = 0.0; i < 1.0; i += spd)
	  {
	     ii = 1.0 - i;
	     x = (fx * ii) + (dx * i);
	     y = (fy * ii) + (dy * i);
	     w = (fw * ii) + (dw * i);
	     h = (fh * ii) + (dh * i);
	     XDrawRectangle(disp, root.win, gc, x, y, w, h);
	     XDrawRectangle(disp, root.win, gc, x + 1, y + 1, w - 2, h - 2);
	     XDrawRectangle(disp, root.win, gc, x + 2, y + 2, w - 4, h - 4);
	     XSync(disp, False);
	     t2 = GetTime();
	     t = t2 - t1;
	     t1 = t2;
	     spd = t / IB_ANIM_TIME;
	     XDrawRectangle(disp, root.win, gc, x, y, w, h);
	     XDrawRectangle(disp, root.win, gc, x + 1, y + 1, w - 2, h - 2);
	     XDrawRectangle(disp, root.win, gc, x + 2, y + 2, w - 4, h - 4);
	  }
     }
   else
     {
	fw = from->w;
	fh = from->h;
	fx = from->x + desks.desk[from->desktop].x;
	fy = from->y + desks.desk[from->desktop].y;
	dw = 0;
	dh = 0;
	dx = to->x + desks.desk[to->desktop].x + (to->w / 2);
	dy = to->y + desks.desk[to->desktop].y + (to->h / 2);
	for (i = 1.0; i >= 0.0; i -= spd)
	  {
	     ii = 1.0 - i;
	     x = (fx * ii) + (dx * i);
	     y = (fy * ii) + (dy * i);
	     w = (fw * ii) + (dw * i);
	     h = (fh * ii) + (dh * i);
	     XDrawRectangle(disp, root.win, gc, x, y, w, h);
	     XDrawRectangle(disp, root.win, gc, x + 1, y + 1, w - 2, h - 2);
	     XDrawRectangle(disp, root.win, gc, x + 2, y + 2, w - 4, h - 4);
	     XSync(disp, False);
	     t2 = GetTime();
	     t = t2 - t1;
	     t1 = t2;
	     spd = t / IB_ANIM_TIME;
	     XDrawRectangle(disp, root.win, gc, x, y, w, h);
	     XDrawRectangle(disp, root.win, gc, x + 1, y + 1, w - 2, h - 2);
	     XDrawRectangle(disp, root.win, gc, x + 2, y + 2, w - 4, h - 4);
	  }
     }
   XFreeGC(disp, gc);
   UngrabX();
}

void
IconifyEwin(EWin * ewin)
{
   static int          call_depth = 0;
   char                was_shaded;

   if (!ewin)
      EDBUG_RETURN_;
   if (GetZoomEWin() == ewin)
      Zoom(NULL);
   if (ewin->ibox)
      EDBUG_RETURN_;
   if (ewin->client.need_input)
     {
	if ((ewin->skiptask) || (ewin->skipwinlist))
	   EDBUG_RETURN_;
     }
   call_depth++;
   if (call_depth > 256)
     {
	call_depth--;
	return;
     }
   if (!ewin->iconified)
     {
	Iconbox            *ib;

	ib = SelectIconboxForEwin(ewin);
	was_shaded = ewin->shaded;
	if (ib)
	  {
	     IB_Animate(1, ewin, ib->ewin);
	     UpdateAppIcon(ewin, ib->icon_mode);
	  }
	HideEwin(ewin);
	if (was_shaded != ewin->shaded)
	   InstantShadeEwin(ewin);
	MakeIcon(ewin);
	ICCCM_Iconify(ewin);
	if (ewin == mode.focuswin)
	  {
	     char                prev_warp;

	     prev_warp = mode.display_warp;
	     mode.display_warp = 0;
	     GetPrevFocusEwin();
	     mode.display_warp = prev_warp;
	  }
     }
   if (ewin->has_transients)
     {
	EWin              **lst;
	int                 i, num;

	lst = ListTransientsFor(ewin->client.win, &num);
	if (lst)
	  {
	     for (i = 0; i < num; i++)
	       {
		  IconifyEwin(lst[i]);
		  if (lst[i] == mode.focuswin)
		     FocusToEWin(NULL);
	       }
	     Efree(lst);
	  }
     }
   call_depth--;
}

void
DeIconifyEwin(EWin * ewin)
{
   static int          call_depth = 0;
   Iconbox            *ib;

   call_depth++;
   if (call_depth > 256)
     {
	call_depth--;
	return;
     }
   if (ewin->iconified)
     {
	ib = SelectIconboxForEwin(ewin);
	if (ib)
	   IB_Animate(0, ewin, ib->ewin);
	RemoveMiniIcon(ewin);
	if (!ewin->sticky)
	  {
	     MoveEwinToDesktopAt(ewin, desks.current, ewin->x, ewin->y);
	     MoveEwinToArea(ewin,
			    desks.desk[desks.current].current_area_x,
			    desks.desk[desks.current].current_area_y);
	  }
	else
	   ConformEwinToDesktop(ewin);
	RaiseEwin(ewin);
	ShowEwin(ewin);
	ICCCM_DeIconify(ewin);
	FocusToEWin(ewin);
	mode.destroy = 1;
     }
   if (ewin->has_transients)
     {
	EWin              **lst;
	int                 i, num;

	lst = ListTransientsFor(ewin->client.win, &num);
	if (lst)
	  {
	     for (i = 0; i < num; i++)
		DeIconifyEwin(lst[i]);
	     Efree(lst);
	  }
     }
   call_depth--;
}

void
MakeIcon(EWin * ewin)
{
   Iconbox            *ib;

   ib = SelectIconboxForEwin(ewin);
   if (ib)
      AddEwinToIconbox(ib, ewin);
}

void
RemoveMiniIcon(EWin * ewin)
{
   Iconbox            *ib;

   ib = SelectIconboxForEwin(ewin);
   if (ib)
      DelEwinFromIconbox(ib, ewin);
}

void
HideIcons(void)
{
   /* ummmmmmmmmm don't need this anymore - but it used to hide the icons */
   /* when a the gnome pager came up */
}

void
ShowIcons(void)
{
   /* ummmmmmmmmm don't need this anymore - but it used to show the icons */
   /* when a the gnome pager came up */
}

void
HandlePager(void)
{
}

Iconbox            *
CreateIconbox(char *name)
{
   Iconbox            *ib;

   ib = Emalloc(sizeof(Iconbox));
   ib->name = duplicate(name);
   ib->orientation = 0;
   ib->scrollbar_side = 1;
   ib->arrow_side = 1;
   ib->nobg = 0;
   ib->shownames = 1;
   ib->iconsize = 48;
   ib->icon_mode = 2;
   ib->auto_resize = 0;
   ib->draw_icon_base = 0;
   ib->scrollbar_hide = 0;
   ib->cover_hide = 0;
   ib->auto_resize_anchor = 0;
   /* FIXME: need to have theme settable params for this and get them */
   ib->scroll_thickness = 12;
   ib->arrow_thickness = 12;
   ib->bar_thickness = 8;
   ib->knob_length = 8;

   ib->w = 0;
   ib->h = 0;
   ib->pos = 0;
   ib->max = 1;
   ib->arrow1_hilited = 0;
   ib->arrow1_clicked = 0;
   ib->arrow2_hilited = 0;
   ib->arrow2_clicked = 0;
   ib->icon_clicked = 0;
   ib->scrollbar_hilited = 0;
   ib->scrollbar_clicked = 0;
   ib->scrollbox_clicked = 0;
   ib->win = ECreateWindow(root.win, 0, 0, 128, 32, 0);
   ib->icon_win = ECreateWindow(ib->win, 0, 0, 128, 26, 0);
   ib->cover_win = ECreateWindow(ib->win, 0, 0, 128, 26, 0);
   ib->scroll_win = ECreateWindow(ib->win, 6, 26, 116, 6, 0);
   ib->arrow1_win = ECreateWindow(ib->win, 0, 26, 6, 6, 0);
   ib->arrow2_win = ECreateWindow(ib->win, 122, 26, 6, 6, 0);
   ib->scrollbar_win = ECreateWindow(ib->scroll_win, 122, 26, 6, 6, 0);
   ib->scrollbarknob_win = ECreateWindow(ib->scrollbar_win, -20, -20, 4, 4, 0);
   ib->pmap = ECreatePixmap(disp, ib->icon_win, 128, 32, id->x.depth);
   XSelectInput(disp, ib->icon_win, EnterWindowMask | LeaveWindowMask |
		ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
   XSelectInput(disp, ib->scroll_win, EnterWindowMask | LeaveWindowMask |
		ButtonPressMask | ButtonReleaseMask);
   XSelectInput(disp, ib->arrow1_win, EnterWindowMask | LeaveWindowMask |
		ButtonPressMask | ButtonReleaseMask);
   XSelectInput(disp, ib->arrow2_win, EnterWindowMask | LeaveWindowMask |
		ButtonPressMask | ButtonReleaseMask);
   XSelectInput(disp, ib->scrollbar_win, EnterWindowMask | LeaveWindowMask |
		ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
   EMapWindow(disp, ib->icon_win);
   EMapWindow(disp, ib->scroll_win);
   EMapWindow(disp, ib->arrow1_win);
   EMapWindow(disp, ib->arrow2_win);
   EMapWindow(disp, ib->scrollbar_win);
   EMapWindow(disp, ib->scrollbarknob_win);
   ib->ewin = NULL;
   ib->num_icons = 0;
   ib->icons = NULL;
   AddItem(ib, ib->name, 0, LIST_TYPE_ICONBOX);
   return ib;
}

void
FreeIconbox(Iconbox * ib)
{
   int                 i;

   RemoveItem((char *)ib, 0, LIST_FINDBY_POINTER, LIST_TYPE_ICONBOX);
   if (ib->name)
      Efree(ib->name);
   for (i = 0; i < ib->num_icons; i++)
      DeIconifyEwin(ib->icons[i]);
   if (ib->icons)
      Efree(ib->icons);
   if (ib->pmap)
      EFreePixmap(disp, ib->pmap);
   EDestroyWindow(disp, ib->win);
   Efree(ib);
   autosave();
}

void
ShowIconbox(Iconbox * ib)
{
   EWin               *ewin = NULL;
   XClassHint         *xch;
   XTextProperty       xtp;

   xtp.encoding = XA_STRING;
   xtp.format = 8;
   xtp.value = (unsigned char *)("Iconbox");
   xtp.nitems = strlen((char *)(xtp.value));
   XSetWMName(disp, ib->win, &xtp);
   xch = XAllocClassHint();
   xch->res_name = ib->name;
   xch->res_class = "Enlightenment_IconBox";
   XSetClassHint(disp, ib->win, xch);
   XFree(xch);
   ewin = AddInternalToFamily(ib->win, 1, "ICONBOX");
   if (ewin)
     {
	Snapshot           *sn;

	ib->ewin = ewin;
	ewin->client.width.min = 8;
	ewin->client.height.min = 8;
	ewin->client.width.max = 16384;
	ewin->client.height.max = 16384;
	ewin->client.no_resize_h = 0;
	ewin->client.no_resize_v = 0;
	if (ib->orientation)
	  {
	     ImageClass         *ic;
	     int                 extra = 0;

	     ic = FindItem("ICONBOX_VERTICAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     if (ic)
		extra = ic->padding.left + ic->padding.right;
	     ib->ewin->client.width.max =
		ib->ewin->client.width.min =
		ib->iconsize + ib->scroll_thickness + extra;
	     ewin->client.no_resize_h = 1;
	  }
	else
	  {
	     ImageClass         *ic;
	     int                 extra = 0;

	     ic = FindItem("ICONBOX_HORIZONTAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     if (ic)
		extra = ic->padding.left + ic->padding.right;
	     ib->ewin->client.height.max =
		ib->ewin->client.height.min =
		ib->iconsize + ib->scroll_thickness + extra;
	     ewin->client.no_resize_v = 1;
	  }
	ewin->desktop = desks.current;
	ewin->ibox = ib;
	DesktopRemoveEwin(ewin);
	ewin->sticky = 1;
	ResizeEwin(ewin, 160, 160);
	sn = FindSnapshot(ewin);
	/* get the size right damnit! */
	if (sn)
	  {
	     if (sn->use_wh)
		ResizeEwin(ewin, sn->w, sn->h);
	     if (sn->use_xy)
		MoveEwin(ewin, sn->x, sn->y);
	  }
	else
	   MoveEwin(ewin, root.w - (ewin->w), root.h - (ewin->h));
	ConformEwinToDesktop(ewin);
	DesktopRemoveEwin(ewin);
	DesktopAddEwinToTop(ewin);
	ShowEwin(ewin);
	RememberImportantInfoForEwin(ewin);
     }
   IconboxResize(ib, ib->ewin->client.w, ib->ewin->client.h);
}

void
HideIconbox(Iconbox * ib)
{
   if (ib->ewin)
      HideEwin(ib->ewin);
}

void
AddEwinToIconbox(Iconbox * ib, EWin * ewin)
{
   int                 i;

   /* check if its already there - then dont add */
   for (i = 0; i < ib->num_icons; i++)
     {
	if (ib->icons[i] == ewin)
	   return;
     }
   ib->num_icons++;
   ib->icons = Erealloc(ib->icons, sizeof(EWin *) * ib->num_icons);
   ib->icons[ib->num_icons - 1] = ewin;
   RedrawIconbox(ib);
}

void
DelEwinFromIconbox(Iconbox * ib, EWin * ewin)
{
   int                 i, j;

   for (i = 0; i < ib->num_icons; i++)
     {
	if (ib->icons[i] == ewin)
	  {
	     for (j = i; j < ib->num_icons - 1; j++)
		ib->icons[j] = ib->icons[j + 1];
	     ib->num_icons--;
	     if (ib->num_icons > 0)
		ib->icons = Erealloc(ib->icons, sizeof(EWin *) * ib->num_icons);
	     else
	       {
		  Efree(ib->icons);
		  ib->icons = NULL;
	       }
	     RedrawIconbox(ib);
	     return;
	  }
     }
}

void
IB_SnapEWin(EWin * ewin)
{
   int                 w, h, ord, rn, i;
   GC                  gc;
   XGCValues           gcv;
   XRectangle         *r = NULL;
   Iconbox            *ib;
   ImageClass         *ic;

   if (!ewin->visible)
      return;
   w = 40;
   h = 40;
   ib = SelectIconboxForEwin(ewin);
   if (ib)
     {
	w = ib->iconsize;
	h = ib->iconsize;
     }
   ic = FindItem("DEFAULT_ICON_BUTTON", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (ic)
     {
	w -= ic->padding.left + ic->padding.right;
	h -= ic->padding.top + ic->padding.bottom;
     }
   if (ewin->w > ewin->h)
      h = (w * ewin->h) / ewin->w;
   else
      w = (h * ewin->w) / ewin->h;
   ewin->icon_pmap_w = w;
   ewin->icon_pmap_h = h;
   ewin->icon_pmap = ECreatePixmap(disp, ewin->win, w, h, id->x.depth);
   PagerScaleRect(ewin->icon_pmap, ewin->win,
		  0, 0, 0, 0,
		  ewin->w, ewin->h, w, h);
   r = EShapeGetRectangles(disp, ewin->win, ShapeBounding, &rn, &ord);
   ewin->icon_mask = ECreatePixmap(disp, ewin->win, w, h, 1);
   gc = XCreateGC(disp, ewin->icon_mask, 0, &gcv);
   if (r)
     {
	XSetForeground(disp, gc, 0);
	XFillRectangle(disp, ewin->icon_mask, gc, 0, 0, w, h);
	XSetForeground(disp, gc, 1);
	for (i = 0; i < rn; i++)
	  {
	     int                 x, y, ww, hh;

	     x = (r[i].x * w) / ewin->w;
	     y = (r[i].y * h) / ewin->h;
	     ww = (r[i].width * w) / ewin->w;
	     hh = (r[i].height * h) / ewin->h;
	     if (ww < 1)
		ww = 1;
	     if (hh < 1)
		hh = 1;
	     XFillRectangle(disp, ewin->icon_mask, gc, x, y, ww, hh);
	  }
	XFree(r);
     }
   else
     {
	XSetForeground(disp, gc, 1);
	XFillRectangle(disp, ewin->icon_mask, gc, 0, 0, w, h);
     }
   XFreeGC(disp, gc);
   if ((ewin->icon_pmap_w < 1) || (ewin->icon_pmap_h < 1))
     {
	if (ewin->icon_pmap)
	   Imlib_free_pixmap(id, ewin->icon_pmap);
	if (ewin->icon_mask)
	   Imlib_free_pixmap(id, ewin->icon_mask);
	ewin->icon_pmap = 0;
	ewin->icon_mask = 0;
     }
}

void
IB_GetAppIcon(EWin * ewin)
{
   /* get the applications icon pixmap and make a copy... */
   int                 x, y;
   unsigned int        w, h, depth, bw;
   Window              rt;

   if (!ewin->client.icon_pmap)
      return;
   w = 0;
   h = 0;
   EGetGeometry(disp, ewin->client.icon_pmap, &rt, &x, &y, &w, &h, &bw, &depth);
   ewin->icon_pmap_w = (int)w;
   ewin->icon_pmap_h = (int)h;
   ewin->icon_pmap = ECreatePixmap(disp, root.win, w, h, root.depth);
   if (ewin->client.icon_mask)
      ewin->icon_mask = ECreatePixmap(disp, root.win, w, h, 1);
   if (depth == 1)
     {
	GC                  gc;
	XGCValues           gcv;
	int                 r, g, b;

	gc = XCreateGC(disp, ewin->icon_pmap, 0, &gcv);
	r = 255;
	g = 255;
	b = 255;
	XSetForeground(disp, gc, Imlib_best_color_match(id, &r, &g, &b));
	XFillRectangle(disp, ewin->icon_pmap, gc, 0, 0, w, h);
	r = 0;
	g = 0;
	b = 0;
	XSetClipOrigin(disp, gc, 0, 0);
	XSetClipMask(disp, gc, ewin->client.icon_pmap);
	XSetForeground(disp, gc, Imlib_best_color_match(id, &r, &g, &b));
	XFillRectangle(disp, ewin->icon_pmap, gc, 0, 0, w, h);
	XFreeGC(disp, gc);
     }
   else
      EPastePixmap(ewin->icon_pmap, ewin->client.icon_pmap, 0, 0, w, h);
   if (ewin->client.icon_mask)
      EPastePixmap(ewin->icon_mask, ewin->client.icon_mask, 0, 0, w, h);
   if ((ewin->icon_pmap_w < 1) || (ewin->icon_pmap_h < 1))
     {
	if (ewin->icon_pmap)
	   Imlib_free_pixmap(id, ewin->icon_pmap);
	if (ewin->icon_mask)
	   Imlib_free_pixmap(id, ewin->icon_mask);
	ewin->icon_pmap = 0;
	ewin->icon_mask = 0;
     }
}

void
IB_PasteDefaultBase(Drawable d, int x, int y, int w, int h)
{
   ImageClass         *ic;
   Pixmap              p, m;

   /* get the base pixmap */
   ic = FindItem("DEFAULT_ICON_BUTTON", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (!ic)
      return;
   IclassApplyCopy(ic, d, w, h, 0, 0, STATE_NORMAL, &p, &m);
   PastePixmap(disp, d, p, m, x, y);
   Imlib_free_pixmap(id, p);
   Imlib_free_pixmap(id, m);
}

void
IB_PasteDefaultBaseMask(Drawable d, int x, int y, int w, int h)
{
   ImageClass         *ic;
   Pixmap              p, m;
   GC                  gc;
   XGCValues           gcv;

   /* get the base pixmap */
   ic = FindItem("DEFAULT_ICON_BUTTON", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (!ic)
      return;
   IclassApplyCopy(ic, d, w, h, 0, 0, STATE_NORMAL, &p, &m);
   if (m)
     {
	PasteMask(disp, d, m, x, y, w, h);
	Imlib_free_pixmap(id, m);
     }
   else
     {
	gc = XCreateGC(disp, d, 0, &gcv);
	XSetForeground(disp, gc, 1);
	XFillRectangle(disp, d, gc, x, y, w, h);
	XFreeGC(disp, gc);
     }
   Imlib_free_pixmap(id, p);
}

void
IB_GetEIcon(EWin * ewin)
{
   /* get the icon defined for this window in E's iconf match file */
   Icondef            *idef;

   idef = IB_MatchIcondef(ewin->client.title, ewin->client.name, ewin->client.class);

   if (!idef)
      return;
   {
      ImlibImage         *im;

      im = ELoadImage(idef->icon_file);
      if (im)
	{
	   Imlib_render(id, im, im->rgb_width, im->rgb_height);
	   ewin->icon_pmap = Imlib_move_image(id, im);
	   ewin->icon_mask = Imlib_move_mask(id, im);
	   ewin->icon_pmap_w = im->rgb_width;
	   ewin->icon_pmap_h = im->rgb_height;
	   Imlib_destroy_image(id, im);
	}
   }
}

void
IB_AddIcondef(char *title, char *name, char *class, char *file)
{
   /* add match for a window pointing to an iconfile */
   /* form: "*term*" "name*" "*class" "path/to_image.png" */
   Icondef            *idef;

   idef = Emalloc(sizeof(Icondef));
   if (!idef)
      return;
   idef->title_match = duplicate(title);
   idef->name_match = duplicate(name);
   idef->class_match = duplicate(class);
   idef->icon_file = duplicate(file);
   AddItem(idef, "", 0, LIST_TYPE_ICONDEF);
}

void
IB_RemoveIcondef(Icondef * idef)
{
   /* remove the pointed to icondef from our database */
   Icondef            *idef2;

   idef2 = RemoveItem((char *)idef, 0, LIST_FINDBY_POINTER, LIST_TYPE_ICONDEF);
   if (!idef2)
      return;
   if (idef->title_match)
      Efree(idef->title_match);
   if (idef->name_match)
      Efree(idef->name_match);
   if (idef->class_match)
      Efree(idef->class_match);
   if (idef->icon_file)
      Efree(idef->icon_file);
   Efree(idef);
}

Icondef            *
IB_MatchIcondef(char *title, char *name, char *class)
{
   /* return an icondef that matches the data given */
   Icondef           **il, *idef;
   int                 i, num;

   il = IB_ListIcondef(&num);
   if (il)
     {
	for (i = 0; i < num; i++)
	  {
	     char                match = 1;

	     if ((il[i]->title_match) && (!title))
		match = 0;
	     if ((il[i]->name_match) && (!name))
		match = 0;
	     if ((il[i]->class_match) && (!class))
		match = 0;
	     if ((il[i]->title_match) && (title))
	       {
		  if (!matchregexp(il[i]->title_match, title))
		     match = 0;
	       }
	     if ((il[i]->name_match) && (name))
	       {
		  if (!matchregexp(il[i]->name_match, name))
		     match = 0;
	       }
	     if ((il[i]->class_match) && (class))
	       {
		  if (!matchregexp(il[i]->class_match, class))
		     match = 0;
	       }
	     if (match)
	       {
		  idef = il[i];
		  Efree(il);
		  return idef;
	       }
	  }
	Efree(il);
     }
   return NULL;
}

Icondef           **
IB_ListIcondef(int *num)
{
   return (Icondef **) ListItemType(num, LIST_TYPE_ICONDEF);
}

static time_t       last_icondefs_time = 0;

void
IB_LoadIcondefs(void)
{
   /* load the icon defs */
   char               *ff = NULL, s[1024], *s1, *s2, *s3, *s4;
   FILE               *f;

   ff = FindFile("icondefs.cfg");
   if (!ff)
      return;
   f = fopen(ff, "r");
   while (fgets(s, 1024, f))
     {
	s[strlen(s) - 1] = 0;
	/* file format : */
	/* "icon/image.png" "*title*" "*name*" "*class*" */
	/* any field except field 1 can be NULL if you dont care */
	/* the default match is: */
	/* "icon/defailt_image.png" NULL NULL NULL */
	/* and must be first in the file */
	s1 = field(s, 0);
	s2 = field(s, 1);
	s3 = field(s, 2);
	s4 = field(s, 3);
	if (s1)
	   IB_AddIcondef(s2, s3, s4, s1);
	if (s1)
	   Efree(s1);
	if (s2)
	   Efree(s2);
	if (s3)
	   Efree(s3);
	if (s4)
	   Efree(s4);
     }
   fclose(f);
   last_icondefs_time = moddate(ff);
   Efree(ff);
}

void
IB_ReLoadIcondefs(void)
{
   /* stat the icondefs and compare mod date to last known mod date - if */
   /* modified, delete all icondefs and load again */
   char               *ff = NULL;
   Icondef           **idef;
   int                 i, num;

   ff = FindFile("icondefs.cfg");
   if (!ff)
     {
	idef = IB_ListIcondef(&num);
	if (idef)
	  {
	     for (i = 0; i < num; i++)
		IB_RemoveIcondef(idef[i]);
	     Efree(idef);
	  }
	return;
     }
   if (moddate(ff) > last_icondefs_time)
     {
	idef = IB_ListIcondef(&num);
	if (idef)
	  {
	     for (i = 0; i < num; i++)
		IB_RemoveIcondef(idef[i]);
	     Efree(idef);
	  }
	IB_LoadIcondefs();
     }
   Efree(ff);
}

static void
IcondefChecker(int val, void *data)
{
   IB_ReLoadIcondefs();
   DoIn("ICONDEF_CHECK", 2.0, IcondefChecker, 0, NULL);
   val = 0;
   data = NULL;
}

void
IB_SaveIcondefs(void)
{
   /* save the icondefs */
   char                s[1024];
   FILE               *f;

   Esnprintf(s, sizeof(s), "%s/icondefs.cfg", UserEDir());
   f = fopen(s, "w");
   if (f)
     {
	Icondef           **idef;
	int                 i, num;

	idef = IB_ListIcondef(&num);
	for (i = num - 1; i >= 0; i--)
	  {
	     char               *f1, *f2, *f3, *f4;

	     f1 = idef[i]->icon_file;
	     f2 = idef[i]->title_match;
	     f3 = idef[i]->name_match;
	     f4 = idef[i]->class_match;

	     if (f1)
	       {
		  fprintf(f, "\"%s\" ", f1);
		  if (f2)
		     fprintf(f, "\"%s\" ", f2);
		  else
		     fprintf(f, "NULL ");
		  if (f3)
		     fprintf(f, "\"%s\" ", f3);
		  else
		     fprintf(f, "NULL ");
		  if (f4)
		     fprintf(f, "\"%s\"\n", f4);
		  else
		     fprintf(f, "NULL\n");
	       }
	  }
	fclose(f);
	last_icondefs_time = moddate(s);
     }
}

Iconbox           **
ListAllIconboxes(int *num)
{
   /* list all currently available Iconboxes */
   return (Iconbox **) ListItemType(num, LIST_TYPE_ICONBOX);
}

Iconbox            *
SelectIconboxForEwin(EWin * ewin)
{
   /* find the appropriate iconbox from all available ones for this app */
   /* if it is to be iconified, or if it is alreayd return which iconbox */
   /* it's in */
   Iconbox           **ib, *ib_sel = NULL;
   int                 i, j, num = 0;

   if (!ewin)
      return NULL;
   ib = ListAllIconboxes(&num);
   if (ib)
     {
	if (ewin->iconified)
	  {
	     /* find the iconbox this window got iconifed into */
	     for (i = 0; i < num; i++)
	       {
		  for (j = 0; j < ib[i]->num_icons; j++)
		    {
		       if (ib[i]->icons[j] == ewin)
			 {
			    Iconbox            *ibr;

			    ibr = ib[i];
			    Efree(ib);
			    return ibr;
			 }
		    }
	       }
	  }
	else
	  {
	     /* pick the closest iconbox physically on screen to put it in */
	     int                 min_dist;

	     ib_sel = ib[0];
	     min_dist = 0x7fffffff;
	     for (i = 0; i < num; i++)
	       {
		  int                 dx, dy, dist;

		  dx = (ib[i]->ewin->x + (ib[i]->ewin->w / 2)) -
		     (ewin->x + (ewin->w / 2));
		  dy = (ib[i]->ewin->y + (ib[i]->ewin->h / 2)) -
		     (ewin->y + (ewin->h / 2));
		  dist = (dx * dx) + (dy * dy);
		  if ((!ib[i]->ewin->sticky) &&
		      (ib[i]->ewin->desktop != ewin->desktop))
		     dist += (root.w * root.w) + (root.h * root.h);
		  if (dist < min_dist)
		    {
		       min_dist = dist;
		       ib_sel = ib[i];
		    }
	       }
	  }
	Efree(ib);
     }
   return ib_sel;
}

void
UpdateAppIcon(EWin * ewin, int imode)
{
   /* free whatever we had before */
   Iconbox            *ib;

   if (ewin->icon_pmap)
      Imlib_free_pixmap(id, ewin->icon_pmap);
   if (ewin->icon_mask)
      Imlib_free_pixmap(id, ewin->icon_mask);
   ewin->icon_pmap = 0;
   ewin->icon_mask = 0;

   switch (imode)
     {
     case 0:
	/* snap first - if fails try app, then e */
	if (!ewin->icon_pmap)
	  {
	     if (ewin->shaded)
		InstantUnShadeEwin(ewin);
	     RaiseEwin(ewin);
	     IB_SnapEWin(ewin);
	  }
	if (!ewin->icon_pmap)
	   IB_GetAppIcon(ewin);
	if (!ewin->icon_pmap)
	   IB_GetEIcon(ewin);
	break;
     case 1:
	/* try app first, then e, then snap */
	if (!ewin->icon_pmap)
	   IB_GetAppIcon(ewin);
	if (!ewin->icon_pmap)
	   IB_GetEIcon(ewin);
	if (!ewin->icon_pmap)
	  {
	     if (ewin->shaded)
		InstantUnShadeEwin(ewin);
	     RaiseEwin(ewin);
	     IB_SnapEWin(ewin);
	  }
	break;
     case 2:
	/* try E first, then snap */
	if (!ewin->icon_pmap)
	   IB_GetEIcon(ewin);
	if (!ewin->icon_pmap)
	  {
	     if (ewin->shaded)
		InstantUnShadeEwin(ewin);
	     RaiseEwin(ewin);
	     IB_SnapEWin(ewin);
	  }
	break;
     default:
	break;
     }
   ib = SelectIconboxForEwin(ewin);
   if ((ewin->iconified) && (ib))
      RedrawIconbox(ib);
}

void
IB_CalcMax(Iconbox * ib)
{
   int                 i, x, y;

   x = 0;
   y = 0;
   for (i = 0; i < ib->num_icons; i++)
     {
	int                 w, h;
	EWin               *ewin;

	w = 8;
	h = 8;
	ewin = ib->icons[i];
	if (!ewin->icon_pmap)
	   UpdateAppIcon(ewin, mode.icon_mode);
	if (ewin->icon_pmap)
	  {
	     w = ewin->icon_pmap_w;
	     h = ewin->icon_pmap_h;
	  }
	if (ib->draw_icon_base)
	  {
	     x += ib->iconsize;
	     y += ib->iconsize;
	  }
	else
	  {
	     x += w + 2;
	     y += h + 2;
	  }
     }
   if (ib->orientation)
      ib->max = y - 2;
   else
      ib->max = x - 2;
   if (ib->max < 1)
      ib->max = 1;
}

EWin               *
IB_FindIcon(Iconbox * ib, int px, int py)
{
   int                 i, x = 0, y = 0;
   ImageClass         *ic = NULL;

   if (ib->orientation)
     {
	ic = FindItem("ICONBOX_VERTICAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	y = -ib->pos;
	x = 0;
	if (ic)
	  {
	     x += ic->padding.left;
	     y += ic->padding.top;
	  }
     }
   else
     {
	ic = FindItem("ICONBOX_HORIZONTAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	x = -ib->pos;
	y = 0;
	if (ic)
	  {
	     x += ic->padding.left;
	     y += ic->padding.top;
	  }
     }
   for (i = 0; i < ib->num_icons; i++)
     {
	int                 w, h, xx, yy;
	EWin               *ewin;

	w = 8;
	h = 8;
	ewin = ib->icons[i];
	if (!ewin->icon_pmap)
	   UpdateAppIcon(ewin, mode.icon_mode);
	if (ewin->icon_pmap)
	  {
	     w = ewin->icon_pmap_w;
	     h = ewin->icon_pmap_h;
	     xx = x;
	     yy = y;
	     if (ib->orientation)
		xx += (ib->iconsize - w) / 2;
	     else
		yy += (ib->iconsize - h) / 2;
	     if ((px >= (xx - 1)) && (py >= (yy - 1)) &&
		 (px < (xx + w + 1)) &&
		 (py < (yy + h + 1)))
		return ewin;
	  }
	if (ib->orientation)
	  {
	     if (ib->draw_icon_base)
		y += ib->iconsize;
	     else
		y += h + 2;
	  }
	else
	  {
	     if (ib->draw_icon_base)
		x += ib->iconsize;
	     else
		x += w + 2;
	  }
     }
   return NULL;
}

void
IB_DrawScroll(Iconbox * ib)
{
   ImageClass         *ic;
   char                show_sb = 1;

   if (ib->orientation)
     {
	int                 bs, bw, bx;

	ic = FindItem("ICONBOX_SCROLLBAR_BASE_VERTICAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if (ib->arrow_side < 3)
	   bs = ib->h - (ib->arrow_thickness * 2);
	else
	   bs = ib->h;
	bw = (ib->h * bs) / ib->max;
	if (ic)
	  {
	     bs -= (ic->padding.top + ic->padding.bottom);
	     bw = ((ib->h - (ic->padding.top + ic->padding.bottom)) *
		   bs) / ib->max;
	  }
	if (bs < 1)
	   bs = 1;
	if (bw > bs)
	   bw = bs;
	if (bw < 1)
	   bw = 1;
	bx = ((ib->pos * bs) / ib->max);
	if (ic)
	   bx += ic->padding.top;
	if ((ib->scrollbar_hide) && (bw == bs))
	   show_sb = 0;
	ic = FindItem("ICONBOX_SCROLLKNOB_VERTICAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if ((ic) && (bw > ib->knob_length))
	   EMoveResizeWindow(disp, ib->scrollbarknob_win,
			     0, (bw - ib->knob_length) / 2,
			     ib->bar_thickness, ib->knob_length);
	else
	   EMoveResizeWindow(disp, ib->scrollbarknob_win,
			     -9999, -9999,
			     ib->bar_thickness, ib->knob_length);
	if (show_sb)
	  {
	     /* fix this area */
	     if (ib->scrollbar_side == 1)
		/* right */
	       {
		  /* start */
		  if (ib->arrow_side == 0)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 ib->w - ib->scroll_thickness, 0,
				   ib->scroll_thickness, ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
			    ib->w - ib->scroll_thickness, ib->arrow_thickness,
				   ib->scroll_thickness, ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
			ib->w - ib->scroll_thickness, ib->arrow_thickness * 2,
					 ib->scroll_thickness, ib->h - (ib->arrow_thickness * 2));
		    }
		  /* both ends */
		  else if (ib->arrow_side == 1)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 ib->w - ib->scroll_thickness, 0,
				   ib->scroll_thickness, ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 ib->w - ib->scroll_thickness, ib->h - ib->arrow_thickness,
				   ib->scroll_thickness, ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
			    ib->w - ib->scroll_thickness, ib->arrow_thickness,
					 ib->scroll_thickness, ib->h - (ib->arrow_thickness * 2));
		    }
		  /* end */
		  else if (ib->arrow_side == 2)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 ib->w - ib->scroll_thickness, ib->h - (ib->arrow_thickness * 2),
				   ib->scroll_thickness, ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 ib->w - ib->scroll_thickness, ib->h - ib->arrow_thickness,
				   ib->scroll_thickness, ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 ib->w - ib->scroll_thickness, 0,
					 ib->scroll_thickness, ib->h - (ib->arrow_thickness * 2));
		    }
		  /* no arrows */
		  else
		    {
		       EUnmapWindow(disp, ib->arrow1_win);
		       EUnmapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 ib->w - ib->scroll_thickness, 0,
					 ib->scroll_thickness, ib->h);
		    }
	       }
	     else
		/* left */
	       {
		  /* start */
		  if (ib->arrow_side == 0)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 0, 0,
				   ib->scroll_thickness, ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 0, ib->arrow_thickness,
				   ib->scroll_thickness, ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 0, ib->arrow_thickness * 2,
					 ib->scroll_thickness, ib->h - (ib->arrow_thickness * 2));
		    }
		  /* both ends */
		  else if (ib->arrow_side == 1)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 0, 0,
				   ib->scroll_thickness, ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 0, ib->h - ib->arrow_thickness,
				   ib->scroll_thickness, ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 0, ib->arrow_thickness,
					 ib->scroll_thickness, ib->h - (ib->arrow_thickness * 2));
		    }
		  /* end */
		  else if (ib->arrow_side == 2)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 0, ib->h - (ib->arrow_thickness * 2),
				   ib->scroll_thickness, ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 0, ib->h - ib->arrow_thickness,
				   ib->scroll_thickness, ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 0, 0,
					 ib->scroll_thickness, ib->h - (ib->arrow_thickness * 2));
		    }
		  /* no arrows */
		  else
		    {
		       EUnmapWindow(disp, ib->arrow1_win);
		       EUnmapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 0, 0,
					 ib->scroll_thickness, ib->h);
		    }
	       }
	  }
	else
	  {
	     EMoveResizeWindow(disp, ib->scroll_win, -9999, -9999, 2, 2);
	     EMoveResizeWindow(disp, ib->arrow1_win, -9999, -9999, 2, 2);
	     EMoveResizeWindow(disp, ib->arrow2_win, -9999, -9999, 2, 2);
	  }
	EMoveResizeWindow(disp, ib->scrollbar_win,
			  (ib->scroll_thickness - ib->bar_thickness) / 2, bx,
			  ib->bar_thickness, bw);

	ic = FindItem("ICONBOX_SCROLLBAR_BASE_VERTICAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if (ic)
	   IclassApply(ic, ib->scroll_win, -1, -1, 0, 0, STATE_NORMAL, 0);
	ic = FindItem("ICONBOX_SCROLLBAR_KNOB_VERTICAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->scrollbar_hilited)
		state = STATE_HILITED;
	     if (ib->scrollbar_clicked)
		state = STATE_CLICKED;
	     IclassApply(ic, ib->scrollbar_win, -1, -1, 0, 0, state, 0);
	  }
	ic = FindItem("ICONBOX_SCROLLKNOB_VERTICAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->scrollbar_hilited)
		state = STATE_HILITED;
	     if (ib->scrollbar_clicked)
		state = STATE_CLICKED;
	     IclassApply(ic, ib->scrollbarknob_win, -1, -1, 0, 0, state, 0);
	  }
	ic = FindItem("ICONBOX_ARROW_UP", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->arrow1_hilited)
		state = STATE_HILITED;
	     if (ib->arrow1_clicked)
		state = STATE_CLICKED;
	     IclassApply(ic, ib->arrow1_win, -1, -1, 0, 0, state, 0);
	  }
	ic = FindItem("ICONBOX_ARROW_DOWN", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->arrow2_hilited)
		state = STATE_HILITED;
	     if (ib->arrow2_clicked)
		state = STATE_CLICKED;
	     IclassApply(ic, ib->arrow2_win, -1, -1, 0, 0, state, 0);
	  }
	/* remove this coment when fixed */
     }
   else
     {
	int                 bs, bw, bx;

	ic = FindItem("ICONBOX_SCROLLBAR_BASE_HORIZONTAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if (ib->arrow_side < 3)
	   bs = ib->w - (ib->arrow_thickness * 2);
	else
	   bs = ib->w;
	bw = (ib->w * bs) / ib->max;
	if (ic)
	  {
	     bs -= (ic->padding.left + ic->padding.right);
	     bw = ((ib->w - (ic->padding.left + ic->padding.right)) *
		   bs) / ib->max;
	  }
	if (bs < 1)
	   bs = 1;
	if (bw > bs)
	   bw = bs;
	if (bw < 1)
	   bw = 1;
	bx = ((ib->pos * bs) / ib->max);
	if (ic)
	   bx += ic->padding.left;
	if ((ib->scrollbar_hide) && (bw == bs))
	   show_sb = 0;
	ic = FindItem("ICONBOX_SCROLLKNOB_HORIZONTAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if ((ic) && (bw > ib->knob_length))
	   EMoveResizeWindow(disp, ib->scrollbarknob_win,
			     (bw - ib->knob_length) / 2, 0,
			     ib->knob_length, ib->bar_thickness);
	else
	   EMoveResizeWindow(disp, ib->scrollbarknob_win,
			     -9999, -9999,
			     ib->knob_length, ib->bar_thickness);

	if (show_sb)
	  {
	     if (ib->scrollbar_side == 1)
		/* bottom */
	       {
		  /* start */
		  if (ib->arrow_side == 0)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 0, ib->h - ib->scroll_thickness,
				   ib->arrow_thickness, ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
			    ib->arrow_thickness, ib->h - ib->scroll_thickness,
				   ib->arrow_thickness, ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
			ib->arrow_thickness * 2, ib->h - ib->scroll_thickness,
					 ib->w - (ib->arrow_thickness * 2), ib->scroll_thickness);
		    }
		  /* both ends */
		  else if (ib->arrow_side == 1)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 0, ib->h - ib->scroll_thickness,
				   ib->arrow_thickness, ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 ib->w - ib->arrow_thickness, ib->h - ib->scroll_thickness,
				   ib->arrow_thickness, ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
			    ib->arrow_thickness, ib->h - ib->scroll_thickness,
					 ib->w - (ib->arrow_thickness * 2), ib->scroll_thickness);
		    }
		  /* end */
		  else if (ib->arrow_side == 2)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 ib->w - (ib->arrow_thickness * 2), ib->h - ib->scroll_thickness,
				   ib->arrow_thickness, ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 ib->w - ib->arrow_thickness, ib->h - ib->scroll_thickness,
				   ib->arrow_thickness, ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 0, ib->h - ib->scroll_thickness,
					 ib->w - (ib->arrow_thickness * 2), ib->scroll_thickness);
		    }
		  /* no arrows */
		  else
		    {
		       EUnmapWindow(disp, ib->arrow1_win);
		       EUnmapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 0, ib->h - ib->scroll_thickness,
					 ib->w, ib->scroll_thickness);
		    }
	       }
	     else
		/* top */
	       {
		  /* start */
		  if (ib->arrow_side == 0)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 0, 0,
				   ib->arrow_thickness, ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 ib->arrow_thickness, 0,
				   ib->arrow_thickness, ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 ib->arrow_thickness * 2, 0,
					 ib->w - (ib->arrow_thickness * 2), ib->scroll_thickness);
		    }
		  /* both ends */
		  else if (ib->arrow_side == 1)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 0, 0,
				   ib->arrow_thickness, ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 ib->w - ib->arrow_thickness, 0,
				   ib->arrow_thickness, ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 ib->arrow_thickness, 0,
					 ib->w - (ib->arrow_thickness * 2), ib->scroll_thickness);
		    }
		  /* end */
		  else if (ib->arrow_side == 2)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 ib->w - (ib->arrow_thickness * 2), 0,
				   ib->arrow_thickness, ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 ib->w - ib->arrow_thickness, 0,
				   ib->arrow_thickness, ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 0, 0,
					 ib->w - (ib->arrow_thickness * 2), ib->scroll_thickness);
		    }
		  /* no arrows */
		  else
		    {
		       EUnmapWindow(disp, ib->arrow1_win);
		       EUnmapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 0, 0,
					 ib->w, ib->scroll_thickness);
		    }
	       }
	  }
	else
	  {
	     EMoveResizeWindow(disp, ib->scroll_win, -9999, -9999, 2, 2);
	     EMoveResizeWindow(disp, ib->arrow1_win, -9999, -9999, 2, 2);
	     EMoveResizeWindow(disp, ib->arrow2_win, -9999, -9999, 2, 2);
	  }

	EMoveResizeWindow(disp, ib->scrollbar_win,
			  bx, (ib->scroll_thickness - ib->bar_thickness) / 2,
			  bw, ib->bar_thickness);

	ic = FindItem("ICONBOX_SCROLLBAR_BASE_HORIZONTAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if (ic)
	   IclassApply(ic, ib->scroll_win, -1, -1, 0, 0, STATE_NORMAL, 0);
	ic = FindItem("ICONBOX_SCROLLBAR_KNOB_HORIZONTAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->scrollbar_hilited)
		state = STATE_HILITED;
	     if (ib->scrollbar_clicked)
		state = STATE_CLICKED;
	     IclassApply(ic, ib->scrollbar_win, -1, -1, 0, 0, state, 0);
	  }
	ic = FindItem("ICONBOX_SCROLLKNOB_HORIZONTAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->scrollbar_hilited)
		state = STATE_HILITED;
	     if (ib->scrollbar_clicked)
		state = STATE_CLICKED;
	     IclassApply(ic, ib->scrollbarknob_win, -1, -1, 0, 0, state, 0);
	  }
	ic = FindItem("ICONBOX_ARROW_LEFT", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->arrow1_hilited)
		state = STATE_HILITED;
	     if (ib->arrow1_clicked)
		state = STATE_CLICKED;
	     IclassApply(ic, ib->arrow1_win, -1, -1, 0, 0, state, 0);
	  }
	ic = FindItem("ICONBOX_ARROW_RIGHT", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->arrow2_hilited)
		state = STATE_HILITED;
	     if (ib->arrow2_clicked)
		state = STATE_CLICKED;
	     IclassApply(ic, ib->arrow2_win, -1, -1, 0, 0, state, 0);
	  }
     }
   PropagateShapes(ib->win);
   {
      Border             *b;

      b = ib->ewin->border;
      SyncBorderToEwin(ib->ewin);
      if (ib->ewin->border == b)
	 PropagateShapes(ib->ewin->win);
   }
}

void
IB_FixPos(Iconbox * ib)
{
   if (ib->orientation)
     {
	ImageClass         *ic;
	int                 v = 0;

	ic = FindItem("ICONBOX_SCROLLBAR_BASE_VERTICAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	v = ib->max - ib->h;
	if (ic)
	   v += ic->padding.top + ic->padding.bottom;
	if (ib->pos > v)
	   ib->pos = v;
     }
   else
     {
	ImageClass         *ic;
	int                 v = 0;

	ic = FindItem("ICONBOX_SCROLLBAR_BASE_HORIZONTAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	v = ib->max - ib->w;
	if (ic)
	   v += ic->padding.left + ic->padding.right;
	if (ib->pos > v)
	   ib->pos = v;
     }
   if (ib->pos < 0)
      ib->pos = 0;

}

void
RedrawIconbox(Iconbox * ib)
{
   ImageClass         *ic;
   Pixmap              m = 0;
   char                pq;

   if (!ib)
      return;

   if (ib->auto_resize)
     {
	int                 add = 0;
	int                 x, y, w, h;

	x = ib->ewin->x;
	y = ib->ewin->y;
	w = ib->ewin->client.w;
	h = ib->ewin->client.h;
	IB_CalcMax(ib);
	if (ib->orientation)
	  {
	     ic = FindItem("ICONBOX_VERTICAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     if (ic)
		add = ic->padding.top + ic->padding.bottom;
	     add += ib->max;
	     if ((ib->ewin->border->border.top +
		  ib->ewin->border->border.bottom + add) >
		 root.h)
		add = root.h - (ib->ewin->border->border.top +
				ib->ewin->border->border.bottom);
	     x = ib->ewin->x;
	     y = ib->ewin->y +
		(((ib->ewin->client.h - add) * ib->auto_resize_anchor) >> 10);
	     w = ib->ewin->client.w;
	     h = add;
	     if ((ib->ewin->y + ib->ewin->border->border.top +
		  ib->ewin->border->border.bottom + add) >
		 root.h)
	       {
		  x = ib->ewin->x;
		  y = root.h - (ib->ewin->border->border.top +
				ib->ewin->border->border.bottom + add);
		  w = ib->ewin->client.w;
		  h = add;
	       }
	  }
	else
	  {
	     ic = FindItem("ICONBOX_HORIZONTAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     if (ic)
		add = ic->padding.left + ic->padding.right;
	     add += ib->max;
	     if ((ib->ewin->border->border.left +
		  ib->ewin->border->border.right + add) >
		 root.w)
		add = root.w - (ib->ewin->border->border.left +
				ib->ewin->border->border.right);
	     x = ib->ewin->x +
		(((ib->ewin->client.w - add) * ib->auto_resize_anchor) >> 10);
	     y = ib->ewin->y;
	     w = add;
	     h = ib->ewin->client.h;
	     if ((ib->ewin->x + ib->ewin->border->border.left +
		  ib->ewin->border->border.right + add) >
		 root.w)
	       {
		  x = root.w - (ib->ewin->border->border.left +
				ib->ewin->border->border.right + add);
		  y = ib->ewin->y;
		  w = add;
		  h = ib->ewin->client.h;
	       }
	  }
	if ((x != ib->ewin->x) || (y != ib->ewin->y) ||
	    (w != ib->ewin->client.w) || (h != ib->ewin->client.h))
	   MoveResizeEwin(ib->ewin, x, y, w, h);
	EResizeWindow(disp, ib->win, w, h);
	EFreePixmap(disp, ib->pmap);
	ib->pmap = ECreatePixmap(disp, ib->icon_win, w, h, id->x.depth);
	ib->w = w;
	ib->h = h;
     }

   pq = queue_up;
   queue_up = 0;

   IB_CalcMax(ib);
   IB_FixPos(ib);
   IB_DrawScroll(ib);

   if (ib->orientation)
     {
	int                 i;
	int                 x, y;

	if (ib->scrollbar_side == 1)
	  {
	     /* right */
	     EMoveResizeWindow(disp, ib->icon_win,
			       0, 0,
			       ib->w - ib->scroll_thickness, ib->h);
	     if ((ic = FindItem("ICONBOX_COVER_VERTICAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS))
		 && (!(ib->cover_hide)))
	       {
		  EMoveResizeWindow(disp, ib->cover_win,
				    0, 0,
				    ib->w - ib->scroll_thickness, ib->h);
		  EMapWindow(disp, ib->cover_win);
		  IclassApply(ic, ib->cover_win, -1, -1, 0, 0, STATE_NORMAL, 0);
	       }
	     else
	       {
		  EMoveResizeWindow(disp, ib->cover_win,
				    -30000, -30000,
				    2, 2);
		  EUnmapWindow(disp, ib->cover_win);
	       }
	  }
	else
	  {
	     /* left */
	     EMoveResizeWindow(disp, ib->icon_win,
			       ib->scroll_thickness, 0,
			       ib->w - ib->scroll_thickness, ib->h);
	     if ((ic = FindItem("ICONBOX_COVER_VERTICAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS))
		 && (!(ib->cover_hide)))
	       {
		  EMoveResizeWindow(disp, ib->cover_win,
				    ib->scroll_thickness, 0,
				    ib->w - ib->scroll_thickness, ib->h);
		  EMapWindow(disp, ib->cover_win);
		  IclassApply(ic, ib->cover_win, -1, -1, 0, 0, STATE_NORMAL, 0);
	       }
	     else
	       {
		  EMoveResizeWindow(disp, ib->cover_win,
				    -30000, -30000,
				    2, 2);
		  EUnmapWindow(disp, ib->cover_win);
	       }
	  }

	ic = FindItem("ICONBOX_VERTICAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if (!ib->nobg)
	  {
	     if (ic)
	       {
		  Pixmap              pmap = 0, mask = 0;
		  int                 iw, ih;

		  GetWinWH(ib->icon_win, (unsigned int *)&iw, (unsigned int *)&ih);
		  IclassApplyCopy(ic, ib->icon_win, iw, ih, 0, 0, STATE_NORMAL,
				  &pmap, &mask);
		  EShapeCombineMask(disp, ib->icon_win, ShapeBounding, 0, 0, mask, ShapeSet);
		  PastePixmap(disp, ib->pmap, pmap, mask, 0, 0);
		  Imlib_free_pixmap(id, pmap);
		  Imlib_free_pixmap(id, mask);
	       }
	  }
	else
	  {
	     int                 iw, ih;
	     GC                  gc;
	     XGCValues           gcv;

	     GetWinWH(ib->icon_win, (unsigned int *)&iw, (unsigned int *)&ih);
	     m = ECreatePixmap(disp, ib->icon_win, iw, ih, 1);
	     gc = XCreateGC(disp, m, 0, &gcv);
	     XSetForeground(disp, gc, 0);
	     XFillRectangle(disp, m, gc, 0, 0, iw, ih);
	     XFreeGC(disp, gc);
	  }

	y = -ib->pos;
	x = 0;
	if (ic)
	  {
	     x += ic->padding.left;
	     y += ic->padding.top;
	  }
	for (i = 0; i < ib->num_icons; i++)
	  {
	     int                 w, h;
	     EWin               *ewin;

	     w = 8;
	     h = 8;
	     ewin = ib->icons[i];
	     if (!ewin->icon_pmap)
		UpdateAppIcon(ewin, mode.icon_mode);
	     if (ewin->icon_pmap)
	       {
		  w = ewin->icon_pmap_w;
		  h = ewin->icon_pmap_h;
		  if (ib->draw_icon_base)
		    {
		       IB_PasteDefaultBase(ib->pmap, x, y, ib->iconsize, ib->iconsize);
		       if (ib->nobg)
			  IB_PasteDefaultBaseMask(m, x, y, ib->iconsize, ib->iconsize);
		    }
		  if (ib->draw_icon_base)
		     PastePixmap(disp, ib->pmap,
				 ewin->icon_pmap,
				 ewin->icon_mask, x + ((ib->iconsize - w) / 2), y + ((ib->iconsize - h) / 2));
		  else
		     PastePixmap(disp, ib->pmap,
				 ewin->icon_pmap,
			    ewin->icon_mask, x + ((ib->iconsize - w) / 2), y);
		  if (ib->nobg)
		     PasteMask(disp, m, ewin->icon_mask, x + ((ib->iconsize - w) / 2), y, w, h);
	       }
	     if (ib->draw_icon_base)
		y += ib->iconsize;
	     else
		y += h + 2;
	  }
     }
   else
     {
	int                 i;
	int                 x, y;

	if (ib->scrollbar_side == 1)
	  {
	     /* bottom */
	     EMoveResizeWindow(disp, ib->icon_win,
			       0, 0,
			       ib->w, ib->h - ib->scroll_thickness);
	     if ((ic = FindItem("ICONBOX_COVER_HORIZONTAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS))
		 && (!(ib->cover_hide)))
	       {
		  EMoveResizeWindow(disp, ib->cover_win,
				    0, 0,
				    ib->w, ib->h - ib->scroll_thickness);
		  EMapWindow(disp, ib->cover_win);
		  IclassApply(ic, ib->cover_win, -1, -1, 0, 0, STATE_NORMAL, 0);
	       }
	     else
	       {
		  EMoveResizeWindow(disp, ib->cover_win,
				    -30000, -30000,
				    2, 2);
		  EUnmapWindow(disp, ib->cover_win);
	       }
	  }
	else
	  {
	     /* top */
	     EMoveResizeWindow(disp, ib->icon_win,
			       0, ib->scroll_thickness,
			       ib->w, ib->h - ib->scroll_thickness);
	     if ((ic = FindItem("ICONBOX_COVER_HORIZONTAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS))
		 && (!(ib->cover_hide)))
	       {
		  EMoveResizeWindow(disp, ib->cover_win,
				    0, ib->scroll_thickness,
				    ib->w, ib->h - ib->scroll_thickness);
		  EMapWindow(disp, ib->cover_win);
		  IclassApply(ic, ib->cover_win, -1, -1, 0, 0, STATE_NORMAL, 0);
	       }
	     else
	       {
		  EMoveResizeWindow(disp, ib->cover_win,
				    -30000, -30000,
				    2, 2);
		  EUnmapWindow(disp, ib->cover_win);
	       }
	  }

	ic = FindItem("ICONBOX_HORIZONTAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if (!ib->nobg)
	  {
	     if (ic)
	       {
		  Pixmap              pmap = 0, mask = 0;
		  int                 iw, ih;

		  GetWinWH(ib->icon_win, (unsigned int *)&iw, (unsigned int *)&ih);
		  IclassApplyCopy(ic, ib->icon_win, iw, ih, 0, 0, STATE_NORMAL,
				  &pmap, &mask);
		  EShapeCombineMask(disp, ib->icon_win, ShapeBounding, 0, 0, mask, ShapeSet);
		  PastePixmap(disp, ib->pmap, pmap, mask, 0, 0);
		  Imlib_free_pixmap(id, pmap);
		  Imlib_free_pixmap(id, mask);
	       }
	  }
	else
	  {
	     int                 iw, ih;
	     GC                  gc;
	     XGCValues           gcv;

	     GetWinWH(ib->icon_win, (unsigned int *)&iw, (unsigned int *)&ih);
	     m = ECreatePixmap(disp, ib->icon_win, iw, ih, 1);
	     gc = XCreateGC(disp, m, 0, &gcv);
	     XSetForeground(disp, gc, 0);
	     XFillRectangle(disp, m, gc, 0, 0, iw, ih);
	     XFreeGC(disp, gc);
	  }

	x = -ib->pos;
	y = 0;
	if (ic)
	  {
	     x += ic->padding.left;
	     y += ic->padding.top;
	  }
	for (i = 0; i < ib->num_icons; i++)
	  {
	     int                 w, h;
	     EWin               *ewin;

	     w = 8;
	     h = 8;
	     ewin = ib->icons[i];
	     if (!ewin->icon_pmap)
		UpdateAppIcon(ewin, mode.icon_mode);
	     if (ewin->icon_pmap)
	       {
		  w = ewin->icon_pmap_w;
		  h = ewin->icon_pmap_h;
		  if (ib->draw_icon_base)
		    {
		       IB_PasteDefaultBase(ib->pmap, x, y, ib->iconsize, ib->iconsize);
		       if (ib->nobg)
			  IB_PasteDefaultBaseMask(m, x, y, ib->iconsize, ib->iconsize);
		    }
		  if (ib->draw_icon_base)
		     PastePixmap(disp, ib->pmap,
				 ewin->icon_pmap,
				 ewin->icon_mask, x + ((ib->iconsize - w) / 2), y + ((ib->iconsize - h) / 2));
		  else
		     PastePixmap(disp, ib->pmap,
				 ewin->icon_pmap,
			    ewin->icon_mask, x, y + ((ib->iconsize - h) / 2));
		  if (ib->nobg)
		     PasteMask(disp, m, ewin->icon_mask, x, y + ((ib->iconsize - h) / 2), w, h);
	       }
	     if (ib->draw_icon_base)
		x += ib->iconsize;
	     else
		x += w + 2;
	  }
     }
   if (ib->nobg)
     {
	EShapeCombineMask(disp, ib->icon_win, ShapeBounding, 0, 0, m, ShapeSet);
	EFreePixmap(disp, m);
	if (ib->num_icons == 0)
	   EMoveWindow(disp, ib->icon_win,
		       -ib->w, -ib->h);
     }
   ESetWindowBackgroundPixmap(disp, ib->icon_win, ib->pmap);
   XClearWindow(disp, ib->icon_win);
   PropagateShapes(ib->win);
   ICCCM_GetShapeInfo(ib->ewin);
   PropagateShapes(ib->ewin->win);
   queue_up = pq;
}

void
IconboxResize(Iconbox * ib, int w, int h)
{
   if ((ib->w == w) && (ib->h == h))
      return;
   EResizeWindow(disp, ib->win, w, h);
   EFreePixmap(disp, ib->pmap);
   ib->pmap = ECreatePixmap(disp, ib->icon_win, w, h, id->x.depth);
   ib->w = w;
   ib->h = h;
   RedrawIconbox(ib);
}

void
IB_Scroll(Iconbox * ib, int dir)
{
   ib->pos += dir;
   IB_FixPos(ib);
   RedrawIconbox(ib);
}

void
IB_ShowMenu(Iconbox * ib, int x, int y)
{
   static Menu        *p_menu = NULL;
   MenuItem           *mi;
   char                s[1024];

   if (p_menu)
      DestroyMenu(p_menu);
   p_menu = CreateMenu();

   AddTitleToMenu(p_menu, "Iconbox Options");
   p_menu->name = duplicate("__IBOX_MENU");
   p_menu->style = FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
   Esnprintf(s, sizeof(s), "iconbox %s", ib->name);
   mi = CreateMenuItem("This Iconbox Settings...", NULL, ACTION_CONFIG, s, NULL);
   AddItemToMenu(p_menu, mi);
   mi = CreateMenuItem("Close Iconbox", NULL, ACTION_KILL, NULL, NULL);
   AddItemToMenu(p_menu, mi);
   mi = CreateMenuItem("Create New Iconbox", NULL, ACTION_CREATE_ICONBOX, NULL, NULL);
   AddItemToMenu(p_menu, mi);
   AddItem(p_menu, p_menu->name, 0, LIST_TYPE_MENU);
   Esnprintf(s, sizeof(s), "named %s", p_menu->name);
   spawnMenu(s);
   x = 0;
   y = 0;
}

void
IB_CompleteRedraw(Iconbox * ib)
{
   ib->ewin->client.width.min = 8;
   ib->ewin->client.height.min = 8;
   ib->ewin->client.width.max = 16384;
   ib->ewin->client.height.max = 16384;
   ib->ewin->client.no_resize_h = 0;
   ib->ewin->client.no_resize_v = 0;
   if (ib->orientation)
     {
	ImageClass         *ic;
	int                 extra = 0;

	ic = FindItem("ICONBOX_VERTICAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if (ic)
	   extra = ic->padding.left + ic->padding.right;
	ib->ewin->client.width.max =
	   ib->ewin->client.width.min =
	   ib->iconsize + ib->scroll_thickness + extra;
	ib->ewin->client.no_resize_h = 1;
     }
   else
     {
	ImageClass         *ic;
	int                 extra = 0;

	ic = FindItem("ICONBOX_HORIZONTAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	if (ic)
	   extra = ic->padding.left + ic->padding.right;
	ib->ewin->client.height.max =
	   ib->ewin->client.height.min =
	   ib->iconsize + ib->scroll_thickness + extra;
	ib->ewin->client.no_resize_v = 1;
     }
   RedrawIconbox(ib);
   ResizeEwin(ib->ewin, ib->ewin->client.w, ib->ewin->client.h);

   SnapshotEwinBorder(ib->ewin);
   SnapshotEwinDesktop(ib->ewin);
   SnapshotEwinSize(ib->ewin);
   SnapshotEwinLocation(ib->ewin);
   SnapshotEwinLayer(ib->ewin);
   SnapshotEwinSticky(ib->ewin);
   SnapshotEwinShade(ib->ewin);
}

void
IB_Setup(void)
{
   EWin              **lst;
   int                 i, num;
   Iconbox           **ibl;

   IcondefChecker(0, NULL);
   ibl = ListAllIconboxes(&num);
   if (ibl)
     {
	for (i = 0; i < num; i++)
	   ShowIconbox(ibl[i]);
	Efree(ibl);
     }
   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     if (lst[i]->client.start_iconified)
		IconifyEwin(lst[i]);
	  }
	Efree(lst);
     }
}

void
IconboxHandleEvent(XEvent * ev)
{
   Iconbox           **ib;
   int                 i, num;

   if (mode.mode != MODE_NONE)
      return;
   ib = ListAllIconboxes(&num);
   if (!ib)
      return;
   for (i = 0; i < num; i++)
     {
	if (ev->xany.window == ib[i]->scroll_win)
	  {
	     if (ev->type == ButtonPress)
	       {
		  if (ev->xbutton.button == 1)
		     ib[i]->scrollbox_clicked = 1;
		  else if (ev->xbutton.button == 3)
		     IB_ShowMenu(ib[i], ev->xbutton.x, ev->xbutton.y);
	       }
	     else if ((ev->type == ButtonRelease) && (ib[i]->scrollbox_clicked))
	       {
		  int                 x, y, w, h;

		  ib[i]->scrollbox_clicked = 0;
		  GetWinXY(ib[i]->scrollbar_win, &x, &y);
		  GetWinWH(ib[i]->scrollbar_win, (unsigned int *)&w, (unsigned int *)&h);
		  if (ev->xbutton.x < x)
		     IB_Scroll(ib[i], -8);
		  if (ev->xbutton.x > (x + w))
		     IB_Scroll(ib[i], 8);
	       }
	  }
	if (ev->xany.window == ib[i]->scrollbar_win)
	  {
	     static int          px, py;

	     if (ev->type == ButtonPress)
	       {
		  if (ev->xbutton.button == 1)
		    {
		       px = ev->xbutton.x_root;
		       py = ev->xbutton.y_root;
		       ib[i]->scrollbar_clicked = 1;
		    }
		  else if (ev->xbutton.button == 3)
		     IB_ShowMenu(ib[i], ev->xbutton.x, ev->xbutton.y);
	       }
	     else if ((ev->type == ButtonRelease) && (ib[i]->scrollbar_clicked))
		ib[i]->scrollbar_clicked = 0;
	     else if (ev->type == EnterNotify)
		ib[i]->scrollbar_hilited = 1;
	     else if (ev->type == LeaveNotify)
		ib[i]->scrollbar_hilited = 0;
	     else if ((ev->type == MotionNotify) && (ib[i]->scrollbar_clicked))
	       {
		  int                 dx, dy, bs, x, y;
		  ImageClass         *ic;

		  dx = ev->xmotion.x_root - px;
		  dy = ev->xmotion.y_root - py;
		  px = ev->xmotion.x_root;
		  py = ev->xmotion.y_root;

		  if (ib[i]->orientation)
		    {
		       ic = FindItem("ICONBOX_SCROLLBAR_BASE_VERTICAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		       GetWinXY(ib[i]->scrollbar_win, &x, &y);
		       bs = ib[i]->h - (ib[i]->arrow_thickness * 2);
		       if (ic)
			 {
			    bs -= (ic->padding.top + ic->padding.bottom);
			    y -= ic->padding.top;
			 }
		       if (bs < 1)
			  bs = 1;
		       ib[i]->pos = ((y + dy + 1) * ib[i]->max) / bs;
		       IB_FixPos(ib[i]);
		       RedrawIconbox(ib[i]);
		    }
		  else
		    {
		       ic = FindItem("ICONBOX_SCROLLBAR_BASE_HORIZONTAL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		       GetWinXY(ib[i]->scrollbar_win, &x, &y);
		       bs = ib[i]->w - (ib[i]->arrow_thickness * 2);
		       if (ic)
			 {
			    bs -= (ic->padding.left + ic->padding.right);
			    x -= ic->padding.left;
			 }
		       if (bs < 1)
			  bs = 1;
		       ib[i]->pos = ((x + dx + 1) * ib[i]->max) / bs;
		       IB_FixPos(ib[i]);
		       RedrawIconbox(ib[i]);
		    }
	       }
	     IB_DrawScroll(ib[i]);
	  }
	else if (ev->xany.window == ib[i]->arrow1_win)
	  {
	     if (ev->type == ButtonPress)
	       {
		  if (ev->xbutton.button == 1)
		     ib[i]->arrow1_clicked = 1;
		  else if (ev->xbutton.button == 3)
		     IB_ShowMenu(ib[i], ev->xbutton.x, ev->xbutton.y);
	       }
	     else if ((ev->type == ButtonRelease) && (ib[i]->arrow1_clicked))
	       {
		  ib[i]->arrow1_clicked = 0;
		  IB_Scroll(ib[i], -8);
	       }
	     else if (ev->type == EnterNotify)
		ib[i]->arrow1_hilited = 1;
	     else if (ev->type == LeaveNotify)
		ib[i]->arrow1_hilited = 0;
	     IB_DrawScroll(ib[i]);
	  }
	else if (ev->xany.window == ib[i]->arrow2_win)
	  {
	     if (ev->type == ButtonPress)
	       {
		  if (ev->xbutton.button == 1)
		     ib[i]->arrow2_clicked = 1;
		  else if (ev->xbutton.button == 3)
		     IB_ShowMenu(ib[i], ev->xbutton.x, ev->xbutton.y);
	       }
	     else if ((ev->type == ButtonRelease) && (ib[i]->arrow2_clicked))
	       {
		  ib[i]->arrow2_clicked = 0;
		  IB_Scroll(ib[i], 8);
	       }
	     else if (ev->type == EnterNotify)
		ib[i]->arrow2_hilited = 1;
	     else if (ev->type == LeaveNotify)
		ib[i]->arrow2_hilited = 0;
	     IB_DrawScroll(ib[i]);
	  }
	else if (ev->xany.window == ib[i]->icon_win)
	  {
	     static EWin        *name_ewin = NULL;

	     if ((ev->type == MotionNotify) || (ev->type == EnterNotify))
	       {
		  EWin               *ewin = NULL;
		  ToolTip            *tt = NULL;

		  if (ev->type == MotionNotify)
		    {
		       ewin = IB_FindIcon(ib[i], ev->xmotion.x, ev->xmotion.y);
		       mode.x = ev->xmotion.x_root;
		       mode.y = ev->xmotion.y_root;
		    }
		  else
		    {
		       ewin = IB_FindIcon(ib[i], ev->xcrossing.x, ev->xcrossing.y);
		       mode.x = ev->xcrossing.x_root;
		       mode.y = ev->xcrossing.y_root;
		    }
		  if (ewin != name_ewin)
		    {
		       tt = FindItem("ICONBOX", 0, LIST_FINDBY_NAME,
				     LIST_TYPE_TOOLTIP);
		       if (tt)
			 {
			    name_ewin = ewin;
			    HideToolTip(tt);
			    if (ewin)
			      {
				 if ((ewin->client.icon_name) &&
				     (strlen(ewin->client.icon_name) > 0))
				    ShowToolTip(tt, ewin->client.icon_name,
						NULL, mode.x, mode.y);
				 else
				    ShowToolTip(tt, ewin->client.title,
						NULL, mode.x, mode.y);
			      }
			 }
		    }

	       }
	     else if (ev->type == LeaveNotify)
	       {
		  ToolTip            *tt = NULL;

		  tt = FindItem("ICONBOX", 0, LIST_FINDBY_NAME,
				LIST_TYPE_TOOLTIP);
		  if (tt)
		    {
		       HideToolTip(tt);
		       name_ewin = NULL;
		    }
	       }
	     else if (ev->type == ButtonPress)
	       {
		  if (ev->xbutton.button == 1)
		     ib[i]->icon_clicked = 1;
		  else if (ev->xbutton.button == 3)
		     IB_ShowMenu(ib[i], ev->xbutton.x, ev->xbutton.y);
	       }
	     else if ((ev->type == ButtonRelease) && (ib[i]->icon_clicked))
	       {
		  EWin               *ewin;
		  EWin              **gwins;
		  int                 j, num;
		  char                iconified;

		  ib[i]->icon_clicked = 0;
		  ewin = IB_FindIcon(ib[i], ev->xbutton.x, ev->xbutton.y);
		  if (ewin)
		    {
		       ToolTip            *tt = NULL;

		       tt = FindItem("ICONBOX", 0, LIST_FINDBY_NAME,
				     LIST_TYPE_TOOLTIP);
		       if (tt)
			  HideToolTip(tt);
		       gwins = ListWinGroupMembersForEwin(ewin, ACTION_ICONIFY, &num);
		       iconified = ewin->iconified;

		       if (gwins)
			 {
			    for (j = 0; j < num; j++)
			      {
				 if ((gwins[j]->iconified) && (iconified))
				    DeIconifyEwin(gwins[j]);
			      }
			    Efree(gwins);
			 }
		    }
	       }
	  }
     }
   Efree(ib);
}
