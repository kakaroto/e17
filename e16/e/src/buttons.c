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
#define DECLARE_STRUCT_BUTTON
#include "E.h"

Button             *
ButtonCreate(char *name, ImageClass * iclass, ActionClass * aclass,
	     TextClass * tclass, char *label, char ontop, int flags,
	     int minw, int maxw, int minh, int maxh, int xo, int yo,
	     int xa, int xr, int ya, int yr, int xsr, int xsa, int ysr,
	     int ysa, char simg, int desk, char sticky)
{
   Button             *b;

   EDBUG(5, "ButtonCreate");

   b = Emalloc(sizeof(Button));

   b->name = duplicate(name);
   b->label = duplicate(label);

   b->iclass = iclass;
   if (!b->iclass)
      b->iclass =
	 FindItem("__FALLBACK_ICLASS", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (b->iclass)
      b->iclass->ref_count++;

   b->aclass = aclass;
   if (b->aclass)
      b->aclass->ref_count++;

   b->tclass = tclass;

   if ((!b->tclass) && (b->label))
     {
	b->tclass =
	   FindItem("__FALLBACK_TCLASS", 0, LIST_FINDBY_NAME, LIST_TYPE_TCLASS);
     }
   if (b->tclass)
      b->tclass->ref_count++;

   b->label = label;

   b->ontop = ontop;
   b->flags = flags;
   b->sticky = sticky;
   b->desktop = desk;
   b->visible = 0;
   b->geom.width.min = minw;
   b->geom.width.max = maxw;
   b->geom.height.min = minh;
   b->geom.height.max = maxh;
   b->geom.xorigin = xo;
   b->geom.yorigin = yo;
   b->geom.xabs = xa;
   b->geom.xrel = xr;
   b->geom.yabs = ya;
   b->geom.yrel = yr;
   b->geom.xsizeabs = xsa;
   b->geom.xsizerel = xsr;
   b->geom.ysizeabs = ysa;
   b->geom.ysizerel = ysr;
   b->geom.size_from_image = simg;
   b->inside_win = 0;
   b->event_win = 0;
   b->destroy_inside_win = 0;
   b->internal = 0;
   b->default_show = 1;
   b->used = 0;
   b->left = 0;
   b->win =
      ECreateWindow(desks.desk[desk % ENLIGHTENMENT_CONF_NUM_DESKTOPS].win,
		    -100, -100, 50, 50, 0);
   XSelectInput(disp, b->win,
		ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask
		| ButtonReleaseMask | EnterWindowMask | LeaveWindowMask |
		PointerMotionMask);
   b->x = -1;
   b->y = -1;
   b->w = -1;
   b->h = -1;
   b->cx = -10;
   b->cy = -10;
   b->cw = -10;
   b->ch = -10;
   b->state = 0;
   b->expose = 0;
   b->ref_count = 0;

   EDBUG_RETURN(b);
}

void
ButtonDestroy(Button * b)
{
   EDBUG(5, "ButtonDestroy");

   if (!b)
      EDBUG_RETURN_;

   if (b->ref_count > 0)
     {
	DialogOK(_("Button Error!"), _("%u references remain\n"), b->ref_count);
	EDBUG_RETURN_;
     }

   while (RemoveItemByPtr(b, LIST_TYPE_BUTTON));

   if (b->name)
      Efree(b->name);

   if (b->win)
      EDestroyWindow(disp, b->win);

   if (b->iclass)
      b->iclass->ref_count--;

   if (b->aclass)
      b->aclass->ref_count--;

   if (b->tclass)
      b->tclass->ref_count--;

   if (b->label)
      Efree(b->label);

   Efree(b);

   EDBUG_RETURN_;
}

static void
ButtonCalc(Button * b)
{
   int                 w, h, x, y, xo, yo;
   Imlib_Image        *im;

   EDBUG(4, "ButtonCalc");
   x = 0;
   y = 0;
   w = 32;
   h = 32;
   if (b->geom.size_from_image)
     {
	if ((b->iclass) && (b->iclass->norm.normal->im_file))
	  {
	     im = ELoadImage(b->iclass->norm.normal->im_file);
	     if (im)
	       {
		  imlib_context_set_image(im);
		  w = imlib_image_get_width();
		  h = imlib_image_get_height();
		  imlib_free_image();
	       }
	     else
	       {
		  w = 32;
		  h = 32;
	       }
	  }
	else
	  {
	     if (!b->iclass)
		b->iclass =
		   FindItem("__FALLBACK_ICLASS", 0, LIST_FINDBY_NAME,
			    LIST_TYPE_ICLASS);
	     w = 32;
	     h = 32;
	  }
     }
   else
     {
	w = ((b->geom.xsizerel * root.w) >> 10) + b->geom.xsizeabs;
	h = ((b->geom.ysizerel * root.h) >> 10) + b->geom.ysizeabs;
     }
   if (w > b->geom.width.max)
      w = b->geom.width.max;
   else if (w < b->geom.width.min)
      w = b->geom.width.min;
   if (h > b->geom.height.max)
      h = b->geom.height.max;
   else if (h < b->geom.height.min)
      h = b->geom.height.min;
   xo = (w * b->geom.xorigin) >> 10;
   yo = (h * b->geom.yorigin) >> 10;
   x = ((b->geom.xrel * root.w) >> 10) + b->geom.xabs - xo;
   y = ((b->geom.yrel * root.h) >> 10) + b->geom.yabs - yo;
   b->x = x;
   b->y = y;
   b->w = w;
   b->h = h;

   EDBUG_RETURN_;
}

void
ButtonShow(Button * b)
{
   char                move, resize;

   EDBUG(4, "ButtonShow");

   ButtonCalc(b);

   move = 0;
   resize = 0;
   if ((b->x != b->cx) || (b->y != b->cy))
      move = 1;
   if ((b->w != b->cw) || (b->h != b->ch))
      resize = 1;

   if ((move) && (resize))
      EMoveResizeWindow(disp, b->win, b->x, b->y, b->w, b->h);
   else if (move)
      EMoveWindow(disp, b->win, b->x, b->y);
   else if (resize)
      EResizeWindow(disp, b->win, b->w, b->h);
   if (b->sticky)
      XRaiseWindow(disp, b->win);

   ButtonDraw(b);
   b->visible = 1;
   EMapWindow(disp, b->win);
   b->cx = b->x;
   b->cy = b->y;
   b->cw = b->w;
   b->ch = b->h;

   EDBUG_RETURN_;
}

void
ButtonMoveToDesktop(Button * b, int num)
{
   EDBUG(3, "ButtonMoveToDesktop");

   if (b->sticky)
     {
	b->desktop = 0;
	EReparentWindow(disp, b->win, desks.desk[0].win, b->x, b->y);
	XRaiseWindow(disp, b->win);
     }
   else
     {
	b->desktop = num;
	EReparentWindow(disp, b->win, desks.desk[DESKTOPS_WRAP_NUM(num)].win,
			b->x, b->y);
     }

   EDBUG_RETURN_;
}

void
ButtonHide(Button * b)
{
   EDBUG(3, "ButtonHide");

   EUnmapWindow(disp, b->win);
   b->visible = 0;

   EDBUG_RETURN_;
}

void
ButtonToggle(Button * b)
{
   EDBUG(3, "ButtonToggle");

   if (b->used)
      EDBUG_RETURN_;

   if (b->visible)
      ButtonHide(b);
   else
      ButtonShow(b);

   EDBUG_RETURN_;
}

void
ButtonDraw(Button * b)
{
   EDBUG(3, "ButtonDraw");

   IclassApply(b->iclass, b->win, b->w, b->h, 0, 0, b->state, 0);

   if (b->label)
      TclassApply(b->iclass, b->win, b->w, b->h, 0, 0, b->state, 0, b->tclass,
		  b->label);
   EDBUG_RETURN_;
}

void
ButtonDrawWithState(Button * b, int state)
{
   b->state = state;
   ButtonDraw(b);
}

void
ButtonMoveToCoord(Button * b, int x, int y)
{
   int                 rx, ry, relx, rely, absx, absy;
   char                move, resize;

   EDBUG(3, "ButtonMoveToCoord");
   if (b->flags & FLAG_FIXED)
      EDBUG_RETURN_;

   if ((x + (b->w >> 1)) < (root.w / 3))
      relx = 0;
   else if ((x + (b->w >> 1)) > ((root.w * 2) / 3))
      relx = 1024;
   else
      relx = 512;
   rx = (relx * root.w) >> 10;
   absx = x - rx;
   if ((y + (b->h >> 1)) < (root.h / 3))
      rely = 0;
   else if ((y + (b->h >> 1)) > ((root.h * 2) / 3))
      rely = 1024;
   else
      rely = 512;
   ry = (rely * root.h) >> 10;
   absy = y - ry;
   if (!(b->flags & FLAG_FIXED_HORIZ))
     {
	b->geom.xorigin = 0;
	b->geom.xabs = absx;
	b->geom.xrel = relx;
     }
   if (!(b->flags & FLAG_FIXED_VERT))
     {
	b->geom.yorigin = 0;
	b->geom.yabs = absy;
	b->geom.yrel = rely;
     }
   ButtonCalc(b);
   move = 0;
   resize = 0;
   if ((b->x != b->cx) || (b->y != b->cy))
      move = 1;
   if ((b->w != b->cw) || (b->h != b->ch))
      resize = 1;
   if ((move) && (resize))
      EMoveResizeWindow(disp, b->win, b->x, b->y, b->w, b->h);
   else if (move)
      EMoveWindow(disp, b->win, b->x, b->y);
   else if (resize)
      EResizeWindow(disp, b->win, b->w, b->h);
   if (b->sticky)
      XRaiseWindow(disp, b->win);
   b->cx = b->x;
   b->cy = b->y;
   b->cw = b->w;
   b->ch = b->h;

   EDBUG_RETURN_;
}

void
ButtonMoveRelative(Button * b, int dx, int dy)
{
   ButtonMoveToCoord(b, b->x + dx, b->y + dy);
}

void
ButtonIncRefcount(Button * b)
{
   b->ref_count++;
}

void
ButtonDecRefcount(Button * b)
{
   b->ref_count--;
}

const char         *
ButtonGetName(Button * b)
{
   return b->name;
}

int
ButtonGetRefcount(Button * b)
{
   return b->ref_count;
}

int
ButtonGetDesktop(Button * b)
{
   return b->desktop;
}

void
ButtonGetGeometry(Button * b, int *x, int *y, unsigned int *w, unsigned int *h)
{
   if (x)
      *x = b->x;
   if (y)
      *y = b->y;
   if (w)
      *w = b->w;
   if (h)
      *h = b->h;
}

int
ButtonGetInfo(Button * b, RectBox * r, int desk)
{
   if (!b->visible || b->internal)
      return -1;
   if (!b->sticky && (b->desktop != desk))
      return -1;

   r->data = NULL;
   r->x = b->x;
   r->y = b->y;
   r->w = b->w;
   r->h = b->h;
   r->p = b->sticky;

   return 0;
}

ActionClass        *
ButtonGetAClass(Button * b)
{
   return b->aclass;
}

int
ButtonIsFixed(Button * b)
{
   return b->flags & FLAG_FIXED;
}

int
ButtonEmbedWindow(Button * ButtonToUse, Window WindowToEmbed)
{

   unsigned int        w, h;

   EDBUG(4, "ButtonEmbedWindow");
   EReparentWindow(disp, WindowToEmbed, ButtonToUse->win, 0, 0);
   ButtonToUse->inside_win = WindowToEmbed;
   GetWinWH(WindowToEmbed, &w, &h);
   EMoveWindow(disp, ButtonToUse->inside_win, (ButtonToUse->w - w) >> 1,
	       (ButtonToUse->h - h) >> 1);
   ButtonToUse->event_win = ECreateEventWindow(ButtonToUse->win, 0, 0, w, h);

   XSelectInput(disp, ButtonToUse->event_win,
		ButtonPressMask | ButtonReleaseMask | EnterWindowMask |
		LeaveWindowMask | ButtonMotionMask);

   EMoveWindow(disp, ButtonToUse->event_win, (ButtonToUse->w - w) >> 1,
	       (ButtonToUse->h - h) >> 1);
   EMapRaised(disp, ButtonToUse->event_win);

   EDBUG_RETURN(0);
}

void
ButtonFindEmptySpotFor(Button * bt, char *listname, char dirtomove)
{

   Button            **blst;
   int                 num = 0, i = 0, j = 0, done = 0;

   blst = (Button **) ListItemTypeName(&num, LIST_TYPE_BUTTON, listname);
   if (blst)
     {
	for (i = 0; i < num; i++)
	  {
	     for (j = 0; j < num; j++)
	       {
		  if ((bt->x + bt->w) <= blst[j]->x
		      || bt->x >= (blst[j]->x + blst[j]->w))
		    {
		       done = 1;
		    }
		  else
		    {
		       if ((bt->y + bt->h) <= blst[j]->y
			   || bt->y > (blst[j]->y + blst[j]->h))
			  done = 1;
		       else
			  done = 0;
		    }
		  if (!done)
		    {
		       if (dirtomove == ICON_RIGHT)
			  bt->x += bt->w;
		       else if (dirtomove == ICON_LEFT)
			  bt->x -= bt->w;
		       else if (dirtomove == ICON_DOWN)
			  bt->y += bt->h;
		       else if (dirtomove == ICON_UP)
			  bt->y -= bt->h;
		    }
	       }
	  }
	Efree(blst);
     }
   ButtonMoveToCoord(bt, bt->x, bt->y);

   EDBUG_RETURN_;
}

/*
 * Functions operating on all buttons
 */

Button             *
FindButton(Window win)
{
   Button             *b;
   Button            **buttons;
   int                 i, num;

   EDBUG(6, "FindButton");

   buttons = (Button **) ListItemType(&num, LIST_TYPE_BUTTON);
   for (i = 0; i < num; i++)
     {
	if ((win == buttons[i]->win) || (win == buttons[i]->inside_win)
	    || (win == buttons[i]->event_win))
	  {
	     b = buttons[i];
	     Efree(buttons);
	     EDBUG_RETURN(b);
	  }
     }
   if (buttons)
      Efree(buttons);
   EDBUG_RETURN(NULL);
}

/*
 * Button event handlers
 */

int
ButtonsEventExpose(XEvent * ev)
{
   int                 used = 0;
   int                 i, num;
   Button            **buttons;
   Window              win = ev->xexpose.window;

   buttons = (Button **) ListItemType(&num, LIST_TYPE_BUTTON);
   if (buttons == NULL)
      return 0;

   for (i = 0; i < num; i++)
     {
	if (win != buttons[i]->win)
	   continue;

	ButtonDraw(buttons[i]);
	used = 1;
	break;
     }
   Efree(buttons);

   return used;
}

int
ButtonsEventMouseDown(XEvent * ev)
{
   int                 used = 0;
   int                 i, num;
   Button            **buttons;
   Window              win = ev->xbutton.window;
   ActionClass        *ac;

   buttons = (Button **) ListItemType(&num, LIST_TYPE_BUTTON);
   if (buttons == NULL)
      return 0;

   for (i = 0; i < num; i++)
     {
	if ((win != buttons[i]->win) && (win != buttons[i]->event_win))
	   continue;

	GrabThePointer(win);
	if (buttons[i]->inside_win)
	  {
	     Window              id = ev->xany.window;	/* ??? */

	     ev->xany.window = buttons[i]->inside_win;
	     XSendEvent(disp, buttons[i]->inside_win, False,
			ButtonPressMask, ev);
	     ev->xany.window = id;
	  }

	mode.button = buttons[i];
	buttons[i]->state = STATE_CLICKED;
	ButtonDraw(buttons[i]);
	ac = FindItem("ACTION_BUTTON_DRAG", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ACLASS);
	if (ac)
	   EventAclass(ev, ac);
	if (buttons[i]->aclass)
	   EventAclass(ev, buttons[i]->aclass);
	used = 1;
	break;
     }
   Efree(buttons);

   return used;
}

int
ButtonsEventMouseUp(XEvent * ev)
{
   int                 used = 0;
   int                 i, num;
   Button            **buttons;
   Window              win = ev->xbutton.window;

   buttons = (Button **) ListItemType(&num, LIST_TYPE_BUTTON);
   if (buttons == NULL)
      return 0;

   for (i = 0; i < num; i++)
     {
	if ((win != buttons[i]->win) && (win != buttons[i]->event_win))
	   continue;

	if (buttons[i]->inside_win && !mode.action_inhibit)
	  {
	     Window              id = ev->xany.window;	/* ??? */

	     ev->xany.window = buttons[i]->inside_win;
	     XSendEvent(disp, buttons[i]->inside_win, False,
			ButtonReleaseMask, ev);
	     ev->xany.window = id;
	  }
	mode.button = buttons[i];
	if ((buttons[i]->state == STATE_CLICKED) && (!buttons[i]->left))
	   buttons[i]->state = STATE_HILITED;
	else
	   buttons[i]->state = STATE_NORMAL;
	buttons[i]->left = 0;
	ButtonDraw(buttons[i]);
	if (buttons[i]->aclass)
	   EventAclass(ev, buttons[i]->aclass);
	used = 1;
	break;
     }
   Efree(buttons);

   return used;
}

int
ButtonsEventMouseIn(XEvent * ev)
{
   int                 used = 0;
   int                 i, num;
   Button            **buttons;
   Window              win = ev->xcrossing.window;

   buttons = (Button **) ListItemType(&num, LIST_TYPE_BUTTON);
   if (buttons == NULL)
      return 0;

   for (i = 0; i < num; i++)
     {
	if ((win != buttons[i]->win) && (win != buttons[i]->event_win))
	   continue;

	mode.button = buttons[i];
	if (buttons[i]->state == STATE_CLICKED)
	   buttons[i]->left = 0;
	else
	  {
	     buttons[i]->state = STATE_HILITED;
	     ButtonDraw(buttons[i]);
	     if (buttons[i]->aclass)
		EventAclass(ev, buttons[i]->aclass);
	  }
	used = 1;
	break;
     }
   Efree(buttons);

   return used;
}

int
ButtonsEventMouseOut(XEvent * ev)
{
   int                 used = 0;
   int                 i, num;
   Button            **buttons;
   Window              win = ev->xcrossing.window;

   buttons = (Button **) ListItemType(&num, LIST_TYPE_BUTTON);
   if (buttons == NULL)
      return 0;

   for (i = 0; i < num; i++)
     {
	if ((win != buttons[i]->win) && (win != buttons[i]->event_win))
	   continue;

	mode.button = NULL;
	if (buttons[i]->state == STATE_CLICKED)
	   buttons[i]->left = 1;
	else
	  {
	     buttons[i]->state = STATE_NORMAL;
	     ButtonDraw(buttons[i]);
	     if (buttons[i]->aclass)
		EventAclass(ev, buttons[i]->aclass);
	  }
	used = 1;
	break;
     }
   Efree(buttons);

   return used;
}
