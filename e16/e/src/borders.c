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
#include <sys/time.h>

#if 0
#define DELETE_EWIN_REFERENCE(ew, ew_ref) \
	({ if (ew_ref == ew) { printf("Stale ewin ref (" #ew_ref ")\n"); ew_ref = NULL; } })
#else
#define DELETE_EWIN_REFERENCE(ew, ew_ref) \
	({ if (ew_ref == ew) { ew_ref = NULL; } })
#endif

void
SetFrameProperty(EWin * ewin)
{
   static Atom         atom_set = 0;
   CARD32              val[4];

   if (!atom_set)
      atom_set = XInternAtom(disp, "_E_FRAME_SIZE", False);
   if (ewin->border)
     {
	val[0] = ewin->border->border.left;
	val[1] = ewin->border->border.right;
	val[2] = ewin->border->border.top;
	val[3] = ewin->border->border.bottom;
     }
   else
      val[0] = val[1] = val[2] = val[3] = 0;
   XChangeProperty(disp, ewin->client.win, atom_set, XA_CARDINAL, 32,
		   PropModeReplace, (unsigned char *)&val, 4);
}

void
KillEwin(EWin * ewin)
{
   EWin              **gwins;
   int                 num, num_groups, i, j;

   if (!ewin)
      EDBUG_RETURN_;
   gwins = ListWinGroupMembersForEwin(ewin, ACTION_KILL, mode.nogroup, &num);
   if (gwins)
     {
	for (i = 0; i < num; i++)
	  {
	     num_groups = gwins[i]->num_groups;
	     for (j = 0; j < num_groups; j++)
		RemoveEwinFromGroup(gwins[i], gwins[i]->groups[0]);
	     ICCCM_Delete(gwins[i]);
	     AUDIO_PLAY("SOUND_WINDOW_CLOSE");
	  }
	Efree(gwins);
     }
   EDBUG_RETURN_;

}

void
DetermineEwinFloat(EWin * ewin, int dx, int dy)
{
   char                dofloat = 0;

   EDBUG(5, "DetermineEwinFloat");

   if ((ewin->desktop != 0) && (ewin->floating < 2)
       && ((desks.desk[ewin->desktop].x != 0)
	   || (desks.desk[ewin->desktop].y != 0)
	   || (desks.current != ewin->desktop)))
     {
	if ((desks.dragdir == 0)
	    && (((ewin->x + dx < 0) || ((ewin->x + dx + ewin->w <= root.w)
					&&
					((DesktopAt
					  (desks.desk[ewin->desktop].x +
					   ewin->x + dx + ewin->w - 1,
					   desks.desk[ewin->desktop].y) !=
					  ewin->desktop))))))
	   dofloat = 1;
	if ((desks.dragdir == 1)
	    && (((ewin->x + dx + ewin->w > root.w) || ((ewin->x + dx >= 0)
						       &&
						       ((DesktopAt
							 (desks.desk
							  [ewin->desktop].x +
							  ewin->x + dx,
							  desks.
							  desk[ewin->desktop].
							  y) !=
							 ewin->desktop))))))
	   dofloat = 1;
	if ((desks.dragdir == 2)
	    &&
	    (((ewin->
	       y + dy < 0) || ((ewin->y + dy + ewin->h <= root.h)
			       &&
			       ((DesktopAt
				 (desks.desk[ewin->desktop].x,
				  desks.desk[ewin->desktop].y + ewin->y + dy +
				  ewin->h - 1) != ewin->desktop))))))
	   dofloat = 1;
	if ((desks.dragdir == 3)
	    && (((ewin->y + dy + ewin->h > root.h) || ((ewin->y + dy >= 0)
						       &&
						       ((DesktopAt
							 (desks.desk
							  [ewin->desktop].x,
							  desks.
							  desk[ewin->desktop].y
							  + ewin->y + dy) !=
							 ewin->desktop))))))
	   dofloat = 1;
	if (dofloat)
	   FloatEwinAt(ewin, ewin->x + desks.desk[ewin->desktop].x,
		       ewin->y + desks.desk[ewin->desktop].y);
     }
   EDBUG_RETURN_;
}

void
SetEInfoOnAll()
{
   int                 i, num;
   EWin              **lst;

   EDBUG(5, "SetEInfoOnAll");

   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if ((lst) && (num > 0))
     {
	for (i = 0; i < num; i++)
	   ICCCM_SetEInfo(lst[i]);
	Efree(lst);
     }
   ICCCM_SetMainEInfo();

   EDBUG_RETURN_;
}

EWin               *
GetEwinPointerInClient()
{
   Window              rt, ch;
   int                 dum, px, py, d, i;

   EDBUG(5, "GetEwinPointerInClient");
   d = DESKTOPS_WRAP_NUM(DesktopAt(mode.x, mode.y));
   XQueryPointer(disp, desks.desk[d].win, &rt, &ch, &(mode.x), &(mode.y), &dum,
		 &dum, (unsigned int *)&dum);
   px = mode.x - desks.desk[d].x;
   py = mode.y - desks.desk[d].y;

   for (i = 0; i < desks.desk[d].num; i++)
     {
	int                 x, y, w, h;

	x = desks.desk[d].list[i]->x;
	y = desks.desk[d].list[i]->y;
	w = desks.desk[d].list[i]->w;
	h = desks.desk[d].list[i]->h;
	if ((px >= x) && (py >= y) && (px < (x + w)) && (py < (y + h))
	    && (desks.desk[d].list[i]->visible))
	   EDBUG_RETURN(desks.desk[d].list[i]);
     }
   EDBUG_RETURN(NULL);
}

EWin               *
GetFocusEwin(void)
{
   EDBUG(4, "GetFocusEwin");

   if (mode.cur_menu_mode)
     {
	if (mode.context_ewin)
	  {
	     EDBUG_RETURN(mode.context_ewin);
	  }
	EDBUG_RETURN(mode.realfocuswin);
     }
   if (mode.borderpartpress)
     {
	if (mode.context_ewin)
	  {
	     EDBUG_RETURN(mode.context_ewin);
	  }
	else
	  {
	     EDBUG_RETURN(mode.realfocuswin);
	  }
     }
   if (mode.mode != MODE_NONE)
      EDBUG_RETURN(mode.ewin);

   if (mode.focuswin)
      EDBUG_RETURN(mode.focuswin);

   EDBUG_RETURN(mode.ewin);
}

void
SlideEwinTo(EWin * ewin, int fx, int fy, int tx, int ty, int speed)
{
   int                 k, spd, x, y, min, tmpx, tmpy, tmpw, tmph;
   struct timeval      timev1, timev2;
   int                 dsec, dusec;
   double              tm;
   char                firstlast;
   Window              winid;

   EDBUG(3, "SlideEwinTo");
   winid = ewin->client.win;
   spd = 16;
   min = 2;
   firstlast = 0;
   mode.doingslide = 1;
   ApplySclass(FindItem
	       ("SOUND_WINDOW_SLIDE", 0, LIST_FINDBY_NAME, LIST_TYPE_SCLASS));

   if (mode.slidemode > 0)
      GrabX();

   for (k = 0; k <= 1024; k += spd)
     {
	gettimeofday(&timev1, NULL);
	x = ((fx * (1024 - k)) + (tx * k)) >> 10;
	y = ((fy * (1024 - k)) + (ty * k)) >> 10;
	tmpx = x;
	tmpy = y;
	tmpw = ewin->client.w;
	tmph = ewin->client.h;
	if (mode.slidemode == 0)
	   EMoveWindow(disp, ewin->win, tmpx, tmpy);
	else
	   DrawEwinShape(ewin, mode.slidemode, tmpx, tmpy, tmpw, tmph,
			 firstlast);
	if (firstlast == 0)
	   firstlast = 1;
	XSync(disp, False);
	gettimeofday(&timev2, NULL);
	dsec = timev2.tv_sec - timev1.tv_sec;
	dusec = timev2.tv_usec - timev1.tv_usec;
	if (dusec < 0)
	  {
	     dsec--;
	     dusec += 1000000;
	  }
	tm = (double)dsec + (((double)dusec) / 1000000);
	spd = (int)((double)speed * tm);
	if (spd < min)
	   spd = min;
     }
   DrawEwinShape(ewin, mode.slidemode, x, y, ewin->client.w, ewin->client.h, 2);
   MoveEwin(ewin, tx, ty);
   mode.doingslide = 0;
   if (mode.slidemode > 0)
      UngrabX();
   ApplySclass(FindItem
	       ("SOUND_WINDOW_SLIDE_END", 0, LIST_FINDBY_NAME,
		LIST_TYPE_SCLASS));
   EDBUG_RETURN_;
}

void
SlideEwinsTo(EWin ** ewin, int *fx, int *fy, int *tx, int *ty, int num_wins,
	     int speed)
{
   int                 k, spd, *x = NULL, *y =
      NULL, min, tmpx, tmpy, tmpw, tmph, i;
   struct timeval      timev1, timev2;
   int                 dsec, dusec;
   double              tm;
   char                firstlast;

   EDBUG(3, "SlideEwinsTo");

   if (num_wins)
     {
	x = Emalloc(sizeof(int) * num_wins);
	y = Emalloc(sizeof(int) * num_wins);
     }
   spd = 16;
   min = 2;
   firstlast = 0;
   mode.doingslide = 1;
   ApplySclass(FindItem
	       ("SOUND_WINDOW_SLIDE", 0, LIST_FINDBY_NAME, LIST_TYPE_SCLASS));
   if (mode.slidemode > 0)
      GrabX();
   for (k = 0; k <= 1024; k += spd)
     {
	for (i = 0; i < num_wins; i++)
	  {
	     if (ewin[i])
	       {
		  gettimeofday(&timev1, NULL);
		  x[i] = ((fx[i] * (1024 - k)) + (tx[i] * k)) >> 10;
		  y[i] = ((fy[i] * (1024 - k)) + (ty[i] * k)) >> 10;
		  tmpx = x[i];
		  tmpy = y[i];
		  tmpw = ewin[i]->client.w;
		  tmph = ewin[i]->client.h;
		  if (ewin[i]->menu)
		     EMoveWindow(disp, ewin[i]->win, tmpx, tmpy);
		  else
		     DrawEwinShape(ewin[i], 0, tmpx, tmpy, tmpw, tmph,
				   firstlast);
		  if (firstlast == 0)
		     firstlast = 1;
		  XSync(disp, False);
		  gettimeofday(&timev2, NULL);
		  dsec = timev2.tv_sec - timev1.tv_sec;
		  dusec = timev2.tv_usec - timev1.tv_usec;
		  if (dusec < 0)
		    {
		       dsec--;
		       dusec += 1000000;
		    }
		  tm = (double)dsec + (((double)dusec) / 1000000);
		  spd = (int)((double)speed * tm);
		  if (spd < min)
		     spd = min;
	       }
	  }
     }

   for (i = 0; i < num_wins; i++)
     {
	if (ewin[i])
	  {
	     DrawEwinShape(ewin[i], 0, x[i], y[i], ewin[i]->client.w,
			   ewin[i]->client.h, 2);
	     MoveEwin(ewin[i], tx[i], ty[i]);
	  }
     }

   mode.doingslide = 0;
   if (mode.slidemode > 0)
      UngrabX();
   ApplySclass(FindItem
	       ("SOUND_WINDOW_SLIDE_END", 0, LIST_FINDBY_NAME,
		LIST_TYPE_SCLASS));
   if (x)
      Efree(x);
   if (y)
      Efree(y);
   EDBUG_RETURN_;
}

void
AddToFamily(Window win)
{
   EWin               *ewin, *ewin2;
   EWin              **lst;
   int                 i, k, num, speed, fx, fy, x, y;
   char                doslide, manplace;
   char                pq;
   Window              winid;
   char                cangrab = 0;

   EDBUG(3, "AddToFamily");
   /* find the client window if it's already managed */
   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   /* it's already managed */
   if (ewin)
     {
	/* if its iconified - de-iconify */
	if (ewin->iconified)
	  {
	     RemoveMiniIcon(ewin);
	     ewin->desktop = desks.current;
	     MoveEwinToArea(ewin, desks.desk[desks.current].current_area_x,
			    desks.desk[desks.current].current_area_y);
	     RaiseEwin(ewin);
	     ConformEwinToDesktop(ewin);
	     ShowEwin(ewin);
	     ICCCM_DeIconify(ewin);
	  }
	EDBUG_RETURN_;
     }
   /* grab that server */
   GrabX();
   winid = win;
   speed = mode.slidespeedmap;
   doslide = mode.mapslide;
   manplace = 0;
   /* adopt the new baby */
   ewin = Adopt(win);
   /* if is an afterstep/windowmaker dock app 0- dock it */
   if (mode.dockapp_support && ewin->docked)
     {
	DockIt(ewin);
	EDBUG_RETURN_;
     }
   /* if set for borderless then dont slide it in */
   if ((!ewin->client.mwm_decor_title) && (!ewin->client.mwm_decor_border))
      doslide = 0;
   if (!mode.startup)
     {
	if (ewin->client.start_iconified)
	   ewin->iconified = 1;
     }
   x = 0;
   y = 0;
   DetermineEwinArea(ewin);
   ResizeEwin(ewin, ewin->client.w, ewin->client.h);

   /* tag the parent window if this is a transient */
   if (ewin->client.transient)
     {
	ewin2 =
	   FindItem(NULL, ewin->client.transient_for, LIST_FINDBY_ID,
		    LIST_TYPE_EWIN);
	if (ewin2)
	   ewin2->has_transients++;
     }
   if ((mode.transientsfollowleader) && (ewin->client.transient))
     {
	ewin2 =
	   FindItem(NULL, ewin->client.transient_for, LIST_FINDBY_ID,
		    LIST_TYPE_EWIN);
	if (ewin2)
	  {
	     ewin->desktop = ewin2->desktop;
	     if ((mode.switchfortransientmap)
		 && (ewin->desktop != desks.current) && (!ewin->iconified))
	       {
		  GotoDesktop(ewin->desktop);
		  SetCurrentArea(ewin2->area_x, ewin2->area_y);
	       }
	  }
	else
	  {
	     ewin2 =
		FindItem(NULL, ewin->client.group, LIST_FINDBY_ID,
			 LIST_TYPE_EWIN);
	     if (ewin2)
	       {
		  ewin->desktop = ewin2->desktop;
		  if ((mode.switchfortransientmap)
		      && (ewin->desktop != desks.current) && (!ewin->iconified))
		    {
		       GotoDesktop(ewin->desktop);
		       SetCurrentArea(ewin2->area_x, ewin2->area_y);
		    }
	       }
	     else
	       {
		  lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
		  if ((lst) && (num > 0))
		    {
		       for (i = 0; i < num; i++)
			 {
			    if ((!lst[i]->iconified)
				&& (ewin->client.group == lst[i]->client.group))
			      {
				 ewin->desktop = lst[i]->desktop;
				 if ((mode.switchfortransientmap)
				     && (ewin->desktop != desks.current)
				     && (!ewin->iconified))
				   {
				      GotoDesktop(ewin->desktop);
				      SetCurrentArea(lst[i]->area_x,
						     lst[i]->area_y);
				   }
				 i = num;
			      }
			 }
		       Efree(lst);
		    }
	       }
	  }
     }
   /* if it hasnt been planed on a desktop - assing it the current desktop */
   if (ewin->desktop < 0)
     {
	ewin->desktop = desks.current;
     }
   else
     {
	/* assign it the desktop it asked for (modulo the number of desks) */
	ewin->desktop = DESKTOPS_WRAP_NUM(ewin->desktop);
     }

   if ((!ewin->client.transient) && (mode.manual_placement)
       && (!ewin->client.already_placed) && (!mode.startup) && (!mode.place))
     {
	cangrab = GrabThePointer(root.win);
	if ((cangrab == GrabNotViewable) || (cangrab == AlreadyGrabbed)
	    || (cangrab == GrabFrozen))
	  {
	     XUngrabPointer(disp, CurrentTime);
	     cangrab = 0;
	  }
	else
	  {
	     manplace = 1;
	     cangrab = 1;
	  }
     }

   /* if it hasn't been placed yet.... find a spot for it */
   if ((!ewin->client.already_placed) && (!manplace))
     {

	/* Place the window below the mouse pointer */
	if (mode.manual_placement_mouse_pointer)
	  {
	     int                 rx, ry, wx, wy;
	     unsigned int        mask;
	     Window              junk, root_return;
	     int                 newWinX = 0, newWinY = 0;

	     /* if the loser has manual placement on and the app asks to be on */
	     /*  a desktop, then send E to that desktop so the user can place */
	     /* the window there */
	     if (ewin->desktop >= 0)
		GotoDesktop(ewin->desktop);

	     GrabThePointer(root.win);
	     XQueryPointer(disp, root.win, &root_return, &junk, &rx, &ry, &wx,
			   &wy, &mask);
	     XUngrabPointer(disp, CurrentTime);
	     mode.x = rx;
	     mode.y = ry;
	     ewin->client.already_placed = 1;

	     /* try to center the window on the mouse pointer */
	     newWinX = rx;
	     newWinY = ry;
	     if (ewin->w)
		newWinX -= ewin->w / 2;
	     if (ewin->h)
		newWinY -= ewin->h / 2;

	     /* keep it all on this screen if possible */
	     newWinX = MIN(newWinX, root.w - ewin->w);
	     newWinY = MIN(newWinY, root.h - ewin->h);
	     newWinX = MAX(newWinX, 0);
	     newWinY = MAX(newWinY, 0);

	     /* this works for me... */
	     x = ewin->x = newWinX;
	     y = ewin->y = newWinY;
	  }
	else
	  {
	     ewin->client.already_placed = 1;
	     ArrangeEwinXY(ewin, &x, &y);
	  }			/* (mode.manual_placement_mouse_pointer) */
     }				/* ((!ewin->client.already_placed) && (!manplace)) */
   else
     {
	x = ewin->client.x;
	y = ewin->client.y;
	switch (ewin->client.grav)
	  {
	  case NorthWestGravity:
	     x += (ewin->client.bw * 2);
	     y += (ewin->client.bw * 2);
	     break;
	  case NorthGravity:
	     y += (ewin->client.bw * 2);
	     break;
	  case NorthEastGravity:
	     y += (ewin->client.bw * 2);
	     if (ewin->border)
		x -= ewin->border->border.left + (ewin->client.bw * 2);
	     break;
	  case EastGravity:
	     if (ewin->border)
		x -= ewin->border->border.left + (ewin->client.bw * 2);
	     break;
	  case SouthEastGravity:
	     if (ewin->border)
	       {
		  x -= ewin->border->border.left + (ewin->client.bw * 2);
		  y -= ewin->border->border.top + (ewin->client.bw * 2);
	       }
	     break;
	  case SouthGravity:
	     if (ewin->border)
		y -= ewin->border->border.top + (ewin->client.bw * 2);
	     break;
	  case SouthWestGravity:
	     x += (ewin->client.bw * 2);
	     if (ewin->border)
		y -= ewin->border->border.top + (ewin->client.bw * 2);
	     break;
	  case WestGravity:
	     x += (ewin->client.bw * 2);
	     break;
	  case CenterGravity:
	     break;
	  default:
	     break;
	  }
     }
   /* add it to our list of managed clients */
   AddItem(ewin, "EWIN", ewin->client.win, LIST_TYPE_EWIN);
   DesktopAddEwinToTop(ewin);
   /* if the window asked to be iconified at the start */
   if (ewin->iconified)
     {
	pq = queue_up;
	queue_up = 0;
	DrawEwin(ewin);
	PropagateShapes(ewin->win);
	queue_up = pq;
	MoveEwinToDesktopAt(ewin, ewin->desktop, x, y);
	RaiseEwin(ewin);
	ShowEwin(ewin);
	StackDesktops();
	UngrabX();
	ewin->iconified = 0;
	IconifyEwin(ewin);
	EDBUG_RETURN_;
     }
   /* if we should slide it in and are not currently in the middle of a slide */
   if ((manplace) && (!ewin->client.already_placed))
     {
	int                 rx, ry, wx, wy;
	unsigned int        mask;
	Window              junk, root_return;

	/* if the loser has manual placement on and the app asks to be on */
	/*  a desktop, then send E to that desktop so the user can place */
	/* the window there */
	if (ewin->desktop >= 0)
	   GotoDesktop(ewin->desktop);
	XQueryPointer(disp, root.win, &root_return, &junk, &rx, &ry, &wx, &wy,
		      &mask);
	mode.x = rx;
	mode.y = ry;
	ewin->client.already_placed = 1;
	x = mode.x + 1;
	y = mode.y + 1;
	pq = queue_up;
	queue_up = 0;
	DrawEwin(ewin);
	ICCCM_Configure(ewin);
	PropagateShapes(ewin->win);
	queue_up = pq;
	MoveEwinToDesktop(ewin, ewin->desktop);
	RaiseEwin(ewin);
	MoveEwin(ewin, x, y);
	RaiseEwin(ewin);
	ShowEwin(ewin);
	StackDesktops();
	FocusToEWin(ewin);
	mode.ewin = ewin;
	GrabThePointer(root.win);
	mode.have_place_grab = 1;
	mode.place = 1;
	ICCCM_Configure(ewin);
	UngrabX();
	doMove(NULL);
	EDBUG_RETURN_;
     }
   else if ((doslide) && (!mode.doingslide))
     {
	MoveEwin(ewin, root.w, root.h);
	k = rand() % 4;
	if (k == 0)
	  {
	     fx = (rand() % (root.w)) - ewin->w;
	     fy = -ewin->h;
	  }
	else if (k == 1)
	  {
	     fx = (rand() % (root.w));
	     fy = root.h;
	  }
	else if (k == 2)
	  {
	     fx = -ewin->w;
	     fy = (rand() % (root.h));
	  }
	else
	  {
	     fx = root.w;
	     fy = (rand() % (root.h)) - ewin->h;
	  }
	pq = queue_up;
	queue_up = 0;
	DrawEwin(ewin);
	PropagateShapes(ewin->win);
	queue_up = pq;
	MoveEwinToDesktop(ewin, ewin->desktop);
	RaiseEwin(ewin);
	MoveEwin(ewin, fx, fy);
	ShowEwin(ewin);
	SlideEwinTo(ewin, fx, fy, x, y, speed);
	MoveEwinToDesktopAt(ewin, ewin->desktop, x, y);
	StackDesktops();
     }
   else
     {
	pq = queue_up;
	queue_up = 0;
	DrawEwin(ewin);
	PropagateShapes(ewin->win);
	queue_up = pq;
	MoveEwinToDesktopAt(ewin, ewin->desktop, x, y);
	RaiseEwin(ewin);
	ShowEwin(ewin);
	StackDesktops();
     }
   /* send synthetic configure notifies and configure the window */
   ICCCM_Configure(ewin);

   DetermineEwinArea(ewin);
   if (mode.all_new_windows_get_focus)
     {
	FocusToEWin(ewin);
	if ((ewin->desktop != desks.current) && (!ewin->iconified))
	  {
	     GotoDesktop(ewin->desktop);
	     SetCurrentArea(ewin->area_x, ewin->area_y);
	  }
     }
   else if (mode.new_transients_get_focus)
     {
	if (ewin->client.transient)
	  {
	     FocusToEWin(ewin);
	     if ((ewin->desktop != desks.current) && (!ewin->iconified))
	       {
		  GotoDesktop(ewin->desktop);
		  SetCurrentArea(ewin->area_x, ewin->area_y);
	       }
	  }
     }
   else if (mode.new_transients_get_focus_if_group_focused)
     {
	ewin2 =
	   FindItem(NULL, ewin->client.transient_for, LIST_FINDBY_ID,
		    LIST_TYPE_EWIN);
	if ((ewin2) && (mode.focuswin == ewin2))
	  {
	     FocusToEWin(ewin);
	     if ((ewin->desktop != desks.current) && (!ewin->iconified))
	       {
		  GotoDesktop(ewin->desktop);
		  SetCurrentArea(ewin->area_x, ewin->area_y);
	       }
	  }
     }
   UngrabX();
   EDBUG_RETURN_;
}

EWin               *
AddInternalToFamily(Window win, char noshow, char *bname, int type, void *ptr)
{
   EWin               *ewin;
   int                 x, y;
   char                pq;
   Window              winid;
   Border             *b;

   EDBUG(3, "AddInternalToFamily");
   winid = win;
   b = NULL;

   if (bname)
     {
	b = FindItem(bname, 0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);
	if (!b)
	   b = FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);
     }
   ewin = AdoptInternal(win, b, type, ptr);
   ResizeEwin(ewin, ewin->client.w, ewin->client.h);
   if (ewin->desktop < 0)
      ewin->desktop = desks.current;
   else
      ewin->desktop = DESKTOPS_WRAP_NUM(ewin->desktop);
   x = ewin->client.x - ewin->border->border.left;
   y = ewin->client.y - ewin->border->border.top;
   AddItem(ewin, "EWIN", ewin->client.win, LIST_TYPE_EWIN);
   pq = queue_up;
   queue_up = 0;
   DrawEwin(ewin);
   PropagateShapes(ewin->win);
   queue_up = pq;
   MoveEwinToDesktopAt(ewin, ewin->desktop, x, y);
   RaiseEwin(ewin);
   StackDesktops();
   if (!noshow)
      ShowEwin(ewin);
   ICCCM_Configure(ewin);
   UngrabX();
   EDBUG_RETURN(ewin);
}

void
HonorIclass(char *s, int id)
{
   AwaitIclass        *a;
   EWin               *ewin;

   EDBUG(4, "HonorIclass");

   a = RemoveItem(s, 0, LIST_FINDBY_NAME, LIST_TYPE_AWAIT_ICLASS);
   if (!a)
      EDBUG_RETURN_;

   ewin = FindItem(NULL, a->client_win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
     {
	if (a->ewin_bit < ewin->border->num_winparts)
	  {
	     if ((ewin->border->part[a->ewin_bit].iclass->external)
		 && (!ewin->bits[a->ewin_bit].win) && (id))
	       {
		  ewin->bits[a->ewin_bit].win = id;
		  RealiseEwinWinpart(ewin, a->ewin_bit);
		  EMapWindow(disp, id);
		  ewin->shapedone = 0;
		  if (!ewin->shapedone)
		    {
		       PropagateShapes(ewin->win);
		    }
		  else
		    {
		       if (ewin->border->changes_shape)
			  PropagateShapes(ewin->win);
		    }
		  ewin->shapedone = 1;
	       }
	  }
     }
   if (a->iclass)
      a->iclass->ref_count--;

   Efree(a);

   EDBUG_RETURN_;
}

void
SyncBorderToEwin(EWin * ewin)
{
   Border             *b;

   EDBUG(4, "SyncBorderToEwin");
   b = ewin->border;
   ICCCM_GetShapeInfo(ewin);
   /*   SetEwinBorder(ewin); */
   SetEwinToBorder(ewin, ewin->border);
   if (b != ewin->border)
     {
	ICCCM_MatchSize(ewin);
	MoveResizeEwin(ewin, ewin->client.x, ewin->client.y, ewin->client.w,
		       ewin->client.h);
     }
   EDBUG_RETURN_;
}

void
UpdateBorderInfo(EWin * ewin)
{
   int                 i;

   for (i = 0; i < ewin->border->num_winparts; i++)
     {
	if (ewin->border->part[i].flags == FLAG_TITLE)
	   ChangeEwinWinpartContents(ewin, i);
     }
}

void
RealiseEwinWinpart(EWin * ewin, int i)
{
   EDBUG(4, "RealiseEwinWinpart");

   if ((ewin->bits[i].cx != ewin->bits[i].x)
       || (ewin->bits[i].cy != ewin->bits[i].y)
       || (ewin->bits[i].cw != ewin->bits[i].w)
       || (ewin->bits[i].ch != ewin->bits[i].h))
     {
	if ((ewin->bits[i].w < 0) || (ewin->bits[i].h < 0))
	   EUnmapWindow(disp, ewin->bits[i].win);
	else
	   EMapWindow(disp, ewin->bits[i].win);
	if ((ewin->bits[i].w > 0) && (ewin->bits[i].h > 0))
	   EMoveResizeWindow(disp, ewin->bits[i].win, ewin->bits[i].x,
			     ewin->bits[i].y, ewin->bits[i].w, ewin->bits[i].h);
     }
   EDBUG_RETURN_;
}

int
DrawEwinWinpart(EWin * ewin, int i)
{
   int                 move = 0, resize = 0, state = 0, ret = 0;

   EDBUG(4, "DrawEwinWinpart");
   if ((ewin->bits[i].x != ewin->bits[i].cx)
       || (ewin->bits[i].y != ewin->bits[i].cy))
      move = 1;
   if ((ewin->bits[i].w != ewin->bits[i].cw)
       || (ewin->bits[i].h != ewin->bits[i].ch))
      resize = 1;
   if ((resize) || (ewin->bits[i].expose))
     {
	state = ewin->bits[i].state;
	IclassApply(ewin->border->part[i].iclass, ewin->bits[i].win,
		    ewin->bits[i].w, ewin->bits[i].h, ewin->active,
		    ewin->sticky, state, ewin->bits[i].expose);
	if (ewin->border->part[i].flags == FLAG_TITLE)
	   TclassApply(ewin->border->part[i].iclass, ewin->bits[i].win,
		       ewin->bits[i].w, ewin->bits[i].h, ewin->active,
		       ewin->sticky, state, ewin->bits[i].expose,
		       ewin->border->part[i].tclass, ewin->client.title);
	ret = 1;
     }
   if ((move) || (resize))
     {
	ret = 1;
	ewin->bits[i].cx = ewin->bits[i].x;
	ewin->bits[i].cy = ewin->bits[i].y;
	ewin->bits[i].cw = ewin->bits[i].w;
	ewin->bits[i].ch = ewin->bits[i].h;
     }
   ewin->bits[i].expose = 0;
   EDBUG_RETURN(ret);
}

int
ChangeEwinWinpart(EWin * ewin, int i)
{
   int                 state = 0, ret = 0;

   EDBUG(3, "ChangeEwinWinpart");
   state = ewin->bits[i].state;
   IclassApply(ewin->border->part[i].iclass, ewin->bits[i].win,
	       ewin->bits[i].w, ewin->bits[i].h, ewin->active, ewin->sticky,
	       state, ewin->bits[i].expose);
   if (ewin->border->part[i].flags == FLAG_TITLE)
      TclassApply(ewin->border->part[i].iclass, ewin->bits[i].win,
		  ewin->bits[i].w, ewin->bits[i].h, ewin->active,
		  ewin->sticky, state, ewin->bits[i].expose,
		  ewin->border->part[i].tclass, ewin->client.title);
   if (ewin->bits[i].win)
      ChangeEwinWinpartContents(ewin, i);
   if (!ewin->shapedone)
      PropagateShapes(ewin->win);
   else
     {
	if (ewin->border->changes_shape)
	   PropagateShapes(ewin->win);
     }
   ewin->shapedone = 1;
   ret = 1;
   EDBUG_RETURN(ret);
}

void
DrawEwin(EWin * ewin)
{
   int                 i, state;

   EDBUG(4, "DrawEwin");

   if (!ewin)
      EDBUG_RETURN_;
   for (i = 0; i < ewin->border->num_winparts; i++)
     {
	state = ewin->bits[i].state;
	IclassApply(ewin->border->part[i].iclass, ewin->bits[i].win,
		    ewin->bits[i].w, ewin->bits[i].h, ewin->active,
		    ewin->sticky, state, ewin->bits[i].expose);
	if (ewin->border->part[i].flags == FLAG_TITLE)
	   TclassApply(ewin->border->part[i].iclass, ewin->bits[i].win,
		       ewin->bits[i].w, ewin->bits[i].h, ewin->active,
		       ewin->sticky, state, ewin->bits[i].expose,
		       ewin->border->part[i].tclass, ewin->client.title);
     }
   if (!ewin->shapedone)
      PropagateShapes(ewin->win);
   else
     {
	if (ewin->border->changes_shape)
	   PropagateShapes(ewin->win);
     }
   ewin->shapedone = 1;
   EDBUG_RETURN_;
}

int
ChangeEwinWinpartContents(EWin * ewin, int i)
{
   int                 state = 0, ret = 0;

   EDBUG(3, "ChangeEwinWinpartContents");
   ret = 1;
   switch (ewin->border->part[i].flags)
     {
     case FLAG_TITLE:
	TclassApply(ewin->border->part[i].iclass, ewin->bits[i].win,
		    ewin->bits[i].w, ewin->bits[i].h, ewin->active,
		    ewin->sticky, state, ewin->bits[i].expose,
		    ewin->border->part[i].tclass, ewin->client.title);
	break;
     case FLAG_MINIICON:
	break;
     default:
	break;
     }
   EDBUG_RETURN(ret);
}

void
CalcEwinWinpart(EWin * ewin, int i)
{
   int                 x, y, w, h, ox, oy, max, min;
   int                 topleft, bottomright;

   EDBUG(4, "CalcEwinWinpart");
   topleft = ewin->border->part[i].geom.topleft.originbox;
   bottomright = ewin->border->part[i].geom.bottomright.originbox;
   if (topleft >= 0)
      CalcEwinWinpart(ewin, topleft);
   if (bottomright >= 0)
      CalcEwinWinpart(ewin, bottomright);
   x = y = 0;
   if (topleft == -1)
     {
	x = ((ewin->border->part[i].geom.topleft.x.percent * ewin->w) >> 10) +
	   ewin->border->part[i].geom.topleft.x.absolute;
	y = ((ewin->border->part[i].geom.topleft.y.percent * ewin->h) >> 10) +
	   ewin->border->part[i].geom.topleft.y.absolute;
     }
   else if (topleft >= 0)
     {
	x = ((ewin->border->part[i].geom.topleft.x.percent *
	      ewin->bits[topleft].w) >> 10) +
	   ewin->border->part[i].geom.topleft.x.absolute +
	   ewin->bits[topleft].x;
	y = ((ewin->border->part[i].geom.topleft.y.percent *
	      ewin->bits[topleft].h) >> 10) +
	   ewin->border->part[i].geom.topleft.y.absolute +
	   ewin->bits[topleft].y;
     }
   ox = oy = 0;
   if (bottomright == -1)
     {
	ox = ((ewin->border->
	       part[i].geom.bottomright.x.percent * ewin->w) >> 10) +
	   ewin->border->part[i].geom.bottomright.x.absolute;
	oy = ((ewin->border->
	       part[i].geom.bottomright.y.percent * ewin->h) >> 10) +
	   ewin->border->part[i].geom.bottomright.y.absolute;
     }
   else if (bottomright >= 0)
     {
	ox = ((ewin->border->part[i].geom.bottomright.x.percent *
	       ewin->bits[bottomright].w) >> 10) +
	   ewin->border->part[i].geom.bottomright.x.absolute +
	   ewin->bits[bottomright].x;
	oy = ((ewin->border->part[i].geom.bottomright.y.percent *
	       ewin->bits[bottomright].h) >> 10) +
	   ewin->border->part[i].geom.bottomright.y.absolute +
	   ewin->bits[bottomright].y;
     }
   /*
    * calculate height before width, because we may need it in order to
    * determine the font size. But we might do it the other way around for
    * side borders :-)
    */

   h = (oy - y) + 1;
   max = ewin->border->part[i].geom.height.max;
   min = ewin->border->part[i].geom.height.min;

   /*
    * If the title bar max size is set to zero, then set the title bar size to
    * just a little bit more than the size of the title text.
    */

   if (max == 0 && ewin->border->part[i].flags == FLAG_TITLE)
     {
	int                 dummywidth, wmax, wmin;
	ImageClass         *iclass;
	TextClass          *tclass;

	/*
	 * calculate width before height, because we need it in order to
	 * determine the font size.
	 */

	w = (ox - x) + 1;
	wmax = ewin->border->part[i].geom.width.max;
	wmin = ewin->border->part[i].geom.width.min;
	if (w > wmax)
	  {
	     w = wmax;
	     x = ((x + ox) - w) >> 1;
	  }
	else if (w < wmin)
	  {
	     w = wmin;
	  }
	iclass = ewin->border->part[i].iclass;
	tclass = ewin->border->part[i].tclass;
	TextSize(tclass, ewin->active, ewin->sticky, ewin->bits[i].state,
		 ewin->client.title, &max, &dummywidth,
		 w - (iclass->padding.top + iclass->padding.bottom));
	max += iclass->padding.left + iclass->padding.right;
	if (h > max)
	  {
	     y = y + (((h - max) * tclass->justification) >> 10);
	     h = max;
	  }
	if (h < min)
	  {
	     h = min;
	  }
     }
   else
     {
	if (h > max)
	  {
	     h = max;
	     y = ((y + oy) - h) >> 1;
	  }
	else if (h < min)
	  {
	     h = min;
	  }
	/*
	 * and now the width.
	 */

	w = (ox - x) + 1;
	max = ewin->border->part[i].geom.width.max;
	min = ewin->border->part[i].geom.width.min;

	/*
	 * If the title bar max size is set to zero, then set the title bar
	 * size to just a little bit more than the size of the title text.
	 */

	if (max == 0 && ewin->border->part[i].flags == FLAG_TITLE)
	  {
	     int                 dummyheight;

	     ImageClass         *iclass;
	     TextClass          *tclass;

	     iclass = ewin->border->part[i].iclass;
	     tclass = ewin->border->part[i].tclass;
	     TextSize(tclass, ewin->active, ewin->sticky, ewin->bits[i].state,
		      ewin->client.title, &max, &dummyheight,
		      h - (iclass->padding.top + iclass->padding.bottom));
	     max += iclass->padding.left + iclass->padding.right;

	     if (w > max)
	       {
		  x = x + (((w - max) * tclass->justification) >> 10);
		  w = max;
	       }
	  }
	if (w > max)
	  {
	     w = max;
	     x = ((x + ox) - w) >> 1;
	  }
	else if (w < min)
	  {
	     w = min;
	  }
     }
   if ((ewin->shaded) && (!ewin->border->part[i].keep_for_shade))
     {
	ewin->bits[i].x = -100;
	ewin->bits[i].y = -100;
	ewin->bits[i].w = -1;
	ewin->bits[i].h = -1;
     }
   else
     {
	ewin->bits[i].x = x;
	ewin->bits[i].y = y;
	ewin->bits[i].w = w;
	ewin->bits[i].h = h;
     }
   EDBUG_RETURN_;
}

void
CalcEwinSizes(EWin * ewin)
{
   int                 i;
   char                reshape;

   EDBUG(4, "CalcEwinSizes");

   if (!ewin)
      EDBUG_RETURN_;
   if (!ewin->border)
      EDBUG_RETURN_;

   for (i = 0; i < ewin->border->num_winparts; i++)
      ewin->bits[i].w = -2;
   for (i = 0; i < ewin->border->num_winparts; i++)
      if (ewin->bits[i].w == -2)
	 CalcEwinWinpart(ewin, i);
   for (i = 0; i < ewin->border->num_winparts; i++)
      RealiseEwinWinpart(ewin, i);

   reshape = 0;
   for (i = 0; i < ewin->border->num_winparts; i++)
     {
	reshape |= DrawEwinWinpart(ewin, i);
	ewin->bits[i].no_expose = 1;
     }
   if ((reshape) || (mode.have_place_grab))
     {
	if (mode.have_place_grab)
	  {
	     char                pq;

	     pq = queue_up;
	     queue_up = 0;
	     PropagateShapes(ewin->win);
	     queue_up = pq;
	  }
	else
	   PropagateShapes(ewin->win);
	ewin->shapedone = 1;
     }

   EDBUG_RETURN_;
}

EWin               *
Adopt(Window win)
{
   Border             *b;
   EWin               *ewin;

   EDBUG(4, "Adopt");
   GrabX();
   ewin = CreateEwin();
   ewin->client.win = win;
   ICCCM_AdoptStart(ewin);
   ICCCM_GetTitle(ewin, 0);
   ICCCM_GetHints(ewin, 0);
   ICCCM_GetInfo(ewin, 0);
   ICCCM_GetColormap(ewin);
   ICCCM_GetShapeInfo(ewin);
/* HintsGetWindowHints(ewin); */
   ICCCM_GetGeoms(ewin, 0);
   SessionGetInfo(ewin, 0);
   MatchEwinToSM(ewin);
   MatchEwinToSnapInfo(ewin);
   ICCCM_GetEInfo(ewin);
   HintsGetWindowHints(ewin);
   if (!ewin->border)
      SetEwinBorder(ewin);

   b = ewin->border;
   ewin->border = NULL;
   ewin->border_new = 1;

   SetEwinToBorder(ewin, b);
   ICCCM_MatchSize(ewin);
   ICCCM_Adopt(ewin);
   HintsSetWindowState(ewin);
   UngrabX();
   if (ewin->shaded)
     {
	ewin->shaded = 0;
	InstantShadeEwin(ewin);
     }
   EDBUG_RETURN(ewin);
}

EWin               *
AdoptInternal(Window win, Border * border, int type, void *ptr)
{
   EWin               *ewin;
   Border             *b;

   EDBUG(4, "AdoptInternal");
   GrabX();
   ewin = CreateEwin();
   ewin->client.win = win;
   ewin->internal = 1;
   ewin->type = type;
   switch (type)
     {
#if 0
     case EWIN_TYPE_DIALOG:
#endif
     case EWIN_TYPE_MENU:
	ewin->layer = 99;
	ewin->skiptask = 1;
	ewin->skip_ext_pager = 1;
	break;
     case EWIN_TYPE_ICONBOX:
	ewin->skiptask = 1;
	ewin->skip_ext_pager = 1;
	break;
     case EWIN_TYPE_PAGER:
	ewin->skiptask = 1;
	ewin->skip_ext_pager = 1;
	break;
     }

   ICCCM_AdoptStart(ewin);
   ICCCM_GetTitle(ewin, 0);
   ICCCM_GetInfo(ewin, 0);
   ICCCM_GetShapeInfo(ewin);
   ICCCM_GetGeoms(ewin, 0);

   /* if (type == 1)
    *   MatchEwinToSnapInfoPager(ewin, (Pager *)ptr);
    * else if (type == 2)
    *   MatchEwinToSnapInfoIconbox(ewin, (Iconbox *)ptr);
    * else  */
   if (!border)
     {
	b = MatchEwinByFunction(ewin,
				(void
				 *(*)(EWin *, WindowMatch *))MatchEwinBorder);
	if (b)
	  {
	     ewin->border = b;
	     SetFrameProperty(ewin);
	  }
     }
   MatchEwinToSnapInfo(ewin);
   if (!ewin->border)
     {
	if (border)
	  {
	     ewin->border = border;
	     SetFrameProperty(ewin);
	  }
	else
	   SetEwinBorder(ewin);
     }
   b = ewin->border;
   ewin->border = NULL;
   ewin->border_new = 1;
   SetEwinToBorder(ewin, b);

   ICCCM_MatchSize(ewin);
   ICCCM_Adopt(ewin);
   HintsSetWindowState(ewin);
   UngrabX();
   if (ewin->shaded)
     {
	ewin->shaded = 0;
	ShadeEwin(ewin);
     }
   EDBUG_RETURN(ewin);
   ptr = NULL;
}

EWin               *
CreateEwin()
{
   EWin               *ewin;
   XSetWindowAttributes att;

   EDBUG(5, "CreateEwin");
   ewin = Emalloc(sizeof(EWin));
   ewin->win = 0;
   ewin->x = -1;
   ewin->y = -1;
   ewin->w = -1;
   ewin->h = -1;
   ewin->reqx = -1;
   ewin->reqy = -1;
   ewin->lx = -1;
   ewin->ly = -1;
   ewin->lw = -1;
   ewin->lh = -1;
   ewin->type = 0;
   ewin->internal = 0;
   ewin->toggle = 0;
   ewin->client.win = 0;
   ewin->client.x = -1;
   ewin->client.y = -1;
   ewin->client.w = -1;
   ewin->client.h = -1;
   ewin->client.title = NULL;
   ewin->client.class = NULL;
   ewin->client.name = NULL;
   ewin->client.role = NULL;
   ewin->client.command = NULL;
   ewin->client.machine = NULL;
   ewin->client.icon_name = NULL;
   ewin->client.is_group_leader = 0;
   ewin->client.no_resize_h = 0;
   ewin->client.no_resize_v = 0;
   ewin->client.shaped = 0;
   ewin->client.icon_win = 0;
   ewin->client.icon_pmap = 0;
   ewin->client.icon_mask = 0;
   ewin->client.start_iconified = 0;
   ewin->client.group = 0;
   ewin->client.need_input = 1;
   ewin->client.transient = 0;
   ewin->client.client_leader = 0;
   ewin->client.transient_for = 0;
   ewin->client.already_placed = 0;
   ewin->client.aspect_min = 0.0;
   ewin->client.aspect_max = 65535.0;
   ewin->client.w_inc = 1;
   ewin->client.h_inc = 1;
   ewin->client.grav = 0;
   ewin->client.base_w = 0;
   ewin->client.base_h = 0;
   ewin->client.width.min = 0;
   ewin->client.height.min = 0;
   ewin->client.width.max = 65535;
   ewin->client.height.max = 65535;
   ewin->client.mwm_decor_border = 1;
   ewin->client.mwm_decor_resizeh = 1;
   ewin->client.mwm_decor_title = 1;
   ewin->client.mwm_decor_menu = 1;
   ewin->client.mwm_decor_minimize = 1;
   ewin->client.mwm_decor_maximize = 1;
   ewin->client.mwm_func_resize = 1;
   ewin->client.mwm_func_move = 1;
   ewin->client.mwm_func_minimize = 1;
   ewin->client.mwm_func_maximize = 1;
   ewin->client.mwm_func_close = 1;
   ewin->border = NULL;
   ewin->previous_border = NULL;
   ewin->border_new = 0;
   ewin->bits = NULL;
   ewin->sticky = 0;
   ewin->desktop = desks.current;
   ewin->groups = NULL;
   ewin->num_groups = 0;
   ewin->visible = 0;
   ewin->active = 0;
   ewin->iconified = 0;
   ewin->parent = 0;
   ewin->layer = 4;
   ewin->never_use_area = 0;
   ewin->floating = 0;
   ewin->win = ECreateWindow(root.win, -10, -10, 1, 1, 1);
   ewin->win_container = ECreateWindow(ewin->win, 0, 0, 1, 1, 0);
   ewin->shapedone = 0;
   ewin->docked = 0;
   ewin->shaded = 0;
   ewin->fixedpos = 0;
#if 0				/* ENABLE_GNOME - Not actually used */
   ewin->expanded_x = 0;
   ewin->expanded_y = 0;
   ewin->expanded_width = -1;
   ewin->expanded_height = -1;
#endif
   ewin->ignorearrange = 0;
   ewin->skiptask = 0;
   ewin->skip_ext_pager = 0;
   ewin->skipwinlist = 0;
   ewin->skipfocus = 0;
   ewin->neverfocus = 0;
   ewin->focusclick = 0;
   ewin->ewmh_flags = 0;
   ewin->menu = NULL;
   ewin->dialog = NULL;
   ewin->shownmenu = 0;
   ewin->pager = NULL;
   ewin->ibox = NULL;
   ewin->area_x = -1;
   ewin->area_y = -1;
   ewin->session_id = NULL;
   ewin->has_transients = 0;
   ewin->mini_w = 0;
   ewin->mini_h = 0;
   ewin->mini_pmm.type = 0;
   ewin->mini_pmm.pmap = 0;
   ewin->mini_pmm.mask = 0;
   ewin->snap = NULL;
   ewin->icon_pmm.type = 0;
   ewin->icon_pmm.pmap = 0;
   ewin->icon_pmm.mask = 0;

   att.event_mask =
      StructureNotifyMask | ResizeRedirectMask | ButtonPressMask |
      ButtonReleaseMask | SubstructureNotifyMask | SubstructureRedirectMask;
   att.do_not_propagate_mask = ButtonPressMask | ButtonReleaseMask;
   XChangeWindowAttributes(disp, ewin->win_container,
			   CWEventMask | CWDontPropagate, &att);
   EMapWindow(disp, ewin->win_container);
   if ((mode.clickalways) || (mode.focusmode == FOCUS_CLICK))
      XGrabButton(disp, AnyButton, 0, ewin->win_container, False,
		  ButtonPressMask, GrabModeSync, GrabModeAsync, None, None);
   att.event_mask =
      StructureNotifyMask | PointerMotionMask | ButtonPressMask |
      ButtonReleaseMask | EnterWindowMask | LeaveWindowMask;
   att.do_not_propagate_mask = ButtonPressMask | ButtonReleaseMask;
   XChangeWindowAttributes(disp, ewin->win, CWEventMask | CWDontPropagate,
			   &att);
   GrabButtonGrabs(ewin);
   EDBUG_RETURN(ewin);
}

void
FreeEwin(EWin * ewin)
{
   EWin               *ewin2;
   int                 i, num_groups;

   EDBUG(5, "FreeEwin");
   if ((mode.slideout) && (FindEwinByChildren(mode.slideout->from_win)))
      HideSlideout(mode.slideout, 0);
   if (!ewin)
      EDBUG_RETURN_;

   if (GetZoomEWin() == ewin)
      Zoom(NULL);

   UnmatchEwinToSnapInfo(ewin);

   DesktopRemoveEwin(ewin);

   PagerEwinOutsideAreaUpdate(ewin);
   PagerHideAllHi();

   mode.windowdestroy = 1;
   /* hide any menus this ewin has brought up if they are still up when we */
   /* destroy this ewin */
   if (ewin->shownmenu)
      MenusHideByWindow(ewin->shownmenu);

   if (ewin == mode.focuswin)
     {
#if 0				/* Clean up if OK -- Remove FocusToNone */
	FocusToNone();
#else
	FocusToEWin(NULL);
#endif
     }

   if (ewin->pager)
      PagerDestroy(ewin->pager);
   if (ewin->ibox)
      IconboxDestroy(ewin->ibox);

   /* May be an overkill but cannot hurt... */
   DELETE_EWIN_REFERENCE(ewin, mode.ewin);
   DELETE_EWIN_REFERENCE(ewin, mode.focuswin);
   DELETE_EWIN_REFERENCE(ewin, mode.realfocuswin);
   DELETE_EWIN_REFERENCE(ewin, mode.mouse_over_win);
   DELETE_EWIN_REFERENCE(ewin, mode.context_ewin);
   DELETE_EWIN_REFERENCE(ewin, mode.moveresize_pending_ewin);

   HintsDelWindowHints(ewin);

   if (ewin->client.transient)
     {
	ewin2 =
	   FindItem(NULL, ewin->client.transient_for, LIST_FINDBY_ID,
		    LIST_TYPE_EWIN);
	if (ewin2)
	  {
	     ewin2->has_transients--;
	     if (ewin2->has_transients < 0)
		ewin2->has_transients = 0;
	  }
     }
   if (ewin->border)
      ewin->border->ref_count--;
   if (ewin->client.title)
      Efree(ewin->client.title);
   if (ewin->client.class)
      Efree(ewin->client.class);
   if (ewin->client.name)
      Efree(ewin->client.name);
   if (ewin->client.role)
      Efree(ewin->client.role);
   if (ewin->client.command)
      Efree(ewin->client.command);
   if (ewin->client.machine)
      Efree(ewin->client.machine);
   if (ewin->client.icon_name)
      Efree(ewin->client.icon_name);
   if (ewin->win)
      EDestroyWindow(disp, ewin->win);
   if (ewin->bits)
      Efree(ewin->bits);
   if (ewin->session_id)
      Efree(ewin->session_id);
   FreePmapMask(&ewin->mini_pmm);
   FreePmapMask(&ewin->icon_pmm);
   if (ewin->groups)
     {
	num_groups = ewin->num_groups;
	for (i = 0; i < num_groups; i++)
	   RemoveEwinFromGroup(ewin, ewin->groups[0]);
     }
   Efree(ewin);

   EDBUG_RETURN_;
}

void
SetEwinBorder(EWin * ewin)
{
   Border             *b;

   EDBUG(4, "SetEwinBorder");
   b = NULL;
   ICCCM_GetShapeInfo(ewin);

   if ((!ewin->client.mwm_decor_title) && (!ewin->client.mwm_decor_border))
      b = (Border *) FindItem("BORDERLESS", 0, LIST_FINDBY_NAME,
			      LIST_TYPE_BORDER);
   else
      b = MatchEwinByFunction(ewin,
			      (void
			       *(*)(EWin *, WindowMatch *))(MatchEwinBorder));
   if (mode.dockapp_support && ewin->docked)
      b = (Border *) FindItem("BORDERLESS", 0, LIST_FINDBY_NAME,
			      LIST_TYPE_BORDER);
   if (!b)
      b = (Border *) FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);
   ewin->border = b;
   SetFrameProperty(ewin);
   EDBUG_RETURN_;
}

void
SetEwinToBorder(EWin * ewin, Border * b)
{
   int                 i;
   int                 px = -1, py = -1;
   char                s[1024];

   AwaitIclass        *await;

   EDBUG(4, "SetEwinToBorder");

   if (!b)
      b = FindItem("__FALLBACK_BORDER", 0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);

   if ((!b) || (ewin->border == b) || (!ewin->border_new))
      EDBUG_RETURN_;

   if (ewin->border)
     {
	px = ewin->border->border.left;
	py = ewin->border->border.top;
	for (i = 0; i < ewin->border->num_winparts; i++)
	  {
	     if (ewin->bits[i].win)
		EDestroyWindow(disp, ewin->bits[i].win);
	  }
	if (ewin->bits)
	   Efree(ewin->bits);
	ewin->bits = NULL;
	ewin->border->ref_count--;
     }
   ewin->border_new = 0;
   ewin->border = b;
   SetFrameProperty(ewin);
   b->ref_count++;

   if (b->num_winparts > 0)
      ewin->bits = Emalloc(sizeof(EWinBit) * b->num_winparts);
   for (i = 0; i < b->num_winparts; i++)
     {
	if (b->part[i].iclass->external)
	  {
	     ewin->bits[i].win = 0;
	     Esnprintf(s, sizeof(s), "request imageclass %s",
		       b->part[i].iclass->name);
	     CommsBroadcast(s);
	     await = Emalloc(sizeof(AwaitIclass));
	     await->client_win = ewin->client.win;
	     await->ewin_bit = i;

	     await->iclass = b->part[i].iclass;
	     if (await->iclass)
		await->iclass->ref_count++;

	     AddItem(await, b->part[i].iclass->name, 0, LIST_TYPE_AWAIT_ICLASS);
	  }
	else
	  {
	     ewin->bits[i].win = ECreateWindow(ewin->win, -10, -10, 1, 1, 0);
	     ApplyECursor(ewin->bits[i].win, b->part[i].ec);
	     EMapWindow(disp, ewin->bits[i].win);
	     /*
	      * KeyPressMask KeyReleaseMask ButtonPressMask 
	      * ButtonReleaseMask
	      * EnterWindowMask LeaveWindowMask PointerMotionMask 
	      * PointerMotionHintMask Button1MotionMask 
	      * Button2MotionMask
	      * Button3MotionMask Button4MotionMask Button5MotionMask
	      * ButtonMotionMask KeymapStateMask ExposureMask 
	      * VisibilityChangeMask StructureNotifyMask 
	      * ResizeRedirectMask 
	      * SubstructureNotifyMask SubstructureRedirectMask 
	      * FocusChangeMask PropertyChangeMas ColormapChangeMask 
	      * OwnerGrabButtonMask
	      */
	     if (b->part[i].flags & FLAG_TITLE)
	       {
		  XSelectInput(disp, ewin->bits[i].win,
			       ExposureMask | KeyPressMask | KeyReleaseMask |
			       ButtonPressMask | ButtonReleaseMask |
			       EnterWindowMask | LeaveWindowMask |
			       PointerMotionMask);
	       }
	     else
	       {
		  XSelectInput(disp, ewin->bits[i].win,
			       KeyPressMask | KeyReleaseMask | ButtonPressMask |
			       ButtonReleaseMask | EnterWindowMask |
			       LeaveWindowMask | PointerMotionMask);
	       }
	     ewin->bits[i].x = -10;
	     ewin->bits[i].y = -10;
	     ewin->bits[i].w = -10;
	     ewin->bits[i].h = -10;
	     ewin->bits[i].cx = -99;
	     ewin->bits[i].cy = -99;
	     ewin->bits[i].cw = -99;
	     ewin->bits[i].ch = -99;
	     ewin->bits[i].state = 0;
	     ewin->bits[i].expose = 0;
	     ewin->bits[i].no_expose = 0;
	     ewin->bits[i].left = 0;
	  }
     }

   {
      Window             *wl;
      int                 j = 0;

      wl = Emalloc((b->num_winparts + 1) * sizeof(Window));
      for (i = b->num_winparts - 1; i >= 0; i--)
	{
	   if (b->part[i].ontop)
	      wl[j++] = ewin->bits[i].win;
	}
      wl[j++] = ewin->win_container;
      for (i = b->num_winparts - 1; i >= 0; i--)
	{
	   if (!b->part[i].ontop)
	      wl[j++] = ewin->bits[i].win;
	}
      XRestackWindows(disp, wl, j);
      Efree(wl);
   }

   if (!ewin->shaded)
      EMoveWindow(disp, ewin->win_container, b->border.left, b->border.top);
   if ((px >= 0) && (py >= 0))
     {
	MoveEwin(ewin, ewin->x + (px - ewin->border->border.left),
		 ewin->y + (py - ewin->border->border.top));
     }
   ICCCM_Configure(ewin);
   CalcEwinSizes(ewin);
   PropagateShapes(ewin->win);

   EDBUG_RETURN_;
}

void
ResizeEwin(EWin * ewin, int w, int h)
{
   char                resize = 0;

   EDBUG(3, "ResizeEwin");
   if ((ewin->client.w != w) || (ewin->client.h != h))
      resize = 1;
   ewin->client.w = w;
   ewin->client.h = h;
   ICCCM_MatchSize(ewin);
   if (!ewin->shaded)
     {
	ewin->w =
	   ewin->client.w + ewin->border->border.left +
	   ewin->border->border.right;
	ewin->h =
	   ewin->client.h + ewin->border->border.top +
	   ewin->border->border.bottom;
     }
   EResizeWindow(disp, ewin->win, ewin->w, ewin->h);
   ICCCM_Configure(ewin);
   CalcEwinSizes(ewin);
   if ((mode.mode == MODE_NONE) && (resize))
     {
	PagerEwinOutsideAreaUpdate(ewin);
	ForceUpdatePagersForDesktop(ewin->desktop);
     }
   if (ewin->pager)
      PagerResize(ewin->pager, ewin->client.w, ewin->client.h);
   if (ewin->ibox)
      IconboxResize(ewin->ibox, ewin->client.w, ewin->client.h);
   EDBUG_RETURN_;
}

void
DetermineEwinArea(EWin * ewin)
{
   int                 pax, pay;

   EDBUG(4, "DetermineEwinArea");

   pax = ewin->area_x;
   pay = ewin->area_y;
   ewin->area_x =
      (ewin->x + (ewin->w / 2) +
       (desks.desk[ewin->desktop].current_area_x * root.w)) / root.w;
   ewin->area_y =
      (ewin->y + (ewin->h / 2) +
       (desks.desk[ewin->desktop].current_area_y * root.h)) / root.h;
   if ((pax != ewin->area_x) || (pay != ewin->area_y))
     {
	HintsSetWindowArea(ewin);
     }
   EDBUG_RETURN_;
}

void
MoveEwin(EWin * ewin, int x, int y)
{
   int                 dx, dy;
   char                move = 0;
   static int          call_depth = 0;

   EDBUG(3, "MoveEwin");
   call_depth++;
   if (call_depth > 256)
     {
	call_depth--;
	EDBUG_RETURN_;
     }
   dx = x - ewin->x;
   dy = y - ewin->y;
   if ((dx != 0) || (dy != 0))
      move = 1;
   ewin->x = x;
   ewin->y = y;
   ewin->reqx = x;
   ewin->reqy = y;
   EMoveWindow(disp, ewin->win, ewin->x, ewin->y);
   if (mode.mode != MODE_MOVE)
      ICCCM_Configure(ewin);
   DetermineEwinArea(ewin);
   if (ewin->has_transients)
     {
	EWin              **lst;
	int                 i, num;

	lst = ListTransientsFor(ewin->client.win, &num);
	if (lst)
	  {
	     for (i = 0; i < num; i++)
	       {
		  if (!((mode.flipp) && (lst[i]->floating))
		      && (lst[i]->client.mwm_decor_border
			  || lst[i]->client.mwm_decor_resizeh
			  || lst[i]->client.mwm_decor_title
			  || lst[i]->client.mwm_decor_menu
			  || lst[i]->client.mwm_decor_minimize
			  || lst[i]->client.mwm_decor_maximize))
		     MoveEwin(lst[i], lst[i]->x + dx, lst[i]->y + dy);
	       }
	     Efree(lst);
	  }
     }
   if ((mode.mode == MODE_NONE) && (move))
     {
	PagerEwinOutsideAreaUpdate(ewin);
	ForceUpdatePagersForDesktop(ewin->desktop);
     }
   call_depth--;
   EDBUG_RETURN_;
}

void
MoveResizeEwin(EWin * ewin, int x, int y, int w, int h)
{
   int                 dx, dy;
   char                change = 0;
   static int          call_depth = 0;

   EDBUG(3, "MoveResizeEwin");
   call_depth++;
   if (call_depth > 256)
     {
	call_depth--;
	EDBUG_RETURN_;
     }
   dx = x - ewin->x;
   dy = y - ewin->y;
   if ((dx != 0) || (dy != 0) || (w != ewin->w) || (h != ewin->h))
      change = 1;
   ewin->x = x;
   ewin->y = y;
   ewin->reqx = x;
   ewin->reqy = y;
   ewin->client.w = w;
   ewin->client.h = h;
   ICCCM_MatchSize(ewin);
   if (!ewin->shaded)
     {
	ewin->w =
	   ewin->client.w + ewin->border->border.left +
	   ewin->border->border.right;
	ewin->h =
	   ewin->client.h + ewin->border->border.top +
	   ewin->border->border.bottom;
     }
   EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w, ewin->h);
   DetermineEwinArea(ewin);
   if ((mode.mode != MODE_MOVE) || (mode.have_place_grab))
      ICCCM_Configure(ewin);
   CalcEwinSizes(ewin);
   if (ewin->has_transients)
     {
	EWin              **lst;
	int                 i, num;

	lst = ListTransientsFor(ewin->client.win, &num);
	if (lst)
	  {
	     for (i = 0; i < num; i++)
	       {
		  if (!((mode.flipp) && (lst[i]->floating))
		      && (lst[i]->client.mwm_decor_border
			  || lst[i]->client.mwm_decor_resizeh
			  || lst[i]->client.mwm_decor_title
			  || lst[i]->client.mwm_decor_menu
			  || lst[i]->client.mwm_decor_minimize
			  || lst[i]->client.mwm_decor_maximize))
		     MoveEwin(lst[i], lst[i]->x + dx, lst[i]->y + dy);
	       }
	     Efree(lst);
	  }
     }
   if ((mode.mode == MODE_NONE) && (change))
     {
	PagerEwinOutsideAreaUpdate(ewin);
	ForceUpdatePagersForDesktop(ewin->desktop);
     }
   if (ewin->pager)
      PagerResize(ewin->pager, ewin->client.w, ewin->client.h);
   if (ewin->ibox)
      IconboxResize(ewin->ibox, ewin->client.w, ewin->client.h);
   call_depth--;
   EDBUG_RETURN_;
}

#if 0				/* Unused */
void
FloatEwin(EWin * ewin)
{
   static int          call_depth = 0;

   EDBUG(3, "FloatEwin");
   call_depth++;
   if (call_depth > 256)
      EDBUG_RETURN_;
   ewin->floating = 1;
   DesktopRemoveEwin(ewin);
   ewin->desktop = 0;
   ConformEwinToDesktop(ewin);
   RaiseEwin(ewin);
   if (ewin->has_transients)
     {
	EWin              **lst;
	int                 i, num;

	lst = ListTransientsFor(ewin->client.win, &num);
	if (lst)
	  {
	     for (i = 0; i < num; i++)
		FloatEwin(lst[i]);
	     Efree(lst);
	  }
     }
   call_depth--;
   EDBUG_RETURN_;
}
#endif

void
FloatEwinAt(EWin * ewin, int x, int y)
{
   int                 dx, dy;
   static int          call_depth = 0;

   EDBUG(3, "FloatEwinAt");
   call_depth++;
   if (call_depth > 256)
      EDBUG_RETURN_;
   if (ewin->floating)
      ewin->floating = 2;
   else
      ewin->floating = 1;
   dx = x - ewin->x;
   dy = y - ewin->y;
   ewin->x = x;
   ewin->y = y;
   ewin->reqx = x;
   ewin->reqy = y;
   ConformEwinToDesktop(ewin);
   if (ewin->has_transients)
     {
	EWin              **lst;
	int                 i, num;

	lst = ListTransientsFor(ewin->client.win, &num);
	if (lst)
	  {
	     for (i = 0; i < num; i++)
		FloatEwinAt(lst[i], lst[i]->x + dx, lst[i]->y + dy);
	     Efree(lst);
	  }
     }
   call_depth--;
   EDBUG_RETURN_;
}

void
RestackEwin(EWin * ewin)
{
   EDBUG(3, "RestackEwin");

   if (ewin->floating)
     {
	XRaiseWindow(disp, ewin->win);
	EDBUG_RETURN_;
     }

   StackDesktop(ewin->desktop);

   if (mode.mode == MODE_NONE)
     {
	PagerEwinOutsideAreaUpdate(ewin);
	ForceUpdatePagersForDesktop(ewin->desktop);
     }
   EDBUG_RETURN_;
}

void
RaiseEwin(EWin * ewin)
{
   static int          call_depth = 0;

   EDBUG(3, "RaiseEwin");
   call_depth++;
   if (call_depth > 256)
      EDBUG_RETURN_;

   if (ewin->win)
     {
	if (ewin->floating)
	   XRaiseWindow(disp, ewin->win);
	else
	  {
	     DesktopAddEwinToTop(ewin);
	     if (ewin->has_transients)
	       {
		  EWin              **lst;
		  int                 i, num;

		  lst = ListTransientsFor(ewin->client.win, &num);
		  if (lst)
		    {
		       for (i = 0; i < num; i++)
			  DesktopAddEwinToTop(lst[i]);
		       Efree(lst);
		    }
	       }
	     RestackEwin(ewin);
	  }
     }
   call_depth--;
   EDBUG_RETURN_;
}

void
LowerEwin(EWin * ewin)
{
   static int          call_depth = 0;

   EDBUG(3, "LowerEwin");
   call_depth++;
   if (call_depth > 256)
      EDBUG_RETURN_;
   if ((ewin->win) && (!ewin->floating))
     {
	if (ewin->has_transients)
	  {
	     EWin              **lst;
	     int                 i, num;

	     lst = ListTransientsFor(ewin->client.win, &num);
	     if (lst)
	       {
		  for (i = 0; i < num; i++)
		     DesktopAddEwinToBottom(lst[i]);
		  Efree(lst);
	       }
	  }
	DesktopAddEwinToBottom(ewin);
	RestackEwin(ewin);
     }
   call_depth--;
   EDBUG_RETURN_;
}

void
ShowEwin(EWin * ewin)
{
   EDBUG(3, "ShowEwin");

   if (ewin->visible)
      EDBUG_RETURN_;
   if (ewin->client.win)
     {
	if (ewin->shaded)
	   EMoveResizeWindow(disp, ewin->win_container, -30, -30, 1, 1);
	EMapWindow(disp, ewin->client.win);
     }
   if (ewin->win)
     {
	EMapWindow(disp, ewin->win);
     }
   ewin->visible = 1;
   SetEwinToCurrentArea(ewin);
   if (mode.mode == MODE_NONE)
     {
	PagerEwinOutsideAreaUpdate(ewin);
	ForceUpdatePagersForDesktop(ewin->desktop);
     }
   EDBUG_RETURN_;
}

void
HideEwin(EWin * ewin)
{
   EDBUG(3, "HideEwin");
   if (!ewin->visible)
      EDBUG_RETURN_;
   if (GetZoomEWin() == ewin)
      Zoom(NULL);
   if (ewin->win)
      EUnmapWindow(disp, ewin->win);
   ewin->visible = 0;
   if (mode.mode == MODE_NONE)
     {
	PagerEwinOutsideAreaUpdate(ewin);
	ForceUpdatePagersForDesktop(ewin->desktop);
     }
   EDBUG_RETURN_;
}

void
FreeBorder(Border * b)
{
   int                 i;

   EDBUG(3, "FreeBorder");

   if (!b)
      EDBUG_RETURN_;

   if (b->ref_count > 0)
     {
	char                stuff[255];

	Esnprintf(stuff, sizeof(stuff), _("%u references remain\n"),
		  b->ref_count);
	DIALOG_OK(_("Border Error!"), stuff);
	EDBUG_RETURN_;
     }
   while (RemoveItemByPtr(b, LIST_TYPE_BORDER));

   for (i = 0; i < b->num_winparts; i++)
     {
	if (b->part[i].iclass)
	   b->part[i].iclass->ref_count--;
	if (b->part[i].tclass)
	   b->part[i].tclass->ref_count--;
	if (b->part[i].aclass)
	   b->part[i].aclass->ref_count--;
	if (b->part[i].ec)
	   b->part[i].ec->ref_count--;
     }

   if (b->num_winparts > 0)
      Efree(b->part);

   if (b->name)
      Efree(b->name);
   if (b->group_border_name)
      Efree(b->group_border_name);

   EDBUG_RETURN_;
}

Border             *
CreateBorder(char *name)
{
   Border             *b;

   EDBUG(5, "CreateBorder");

   b = Emalloc(sizeof(Border));
   if (!b)
      EDBUG_RETURN(NULL);

   b->name = duplicate(name);
   b->group_border_name = NULL;
   b->border.left = 0;
   b->border.right = 0;
   b->border.top = 0;
   b->border.bottom = 0;
   b->num_winparts = 0;
   b->part = NULL;
   b->changes_shape = 0;
   b->shadedir = 2;
   b->ref_count = 0;

   EDBUG_RETURN(b);
}

void
AddBorderPart(Border * b, ImageClass * iclass, ActionClass * aclass,
	      TextClass * tclass, ECursor * ec, char ontop, int flags,
	      char isregion, int wmin, int wmax, int hmin, int hmax,
	      int torigin, int txp, int txa, int typ, int tya, int borigin,
	      int bxp, int bxa, int byp, int bya, char keep_for_shade)
{
   int                 n;

   EDBUG(6, "AddBorderPart");

   b->num_winparts++;
   n = b->num_winparts;

   isregion = 0;
   if (!b->part)
     {
	b->part = Emalloc(n * sizeof(WinPart));
     }
   else
     {
	b->part = Erealloc(b->part, n * sizeof(WinPart));
     }

   if (!iclass)
      iclass =
	 FindItem("__FALLBACK_ICLASS", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);

   b->part[n - 1].iclass = iclass;
   if (iclass)
      iclass->ref_count++;

   b->part[n - 1].aclass = aclass;
   if (aclass)
      aclass->ref_count++;

   b->part[n - 1].tclass = tclass;
   if (tclass)
      tclass->ref_count++;

   b->part[n - 1].ec = ec;
   if (ec)
      ec->ref_count++;

   b->part[n - 1].ontop = ontop;
   b->part[n - 1].flags = flags;
   b->part[n - 1].keep_for_shade = keep_for_shade;
   b->part[n - 1].geom.width.min = wmin;
   b->part[n - 1].geom.width.max = wmax;
   b->part[n - 1].geom.height.min = hmin;
   b->part[n - 1].geom.height.max = hmax;
   b->part[n - 1].geom.topleft.originbox = torigin;
   b->part[n - 1].geom.topleft.x.percent = txp;
   b->part[n - 1].geom.topleft.x.absolute = txa;
   b->part[n - 1].geom.topleft.y.percent = typ;
   b->part[n - 1].geom.topleft.y.absolute = tya;
   b->part[n - 1].geom.bottomright.originbox = borigin;
   b->part[n - 1].geom.bottomright.x.percent = bxp;
   b->part[n - 1].geom.bottomright.x.absolute = bxa;
   b->part[n - 1].geom.bottomright.y.percent = byp;
   b->part[n - 1].geom.bottomright.y.absolute = bya;

   EDBUG_RETURN_;
}

#define DO_CALC \
for (i = 0; i < ewin->border->num_winparts; i++) \
ewin->bits[i].w = -2; \
for (i = 0; i < ewin->border->num_winparts; i++) \
if (ewin->bits[i].w == -2) \
CalcEwinWinpart(ewin, i);

#define FIND_MAX \
*mw = 0; \
*mh = 0; \
for (i = 0; i < ewin->border->num_winparts; i++) \
{ \
	if (ewin->border->part[i].keep_for_shade) \
	{ \
		if (*mw < (ewin->bits[i].w + ewin->bits[i].x)) \
			*mw = ewin->bits[i].w + ewin->bits[i].x; \
				if (*mh < (ewin->bits[i].h + ewin->bits[i].y)) \
					*mh = ewin->bits[i].h + ewin->bits[i].y; \
	} \
}

void
MinShadeSize(EWin * ewin, int *mw, int *mh)
{
   int                 i, p;
   int                 leftborderwidth, rightborderwidth;
   int                 topborderwidth, bottomborderwidth;

   *mw = 32;
   *mh = 32;
   if (!ewin)
      EDBUG_RETURN_;
   switch (ewin->border->shadedir)
     {
     case 0:
     case 1:
	p = ewin->w;
	/* get the correct width, based on the borderparts that */
	/*are remaining visible */
	leftborderwidth = rightborderwidth = 0;
	for (i = 0; i < ewin->border->num_winparts; i++)
	  {
	     if (ewin->border->part[i].keep_for_shade)
	       {
		  if (ewin->border->border.left - ewin->bits[i].x >
		      leftborderwidth)
		     leftborderwidth =
			ewin->border->border.left - ewin->bits[i].x;
		  if ((ewin->bits[i].x + ewin->bits[i].w) -
		      (ewin->w - ewin->border->border.right) > rightborderwidth)
		     rightborderwidth =
			(ewin->bits[i].x + ewin->bits[i].w) - (ewin->w -
							       ewin->border->
							       border.right);
	       }
	  }
	ewin->w = rightborderwidth + leftborderwidth;
	DO_CALC;
	FIND_MAX;
	ewin->w = p;
	DO_CALC;
	break;
     case 2:
     case 3:
	p = ewin->h;
	topborderwidth = bottomborderwidth = 0;
	for (i = 0; i < ewin->border->num_winparts; i++)
	  {
	     if (ewin->border->part[i].keep_for_shade)
	       {
		  if (ewin->border->border.top - ewin->bits[i].y >
		      topborderwidth)
		     topborderwidth =
			ewin->border->border.top - ewin->bits[i].y;
		  if ((ewin->bits[i].y + ewin->bits[i].h) -
		      (ewin->h - ewin->border->border.bottom) >
		      bottomborderwidth)
		     bottomborderwidth =
			(ewin->bits[i].y + ewin->bits[i].h) - (ewin->h -
							       ewin->border->
							       border.bottom);
	       }
	  }
	ewin->h = bottomborderwidth + topborderwidth;
	DO_CALC;
	FIND_MAX;
	ewin->h = p;
	DO_CALC;
	break;
     default:
	break;
     }
}

void
InstantShadeEwin(EWin * ewin)
{
   XSetWindowAttributes att;
   int                 b, d;
   char                pq;

   EDBUG(4, "InstantShadeEwin");

   if ((ewin->border->border.left == 0) && (ewin->border->border.right == 0)
       && (ewin->border->border.top == 0) && (ewin->border->border.bottom == 0))
      EDBUG_RETURN_;
   if (GetZoomEWin() == ewin)
      EDBUG_RETURN_;
   if (ewin->shaded)
      EDBUG_RETURN_;
   pq = queue_up;
   queue_up = 0;
   switch (ewin->border->shadedir)
     {
     case 0:
	att.win_gravity = EastGravity;
	XChangeWindowAttributes(disp, ewin->client.win, CWWinGravity, &att);
	MinShadeSize(ewin, &b, &d);
	ewin->shaded = 2;
	ewin->w = b;
	EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w, ewin->h);
	EMoveResizeWindow(disp, ewin->win_container, -30, -30, 1, 1);
	CalcEwinSizes(ewin);
	XSync(disp, False);
	break;
     case 1:
	att.win_gravity = WestGravity;
	XChangeWindowAttributes(disp, ewin->client.win, CWWinGravity, &att);
	MinShadeSize(ewin, &b, &d);
	d = ewin->x + ewin->w - b;
	ewin->shaded = 2;
	ewin->w = b;
	ewin->x = d;
	ewin->reqx = d;
	EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w, ewin->h);
	EMoveResizeWindow(disp, ewin->win_container, -30, -30, 1, 1);
	CalcEwinSizes(ewin);
	XSync(disp, False);
	break;
     case 2:
	att.win_gravity = SouthGravity;
	XChangeWindowAttributes(disp, ewin->client.win, CWWinGravity, &att);
	MinShadeSize(ewin, &b, &d);
	b = d;
	ewin->shaded = 2;
	ewin->h = b;
	EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w, ewin->h);
	EMoveResizeWindow(disp, ewin->win_container, -30, -30, 1, 1);
	CalcEwinSizes(ewin);
	XSync(disp, False);
	break;
     case 3:
	att.win_gravity = SouthGravity;
	XChangeWindowAttributes(disp, ewin->client.win, CWWinGravity, &att);
	MinShadeSize(ewin, &b, &d);
	b = d;
	d = ewin->y + ewin->h - b;
	ewin->shaded = 2;
	ewin->h = b;
	ewin->y = d;
	ewin->reqy = d;
	EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w, ewin->h);
	EMoveResizeWindow(disp, ewin->win_container, -30, -30, 1, 1);
	CalcEwinSizes(ewin);
	XSync(disp, False);
	break;
     default:
	break;
     }
   PropagateShapes(ewin->win);
   queue_up = pq;
   HintsSetWindowState(ewin);
   if (mode.mode == MODE_NONE)
     {
	PagerEwinOutsideAreaUpdate(ewin);
	ForceUpdatePagersForDesktop(ewin->desktop);
     }
   EDBUG_RETURN_;
}

