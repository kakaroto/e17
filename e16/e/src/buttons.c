/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2006 Kim Woelders
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
#include "aclass.h"
#include "buttons.h"
#include "desktops.h"
#include "e16-ecore_list.h"
#include "eimage.h"
#include "emodule.h"
#include "iclass.h"
#include "tclass.h"
#include "tooltips.h"
#include "xwin.h"

#define BUTTON_EVENT_MASK \
  (KeyPressMask | KeyReleaseMask | \
   ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | \
   PointerMotionMask)

typedef struct _bgeometry
{
   Constraints         width, height;
   int                 xorigin, yorigin;
   int                 xabs, xrel;
   int                 yabs, yrel;
   int                 xsizerel, xsizeabs;
   int                 ysizerel, ysizeabs;
   char                size_from_image;
}
BGeometry;

struct _button
{
   EObj                o;
   BGeometry           geom;
   ImageClass         *iclass;
   ActionClass        *aclass;
   TextClass          *tclass;
   char               *label;
   int                 id;
   int                 flags;
   char                internal;
   char                default_show;
   EObj               *owner;
   ButtonCbFunc       *func;

   int                 state;
   Window              inside_win;
   Window              event_win;
   char                left;
   unsigned int        ref_count;
};

static Ecore_List  *button_list = NULL;

static struct
{
   Button             *button;
   char                loading_user;
   char                move_pending;
   char                action_inhibit;
   int                 start_x, start_y;
} Mode_buttons;

static void         ButtonHandleEvents(XEvent * ev, void *btn);

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

static int
ButtonIsFixed(const Button * b)
{
   return b->flags & FLAG_FIXED;
}

static int
ButtonIsInternal(const Button * b)
{
   return b->internal;
}

Button             *
ButtonCreate(const char *name, int id, ImageClass * iclass,
	     ActionClass * aclass, TextClass * tclass, const char *label,
	     char ontop, int flags, int minw, int maxw, int minh, int maxh,
	     int xo, int yo, int xa, int xr, int ya, int yr, int xsr, int xsa,
	     int ysr, int ysa, char simg, int desk, char sticky)
{
   Button             *b;

   if (desk < 0 || desk >= (int)DesksGetNumber())
      return NULL;

   if (sticky && ontop == 1)
      desk = 0;

   b = Ecalloc(1, sizeof(Button));

   if (!button_list)
      button_list = ecore_list_new();
   ecore_list_append(button_list, b);

   b->id = id;
   b->label = Estrdup(label);

   b->iclass = iclass;
   if (!b->iclass)
      b->iclass = ImageclassFind(NULL, 0);
   if (b->iclass)
      ImageclassIncRefcount(b->iclass);

   b->aclass = aclass;
   if (b->aclass)
      ActionclassIncRefcount(b->aclass);

   b->tclass = tclass;
   if (!b->tclass && b->label)
      b->tclass = TextclassFind(NULL, 0);
   if (b->tclass)
      TextclassIncRefcount(b->tclass);

   b->flags = flags;
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
   b->default_show = 1;

   EoSetSticky(b, sticky);
   EoSetDesk(b, DeskGet(desk));
   EoInit(b, EOBJ_TYPE_BUTTON, None, -100, -100, 50, 50, 0, name);
   EoSetLayer(b, ontop);
   EoSetShadow(b, 0);

   ESelectInput(EoGetWin(b), BUTTON_EVENT_MASK);
   EventCallbackRegister(EoGetWin(b), 0, ButtonHandleEvents, b);

   return b;
}

void
ButtonDestroy(Button * b)
{
   if (!b)
      return;

   if (b->ref_count > 0)
     {
	DialogOK(_("Button Error!"), _("%u references remain\n"), b->ref_count);
	return;
     }

   ecore_list_remove_node(button_list, b);

   EoFini(b);

   if (b->iclass)
      ImageclassDecRefcount(b->iclass);

   if (b->aclass)
      ActionclassDecRefcount(b->aclass);

   if (b->tclass)
      TextclassDecRefcount(b->tclass);

   if (b->label)
      Efree(b->label);

   Efree(b);
}

static int
_ButtonMatchName(const void *data, const void *match)
{
   return strcmp(((const Button *)data)->o.name, match);
}

Button             *
ButtonFind(const char *name)
{
   return ecore_list_find(button_list, _ButtonMatchName, name);
}

