/*
 * Copyright (C) 2000 Carsten Haitzler, Geoff Harrison and various contributors
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

static void         ReverseTimeout(int val, void *data);
static void         AutoraiseTimeout(int val, void *data);

static int          new_desk_focus_nesting = 0;

/* Mostly stolen from the temporary 'ToolTipTimeout' */
static void
AutoraiseTimeout(int val, void *data)
{
   EWin               *found_ewin;

   if (mode.focusmode == FOCUS_CLICK)
      return;
   found_ewin = FindItem("", val, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (found_ewin)
      RaiseEwin(found_ewin);
   data = NULL;
}

static void
ReverseTimeout(int val, void *data)
{
   EWin               *ewin;

   ewin = RemoveItem("EWIN", val, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
      AddItem(ewin, "EWIN", ewin->client.win, LIST_TYPE_EWIN);
   val = 0;
   data = NULL;
}

void
GetNextFocusEwin(void)
{
   EWin              **lst0, **lst, *ewin;
   int                 i, num0, num, ax, ay;

   EDBUG(5, "GetNextFocusEwin");
   if (mode.display_warp)
     {
	WarpFocus(1);
	EDBUG_RETURN_;
     }
   lst0 = (EWin **) ListItemType(&num0, LIST_TYPE_EWIN);
   num = 0;
   lst = NULL;
   GetCurrentArea(&ax, &ay);
   if (lst0)
     {
	for (i = 0; i < num0; i++)
	  {
	     ewin = lst0[i];
	     if (((ewin->sticky) || (ewin->desktop == desks.current)) && ((ewin->area_x == ax) && (ewin->area_y == ay)) && (!ewin->skipfocus) && (!ewin->neverfocus) && (!ewin->iconified) && (!ewin->menu) && (!ewin->pager) && (!ewin->ibox) && (!ewin->iconified)	/*&& (ewin->client.mwm_decor_title) &&
																																	 * * * * (ewin->client.mwm_decor_border) */
		)
	       {
		  num++;
		  lst = Erealloc(lst, sizeof(EWin *) * num);
		  lst[num - 1] = lst0[i];
	       }
	  }
	Efree(lst0);
     }
   ewin = NULL;
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     if (mode.focuswin == lst[i])
	       {
		  if (i < num - 1)
		     ewin = lst[i + 1];
		  else
		     ewin = lst[0];
		  Efree(lst);
		  if (mode.raise_on_next_focus)
		     RaiseEwin(ewin);
		  if (mode.warp_on_next_focus)
		     XWarpPointer(disp, None, ewin->win, 0, 0, 0, 0,
				  ewin->w / 2, ewin->h / 2);
		  FocusToEWin(ewin);
		  EDBUG_RETURN_;
	       }
	  }
	ewin = lst[0];
	Efree(lst);
	if (mode.raise_on_next_focus)
	   RaiseEwin(ewin);
	if (mode.warp_on_next_focus)
	   XWarpPointer(disp, None, ewin->win, 0, 0, 0, 0, ewin->w / 2,
			ewin->h / 2);
	FocusToEWin(ewin);
     }
   EDBUG_RETURN_;
}

void
GetPrevFocusEwin(void)
{
   EWin              **lst0, **lst, *ewin;
   int                 i, num0, num, ax, ay;

   EDBUG(5, "GetPrevFocusEwin");
   if (mode.display_warp)
     {
	WarpFocus(-1);
	EDBUG_RETURN_;
     }
   RemoveTimerEvent("REVERSE_FOCUS_TIMEOUT");
   DoIn("REVERSE_FOCUS_TIMEOUT", 1.0, ReverseTimeout, 0, NULL);
   lst0 = (EWin **) ListItemType(&num0, LIST_TYPE_EWIN);
   num = 0;
   lst = NULL;
   GetCurrentArea(&ax, &ay);
   if (lst0)
     {
	for (i = 0; i < num0; i++)
	  {
	     ewin = lst0[i];
	     DetermineEwinArea(ewin);
	     if (((ewin->sticky) || (ewin->desktop == desks.current))
		 && ((ewin->area_x == ax) && (ewin->area_y == ay))
		 && (!ewin->skipfocus) && (!ewin->neverfocus) && (!ewin->shaded)
		 && (!ewin->menu) && (!ewin->pager) && (!ewin->ibox)
		 && (!ewin->iconified)
		 /* && (ewin->client.mwm_decor_title) &&
		  *    (ewin->client.mwm_decor_border) */
		)
	       {
		  num++;
		  lst = Erealloc(lst, sizeof(EWin *) * num);
		  lst[num - 1] = lst0[i];
	       }
	  }
	Efree(lst0);
     }
   ewin = NULL;
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     if (mode.focuswin == lst[i])
	       {
		  if (i == 0)
		     ewin = lst[num - 1];
		  else
		     ewin = lst[i - 1];
		  Efree(lst);
		  if (mode.raise_on_next_focus)
		     RaiseEwin(ewin);
		  if (mode.warp_on_next_focus)
		     XWarpPointer(disp, None, ewin->win, 0, 0, 0, 0,
				  ewin->w / 2, ewin->h / 2);
		  FocusToEWin(ewin);
		  EDBUG_RETURN_;
	       }
	  }
	ewin = lst[0];
	Efree(lst);
	if (mode.raise_on_next_focus)
	   RaiseEwin(ewin);
	if (mode.warp_on_next_focus)
	   XWarpPointer(disp, None, ewin->win, 0, 0, 0, 0, ewin->w / 2,
			ewin->h / 2);
	FocusToEWin(ewin);
     }
   EDBUG_RETURN_;
}

