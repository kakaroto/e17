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

#if ENABLE_KDE

/* This code is copyright 2000 The Enlightenment Development Team */
/* based on code by Matthias Ettrich from KWM (http://www.kde.org)
 * and code by Alfredo Kojima in Window Maker (http://www.windowmaker.org)
 * both of which are copyright under the GPL  (http://www.fsf.org)
 *
 * To anyone who hasn't implemented KDE hints in your window manager,
 * feel free to snarf this code and make it work for you, too.
 *
 * thanks to everyone who helped me get this working
 * --Mandrake
 */

/* some #defines to make this a little more legible later */

#define KDE_NO_DECORATION 0
#define KDE_NORMAL_DECORATION 1
#define KDE_TINY_DECORATION 2
#define KDE_NO_FOCUS 256
#define KDE_STANDALONE_MENUBAR 512
#define KDE_DESKTOP_ICON 1024
#define KDE_ONTOP 2048

/* initialize all the KDE Hint Atoms */

static Atom         KDE_COMMAND = 0;
static Atom         KDE_ACTIVE_WINDOW = 0;
static Atom         KDE_ACTIVATE_WINDOW = 0;
static Atom         KDE_DO_NOT_MANAGE = 0;
static Atom         KDE_DOCKWINDOW = 0;
static Atom         KDE_RUNNING = 0;
static Atom         KDE_MAXIMIZE_WINDOW = 0;
static Atom         KDE_STICKY_WINDOW = 0;

static Atom         KDE_CURRENT_DESKTOP = 0;
static Atom         KDE_NUMBER_OF_DESKTOPS = 0;
static Atom         KDE_DESKTOP_NAME[ENLIGHTENMENT_CONF_NUM_DESKTOPS];
static Atom         KDE_DESKTOP_WINDOW_REGION[32];

static Atom         KDE_MODULE = 0;
static Atom         KDE_MODULE_INIT = 0;
static Atom         KDE_MODULE_INITIALIZED = 0;
static Atom         KDE_MODULE_DESKTOP_CHANGE = 0;
static Atom         KDE_MODULE_DESKTOP_NAME_CHANGE = 0;
static Atom         KDE_MODULE_DESKTOP_NUMBER_CHANGE = 0;
static Atom         KDE_MODULE_WIN_ADD = 0;
static Atom         KDE_MODULE_WIN_REMOVE = 0;
static Atom         KDE_MODULE_WIN_CHANGE = 0;
static Atom         KDE_MODULE_WIN_RAISE = 0;
static Atom         KDE_MODULE_WIN_LOWER = 0;
static Atom         KDE_MODULE_WIN_ACTIVATE = 0;
static Atom         KDE_MODULE_WIN_ICON_CHANGE = 0;
static Atom         KDE_MODULE_DOCKWIN_ADD = 0;
static Atom         KDE_MODULE_DOCKWIN_REMOVE = 0;

static Atom         KDE_WIN_UNSAVED_DATA = 0;
static Atom         KDE_WIN_DECORATION = 0;
static Atom         KDE_WIN_DESKTOP = 0;
static Atom         KDE_WIN_GEOMETRY_RESTORE = 0;
static Atom         KDE_WIN_ICONIFIED = 0;
static Atom         KDE_WIN_MAXIMIZED = 0;
static Atom         KDE_WIN_STICKY = 0;
static Atom         KDE_WIN_ICON_GEOMETRY = 0;
static Atom         KDE_WIN_TITLE = 0;

/* the modules I have to communicate to */
typedef struct KModuleList
{

   Window              win;
   struct KModuleList *next;

}
KModuleList;

static KModuleList *KModules = NULL;

static void
KDE_ClientMessage(Window win, Atom atom, long data, Time timestamp)
{

   XEvent              ev;
   long                mask = 0;

   EDBUG(6, "KDE_ClientMessage");

   memset(&ev, 0, sizeof(XEvent));
   ev.xclient.window = win;
   ev.xclient.message_type = atom;
   ev.xclient.type = ClientMessage;
   ev.xclient.format = 32;
   ev.xclient.data.l[0] = data;
   ev.xclient.data.l[1] = timestamp;

   XSendEvent(disp, win, False, mask, &ev);

   EDBUG_RETURN_;

}

static void
KDE_ClientTextMessage(Window win, Atom atom, char *data)
{

   XEvent              ev;
   long                mask = 0;

   EDBUG(6, "KDE_ClientTextMessage");

   memset(&ev, 0, sizeof(XEvent));
   ev.xclient.window = win;
   ev.xclient.message_type = atom;
   ev.xclient.type = ClientMessage;
   ev.xclient.format = 32;
   strcpy(ev.xclient.data.b, data);

   XSendEvent(disp, win, False, mask, &ev);

   EDBUG_RETURN_;

}

static void
KDE_SendMessagesToModules(Atom atom, long data)
{

   KModuleList        *ptr;

   EDBUG(6, "KDE_SendMessagesToModules");

   ptr = KModules;
   while (ptr)
     {
	KDE_ClientMessage(ptr->win, atom, data, CurrentTime);
	ptr = ptr->next;
     }

   EDBUG_RETURN_;

}

