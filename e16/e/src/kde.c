
#include "E.h"

/* This code is copyright 1999 The Enlightenment Development Team */
/* based on code by Matthias Ettrich from KWM (http://www.kde.org)
 * and code by Alfredo Kojima in Window Maker (http://www.windowmaker.org)
 * both of which are copyright under the GPL
 *
 * To anyone who hasn't implemented KDE hints in your window manager,
 * feel free to snarf this code and make it work for you, too.
 *
 * thanks to everyone who helped me get this working
 * --Mandrake
 */

/* some #defines to make this a little more legible later */

#define _KDE_NO_DECORATION 0
#define _KDE_NORMAL_DECORATION 1
#define _KDE_TINY_DECORATION 2
#define _KDE_NO_FOCUS 256
#define _KDE_STANDALONE_MENUBAR 512
#define _KDE_DESKTOP_ICON 1024
#define _KDE_ONTOP 2048

#define KWM_STICKY 1<<0
#define KWM_MAXIMIZED 1<<1
#define KWM_ICONIFIED 1<<2
#define KWM_ALL 7

/* initialize all the KDE Hint Atoms */

																								  /*#if 0 *//* we dont need these right now */
static Atom         KDE_COMMAND = 0;
static Atom         KDE_ACTIVE_WINDOW = 0;
static Atom         KDE_ACTIVATE_WINDOW = 0;
static Atom         KDE_DO_NOT_MANAGE = 0;
static Atom         KDE_DOCKWINDOW = 0;
static Atom         KDE_RUNNING = 0;

static Atom         KDE_CURRENT_DESKTOP = 0;
static Atom         KDE_NUMBER_OF_DESKTOPS = 0;
static Atom         KDE_DESKTOP_NAME[ENLIGHTENMENT_CONF_NUM_DESKTOPS];
static Atom         KDE_DESKTOP_WINDOW_REGION[ENLIGHTENMENT_CONF_NUM_DESKTOPS];

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

/* #endif */

/* the modules I have to communicate to */
typedef struct KModuleList
  {

     EWin               *ewin;
     struct KModuleList *next;

  }
KModuleList;

static KModuleList *KModules = NULL;

void
KDE_ClientMessage(EWin * ewin, Atom atom, long data, Time timestamp)
{

   XEvent              ev;
   long                mask = 0;

   EDBUG(6, "KDE_ClientMessage");

   memset(&ev, 0, sizeof(XEvent));
   ev.xclient.window = ewin->win;
   ev.xclient.message_type = atom;
   ev.xclient.type = ClientMessage;
   ev.xclient.format = 32;
   ev.xclient.data.l[0] = data;
   ev.xclient.data.l[1] = timestamp;

   XSendEvent(disp, ewin->win, False, mask, &ev);

   EDBUG_RETURN_;

}

void
KDE_ClientTextMessage(EWin * ewin, Atom atom, char *data)
{

   XEvent              ev;
   long                mask = 0;

   EDBUG(6, "KDE_ClientTextMessage");

   memset(&ev, 0, sizeof(XEvent));
   ev.xclient.window = ewin->win;
   ev.xclient.message_type = atom;
   ev.xclient.type = ClientMessage;
   ev.xclient.format = 32;
   strcpy(ev.xclient.data.b, data);

   XSendEvent(disp, ewin->win, False, mask, &ev);

   EDBUG_RETURN_;

}

void
KDE_SendMessagesToModules(Atom atom, long data)
{

   KModuleList        *ptr;

   EDBUG(6, "KDE_SendMessagesToModules");

   ptr = KModules;
   while (ptr)
     {
	KDE_ClientMessage(ptr->ewin, atom, data, CurrentTime);
	ptr = ptr->next;
     }

   EDBUG_RETURN_;

}

void
KDE_AddModule(EWin * ewin)
{

   KModuleList        *ptr;

   EDBUG(6, "KDE_AddModule");

   /* create a new Module and add it to the beginning */
   ptr = Emalloc(sizeof(KModuleList));
   ptr->next = KModules;
   KModules = ptr;

   /* then tack our window in there */
   ptr->ewin = ewin;

   KDE_ClientMessage(ptr->ewin, KDE_MODULE_INIT, 0, CurrentTime);

   {
      if (*(getSimpleHint(ewin->win, KDE_MODULE)) == 2)
	{
	   if (mode.kde_dock)
	     {
		KModuleList        *ptr;

		mode.kde_dock = ewin;

		ptr = KModules;
		while (ptr)
		  {
		     KDE_ClientMessage(ewin, KDE_MODULE_DOCKWIN_ADD,
				       ptr->ewin->win, CurrentTime);
		     ptr = ptr->next;
		  }
	     }
	   else
	     {
		setSimpleHint(ewin->win, KDE_MODULE, 1);
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
		   KDE_ClientMessage(ewin, KDE_MODULE_WIN_ADD, lst[i]->win,
				     CurrentTime);
		}
	      Efree(lst);
	   }
      }

      /* send it the focused window */
      if (mode.focuswin)
	{
	   KDE_ClientMessage(ptr->ewin, KDE_MODULE_WIN_ACTIVATE,
			     mode.focuswin->win, CurrentTime);
	}
      /* identify the window manager */
      KDE_ClientTextMessage(ptr->ewin, KDE_COMMAND, "wm:enlightenment");

      /* tell them we're done */
      KDE_ClientMessage(ptr->ewin, KDE_MODULE_INITIALIZED, 0, CurrentTime);

   }

   EDBUG_RETURN_;

}