static void
ButtonCalc(Button * b)
{
   int                 w, h, x, y, xo, yo;
   EImage             *im;

   x = 0;
   y = 0;
   w = 32;
   h = 32;
   if (b->geom.size_from_image)
     {
	im = ImageclassGetImage(b->iclass, 0, 0, 0);
	if (im)
	  {
	     EImageGetSize(im, &w, &h);
	     EImageFree(im);
	  }
	else
	  {
	     if (!b->iclass)
		b->iclass = ImageclassFind(NULL, 0);
	     w = 32;
	     h = 32;
	  }
     }
   else
     {
	w = ((b->geom.xsizerel * VRoot.w) >> 10) + b->geom.xsizeabs;
	h = ((b->geom.ysizerel * VRoot.h) >> 10) + b->geom.ysizeabs;
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
   x = ((b->geom.xrel * VRoot.w) >> 10) + b->geom.xabs - xo;
   y = ((b->geom.yrel * VRoot.h) >> 10) + b->geom.yabs - yo;

   EoMoveResize(b, x, y, w, h);
}

static void
ButtonDraw(Button * b)
{
   ITApply(EoGetWin(b), b->iclass, NULL, EoGetW(b), EoGetH(b),
	   b->state, 0, 0, 0, ST_BUTTON, b->tclass, NULL, b->label);
   EoShapeUpdate(b, 0);
}

#if 0				/* Unused */
void
ButtonDrawWithState(Button * b, int state)
{
   b->state = state;
   ButtonDraw(b);
}
#endif

void
ButtonShow(Button * b)
{
   ButtonCalc(b);
   ButtonDraw(b);
   EoMap(b, 0);
}

void
ButtonSwallowInto(Button * b, EObj * eo)
{
   b->internal = 1;
   b->default_show = 0;
   b->flags |= FLAG_FIXED;
   b->owner = eo;
   b->ref_count++;
   EobjReparent(EoObj(b), eo, 0, 0);
   ButtonCalc(b);
   ButtonDraw(b);
   EMapWindow(EoGetWin(b));
}

void
ButtonSetCallback(Button * b, ButtonCbFunc * func, EObj * eo)
{
   b->owner = eo;
   b->func = func;
}

static void
ButtonMoveToDesktop(Button * b, Desk * dsk)
{
   if (EoIsSticky(b) && EoGetLayer(b) == 1)
      dsk = DeskGet(0);

   if (!dsk)
      return;

   if (EoGetDesk(b) != dsk)
      EoReparent(b, EoObj(dsk), EoGetX(b), EoGetY(b));
}

void
ButtonHide(Button * b)
{
   EoUnmap(b);
}

static void
ButtonToggle(Button * b)
{
   if (b->internal)
      return;

   if (EoIsShown(b))
      ButtonHide(b);
   else
      ButtonShow(b);
}

void
ButtonMoveToCoord(Button * b, int x, int y)
{
   int                 rx, ry, relx, rely, absx, absy;

   if (ButtonIsFixed(b))
      return;

   if ((x + (EoGetW(b) >> 1)) < (VRoot.w / 3))
      relx = 0;
   else if ((x + (EoGetW(b) >> 1)) > ((VRoot.w * 2) / 3))
      relx = 1024;
   else
      relx = 512;
   rx = (relx * VRoot.w) >> 10;
   absx = x - rx;
   if ((y + (EoGetH(b) >> 1)) < (VRoot.h / 3))
      rely = 0;
   else if ((y + (EoGetH(b) >> 1)) > ((VRoot.h * 2) / 3))
      rely = 1024;
   else
      rely = 512;
   ry = (rely * VRoot.h) >> 10;
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
}

void
ButtonMoveRelative(Button * b, int dx, int dy)
{
   ButtonMoveToCoord(b, EoGetX(b) + dx, EoGetY(b) + dy);
}

int
ButtonGetInfo(const Button * b, RectBox * r, Desk * desk)
{
   if (!EoIsShown(b) || ButtonIsInternal(b))
      return -1;
   if (!EoIsSticky(b) && EoGetDesk(b) != desk)
      return -1;

   r->data = NULL;
   r->x = EoGetX(b);
   r->y = EoGetY(b);
   r->w = EoGetW(b);
   r->h = EoGetH(b);
   r->p = EoIsSticky(b);

   return 0;
}

int
ButtonDoShowDefault(const Button * b)
{
   return !b->internal && b->default_show;
}

#if 1				/* Unused */
int
ButtonEmbedWindow(Button * b, Window WindowToEmbed)
{

   int                 w, h;

   EReparentWindow(WindowToEmbed, EoGetWin(b), 0, 0);
   b->inside_win = WindowToEmbed;
   EGetGeometry(WindowToEmbed, NULL, NULL, NULL, &w, &h, NULL, NULL);
   EMoveWindow(b->inside_win, (EoGetW(b) - w) >> 1, (EoGetH(b) - h) >> 1);
   b->event_win = ECreateEventWindow(EoGetWin(b), 0, 0, w, h);
   EventCallbackRegister(b->event_win, 0, ButtonHandleEvents, b);

   ESelectInput(b->event_win,
		ButtonPressMask | ButtonReleaseMask | EnterWindowMask |
		LeaveWindowMask | ButtonMotionMask);

   EMoveWindow(b->event_win, (EoGetW(b) - w) >> 1, (EoGetH(b) - h) >> 1);
   EMapRaised(b->event_win);

   return 0;
}
#endif

static void
ButtonDragStart(Button * b)
{
   if (ButtonIsFixed(b))
      return;

   GrabPointerSet(EoGetWin(b), ECSR_GRAB, 0);
   Mode.mode = MODE_BUTTONDRAG;
   Mode_buttons.move_pending = 1;
   Mode_buttons.start_x = Mode.events.x;
   Mode_buttons.start_y = Mode.events.y;
}

static void
ButtonDragEnd(Button * b)
{
   Desk               *dsk;

   Mode.mode = MODE_NONE;

   if (!Mode_buttons.move_pending)
     {
	dsk = DesktopAt(Mode.events.x, Mode.events.y);
	ButtonMoveToDesktop(b, dsk);
	dsk = EoGetDesk(b);
	ButtonMoveRelative(b, -EoGetX(dsk), -EoGetY(dsk));
     }
   else
      Mode_buttons.move_pending = 0;

   autosave();
}

Button            **
ButtonsGetList(int *pnum)
{
   return (Button **) ecore_list_items_get(button_list, pnum);
}

void
ButtonsForeach(int id, Desk * dsk, void (*func) (Button * b))
{
   Button             *b;

   for (ecore_list_goto_first(button_list);
	(b = ecore_list_next(button_list)) != NULL;)
     {
	if (id >= 0 && id != b->id)
	   continue;
	if (dsk && dsk != EoGetDesk(b))
	   continue;
	func(b);
     }
}

void
ButtonsMoveStickyToDesk(Desk * dsk)
{
   Button             *b;

   ECORE_LIST_FOR_EACH(button_list, b)
   {
      if (!EoIsSticky(b) || ButtonIsInternal(b))
	 continue;

      ButtonMoveToDesktop(b, dsk);
   }
}

/*
 * Button event handlers
 */

static void
ButtonDoAction(Button * b, XEvent * ev)
{
   if (b->owner && b->func)
      b->func(b->owner, ev, b->aclass);
   else
      ActionclassEvent(b->aclass, ev, NULL);
}

static void
ButtonEventMouseDown(Button * b, XEvent * ev)
{
   Mode_buttons.button = b;

   GrabPointerSet(EoGetWin(b), ECSR_GRAB, 0);

   if (b->inside_win)
     {
	Window              win = ev->xbutton.window;

	ev->xbutton.window = b->inside_win;
	XSendEvent(disp, b->inside_win, False, ButtonPressMask, ev);
	ev->xbutton.window = win;
     }

   b->state = STATE_CLICKED;
   ButtonDraw(b);

   if (!ButtonIsInternal(b))
     {
	ActionClass        *ac;

	ac = ActionclassFind("ACTION_BUTTON_DRAG");
	if (ac && !Mode_buttons.action_inhibit)
	   ActionclassEvent(ac, ev, NULL);
     }

   if (b->aclass && !Mode_buttons.action_inhibit)
      ButtonDoAction(b, ev);
}

static void
ButtonEventMouseUp(Button * b, XEvent * ev)
{
   if (b->inside_win && !Mode_buttons.action_inhibit)
     {
	Window              win = ev->xbutton.window;

	ev->xbutton.window = b->inside_win;
	XSendEvent(disp, b->inside_win, False, ButtonReleaseMask, ev);
	ev->xbutton.window = win;
     }

   if ((b->state == STATE_CLICKED) && (!b->left))
      b->state = STATE_HILITED;
   else
      b->state = STATE_NORMAL;
   ButtonDraw(b);

   GrabPointerRelease();

   if (b->aclass && !b->left && !Mode_buttons.action_inhibit)
      ButtonDoAction(b, ev);

   b->left = 0;

   if (Mode.mode == MODE_BUTTONDRAG)
      ButtonDragEnd(Mode_buttons.button);
   Mode_buttons.button = NULL;

   Mode_buttons.action_inhibit = 0;
}

static void
ButtonEventMotion(Button * b, XEvent * ev __UNUSED__)
{
   int                 dx, dy;

   if (Mode.mode != MODE_BUTTONDRAG)
      return;

   dx = Mode.events.x - Mode.events.px;
   dy = Mode.events.y - Mode.events.py;

   if (Mode_buttons.move_pending)
     {
	int                 x, y;

	x = Mode.events.x - Mode_buttons.start_x;
	y = Mode.events.y - Mode_buttons.start_y;
	if (x < 0)
	   x = -x;
	if (y < 0)
	   y = -y;
	if ((x > Conf.button_move_resistance) ||
	    (y > Conf.button_move_resistance))
	   Mode_buttons.move_pending = 0;
	Mode_buttons.action_inhibit = 1;
     }
   if (!Mode_buttons.move_pending)
      ButtonMoveRelative(b, dx, dy);
}

static void
ButtonEventMouseIn(Button * b, XEvent * ev)
{
   if (b->state == STATE_CLICKED)
      b->left = 0;
   else
     {
	b->state = STATE_HILITED;
	ButtonDraw(b);
	if (b->aclass && !Mode_buttons.action_inhibit)
	   ActionclassEvent(b->aclass, ev, NULL);
     }
}

static void
ButtonEventMouseOut(Button * b, XEvent * ev)
{
   if (b->state == STATE_CLICKED)
      b->left = 1;
   else
     {
	b->state = STATE_NORMAL;
	ButtonDraw(b);
	if (b->aclass && !Mode_buttons.action_inhibit)
	   ActionclassEvent(b->aclass, ev, NULL);
     }
}

static ActionClass *
ButtonGetAclass(void *data)
{
   Button             *b = data;

   /* Validate button */
   if (!ecore_list_goto(button_list, b))
      return NULL;

   return b->aclass;
}

static void
ButtonHandleTooltip(Button * b, int event)
{
   switch (event)
     {
     case ButtonPress:
     case LeaveNotify:
	TooltipsSetPending(0, NULL, NULL);
	break;
     case ButtonRelease:
     case EnterNotify:
     case MotionNotify:
	TooltipsSetPending(0, ButtonGetAclass, b);
	break;
     }
}

static void
ButtonHandleEvents(XEvent * ev, void *prm)
{
   Button             *b = (Button *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	ButtonEventMouseDown(b, ev);
	break;
     case ButtonRelease:
	ButtonEventMouseUp(b, ev);
	break;
     case MotionNotify:
	ButtonEventMotion(b, ev);
	break;
     case EnterNotify:
	ButtonEventMouseIn(b, ev);
	break;
     case LeaveNotify:
	ButtonEventMouseOut(b, ev);
	break;
     }

   if (b->aclass)
      ButtonHandleTooltip(b, ev->type);

   if (b->func)
      b->func(b->owner, ev, NULL);
}

/*
 * Configuration load/save
 */
#include "conf.h"

int
ButtonsConfigLoad(FILE * ConfigFile)
{
   int                 err = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   char               *name = NULL;
   char               *label = NULL;
   ActionClass        *ac = NULL;
   ImageClass         *ic = NULL;
   TextClass          *tc = NULL;
   Button             *bt = NULL;
   Button             *pbt = NULL;
   char                ontop = 0;
   int                 flags = 0, minw = 1, maxw = 99999, minh = 1;
   int                 maxh = 99999, xo = 0, yo = 0, xa = 0;
   int                 xr = 0, ya = 0, yr = 0;
   int                 xsr = 0, xsa = 0, ysr = 0, ysa = 0;
   char                simg = 0;
   int                 desk = 0;
   char                sticky = 0;
   char                show = 1;
   char                internal = 0;
   int                 fields;
   const char         *ss;

   while (GetLine(s, sizeof(s), ConfigFile))
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
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	       }
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
		  i1 = CONFIG_INVALID;
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     if (!pbt && !Mode_buttons.loading_user)
	       {
		  bt = ButtonCreate(name, 0, ic, ac, tc, label, ontop, flags,
				    minw, maxw, minh, maxh, xo, yo, xa, xr,
				    ya, yr, xsr, xsa, ysr, ysa, simg, desk,
				    sticky);
		  bt->default_show = show;
		  bt->internal = internal;
	       }
	     goto done;
	  case CONFIG_TEXT:
	     tc = TextclassFind(s2, 1);
	     if (pbt)
		pbt->tclass = tc;
	     break;
	  case BUTTON_LABEL:
	     _EFREE(label);
	     ss = atword(s, 2);
	     label = Estrdup(ss);
	     if (pbt)
	       {
		  _EFREE(pbt->label);
		  pbt->label = label;
	       }
	     break;
	  case BORDERPART_ONTOP:
	     ontop = atoi(s2);
	     if (pbt)
		EoSetLayer(pbt, ontop);
	     break;
	  case CONFIG_CLASSNAME:
	  case BUTTON_NAME:
	     _EFREE(name);
	     name = Estrdup(s2);
	     pbt = ButtonFind(name);
	     break;
	  case CONFIG_ACTIONCLASS:
	  case BUTTON_ACLASS:
	     ac = ActionclassFind(s2);
	     if (pbt)
		pbt->aclass = ac;
	     break;
	  case CONFIG_IMAGECLASS:
	  case BUTTON_ICLASS:
	     ic = ImageclassFind(s2, 1);
	     if (pbt)
		pbt->iclass = ic;
	     break;
	  case BORDERPART_WMIN:
	     minw = atoi(s2);
	     if (pbt)
		pbt->geom.width.min = minw;
	     break;
	  case BORDERPART_WMAX:
	     maxw = atoi(s2);
	     if (pbt)
		pbt->geom.width.max = maxw;
	     break;
	  case BORDERPART_HMIN:
	     minh = atoi(s2);
	     if (pbt)
		pbt->geom.height.min = minh;
	     break;
	  case BORDERPART_FLAGS:
	     flags = atoi(s2);
	     if (pbt)
		pbt->flags = flags;
	     break;
	  case BORDERPART_HMAX:
	     maxh = atoi(s2);
	     if (pbt)
		pbt->geom.height.max = maxh;
	     break;
	  case BUTTON_XO:
	     xo = atoi(s2);
	     if (pbt)
		pbt->geom.xorigin = xo;
	     break;
	  case BUTTON_YO:
	     yo = atoi(s2);
	     if (pbt)
		pbt->geom.yorigin = yo;
	     break;
	  case BUTTON_XA:
	     xa = atoi(s2);
	     if (pbt)
		pbt->geom.xabs = xa;
	     break;
	  case BUTTON_XR:
	     xr = atoi(s2);
	     if (pbt)
		pbt->geom.xrel = xr;
	     break;
	  case BUTTON_YA:
	     ya = atoi(s2);
	     if (pbt)
		pbt->geom.yabs = ya;
	     break;
	  case BUTTON_YR:
	     yr = atoi(s2);
	     if (pbt)
		pbt->geom.yrel = yr;
	     break;
	  case BUTTON_XSR:
	     xsr = atoi(s2);
	     if (pbt)
		pbt->geom.xsizerel = xsr;
	     break;
	  case BUTTON_XSA:
	     xsa = atoi(s2);
	     if (pbt)
		pbt->geom.xsizeabs = xsa;
	     break;
	  case BUTTON_YSR:
	     ysr = atoi(s2);
	     if (pbt)
		pbt->geom.ysizerel = ysr;
	     break;
	  case BUTTON_YSA:
	     ysa = atoi(s2);
	     if (pbt)
		pbt->geom.ysizeabs = ysa;
	     break;
	  case BUTTON_SIMG:
	     simg = atoi(s2);
	     if (pbt)
		pbt->geom.size_from_image = simg;
	     break;
	  case BUTTON_DESK:
	     desk = atoi(s2);
	     if (pbt)
		ButtonMoveToDesktop(pbt, DeskGet(desk));
	     break;
	  case BUTTON_STICKY:
	     sticky = atoi(s2);
	     if (pbt)
		EoSetSticky(pbt, sticky);
	     break;
	  case BUTTON_INTERNAL:
	     internal = atoi(s2);
	     if (pbt)
		pbt->internal = internal;
	     break;
	  case BUTTON_SHOW:
	     show = atoi(s2);
	     if (pbt)
		pbt->default_show = show;
	     break;
	  default:
	     break;
	  }
     }
   err = -1;

 done:
   _EFREE(name);
   _EFREE(label);

   return err;
}