void
KDE_UpdateFocusedWindow(void)
{

   EWin               *ewin;

   EDBUG(6, "KDE_UpdateWindows");

   ewin = GetFocusEwin();

   if (getSimpleHint(root.win, KDE_ACTIVE_WINDOW))
     {
	if (ewin)
	  {
	     if (ewin->client.win ==
		 (Window) * (getSimpleHint(root.win, KDE_ACTIVE_WINDOW)))
		EDBUG_RETURN_;
	  }
	else
	  {
	     if (0 == *(getSimpleHint(root.win, KDE_ACTIVE_WINDOW)))
		EDBUG_RETURN_;
	  }
     }
   if (ewin)
     {
	XChangeProperty(disp, root.win, KDE_ACTIVE_WINDOW, KDE_ACTIVE_WINDOW,
			32, PropModeReplace,
			(unsigned char *)&(ewin->client.win), 1);
     }
   else
     {
	if (getSimpleHint(root.win, KDE_ACTIVE_WINDOW))
	   deleteHint(root.win, KDE_ACTIVE_WINDOW);
     }

   if (ewin)
     {
	KDE_SendMessagesToModules(KDE_MODULE_WIN_ACTIVATE, ewin->client.win);
     }
   else
     {
	KDE_SendMessagesToModules(KDE_MODULE_WIN_ACTIVATE, 0);
     }

   EDBUG_RETURN_;

}

void
KDE_NewWindow(EWin * ewin)
{

   EDBUG(6, "KDE_NewWindow");

   if (!ewin)
      EDBUG_RETURN_;

   if (!(ewin->internal))
     {
	if (!ewin->kde_hint)
	  {
	     XChangeProperty(disp, ewin->client.win, KDE_WIN_TITLE, XA_STRING,
			     8, PropModeReplace,
			     (unsigned char *)ewin->client.title,
			     strlen(ewin->client.title) + 1);

	     if (ewin->sticky)
	       {
		  setSimpleHint(ewin->client.win, KDE_WIN_STICKY, 1);
	       }
	     else
	       {
		  setSimpleHint(ewin->client.win, KDE_WIN_STICKY, 0);
	       }

	     if (ewin->iconified)
	       {
		  setSimpleHint(ewin->client.win, KDE_WIN_ICONIFIED, 1);
	       }
	     else
	       {
		  setSimpleHint(ewin->client.win, KDE_WIN_ICONIFIED, 0);
	       }

	     if (ewin->toggle)
	       {
		  setSimpleHint(ewin->client.win, KDE_WIN_MAXIMIZED, 1);
	       }
	     else
	       {
		  setSimpleHint(ewin->client.win, KDE_WIN_MAXIMIZED, 0);
	       }

	     setSimpleHint(ewin->client.win, KDE_WIN_DESKTOP,
			   ewin->desktop + 1);

	     ewin->kde_hint = 1;
	     KDE_SendMessagesToModules(KDE_MODULE_WIN_ADD, ewin->client.win);
	  }
     }
   EDBUG_RETURN_;

}

void
KDE_RemoveWindow(EWin * ewin)
{

   EDBUG(6, "KDE_RemoveWindow");

   if (!ewin)
      EDBUG_RETURN_;

   /* hose the hints from the window */
   if (getSimpleHint(ewin->client.win, KDE_WIN_TITLE))
      deleteHint(ewin->client.win, KDE_WIN_TITLE);
   if (getSimpleHint(ewin->client.win, KDE_WIN_STICKY))
      deleteHint(ewin->client.win, KDE_WIN_STICKY);
   if (getSimpleHint(ewin->client.win, KDE_WIN_ICONIFIED))
      deleteHint(ewin->client.win, KDE_WIN_ICONIFIED);
   if (getSimpleHint(ewin->client.win, KDE_WIN_MAXIMIZED))
      deleteHint(ewin->client.win, KDE_WIN_MAXIMIZED);
   if (getSimpleHint(ewin->client.win, KDE_WIN_DESKTOP))
      deleteHint(ewin->client.win, KDE_WIN_DESKTOP);

   if (!(ewin->internal))
      KDE_SendMessagesToModules(KDE_MODULE_WIN_REMOVE, ewin->client.win);

   ewin->kde_hint = 0;

   EDBUG_RETURN_;

}