void
InstantUnShadeEwin(EWin * ewin)
{
   XSetWindowAttributes att;
   int                 b, d;
   char                pq;

   EDBUG(4, "InstantUnShadeEwin");
   if (GetZoomEWin() == ewin)
      EDBUG_RETURN_;
   if (!ewin->shaded)
      EDBUG_RETURN_;
   pq = queue_up;
   queue_up = 0;
   switch (ewin->border->shadedir)
     {
     case 0:
	att.win_gravity = EastGravity;
	XChangeWindowAttributes(disp, ewin->client.win, CWWinGravity, &att);
	b = ewin->client.w + ewin->border->border.left +
	   ewin->border->border.right;
	ewin->shaded = 0;
	ewin->w = b;
	MoveResizeEwin(ewin, ewin->x, ewin->y, ewin->client.w, ewin->client.h);
	XSync(disp, False);
	break;
     case 1:
	att.win_gravity = WestGravity;
	XChangeWindowAttributes(disp, ewin->client.win, CWWinGravity, &att);
	b = ewin->client.w + ewin->border->border.left +
	   ewin->border->border.right;
	d = ewin->x + ewin->w - (ewin->border->border.right + ewin->client.w +
				 ewin->border->border.left);
	ewin->shaded = 0;
	ewin->w = b;
	ewin->x = d;
	ewin->reqx = d;
	MoveResizeEwin(ewin, ewin->x, ewin->y, ewin->client.w, ewin->client.h);
	XSync(disp, False);
	break;
     case 2:
	att.win_gravity = SouthGravity;
	XChangeWindowAttributes(disp, ewin->client.win, CWWinGravity, &att);
	b = ewin->client.h + ewin->border->border.top +
	   ewin->border->border.bottom;
	ewin->shaded = 0;
	ewin->h = b;
	MoveResizeEwin(ewin, ewin->x, ewin->y, ewin->client.w, ewin->client.h);
	XSync(disp, False);
	break;
     case 3:
	att.win_gravity = SouthGravity;
	XChangeWindowAttributes(disp, ewin->client.win, CWWinGravity, &att);
	b = ewin->client.h + ewin->border->border.top +
	   ewin->border->border.bottom;
	d = ewin->y + ewin->h - (ewin->border->border.bottom +
				 ewin->client.h + ewin->border->border.top);
	ewin->shaded = 0;
	ewin->h = b;
	ewin->y = d;
	ewin->reqy = d;
	MoveResizeEwin(ewin, ewin->x, ewin->y, ewin->client.w, ewin->client.h);
	XSync(disp, False);
	break;
     default:
	break;
     }
   PropagateShapes(ewin->win);
   queue_up = pq;
   HintsSetWindowState(ewin);
   if (mode.mode == MODE_NONE)
     {
	PagerEwinOutsideAreaUpdate(ewin);
	ForceUpdatePagersForDesktop(ewin->desktop);
     }
   EDBUG_RETURN_;
}