static void
ButtonsConfigLoadUser(void)
{
   char                s[4096];

   Esnprintf(s, sizeof(s), "%s.buttons", EGetSavePrefix());

   Mode_buttons.loading_user = 1;
   ConfigFileLoad(s, NULL, ConfigFileRead, 0);
   Mode_buttons.loading_user = 0;
}

static void
ButtonsConfigSave(void)
{
   char                s[FILEPATH_LEN_MAX], st[FILEPATH_LEN_MAX];
   FILE               *fs;
   int                 i, num;
   Button            **blst;
   int                 flags;

   blst = ButtonsGetList(&num);
   if (!blst)
      return;

   Etmp(st);
   fs = fopen(st, "w");
   if (!fs)
      return;

   for (i = 0; i < num; i++)
     {
	if (blst[i]->id != 0 || blst[i]->internal)
	   continue;

	fprintf(fs, "4 999\n");
	fprintf(fs, "100 %s\n", EoGetName(blst[i]));
	if (blst[i]->iclass)
	   fprintf(fs, "12 %s\n", ImageclassGetName(blst[i]->iclass));
	if (blst[i]->aclass)
	   fprintf(fs, "11 %s\n", ActionclassGetName(blst[i]->aclass));
	if (EoGetLayer(blst[i]) >= 0)
	   fprintf(fs, "453 %i\n", EoGetLayer(blst[i]));
	fprintf(fs, "456 %i\n", blst[i]->geom.width.min);
	fprintf(fs, "457 %i\n", blst[i]->geom.width.max);
	fprintf(fs, "468 %i\n", blst[i]->geom.height.min);
	fprintf(fs, "469 %i\n", blst[i]->geom.height.max);
	fprintf(fs, "528 %i\n", blst[i]->geom.xorigin);
	fprintf(fs, "529 %i\n", blst[i]->geom.yorigin);
	fprintf(fs, "530 %i\n", blst[i]->geom.xabs);
	fprintf(fs, "531 %i\n", blst[i]->geom.xrel);
	fprintf(fs, "532 %i\n", blst[i]->geom.yabs);
	fprintf(fs, "533 %i\n", blst[i]->geom.yrel);
	fprintf(fs, "534 %i\n", blst[i]->geom.xsizerel);
	fprintf(fs, "535 %i\n", blst[i]->geom.xsizeabs);
	fprintf(fs, "536 %i\n", blst[i]->geom.ysizerel);
	fprintf(fs, "537 %i\n", blst[i]->geom.ysizeabs);
	fprintf(fs, "538 %i\n", blst[i]->geom.size_from_image);
	fprintf(fs, "539 %i\n", EoGetDeskNum(blst[i]));
	fprintf(fs, "540 %i\n", EoIsSticky(blst[i]));
	fprintf(fs, "542 %i\n", EoIsShown(blst[i]));

	if (blst[i]->flags)
	  {
	     flags = 0;
	     if (((blst[i]->flags & FLAG_FIXED_HORIZ)
		  && (blst[i]->flags & FLAG_FIXED_VERT))
		 || (blst[i]->flags & FLAG_FIXED))
		flags = 2;
	     else if (blst[i]->flags & FLAG_FIXED_HORIZ)
		flags = 3;
	     else if (blst[i]->flags & FLAG_FIXED_VERT)
		flags = 4;
	     else if (blst[i]->flags & FLAG_TITLE)
		flags = 0;
	     else if (blst[i]->flags & FLAG_MINIICON)
		flags = 1;
	     fprintf(fs, "454 %i\n", flags);
	  }
	fprintf(fs, "1000\n");
     }

   fclose(fs);

   Esnprintf(s, sizeof(s), "%s.buttons", EGetSavePrefix());
   E_mv(st, s);

   Efree(blst);
}