void
FixFocus(void)
{
   EWin              **lst, *ewin;
   int                 i, num;

   EDBUG(5, "FixFocus");
   num = 0;
   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     if (mode.focusmode == FOCUS_CLICK)
	       {
		  if (!(ewin->active))
		     XGrabButton(disp, AnyButton, AnyModifier,
				 ewin->win_container, False, ButtonPressMask,
				 GrabModeSync, GrabModeAsync, None, None);
	       }
	     else
	       {
		  XUngrabButton(disp, AnyButton, AnyModifier,
				ewin->win_container);
		  GrabButtonGrabs(ewin);
	       }
	  }
	Efree(lst);
     }
}

void
FocusToEWin(EWin * ewin)
{
   int                 ax, ay;

   EDBUG(4, "FocusToEWin");

   if (mode.slideout)
      EDBUG_RETURN_;

   if (clickmenu)
      EDBUG_RETURN_;

   ICCCM_Cmap(ewin);
   if ((!ewin) && (mode.focusmode != FOCUS_POINTER))
     {
	ewin = FindItem("", 0, LIST_FINDBY_NONE, LIST_TYPE_EWIN);
	if (mode.focusmode == FOCUS_CLICK)
	  {
	     if ((mode.focuswin) && (ewin))
	       {
		  if (!
		      (((ewin->desktop
			 == mode.focuswin->desktop) || (ewin->sticky))
		       && (ewin->area_x == mode.focuswin->area_x)
		       && (ewin->area_y == mode.focuswin->area_y)))
		     ewin = NULL;
	       }
	     else if (!mode.focuswin)
		ewin = NULL;
	  }
     }
   if (ewin == mode.focuswin)
     {
	if (!mode.cur_menu_mode)
	   mode.context_ewin = ewin;
	EDBUG_RETURN_;
     }
   /* Never focus a window that's not on the current desktop.  That's just dumb. -- mej */
   if ((ewin) && (ewin->desktop != desks.current && !(ewin->sticky)))
      ewin = NULL;
   if ((ewin) && (ewin->neverfocus))
     {
	EDBUG_RETURN_;
     }
   if ((ewin) && (!ewin->client.need_input))
     {
	if (mode.windowdestroy == 1)
	  {
	     mode.focuswin = NULL;
	     mode.realfocuswin = NULL;
	     mode.windowdestroy = 0;
	  }
	if (!mode.cur_menu_mode)
	   mode.context_ewin = ewin;
	EDBUG_RETURN_;
     }
   mode.windowdestroy = 0;
   if (mode.focuswin)
     {
	if (mode.autoraise)
	   RemoveTimerEvent("AUTORAISE_TIMEOUT");
	mode.focuswin->active = 0;
	/* losing the focus may cause the titlebar to be resized */
	CalcEwinSizes(mode.focuswin);
	DrawEwin(mode.focuswin);
	if ((mode.clickalways) || (mode.focusmode == FOCUS_CLICK))
	   XGrabButton(disp, AnyButton, AnyModifier,
		       mode.focuswin->win_container, False, ButtonPressMask,
		       GrabModeSync, GrabModeAsync, None, None);
     }
   if (!ewin)
     {
	mode.focuswin = NULL;
	mode.realfocuswin = NULL;
	mode.context_ewin = NULL;
	ICCCM_Focus(NULL);
	EDBUG_RETURN_;
     }
   else if (!ewin->menu)
      mode.realfocuswin = ewin;
   if (ewin)
     {
	if (!mode.cur_menu_mode)
	   mode.context_ewin = ewin;
	mode.focuswin = ewin;
	mode.focuswin->active = 1;
     }
   /* gaining the focus may cause the titlebar to be resized */
   if ((mode.focusmode == FOCUS_CLICK) && (mode.focuswin))
     {
	XUngrabButton(disp, AnyButton, AnyModifier,
		      mode.focuswin->win_container);
	GrabButtonGrabs(mode.focuswin);
     }
   if (mode.focuswin)
     {
	CalcEwinSizes(mode.focuswin);
	DrawEwin(mode.focuswin);
	AUDIO_PLAY("SOUND_FOCUS_SET");
	ICCCM_Focus(mode.focuswin);
     }
/*   ReZoom(mode.focuswin); */
   if ((mode.autoraise) && (mode.focuswin) && (!mode.focuswin->menu)
       && (mode.focusmode != FOCUS_CLICK))
      DoIn("AUTORAISE_TIMEOUT", mode.autoraisetime, AutoraiseTimeout,
	   mode.focuswin->client.win, NULL);
   if (mode.focusmode == FOCUS_CLICK)
     {
	if (ewin)
	  {
	     if (!ewin->sticky)
	       {
		  if ((ewin->desktop != desks.current) && (!ewin->iconified))
		     GotoDesktop(ewin->desktop);
	       }
	     if ((!ewin->fixedpos) && (!ewin->sticky))
	       {
		  GetCurrentArea(&ax, &ay);
		  if ((ax != ewin->area_x) || (ay != ewin->area_y))
		    {
		       if ((ewin->x >= root.w) || (ewin->y >= root.h)
			   || ((ewin->x + ewin->w) < 0)
			   || ((ewin->y + ewin->h) < 0))
			  SetCurrentArea(ewin->area_x, ewin->area_y);
		    }
	       }
	  }
     }
   RemoveTimerEvent("REVERSE_FOCUS_TIMEOUT");
   if (mode.focuswin)
      DoIn("REVERSE_FOCUS_TIMEOUT", 0.5, ReverseTimeout,
	   mode.focuswin->client.win, NULL);
   EDBUG_RETURN_;
}

