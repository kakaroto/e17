#include "E.h"

#define WIN_LAYER_DESKTOP                0
#define WIN_LAYER_BELOW                  2
#define WIN_LAYER_NORMAL                 4
#define WIN_LAYER_ONTOP                  6
#define WIN_LAYER_DOCK                   8
#define WIN_LAYER_ABOVE_DOCK             10
#define WIN_LAYER_MENU                   12

#define WIN_STATE_STICKY          (1<<0)	/* everyone knows sticky */
#define WIN_STATE_RESERVED_BIT1   (1<<1)	/* removed minimize here */
#define WIN_STATE_MAXIMIZED_VERT  (1<<2)	/* window in maximized V state */
#define WIN_STATE_MAXIMIZED_HORIZ (1<<3)	/* window in maximized H state */
#define WIN_STATE_HIDDEN          (1<<4)	/* not on taskbar but window visible */
#define WIN_STATE_SHADED          (1<<5)	/* shaded (NeXT style) */
#define WIN_STATE_HID_WORKSPACE   (1<<6)	/* not on current desktop */
#define WIN_STATE_HID_TRANSIENT   (1<<7)	/* owner of transient is hidden */
#define WIN_STATE_FIXED_POSITION  (1<<8)	/* window is fixed in position even */
#define WIN_STATE_ARRANGE_IGNORE  (1<<9)	/* ignore for auto arranging */

static ToolTip     *ttip = NULL;
struct _mdata
  {
     Menu               *m;
     MenuItem           *mi;
     EWin               *ewin;
  };

void                ToolTipTimeout(int val, void *data);
void                SubmenuShowTimeout(int val, void *dat);

static char         sentpress = 0;
static Window       click_was_in = 0;
static Time         last_time = 0;
static int          last_button = 0;
static int          pgd_x = 0, pgd_y = 0;

void
ToolTipTimeout(int val, void *data)
{
   int                 x, y, dum;
   unsigned int        mask;
   Window              win, rt, ch;
   ActionClass        *ac;

   EDBUG(5, "ToolTipTimeout");

   /* In the case of multiple screens, check to make sure
    * the root window is still where the mouse is... */
   if (False == XQueryPointer(disp, root.win, &rt, &ch, &x, &y, &dum,
			      &dum, &mask))
      EDBUG_RETURN_;
   /* dont pop up tooltip is mouse button down */
   if (mask & (Button1Mask | Button2Mask | Button3Mask |
	       Button4Mask | Button5Mask))
      EDBUG_RETURN_;
   win = WindowAtXY(x, y);
   ac = FindActionClass(win);
   if (!ac)
      EDBUG_RETURN_;

   if (!ttip)
      ttip = FindItem("DEFAULT", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_TOOLTIP);

   if (ac->tooltipstring)
      ShowToolTip(ttip, ac->tooltipstring, ac, x, y);

   data = NULL;
   val = 0;

   EDBUG_RETURN_;
}

void
HandleClientMessage(XEvent * ev)
{
   EWin               *ewin;

   static Atom         a1 = 0, a2 = 0, a3 = 0, a4 = 0, a5 = 0, a6 = 0;

   EDBUG(5, "HandleClientMessage");
   if (!a1)
      a1 = XInternAtom(disp, "ENL_MSG", False);
   if (!a2)
      a2 = XInternAtom(disp, "_WIN_AREA", False);
   if (!a3)
      a3 = XInternAtom(disp, "_WIN_WORKSPACE", False);
   if (!a4)
      a4 = XInternAtom(disp, "_WIN_LAYER", False);
   if (!a5)
      a5 = XInternAtom(disp, "_WIN_STATE", False);
   if (!a6)
      a6 = XInternAtom(disp, "WM_CHANGE_STATE", False);
   if (ev->xclient.message_type == a1)
     {
	HandleComms(ev);
	EDBUG_RETURN_;
     }
   if (ev->xclient.message_type == a2)
     {
	SetCurrentArea(ev->xclient.data.l[0], ev->xclient.data.l[1]);
	EDBUG_RETURN_;
     }
   if (ev->xclient.message_type == a3)
     {
	GotoDesktop(ev->xclient.data.l[0]);
	EDBUG_RETURN_;
     }
   if (ev->xclient.message_type == a4)
     {
	ewin = FindItem(NULL, ev->xclient.window, LIST_FINDBY_ID, LIST_TYPE_EWIN);
	if (ewin)
	  {
	     ewin->layer = ev->xclient.data.l[0];
	     XChangeProperty(disp, ewin->win, a4, XA_CARDINAL, 32,
			     PropModeReplace,
			     (unsigned char *)(&(ev->xclient.data.l[0])), 1);
	     RaiseEwin(ewin);
	  }
	EDBUG_RETURN_;
     }
   if (ev->xclient.message_type == a5)
     {
	ewin = FindItem(NULL, ev->xclient.window, LIST_FINDBY_ID, LIST_TYPE_EWIN);
	if (!ewin)
	   EDBUG_RETURN_;
	if (ev->xclient.data.l[0] & WIN_STATE_FIXED_POSITION)
	  {
	     if (ev->xclient.data.l[1] & WIN_STATE_FIXED_POSITION)
		ewin->fixedpos = 1;
	     else
		ewin->fixedpos = 0;
	  }
	if (ev->xclient.data.l[0] & WIN_STATE_ARRANGE_IGNORE)
	  {
	     if (ev->xclient.data.l[1] & WIN_STATE_ARRANGE_IGNORE)
		ewin->ignorearrange = 1;
	     else
		ewin->ignorearrange = 0;
	  }
	if ((ev->xclient.data.l[0] & WIN_STATE_STICKY) && (!ewin->ignorearrange))
	  {
	     if (ev->xclient.data.l[1] & WIN_STATE_STICKY)
	       {
		  if (!(ewin->sticky))
		    {
		       ewin->sticky = 1;
		       RaiseEwin(ewin);
		       DrawEwin(ewin);
		       ApplySclass(FindItem("SOUND_WINDOW_STICK", 0,
					 LIST_FINDBY_NAME, LIST_TYPE_SCLASS));
		    }
	       }
	     else
	       {
		  if (ewin->sticky)
		    {
		       ewin->sticky = 0;
		       RaiseEwin(ewin);
		       DrawEwin(ewin);
		       ApplySclass(FindItem("SOUND_WINDOW_UNSTICK", 0,
					 LIST_FINDBY_NAME, LIST_TYPE_SCLASS));
		    }
	       }
	  }
	if (ev->xclient.data.l[0] & WIN_STATE_SHADED)
	  {
	     if (ev->xclient.data.l[1] & WIN_STATE_SHADED)
		ShadeEwin(ewin);
	     else
		UnShadeEwin(ewin);
	  }
	GNOME_SetHint(ewin);
	EDBUG_RETURN_;
     }
   if (ev->xclient.message_type == a6)
     {
	ewin = FindItem(NULL, ev->xclient.window, LIST_FINDBY_ID, LIST_TYPE_EWIN);
	if ((ewin) && (ev->xclient.data.l[0] == IconicState) &&
	    (!(ewin->iconified)))
	  {
	     IconifyEwin(ewin);
	     EDBUG_RETURN_;
	  }
     }

   if (mode.kde_support)
      KDE_ProcessClientMessage(&(ev->xclient));

   EDBUG_RETURN_;

}

void
HandleFocusWindowIn(Window win)
{
   EWin               *ewin;

   EDBUG(5, "HandleFocusWindowIn");
   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin != mode.focuswin)
     {
	if (mode.focuswin)
	  {
	     mode.focuswin->active = 0;
	     DrawEwin(mode.focuswin);
	     if (mode.focusmode == FOCUS_CLICK)
		XGrabButton(disp, AnyButton, AnyModifier,
			    mode.focuswin->win_container, False,
			    ButtonPressMask,
			    GrabModeSync, GrabModeAsync, None, None);
	  }
	mode.focuswin = ewin;
	if ((ewin) && (!ewin->menu))
	  {
	     mode.realfocuswin = ewin;
	     if (!mode.cur_menu_mode)
		mode.context_ewin = ewin;
	  }
	if (mode.focuswin)
	  {
	     mode.focuswin->active = 1;
	     DrawEwin(mode.focuswin);
	     if (mode.focusmode == FOCUS_CLICK)
	       {
		  XUngrabButton(disp, AnyButton, AnyModifier, mode.focuswin->win_container);
		  GrabButtonGrabs(mode.focuswin);
	       }
	  }
     }
   EDBUG_RETURN_;
}

void
HandleFocusWindow(Window win)
{
   EWin               *found_ewin;

   EDBUG(5, "HandleFocusWindow");
   if (root.focuswin == win)
      FocusToEWin(NULL);
   else
     {
	found_ewin = FindEwinByChildren(win);
	if (!found_ewin)
	   found_ewin = FindEwinByBase(win);
	if (mode.focusmode == FOCUS_CLICK)
	   mode.mouse_over_win = found_ewin;
	else if (mode.focusmode == FOCUS_SLOPPY)
	  {
	     if (!found_ewin)
		ICCCM_Cmap(NULL);
	     if (found_ewin)
		FocusToEWin(found_ewin);
	     mode.mouse_over_win = found_ewin;
	  }
	else if (mode.focusmode == FOCUS_POINTER)
	  {
	     if (!found_ewin)
		found_ewin = GetEwinPointerInClient();
	     FocusToEWin(found_ewin);
	     mode.mouse_over_win = found_ewin;
	  }
     }
   EDBUG_RETURN_;
}

void
HandleChildShapeChange(XEvent * ev)
{
   Window              win;
   EWin               *ewin;
   Border             *b;

   EDBUG(5, "HandleChildShapeChange");
   win = ((XShapeEvent *) ev)->window;
   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
     {
	b = ewin->border;
	SyncBorderToEwin(ewin);
	if (ewin->border == b)
	   PropagateShapes(ewin->win);
     }
   EDBUG_RETURN_;
}