static void
KDE_AddModule(Window win)
{

   /*
    * This function will add a new module into the KModules list
    */

   KModuleList        *ptr;

   EDBUG(6, "KDE_AddModule");

   if (!win)
      EDBUG_RETURN_;

   /* create a new Module and add it to the beginning */
   ptr = Emalloc(sizeof(KModuleList));
   ptr->next = KModules;
   KModules = ptr;

   /* then tack our window in there */
   ptr->win = win;

   KDE_ClientMessage(ptr->win, KDE_MODULE_INIT, 0, CurrentTime);

   {
      if (*(getSimpleHint(win, KDE_MODULE)) == 2)
	{
	   if (mode.kde_dock)
	     {
		KModuleList        *ptr;

		mode.kde_dock = win;

		ptr = KModules;
		while (ptr)
		  {
		     KDE_ClientMessage(win, KDE_MODULE_DOCKWIN_ADD, ptr->win,
				       CurrentTime);
		     ptr = ptr->next;
		  }
	     }
	   else
	     {
		setSimpleHint(win, KDE_MODULE, 1);
	     }
	}
      /* send it a list of windows */
      {
	 EWin              **lst;
	 int                 num, i;

	 lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
	 if (lst)
	   {
	      for (i = 0; i < num; i++)
		{
		   if (!(lst[i]->internal))
		     {
			XChangeProperty(disp, lst[i]->client.win, KDE_WIN_TITLE,
					XA_STRING, 8, PropModeReplace,
					(unsigned char *)lst[i]->client.title,
					strlen(lst[i]->client.title) + 1);
			if (lst[i]->sticky)
			  {
			     if (!getSimpleHint
				 (lst[i]->client.win, KDE_WIN_STICKY))
				setSimpleHint(lst[i]->client.win,
					      KDE_WIN_STICKY, 1);
			  }
			else
			  {
			     if (getSimpleHint
				 (lst[i]->client.win, KDE_WIN_STICKY))
				setSimpleHint(lst[i]->client.win,
					      KDE_WIN_STICKY, 0);
			  }
			if (lst[i]->iconified)
			  {
			     if (!getSimpleHint
				 (lst[i]->client.win, KDE_WIN_ICONIFIED))
				setSimpleHint(lst[i]->client.win,
					      KDE_WIN_ICONIFIED, 1);
			  }
			else
			  {
			     if (getSimpleHint
				 (lst[i]->client.win, KDE_WIN_ICONIFIED))
				setSimpleHint(lst[i]->client.win,
					      KDE_WIN_ICONIFIED, 0);
			  }
			if (lst[i]->toggle)
			  {
			     if (!getSimpleHint
				 (lst[i]->client.win, KDE_WIN_MAXIMIZED))
				setSimpleHint(lst[i]->client.win,
					      KDE_WIN_MAXIMIZED, 1);
			  }
			else
			  {
			     if (getSimpleHint
				 (lst[i]->client.win, KDE_WIN_MAXIMIZED))
				setSimpleHint(lst[i]->client.win,
					      KDE_WIN_MAXIMIZED, 0);
			  }
			if (!getSimpleHint(lst[i]->client.win, KDE_WIN_DESKTOP))
			   setSimpleHint(lst[i]->client.win, KDE_WIN_DESKTOP,
					 lst[i]->desktop + 1);
			KDE_ClientMessage(win, KDE_MODULE_WIN_ADD,
					  lst[i]->client.win, CurrentTime);
			lst[i]->kde_hint = 1;
		     }
		}
	      Efree(lst);
	   }
      }

      /* send it the focused window */
      if (mode.focuswin)
	{
	   KDE_ClientMessage(ptr->win, KDE_MODULE_WIN_ACTIVATE,
			     mode.focuswin->client.win, CurrentTime);
	}
      /* identify the window manager */
      KDE_ClientTextMessage(ptr->win, KDE_COMMAND, "wm:enlightenment");

      /* tell them we're done */
      KDE_ClientMessage(ptr->win, KDE_MODULE_INITIALIZED, 0, CurrentTime);

   }

   EDBUG_RETURN_;

}

void
KDE_RemoveModule(Window win)
{

   /*
    * This function will remove a module from the KModules list.
    */

   KModuleList        *ptr, *last;

   EDBUG(6, "KDE_RemoveModule");

   if (!KModules)
     {
	EDBUG_RETURN_;
     }
   ptr = KModules;
   last = KModules;

   /* let's traverse the tree and unlink that node */
   while (ptr)
     {
	if (ptr->win == win)
	  {
	     if (ptr == KModules)
	       {
		  KModules = ptr->next;
	       }
	     else
	       {
		  last->next = ptr->next;
	       }
	     Efree(ptr);
	     break;
	  }
	if (ptr != last)
	  {
	     last = ptr;
	  }
	ptr = ptr->next;
     }

   /*
    * then we'll make sure we're not referencing a dead window as the
    * dockwin
    */

   if (win == mode.kde_dock)
     {
	mode.kde_dock = 0;
     }
   EDBUG_RETURN_;

}