void
BeginNewDeskFocus(void)
{
   EWin               *ewin, **lst;
   int                 i, j, num;

   if (new_desk_focus_nesting++)
      return;

   /* we are about to flip desktops or areas - disable enter and leave events
    * temporarily */

   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     XSelectInput(disp, ewin->win,
			  FocusChangeMask | SubstructureNotifyMask |
			  SubstructureRedirectMask | PropertyChangeMask |
			  ResizeRedirectMask);
	     if (ewin->pager)
		XSelectInput(disp, ewin->client.win,
			     PropertyChangeMask | FocusChangeMask |
			     ResizeRedirectMask | StructureNotifyMask |
			     ColormapChangeMask | ButtonPressMask |
			     ButtonReleaseMask | PointerMotionMask);
	     else if (ewin->dialog)
		XSelectInput(disp, ewin->client.win,
			     PropertyChangeMask | FocusChangeMask |
			     ResizeRedirectMask | StructureNotifyMask |
			     ColormapChangeMask | ExposureMask | KeyPressMask);
	     else
		XSelectInput(disp, ewin->client.win,
			     PropertyChangeMask | FocusChangeMask |
			     ResizeRedirectMask | StructureNotifyMask |
			     ColormapChangeMask);
	     for (j = 0; j < ewin->border->num_winparts; j++)
	       {
		  if (ewin->border->part[j].flags & FLAG_TITLE)
		     XSelectInput(disp, ewin->bits[j].win,
				  ExposureMask | ButtonPressMask |
				  ButtonReleaseMask);
		  else
		     XSelectInput(disp, ewin->bits[j].win,
				  ButtonPressMask | ButtonReleaseMask);
	       }
	  }
	Efree(lst);
     }

   for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
      XSelectInput(disp, desks.desk[i].win,
		   PropertyChangeMask | SubstructureRedirectMask |
		   ButtonPressMask | ButtonReleaseMask);
}