void
ShadeEwin(EWin * ewin)
{
   XSetWindowAttributes att;
   int                 i, j, speed, a, b, c, d, ww, hh;
   int                 k, spd, min;
   struct timeval      timev1, timev2;
   int                 dsec, dusec;
   double              tm;
   char                pq;

   EDBUG(4, "ShadeEwin");

   if ((ewin->border->border.left == 0) && (ewin->border->border.right == 0)
       && (ewin->border->border.top == 0) && (ewin->border->border.bottom == 0))
      EDBUG_RETURN_;
   if (GetZoomEWin() == ewin)
      EDBUG_RETURN_;
   if (ewin->shaded)
      EDBUG_RETURN_;
   if ((ewin->border) && (!strcmp(ewin->border->name, "BORDERLESS")))
      EDBUG_RETURN_;
   pq = queue_up;
   queue_up = 0;
   speed = mode.shadespeed;
   spd = 32;
   min = 2;
   GrabX();
   switch (ewin->border->shadedir)
     {
     case 0:
	att.win_gravity = EastGravity;
	XChangeWindowAttributes(disp, ewin->client.win, CWWinGravity, &att);
	MinShadeSize(ewin, &b, &d);
	a = ewin->w;
	if ((mode.animate_shading) || (ewin->menu))
	   for (k = 0; k <= 1024; k += spd)
	     {
		gettimeofday(&timev1, NULL);
		i = ((a * (1024 - k)) + (b * k)) >> 10;
		ewin->w = i;
		if (ewin->w < 1)
		   ewin->w = 1;
		ww = ewin->w - ewin->border->border.left -
		   ewin->border->border.right;
		if (ww < 1)
		   ww = 1;
		hh = ewin->client.h;
		EMoveResizeWindow(disp, ewin->win_container,
				  ewin->border->border.left,
				  ewin->border->border.top, ww, hh);
		EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w,
				  ewin->h);
		CalcEwinSizes(ewin);
		if (ewin->client.shaped)
		   EShapeCombineShape(disp, ewin->win_container,
				      ShapeBounding, -(ewin->client.w - ww),
				      0, ewin->client.win, ShapeBounding,
				      ShapeSet);
		PropagateShapes(ewin->win);
		gettimeofday(&timev2, NULL);
		dsec = timev2.tv_sec - timev1.tv_sec;
		dusec = timev2.tv_usec - timev1.tv_usec;
		if (dusec < 0)
		  {
		     dsec--;
		     dusec += 1000000;
		  }
		tm = (double)dsec + (((double)dusec) / 1000000);
		spd = (int)((double)speed * tm);
		if (spd < min)
		   spd = min;
	     }
	ewin->shaded = 2;
	ewin->w = b;
	EMoveResizeWindow(disp, ewin->win_container, -30, -30, 1, 1);
	EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w, ewin->h);
	CalcEwinSizes(ewin);
	XSync(disp, False);
	break;
     case 1:
	att.win_gravity = WestGravity;
	XChangeWindowAttributes(disp, ewin->client.win, CWWinGravity, &att);
	MinShadeSize(ewin, &b, &d);
	a = ewin->w;
	c = ewin->x;
	d = ewin->x + ewin->w - b;
	if ((mode.animate_shading) || (ewin->menu))
	   for (k = 0; k <= 1024; k += spd)
	     {
		gettimeofday(&timev1, NULL);
		i = ((a * (1024 - k)) + (b * k)) >> 10;
		j = ((c * (1024 - k)) + (d * k)) >> 10;
		ewin->w = i;
		ewin->x = j;
		ewin->reqx = j;
		if (ewin->w < 1)
		   ewin->w = 1;
		ww = ewin->w - ewin->border->border.left -
		   ewin->border->border.right;
		if (ww < 1)
		   ww = 1;
		hh = ewin->client.h;
		EMoveResizeWindow(disp, ewin->win_container,
				  ewin->border->border.left,
				  ewin->border->border.top, ww, hh);
		EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w,
				  ewin->h);
		CalcEwinSizes(ewin);
		if (ewin->client.shaped)
		   EShapeCombineShape(disp, ewin->win_container,
				      ShapeBounding, 0, 0, ewin->client.win,
				      ShapeBounding, ShapeSet);
		PropagateShapes(ewin->win);
		gettimeofday(&timev2, NULL);
		dsec = timev2.tv_sec - timev1.tv_sec;
		dusec = timev2.tv_usec - timev1.tv_usec;
		if (dusec < 0)
		  {
		     dsec--;
		     dusec += 1000000;
		  }
		tm = (double)dsec + (((double)dusec) / 1000000);
		spd = (int)((double)speed * tm);
		if (spd < min)
		   spd = min;
	     }
	ewin->shaded = 2;
	ewin->w = b;
	ewin->x = d;
	ewin->reqx = d;
	EMoveResizeWindow(disp, ewin->win_container, -30, -30, 1, 1);
	EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w, ewin->h);
	CalcEwinSizes(ewin);
	XSync(disp, False);
	break;
     case 2:
	att.win_gravity = SouthGravity;
	XChangeWindowAttributes(disp, ewin->client.win, CWWinGravity, &att);
	a = ewin->h;
	MinShadeSize(ewin, &b, &d);
	b = d;
	if ((mode.animate_shading) || (ewin->menu))
	   for (k = 0; k <= 1024; k += spd)
	     {
		gettimeofday(&timev1, NULL);
		i = ((a * (1024 - k)) + (b * k)) >> 10;
		ewin->h = i;
		if (ewin->h < 1)
		   ewin->h = 1;
		hh = ewin->h - ewin->border->border.top -
		   ewin->border->border.bottom;
		if (hh < 1)
		   hh = 1;
		ww = ewin->client.w;
		EMoveResizeWindow(disp, ewin->win_container,
				  ewin->border->border.left,
				  ewin->border->border.top, ww, hh);
		EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w,
				  ewin->h);
		CalcEwinSizes(ewin);
		if (ewin->client.shaped)
		   EShapeCombineShape(disp, ewin->win_container,
				      ShapeBounding, 0,
				      -(ewin->client.h - hh),
				      ewin->client.win, ShapeBounding,
				      ShapeSet);
		PropagateShapes(ewin->win);
		gettimeofday(&timev2, NULL);
		dsec = timev2.tv_sec - timev1.tv_sec;
		dusec = timev2.tv_usec - timev1.tv_usec;
		if (dusec < 0)
		  {
		     dsec--;
		     dusec += 1000000;
		  }
		tm = (double)dsec + (((double)dusec) / 1000000);
		spd = (int)((double)speed * tm);
		if (spd < min)
		   spd = min;
	     }
	ewin->shaded = 2;
	ewin->h = b;
	EMoveResizeWindow(disp, ewin->win_container, -30, -30, 1, 1);
	EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w, ewin->h);
	CalcEwinSizes(ewin);
	XSync(disp, False);
	break;
     case 3:
	att.win_gravity = SouthGravity;
	XChangeWindowAttributes(disp, ewin->client.win, CWWinGravity, &att);
	MinShadeSize(ewin, &b, &d);
	a = ewin->h;
	b = d;
	c = ewin->y;
	d = ewin->y + ewin->h - b;
	if ((mode.animate_shading) || (ewin->menu))
	   for (k = 0; k <= 1024; k += spd)
	     {
		gettimeofday(&timev1, NULL);
		i = ((a * (1024 - k)) + (b * k)) >> 10;
		j = ((c * (1024 - k)) + (d * k)) >> 10;
		ewin->h = i;
		ewin->y = j;
		ewin->reqy = j;
		if (ewin->h < 1)
		   ewin->h = 1;
		hh = ewin->h - ewin->border->border.top -
		   ewin->border->border.bottom;
		if (hh < 1)
		   hh = 1;
		ww = ewin->client.w;
		EMoveResizeWindow(disp, ewin->win_container,
				  ewin->border->border.left,
				  ewin->border->border.top, ww, hh);
		EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w,
				  ewin->h);
		CalcEwinSizes(ewin);
		if (ewin->client.shaped)
		   EShapeCombineShape(disp, ewin->win_container,
				      ShapeBounding, 0, 0, ewin->client.win,
				      ShapeBounding, ShapeSet);
		PropagateShapes(ewin->win);
		gettimeofday(&timev2, NULL);
		dsec = timev2.tv_sec - timev1.tv_sec;
		dusec = timev2.tv_usec - timev1.tv_usec;
		if (dusec < 0)
		  {
		     dsec--;
		     dusec += 1000000;
		  }
		tm = (double)dsec + (((double)dusec) / 1000000);
		spd = (int)((double)speed * tm);
		if (spd < min)
		   spd = min;
	     }
	ewin->shaded = 2;
	ewin->h = b;
	ewin->y = d;
	ewin->reqy = d;
	EMoveResizeWindow(disp, ewin->win_container, -30, -30, 1, 1);
	EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w, ewin->h);
	CalcEwinSizes(ewin);
	XSync(disp, False);
	break;
     default:
	break;
     }
   UngrabX();
   if (ewin->client.shaped)
      EShapeCombineShape(disp, ewin->win_container, ShapeBounding, 0, 0,
			 ewin->client.win, ShapeBounding, ShapeSet);
   PropagateShapes(ewin->win);
   queue_up = pq;
   HintsSetWindowState(ewin);
   if (mode.mode == MODE_NONE)
     {
	PagerEwinOutsideAreaUpdate(ewin);
	ForceUpdatePagersForDesktop(ewin->desktop);
     }
   EDBUG_RETURN_;
}