/*
 * Buttons Module
 */

static void
ButtonsSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_INIT:
	memset(&Mode_buttons, 0, sizeof(Mode_buttons));
	break;

     case ESIGNAL_CONFIGURE:
	ButtonsConfigLoadUser();
	break;

     case ESIGNAL_EXIT:
	if (Mode.wm.save_ok)
	   ButtonsConfigSave();
	break;
     }
}

typedef struct
{
   int                 id;
   int                 match;
   const char         *regex;
} button_match_data;

static void
_ButtonHideShow(void *data, void *prm)
{
   Button             *b = data;
   button_match_data  *bmd = prm;
   int                 match;

   if (bmd->id >= 0 && bmd->id != b->id)
      return;

   if (bmd->regex)
     {
	match = matchregexp(bmd->regex, EoGetName(b));
	if ((match && !bmd->match) || (!match && bmd->match))
	   return;
	if (!strcmp(EoGetName(b), "_DESKTOP_DESKRAY_DRAG_CONTROL"))	/* FIXME - ??? */
	   return;
     }

   ButtonToggle(b);
}

static void
doHideShowButton(const char *params)
{
   Button             *b;
   char                s[1024];
   const char         *ss;
   button_match_data   bmd = { -1, 1, NULL };

   if (!params)
     {
	bmd.id = 0;
	ecore_list_for_each(button_list, _ButtonHideShow, &bmd);
	goto done;
     }

   sscanf(params, "%1000s", s);
   if (!strcmp(s, "button"))
     {
	sscanf(params, "%*s %1000s", s);
	b = ButtonFind(s);
	if (b)
	   ButtonToggle(b);
     }
   else if (!strcmp(s, "buttons"))
     {
	ss = atword(params, 2);
	if (!ss)
	   return;

	bmd.regex = ss;
	ecore_list_for_each(button_list, _ButtonHideShow, &bmd);
     }
   else if (!strcmp(s, "all_buttons_except"))
     {
	ss = atword(params, 2);
	if (!ss)
	   return;

	bmd.id = 0;
	bmd.match = 0;
	bmd.regex = ss;
	ecore_list_for_each(button_list, _ButtonHideShow, &bmd);
     }
   else if (!strcmp(s, "all"))
     {
	ecore_list_for_each(button_list, _ButtonHideShow, &bmd);
     }

 done:
   autosave();
}