void
KDE_Init(void)
{
   /*
    * In this function we initialize pretty much everything that
    * we need to initialize to make sure everyone knows we work just like
    * KWM.
    */

   EDBUG(6, "KDE_Init");

   if (!KDE_WIN_STICKY)
     {
	KDE_WIN_UNSAVED_DATA = XInternAtom(disp, "KWM_WIN_UNSAVED_DATA", False);

	KDE_WIN_DECORATION = XInternAtom(disp, "KWM_WIN_DECORATION", False);

	KDE_WIN_DESKTOP = XInternAtom(disp, "KWM_WIN_DESKTOP", False);

	KDE_WIN_GEOMETRY_RESTORE =
	   XInternAtom(disp, "KWM_WIN_GEOMETRY_RESTORE", False);

	KDE_WIN_STICKY = XInternAtom(disp, "KWM_WIN_STICKY", False);

	KDE_WIN_ICONIFIED = XInternAtom(disp, "KWM_WIN_ICONIFIED", False);

	KDE_WIN_MAXIMIZED = XInternAtom(disp, "KWM_WIN_MAXIMIZED", False);

	KDE_WIN_TITLE = XInternAtom(disp, "KWM_WIN_TITLE", False);

	KDE_WIN_ICON_GEOMETRY =
	   XInternAtom(disp, "KWM_WIN_ICON_GEOMETRY", False);

	KDE_COMMAND = XInternAtom(disp, "KWM_COMMAND", False);

	KDE_ACTIVE_WINDOW = XInternAtom(disp, "KWM_ACTIVE_WINDOW", False);

	KDE_ACTIVATE_WINDOW = XInternAtom(disp, "KWM_ACTIVATE_WINDOW", False);

	KDE_MAXIMIZE_WINDOW = XInternAtom(disp, "KWM_MAXIMIZE_WINDOW", False);

	KDE_STICKY_WINDOW = XInternAtom(disp, "KWM_KEEP_ON_TOP", False);

	KDE_DO_NOT_MANAGE = XInternAtom(disp, "KWM_DO_NOT_MANAGE", False);

	KDE_CURRENT_DESKTOP = XInternAtom(disp, "KWM_CURRENT_DESKTOP", False);

	KDE_NUMBER_OF_DESKTOPS =
	   XInternAtom(disp, "KWM_NUMBER_OF_DESKTOPS", False);

	KDE_DOCKWINDOW = XInternAtom(disp, "KWM_DOCKWINDOW", False);

	KDE_RUNNING = XInternAtom(disp, "KWM_RUNNING", False);

	KDE_MODULE = XInternAtom(disp, "KWM_MODULE", False);

	KDE_MODULE_INIT = XInternAtom(disp, "KWM_MODULE_INIT", False);
	KDE_MODULE_INITIALIZED =
	   XInternAtom(disp, "KWM_MODULE_INITIALIZED", False);

	KDE_MODULE_DESKTOP_CHANGE =
	   XInternAtom(disp, "KWM_MODULE_DESKTOP_CHANGE", False);
	KDE_MODULE_DESKTOP_NAME_CHANGE =
	   XInternAtom(disp, "KWM_MODULE_DESKTOP_NAME_CHANGE", False);
	KDE_MODULE_DESKTOP_NUMBER_CHANGE =
	   XInternAtom(disp, "KWM_MODULE_DESKTOP_NUMBER_CHANGE", False);

	KDE_MODULE_WIN_ADD = XInternAtom(disp, "KWM_MODULE_WIN_ADD", False);
	KDE_MODULE_WIN_REMOVE =
	   XInternAtom(disp, "KWM_MODULE_WIN_REMOVE", False);
	KDE_MODULE_WIN_CHANGE =
	   XInternAtom(disp, "KWM_MODULE_WIN_CHANGE", False);
	KDE_MODULE_WIN_RAISE = XInternAtom(disp, "KWM_MODULE_WIN_RAISE", False);
	KDE_MODULE_WIN_LOWER = XInternAtom(disp, "KWM_MODULE_WIN_LOWER", False);
	KDE_MODULE_WIN_ACTIVATE =
	   XInternAtom(disp, "KWM_MODULE_WIN_ACTIVATE", False);
	KDE_MODULE_WIN_ICON_CHANGE =
	   XInternAtom(disp, "KWM_MODULE_WIN_ICON_CHANGE", False);
	KDE_MODULE_DOCKWIN_ADD =
	   XInternAtom(disp, "KWM_MODULE_DOCKWIN_ADD", False);
	KDE_MODULE_DOCKWIN_REMOVE =
	   XInternAtom(disp, "KWM_MODULE_DOCKWIN_REMOVE", False);

	memset(KDE_DESKTOP_WINDOW_REGION, 0, sizeof(KDE_DESKTOP_WINDOW_REGION));

	memset(KDE_DESKTOP_NAME, 0, sizeof(KDE_DESKTOP_NAME));
     }
   KDE_SetRootArea();

   /* then we're going to set a series of string hints on the root window */
#define SETSTR(hint, str) {\
	static Atom a = 0; if (!a) a = XInternAtom(disp, #hint, False);\
		XChangeProperty(disp, root.win, a, XA_STRING, 8, PropModeReplace,\
				(unsigned char*)str, strlen(str));\
}

   SETSTR(KWM_STRING_MAXIMIZE, "Maximize");
   SETSTR(KWM_STRING_UNMAXIMIZE, "Unmaximize");
   SETSTR(KWM_STRING_ICONIFY, "Miniaturize");
   SETSTR(KWM_STRING_UNICONIFY, "Deminiaturize");
   SETSTR(KWM_STRING_STICKY, "Omnipresent");
   SETSTR(KWM_STRING_UNSTICKY, "Not Omnipresent");
   SETSTR(KWM_STRING_STRING_MOVE, "Move");
   SETSTR(KWM_STRING_STRING_RESIZE, "Resize");
   SETSTR(KWM_STRING_CLOSE, "Close");
   SETSTR(KWM_STRING_TODESKTOP, "Move To");
   SETSTR(KWM_STRING_ONTOCURRENTDESKTOP, "Bring Here");

   KDE_SetNumDesktops();

/* and we tell the root window to announce we're KDE compliant */
   setSimpleHint(root.win, KDE_RUNNING, 1);

   mode.kde_support = 1;
   EDBUG_RETURN_;

}

