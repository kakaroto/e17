
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

   /*
    * This function will add a new module into the KModules list
    */

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
   /*
    * In this function we initialize pretty much everything that
    * we need to initialize to make sure everyone knows we work just like
    * KWM.
    */

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
   /* and we tell the root window to announce we're KDE compliant */
   setSimpleHint(root.win, KDE_RUNNING, 1);

   EDBUG_RETURN_;

}

void
KDE_Shutdown(void)
{

   EDBUG(6, "KDE_Shutdown");

   /* tell the root window we're not doing KDE compliance anymore */
   deleteHint(root.win, KDE_RUNNING);

   EDBUG_RETURN_;
}

void
KDE_ClientInit(EWin * ewin)
{

   EDBUG(6, "KDE_ClientInit");

   /* grab everything from the Client about KStuffs */
   if (getSimpleHint(ewin->win, KDE_WIN_STICKY))
     {
	MakeWindowSticky(ewin);
     }
   if (getSimpleHint(ewin->win, KDE_WIN_ICONIFIED))
     {
	IconifyEwin(ewin);
     }
   if (getSimpleHint(ewin->win, KDE_WIN_MAXIMIZED))
     {
	MaxSize(ewin, "conservative");
     }
   if (getSimpleHint(ewin->win, KDE_WIN_DECORATION))
     {
	KDE_GetDecorationHint(ewin, getSimpleHint(ewin->win,
						  KDE_WIN_DECORATION));
     }
   /* we currently do not support the GEOMETRY RESTORE HINT */

   EDBUG_RETURN_;

}

void
KDE_ClientChange(EWin * ewin, XPropertyEvent * event)
{

   EDBUG(6, "KDE_ClientChange");

   if (event->atom == KDE_WIN_STICKY)
     {
	if (getSimpleHint(ewin->win, KDE_WIN_STICKY))
	  {
	     MakeWindowSticky(ewin);
	  }
	else
	  {
	     MakeWindowUnSticky(ewin);
	  }
     }
   else if (event->atom == KDE_WIN_MAXIMIZED)
     {
	if (getSimpleHint(ewin->win, KDE_WIN_MAXIMIZED))
	  {
	     ewin->toggle = 0;
	     MaxSize(ewin, "conservative");
	  }
	else
	  {
	     ewin->toggle = 1;
	     MaxSize(ewin, "conservative");
	  }
     }
   else if (event->atom == KDE_WIN_ICONIFIED)
     {
	if (getSimpleHint(ewin->win, KDE_WIN_ICONIFIED))
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
   else if (event->atom == KDE_WIN_DECORATION)
     {
	KDE_GetDecorationHint(ewin, getSimpleHint(ewin->win,
						  KDE_WIN_DECORATION));
     }
   else if (event->atom == KDE_WIN_DESKTOP)
     {
	if (getSimpleHint(ewin->win, KDE_WIN_DESKTOP))
	  {
	     long               *desktop;

	     desktop = getSimpleHint(ewin->win, KDE_WIN_DESKTOP) - 1;
	     if (ewin->desktop != *desktop)
	       {
		  MoveEwinToDesktop(ewin, *desktop);

	       }
	  }
     }
   /* we currently do not support the GEOMETRY RESTORE HINT */
   EDBUG_RETURN_;

}

void
KDE_GetDecorationHint(EWin * ewin, long *dechints)
{

   Border             *b = NULL;

   EDBUG(6, "KDE_GetDecorationHint");

   ewin->skipfocus = *dechints & _KDE_NO_FOCUS;

   switch (*dechints & ~_KDE_NO_FOCUS)
     {
     case _KDE_NO_DECORATION:
	b = (Border *) FindItem("BORDERLESS", 0, LIST_FINDBY_NAME,
				LIST_TYPE_BORDER);
	break;
     case _KDE_TINY_DECORATION:
	b = (Border *) FindItem("TRANSIENT", 0, LIST_FINDBY_NAME,
				LIST_TYPE_BORDER);
	break;
     case _KDE_NORMAL_DECORATION:
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
   MoveResizeEwin(ewin, ewin->x, ewin->y, ewin->client.w,
		  ewin->client.h);

   EDBUG_RETURN_;

}

int
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

void
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

     }
   else if (!strcmp(cmd, "desktop+1"))
     {

     }
   else if (!strcmp(cmd, "desktop-1"))
     {

     }
   else if (!strcmp(cmd, "desktop+2"))
     {

     }
   else if (!strcmp(cmd, "desktop-2"))
     {

     }
   else if (!strncmp(cmd, "desktop", 7))
     {

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
	     ev.xclient.window = ptr->ewin->win;
	     if (ptr->ewin->win == root.win)
	       {
		  mask = SubstructureRedirectMask;
	       }
	     else
	       {
		  mask = 0;
	       }

	     XSendEvent(disp, ptr->ewin->win, False, mask, &ev);

	     ptr = ptr->next;
	  }
     }

   EDBUG_RETURN_;

}