static void
ButtonsIpc(const char *params, Client * c __UNUSED__)
{
   const char         *p;
   char                cmd[128], prm[4096];
   int                 len;
   Button             *b;

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
     }
   else if (!strncmp(cmd, "list", 2))
     {
	IpcPrintf("Win       d  s  l     x     y     w     h name\n");
	ECORE_LIST_FOR_EACH(button_list, b)
	   IpcPrintf("%#lx %2d %2d %2d %5d+%5d %5dx%5d %s\n",
		     EoGetWin(b), EoGetDeskNum(b), EoIsSticky(b), EoGetLayer(b),
		     EoGetX(b), EoGetY(b), EoGetW(b), EoGetH(b), EoGetName(b));
     }
   else if (!strncmp(cmd, "move", 2))
     {
	if (Mode_buttons.button)
	   ButtonDragStart(Mode_buttons.button);
     }
}

static void
IPC_ButtonShow(const char *params, Client * c __UNUSED__)
{
   doHideShowButton(params);
}

static const IpcItem ButtonsIpcArray[] = {
   {
    ButtonsIpc,
    "button", "btn",
    "Button functions",
    "  button list               List buttons\n"},
   {
    IPC_ButtonShow,
    "button_show", NULL,
    "Show or Hide buttons on desktop",
    "use \"button_show <button/buttons/all_buttons_except/all> "
    "<BUTTON_STRING>\"\nexamples: \"button_show buttons all\" "
    "(removes all buttons and the dragbar)\n\"button_show\" "
    "(removes all buttons)\n \"button_show buttons CONFIG*\" "
    "(removes all buttons with CONFIG in the start)\n"},
};
#define N_IPC_FUNCS (sizeof(ButtonsIpcArray)/sizeof(IpcItem))

#if 0
static const CfgItem ButtonsCfgItems[] = {
   CFG_ITEM_BOOL(Conf.buttons, enable, 1),
};
#define N_CFG_ITEMS (sizeof(ButtonsCfgItems)/sizeof(CfgItem))
#endif

/*
 * Module descriptor
 */
EModule             ModButtons = {
   "buttons", "btn",
   ButtonsSighan,
   {N_IPC_FUNCS, ButtonsIpcArray},
   {0, NULL}
};