void
HandleMotion(XEvent * ev)
{
   int                 dx, dy;
   int                 pw, ph;
   int                 x, y, w, h;
   EWin               *ewin;

   EDBUG(5, "HandleMotion");

   if (ttip)
      HideToolTip(ttip);
   RemoveTimerEvent("TOOLTIP_TIMEOUT");
   if (mode.tooltips)
      DoIn("TOOLTIP_TIMEOUT", mode.tiptime, ToolTipTimeout, 0, NULL);

   EdgeHandleMotion(ev);
   mode.px = mode.x;
   mode.py = mode.y;
   mode.x = ev->xmotion.x_root;
   mode.y = ev->xmotion.y_root;
   ewin = NULL;
   desks.current = DesktopAt(mode.x, mode.y);

   if ((!(ev->xmotion.state & (Button1Mask | Button2Mask |
			       Button3Mask | Button4Mask |
			       Button5Mask)) &&
	(!mode.place)))
     {
	switch (mode.mode)
	  {
	  case MODE_RESIZE:
	  case MODE_RESIZE_H:
	  case MODE_RESIZE_V:
	     doResizeEnd(NULL);
	     EDBUG_RETURN_;
	     break;
	  case MODE_MOVE:
	     doMoveEnd(NULL);
	     EDBUG_RETURN_;
	     break;
	  case MODE_BUTTONDRAG:
	     doDragButtonEnd(NULL);
	     EDBUG_RETURN_;
	     break;
	  default:
	     break;
	  }
     }
   switch (mode.mode)
     {
     case MODE_MOVE:
	if (mode.ewin)
	  {
	     EWin              **gwins;
	     int                 i, num;
	     int                 ndx, ndy;
	     int                 prx, pry;
	     int                 screen_snap_dist;

	     ewin = mode.ewin;
	     gwins = ListWinGroupMembersForEwin(ewin, ACTION_MOVE, mode.nogroup, &num);

	     if ((mode.moveresize_pending_ewin) &&
		 (mode.ewin == mode.moveresize_pending_ewin))
	       {
		  for (i = 0; i < num; i++)
		     DrawEwinShape(gwins[i], mode.movemode, gwins[i]->x, gwins[i]->y,
				   gwins[i]->client.w, gwins[i]->client.h, 0);
		  mode.moveresize_pending_ewin = NULL;
	       }
	     dx = mode.x - mode.px;
	     dy = mode.y - mode.py;
	     if (mode.next_move_x_plus != 0)
	       {
		  dx += mode.next_move_x_plus;
		  mode.next_move_x_plus = 0;
	       }
	     if (mode.next_move_y_plus != 0)
	       {
		  dy += mode.next_move_y_plus;
		  mode.next_move_y_plus = 0;
	       }
	     {
		char                jumpx, jumpy;
		int                 min_dx, max_dx, min_dy, max_dy;

		jumpx = 0;
		jumpy = 0;
		min_dx = dx;
		min_dy = dy;
		max_dx = dx;
		max_dy = dy;
		for (i = 0; i < num; i++)
		  {
		     ndx = dx;
		     ndy = dy;
		     /* make our ewin resist other ewins around the place */
		     SnapEwin(gwins[i], dx, dy, &ndx, &ndy);
		     if ((dx < 0) && (ndx <= 0))
		       {
			  if (ndx > min_dx)
			     min_dx = ndx;
			  if (ndx < max_dx)
			     max_dx = ndx;
		       }
		     else if (ndx >= 0)
		       {
			  if (ndx < min_dx)
			     min_dx = ndx;
			  if (ndx > max_dx)
			     max_dx = ndx;
		       }
		     if ((dy < 0) && (ndy <= 0))
		       {
			  if (ndy > min_dy)
			     min_dy = ndy;
			  if (ndy < max_dy)
			     max_dy = ndy;
		       }
		     else if (ndy >= 0)
		       {
			  if (ndy < min_dy)
			     min_dy = ndy;
			  if (ndy > max_dy)
			     max_dy = ndy;
		       }
		  }
		if (min_dx == dx)
		   ndx = max_dx;
		else
		   ndx = min_dx;
		if (min_dy == dy)
		   ndy = max_dy;
		else
		   ndy = min_dy;
		screen_snap_dist = mode.constrained ? (root.w + root.h)
		   : mode.screen_snap_dist;
		for (i = 0; i < num; i++)
		  {
		     /* jump out of snap horizontally */
		     if ((ndx != dx) &&
			 (((gwins[i]->x == 0) &&
			   (!(IN_RANGE(gwins[i]->reqx, gwins[i]->x, screen_snap_dist)))) ||
			  ((gwins[i]->x == (root.w - gwins[i]->w)) &&
			   (!(IN_RANGE(gwins[i]->reqx, gwins[i]->x, screen_snap_dist)))) ||
			  ((gwins[i]->x != 0) &&
			   (gwins[i]->x != (root.w - gwins[i]->w) &&
			    (!(IN_RANGE(gwins[i]->reqx, gwins[i]->x, mode.edge_snap_dist)))))))
		       {
			  jumpx = 1;
			  ndx = gwins[i]->reqx - gwins[i]->x + dx;
		       }
		     /* jump out of snap vertically */
		     if ((ndy != dy) &&
			 (((gwins[i]->y == 0) &&
			   (!(IN_RANGE(gwins[i]->reqy, gwins[i]->y, screen_snap_dist)))) ||
			  ((gwins[i]->y == (root.h - gwins[i]->h)) &&
			   (!(IN_RANGE(gwins[i]->reqy, gwins[i]->y, screen_snap_dist)))) ||
			  ((gwins[i]->y != 0) &&
			   (gwins[i]->y != (root.h - gwins[i]->h) &&
			    (!(IN_RANGE(gwins[i]->reqy, gwins[i]->y, mode.edge_snap_dist)))))))
		       {
			  jumpy = 1;
			  ndy = gwins[i]->reqy - gwins[i]->y + dy;
		       }
		  }
		for (i = 0; i < num; i++)
		  {
		     /* if its opaque move mode check to see if we have to float */
		     /* the window aboe all desktops (reparent to root) */
		     if (mode.movemode == 0)
			DetermineEwinFloat(gwins[i], ndx, ndy);
		     /* draw the new position of the window */
		     prx = gwins[i]->reqx;
		     pry = gwins[i]->reqy;
		     DrawEwinShape(gwins[i], mode.movemode,
				   gwins[i]->x + ndx, gwins[i]->y + ndy,
				   gwins[i]->client.w, gwins[i]->client.h,
				   mode.firstlast);
		     /* if we didnt jump the winow after a reist at the edge */
		     /* reset the requested x to be the prev requested + delta */
		     if (!(jumpx))
			gwins[i]->reqx = prx + dx;
		     if (!(jumpy))
			gwins[i]->reqy = pry + dy;
		  }
	     }
	     Efree(gwins);
	  }
	break;
     case MODE_RESIZE:
	if (mode.ewin)
	  {
	     ewin = mode.ewin;
	     switch (mode.resize_detail)
	       {
	       case 0:
		  pw = ewin->client.w;
		  ph = ewin->client.h;
		  w = mode.win_w - (mode.x - mode.start_x);
		  h = mode.win_h - (mode.y - mode.start_y);
		  x = mode.win_x + (mode.x - mode.start_x);
		  y = mode.win_y + (mode.y - mode.start_y);
		  ewin->client.w = w;
		  ewin->client.h = h;
		  ICCCM_MatchSize(ewin);
		  w = ewin->client.w;
		  h = ewin->client.h;
		  if (pw == ewin->client.w)
		     x = ewin->x;
		  else
		     x = mode.win_x + mode.win_w - w;
		  if (ph == ewin->client.h)
		     y = ewin->y;
		  else
		     y = mode.win_y + mode.win_h - h;
		  ewin->client.w = pw;
		  ewin->client.h = ph;
		  DrawEwinShape(ewin, mode.resizemode, x, y, w, h, mode.firstlast);
		  break;
	       case 1:
		  ph = ewin->client.h;
		  w = mode.win_w + (mode.x - mode.start_x);
		  h = mode.win_h - (mode.y - mode.start_y);
		  x = ewin->x;
		  y = mode.win_y + (mode.y - mode.start_y);
		  ewin->client.h = h;
		  ICCCM_MatchSize(ewin);
		  h = ewin->client.h;
		  if (ph == ewin->client.h)
		     y = ewin->y;
		  else
		     y = mode.win_y + mode.win_h - h;
		  ewin->client.h = ph;
		  DrawEwinShape(ewin, mode.resizemode, x, y, w, h, mode.firstlast);
		  break;
	       case 2:
		  pw = ewin->client.w;
		  w = mode.win_w - (mode.x - mode.start_x);
		  h = mode.win_h + (mode.y - mode.start_y);
		  x = mode.win_x + (mode.x - mode.start_x);
		  y = ewin->y;
		  ewin->client.w = w;
		  ICCCM_MatchSize(ewin);
		  w = ewin->client.w;
		  if (pw == ewin->client.w)
		     x = ewin->x;
		  else
		     x = mode.win_x + mode.win_w - w;
		  ewin->client.w = pw;
		  DrawEwinShape(ewin, mode.resizemode, x, y, w, h, mode.firstlast);
		  break;
	       case 3:
		  w = mode.win_w + (mode.x - mode.start_x);
		  h = mode.win_h + (mode.y - mode.start_y);
		  x = ewin->x;
		  y = ewin->y;
		  DrawEwinShape(ewin, mode.resizemode, x, y, w, h, mode.firstlast);
		  break;
	       default:
		  break;
	       }
	  }
	break;
     case MODE_RESIZE_H:
	if (mode.ewin)
	  {
	     ewin = mode.ewin;
	     switch (mode.resize_detail)
	       {
	       case 0:
		  pw = ewin->client.w;
		  w = mode.win_w - (mode.x - mode.start_x);
		  h = ewin->client.h;
		  x = mode.win_x + (mode.x - mode.start_x);
		  y = ewin->y;
		  ewin->client.w = w;
		  ICCCM_MatchSize(ewin);
		  w = ewin->client.w;
		  if (pw == ewin->client.w)
		     x = ewin->x;
		  else
		     x = mode.win_x + mode.win_w - w;
		  ewin->client.w = pw;
		  DrawEwinShape(ewin, mode.resizemode, x, y, w, h, mode.firstlast);
		  break;
	       case 1:
		  w = mode.win_w + (mode.x - mode.start_x);
		  h = ewin->client.h;
		  x = ewin->x;
		  y = ewin->y;
		  DrawEwinShape(ewin, mode.resizemode, x, y, w, h, mode.firstlast);
		  break;
	       default:
		  break;
	       }
	  }
	break;
     case MODE_RESIZE_V:
	if (mode.ewin)
	  {
	     ewin = mode.ewin;
	     switch (mode.resize_detail)
	       {
	       case 0:
		  ph = ewin->client.h;
		  w = ewin->client.w;
		  h = mode.win_h - (mode.y - mode.start_y);
		  x = ewin->x;
		  y = mode.win_y + (mode.y - mode.start_y);
		  ewin->client.h = h;
		  ICCCM_MatchSize(ewin);
		  h = ewin->client.h;
		  if (ph == ewin->client.h)
		     y = ewin->y;
		  else
		     y = mode.win_y + mode.win_h - h;
		  ewin->client.h = ph;
		  DrawEwinShape(ewin, mode.resizemode, x, y, w, h, mode.firstlast);
		  break;
	       case 1:
		  w = ewin->client.w;
		  h = mode.win_h + (mode.y - mode.start_y);
		  x = ewin->x;
		  y = ewin->y;
		  DrawEwinShape(ewin, mode.resizemode, x, y, w, h, mode.firstlast);
		  break;
	       default:
		  break;
	       }
	  }
	break;
     case MODE_DESKDRAG:
	dx = mode.x - mode.px;
	dy = mode.y - mode.py;
	switch (desks.dragdir)
	  {
	  case 0:
	     if ((desks.desk[mode.deskdrag].x + dx) < 0)
		dx = -desks.desk[mode.deskdrag].x;
	     MoveDesktop(mode.deskdrag, desks.desk[mode.deskdrag].x + dx,
			 desks.desk[mode.deskdrag].y);
	     break;
	  case 1:
	     if ((desks.desk[mode.deskdrag].x + dx) > 0)
		MoveDesktop(mode.deskdrag, 0, desks.desk[mode.deskdrag].y);
	     else
		MoveDesktop(mode.deskdrag, desks.desk[mode.deskdrag].x + dx,
			    desks.desk[mode.deskdrag].y);
	     break;
	  case 2:
	     if ((desks.desk[mode.deskdrag].y + dy) < 0)
		dy = -desks.desk[mode.deskdrag].y;
	     MoveDesktop(mode.deskdrag, desks.desk[mode.deskdrag].x,
			 desks.desk[mode.deskdrag].y + dy);
	     break;
	  case 3:
	     if ((desks.desk[mode.deskdrag].y + dy) > 0)
		MoveDesktop(mode.deskdrag, desks.desk[mode.deskdrag].x, 0);
	     else
		MoveDesktop(mode.deskdrag, desks.desk[mode.deskdrag].x,
			    desks.desk[mode.deskdrag].y + dy);
	     break;
	  default:
	     break;
	  }
	break;
     case MODE_BUTTONDRAG:
	dx = mode.x - mode.px;
	dy = mode.y - mode.py;
	if (mode.button_move_pending)
	  {
	     x = mode.x - mode.start_x;
	     y = mode.y - mode.start_y;
	     if (x < 0)
		x = -x;
	     if (y < 0)
		y = -y;
	     if ((x > mode.button_move_resistance) ||
		 (y > mode.button_move_resistance))
		mode.button_move_pending = 0;
	  }
	if (!mode.button_move_pending)
	  {
	     if (mode.button)
	       {
		  MovebuttonToCoord(mode.button,
				    mode.button->x + dx,
				    mode.button->y + dy);
		  if (mode.deskmode == MODE_DESKRAY)
		    {
		       MoveDesktop(mode.deskdrag, desks.desk[mode.deskdrag].x,
				   desks.desk[mode.deskdrag].y + dy);
		    }
	       }
	  }
	break;
     default:
	break;
     }
#define SCROLL_RATIO 2/3
   if (((mode.cur_menu_mode) || (clickmenu)) && (mode.cur_menu_depth > 0))
     {
	int                 i, offx = 0, offy = 0, xdist = 0, ydist = 0;
	EWin               *ewin;

	EWin               *menus[256];
	int                 fx[256];
	int                 fy[256];
	int                 tx[256];
	int                 ty[256];
	static int          menu_scroll_dist = 13;

	if (mode.x > (root.w - (menu_scroll_dist + 1)))
	   xdist = -(menu_scroll_dist + (mode.x - root.w));
	else if (mode.x < menu_scroll_dist)
	   xdist = menu_scroll_dist - (mode.x);

	if (mode.y > (root.h - (menu_scroll_dist + 1)))
	   ydist = -(menu_scroll_dist + (mode.y - root.h));
	else if (mode.y < menu_scroll_dist)
	   ydist = menu_scroll_dist - (mode.y);

	/* That's a hack to avoid unwanted events:
	 * If the user entered the border area, he has to
	 * leave it first, before he can scroll menus again ...
	 */
	if ((xdist != 0) || (ydist != 0) || mode.doingslide)
	   menu_scroll_dist = -10;	/* -10 has no meaning, only makes sure that the if's */
	else			/* above can't be fulfilled ... */
	   menu_scroll_dist = 13;

	if (mode.cur_menu_depth > 0)
	  {
	     int                 x1, y1, x2, y2;

	     x1 = root.w;
	     x2 = -1;
	     y1 = root.h;
	     y2 = -1;
	     /* work out the minimum and maximum extents of our */
	     /* currently active menus */
	     for (i = 0; i < mode.cur_menu_depth; i++)
	       {
		  if (mode.cur_menu[i])
		    {
		       ewin = FindEwinByMenu(mode.cur_menu[i]);
		       if (ewin)
			 {
			    if (ewin->x < x1)
			       x1 = ewin->x;
			    if (ewin->y < y1)
			       y1 = ewin->y;
			    if ((ewin->x + ewin->w - 1) > x2)
			       x2 = ewin->x + ewin->w - 1;
			    if ((ewin->y + ewin->h - 1) > y2)
			       y2 = ewin->y + ewin->h - 1;
			 }
		    }
	       }

	     if (xdist < 0)
		offx = root.w - x2;
	     else if (xdist > 0)
		offx = -x1;
	     if (ydist < 0)
		offy = root.h - y2;
	     else if (ydist > 0)
		offy = -y1;

	     if ((xdist < 0) && (offx <= 0))
		xdist = offx;
	     if ((xdist > 0) && (offx >= 0))
		xdist = offx;
	     if ((ydist < 0) && (offy <= 0))
		ydist = offy;
	     if ((ydist > 0) && (offy >= 0))
		ydist = offy;

	     /* only if any active menus are partially off screen then scroll */
	     if ((((xdist > 0) && (x1 < 0)) || ((xdist < 0) && (x2 >= root.w))) ||
	       (((ydist > 0) && (y1 < 0)) || ((ydist < 0) && (y2 >= root.h))))
	       {
		  /* If we would scroll too far, limit scrolling to 2/3s of screen */
		  if (ydist < -root.h)
		     ydist = -root.h * SCROLL_RATIO;
		  if (ydist > root.h)
		     ydist = root.h * SCROLL_RATIO;

		  if (xdist < -root.w)
		     xdist = -root.w * SCROLL_RATIO;
		  if (xdist > root.w)
		     xdist = root.w * SCROLL_RATIO;

		  for (i = 0; i < mode.cur_menu_depth; i++)
		    {
		       menus[i] = NULL;
		       if (mode.cur_menu[i])
			 {
			    ewin = FindEwinByMenu(mode.cur_menu[i]);
			    if (ewin)
			      {
				 menus[i] = ewin;
				 fx[i] = ewin->x;
				 fy[i] = ewin->y;
				 tx[i] = ewin->x + xdist;
				 ty[i] = ewin->y + ydist;
			      }
			 }
		    }
		  SlideEwinsTo(menus, fx, fy, tx, ty, mode.cur_menu_depth, mode.shadespeed);
	       }
	  }
	if ((xdist != 0) || (ydist != 0))
	   XWarpPointer(disp, None, None, 0, 0, 0, 0, xdist, ydist);
     }
   if (mode.mode == MODE_NONE)
     {
	Pager              *p;

	p = FindPager(ev->xmotion.window);
	if (p)
	   PagerHandleMotion(p, ev->xmotion.window, ev->xmotion.x, ev->xmotion.y);
	else
	   PagerHandleMotion(NULL, ev->xmotion.window, -99, -99);
     }
   if ((mode.mode == MODE_PAGER_DRAG_PENDING) || (mode.mode == MODE_PAGER_DRAG))
     {
	Pager              *p;

	mode.mode = MODE_PAGER_DRAG;
	p = mode.context_pager;
	if (p)
	  {
	     int                 ax, ay, cx, cy;

	     cx = desks.desk[p->desktop].current_area_x;
	     cy = desks.desk[p->desktop].current_area_y;
	     GetAreaSize(&ax, &ay);
	     dx = mode.x - mode.px;
	     dy = mode.y - mode.py;
	     if (!FindItem((char *)p->hi_ewin, 0,
			   LIST_FINDBY_POINTER, LIST_TYPE_EWIN))
		p->hi_ewin = NULL;
	     if ((p->hi_ewin) && (!p->hi_ewin->pager) &&
		 (!p->hi_ewin->fixedpos))
	       {
		  GetWinXY(p->hi_win, &x, &y);
		  XRaiseWindow(disp, p->hi_win);
		  EMoveWindow(disp, p->hi_win, x + dx, y + dy);
	       }
	     if ((p->hi_ewin) && (!p->hi_ewin->pager) &&
		 (!p->hi_ewin->fixedpos))
	       {
		  Window              dw;
		  int                 px, py;

		  XTranslateCoordinates(disp, p->win, root.win, 0, 0, &px, &py, &dw);
		  MoveEwin(p->hi_ewin,
			   ((x + dx - px) - (cx * (p->w / ax))) *
			   (root.w / (p->w / ax)),
			   ((y + dy - py) - (cy * (p->h / ay))) *
			   (root.h / (p->h / ay)));
	       }
	  }
     }
   {
      Dialog             *d;
      DItem              *di;
      int                 dx, dy;

      di = FindDialogItem(ev->xmotion.window, &d);
      if (di)
	{
	   if (di->type == DITEM_AREA)
	     {
		if (di->item.area.event_func)
		   (di->item.area.event_func) (0, ev);
	     }
	   else if ((di->type == DITEM_SLIDER) && (di->item.slider.in_drag))
	     {
		if (ev->xmotion.window == di->item.slider.knob_win)
		  {
		     dx = mode.x - mode.px;
		     dy = mode.y - mode.py;
		     if (di->item.slider.horizontal)
		       {
			  di->item.slider.wanted_val += dx;
			  di->item.slider.val = di->item.slider.lower +
			     (((di->item.slider.wanted_val *
			    (di->item.slider.upper - di->item.slider.lower)) /
			  (di->item.slider.base_w - di->item.slider.knob_w)) /
			      di->item.slider.unit) * di->item.slider.unit;
		       }
		     else
		       {
			  di->item.slider.wanted_val += dy;
			  di->item.slider.val = di->item.slider.lower +
			     ((((di->item.slider.base_h - di->item.slider.knob_h -
				 di->item.slider.wanted_val) *
			    (di->item.slider.upper - di->item.slider.lower)) /
			  (di->item.slider.base_h - di->item.slider.knob_h)) /
			      di->item.slider.unit) * di->item.slider.unit;
		       }
		     if (di->item.slider.val < di->item.slider.lower)
			di->item.slider.val = di->item.slider.lower;
		     if (di->item.slider.val > di->item.slider.upper)
			di->item.slider.val = di->item.slider.upper;
		     if (di->item.slider.val_ptr)
			*di->item.slider.val_ptr = di->item.slider.val;
		     if (di->func)
			(di->func) (di->val, di->data);
		  }
		DialogDrawItems(d, di, 0, 0, 99999, 99999);
	     }
	}
   }
   EDBUG_RETURN_;
}

