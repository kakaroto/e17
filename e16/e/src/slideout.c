/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "E.h"

struct _slideout
{
   char               *name;
   char                direction;
   int                 num_buttons;
   Button            **button;
   int                 w, h;
   Window              win;
   Window              from_win;
   unsigned int        ref_count;
};

static void         SlideoutCalcSize(Slideout * s);

static void
SlideWindowSizeTo(Window win, int fx, int fy, int tx, int ty, int fw, int fh,
		  int tw, int th, int speed)
{
   int                 k, x, y, w, h;

   ecore_x_grab();

   ETimedLoopInit(0, 1024, speed);
   for (k = 0; k <= 1024;)
     {
	x = ((fx * (1024 - k)) + (tx * k)) >> 10;
	y = ((fy * (1024 - k)) + (ty * k)) >> 10;
	w = ((fw * (1024 - k)) + (tw * k)) >> 10;
	h = ((fh * (1024 - k)) + (th * k)) >> 10;
	EMoveResizeWindow(win, x, y, w, h);

	k = ETimedLoopNext();
     }
   EMoveResizeWindow(win, tx, ty, tw, th);

   ecore_x_ungrab();
}

static Slideout    *
SlideoutCreate(char *name, char dir)
{
   Slideout           *s;

   s = Emalloc(sizeof(Slideout));
   if (!s)
      return NULL;

   s->name = Estrdup(name);
   s->direction = dir;
   s->num_buttons = 0;
   s->button = NULL;
   s->w = 0;
   s->h = 0;
   s->win = ECreateWindow(VRoot.win, -10, -10, 1, 1, 1);
   s->from_win = 0;
   s->ref_count = 0;

   return s;
}