void
KDE_Shutdown(void)
{

   KModuleList        *ptr;

   EDBUG(6, "KDE_Shutdown");

   /* tell the root window we're not doing KDE compliance anymore */
   deleteHint(root.win, KDE_RUNNING);
   deleteHint(root.win, KDE_ACTIVE_WINDOW);
   deleteHint(root.win, KDE_NUMBER_OF_DESKTOPS);

   /* kill off the modules */
   ptr = KModules;

   while (ptr)
     {
	XKillClient(disp, ptr->win);
	ptr = ptr->next;
     }

   /* blank out the modules list */
   while (ptr)
     {
	KModules = ptr->next;
	Efree(ptr);
	ptr = KModules;
     }

   if (ptr)
      Efree(ptr);

   if (KModules)
      Efree(KModules);

   KModules = NULL;

   /* kill off all the KDE_WIN_TITLE hints */
   {

      EWin              **lst;
      int                 i, num;

      lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);

      if (lst)
	{
	   for (i = 0; i < num; i++)
	     {
		if (getSimpleHint(lst[i]->win, KDE_WIN_TITLE))
		   deleteHint(lst[i]->win, KDE_WIN_TITLE);
		if (getSimpleHint(lst[i]->win, KDE_WIN_MAXIMIZED))
		   deleteHint(lst[i]->win, KDE_WIN_MAXIMIZED);
		if (getSimpleHint(lst[i]->win, KDE_WIN_ICONIFIED))
		   deleteHint(lst[i]->win, KDE_WIN_ICONIFIED);
		if (getSimpleHint(lst[i]->win, KDE_WIN_STICKY))
		   deleteHint(lst[i]->win, KDE_WIN_STICKY);
		lst[i]->kde_hint = 0;
	     }
	   Efree(lst);
	}
   }

   mode.kde_x1 = 0;
   mode.kde_y1 = 0;
   mode.kde_x2 = root.w;
   mode.kde_y2 = root.h;
   mode.kde_dock = 0;
   mode.kde_support = 0;
   EDBUG_RETURN_;

}

static void
KDE_GetDecorationHint(Window win, long *dechints)
{

   Border             *b = NULL;
   EWin               *ewin;

   EDBUG(6, "KDE_GetDecorationHint");

   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);

   if (!ewin)
      EDBUG_RETURN_;

   if (*dechints & KDE_NO_FOCUS)
      ewin->skipfocus = 1;

   switch (*dechints & ~KDE_NO_FOCUS)
     {
     case KDE_NO_DECORATION:
	b = (Border *) FindItem("BORDERLESS", 0, LIST_FINDBY_NAME,
				LIST_TYPE_BORDER);
	break;
     case KDE_TINY_DECORATION:
	b = (Border *) FindItem("TRANSIENT", 0, LIST_FINDBY_NAME,
				LIST_TYPE_BORDER);
	break;
     case KDE_STANDALONE_MENUBAR:
	b = (Border *) FindItem("BORDERLESS", 0, LIST_FINDBY_NAME,
				LIST_TYPE_BORDER);
	ewin->skipfocus = 1;
	break;
     case KDE_DESKTOP_ICON:
	b = (Border *) FindItem("BORDERLESS", 0, LIST_FINDBY_NAME,
				LIST_TYPE_BORDER);
	ewin->skipfocus = 1;
	ewin->sticky = 1;
	ewin->layer = 1;
	break;
     case KDE_ONTOP:
	ewin->layer = 11;
	break;
     case KDE_NORMAL_DECORATION:
     default:
	b = (Border *) FindItem("DEFAULT", 0, LIST_FINDBY_NAME,
				LIST_TYPE_BORDER);
	break;

     }
   if (!b)
     {
	b = (Border *) FindItem("DEFAULT", 0, LIST_FINDBY_NAME,
				LIST_TYPE_BORDER);
	if (!b)
	  {
	     b = (Border *) FindItem("__FALLBACK_BORDER", 0, LIST_FINDBY_NAME,
				     LIST_TYPE_BORDER);

	  }
     }
   ewin->border_new = 1;
   SetEwinToBorder(ewin, b);
   ICCCM_MatchSize(ewin);
   MoveResizeEwin(ewin, ewin->x, ewin->y, ewin->client.w, ewin->client.h);

   EDBUG_RETURN_;

}

