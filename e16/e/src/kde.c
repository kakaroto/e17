
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

#if 0 /* we dont need these right now */
static Atom         KDE_COMMAND = 0;
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
#endif

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