void
UnShadeEwin(EWin * ewin)
{
   XSetWindowAttributes att;
   int                 i, j, speed, a, b, c, d;
   int                 k, spd, min;
   struct timeval      timev1, timev2;
   int                 dsec, dusec;
   double              tm;
   char                pq;

   EDBUG(4, "UnShadeEwin");
   if (GetZoomEWin() == ewin)
      EDBUG_RETURN_;
   if (!ewin->shaded)
      EDBUG_RETURN_;
   pq = queue_up;
   queue_up = 0;
   speed = mode.shadespeed;
   spd = 32;
   min = 2;
   GrabX();
   switch (ewin->border->shadedir)
     {
     case 0:
	att.win_gravity = EastGravity;
	XChangeWindowAttributes(disp, ewin->client.win, CWWinGravity, &att);
	a = ewin->border->border.left;
	b = ewin->client.w + ewin->border->border.left +
	   ewin->border->border.right;
	ewin->shaded = 0;
	EMoveResizeWindow(disp, ewin->win_container,
			  ewin->border->border.left, ewin->border->border.top,
			  1, ewin->client.h);
	EMoveResizeWindow(disp, ewin->client.win, -ewin->client.w, 0,
			  ewin->client.w, ewin->client.h);
	EMapWindow(disp, ewin->client.win);
	EMapWindow(disp, ewin->win_container);
	if ((mode.animate_shading) || (ewin->menu))
	   for (k = 0; k <= 1024; k += spd)
	     {
		gettimeofday(&timev1, NULL);
		i = ((a * (1024 - k)) + (b * k)) >> 10;
		ewin->w = i;
		EMoveResizeWindow(disp, ewin->win_container,
				  ewin->border->border.left,
				  ewin->border->border.top,
				  ewin->w - ewin->border->border.left -
				  ewin->border->border.right, ewin->client.h);
		EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w,
				  ewin->h);
		CalcEwinSizes(ewin);
		if (ewin->client.shaped)
		   EShapeCombineShape(disp, ewin->win_container,
				      ShapeBounding,
				      -(ewin->client.w -
					(ewin->w - ewin->border->border.left -
					 ewin->border->border.right)), 0,
				      ewin->client.win, ShapeBounding,
				      ShapeSet);
		PropagateShapes(ewin->win);
		gettimeofday(&timev2, NULL);
		dsec = timev2.tv_sec - timev1.tv_sec;
		dusec = timev2.tv_usec - timev1.tv_usec;
		if (dusec < 0)
		  {
		     dsec--;
		     dusec += 1000000;
		  }
		tm = (double)dsec + (((double)dusec) / 1000000);
		spd = (int)((double)speed * tm);
		if (spd < min)
		   spd = min;
	     }
	ewin->w = b;
	MoveResizeEwin(ewin, ewin->x, ewin->y, ewin->client.w, ewin->client.h);
	XSync(disp, False);
	break;
     case 1:
	att.win_gravity = WestGravity;
	XChangeWindowAttributes(disp, ewin->client.win, CWWinGravity, &att);
	a = ewin->border->border.right;
	b = ewin->client.w + ewin->border->border.left +
	   ewin->border->border.right;
	c = ewin->x;
	d = ewin->x + ewin->w - (ewin->border->border.right + ewin->client.w +
				 ewin->border->border.left);
	ewin->shaded = 0;
	EMoveResizeWindow(disp, ewin->win_container,
			  ewin->border->border.left, ewin->border->border.top,
			  1, ewin->client.h);
	EMoveResizeWindow(disp, ewin->client.win, 0, 0, ewin->client.w,
			  ewin->client.h);
	EMapWindow(disp, ewin->client.win);
	EMapWindow(disp, ewin->win_container);
	if ((mode.animate_shading) || (ewin->menu))
	   for (k = 0; k <= 1024; k += spd)
	     {
		gettimeofday(&timev1, NULL);
		i = ((a * (1024 - k)) + (b * k)) >> 10;
		j = ((c * (1024 - k)) + (d * k)) >> 10;
		ewin->w = i;
		ewin->x = j;
		ewin->reqx = j;
		EMoveResizeWindow(disp, ewin->win_container,
				  ewin->border->border.left,
				  ewin->border->border.top,
				  ewin->w - ewin->border->border.left -
				  ewin->border->border.right, ewin->client.h);
		EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w,
				  ewin->h);
		CalcEwinSizes(ewin);
		if (ewin->client.shaped)
		   EShapeCombineShape(disp, ewin->win_container,
				      ShapeBounding, 0, 0, ewin->client.win,
				      ShapeBounding, ShapeSet);
		PropagateShapes(ewin->win);
		gettimeofday(&timev2, NULL);
		dsec = timev2.tv_sec - timev1.tv_sec;
		dusec = timev2.tv_usec - timev1.tv_usec;
		if (dusec < 0)
		  {
		     dsec--;
		     dusec += 1000000;
		  }
		tm = (double)dsec + (((double)dusec) / 1000000);
		spd = (int)((double)speed * tm);
		if (spd < min)
		   spd = min;
	     }
	ewin->w = b;
	ewin->x = d;
	ewin->reqx = d;
	MoveResizeEwin(ewin, ewin->x, ewin->y, ewin->client.w, ewin->client.h);
	XSync(disp, False);
	break;
     case 2:
	att.win_gravity = SouthGravity;
	XChangeWindowAttributes(disp, ewin->client.win, CWWinGravity, &att);
	a = ewin->border->border.top;
	b = ewin->client.h + ewin->border->border.top +
	   ewin->border->border.bottom;
	ewin->shaded = 0;
	EMoveResizeWindow(disp, ewin->win_container,
			  ewin->border->border.left, ewin->border->border.top,
			  ewin->client.w, 1);
	EMoveResizeWindow(disp, ewin->client.win, 0, -ewin->client.h,
			  ewin->client.w, ewin->client.h);
	EMapWindow(disp, ewin->client.win);
	EMapWindow(disp, ewin->win_container);
	if ((mode.animate_shading) || (ewin->menu))
	   for (k = 0; k <= 1024; k += spd)
	     {
		gettimeofday(&timev1, NULL);
		i = ((a * (1024 - k)) + (b * k)) >> 10;
		ewin->h = i;
		EMoveResizeWindow(disp, ewin->win_container,
				  ewin->border->border.left,
				  ewin->border->border.top, ewin->client.w,
				  ewin->h - ewin->border->border.top -
				  ewin->border->border.bottom);
		EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w,
				  ewin->h);
		CalcEwinSizes(ewin);
		if (ewin->client.shaped)
		   EShapeCombineShape(disp, ewin->win_container,
				      ShapeBounding, 0,
				      -(ewin->client.h -
					(ewin->h - ewin->border->border.top -
					 ewin->border->border.bottom)),
				      ewin->client.win, ShapeBounding,
				      ShapeSet);
		PropagateShapes(ewin->win);
		gettimeofday(&timev2, NULL);
		dsec = timev2.tv_sec - timev1.tv_sec;
		dusec = timev2.tv_usec - timev1.tv_usec;
		if (dusec < 0)
		  {
		     dsec--;
		     dusec += 1000000;
		  }
		tm = (double)dsec + (((double)dusec) / 1000000);
		spd = (int)((double)speed * tm);
		if (spd < min)
		   spd = min;
	     }
	ewin->h = b;
	MoveResizeEwin(ewin, ewin->x, ewin->y, ewin->client.w, ewin->client.h);
	XSync(disp, False);
	break;
     case 3:
	att.win_gravity = SouthGravity;
	XChangeWindowAttributes(disp, ewin->client.win, CWWinGravity, &att);
	a = ewin->border->border.bottom;
	b = ewin->client.h + ewin->border->border.top +
	   ewin->border->border.bottom;
	c = ewin->y;
	d = ewin->y + ewin->h - (ewin->border->border.bottom +
				 ewin->client.h + ewin->border->border.top);
	ewin->shaded = 0;
	EMoveResizeWindow(disp, ewin->win_container,
			  ewin->border->border.left, ewin->border->border.top,
			  ewin->client.w, 1);
	EMoveResizeWindow(disp, ewin->client.win, 0, 0, ewin->client.w,
			  ewin->client.h);
	EMapWindow(disp, ewin->client.win);
	EMapWindow(disp, ewin->win_container);
	if ((mode.animate_shading) || (ewin->menu))
	   for (k = 0; k <= 1024; k += spd)
	     {
		gettimeofday(&timev1, NULL);
		i = ((a * (1024 - k)) + (b * k)) >> 10;
		j = ((c * (1024 - k)) + (d * k)) >> 10;
		ewin->h = i;
		ewin->y = j;
		ewin->reqy = j;
		EMoveResizeWindow(disp, ewin->win_container,
				  ewin->border->border.left,
				  ewin->border->border.top, ewin->client.w,
				  ewin->h - ewin->border->border.top -
				  ewin->border->border.bottom);
		EMoveResizeWindow(disp, ewin->win, ewin->x, ewin->y, ewin->w,
				  ewin->h);
		CalcEwinSizes(ewin);
		if (ewin->client.shaped)
		   EShapeCombineShape(disp, ewin->win_container,
				      ShapeBounding, 0, 0, ewin->client.win,
				      ShapeBounding, ShapeSet);
		PropagateShapes(ewin->win);
		gettimeofday(&timev2, NULL);
		dsec = timev2.tv_sec - timev1.tv_sec;
		dusec = timev2.tv_usec - timev1.tv_usec;
		if (dusec < 0)
		  {
		     dsec--;
		     dusec += 1000000;
		  }
		tm = (double)dsec + (((double)dusec) / 1000000);
		spd = (int)((double)speed * tm);
		if (spd < min)
		   spd = min;
	     }
	ewin->h = b;
	ewin->y = d;
	ewin->reqy = d;
	MoveResizeEwin(ewin, ewin->x, ewin->y, ewin->client.w, ewin->client.h);
	XSync(disp, False);
	break;
     default:
	break;
     }
   UngrabX();
   if (ewin->client.shaped)
      EShapeCombineShape(disp, ewin->win_container, ShapeBounding, 0, 0,
			 ewin->client.win, ShapeBounding, ShapeSet);
   PropagateShapes(ewin->win);
   queue_up = pq;
   HintsSetWindowState(ewin);
   if (mode.mode == MODE_NONE)
     {
	PagerEwinOutsideAreaUpdate(ewin);
	ForceUpdatePagersForDesktop(ewin->desktop);
     }
   EDBUG_RETURN_;
}