void
KDE_ClientChange(Window win, Atom a)
{

   EWin               *ewin;

   EDBUG(6, "KDE_ClientChange");

   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);

   if (!ewin)
      EDBUG_RETURN_;

   if (a == KDE_WIN_STICKY)
     {
	if (getSimpleHint(win, KDE_WIN_STICKY))
	  {
	     if (*(getSimpleHint(win, KDE_WIN_STICKY)))
	       {
		  if (!(ewin->sticky))
		     MakeWindowSticky(ewin);
	       }
	     else
	       {
		  if (ewin->sticky)
		     MakeWindowUnSticky(ewin);
	       }
	  }
     }
   else if (a == KDE_WIN_MAXIMIZED)
     {
	if (getSimpleHint(win, KDE_WIN_MAXIMIZED))
	  {
	     if (*(getSimpleHint(ewin->client.win, KDE_WIN_MAXIMIZED)))
	       {
		  if (!(ewin->toggle))
		    {
		       MaxSize(ewin, "conservative");
		    }
	       }
	     else
	       {
		  if (ewin->toggle)
		    {
		       MaxSize(ewin, "conservative");
		    }
	       }
	  }
     }
   else if (a == KDE_WIN_ICONIFIED)
     {
	if (getSimpleHint(win, KDE_WIN_MAXIMIZED))
	  {
	     if (*(getSimpleHint(ewin->client.win, KDE_WIN_ICONIFIED)))
	       {
		  if (!ewin->iconified)
		    {
		       IconifyEwin(ewin);
		    }
	       }
	     else
	       {
		  if (ewin->iconified)
		    {
		       DeIconifyEwin(ewin);
		    }
	       }
	  }
     }
   else if (a == KDE_WIN_DECORATION)
     {
	if (getSimpleHint(win, KDE_WIN_DECORATION))
	   KDE_GetDecorationHint(win, getSimpleHint(win, KDE_WIN_DECORATION));
     }
   else if (a == KDE_WIN_DESKTOP)
     {
	if (getSimpleHint(win, KDE_WIN_DESKTOP))
	  {
	     long                desktop;

	     desktop = *(getSimpleHint(win, KDE_WIN_DESKTOP)) - 1;
	     if (ewin->desktop != desktop)
	       {
		  MoveEwinToDesktop(ewin, desktop);

	       }
	  }
     }
   /* we currently do not support the GEOMETRY RESTORE HINT */
   EDBUG_RETURN_;

}

static int
KDE_WindowCommand(EWin * ewin, char *cmd)
{

   EDBUG(6, "KDE_WindowCommand");

   if (!ewin)
      EDBUG_RETURN(0);

   if (!cmd)
      EDBUG_RETURN(0);

   if (!strcmp(cmd, "winMove"))
     {
	/* this isn't supported right now */
     }
   else if (!strcmp(cmd, "winResize"))
     {
	/* this isn't supported right now */
     }
   else if (!strcmp(cmd, "winRestore"))
     {
	MaxSize(ewin, "conservative");
     }
   else if (!strcmp(cmd, "winMaximize"))
     {
	MaxSize(ewin, "conservative");
     }
   else if (!strcmp(cmd, "winIconify"))
     {
	IconifyEwin(ewin);
     }
   else if (!strcmp(cmd, "winClose"))
     {
	KillEwin(ewin);
     }
   else if (!strcmp(cmd, "winSticky"))
     {
	MakeWindowSticky(ewin);
     }
   else if (!strcmp(cmd, "winShade"))
     {
	ShadeEwin(ewin);
     }
   else if (!strcmp(cmd, "winOperation"))
     {
	/* this isn't supported right now */
     }
   else
     {
	EDBUG_RETURN(0);
     }
   EDBUG_RETURN(1);

}

static void
KDE_Command(char *cmd, XClientMessageEvent * event)
{

   EDBUG(6, "KDE_Command");

   if (!strcmp(cmd, "commandLine"))
     {
	/* not supported right now */
     }
   else if (!strcmp(cmd, "execute"))
     {
	/* not supported right now */
     }
   else if (!strcmp(cmd, "logout"))
     {
	doExit("logout");
     }
   else if (!strcmp(cmd, "refreshScreen"))
     {
	RefreshScreen();
     }
   else if (!strncmp(cmd, "go:", 3))
     {
	/* not supported right now */
     }
   else if (!strcmp(cmd, "desktop+1"))
     {
	GotoDesktop(desks.current + 1);
     }
   else if (!strcmp(cmd, "desktop-1"))
     {
	GotoDesktop(desks.current - 1);
     }
   else if (!strcmp(cmd, "desktop+2"))
     {
	GotoDesktop(desks.current + 2);
     }
   else if (!strcmp(cmd, "desktop-2"))
     {
	GotoDesktop(desks.current - 2);
     }
   else if (!strncmp(cmd, "desktop", 7))
     {
	GotoDesktop(atoi(&cmd[7]));
     }
   else if (!strcmp(cmd, "deskUnclutter"))
     {
	doCleanup(NULL);
     }
   else if (!KDE_WindowCommand(GetFocusEwin(), cmd))
     {
	XEvent              ev;
	KModuleList        *ptr = KModules;
	long                mask;

	ev.xclient = *event;

	while (ptr)
	  {
	     ev.xclient.window = ptr->win;
	     if (ptr->win == root.win)
	       {
		  mask = SubstructureRedirectMask;
	       }
	     else
	       {
		  mask = 0;
	       }

	     XSendEvent(disp, ptr->win, False, mask, &ev);

	     ptr = ptr->next;
	  }
     }
   EDBUG_RETURN_;

}