void
HandleDestroy(XEvent * ev)
{
   Window              win;
   EWin               *ewin;
   Client             *c;

   EDBUG(5, "HandleDestroy");
   win = ev->xdestroywindow.window;
   DelXID(win);
   ewin = RemoveItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
      if (ewin->iconified > 0)
	 RemoveMiniIcon(ewin);

   mode.context_win = win;

   if (ewin)
     {
	Pager              *p;

	p = FindPager(ev->xdestroywindow.window);
	if (p)
	  {
	     if ((mode.mode == MODE_PAGER_DRAG) && (p->hi_ewin))
	       {
		  PagerHideHi(p);
	       }
	     mode.mode = MODE_NONE;
	     mode.context_pager = NULL;
	  }
	if (ewin == mode.ewin)
	  {
	     if (mode.slideout)
		HideSlideout(mode.slideout, mode.context_win);
	     switch (mode.mode)
	       {
	       case MODE_RESIZE:
	       case MODE_RESIZE_H:
	       case MODE_RESIZE_V:
		  doResizeEnd(NULL);
		  break;
	       case MODE_MOVE:
		  doMoveEnd(NULL);
		  break;
	       default:
		  break;
	       }
	  }
	if (mode.doingslide)
	  {
	     DrawEwinShape(ewin, mode.slidemode, ewin->x, ewin->y, ewin->client.w, ewin->client.h, 2);
	     mode.doingslide = 0;
	  }
	if (ewin == mode.focuswin)
	   FocusToEWin(NULL);
	if (ewin == mode.mouse_over_win)
	   mode.mouse_over_win = NULL;
	if (ewin == mode.ewin)
	   mode.ewin = NULL;
	if (ewin->docked)
	   DockDestroy(ewin);
	DesktopRemoveEwin(ewin);
	FreeEwin(ewin);
	GNOME_SetClientList();
	EDBUG_RETURN_;
     }
   c = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_CLIENT);
   if (c)
      DeleteClient(c);
   if (win == external_pager_window)
     {
	ShowIcons();
     }
   EDBUG_RETURN_;
}