void
SlideoutShow(Slideout * s, EWin * ewin, Window win)
{
   int                 x, y, i, xx, yy, di;
   Window              dw;
   char                pdir;
   XSetWindowAttributes att;
   int                 w, h, d;

   /* Don't ever show more than one slideout */
   if (Mode.slideout)
      return;

   SlideoutCalcSize(s);
   EGetGeometry(win, &dw, &di, &di, &w, &h, &d, &d);
   XTranslateCoordinates(disp, win, VRoot.win, 0, 0, &x, &y, &dw);

   xx = 0;
   yy = 0;
   switch (s->direction)
     {
     case 2:
	xx = x + ((w - s->w) >> 1);
	yy = y - s->h;
	if ((yy < 0) && (s->h < VRoot.h))
	  {
	     pdir = s->direction;
	     s->direction = 1;
	     SlideoutShow(s, ewin, win);
	     s->direction = pdir;
	     return;
	  }
	break;
     case 3:
	xx = x + ((w - s->w) >> 1);
	yy = y + h;
	if (((yy + s->h) > VRoot.h) && (s->h < VRoot.h))
	  {
	     pdir = s->direction;
	     s->direction = 0;
	     SlideoutShow(s, ewin, win);
	     s->direction = pdir;
	     return;
	  }
	break;
     case 0:
	xx = x - s->w;
	yy = y + ((h - s->h) >> 1);
	if ((xx < 0) && (s->w < VRoot.w))
	  {
	     pdir = s->direction;
	     s->direction = 1;
	     SlideoutShow(s, ewin, win);
	     s->direction = pdir;
	     return;
	  }
	break;
     case 1:
	xx = x + w;
	yy = y + ((h - s->h) >> 1);
	if (((xx + s->w) > VRoot.w) && (s->w < VRoot.w))
	  {
	     pdir = s->direction;
	     s->direction = 0;
	     SlideoutShow(s, ewin, win);
	     s->direction = pdir;
	     return;
	  }
	break;
     default:
	break;
     }

   /* If the slideout is associated with an ewin,
    * put it on the same virtual desktop. */
   dw = VRoot.win;
   if (ewin && BorderWinpartIndex(ewin, win) >= 0 &&
       !EoIsFloating(ewin) /* && !ewin->sticky */ )
     {
	int                 desk = EoGetDesk(ewin);

	xx -= DeskGetX(desk);
	yy -= DeskGetY(desk);
	dw = DeskGetWin(desk);
     }
   EReparentWindow(s->win, dw, xx, yy);

   switch (s->direction)
     {
     case 0:
	att.win_gravity = SouthEastGravity;
	EChangeWindowAttributes(s->win, CWWinGravity, &att);
	att.win_gravity = NorthWestGravity;
	for (i = 0; i < s->num_buttons; i++)
	   EChangeWindowAttributes(ButtonGetWindow(s->button[i]),
				   CWWinGravity, &att);
	EMoveResizeWindow(s->win, xx, yy, 1, 1);
	ecore_x_sync();
	EMapRaised(s->win);
	SlideWindowSizeTo(s->win, xx + s->w, yy, xx, yy, 0, s->h, s->w, s->h,
			  Conf.slidespeedmap);
	break;
     case 1:
	att.win_gravity = NorthWestGravity;
	EChangeWindowAttributes(s->win, CWWinGravity, &att);
	att.win_gravity = SouthEastGravity;
	for (i = 0; i < s->num_buttons; i++)
	   EChangeWindowAttributes(ButtonGetWindow(s->button[i]),
				   CWWinGravity, &att);
	EMoveResizeWindow(s->win, xx, yy, 1, 1);
	ecore_x_sync();
	EMapRaised(s->win);
	SlideWindowSizeTo(s->win, xx, yy, xx, yy, 0, s->h, s->w, s->h,
			  Conf.slidespeedmap);
	break;
     case 2:
	att.win_gravity = SouthEastGravity;
	EChangeWindowAttributes(s->win, CWWinGravity, &att);
	att.win_gravity = NorthWestGravity;
	for (i = 0; i < s->num_buttons; i++)
	   EChangeWindowAttributes(ButtonGetWindow(s->button[i]),
				   CWWinGravity, &att);
	EMoveResizeWindow(s->win, xx, yy, 1, 1);
	ecore_x_sync();
	EMapRaised(s->win);
	SlideWindowSizeTo(s->win, xx, yy + s->h, xx, yy, s->w, 0, s->w, s->h,
			  Conf.slidespeedmap);
	break;
     case 3:
	att.win_gravity = NorthWestGravity;
	EChangeWindowAttributes(s->win, CWWinGravity, &att);
	att.win_gravity = SouthEastGravity;
	for (i = 0; i < s->num_buttons; i++)
	   EChangeWindowAttributes(ButtonGetWindow(s->button[i]),
				   CWWinGravity, &att);
	EMoveResizeWindow(s->win, xx, yy, 1, 1);
	ecore_x_sync();
	EMapRaised(s->win);
	SlideWindowSizeTo(s->win, xx, yy, xx, yy, s->w, 0, s->w, s->h,
			  Conf.slidespeedmap);
	break;
     default:
	break;
     }
   s->from_win = win;
   s->ref_count++;

   Mode.slideout = s;
}

void
SlideoutHide(Slideout * s)
{
   if (!s)
      return;

   EUnmapWindow(s->win);
   s->from_win = 0;
   s->ref_count--;
   Mode.slideout = NULL;
}