void
KDE_ProcessClientMessage(XClientMessageEvent * event)
{

   EWin               *ewin;

   EDBUG(6, "KDE_ProcessClientMessage");

   ewin = FindItem(NULL, event->data.l[0], LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (event->message_type == KDE_COMMAND && event->format == 8)
     {
	char                s[24];

	strcpy(s, event->data.b);
	KDE_Command(s, event);
     }
   else if (event->message_type == KDE_ACTIVATE_WINDOW)
     {
	FocusToEWin(ewin);
	KDE_SetRootArea();
     }
   else if (event->message_type == KDE_MAXIMIZE_WINDOW)
     {
	MaxSize(ewin, "conservative");
     }
   else if (event->message_type == KDE_STICKY_WINDOW)
     {
     }
   else if (event->message_type == KDE_DO_NOT_MANAGE)
     {
	Border             *b;

	if (ewin)
	  {
	     b = (Border *) FindItem("BORDERLESS", 0, LIST_FINDBY_NAME,
				     LIST_TYPE_BORDER);

	     MakeWindowSticky(ewin);
	     ewin->border_new = 1;
	     SetEwinToBorder(ewin, b);
	     ICCCM_MatchSize(ewin);
	     MoveResizeEwin(ewin, ewin->x, ewin->y, ewin->client.w,
			    ewin->client.h);
	  }
     }
   else if (event->message_type == KDE_MODULE)
     {
	if (*(getSimpleHint(event->data.l[0], KDE_MODULE)))
	  {
	     KDE_AddModule(event->data.l[0]);
	  }
	else
	  {
	     KDE_RemoveModule(event->data.l[0]);
	  }
     }
   else
     {
	/* not supported right now */
     }

   EDBUG_RETURN_;

}

void
KDE_SetRootArea(void)
{

   EDBUG(6, "KDE_SetRootArea");

   if (getSimpleHint(root.win, KDE_CURRENT_DESKTOP))
      if (*(getSimpleHint(root.win, KDE_CURRENT_DESKTOP)) == desks.current + 1)
	 EDBUG_RETURN_;

   setSimpleHint(root.win, KDE_CURRENT_DESKTOP, desks.current + 1);

   KDE_SendMessagesToModules(KDE_MODULE_DESKTOP_CHANGE, desks.current + 1);

   EDBUG_RETURN_;

}

void
KDE_SetNumDesktops(void)
{

   char                s[32];
   int                 i;

   EDBUG(6, "KDE_SetRootArea");

   if (!(getSimpleHint(root.win, KDE_NUMBER_OF_DESKTOPS))
       || !(*(getSimpleHint(root.win, KDE_NUMBER_OF_DESKTOPS)) ==
	    mode.numdesktops))
     {
	setSimpleHint(root.win, KDE_NUMBER_OF_DESKTOPS, mode.numdesktops);
	KDE_SendMessagesToModules(KDE_MODULE_DESKTOP_NUMBER_CHANGE,
				  mode.numdesktops);
     }
   for (i = 0; i < mode.numdesktops; i++)
     {

	if (!KDE_DESKTOP_NAME[i])
	  {
	     Esnprintf(s, sizeof(s), "KWM_DESKTOP_NAME_%d", i + 1);
	     KDE_DESKTOP_NAME[i] = XInternAtom(disp, s, False);
	  }
	if (!getSimpleHint(root.win, KDE_DESKTOP_NAME[i]))
	  {
	     Esnprintf(s, sizeof(s), "Desk %d", i);
	     XChangeProperty(disp, root.win, KDE_DESKTOP_NAME[i], XA_STRING, 8,
			     PropModeReplace, (unsigned char *)s,
			     strlen(s) + 1);
	  }
     }

   EDBUG_RETURN_;
}

void
KDE_HintChange(Atom a)
{

   EDBUG(6, "KDE_HintChange");

   if (a == KDE_CURRENT_DESKTOP)
     {
	GotoDesktop((*(getSimpleHint(root.win, KDE_CURRENT_DESKTOP)) - 1));
     }
   else if (a == KDE_NUMBER_OF_DESKTOPS)
     {
	ChangeNumberOfDesktops(*
			       (getSimpleHint
				(root.win, KDE_NUMBER_OF_DESKTOPS)));
     }
   else
     {
	int                 i;

	for (i = 0; i < 32; i++)
	  {
	     char                s[FILEPATH_LEN_MAX];

	     if (!KDE_DESKTOP_WINDOW_REGION[i])
	       {
		  Esnprintf(s, sizeof(s), "KWM_WINDOW_REGION_%d", i + 1);
		  KDE_DESKTOP_WINDOW_REGION[i] = XInternAtom(disp, s, False);
	       }
	     if (a == KDE_DESKTOP_WINDOW_REGION[i])
	       {
		  Atom                type_ret;
		  int                 fmt_ret;
		  unsigned long       num;
		  unsigned long       extra;
		  unsigned long      *data;

		  if (XGetWindowProperty
		      (disp, root.win, a, 0, 4, False, a, &type_ret, &fmt_ret,
		       &num, &extra, (unsigned char **)&data) != Success
		      || !data)
		    {
		       EDBUG_RETURN_;
		    }
		  else
		    {
		       if (a != type_ret)
			 {
			    XFree(data);
			    EDBUG_RETURN_;
			 }
		       if (i == 1)
			 {
			    /* Esnprintf(s, sizeof(s), "region %d changed to area:\n"
			     * "X %d to %d\nY to %d to %d\n", i + 1, data[0], data[2],
			     * data[1], data[3]);
			     * DIALOG_OK("kde hint",s); */
			    mode.kde_x1 = data[0];
			    mode.kde_x2 = data[2];
			    mode.kde_y1 = data[1];
			    mode.kde_y2 = data[3];

			 }
		       XFree(data);
		    }
	       }
	  }
	/* or we're not supported right now */
     }

   EDBUG_RETURN_;

}

void
KDE_UpdateClient(EWin * ewin)
{

   EDBUG(6, "KDE_UpdateClient");

   if (!ewin)
      EDBUG_RETURN_;

   if (!ewin->internal)
     {
	XChangeProperty(disp, ewin->client.win, KDE_WIN_TITLE, XA_STRING, 8,
			PropModeReplace, (unsigned char *)ewin->client.title,
			strlen(ewin->client.title) + 1);
     }
   if (getSimpleHint(ewin->client.win, KDE_WIN_STICKY))
     {
	if (ewin->sticky)
	  {
	     if (!*(getSimpleHint(ewin->client.win, KDE_WIN_STICKY)))
		setSimpleHint(ewin->client.win, KDE_WIN_STICKY, 1);
	  }
	else
	  {
	     if (*(getSimpleHint(ewin->client.win, KDE_WIN_STICKY)))
		setSimpleHint(ewin->client.win, KDE_WIN_STICKY, 0);
	  }
     }
   else
     {
	setSimpleHint(ewin->client.win, KDE_WIN_STICKY, ewin->sticky ? 1 : 0);
     }

   if (getSimpleHint(ewin->client.win, KDE_WIN_ICONIFIED))
     {
	if (ewin->iconified)
	  {
	     if (!*(getSimpleHint(ewin->client.win, KDE_WIN_ICONIFIED)))
		setSimpleHint(ewin->client.win, KDE_WIN_ICONIFIED, 1);
	  }
	else
	  {
	     if (*(getSimpleHint(ewin->client.win, KDE_WIN_ICONIFIED)))
		setSimpleHint(ewin->client.win, KDE_WIN_ICONIFIED, 0);
	  }
     }
   else
     {
	setSimpleHint(ewin->client.win, KDE_WIN_ICONIFIED,
		      ewin->iconified ? 1 : 0);
     }
   if (getSimpleHint(ewin->client.win, KDE_WIN_MAXIMIZED))
     {
	if (ewin->toggle)
	  {
	     if (!*(getSimpleHint(ewin->client.win, KDE_WIN_MAXIMIZED)))
		setSimpleHint(ewin->client.win, KDE_WIN_MAXIMIZED, 1);
	  }
	else
	  {
	     if (*(getSimpleHint(ewin->client.win, KDE_WIN_MAXIMIZED)))
		setSimpleHint(ewin->client.win, KDE_WIN_MAXIMIZED, 0);
	  }
     }
   else
     {
	setSimpleHint(ewin->client.win, KDE_WIN_MAXIMIZED,
		      ewin->toggle ? 1 : 0);
     }
   if (getSimpleHint(ewin->client.win, KDE_WIN_DESKTOP))
     {
	if ((*(getSimpleHint(ewin->client.win, KDE_WIN_DESKTOP)) - 1) !=
	    ewin->desktop)
	   setSimpleHint(ewin->client.win, KDE_WIN_DESKTOP,
			 (ewin->desktop) + 1);
     }
   else
     {
	setSimpleHint(ewin->client.win, KDE_WIN_DESKTOP, (ewin->desktop) + 1);
     }

   if (!(ewin->internal))
      KDE_SendMessagesToModules(KDE_MODULE_WIN_CHANGE, ewin->client.win);

   EDBUG_RETURN_;

}

#endif