void
HandleProperty(XEvent * ev)
{
   Window              win;
   EWin               *ewin;
   char                title[10240];
   int                 desktop;

   EDBUG(5, "HandleProperty");
   win = ev->xproperty.window;
   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
     {
	GrabX();
	if (ewin->client.title)
	   strncpy(title, ewin->client.title, 10240);
	desktop = ewin->desktop;
	GNOME_GetHintDesktop(ewin, ev->xproperty.atom);
	if ((desktop != ewin->desktop) && (!ewin->sticky))
	   MoveEwinToDesktop(ewin, ewin->desktop);
	GNOME_GetHintIcons(ewin, ev->xproperty.atom);
	GNOME_GetHintAppState(ewin, ev->xproperty.atom);
	GNOME_GetExpandedSize(ewin, ev->xproperty.atom);
	GNOME_GetHint(ewin, ev->xproperty.atom);
	ICCCM_GetTitle(ewin, ev->xproperty.atom);
	ICCCM_GetHints(ewin, ev->xproperty.atom);
	ICCCM_GetInfo(ewin, ev->xproperty.atom);
	ICCCM_Cmap(ewin);
	ICCCM_GetGeoms(ewin, ev->xproperty.atom);
	SessionGetInfo(ewin, ev->xproperty.atom);
	SyncBorderToEwin(ewin);
	if (ewin->client.title)
	   if (strncmp(title, ewin->client.title, 10240))
	     {
		UpdateBorderInfo(ewin);
		CalcEwinSizes(ewin);
	     }
	UngrabX();
     }
   else if (win == root.win)
     {
	/* we're in the root window, not in a client */
	if (mode.kde_support)
	  {
	     KDE_HintChange(ev->xproperty.atom);
	  }
     }
   EDBUG_RETURN_;
}

void
HandleCirculate(XEvent * ev)
{
   Window              win;
   EWin               *ewin;

   EDBUG(5, "HandleCirculate");
   win = ev->xcirculaterequest.window;
   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
     {
	if (ev->xcirculaterequest.place == PlaceOnTop)
	   RaiseEwin(ewin);
	else
	   LowerEwin(ewin);
     }
   else
     {
	if (ev->xcirculaterequest.place == PlaceOnTop)
	   XRaiseWindow(disp, win);
	else
	   XLowerWindow(disp, win);
     }
   EDBUG_RETURN_;
}