void
NewDeskFocus(void)
{
   EWin               *ewin, **lst;
   int                 i, j, num;

   EDBUG(4, "NewDeskFocus");

   if (--new_desk_focus_nesting)
      return;

   /* we flipped - re-enable enter and leave events */
   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     XSelectInput(disp, ewin->win,
			  FocusChangeMask | SubstructureNotifyMask |
			  SubstructureRedirectMask | EnterWindowMask |
			  LeaveWindowMask | PointerMotionMask |
			  PropertyChangeMask | ResizeRedirectMask |
			  ButtonPressMask | ButtonReleaseMask);
	     if (ewin->pager)
		XSelectInput(disp, ewin->client.win,
			     PropertyChangeMask | EnterWindowMask |
			     LeaveWindowMask | FocusChangeMask |
			     ResizeRedirectMask | StructureNotifyMask |
			     ColormapChangeMask | ButtonPressMask |
			     ButtonReleaseMask | PointerMotionMask);
	     else if (ewin->dialog)
		XSelectInput(disp, ewin->client.win,
			     PropertyChangeMask | EnterWindowMask |
			     LeaveWindowMask | FocusChangeMask |
			     ResizeRedirectMask | StructureNotifyMask |
			     ColormapChangeMask | ExposureMask | KeyPressMask);
	     else
		XSelectInput(disp, ewin->client.win,
			     PropertyChangeMask | EnterWindowMask |
			     LeaveWindowMask | FocusChangeMask |
			     ResizeRedirectMask | StructureNotifyMask |
			     ColormapChangeMask);
	     for (j = 0; j < ewin->border->num_winparts; j++)
	       {
		  if (ewin->border->part[j].flags & FLAG_TITLE)
		     XSelectInput(disp, ewin->bits[j].win,
				  ExposureMask | KeyPressMask | KeyReleaseMask |
				  ButtonPressMask | ButtonReleaseMask |
				  EnterWindowMask | LeaveWindowMask |
				  PointerMotionMask);
		  else
		     XSelectInput(disp, ewin->bits[j].win,
				  KeyPressMask | KeyReleaseMask |
				  ButtonPressMask | ButtonReleaseMask |
				  EnterWindowMask | LeaveWindowMask |
				  PointerMotionMask);
	       }
	  }
	Efree(lst);
     }

   for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
      XSelectInput(disp, desks.desk[i].win,
		   SubstructureNotifyMask | ButtonPressMask |
		   ButtonReleaseMask | EnterWindowMask | LeaveWindowMask |
		   ButtonMotionMask | PropertyChangeMask |
		   SubstructureRedirectMask | KeyPressMask | KeyReleaseMask |
		   PointerMotionMask);

   if ((mode.focusmode == FOCUS_POINTER) || (mode.focusmode == FOCUS_SLOPPY))
     {
	ewin = GetEwinPointerInClient();
	if (ewin)
	   ICCCM_Focus(ewin);
	else
	   ICCCM_Focus(NULL);
     }
   else
     {
	lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
	if (lst)
	  {
	     int                 ax, ay;

	     GetCurrentArea(&ax, &ay);
	     for (i = 0; i < num; i++)
	       {
		  ewin = lst[i];
		  if ((ewin->sticky)
		      ||
		      ((((ewin->area_x
			  == ax) && (ewin->area_y == ay)) || (ewin->fixedpos))
		       && (ewin->desktop == desks.current)))
		    {
		       ICCCM_Focus(ewin);
		       break;
		    }
	       }
	     Efree(lst);
	  }
     }

   EDBUG_RETURN_;
}

#if 0				/* Clean up if OK -- Remove FocusToNone */
void
FocusToNone(void)
{

   EDBUG(4, "FocusToNone");

   if ((mode.focuswin) && (mode.focuswin->floating))
      EDBUG_RETURN_;
   ICCCM_Cmap(NULL);
   if (mode.focuswin)
     {
	if (mode.autoraise)
	   RemoveTimerEvent("AUTORAISE_TIMEOUT");
	mode.focuswin->active = 0;
	/* losing the focus may cause the titlebar to be resized */
	CalcEwinSizes(mode.focuswin);
	DrawEwin(mode.focuswin);
	if (mode.focusmode == FOCUS_CLICK)
	   XGrabButton(disp, AnyButton, AnyModifier,
		       mode.focuswin->win_container, False, ButtonPressMask,
		       GrabModeSync, GrabModeAsync, None, None);
     }
   XSetInputFocus(disp, root.win, RevertToPointerRoot, CurrentTime);
   mode.focuswin = NULL;
   mode.realfocuswin = NULL;
   EDBUG_RETURN_;
}
#endif