static void
SlideoutCalcSize(Slideout * s)
{
   int                 i;
   int                 mx, my, x, y;
   int                 bw, bh;

   if (!s)
      return;

   mx = 0;
   my = 0;
   x = 0;
   y = 0;
   for (i = 0; i < s->num_buttons; i++)
     {
	bw = ButtonGetWidth(s->button[i]);
	bh = ButtonGetHeight(s->button[i]);

	switch (s->direction)
	  {
	  case 2:
	  case 3:
	     if (bw > mx)
		mx = bw;
	     my += bh;
	     break;
	  case 0:
	  case 1:
	     if (bh > my)
		my = bh;
	     mx += bw;
	     break;
	  default:
	     break;
	  }
     }

   EResizeWindow(s->win, mx, my);
   s->w = mx;
   s->h = my;

   for (i = 0; i < s->num_buttons; i++)
     {
	bw = ButtonGetWidth(s->button[i]);
	bh = ButtonGetHeight(s->button[i]);

	switch (s->direction)
	  {
	  case 2:
	     y += bh;
	     EMoveWindow(ButtonGetWindow(s->button[i]),
			 (s->w - bw) >> 1, s->h - y);
	     break;
	  case 3:
	     EMoveWindow(ButtonGetWindow(s->button[i]), (s->w - bw) >> 1, y);
	     y += bh;
	     break;
	  case 0:
	     x += bw;
	     EMoveWindow(ButtonGetWindow(s->button[i]), s->w - x,
			 (s->h - bh) >> 1);
	     break;
	  case 1:
	     EMoveWindow(ButtonGetWindow(s->button[i]), x, (s->h - bh) >> 1);
	     x += bw;
	  default:
	     break;
	  }
     }
   PropagateShapes(s->win);
}

static void
SlideoutAddButton(Slideout * s, Button * b)
{
   if (!b)
      return;
   if (!s)
      return;

   s->num_buttons++;
   s->button = Erealloc(s->button, sizeof(Button *) * s->num_buttons);
   s->button[s->num_buttons - 1] = b;
   EReparentWindow(ButtonGetWindow(b), s->win, 0, 0);
   ButtonSetSwallowed(b);
   ButtonShow(b);
   SlideoutCalcSize(s);
}

#if 0
static void
SlideoutRemoveButton(Slideout * s, Button * b)
{
   s = NULL;
   b = NULL;
}
#endif

static const char  *
SlideoutGetName(Slideout * s)
{
   return s->name;
}

#if 0
static EWin        *
SlideoutsGetContextEwin(void)
{
   if (Mode.slideout)
      return FindEwinByChildren(Mode.slideout->from_win);

   return NULL;
}
#endif

void
SlideoutsHide(void)
{
   if (Mode.slideout)
      SlideoutHide(Mode.slideout);
}

#if 0
static void
SlideoutsHideIfContextWin(Window win)
{
   if ((Mode.slideout) && (Mode.slideout->from_win == win))
      SlideoutHide(Mode.slideout);
}
#endif

/*
 * Configuration load/save
 */
#include "conf.h"
int
SlideoutsConfigLoad(FILE * fs)
{
   int                 err = 0;
   Slideout           *slideout = 0;
   int                 i1;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   char               *name = 0;
   int                 fields;

   while (GetLine(s, sizeof(s), fs))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	  {
	     i1 = CONFIG_INVALID;
	  }
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
		  i1 = CONFIG_INVALID;
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     if (slideout)
		AddItem(slideout, SlideoutGetName(slideout), 0,
			LIST_TYPE_SLIDEOUT);
	     goto done;
	  case CONFIG_CLASSNAME:
	     if (name)
		Efree(name);
	     name = Estrdup(s2);
	     break;
	  case SLIDEOUT_DIRECTION:
	     slideout = SlideoutCreate(name, (char)atoi(s2));
	     if (name)
		Efree(name);
	     break;
	  case CONFIG_BUTTON:
	     {
		Button             *b;

		b = (Button *) FindItem(s2, 0, LIST_FINDBY_NAME,
					LIST_TYPE_BUTTON);
		if (b)
		   SlideoutAddButton(slideout, b);
	     }
	     break;
	  default:
	     Alert(_("Warning: unable to determine what to do with\n"
		     "the following text in the middle of current Text "
		     "definition:\n" "%s\nWill ignore and continue...\n"), s);
	  }

     }
   err = -1;

 done:
   return err;
}

/*
 * Slideouts Module
 */

static void
SlideoutsSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_AREA_SWITCH_START:
     case ESIGNAL_DESK_SWITCH_START:
	SlideoutsHide();
	break;
     }
}

/*
 * Module descriptor
 */
EModule             ModSlideouts = {
   "slideouts", "slideout",
   SlideoutsSighan,
   {0, NULL}
   ,
   {0, NULL}
};