void
HandleReparent(XEvent * ev)
{
   Window              par;
   EWin               *ewin, *ewin2;
   void              **lst;
   int                 i, num, found = 0;

   EDBUG(5, "HandleReparent");
   par = ev->xreparent.window;
   EDBUG_RETURN_;
   ewin = FindItem(NULL, ev->xreparent.window, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (!ewin)
      EDBUG_RETURN_;
   lst = ListItemType(&num, LIST_TYPE_EWIN);
   if ((lst) && (num > 0))
     {
	for (i = 0; i < num; i++)
	  {
	     ewin2 = (EWin *) lst[i];
	     if (ewin2->win == ev->xreparent.parent)
		found = 1;
	  }
     }
   if (lst)
      Efree(lst);
   if (!found)
     {
	ewin = RemoveItem(NULL, ev->xreparent.window, LIST_FINDBY_ID, LIST_TYPE_EWIN);
	if (ewin)
	  {
	     if (ewin == mode.ewin)
	       {
		  switch (mode.mode)
		    {
		    case MODE_RESIZE:
		    case MODE_RESIZE_H:
		    case MODE_RESIZE_V:
		       doResizeEnd(NULL);
		       break;
		    case MODE_MOVE:
		       doMoveEnd(NULL);
		       break;
		    default:
		       break;
		    }
	       }
	     ewin->client.win = 0;
	     if ((mode.slideout) && (ewin == mode.ewin))
		HideSlideout(mode.slideout, mode.context_win);
	     FreeEwin(ewin);
	     GNOME_SetClientList();
	  }
     }
   EDBUG_RETURN_;
}

void
HandleConfigureRequest(XEvent * ev)
{
   Window              win, winrel;
   EWin               *ewin, *ewin2;
   int                 x = 0, y = 0, w = 0, h = 0;
   XWindowChanges      xwc;

   EDBUG(5, "HandleConfigureRequest");
   win = ev->xconfigurerequest.window;
   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
     {
	x = ewin->x + ewin->border->border.left;
	y = ewin->y + ewin->border->border.top;
	w = ewin->client.w;
	h = ewin->client.h;
	winrel = 0;
	if (ev->xconfigurerequest.value_mask & CWX)
	   x = ev->xconfigurerequest.x;
	if (ev->xconfigurerequest.value_mask & CWY)
	   y = ev->xconfigurerequest.y;
	if (ev->xconfigurerequest.value_mask & CWWidth)
	   w = ev->xconfigurerequest.width;
	if (ev->xconfigurerequest.value_mask & CWHeight)
	   h = ev->xconfigurerequest.height;
	if (ev->xconfigurerequest.value_mask & CWSibling)
	   winrel = ev->xconfigurerequest.above;
	if (ev->xconfigurerequest.value_mask & CWStackMode)
	  {
	     ewin2 = FindItem(NULL, winrel, LIST_FINDBY_ID, LIST_TYPE_EWIN);
	     if (ewin2)
		winrel = ewin2->win;
	     xwc.sibling = winrel;
	     xwc.stack_mode = ev->xconfigurerequest.detail;
	     if (mode.mode == MODE_NONE)
	       {
		  if (xwc.stack_mode == Above)
		     RaiseEwin(ewin);
		  else if (xwc.stack_mode == Below)
		     LowerEwin(ewin);
	       }
/*        else
 * XConfigureWindow(disp, ewin->win,
 * ev->xconfigurerequest.value_mask &
 * (CWSibling | CWStackMode), &xwc); */
	  }
	/* this ugly workaround here is because x11amp is very brain-dead */
	/* and sets its minunum and maximm sizes the same - fair enough */
	/* to ensure it doesnt get resized - BUT hwne it shades itself */
	/* it resizes down to a smaller size - of course keeping the */
	/* minimum and maximim size same - E unconditionally disallows any */
	/* client window to be resized outside of its constraints */
	/* (any client could do this resize - not just x11amp thus E is */
	/* imposing the hints x11amp set up - this works around by */
	/* modifying the constraints to fit what the app requested */
	if (w < ewin->client.width.min)
	   ewin->client.width.min = w;
	if (w > ewin->client.width.max)
	   ewin->client.width.max = w;
	if (h < ewin->client.height.min)
	   ewin->client.height.min = h;
	if (h > ewin->client.height.max)
	   ewin->client.height.max = h;
	MoveResizeEwin(ewin,
		       x - ewin->border->border.left,
		       y - ewin->border->border.top,
		       w, h);
	if (mode.mode == MODE_MOVE)
	   ICCCM_Configure(ewin);
	{
	   char                pshaped;

	   pshaped = ewin->client.shaped;
	   ICCCM_GetShapeInfo(ewin);
	   if (pshaped != ewin->client.shaped)
	     {
		SyncBorderToEwin(ewin);
		PropagateShapes(ewin->win);
	     }
	}
	ReZoom(ewin);
     }
   else
     {
	xwc.x = ev->xconfigurerequest.x;
	xwc.y = ev->xconfigurerequest.y;
	xwc.width = ev->xconfigurerequest.width;
	xwc.height = ev->xconfigurerequest.height;
	xwc.border_width = ev->xconfigurerequest.border_width;
	xwc.sibling = ev->xconfigurerequest.above;
	xwc.stack_mode = ev->xconfigurerequest.detail;
	XConfigureWindow(disp, win, ev->xconfigurerequest.value_mask, &xwc);
     }
   EDBUG_RETURN_;
}

void
HandleResizeRequest(XEvent * ev)
{
   Window              win;
   EWin               *ewin;
   int                 w, h;

   EDBUG(5, "HandleResizeRequest");
   win = ev->xresizerequest.window;
   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
     {
	w = ev->xresizerequest.width;
	h = ev->xresizerequest.height;
	ResizeEwin(ewin, w, h);
	{
	   char                pshaped;

	   pshaped = ewin->client.shaped;
	   ICCCM_GetShapeInfo(ewin);
	   if (pshaped != ewin->client.shaped)
	     {
		SyncBorderToEwin(ewin);
		PropagateShapes(ewin->win);
	     }
	}
	ReZoom(ewin);
     }
   else
      EResizeWindow(disp, win, ev->xresizerequest.width, ev->xresizerequest.height);
   EDBUG_RETURN_;
}

void
HandleUnmap(XEvent * ev)
{
   Window              win;
   EWin               *ewin;
   int                 i, num_groups;

   EDBUG(5, "HandleUnmap");
   win = ev->xunmap.window;
   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
     {
	if (ewin->iconified > 1)
	  {
	     ewin->iconified = ewin->iconified - 1;
	     EDBUG_RETURN_;
	  }
     }
   if (ewin)
     {
	if (ewin->pager)
	  {
	     if ((mode.mode == MODE_PAGER_DRAG) && (ewin->pager->hi_ewin))
		PagerHideHi(ewin->pager);
	     mode.mode = MODE_NONE;
	     mode.context_pager = NULL;
	  }
	if (ewin->docked)
	   DockDestroy(ewin);
	if (ewin == mode.ewin)
	  {
	     switch (mode.mode)
	       {
	       case MODE_RESIZE:
	       case MODE_RESIZE_H:
	       case MODE_RESIZE_V:
		  doResizeEnd(NULL);
		  break;
	       case MODE_MOVE:
		  doMoveEnd(NULL);
		  break;
	       default:
		  break;
	       }
	  }
	if (!ewin->iconified)
	  {
	     if ((mode.slideout) && (ewin == mode.ewin))
		HideSlideout(mode.slideout, mode.context_win);
	  }
	if (ewin == mode.focuswin)
	   FocusToEWin(NULL);
	if (ewin == mode.mouse_over_win)
	   mode.mouse_over_win = NULL;
	if (ewin == mode.ewin)
	   mode.ewin = NULL;
	num_groups = ewin->num_groups;
	for (i = 0; i < num_groups; i++)
	   RemoveEwinFromGroup(ewin, ewin->groups[0]);
	if (!ewin->iconified)
	  {
	     XTranslateCoordinates(disp, ewin->client.win, root.win,
				   -ewin->border->border.left,
				   -ewin->border->border.top,
				   &ewin->client.x, &ewin->client.y, &win);
	     EReparentWindow(disp, ewin->client.win, root.win,
			     ewin->client.x, ewin->client.y);
	     XRemoveFromSaveSet(disp, ewin->client.win);
	     ICCCM_Withdraw(ewin);
	     RemoveItem(NULL, ewin->client.win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
	     DesktopRemoveEwin(ewin);
	     FreeEwin(ewin);
	     GNOME_SetClientList();
	  }
	else
	   HideEwin(ewin);
     }
   EDBUG_RETURN_;
}

void
HandleMapRequest(XEvent * ev)
{
   EDBUG(5, "HandleMapRequest");
   AddToFamily(ev->xmap.window);
   GNOME_SetClientList();
   EDBUG_RETURN_;
}

void
HandleExpose(XEvent * ev)
{
   Window              win;
   EWin              **ewin;
   int                 i, j, num;

   EDBUG(5, "HandleExpose");

   win = ev->xexpose.window;

   ewin = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   for (i = 0; i < num; i++)
     {
	for (j = 0; j < ewin[i]->border->num_winparts; j++)
	  {
	     if (win == ewin[i]->bits[j].win)
	       {
		  ewin[i]->bits[j].no_expose = 0;
		  ewin[i]->bits[j].expose = 1;
		  if ((DrawEwinWinpart(ewin[i], j)) &&
		      (IsPropagateEwinOnQueue(ewin[i])))
		     PropagateShapes(ewin[i]->win);
		  Efree(ewin);
		  EDBUG_RETURN_;
	       }
	  }
     }

   if (ewin)
      Efree(ewin);

   {
      Button            **button;

      button = (Button **) ListItemType(&num, LIST_TYPE_BUTTON);
      for (i = 0; i < num; i++)
	{
	   if (win == button[i]->win)
	     {
		DrawButton(button[i]);
		Efree(button);
		EDBUG_RETURN_;
	     }
	}
      if (button)
	 Efree(button);
   }

   {
      Dialog             *d;
      int                 bnum;

      d = FindDialogButton(win, &bnum);
      if (d)
	 DialogDrawButton(d, bnum);
      else
	{
	   d = FindDialog(win);
	   if (d)
	     {
		DialogDrawArea(d, ev->xexpose.x, ev->xexpose.y,
			       ev->xexpose.width, ev->xexpose.height);
	     }
	   else
	     {
		DItem              *di;
		int                 x, y, w, h;

		di = FindDialogItem(win, &d);
		GetWinXY(win, &x, &y);
		GetWinWH(win, (unsigned int *)&w, (unsigned int *)&h);
		if (d)
		   DialogDrawArea(d, x, y, w, h);
		if (di)
		  {
		     if (di->type == DITEM_AREA)
		       {
			  if (di->func)
			     (di->func) (di->val, di->data);
		       }
		  }
	     }
	}
   }

   EDBUG_RETURN_;
}

static int          pwin_px, pwin_py;

void
HandleMouseDown(XEvent * ev)
{
   Window              win;
   EWin               *ewin;
   EWin              **ewins;
   int                 i, j, num;
   Button            **buttons;
   ActionClass        *ac;
   Menu               *m;
   MenuItem           *mi;
   int                 desk_click = -1;
   char                double_click = 0;
   float               mode_double_click_time = 0.25;

   EDBUG(5, "HandleMouseDown");

   /* DON'T handle clicks whilst moving/resizing things unless doing manual placement */
   if (mode.mode != MODE_NONE)
      EDBUG_RETURN_;
   if ((mode.cur_menu_mode) && (!clickmenu))
      EDBUG_RETURN_;
   if (ttip)
      HideToolTip(ttip);
   RemoveTimerEvent("TOOLTIP_TIMEOUT");
   if (mode.tooltips)
      DoIn("TOOLTIP_TIMEOUT", mode.tiptime, ToolTipTimeout, 0, NULL);

   if ((((float)(ev->xbutton.time - last_time) / 1000) < mode_double_click_time) &&
       ((int)(ev->xbutton.button) == (int)(last_button)))
      double_click = 1;
   last_time = ev->xbutton.time;
   last_button = ev->xbutton.button;

   last_bpress = click_was_in = win = ev->xbutton.window;

   mode.x = ev->xbutton.x_root;
   mode.y = ev->xbutton.y_root;

   mode.context_win = win;
   for (i = 0; i < mode.numdesktops; i++)
     {
	if (win == desks.desk[i].win)
	  {
	     desk_click = i;
	     break;
	  }
     }
   if (desk_click >= 0)
     {
	ActionClass        *ac;

	XUngrabPointer(disp, CurrentTime);

	ac = FindItem("DESKBINDINGS", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ACLASS);
	if (ac)
	  {
	     if (!EventAclass(ev, ac))
		XSendEvent(disp, bpress_win, False, SubstructureNotifyMask, ev);
	  }
	EDBUG_RETURN_;
     }
   m = FindMenuItem(click_was_in, &mi);
   if ((!m) && ((mode.clickalways) || (mode.focusmode == FOCUS_CLICK)))
     {
	ewin = FindEwinByChildren(win);
	if (!ewin)
	   ewin = FindEwinByBase(win);
	if (ewin)
	   FocusToEWin(ewin);
	if (ewin)
	   RaiseEwin(ewin);
/* allow click to pass thorugh */
	if ((ewin) && (ewin->win_container == win))
	  {
	     XSync(disp, False);
	     XAllowEvents(disp, ReplayPointer, CurrentTime);
	     XSync(disp, False);
	  }
/* done */
     }
   if (m)
     {
	mode.cur_menu_mode = 1;
	mi->state = STATE_CLICKED;
	DrawMenuItem(m, mi, 1);
	if (mi->child)
	  {
	     int                 mx, my;
	     unsigned int        mw, mh;
	     EWin               *ewin2;

	     mode.cur_menu[0] = m;
	     mode.cur_menu_depth = 1;
	     ShowMenuMasker(m);
	     XUngrabPointer(disp, CurrentTime);
	     ewin = FindEwinByMenu(m);
	     if (ewin)
	       {
		  GetWinXY(mi->win, &mx, &my);
		  GetWinWH(mi->win, &mw, &mh);
		  ShowMenu(mi->child, 1);
		  ewin2 = FindEwinByMenu(mi->child);
		  if (ewin2)
		    {
		       MoveEwin(ewin2, ewin->x + ewin->border->border.left + mx + mw,
				ewin->y + ewin->border->border.top + my -
				ewin2->border->border.top);
		       RaiseEwin(ewin2);
		       ShowEwin(ewin2);
		       if (mode.menuslide)
			  UnShadeEwin(ewin2);
		       mode.cur_menu[mode.cur_menu_depth++] = mi->child;
		    }
	       }
	  }
	EDBUG_RETURN_;
     }
   if (double_click)
      ev->xbutton.time = 0;

   ewins = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if (ewins)
     {
	for (i = 0; i < num; i++)
	  {
	     for (j = 0; j < ewins[i]->border->num_winparts; j++)
	       {
		  if (win == ewins[i]->bits[j].win)
		    {
		       if (!clickmenu)
			 {
			    GrabThePointer(win);
			    mode.ewin = ewins[i];
			    ewins[i]->bits[j].state = STATE_CLICKED;
			    ChangeEwinWinpart(ewins[i], j);
			    if ((!ewins[i]->menu) && (!mode.cur_menu_mode))
			       mode.context_ewin = ewins[i];
			    mode.borderpartpress = 1;
			    if (ewins[i]->border->part[j].aclass)
			       EventAclass(ev, ewins[i]->border->part[j].aclass);
			    mode.borderpartpress = 0;
			 }
		       Efree(ewins);
		       EDBUG_RETURN_;
		    }
	       }
	  }
	Efree(ewins);
     }
   if (win)
     {
	buttons = (Button **) ListItemType(&num, LIST_TYPE_BUTTON);
	for (i = 0; i < num; i++)
	  {
	     if ((win == buttons[i]->win) || (win == buttons[i]->event_win))
	       {
		  GrabThePointer(win);
		  if (buttons[i]->inside_win)
		     XSendEvent(disp, buttons[i]->inside_win, False, ButtonPressMask, ev);
		  mode.button = buttons[i];
		  buttons[i]->state = STATE_CLICKED;
		  DrawButton(buttons[i]);
		  ac = FindItem("ACTION_BUTTON_DRAG", 0, LIST_FINDBY_NAME,
				LIST_TYPE_ACLASS);
		  if (ac)
		     EventAclass(ev, ac);
		  if (buttons[i]->aclass)
		     EventAclass(ev, buttons[i]->aclass);
		  Efree(buttons);
		  EDBUG_RETURN_;
	       }
	  }
	if (buttons)
	   Efree(buttons);
     }
   {
      Dialog             *d;
      int                 bnum;

      d = FindDialogButton(win, &bnum);
      if (d)
	 DialogActivateButton(win, 2);
      else
	{
	   DItem              *di;

	   di = FindDialogItem(win, &d);
	   if (di)
	     {
		if (di->type == DITEM_AREA)
		  {
		     if (di->item.area.event_func)
			(di->item.area.event_func) (0, ev);
		  }
		else if (di->type == DITEM_SLIDER)
		  {
		     if (win == di->item.slider.base_win)
		       {
			  if (di->item.slider.horizontal)
			    {
			       if (ev->xbutton.x >
				   (di->item.slider.knob_x +
				    (di->item.slider.knob_w / 2)))
				  di->item.slider.val += di->item.slider.jump;
			       else
				  di->item.slider.val -= di->item.slider.jump;
			    }
			  else
			    {
			       if (ev->xbutton.y >
				   (di->item.slider.knob_y +
				    (di->item.slider.knob_h / 2)))
				  di->item.slider.val -= di->item.slider.jump;
			       else
				  di->item.slider.val += di->item.slider.jump;
			    }
			  if (di->item.slider.val < di->item.slider.lower)
			     di->item.slider.val = di->item.slider.lower;
			  if (di->item.slider.val > di->item.slider.upper)
			     di->item.slider.val = di->item.slider.upper;
			  if (di->item.slider.val_ptr)
			     *di->item.slider.val_ptr = di->item.slider.val;
			  if (di->func)
			     (di->func) (di->val, di->data);
		       }
		     else if (win == di->item.slider.knob_win)
		       {
			  di->item.slider.in_drag = 1;
			  if (di->item.slider.horizontal)
			     di->item.slider.wanted_val = di->item.slider.knob_x;
			  else
			     di->item.slider.wanted_val = di->item.slider.knob_y;
		       }
		  }
		di->clicked = 1;
		DialogDrawItems(d, di, 0, 0, 99999, 99999);
	     }
	}
   }
   ewin = FindEwinByBase(ev->xbutton.window);
   if (ewin)
     {
	ActionClass        *ac;

	ac = (ActionClass *) FindItem("BUTTONBINDINGS", 0,
				      LIST_FINDBY_NAME,
				      LIST_TYPE_ACLASS);
	if (ac)
	  {
	     mode.ewin = ewin;
	     GrabThePointer(ewin->win);
	     mode.borderpartpress = 1;
	     if (EventAclass(ev, ac))
	       {
		  mode.borderpartpress = 0;
		  EDBUG_RETURN_;
	       }
	     mode.borderpartpress = 0;
	  }
     }
   {
      Pager              *p;

      p = FindPager(ev->xbutton.window);
      if (p)
	{
	   if (ev->xbutton.window == p->hi_win)
	     {
		int                 hx, hy;
		Window              dw;

		XTranslateCoordinates(disp, p->hi_win, p->win, 0, 0, &hx, &hy, &dw);
		ev->xbutton.x += hx;
		ev->xbutton.y += hy;
	     }
	   if (ev->xbutton.button == 3)
	     {
		if ((ev->xbutton.x >= 0) && (ev->xbutton.y >= 0) &&
		    (ev->xbutton.x < p->w) && (ev->xbutton.y < p->h))
		   PagerShowMenu(p, ev->xbutton.x, ev->xbutton.y);
	     }
	   else if (ev->xbutton.button == 1)
	     {
		ewin = EwinInPagerAt(p, ev->xbutton.x, ev->xbutton.y);
		if ((ewin) && (!ewin->pager))
		  {
		     Window              dw;
		     int                 wx, wy, ww, wh, ax, ay, cx, cy,
		                         px, py;

		     PagerHideHi(p);
		     pwin_px = ewin->x;
		     pwin_py = ewin->y;
		     GetAreaSize(&ax, &ay);
		     cx = desks.desk[p->desktop].current_area_x;
		     cy = desks.desk[p->desktop].current_area_y;
		     wx = ((ewin->x + (cx * root.w)) * (p->w / ax)) / root.w;
		     wy = ((ewin->y + (cy * root.h)) * (p->h / ay)) / root.h;
		     ww = ((ewin->w) * (p->w / ax)) / root.w;
		     wh = ((ewin->h) * (p->h / ay)) / root.h;
		     XTranslateCoordinates(disp, p->win, root.win, 0, 0, &px, &py, &dw);
		     EMoveResizeWindow(disp, p->hi_win, px + wx, py + wy, ww, wh);
		     ESetWindowBackgroundPixmap(disp, p->hi_win, ewin->mini_pmap);
		     EMapRaised(disp, p->hi_win);
		     GrabThePointer(p->hi_win);
		     p->hi_visible = 1;
		     mode.mode = MODE_PAGER_DRAG_PENDING;
		     mode.context_pager = p;
		     pgd_x = ewin->x;
		     pgd_y = ewin->y;
		  }
	     }
	}
   }
   EDBUG_RETURN_;
}

void
HandleMouseUp(XEvent * ev)
{
   Window              win, win2;
   EWin               *ewin;
   EWin              **ewins;
   int                 i, j, num;
   Button            **buttons;
   char                wasdrag;
   char                wasmovres;
   char                justclicked = 0;
   Slideout           *pslideout;
   Menu               *m;
   MenuItem           *mi;
   EWin              **gwins;

   EDBUG(5, "HandleMouseUp");

   /* DON'T handle clicks whilst moving/resizing things */
   if ((mode.mode != MODE_NONE) &&
       (!((mode.place) && (mode.mode == MODE_MOVE))))
     {
	if ((int)last_button != (int)ev->xbutton.button)
	   EDBUG_RETURN_;
     }
   if (ttip)
      HideToolTip(ttip);
   RemoveTimerEvent("TOOLTIP_TIMEOUT");
   if (mode.tooltips)
      DoIn("TOOLTIP_TIMEOUT", mode.tiptime, ToolTipTimeout, 0, NULL);

   UnGrabTheButtons();
   win2 = WindowAtXY(ev->xbutton.x_root, ev->xbutton.y_root);
   win = ev->xbutton.window;

   mode.x = ev->xbutton.x_root;
   mode.y = ev->xbutton.y_root;

   if ((mode.place) && (mode.mode == MODE_MOVE))
     {
	ewin = GetEwin();
	if (ewin)
	  {
	     gwins = ListWinGroupMembersForEwin(ewin, ACTION_MOVE, mode.nogroup, &num);
	     if ((mode.movemode == 0) && (mode.mode == MODE_MOVE))
		for (i = 0; i < num; i++)
		   DetermineEwinFloat(gwins[i], 0, 0);
	     Efree(gwins);
	  }
	doMoveEnd(NULL);
	if (mode.have_place_grab)
	  {
	     mode.have_place_grab = 0;
	     XUngrabPointer(disp, CurrentTime);
	  }
	mode.place = 0;
	EDBUG_RETURN_;
     }
   wasdrag = 0;
   wasmovres = 0;
   pslideout = mode.slideout;

   if ((last_bpress) && (last_bpress != win))
     {
	ewin = FindEwinByChildren(last_bpress);
	if (ewin)
	  {
	     for (j = 0; j < ewin->border->num_winparts; j++)
	       {
		  if (last_bpress == ewin->bits[j].win)
		    {
		       if ((ewin->bits[j].state == STATE_CLICKED) &&
			   (!ewin->bits[j].left))
			  ewin->bits[j].state = STATE_HILITED;
		       else
			  ewin->bits[j].state = STATE_NORMAL;
		       ewin->bits[j].left = 0;
		       ChangeEwinWinpart(ewin, j);
		       break;
		    }
	       }
	  }
	last_bpress = 0;
     }
   wasdrag = 0;
   wasmovres = 0;
   switch (mode.mode)
     {
     case MODE_RESIZE:
     case MODE_RESIZE_H:
     case MODE_RESIZE_V:
	doResizeEnd(NULL);
	wasmovres = 1;
	break;
     case MODE_MOVE:
	ewin = GetEwin();
	if (ewin)
	  {
	     gwins = ListWinGroupMembersForEwin(ewin, ACTION_MOVE, mode.nogroup, &num);
	     if ((mode.movemode == 0) && (mode.mode == MODE_MOVE))
		for (i = 0; i < num; i++)
		   DetermineEwinFloat(gwins[i], 0, 0);
	     Efree(gwins);
	  }
	doMoveEnd(NULL);
	if (mode.have_place_grab)
	  {
	     mode.have_place_grab = 0;
	     XUngrabPointer(disp, CurrentTime);
	  }
	mode.place = 0;
	wasmovres = 1;
	mode.ewin = NULL;
	break;
     default:
	break;
     }

   if (sentpress)
     {
	sentpress = 0;
	XSendEvent(disp, bpress_win, False, SubstructureNotifyMask, ev);
     }
   mode.context_win = click_was_in;
   mode.destroy = 0;
   pslideout = mode.slideout;
   if (mode.slideout)
     {
	ewin = FindEwinByChildren(mode.slideout->from_win);
	if (ewin)
	   mode.ewin = ewin;
     }
   if (mode.mode == MODE_DESKDRAG)
      mode.mode = MODE_NONE;
   if (mode.mode == MODE_BUTTONDRAG)
     {
	if (!mode.button_move_pending)
	   wasdrag = 1;
	doDragButtonEnd(NULL);
     }
   m = FindMenuItem(win, &mi);
   if ((((float)(ev->xbutton.time - last_time) / 1000) < 0.5) &&
       (mode.cur_menu_depth > 0) && (!clickmenu))
     {
	clickmenu = 1;
	justclicked = 1;
     }
   if ((m) && (mi->state))
     {
	mi->state = STATE_HILITED;
	DrawMenuItem(m, mi, 1);
	if ((mi->act_id) && (!justclicked))
	  {
	     (*(ActionFunctions[mi->act_id])) (mi->params);
	     if (clickmenu)
	       {
		  for (i = 0; i < mode.cur_menu_depth; i++)
		    {
		       if (!mode.cur_menu[i]->stuck)
			  HideMenu(mode.cur_menu[i]);
		    }
		  HideMenuMasker();
		  mode.cur_menu_depth = 0;
		  mode.cur_menu_mode = 0;
		  mode.context_ewin = NULL;
		  last_bpress = 0;
		  clickmenu = 0;
		  EDBUG_RETURN_;
	       }
	  }
     }
   if ((mode.cur_menu_mode) && (!clickmenu))
     {
	if (!m)
	  {
	     Window              ww;

	     ww = WindowAtXY(mode.x, mode.y);
	     if ((ewin = FindEwinByChildren(ww)))
	       {
		  for (i = 0; i < ewin->border->num_winparts; i++)
		    {
		       if (ww == ewin->bits[i].win)
			 {
			    if ((ewin->border->part[i].flags & FLAG_TITLE) &&
				(ewin->menu))
			      {
				 ewin->menu->stuck = 1;
				 mode.context_ewin = ewin;
				 i = ewin->border->num_winparts;
			      }
			 }
		    }
	       }
	  }
	for (i = 0; i < mode.cur_menu_depth; i++)
	  {
	     if (!mode.cur_menu[i]->stuck)
		HideMenu(mode.cur_menu[i]);
	  }
	HideMenuMasker();
	mode.cur_menu_depth = 0;
	mode.cur_menu_mode = 0;
	last_bpress = 0;
	clickmenu = 0;
	EDBUG_RETURN_;
     }
   if ((mode.cur_menu_mode) && (!justclicked))
     {
	for (i = 0; i < mode.cur_menu_depth; i++)
	  {
	     if (!mode.cur_menu[i]->stuck)
		HideMenu(mode.cur_menu[i]);
	  }
	HideMenuMasker();
	mode.cur_menu_depth = 0;
	mode.cur_menu_mode = 0;
	last_bpress = 0;
	clickmenu = 0;
	EDBUG_RETURN_;
     }
   ewins = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   for (i = 0; i < num; i++)
     {
	for (j = 0; j < ewins[i]->border->num_winparts; j++)
	  {
	     if (click_was_in == ewins[i]->bits[j].win)
	       {
		  if (!clickmenu)
		    {
		       if (ewins[i] == mode.ewin)
			 {
			    switch (mode.mode)
			      {
			      case MODE_RESIZE:
			      case MODE_RESIZE_H:
			      case MODE_RESIZE_V:
				 doResizeEnd(NULL);
				 break;
			      case MODE_MOVE:
				 doMoveEnd(NULL);
				 break;
			      default:
				 break;
			      }
			 }
		       if ((ewins[i]->bits[j].state == STATE_CLICKED) &&
			   (!ewins[i]->bits[j].left))
			  ewins[i]->bits[j].state = STATE_HILITED;
		       else
			  ewins[i]->bits[j].state = STATE_NORMAL;
		       ewins[i]->bits[j].left = 0;
		       ChangeEwinWinpart(ewins[i], j);
		       if ((!ewins[i]->menu) && (!mode.cur_menu_mode))
			  mode.context_ewin = ewins[i];
		       mode.borderpartpress = 1;
		       if ((click_was_in == win2) && (ewins[i]->border->part[j].aclass) &&
			   (!wasmovres))
			  EventAclass(ev, ewins[i]->border->part[j].aclass);
		       mode.borderpartpress = 0;
		       if ((mode.slideout) && (pslideout))
			  HideSlideout(mode.slideout, mode.context_win);
		    }
		  Efree(ewins);
		  click_was_in = 0;
		  last_bpress = 0;
		  EDBUG_RETURN_;
	       }
	  }
     }
   if (ewins)
      Efree(ewins);
   if (click_was_in)
     {
	buttons = (Button **) ListItemType(&num, LIST_TYPE_BUTTON);
	for (i = 0; i < num; i++)
	  {
	     if ((click_was_in == buttons[i]->win) || (click_was_in == buttons[i]->event_win))
	       {
		  if ((buttons[i]->inside_win) && (!wasmovres))
		     XSendEvent(disp, buttons[i]->inside_win, False,
				ButtonReleaseMask, ev);
		  mode.button = buttons[i];
		  if ((buttons[i]->state == STATE_CLICKED) &&
		      (!buttons[i]->left))
		     buttons[i]->state = STATE_HILITED;
		  else
		     buttons[i]->state = STATE_NORMAL;
		  buttons[i]->left = 0;
		  DrawButton(buttons[i]);
		  if ((buttons[i]->aclass) && (!wasdrag) && (!wasmovres))
		     EventAclass(ev, buttons[i]->aclass);
		  mode.destroy = 0;
		  if ((mode.slideout) && (pslideout))
		     HideSlideout(mode.slideout, mode.context_win);
		  Efree(buttons);
		  click_was_in = 0;
		  last_bpress = 0;
		  EDBUG_RETURN_;
	       }
	  }
	if (buttons)
	   Efree(buttons);
     }
   {
      Dialog             *d;
      int                 bnum;

      d = FindDialogButton(click_was_in, &bnum);
      if (d)
	 DialogActivateButton(click_was_in, 3);
      else
	{
	   DItem              *di;

	   di = FindDialogItem(click_was_in, &d);
	   if (di)
	     {
		di->clicked = 0;
		if (click_was_in)
		  {
		     if (di->type == DITEM_AREA)
		       {
			  if (di->item.area.event_func)
			     (di->item.area.event_func) (0, ev);
		       }
		     else if (di->type == DITEM_CHECKBUTTON)
		       {
			  if (di->item.check_button.onoff)
			     di->item.check_button.onoff = 0;
			  else
			     di->item.check_button.onoff = 1;
			  if (di->item.check_button.onoff_ptr)
			     *di->item.check_button.onoff_ptr =
				di->item.check_button.onoff;
		       }
		     else if (di->type == DITEM_RADIOBUTTON)
		       {
			  DItem              *dii;

			  dii = di->item.radio_button.first;
			  while (dii)
			    {
			       if (dii->item.radio_button.onoff)
				 {
				    dii->item.radio_button.onoff = 0;
				    DialogDrawItems(d, dii, 0, 0, 99999, 99999);
				 }
			       dii = dii->item.radio_button.next;
			    }
			  di->item.radio_button.onoff = 1;
			  if (di->item.radio_button.val_ptr)
			     *di->item.radio_button.val_ptr =
				di->item.radio_button.val;
		       }
		     else if (di->type == DITEM_SLIDER)
		       {
			  if (win == di->item.slider.knob_win)
			     di->item.slider.in_drag = 0;
		       }
		  }
		DialogDrawItems(d, di, 0, 0, 99999, 99999);
		if (click_was_in)
		  {
		     if (di->func)
			(di->func) (di->val, di->data);
		  }
	     }
	}
   }
   ewin = FindEwinByBase(ev->xbutton.window);
   if (ewin)
     {
	ActionClass        *ac;

	ac = (ActionClass *) FindItem("BUTTONBINDINGS", 0,
				      LIST_FINDBY_NAME,
				      LIST_TYPE_ACLASS);
	if (ac)
	  {
	     mode.borderpartpress = 1;
	     if (EventAclass(ev, ac))
	       {
		  mode.borderpartpress = 0;
		  mode.destroy = 0;
		  if ((mode.slideout) && (pslideout))
		     HideSlideout(mode.slideout, mode.context_win);
		  click_was_in = 0;
		  EDBUG_RETURN_;
	       }
	     mode.borderpartpress = 0;
	  }
     }
   if (!wasmovres)
     {
	Pager              *p;
	int                 pax, pay;

	p = FindPager(ev->xbutton.window);
	if ((p) && (ev->xbutton.button == 2))
	  {
	     PagerAreaAt(p, ev->xbutton.x, ev->xbutton.y, &pax, &pay);
	     GotoDesktop(p->desktop);
	     if (p->desktop != desks.current)
	       {
		  AUDIO_PLAY("SOUND_DESKTOP_SHUT");
	       }
	     SetCurrentArea(pax, pay);
	  }
	else if ((p) && (ev->xbutton.button == 1))
	  {
	     if (ev->xbutton.window == p->hi_win)
	       {
		  int                 hx, hy;
		  Window              dw;

		  XTranslateCoordinates(disp, p->hi_win, p->win, 0, 0, &hx, &hy, &dw);
		  ev->xbutton.x += hx;
		  ev->xbutton.y += hy;
	       }
	     if (!FindItem((char *)p->hi_ewin, 0,
			   LIST_FINDBY_POINTER, LIST_TYPE_EWIN))
		p->hi_ewin = NULL;
	     if ((mode.mode == MODE_PAGER_DRAG) && (p->hi_ewin))
	       {
		  ewin = NULL;
		  for (i = 0; i < desks.desk[desks.current].num; i++)
		    {
		       EWin               *ew;

		       ew = desks.desk[desks.current].list[i];
		       if (((ew->pager) || (ew->ibox)) &&
			   ((ew->desktop == desks.current) ||
			    (ew->sticky)))
			 {
			    if ((ev->xbutton.x_root >= (ew->x + ew->border->border.left)) &&
				(ev->xbutton.x_root < (ew->x + ew->w - ew->border->border.right)) &&
				(ev->xbutton.y_root >= (ew->y + ew->border->border.top)) &&
				(ev->xbutton.y_root < (ew->y + ew->h - ew->border->border.bottom)))
			      {
				 ewin = ew;
				 i = desks.desk[desks.current].num;
			      }
			 }
		    }
		  ewin = GetEwinPointerInClient();
		  if ((ewin) && (ewin->pager))
		    {
		       Pager              *pp;
		       int                 w, h, x, y, ax, ay, cx, cy, px,
		                           py;
		       int                 wx, wy;
		       Window              dw;

		       pp = ewin->pager;
		       cx = desks.desk[pp->desktop].current_area_x;
		       cy = desks.desk[pp->desktop].current_area_y;
		       GetAreaSize(&ax, &ay);
		       GetWinXY(p->hi_win, &x, &y);
		       GetWinWH(p->hi_win, &w, &h);
		       XTranslateCoordinates(disp, pp->win, root.win, 0, 0,
					     &px, &py, &dw);
		       wx = ((x - px) - (cx * (pp->w / ax))) *
			  (root.w / (pp->w / ax));
		       wy = ((y - py) - (cy * (pp->h / ay))) *
			  (root.h / (pp->h / ay));
		       if (((x + w) <= px) ||
			   ((y + h) <= py) ||
			   (x >= (px + pp->w)) ||
			   (y >= (py + pp->h)))
			 {
			    int                 ndesk, nx, ny;

			    ndesk = desks.current;
			    nx = (int)ev->xbutton.x_root -
			       desks.desk[desks.current].x -
			       ((int)p->hi_ewin->w / 2);
			    ny = (int)ev->xbutton.y_root -
			       desks.desk[desks.current].y -
			       ((int)p->hi_ewin->h / 2);
			    MoveEwin(p->hi_ewin, nx, ny);
			    MoveEwinToDesktop(p->hi_ewin, ndesk);
			 }
		       else
			  MoveEwinToDesktopAt(p->hi_ewin, pp->desktop, wx, wy);
		    }
		  else if ((ewin) && (ewin->ibox) &&
			   (!((p->hi_ewin->ibox) ||
			      ((ewin->client.need_input) &&
			       ((ewin->skiptask) || (ewin->skipwinlist)))
			    )))
		    {
		       char                was_shaded;

		       MoveEwin(p->hi_ewin, pwin_px, pwin_py);
		       ICCCM_Configure(p->hi_ewin);
		       was_shaded = p->hi_ewin->shaded;
		       if (ewin->ibox)
			 {
			    IB_Animate(1, p->hi_ewin, ewin->ibox->ewin);
			    UpdateAppIcon(p->hi_ewin, ewin->ibox->icon_mode);
			 }
		       HideEwin(p->hi_ewin);
		       MoveEwin(p->hi_ewin,
				pwin_px + ((desks.desk[p->hi_ewin->desktop].current_area_x) - p->hi_ewin->area_x) * root.w,
				pwin_py + ((desks.desk[p->hi_ewin->desktop].current_area_y) - p->hi_ewin->area_y) * root.h);
		       if (was_shaded != p->hi_ewin->shaded)
			  InstantShadeEwin(p->hi_ewin);
		       AddEwinToIconbox(ewin->ibox, p->hi_ewin);
		       ICCCM_Iconify(p->hi_ewin);
		    }
		  else
		    {
		       int                 ndesk, nx, ny;

		       ndesk = desks.current;
		       nx = (int)ev->xbutton.x_root -
			  desks.desk[desks.current].x -
			  ((int)p->hi_ewin->w / 2);
		       ny = (int)ev->xbutton.y_root -
			  desks.desk[desks.current].y -
			  ((int)p->hi_ewin->h / 2);
		       MoveEwin(p->hi_ewin, nx, ny);
		       MoveEwinToDesktop(p->hi_ewin, ndesk);
		    }
		  RedrawPagersForDesktop(p->hi_ewin->desktop, 3);
		  ForceUpdatePagersForDesktop(p->hi_ewin->desktop);
		  p->hi_visible = 1;
		  PagerHideHi(p);
	       }
	     else if ((ev->xbutton.x >= 0) && (ev->xbutton.y >= 0) &&
		      (ev->xbutton.x < p->w) && (ev->xbutton.y < p->h))
	       {
		  PagerAreaAt(p, ev->xbutton.x, ev->xbutton.y, &pax, &pay);
		  GotoDesktop(p->desktop);
		  SetCurrentArea(pax, pay);
		  ewin = EwinInPagerAt(p, ev->xbutton.x, ev->xbutton.y);
		  if (ewin)
		    {
		       RaiseEwin(ewin);
		       FocusToEWin(ewin);
		    }
	       }
	     mode.mode = MODE_NONE;
	     mode.context_pager = NULL;
	  }
     }
   mode.destroy = 0;
   if ((mode.slideout) && (pslideout))
      HideSlideout(mode.slideout, mode.context_win);
   click_was_in = 0;
   EDBUG_RETURN_;
}

void
SubmenuShowTimeout(int val, void *dat)
{
   int                 mx, my;
   unsigned int        mw, mh;
   EWin               *ewin2;
   struct _mdata      *data;

   data = (struct _mdata *)dat;
   if (!data)
      return;
   if (!data->m)
      return;
   if (!FindEwinByMenu(data->m))
      return;
   GetWinXY(data->mi->win, &mx, &my);
   GetWinWH(data->mi->win, &mw, &mh);
   ShowMenu(data->mi->child, 1);
   ewin2 = FindEwinByMenu(data->mi->child);
   if (ewin2)
     {
	MoveEwin(ewin2, data->ewin->x + data->ewin->border->border.left + mx + mw,
		 data->ewin->y + data->ewin->border->border.top + my -
		 ewin2->border->border.top);
	RaiseEwin(ewin2);
	ShowEwin(ewin2);
	if (mode.menuslide)
	   UnShadeEwin(ewin2);
	if (mode.cur_menu[mode.cur_menu_depth - 1] != data->mi->child)
	   mode.cur_menu[mode.cur_menu_depth++] = data->mi->child;
     }
   val = 0;
}

void
HandleMouseIn(XEvent * ev)
{
   Window              win;
   EWin               *ewin;
   EWin              **ewins;
   int                 i, j, num;
   Button            **buttons;
   Menu               *m;
   MenuItem           *mi;
   static struct _mdata mdata;

   EDBUG(5, "HandleMouseIn");

   if (ttip)
      HideToolTip(ttip);
   RemoveTimerEvent("TOOLTIP_TIMEOUT");
   if (mode.tooltips)
      DoIn("TOOLTIP_TIMEOUT", mode.tiptime, ToolTipTimeout, 0, NULL);

   EdgeHandleEnter(ev);
   win = ev->xcrossing.window;

   mode.context_win = win;

   m = FindMenuItem(win, &mi);
   if (m)
     {
	int                 j;

	PagerHideAllHi();
	if ((win == mi->icon_win) && (ev->xcrossing.detail == NotifyAncestor))
	   EDBUG_RETURN_;
	if ((win == mi->win) && (ev->xcrossing.detail == NotifyInferior))
	   EDBUG_RETURN_;
	mi->state = STATE_HILITED;
	DrawMenuItem(m, mi, 1);

	RemoveTimerEvent("SUBMENU_SHOW");
	for (i = 0; i < mode.cur_menu_depth; i++)
	  {
	     if (mode.cur_menu[i] == m)
	       {
		  if ((!mi->child) ||
		      ((mi->child) && (mode.cur_menu[i + 1] != mi->child)))
		    {
		       for (j = i + 1; j < mode.cur_menu_depth; j++)
			  HideMenu(mode.cur_menu[j]);
		       mode.cur_menu_depth = i + 1;
		       i = mode.cur_menu_depth;
		       break;
		    }
	       }
	  }
	if ((mi->child) && (mode.cur_menu_mode))
	  {
	     ewin = FindEwinByMenu(m);
	     if (ewin)
	       {
		  mdata.m = m;
		  mdata.mi = mi;
		  mdata.ewin = ewin;
		  DoIn("SUBMENU_SHOW", 0.2, SubmenuShowTimeout,
		       0, &mdata);
	       }
	  }
	EDBUG_RETURN_;
     }
   ewins = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   for (i = 0; i < num; i++)
     {
	for (j = 0; j < ewins[i]->border->num_winparts; j++)
	  {
	     if (win == ewins[i]->bits[j].win)
	       {
		  PagerHideAllHi();
		  if (!clickmenu)
		    {
		       mode.noewin = 0;
		       mode.ewin = ewins[i];
		       if (ewins[i]->bits[j].state == STATE_CLICKED)
			  ewins[i]->bits[j].left = 0;
		       else
			 {
			    ewins[i]->bits[j].state = STATE_HILITED;
			    ChangeEwinWinpart(ewins[i], j);
			    if ((!ewins[i]->menu) && (!mode.cur_menu_mode))
			       mode.context_ewin = ewins[i];
			    if (ewins[i]->border->part[j].aclass)
			       EventAclass(ev, ewins[i]->border->part[j].aclass);
			 }
		    }
		  Efree(ewins);
		  EDBUG_RETURN_;
	       }
	  }
     }
   if (ewins)
      Efree(ewins);
   if (win)
     {
	buttons = (Button **) ListItemType(&num, LIST_TYPE_BUTTON);
	for (i = 0; i < num; i++)
	  {
	     if ((win == buttons[i]->win) || (win == buttons[i]->event_win))
	       {
		  PagerHideAllHi();
		  mode.button = buttons[i];
		  if (buttons[i]->state == STATE_CLICKED)
		     buttons[i]->left = 0;
		  else
		    {
		       buttons[i]->state = STATE_HILITED;
		       DrawButton(buttons[i]);
		       if (buttons[i]->aclass)
			  EventAclass(ev, buttons[i]->aclass);
		    }
		  Efree(buttons);
		  EDBUG_RETURN_;
	       }
	  }
	if (buttons)
	   Efree(buttons);
     }
   {
      Dialog             *d;
      int                 bnum;

      d = FindDialogButton(win, &bnum);

      if (d)
	{
	   PagerHideAllHi();
	   DialogActivateButton(win, 0);
	}
      else
	{
	   DItem              *di;

	   di = FindDialogItem(win, &d);
	   if (di)
	     {
		PagerHideAllHi();
		if (di->type == DITEM_AREA)
		  {
		     if (di->item.area.event_func)
			(di->item.area.event_func) (0, ev);
		  }
		di->hilited = 1;
		DialogDrawItems(d, di, 0, 0, 99999, 99999);
	     }
	}
   }
   EDBUG_RETURN_;
}

void
HandleMouseOut(XEvent * ev)
{
   Window              win;
   EWin              **ewins;
   int                 i, j, num;
   Button            **buttons;
   Menu               *m;
   MenuItem           *mi;

   EDBUG(5, "HandleMouseOut");

   if (ttip)
      HideToolTip(ttip);
   RemoveTimerEvent("TOOLTIP_TIMEOUT");
   if (mode.tooltips)
      DoIn("TOOLTIP_TIMEOUT", mode.tiptime, ToolTipTimeout, 0, NULL);

   EdgeHandleLeave(ev);

   win = ev->xcrossing.window;

   mode.context_win = win;

   m = FindMenuItem(win, &mi);
   if (m)
     {
	if ((win == mi->icon_win) && (ev->xcrossing.detail == NotifyAncestor))
	   EDBUG_RETURN_;
	if ((win == mi->win) && (ev->xcrossing.detail == NotifyInferior))
	   EDBUG_RETURN_;
	mi->state = STATE_NORMAL;
	DrawMenuItem(m, mi, 1);
	EDBUG_RETURN_;
     }
   ewins = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   ICCCM_Cmap(NULL);
   for (i = 0; i < num; i++)
     {
	for (j = 0; j < ewins[i]->border->num_winparts; j++)
	  {
	     if (win == ewins[i]->bits[j].win)
	       {
		  if (!clickmenu)
		    {
		       if (mode.mode == MODE_NONE)
			  mode.ewin = NULL;
		       else
			  mode.noewin = 1;
		       if (ewins[i]->bits[j].state == STATE_CLICKED)
			  ewins[i]->bits[j].left = 1;
		       else
			 {
			    ewins[i]->bits[j].state = STATE_NORMAL;
			    ChangeEwinWinpart(ewins[i], j);
			    if ((!ewins[i]->menu) && (!mode.cur_menu_mode))
			       mode.context_ewin = ewins[i];
			    if (ewins[i]->border->part[j].aclass)
			       EventAclass(ev, ewins[i]->border->part[j].aclass);
			 }
		    }
		  Efree(ewins);
		  EDBUG_RETURN_;
	       }
	  }
     }
   if (ewins)
      Efree(ewins);
   if (win)
     {
	buttons = (Button **) ListItemType(&num, LIST_TYPE_BUTTON);
	for (i = 0; i < num; i++)
	  {
	     if ((win == buttons[i]->win) || (win == buttons[i]->event_win))
	       {
		  mode.button = NULL;
		  if (buttons[i]->state == STATE_CLICKED)
		     buttons[i]->left = 1;
		  else
		    {
		       buttons[i]->state = STATE_NORMAL;
		       DrawButton(buttons[i]);
		       if (buttons[i]->aclass)
			  EventAclass(ev, buttons[i]->aclass);
		    }
		  Efree(buttons);
		  EDBUG_RETURN_;
	       }
	  }
	if (buttons)
	   Efree(buttons);
     }
   {
      Dialog             *d;
      int                 bnum;

      d = FindDialogButton(win, &bnum);
      if (d)
	 DialogActivateButton(win, 1);
      else
	{
	   DItem              *di;

	   di = FindDialogItem(win, &d);
	   if (di)
	     {
		if (di->type == DITEM_AREA)
		  {
		     if (di->item.area.event_func)
			(di->item.area.event_func) (0, ev);
		  }
		di->hilited = 0;
		DialogDrawItems(d, di, 0, 0, 99999, 99999);
	     }
	}
   }
   EDBUG_RETURN_;
}