void
KDE_RemoveModule(EWin * ewin)
{

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
	if (ptr->ewin == ewin)
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

   if (ewin == mode.kde_dock)
     {
	mode.kde_dock = NULL;
     }
   EDBUG_RETURN_;

}

void
KDE_Init(void)
{

   EDBUG(6, "KDE_Init");

   if (!KDE_WIN_STICKY)
     {
	KDE_WIN_UNSAVED_DATA = XInternAtom(disp, "KWM_WIN_UNSAVED_DATA",
					   False);

	KDE_WIN_DECORATION = XInternAtom(disp, "KWM_WIN_DECORATION", False);

	KDE_WIN_DESKTOP = XInternAtom(disp, "KWM_WIN_DESKTOP", False);

	KDE_WIN_GEOMETRY_RESTORE = XInternAtom(disp,
					       "KWM_WIN_GEOMETRY_RESTORE",
					       False);

	KDE_WIN_STICKY = XInternAtom(disp, "KWM_WIN_STICKY", False);

	KDE_WIN_ICONIFIED = XInternAtom(disp, "KWM_WIN_ICONIFIED", False);

	KDE_WIN_MAXIMIZED = XInternAtom(disp, "KWM_WIN_MAXIMIZED", False);

	KDE_WIN_ICON_GEOMETRY = XInternAtom(disp, "KWM_WIN_ICON_GEOMETRY",
					    False);

	KDE_COMMAND = XInternAtom(disp, "KWM_COMMAND", False);

	KDE_ACTIVE_WINDOW = XInternAtom(disp, "KWM_ACTIVE_WINDOW", False);

	KDE_ACTIVATE_WINDOW = XInternAtom(disp, "KWM_ACTIVATE_WINDOW",
					  False);

	KDE_DO_NOT_MANAGE = XInternAtom(disp, "KWM_DO_NOT_MANAGE", False);

	KDE_CURRENT_DESKTOP = XInternAtom(disp, "KWM_CURRENT_DESKTOP",
					  False);

	KDE_NUMBER_OF_DESKTOPS = XInternAtom(disp, "KWM_NUMBER_OF_DESKTOPS",
					     False);

	KDE_DOCKWINDOW = XInternAtom(disp, "KWM_DOCKWINDOW", False);

	KDE_RUNNING = XInternAtom(disp, "KWM_RUNNING", False);

	KDE_MODULE = XInternAtom(disp, "KWM_MODULE", False);

	KDE_MODULE_INIT = XInternAtom(disp, "KWM_MODULE_INIT", False);
	KDE_MODULE_INITIALIZED = XInternAtom(disp, "KWM_MODULE_INITIALIZED",
					     False);

	KDE_MODULE_DESKTOP_CHANGE = XInternAtom(disp,
					  "KWM_MODULE_DESKTOP_CHANGE", False);
	KDE_MODULE_DESKTOP_NAME_CHANGE = XInternAtom(disp,
				     "KWM_MODULE_DESKTOP_NAME_CHANGE", False);
	KDE_MODULE_DESKTOP_NUMBER_CHANGE = XInternAtom(disp,
				   "KWM_MODULE_DESKTOP_NUMBER_CHANGE", False);

	KDE_MODULE_WIN_ADD = XInternAtom(disp, "KWM_MODULE_WIN_ADD", False);
	KDE_MODULE_WIN_REMOVE = XInternAtom(disp, "KWM_MODULE_WIN_REMOVE",
					    False);
	KDE_MODULE_WIN_CHANGE = XInternAtom(disp, "KWM_MODULE_WIN_CHANGE",
					    False);
	KDE_MODULE_WIN_RAISE = XInternAtom(disp, "KWM_MODULE_WIN_RAISE", False);
	KDE_MODULE_WIN_LOWER = XInternAtom(disp, "KWM_MODULE_WIN_LOWER", False);
	KDE_MODULE_WIN_ACTIVATE = XInternAtom(disp, "KWM_MODULE_WIN_ACTIVATE",
					      False);
	KDE_MODULE_WIN_ICON_CHANGE = XInternAtom(disp,
					 "KWM_MODULE_WIN_ICON_CHANGE", False);
	KDE_MODULE_DOCKWIN_ADD = XInternAtom(disp, "KWM_MODULE_DOCKWIN_ADD",
					     False);
	KDE_MODULE_DOCKWIN_REMOVE = XInternAtom(disp,
					  "KWM_MODULE_DOCKWIN_REMOVE", False);

	memset(KDE_DESKTOP_WINDOW_REGION, 0, sizeof(KDE_DESKTOP_WINDOW_REGION));

	memset(KDE_DESKTOP_NAME, 0, sizeof(KDE_DESKTOP_NAME));
     }

   setSimpleHint(root.win, KDE_RUNNING, 1);

   EDBUG_RETURN_;

}

void
KDE_Shutdown(void)
{

   EDBUG(6, "KDE_Shutdown");

   deleteHint(root.win, KDE_RUNNING);

   EDBUG_RETURN_;
}
