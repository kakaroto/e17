/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "timestamp.h"
#ifdef __EMX__
#include <process.h>
#endif

typedef struct
{
   char                need_ewin;
   char                ok_zoom;
   char                ok_movres;
   char                hide_slideouts;
   int                 (*func) (EWin * ewin, void *params);
} ActionFunction;

static ActionFunction ActionFunctions[ACTION_NUMBEROF];
static char         mode_action_destroy = 0;

ActionClass        *
CreateAclass(char *name)
{
   ActionClass        *a;

   EDBUG(5, "CreateAclass");
   a = Emalloc(sizeof(ActionClass));
   a->name = duplicate(name);
   a->num = 0;
   a->list = NULL;
   a->tooltipstring = NULL;
   a->ref_count = 0;
   EDBUG_RETURN(a);
}

void
RefreshScreen(void)
{

   Window              win;
   XSetWindowAttributes attr;

   EDBUG(4, "CreateAclass");

   attr.save_under = False;
   attr.backing_store = NotUseful;

   win =
      XCreateWindow(disp, root.win, 0, 0, root.w, root.h, 0, CopyFromParent,
		    CopyFromParent, CopyFromParent,
		    CWBackingStore | CWSaveUnder, &attr);
   XMapRaised(disp, win);
   XDestroyWindow(disp, win);
   XFlush(disp);

   EDBUG_RETURN_;
}

void
GrabButtonGrabs(EWin * ewin)
{
   ActionClass        *ac;
   int                 j;
   Action             *a;
   Window              pager_hi_win;

   ac = (ActionClass *) FindItem("BUTTONBINDINGS", 0, LIST_FINDBY_NAME,
				 LIST_TYPE_ACLASS);

   if (ac)
     {
	pager_hi_win = PagerGetHiWin(ewin->pager);
	ac->ref_count++;
	for (j = 0; j < ac->num; j++)
	  {
	     a = ac->list[j];
	     if ((a)
		 && ((a->event == EVENT_MOUSE_DOWN)
		     || (a->event == EVENT_MOUSE_UP)))
	       {
		  unsigned int        mod, button, mask;

		  mod = 0;
		  button = 0;
		  if (a->anymodifier)
		     mod = AnyModifier;
		  else
		     mod = a->modifiers;
		  if (a->anybutton)
		     button = AnyButton;
		  else
		     button = a->button;
		  mask = ButtonPressMask | ButtonReleaseMask;
		  if (mod == AnyModifier)
		    {
#if 0				/* Why? */
		       if (pager_hi_win)
			  XGrabButton(disp, button, mod, pager_hi_win,
				      False, mask, GrabModeSync, GrabModeAsync,
				      None, None);
#endif
		       XGrabButton(disp, button, mod, ewin->win, False, mask,
				   GrabModeSync, GrabModeAsync, None, None);
		    }
		  else
		    {
		       int                 i;

		       for (i = 0; i < 8; i++)
			 {
#if 0				/* Why? */
			    if (pager_hi_win)
			       XGrabButton(disp, button,
					   mod | mask_mod_combos[i],
					   pager_hi_win, False, mask,
					   GrabModeSync, GrabModeAsync, None,
					   None);
#endif
			    XGrabButton(disp, button, mod | mask_mod_combos[i],
					ewin->win, False, mask, GrabModeSync,
					GrabModeAsync, None, None);
			 }
		    }
	       }
	  }
     }
}

void
UnGrabButtonGrabs(EWin * ewin)
{
   ActionClass        *ac;
   int                 j;
   Action             *a;
   Window              pager_hi_win;

   ac = (ActionClass *) FindItem("BUTTONBINDINGS", 0, LIST_FINDBY_NAME,
				 LIST_TYPE_ACLASS);

   if (ac)
     {
	pager_hi_win = PagerGetHiWin(ewin->pager);
	ac->ref_count--;
	for (j = 0; j < ac->num; j++)
	  {
	     a = ac->list[j];
	     if ((a)
		 && ((a->event == EVENT_MOUSE_DOWN)
		     || (a->event == EVENT_MOUSE_UP)))
	       {
		  unsigned int        mod, button;

		  mod = 0;
		  button = 0;
		  if (a->anymodifier)
		     mod = AnyModifier;
		  else
		     mod = a->modifiers;
		  if (a->anybutton)
		     button = AnyButton;
		  else
		     button = a->button;
		  if (mod == AnyModifier)
		    {
		       if (pager_hi_win)
			  XUngrabButton(disp, button, mod, pager_hi_win);
		       XUngrabButton(disp, button, mod, ewin->win);
		    }
		  else
		    {
		       int                 i;

		       for (i = 0; i < 8; i++)
			 {
			    if (pager_hi_win)
			       XUngrabButton(disp, button,
					     mod | mask_mod_combos[i],
					     pager_hi_win);
			    XUngrabButton(disp, button,
					  mod | mask_mod_combos[i], ewin->win);
			 }
		    }
	       }
	  }
     }
}

Action             *
CreateAction(char event, char anymod, int mod, int anybut, int but,
	     char anykey, char *key, char *tooltipstring)
{
   Action             *act;

   EDBUG(5, "CreateAction");

   act = Emalloc(sizeof(Action));
   act->action = NULL;
   act->event = event;
   act->anymodifier = anymod;
   act->modifiers = mod;
   act->anybutton = anybut;
   act->button = but;
   act->anykey = anykey;
   if (!key)
      act->key = 0;
   else
      act->key = XKeysymToKeycode(disp, XStringToKeysym(key));
   act->key_str = duplicate(key);
   if (tooltipstring)
      act->tooltipstring = duplicate(tooltipstring);
   else
      act->tooltipstring = NULL;

   EDBUG_RETURN(act);
}

static void
RemoveActionType(ActionType * ActionTypeToRemove)
{
   ActionType         *ptr, *pp;

   EDBUG(5, "RemoveActionType");

   ptr = ActionTypeToRemove;
   while (ptr)
     {
	if (ptr->params)
	   Efree(ptr->params);
	pp = ptr;
	ptr = ptr->Next;
	Efree(pp);
     }

   EDBUG_RETURN_;
}

static void
RemoveAction(Action * ActionToRemove)
{
   EDBUG(5, "RemoveAction");

   if (!ActionToRemove)
      EDBUG_RETURN_;

   if ((ActionToRemove->event == EVENT_KEY_DOWN)
       || (ActionToRemove->event == EVENT_KEY_UP))
      UnGrabActionKey(ActionToRemove);
   if (ActionToRemove->action)
      RemoveActionType(ActionToRemove->action);
   if (ActionToRemove->tooltipstring)
      Efree(ActionToRemove->tooltipstring);
   if (ActionToRemove->key_str)
      Efree(ActionToRemove->key_str);
   Efree(ActionToRemove);

   EDBUG_RETURN_;

}

void
RemoveActionClass(ActionClass * ActionClassToRemove)
{
   int                 i;

   EDBUG(5, "RemoveActionClass");

   if (!ActionClassToRemove)
      EDBUG_RETURN_;

   if (ActionClassToRemove->ref_count > 0)
     {
	DialogOK(_("ActionClass Error!"), _("%u references remain\n"),
		 ActionClassToRemove->ref_count);
	EDBUG_RETURN_;
     }
   while (RemoveItemByPtr(ActionClassToRemove, LIST_TYPE_ACLASS));

   for (i = 0; i < ActionClassToRemove->num; i++)
      RemoveAction(ActionClassToRemove->list[i]);
   if (ActionClassToRemove->list)
      Efree(ActionClassToRemove->list);
   if (ActionClassToRemove->name)
      Efree(ActionClassToRemove->name);
   if (ActionClassToRemove->tooltipstring)
      Efree(ActionClassToRemove->tooltipstring);
   Efree(ActionClassToRemove);
   mode_action_destroy = 1;

   EDBUG_RETURN_;
}

void
AddToAction(Action * act, int id, void *params)
{
   ActionType         *pptr, *ptr, *at;

   EDBUG(5, "AddToAction");
   pptr = NULL;
   at = Emalloc(sizeof(ActionType));
   at->Next = NULL;
   at->Type = id;
   at->params = params;
   if (!act->action)
     {
	act->action = at;
     }
   else
     {
	ptr = act->action;
	while (ptr)
	  {
	     pptr = ptr;
	     ptr = ptr->Next;
	  }
	pptr->Next = at;
     }
   EDBUG_RETURN_;
}

void
AddAction(ActionClass * a, Action * act)
{
   EDBUG(5, "AddAction");
   a->num++;
   if (!a->list)
      a->list = Emalloc(sizeof(Action *));
   else
      a->list = Erealloc(a->list, a->num * sizeof(Action *));
   a->list[a->num - 1] = act;
   EDBUG_RETURN_;
}

int
ActionsCall(unsigned int id, EWin * ewin, void *params)
{
   ActionFunction     *af;

   if (id >= ACTION_NUMBEROF)
      return -1;

   af = &ActionFunctions[id];

   if (!af->ok_zoom && InZoom())
      return 0;
   if (!af->ok_movres &&
       ((mode.mode == MODE_MOVE_PENDING) || (mode.mode == MODE_MOVE) ||
	(mode.mode == MODE_RESIZE_H) || (mode.mode == MODE_RESIZE_V) ||
	(mode.mode == MODE_RESIZE)))
      return 0;

   if (af->hide_slideouts && mode.slideout)
      SlideoutsHide();

   if (af->need_ewin)
     {
	if (ewin == NULL)
	  {
	     if (params)
		ewin = FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID,
				LIST_TYPE_EWIN);
	     if (ewin == NULL)
		ewin = GetContextEwin();
	  }

	if (ewin == NULL)
	   return -1;

	if (ewin->no_actions)
	   return 0;
     }

   return ActionFunctions[id].func(ewin, params);
}

static int
handleAction(EWin * ewin, ActionType * Action)
{
   /* This function will handle any type of action that is passed into
    * it.  ALL internal events should be passed through this function.
    * No exceptions.  --Mandrake (02/26/98)
    */

   int                 error;

   EDBUG(5, "handleAction");
   error = ActionsCall(Action->Type, ewin, Action->params);

   /* Did we just hose ourselves? if so, we'd best not stick around here */
   if (mode_action_destroy)
      EDBUG_RETURN(0);

   /* If there is another action in this series, (now that
    * we're sure we didn't already die) perform it
    */
   if (!error)
      if (Action->Next)
	 error = handleAction(ewin, Action->Next);

   EDBUG_RETURN(error);
}

int
EventAclass(XEvent * ev, EWin * ewin, ActionClass * a)
{
   KeyCode             key;
   int                 i, type, button, modifiers, ok, mouse, mask, val = 0;
   Action             *act;

   EDBUG(5, "EventAclass");

   if (mode.action_inhibit)
      EDBUG_RETURN(0);

   key = type = button = modifiers = mouse = 0;

#if 0				/* FIXME - Do we use this? */
   if ((conf.movemode == 0) && (ewin) &&
       ((mode.mode == MODE_MOVE) || (mode.mode == MODE_MOVE_PENDING)))
      DetermineEwinFloat(ewin, 0, 0);
#endif

   mask =
      (ShiftMask | ControlMask | Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask |
       Mod5Mask) & (~(numlock_mask | scrollock_mask | LockMask));

   switch (ev->type)
     {
     case KeyPress:
	type = EVENT_KEY_DOWN;
	key = ev->xkey.keycode;
	modifiers = ev->xbutton.state & mask;
	mouse = 0;
	break;
     case KeyRelease:
	type = EVENT_KEY_UP;
	key = ev->xkey.keycode;
	modifiers = ev->xbutton.state & mask;
	mouse = 0;
	break;
     case ButtonPress:
	if (ev->xbutton.time == 0)
	   type = EVENT_DOUBLE_DOWN;
	else
	   type = EVENT_MOUSE_DOWN;
	button = ev->xbutton.button;
	modifiers = ev->xbutton.state & mask;
	mouse = 1;
	break;
     case ButtonRelease:
	type = EVENT_MOUSE_UP;
	button = ev->xbutton.button;
	modifiers = ev->xbutton.state & mask;
	mouse = 1;
	break;
     case EnterNotify:
	type = EVENT_MOUSE_ENTER;
	button = -1;
	modifiers = ev->xcrossing.state & mask;
	mouse = 1;
	break;
     case LeaveNotify:
	type = EVENT_MOUSE_LEAVE;
	button = -1;
	modifiers = ev->xcrossing.state & mask;
	mouse = 1;
	break;
     default:
	break;
     }

   mode_action_destroy = 0;

   for (i = 0; i < a->num; i++)
     {
	if (!mode_action_destroy)
	  {
	     act = a->list[i];
	     ok = 0;
	     if ((act->event == type) && (act->action))
	       {
		  if (mouse)
		    {
		       if (button < 0)
			 {
			    if (act->anymodifier)
			       ok = 1;
			    else if (act->modifiers == modifiers)
			       ok = 1;
			 }
		       else
			 {
			    if (act->anymodifier)
			      {
				 if (act->anybutton)
				    ok = 1;
				 else if (act->button == button)
				    ok = 1;
			      }
			    else if (act->modifiers == modifiers)
			      {
				 if (act->anybutton)
				    ok = 1;
				 else if (act->button == button)
				    ok = 1;
			      }
			 }
		    }
		  else
		    {
		       if (act->anymodifier)
			 {
			    if (act->anykey)
			       ok = 1;
			    else if (act->key == key)
			       ok = 1;
			 }
		       else if (act->modifiers == modifiers)
			 {
			    if (act->anykey)
			       ok = 1;
			    else if (act->key == key)
			       ok = 1;
			 }
		    }
		  if (ok)
		    {
		       handleAction(ewin, act->action);
		       val = 1;
		    }
	       }
	  }
	if (mode_action_destroy)
	   break;
     }

   mode_action_destroy = 0;

   EDBUG_RETURN(val);
}

/*
 * The action functions
 */

static int
doNothing(EWin * ewin, void *params)
{
   EDBUG(6, "doNothing");
   EDBUG_RETURN(0);
}

static int
spawnMenu(EWin * ewin, void *params)
{
   char                s[1024];
   char                s2[1024];
   int                 x, y, di;
   Window              dw;
   unsigned int        w, h, d;
   char                desk_click = 0;
   int                 i;

   EDBUG(6, "spawnMenu");

   if (!params)
      EDBUG_RETURN(0);

   if (mode.cur_menu_depth > 0)
      EDBUG_RETURN(0);

   for (i = 0; i < conf.desks.num; i++)
     {
	if (mode.context_win == desks.desk[i].win)
	  {
	     desk_click = 1;
	     break;
	  }
     }
   if (!desk_click)
     {
	if ((ewin) && (ewin->win != mode.context_win) && (mode.context_win))
	  {
	     EGetGeometry(disp, mode.context_win, &dw, &di, &di, &w, &h, &d,
			  &d);
	     XTranslateCoordinates(disp, mode.context_win, root.win, 0, 0, &x,
				   &y, &dw);

	     if (w >= h)
		mode.y = -(y + h);
	     else
		mode.x = -(x + w);
	     mode.context_w = w;
	     mode.context_h = h;
	  }
     }

   if (mode.button)
      clickmenu = 1;

   sscanf((char *)params, "%1000s %1000s", s, s2);
   if (!strcmp(s, "deskmenu"))
     {
	SoundPlay("SOUND_MENU_SHOW");
	ShowDeskMenu();
     }
   else if (!strcmp(s, "taskmenu"))
     {
	SoundPlay("SOUND_MENU_SHOW");
	ShowAllTaskMenu();
     }
   else if (!strcmp(s, "groupmenu"))
     {
	SoundPlay("SOUND_MENU_SHOW");
	ShowGroupMenu();
     }
   else if (!strcmp(s, "named"))
     {
	SoundPlay("SOUND_MENU_SHOW");
	ShowNamedMenu(s2);
     }

   if (((ewin) && (ewin->win == mode.context_win))
       || (ewin = FindEwinByChildren(mode.context_win)))
     {
	if ((ewin) && (mode.cur_menu_depth > 0) && (mode.cur_menu[0]))
	   ewin->shownmenu = MenuWindow(mode.cur_menu[0]);
     }

   if (mode.cur_menu_depth == 0)
      EDBUG_RETURN(0);
   EDBUG_RETURN(1);
}

static int
hideMenu(EWin * ewin, void *params)
{
   EDBUG(6, "hideMenu");
   EDBUG_RETURN(0);
}

static int
runApp(char *exe, char *params)
{

   char               *sh;
   char               *path;
   char               *real_exec;
   int                 fd;

   EDBUG(6, "runApp");

#ifndef __EMX__
   if (fork())
      EDBUG_RETURN(0);
   setsid();
   /* Close all file descriptors except the std 3 */
   for (fd = 3; fd < 1024; fd++)
     {
	close(fd);
     }
#endif
   sh = usershell(getuid());
   if (exe)
     {
	path = pathtoexec(exe);
	if (path)
	  {
	     Efree(path);
	     real_exec = (char *)Emalloc(strlen(params) + 6);
	     sprintf(real_exec, "exec %s", params);
#ifndef __EMX__
	     execl(sh, sh, "-c", (char *)real_exec, NULL);
	     exit(0);
#else
	     spawnl(P_NOWAIT, sh, sh, "-c", (char *)real_exec, NULL);
	     EDBUG_RETURN(0);
#endif
	  }
	if (!mode.startup)
	  {
	     path = pathtofile(exe);
	     if (!path)
	       {
		  /* absolute path */
#ifndef __EMX__
		  if (((char *)exe)[0] == '/')
#else
		  if (_fnisabs((char *)exe))
#endif
		     DialogAlertOK(_
				   ("There was an error running the program:\n"
				    "%s\n"
				    "This program could not be executed.\n"
				    "This is because the file does not exist.\n"),
				   (char *)exe);
		  /* relative path */
		  else
		     DialogAlertOK(_
				   ("There was an error running the program:\n"
				    "%s\n"
				    "This program could not be executed.\n"
				    "This is most probably because this "
				    "program is not in the\n"
				    "path for your shell which is %s. "
				    "I suggest you read the manual\n"
				    "page for that shell and read up how to "
				    "change or add to your\n"
				    "execution path.\n"), (char *)exe, sh);
	       }
	     else
		/* it is a node on the filing sys */
	       {
		  /* it's a file */
		  if (isfile((char *)path))
		    {
		       /* can execute it */
		       if (canexec((char *)path))
			  DialogAlertOK(_
					("There was an error running the program:\n"
					 "%s\n"
					 "This program could not be executed.\n"
					 "I am unsure as to why you could not "
					 "do this. The file exists,\n"
					 "is a file, and you are allowed to "
					 "execute it. I suggest you look\n"
					 "into this.\n"), (char *)path);
		       /* not executable file */
		       else
			  DialogAlertOK(_
					("There was an error running the program:\n"
					 "%s\n"
					 "This program could not be executed.\n"
					 "This is because the file exists, is a "
					 "file, but you are unable\n"
					 "to execute it because you do not "
					 "have execute "
					 "access to this file.\n"),
					(char *)path);
		    }
		  /* it's not a file */
		  else
		    {
		       /* its a dir */
		       if (isdir((char *)path))
			  DialogAlertOK(_
					("There was an error running the program:\n"
					 "%s\n"
					 "This program could not be executed.\n"
					 "This is because the file is in fact "
					 "a directory.\n"), (char *)path);
		       /* its not a file or a dir */
		       else
			  DialogAlertOK(_
					("There was an error running the program:\n"
					 "%s\n"
					 "This program could not be executed.\n"
					 "This is because the file is not a "
					 "regular file.\n"), (char *)path);
		    }
		  if (path)
		     Efree(path);
	       }
	  }
#ifndef __EMX__
	exit(100);
#else
	EDBUG_RETURN(0);
#endif
     }
   real_exec = (char *)Emalloc(strlen(params) + 6);
   sprintf(real_exec, "exec %s", (char *)params);
#ifndef __EMX__
   execl(sh, sh, "-c", (char *)real_exec, NULL);
   exit(0);
#else
   spawnl(P_NOWAIT, sh, sh, "-c", (char *)real_exec, NULL);
#endif
   EDBUG_RETURN(0);

}

int
execApplication(void *params)
{
   char                exe[FILEPATH_LEN_MAX];
   char                s[FILEPATH_LEN_MAX], *ss;
   int                 i, l;
   FILE               *f;

   EDBUG(6, "execApplication");
   if (!params)
      EDBUG_RETURN(0);
   ss = params;
   l = strlen(ss);
   exe[0] = 0;
   strcat(exe, "echo \"");
   for (i = 0; i < l; i++)
     {
	if (ss[i] == '"')
	   strcat(exe, "\\\"");
	else
	  {
	     char                ch[2];

	     ch[0] = ss[i];
	     ch[1] = 0;
	     strcat(exe, ch);
	  }
     }
   strcat(exe, "\"");
   f = popen(exe, "r");
   if (f)
     {
	int                 n;

	n = fread(s, 1, FILEPATH_LEN_MAX - 1, f);
	/* True64 bug workaround */
	if (n == 0)
	   n = fread(s, 1, FILEPATH_LEN_MAX - 1, f);
	if (n > 1)
	  {
	     s[n] = 0;
	     l = strlen(s);
	     s[l - 1] = 0;
	     sscanf(s, "%4000s", exe);
	     runApp(exe, s);
	  }
	pclose(f);
     }
   EDBUG_RETURN(0);
}

static int
doExec(EWin * edummy, void *params)
{
   return execApplication(params);
}

static int
doAlert(EWin * edummy, void *params)
{
   char               *pp;
   int                 i;

   EDBUG(6, "doAlert");

   pp = duplicate((char *)params);
   if (!pp)
      EDBUG_RETURN(1);
   if (strlen(pp) <= 0)
      EDBUG_RETURN(1);

   i = 1;
   while (pp[i])
     {
	if ((pp[i - 1] == '\\') && (((char *)params)[i] == 'n'))
	  {
	     pp[i - 1] = ' ';
	     pp[i] = '\n';
	  }
	i++;
     }
   DialogAlertOK(pp);
   Efree(pp);

   EDBUG_RETURN(0);
}

static int
doExit(EWin * ewin, void *params)
{
   EDBUG(6, "doExit");

   SessionExit(params);

   EDBUG_RETURN(0);
}

static int
doResize(EWin * ewin, void *params)
{
   return ActionResizeStart(ewin, params, MODE_RESIZE);
}

static int
doResizeH(EWin * ewin, void *params)
{
   return ActionResizeStart(ewin, params, MODE_RESIZE_H);
}

static int
doResizeV(EWin * ewin, void *params)
{
   return ActionResizeStart(ewin, params, MODE_RESIZE_V);
}

static int
doMove(EWin * ewin, void *params)
{
   return ActionMoveStart(ewin, params, 0, 0);
}

static int
doMoveConstrained(EWin * ewin, void *params)
{
   return ActionMoveStart(ewin, params, 1, 0);
}

static int
doMoveNoGroup(EWin * ewin, void *params)
{
   return ActionMoveStart(ewin, params, 0, 1);
}

static int
doSwapMove(EWin * ewin, void *params)
{
   mode.swapmovemode = 1;
   return ActionMoveStart(ewin, params, 0, 0);
}

#if 0				/* Not used */
static int
doMoveConstrainedNoGroup(EWin * ewin, void *params)
{
   return ActionMoveStart(ewin, params, 1, 1);
}
#endif

void
ActionsHandleMotion(void)
{
   int                 dx, dy;
   int                 x, y;

   switch (mode.mode)
     {
     case MODE_MOVE_PENDING:
     case MODE_MOVE:
	ActionMoveHandleMotion();
	break;

     case MODE_RESIZE:
     case MODE_RESIZE_H:
     case MODE_RESIZE_V:
	ActionResizeHandleMotion();
	break;

     case MODE_DESKDRAG:
	dx = mode.x - mode.px;
	dy = mode.y - mode.py;
	switch (conf.desks.dragdir)
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
	     if ((x > conf.button_move_resistance)
		 || (y > conf.button_move_resistance))
		mode.button_move_pending = 0;
	  }
	if (!mode.button_move_pending)
	  {
	     if (mode.button)
	       {
		  ButtonMoveRelative(mode.button, dx, dy);
		  if (conf.deskmode == MODE_DESKRAY)
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
}

int
ActionsSuspend(void)
{
   switch (mode.mode)
     {
     case MODE_MOVE_PENDING:
     case MODE_MOVE:
	ActionMoveSuspend();
	break;
     case MODE_RESIZE:
     case MODE_RESIZE_H:
     case MODE_RESIZE_V:
	ActionResizeEnd(NULL);
	break;
     }

   return 0;
}

int
ActionsResume(void)
{
   switch (mode.mode)
     {
     case MODE_MOVE_PENDING:
     case MODE_MOVE:
	ActionMoveResume();
	break;
     }

   return 0;
}

int
ActionsEnd(EWin * ewin)
{
   int                 did_end = 1;

   switch (mode.mode)
     {
     case MODE_RESIZE:
     case MODE_RESIZE_H:
     case MODE_RESIZE_V:
	ActionResizeEnd(ewin);
	mode.action_inhibit = 1;
	break;
     case MODE_MOVE_PENDING:
     case MODE_MOVE:
	ActionMoveEnd(ewin);
	mode.action_inhibit = 1;
	break;
     case MODE_DESKDRAG:
	mode.mode = MODE_NONE;
	break;
     case MODE_BUTTONDRAG:
	if (!mode.button_move_pending)
	   mode.action_inhibit = 1;
	doDragButtonEnd(NULL);
	break;
     default:
	did_end = 0;
	break;
     }

   return did_end;
}

static int
DoRaise(EWin * ewin, void *params, int nogroup)
{
   EWin              **gwins = NULL;
   int                 i, num;

   EDBUG(6, "DoRaise");
   SoundPlay("SOUND_RAISE");
   gwins = ListWinGroupMembersForEwin(ewin, ACTION_RAISE, nogroup, &num);
   for (i = 0; i < num; i++)
      RaiseEwin(gwins[i]);
   Efree(gwins);
   EDBUG_RETURN(0);
}

static int
DoLower(EWin * ewin, void *params, int nogroup)
{
   EWin              **gwins = NULL;
   int                 i, num;

   EDBUG(6, "DoLower");
   SoundPlay("SOUND_LOWER");
   gwins = ListWinGroupMembersForEwin(ewin, ACTION_LOWER, nogroup, &num);
   for (i = 0; i < num; i++)
      LowerEwin(gwins[i]);
   Efree(gwins);
   EDBUG_RETURN(0);
}

static int
doRaise(EWin * ewin, void *params)
{
   return DoRaise(ewin, params, 0);
}

static int
doRaiseNoGroup(EWin * ewin, void *params)
{
   return DoRaise(ewin, params, 1);
}

static int
doLower(EWin * ewin, void *params)
{
   return DoLower(ewin, params, 0);
}

static int
doLowerNoGroup(EWin * ewin, void *params)
{
   return DoLower(ewin, params, 1);
}

static int
FindEwinInList(EWin * ewin, EWin ** gwins, int num)
{
   int                 i;

   if (ewin && gwins)
     {
	for (i = 0; i < num; i++)
	  {
	     if (ewin == gwins[i])
		return 1;
	  }
     }
   return 0;
}

static int
DoRaiseLower(EWin * ewin, void *params, int nogroup)
{
   EWin              **gwins;
   int                 i, num, j, raise = 0;

   EDBUG(6, "doRaiseLower");

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_RAISE_LOWER, nogroup, &num);
   for (j = 0; j < num; j++)
     {
	ewin = gwins[j];
	if (desks.desk[ewin->desktop].list)
	  {
	     for (i = 0; i < desks.desk[ewin->desktop].num - 1; i++)
	       {
		  if (desks.desk[ewin->desktop].list[i]->layer == ewin->layer
		      && (desks.desk[ewin->desktop].list[i] == ewin
			  || !FindEwinInList(desks.desk[ewin->desktop].list[i],
					     gwins, num)))
		    {
		       if (desks.desk[ewin->desktop].list[i] != ewin)
			  raise = 1;

		       j = num;
		       break;
		    }
	       }
	  }
     }

   if (!raise)
     {
	SoundPlay("SOUND_LOWER");
	for (j = 0; j < num; j++)
	   LowerEwin(gwins[j]);
     }
   else
     {
	SoundPlay("SOUND_RAISE");
	for (j = 0; j < num; j++)
	   RaiseEwin(gwins[j]);
     }

   if (gwins)
      Efree(gwins);

   EDBUG_RETURN(0);
}

static int
doRaiseLower(EWin * ewin, void *params)
{
   return DoRaiseLower(ewin, params, 0);
}

static int
doRaiseLowerNoGroup(EWin * ewin, void *params)
{
   return DoRaiseLower(ewin, params, 1);
}

static int
doCleanup(EWin * edummy, void *params)
{
   char               *type;
   int                 method;
   void              **lst;
   int                 i, j, k, num, speed;
   RectBox            *fixed, *ret, *floating;
   char                doslide;
   EWin               *ewin;
   Button            **blst;

   EDBUG(6, "doCleanup");

   type = (char *)params;
   method = ARRANGE_BY_SIZE;
   speed = conf.slidespeedcleanup;
   doslide = conf.cleanupslide;

   if (params)
     {
	if (!strcmp("order", type))
	  {
	     method = ARRANGE_VERBATIM;
	  }
	else if (!strcmp("place", type))
	  {
	     method = ARRANGE_BY_POSITION;
	  }
     }
   lst = ListItemType(&num, LIST_TYPE_EWIN);
   if (lst)
     {
	fixed = NULL;
	floating = Emalloc(sizeof(RectBox) * num);
	ret = Emalloc(sizeof(RectBox) * (num));
	j = 0;
	k = 0;
	for (i = 0; i < num; i++)
	  {
	     if ((((EWin *) lst[i])->desktop == desks.current)
		 && (!((EWin *) lst[i])->sticky)
		 && (!((EWin *) lst[i])->floating)
		 && (!((EWin *) lst[i])->iconified)
		 && (!((EWin *) lst[i])->ignorearrange)
		 && (!((EWin *) lst[i])->menu)
		 && (((EWin *) lst[i])->area_x ==
		     desks.desk[((EWin *) lst[i])->desktop].current_area_x)
		 && (((EWin *) lst[i])->area_y ==
		     desks.desk[((EWin *) lst[i])->desktop].current_area_y))
	       {
		  floating[j].data = lst[i];
		  floating[j].x = ((EWin *) lst[i])->x;
		  floating[j].y = ((EWin *) lst[i])->y;
		  floating[j].w = ((EWin *) lst[i])->w;
		  floating[j].p = ((EWin *) lst[i])->layer;
		  floating[j++].h = ((EWin *) lst[i])->h;
	       }
	     else if (((((EWin *) lst[i])->desktop == desks.current)
		       || (((EWin *) lst[i])->sticky))
		      && (((EWin *) lst[i])->layer != 4)
		      && (((EWin *) lst[i])->layer != 0)
		      && (!((EWin *) lst[i])->menu))
	       {
		  fixed = Erealloc(fixed, sizeof(RectBox) * (k + 1));
		  fixed[k].data = lst[i];
		  fixed[k].x = ((EWin *) lst[i])->x;
		  fixed[k].y = ((EWin *) lst[i])->y;
		  fixed[k].w = ((EWin *) lst[i])->w;
		  fixed[k].h = ((EWin *) lst[i])->h;
		  if (fixed[k].x < 0)
		    {
		       fixed[k].x += fixed[k].w;
		       fixed[k].x = 0;
		    }
		  if ((fixed[k].x + fixed[k].w) > root.w)
		     fixed[k].w = root.w - fixed[k].x;
		  if (fixed[k].y < 0)
		    {
		       fixed[k].y += fixed[k].h;
		       fixed[k].y = 0;
		    }
		  if ((fixed[k].y + fixed[k].h) > root.h)
		     fixed[k].h = root.h - fixed[k].y;
		  if ((fixed[k].w > 0) && (fixed[k].h > 0))
		    {
		       if (!((EWin *) lst[i])->never_use_area)
			  fixed[k].p = ((EWin *) lst[i])->layer;
		       else
			  fixed[k].p = 99;
		       k++;
		    }
	       }
	  }

	blst = (Button **) ListItemType(&num, LIST_TYPE_BUTTON);
	if (blst)
	  {
	     fixed = Erealloc(fixed, sizeof(RectBox) * (num + k));
	     ret = Erealloc(ret, sizeof(RectBox) * ((num + j) + 1 + k));
	     for (i = 0; i < num; i++)
	       {
		  if (ButtonGetInfo(blst[i], &fixed[k], desks.current))
		     continue;

		  if (fixed[k].x < 0)
		    {
		       fixed[k].x += fixed[k].w;
		       fixed[k].x = 0;
		    }
		  if ((fixed[k].x + fixed[k].w) > root.w)
		     fixed[k].w = root.w - fixed[k].x;
		  if (fixed[k].y < 0)
		    {
		       fixed[k].y += fixed[k].h;
		       fixed[k].y = 0;
		    }
		  if ((fixed[k].y + fixed[k].h) > root.h)
		     fixed[k].h = root.h - fixed[k].y;
		  if ((fixed[k].w <= 0) || (fixed[k].h <= 0))
		     continue;

		  if (fixed[k].p)	/* Sticky */
		     fixed[k].p = 50;
		  else
		     fixed[k].p = 0;
		  k++;
	       }
	     Efree(blst);
	  }

	ArrangeRects(fixed, k, floating, j, ret, 0, 0, root.w, root.h, method,
		     0);

	for (i = 0; i < (j + k); i++)
	  {
	     if (ret[i].data)
	       {
		  if (doslide)
		    {
		       ewin = (EWin *) ret[i].data;
		       if (ewin)
			 {
			    if ((ewin->x != ret[i].x) || (ewin->y != ret[i].y))
			      {
				 SlideEwinTo(ewin, ewin->x, ewin->y, ret[i].x,
					     ret[i].y, speed);
				 ICCCM_Configure(ewin);
			      }
			 }
		    }
		  else
		    {
		       ewin = (EWin *) ret[i].data;
		       if (ewin)
			 {
			    if ((ewin->x != ret[i].x) || (ewin->y != ret[i].y))
			       MoveEwin((EWin *) ret[i].data, ret[i].x,
					ret[i].y);
			 }
		    }
	       }
	  }

	if (fixed)
	   Efree(fixed);
	if (ret)
	   Efree(ret);
	if (floating)
	   Efree(floating);
	if (lst)
	   Efree(lst);
     }
   EDBUG_RETURN(0);
}

static int
doKill(EWin * ewin, void *params)
{
   EDBUG(6, "doKill");
   KillEwin(ewin, 0);
   EDBUG_RETURN(0);
}

static int
doKillNoGroup(EWin * ewin, void *params)
{
   EDBUG(6, "doKillNoGroup");
   KillEwin(ewin, 1);
   EDBUG_RETURN(0);
}

static int
doKillNasty(EWin * ewin, void *params)
{
   EDBUG(6, "doKillNasty");

   SoundPlay("SOUND_WINDOW_CLOSE");
   EDestroyWindow(disp, ewin->client.win);
   EDBUG_RETURN(0);
}

/* Desktop actions */

static int
DoGotoDesktop(EWin * edummy, void *params, int num)
{
   int                 pd;

   EDBUG(6, "DoGotoDesktop");

   pd = desks.current;

   if (params)
      sscanf((char *)params, "%i", &num);
   GotoDesktop(num);

   if (desks.current != pd)
      SoundPlay("SOUND_DESKTOP_SHUT");

   EDBUG_RETURN(0);
}

static int
doNextDesktop(EWin * edummy, void *params)
{
   return DoGotoDesktop(edummy, NULL, desks.current + 1);
}

static int
doPrevDesktop(EWin * edummy, void *params)
{
   return DoGotoDesktop(edummy, NULL, desks.current - 1);
}

static int
doGotoDesktop(EWin * edummy, void *params)
{
   return DoGotoDesktop(edummy, params, desks.current);
}

int
doInplaceDesktop(EWin * edummy, void *params)
{
   return DoGotoDesktop(edummy, params, desks.current);
}

static int
doRaiseDesktop(EWin * edummy, void *params)
{
   int                 d = 0;

   EDBUG(6, "doRaiseDesktop");

   if (!params)
      d = desks.current;
   else
      d = atoi((char *)params);
   SoundPlay("SOUND_DESKTOP_RAISE");
   RaiseDesktop(d);

   EDBUG_RETURN(0);
}

static int
doLowerDesktop(EWin * edummy, void *params)
{
   int                 d = 0;

   EDBUG(6, "doLowerDesktop");

   if (!params)
      d = desks.current;
   else
      d = atoi((char *)params);
   SoundPlay("SOUND_DESKTOP_LOWER");
   LowerDesktop(d);

   EDBUG_RETURN(0);
}

static int
doDragDesktop(EWin * edummy, void *params)
{
   int                 d = 0;

   EDBUG(6, "doDragDesktop");

   if (!params)
      d = desks.current;
   else
      d = atoi((char *)params);
   mode.deskdrag = d;
   mode.mode = MODE_DESKDRAG;
   mode.start_x = mode.x;
   mode.start_y = mode.y;
   mode.win_x = desks.desk[d].x;
   mode.win_y = desks.desk[d].y;

   EDBUG_RETURN(0);
}

/* Window ops */

static int
DoStick(EWin * ewin, void *params, int nogroup)
{
   EWin              **gwins = NULL;
   Group              *curr_group = NULL;
   int                 i, num;
   char                sticky;

   EDBUG(6, "DoStick");

   sticky = ewin->sticky;

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_STICK, nogroup, &num);
   for (i = 0; i < num; i++)
     {
	curr_group = EwinsInGroup(ewin, gwins[i]);
	if (gwins[i]->sticky
	    && ((curr_group && !curr_group->cfg.mirror) || sticky))
	   MakeWindowUnSticky(gwins[i]);
	else if (!gwins[i]->sticky
		 && ((curr_group && !curr_group->cfg.mirror) || !sticky))
	   MakeWindowSticky(gwins[i]);
	RememberImportantInfoForEwin(gwins[i]);
     }
   if (gwins)
      Efree(gwins);

   EDBUG_RETURN(0);
}

static int
doStick(EWin * ewin, void *params)
{
   return DoStick(ewin, params, 0);
}

static int
doStickNoGroup(EWin * ewin, void *params)
{
   return DoStick(ewin, params, 1);
}

static int
doSkipLists(EWin * ewin, void *params)
{
   char                skip;

   EDBUG(6, "doSkipLists");

   skip = ewin->skipfocus;
   ewin->skiptask = !(skip);
   ewin->skipwinlist = !(skip);
   ewin->skipfocus = !(skip);
   HintsSetWindowState(ewin);
   HintsSetClientList();
   RememberImportantInfoForEwin(ewin);

   EDBUG_RETURN(0);
}

static int
doSkipTask(EWin * ewin, void *params)
{
   EDBUG(6, "doSkipTask");

   ewin->skiptask = !ewin->skiptask;
   HintsSetWindowState(ewin);
   HintsSetClientList();
   RememberImportantInfoForEwin(ewin);

   EDBUG_RETURN(0);
}

static int
doSkipFocus(EWin * ewin, void *params)
{
   EDBUG(6, "doSkipFocus");

   ewin->skipfocus = !ewin->skipfocus;
   HintsSetWindowState(ewin);
   RememberImportantInfoForEwin(ewin);

   EDBUG_RETURN(0);
}

static int
doSkipWinList(EWin * ewin, void *params)
{
   EDBUG(6, "doSkipWinList");

   ewin->skipwinlist = !ewin->skipwinlist;
   HintsSetWindowState(ewin);
   RememberImportantInfoForEwin(ewin);

   EDBUG_RETURN(0);
}

static int
doNeverFocus(EWin * ewin, void *params)
{
   EDBUG(6, "doSkipWinList");

   ewin->neverfocus = !ewin->neverfocus;
   HintsSetWindowState(ewin);
   RememberImportantInfoForEwin(ewin);

   EDBUG_RETURN(0);
}

/* Button actions */

static int
doDragButtonStart(EWin * edummy, void *params)
{
   Button             *b;

   EDBUG(6, "doDragButtonStart");

   b = mode.button;
   if (!b)
      EDBUG_RETURN(0);

   if (ButtonIsFixed(b))
     {
	mode.button = NULL;
	EDBUG_RETURN(0);
     }

   GrabThePointer(root.win);
   mode.mode = MODE_BUTTONDRAG;
   mode.button_move_pending = 1;
   mode.start_x = mode.x;
   mode.start_y = mode.y;
   ButtonGetGeometry(b, &mode.win_x, &mode.win_y, NULL, NULL);
   mode.firstlast = 0;

   EDBUG_RETURN(0);
}

int
doDragButtonEnd(void *params)
{
   Button             *b;
   int                 d;

   EDBUG(6, "doDragButtonEnd");

   b = mode.button;
   if (!b)
      EDBUG_RETURN(0);

   mode.mode = MODE_NONE;
   UnGrabTheButtons();
   if (!mode.button_move_pending)
     {
	d = DesktopAt(mode.x, mode.y);
	ButtonMoveToDesktop(b, d);
	d = ButtonGetDesktop(b);
	ButtonMoveRelative(b, -desks.desk[d].x, -desks.desk[d].y);
     }
   else
      mode.button_move_pending = 0;

   autosave();

   EDBUG_RETURN(0);
}

/* Settings */

static int
doFocusModeSet(EWin * edummy, void *params)
{
   EDBUG(6, "doFocusModeSet");
   if (params)
     {
	if (!strcmp("pointer", (char *)params))
	   conf.focus.mode = MODE_FOCUS_POINTER;
	else if (!strcmp("sloppy", (char *)params))
	   conf.focus.mode = MODE_FOCUS_SLOPPY;
	else if (!strcmp("click", (char *)params))
	   conf.focus.mode = MODE_FOCUS_CLICK;
     }
   else
     {
	if (conf.focus.mode == MODE_FOCUS_POINTER)
	   conf.focus.mode = MODE_FOCUS_SLOPPY;
	else if (conf.focus.mode == MODE_FOCUS_SLOPPY)
	   conf.focus.mode = MODE_FOCUS_CLICK;
	else if (conf.focus.mode == MODE_FOCUS_CLICK)
	   conf.focus.mode = MODE_FOCUS_POINTER;
     }
   FocusFix();
   autosave();
   EDBUG_RETURN(0);
}

static int
doMoveModeSet(EWin * edummy, void *params)
{
   EDBUG(6, "doMoveModeSet");
   if (params)
     {
	conf.movemode = atoi((char *)params);
     }
   else
     {
	conf.movemode++;
	if (conf.movemode > 5)
	   conf.movemode = 0;
     }
#if !USE_IMLIB2
   if ((prImlib_Context) && (conf.movemode == 5))
      conf.movemode = 3;
#endif
   autosave();
   EDBUG_RETURN(0);
}

static int
doResizeModeSet(EWin * edummy, void *params)
{
   EDBUG(6, "doResizeModeSet");
   if (params)
     {
	conf.resizemode = atoi((char *)params);
     }
   else
     {
	conf.resizemode++;
	if (conf.resizemode > 4)
	   conf.resizemode = 0;
     }
   if (conf.resizemode == 5)
      conf.resizemode = 3;
   autosave();
   EDBUG_RETURN(0);
}

static int
doSlideModeSet(EWin * edummy, void *params)
{
   EDBUG(6, "doSlideModeSet");
   if (params)
     {
	conf.slidemode = atoi((char *)params);
     }
   else
     {
	conf.slidemode++;
	if (conf.slidemode > 4)
	   conf.slidemode = 0;
     }
   autosave();
   EDBUG_RETURN(0);
}

static int
doCleanupSlideSet(EWin * edummy, void *params)
{
   EDBUG(6, "doCleanupSlideSet");
   if (params)
     {
	conf.cleanupslide = atoi((char *)params);
     }
   else
     {
	if (conf.cleanupslide)
	   conf.cleanupslide = 0;
	else
	   conf.cleanupslide = 1;
     }
   autosave();
   EDBUG_RETURN(0);
}

static int
doMapSlideSet(EWin * edummy, void *params)
{
   EDBUG(6, "doMapSlideSet");
   if (params)
      conf.mapslide = atoi((char *)params);
   else
     {
	if (conf.mapslide)
	   conf.mapslide = 0;
	else
	   conf.mapslide = 1;
     }
   autosave();
   EDBUG_RETURN(0);
}

static int
doSoundSet(EWin * edummy, void *params)
{
   char                snd;

   EDBUG(6, "doSoundSet");
   snd = conf.sound;
   if (params)
      conf.sound = atoi((char *)params);
   else
     {
	if (conf.sound)
	   conf.sound = 0;
	else
	   conf.sound = 1;
     }
   if (conf.sound != snd)
     {
	if (conf.sound)
	   SoundInit();
	else
	   SoundExit();
     }
   autosave();
   EDBUG_RETURN(0);
}

static int
doButtonMoveResistSet(EWin * edummy, void *params)
{
   EDBUG(6, "doButtonMoveResistSet");
   if (params)
      conf.button_move_resistance = atoi((char *)params);
   autosave();
   EDBUG_RETURN(0);
}

static int
doDesktopBgTimeoutSet(EWin * edummy, void *params)
{
   EDBUG(6, "doDesktopBgTimeoutSet");
   if (params)
      conf.backgrounds.timeout = atoi((char *)params);
   autosave();
   EDBUG_RETURN(0);
}

static int
doMapSlideSpeedSet(EWin * edummy, void *params)
{
   EDBUG(6, "doMapSlideSpeedSet");
   if (params)
      conf.slidespeedmap = atoi((char *)params);
   autosave();
   EDBUG_RETURN(0);
}

static int
doCleanupSlideSpeedSet(EWin * edummy, void *params)
{
   EDBUG(6, "doCleanupSlideSpeedSet");
   if (params)
      conf.slidespeedcleanup = atoi((char *)params);
   autosave();
   EDBUG_RETURN(0);
}

static int
doDragdirSet(EWin * edummy, void *params)
{
   char                pd;
   Button             *b;
   int                 i;

   EDBUG(6, "doDragdirSet");
   pd = conf.desks.dragdir;
   if (params)
      conf.desks.dragdir = atoi((char *)params);
   else
     {
	conf.desks.dragdir++;
	if (conf.desks.dragdir > 3)
	   conf.desks.dragdir = 0;
     }
   if (pd != conf.desks.dragdir)
     {
	GotoDesktop(desks.current);
	for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
	   MoveDesktop(i, 0, 0);
	while ((b = RemoveItem("_DESKTOP_DRAG_CONTROL", 0,
			       LIST_FINDBY_NAME, LIST_TYPE_BUTTON)))
	   ButtonDestroy(b);
	while ((b = RemoveItem("_DESKTOP_DESKRAY_DRAG_CONTROL", 0,
			       LIST_FINDBY_NAME, LIST_TYPE_BUTTON)))
	   ButtonDestroy(b);
	InitDesktopControls();
	ShowDesktopControls();
     }
   autosave();
   EDBUG_RETURN(0);
}

static int
doDragbarOrderSet(EWin * edummy, void *params)
{
   char                pd;
   Button             *b;

   EDBUG(6, "doDragbarOrderSet");
   pd = conf.desks.dragbar_ordering;
   if (params)
      conf.desks.dragbar_ordering = atoi((char *)params);
   else
     {
	conf.desks.dragbar_ordering++;
	if (conf.desks.dragbar_ordering > 5)
	   conf.desks.dragbar_ordering = 0;
     }
   if (pd != conf.desks.dragbar_ordering)
     {
	while ((b = RemoveItem("_DESKTOP_DRAG_CONTROL", 0,
			       LIST_FINDBY_NAME, LIST_TYPE_BUTTON)))
	   ButtonDestroy(b);
	InitDesktopControls();
	ShowDesktopControls();
     }
   autosave();
   EDBUG_RETURN(0);
}

static int
doDragbarWidthSet(EWin * edummy, void *params)
{
   int                 pd;
   Button             *b;

   EDBUG(6, "doDragbarWidthSet");
   pd = conf.desks.dragbar_width;
   if (params)
      conf.desks.dragbar_width = atoi((char *)params);
   if (pd != conf.desks.dragbar_width)
     {
	while ((b = RemoveItem("_DESKTOP_DRAG_CONTROL", 0,
			       LIST_FINDBY_NAME, LIST_TYPE_BUTTON)))
	   ButtonDestroy(b);
	InitDesktopControls();
	ShowDesktopControls();
     }
   autosave();
   EDBUG_RETURN(0);
}

static int
doDragbarLengthSet(EWin * edummy, void *params)
{
   int                 pd;
   Button             *b;

   EDBUG(6, "doDragbarLengthSet");
   pd = conf.desks.dragbar_length;
   if (params)
      conf.desks.dragbar_length = atoi((char *)params);
   if (pd != conf.desks.dragbar_length)
     {
	while ((b = RemoveItem("_DESKTOP_DRAG_CONTROL", 0,
			       LIST_FINDBY_NAME, LIST_TYPE_BUTTON)))
	   ButtonDestroy(b);
	InitDesktopControls();
	ShowDesktopControls();
     }
   autosave();
   EDBUG_RETURN(0);
}

static int
doDeskSlideSet(EWin * edummy, void *params)
{
   EDBUG(6, "doDeskSlideSet");
   if (params)
      conf.desks.slidein = atoi((char *)params);
   else
     {
	if (conf.desks.slidein)
	   conf.desks.slidein = 0;
	else
	   conf.desks.slidein = 1;
     }
   autosave();
   EDBUG_RETURN(0);
}

static int
doDeskSlideSpeedSet(EWin * edummy, void *params)
{
   EDBUG(6, "doDeskSlideSpeedSet");
   if (params)
      conf.desks.slidespeed = atoi((char *)params);
   autosave();
   EDBUG_RETURN(0);
}

static int
doHiQualityBgSet(EWin * edummy, void *params)
{
   EDBUG(6, "doHiQualityBgSet");
   if (params)
      conf.backgrounds.hiquality = atoi((char *)params);
   else
     {
	if (conf.backgrounds.hiquality)
	   conf.backgrounds.hiquality = 0;
	else
	   conf.backgrounds.hiquality = 1;
     }
   autosave();
   EDBUG_RETURN(0);
}

static int
doAutosaveSet(EWin * edummy, void *params)
{
   EDBUG(6, "doAutosaveSet");
   if (params)
      conf.autosave = atoi((char *)params);
   else
     {
	if (conf.autosave)
	   conf.autosave = 0;
	else
	   conf.autosave = 1;
     }
   EDBUG_RETURN(0);
}

static int
doToolTipSet(EWin * edummy, void *params)
{
   EDBUG(6, "doToolTipSet");
   if (params)
      conf.tooltips.enable = atoi((char *)params);
   else
      conf.tooltips.enable = !conf.tooltips.enable;
   autosave();
   EDBUG_RETURN(0);
}

/* Misc actions */

static int
doPlaySoundClass(EWin * edummy, void *params)
{
   EDBUG(6, "doPlaySoundClass");

   if (!params)
      EDBUG_RETURN(0);

   SoundPlay((char *)params);

   EDBUG_RETURN(0);
}

static int
doDeskray(EWin * edummy, void *params)
{
   EDBUG(6, "doDeskray");
   if (params)
     {
	if (!atoi((char *)params))
	  {
	     HideDesktopTabs();
	     conf.deskmode = MODE_NONE;
	  }
	else
	  {
	     conf.deskmode = MODE_DESKRAY;
	     ShowDesktopTabs();
	  }
     }
   else
     {
	if (conf.deskmode == MODE_DESKRAY)
	  {
	     HideDesktopTabs();
	     conf.deskmode = MODE_NONE;
	  }
	else
	  {
	     conf.deskmode = MODE_DESKRAY;
	     ShowDesktopTabs();
	  }
     }
   EDBUG_RETURN(0);
}

static int
doHideShowButton(EWin * edummy, void *params)
{
   Button            **lst, *b;
   char                s[1024], *ss;
   int                 num, i;

   EDBUG(6, "doHideShowButton");

   if (params)
     {
	sscanf((char *)params, "%1000s", s);
	if (!strcmp(s, "button"))
	  {
	     sscanf((char *)params, "%*s %1000s", s);
	     b = (Button *) FindItem(s, 0, LIST_FINDBY_NAME, LIST_TYPE_BUTTON);
	     if (b)
		ButtonToggle(b);
	  }
	else if (!strcmp(s, "buttons"))
	  {
	     ss = atword((char *)params, 2);
	     if (ss)
	       {
		  lst = (Button **) ListItemType(&num, LIST_TYPE_BUTTON);
		  if (lst)
		    {
		       for (i = 0; i < num; i++)
			 {
			    if (matchregexp(ss, ButtonGetName(lst[i])))
			      {
				 if (strcmp(ButtonGetName(lst[i]),
					    "_DESKTOP_DESKRAY_DRAG_CONTROL"))
				    ButtonToggle(lst[i]);
			      }
			 }
		    }
	       }
	  }
	else if (!strcmp(s, "all_buttons_except"))
	  {
	     ss = atword((char *)params, 2);
	     if (ss)
	       {
		  lst = (Button **) ListItemTypeID(&num, LIST_TYPE_BUTTON, 0);
		  if (lst)
		    {
		       for (i = 0; i < num; i++)
			 {
			    if (!matchregexp(ss, ButtonGetName(lst[i])))
			      {
				 if (strcmp(ButtonGetName(lst[i]),
					    "_DESKTOP_DESKRAY_DRAG_CONTROL"))
				    ButtonToggle(lst[i]);
			      }
			 }
		    }
	       }
	  }
	else if (!strcmp(s, "all"))
	  {
	     lst = (Button **) ListItemType(&num, LIST_TYPE_BUTTON);
	     if (lst)
	       {
		  for (i = 0; i < num; i++)
		    {
		       if (strcmp
			   (ButtonGetName(lst[i]),
			    "_DESKTOP_DESKRAY_DRAG_CONTROL"))
			  ButtonToggle(lst[i]);
		    }
	       }
	  }
     }
   else
     {
	lst = (Button **) ListItemTypeID(&num, LIST_TYPE_BUTTON, 0);
	if (lst)
	  {
	     for (i = 0; i < num; i++)
	       {
		  ButtonToggle(lst[i]);
	       }
	  }
     }
   StackDesktops();
   autosave();

   EDBUG_RETURN(0);
}

static int
doScrollContainer(EWin * edummy, void *params)
{
   EDBUG(6, "doScrollContainer");
   EDBUG_RETURN(0);
}

/* More winops */

static int
DoIconifyWindow(EWin * ewin, void *params, int nogroup)
{
   Group              *curr_group = NULL;
   char                iconified;
   EWin              **gwins = NULL;
   int                 i, num;

   EDBUG(6, "DoIconifyWindow");

#if 0				/* FIXME - Do we use this? */
   if (params)
     {
	ewin =
	   FindItem("ICON", atoi((char *)params), LIST_FINDBY_BOTH,
		    LIST_TYPE_ICONIFIEDS);
	if (!ewin)
	   ewin =
	      FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID,
		       LIST_TYPE_EWIN);
     }
   else
      ewin = GetFocusEwin();

   if (!ewin)
      EDBUG_RETURN(1);
#endif

   iconified = ewin->iconified;

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_ICONIFY, nogroup, &num);
   for (i = 0; i < num; i++)
     {
	curr_group = EwinsInGroup(ewin, gwins[i]);
	if (gwins[i]->iconified
	    && ((curr_group && !curr_group->cfg.mirror) || iconified))
	  {
	     DeIconifyEwin(gwins[i]);
	  }
	else if (!gwins[i]->iconified
		 && ((curr_group && !curr_group->cfg.mirror) || !iconified))
	  {
	     IconifyEwin(gwins[i]);
	  }
     }
   if (gwins)
      Efree(gwins);
   EDBUG_RETURN(0);
}

static int
doIconifyWindow(EWin * ewin, void *params)
{
   return DoIconifyWindow(ewin, params, 0);
}

static int
doIconifyWindowNoGroup(EWin * ewin, void *params)
{
   return DoIconifyWindow(ewin, params, 1);
}

/* More misc */

static int
doSlideout(EWin * ewin, void *params)
{
   Slideout           *s;

   EDBUG(6, "doSlideout");
   if (!params)
      EDBUG_RETURN(0);

   s = FindItem((char *)params, 0, LIST_FINDBY_NAME, LIST_TYPE_SLIDEOUT);
   if (s)
     {
	SoundPlay("SOUND_SLIDEOUT_SHOW");
	SlideoutShow(s, ewin, mode.context_win);
     }
   EDBUG_RETURN(0);
}

static int
doScrollWindows(EWin * edummy, void *params)
{
   int                 x, y, num, i;
   EWin              **lst;

   EDBUG(6, "doScrollWindows");
   if (!params)
      EDBUG_RETURN(0);

   x = 0;
   y = 0;
   sscanf((char *)params, "%i %i", &x, &y);

   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);

   if ((lst) && (num > 0))
     {
	for (i = 0; i < num; i++)
	  {
	     if ((lst[i]->desktop == desks.current) && (!lst[i]->sticky)
		 && (!lst[i]->floating) && (!lst[i]->fixedpos))
		MoveEwin(lst[i], lst[i]->x + x, lst[i]->y + y);
	  }
	Efree(lst);
     }
   EDBUG_RETURN(0);
}

/* More winops */

static int
DoShade(EWin * ewin, void *params, int nogroup)
{
   EWin              **gwins = NULL;
   Group              *curr_group = NULL;
   int                 i, num;
   char                shaded;

   EDBUG(6, "doShade");

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_SHADE, nogroup, &num);
   shaded = ewin->shaded;
   for (i = 0; i < num; i++)
     {
	curr_group = EwinsInGroup(ewin, gwins[i]);
	if (gwins[i]->shaded
	    && ((curr_group && !curr_group->cfg.mirror) || shaded))
	  {
	     SoundPlay("SOUND_UNSHADE");
	     UnShadeEwin(gwins[i]);
	  }
	else if (!gwins[i]->shaded
		 && ((curr_group && !curr_group->cfg.mirror) || !shaded))
	  {
	     SoundPlay("SOUND_SHADE");
	     ShadeEwin(gwins[i]);
	  }
	RememberImportantInfoForEwin(gwins[i]);
     }
   Efree(gwins);
   EDBUG_RETURN(0);
}

static int
doShade(EWin * ewin, void *params)
{
   return DoShade(ewin, params, 0);
}

static int
doShadeNoGroup(EWin * ewin, void *params)
{
   return DoShade(ewin, params, 1);
}

static int
doMaxH(EWin * ewin, void *params)
{
   EDBUG(6, "doMaxH");
   if (ewin->shaded)
      EDBUG_RETURN(0);
   MaxHeight(ewin, (char *)params);
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

static int
doMaxW(EWin * ewin, void *params)
{
   EDBUG(6, "doMaxW");
   if (ewin->shaded)
      EDBUG_RETURN(0);
   MaxWidth(ewin, (char *)params);
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

static int
doMax(EWin * ewin, void *params)
{
   EDBUG(6, "doMax");
   if (ewin->shaded)
      EDBUG_RETURN(0);
   MaxSize(ewin, (char *)params);
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

static int
doSendToNextDesk(EWin * ewin, void *params)
{
   EDBUG(6, "doSendToNextDesk");
   MoveEwinToDesktop(ewin, ewin->desktop + 1);
   RaiseEwin(ewin);
   ICCCM_Configure(ewin);
   ewin->sticky = 0;
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

static int
doSendToPrevDesk(EWin * ewin, void *params)
{
   EDBUG(6, "doSendToPrevDesk");
   MoveEwinToDesktop(ewin, ewin->desktop - 1);
   RaiseEwin(ewin);
   ICCCM_Configure(ewin);
   ewin->sticky = 0;
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

static int
doSnapshot(EWin * ewin, void *params)
{
   EDBUG(6, "doSnapshot");

   if (!params)
      SnapshotEwinAll(ewin);
   else if (!strcmp((char *)params, "none"))
      UnsnapshotEwin(ewin);
   else if (!strcmp((char *)params, "border"))
      SnapshotEwinBorder(ewin);
   else if (!strcmp((char *)params, "desktop"))
      SnapshotEwinDesktop(ewin);
   else if (!strcmp((char *)params, "size"))
      SnapshotEwinSize(ewin);
   else if (!strcmp((char *)params, "location"))
      SnapshotEwinLocation(ewin);
   else if (!strcmp((char *)params, "layer"))
      SnapshotEwinLayer(ewin);
   else if (!strcmp((char *)params, "sticky"))
      SnapshotEwinSticky(ewin);
   else if (!strcmp((char *)params, "icon"))
      SnapshotEwinIcon(ewin);
   else if (!strcmp((char *)params, "shade"))
      SnapshotEwinShade(ewin);
   else if (!strcmp((char *)params, "group"))
      SnapshotEwinGroups(ewin, 1);
   else if (!strcmp((char *)params, "dialog"))
      SnapshotEwinDialog(ewin);
   EDBUG_RETURN(0);
}

static int
doToggleFixedPos(EWin * ewin, void *params)
{
   EDBUG(6, "doToggleFixedPos");

   if (ewin->fixedpos)
      ewin->fixedpos = 0;
   else
      ewin->fixedpos = 1;

   EDBUG_RETURN(0);
}

static int
doSetLayer(EWin * ewin, void *params)
{
   int                 l;

   EDBUG(6, "doSetLayer");

   if (!params)
      EDBUG_RETURN(0);
   l = atoi((char *)params);
   if (ewin->layer > l)
     {
	SoundPlay("SOUND_WINDOW_CHANGE_LAYER_DOWN");
     }
   else if (ewin->layer < l)
     {
	SoundPlay("SOUND_WINDOW_CHANGE_LAYER_UP");
     }
   ewin->layer = l;
   RaiseEwin(ewin);
   HintsSetWindowState(ewin);
   RememberImportantInfoForEwin(ewin);

   EDBUG_RETURN(0);
}

/* Focus actions */

static int
doFocusNext(EWin * edummy, void *params)
{
   EDBUG(6, "doFocusNext");
   if (conf.warplist.enable && mode.current_event->type == KeyPress)
      WarpFocus(1);
   else
      FocusGetNextEwin();
   EDBUG_RETURN(0);
}

static int
doFocusPrev(EWin * edummy, void *params)
{
   EDBUG(6, "doFocusPrev");
   FocusGetPrevEwin();
   EDBUG_RETURN(0);
}

static int
doFocusSet(EWin * ewin, void *params)
{
   EDBUG(6, "doFocusSet");

   GotoDesktopByEwin(ewin);

   if (ewin->iconified)
      DeIconifyEwin(ewin);
   if (ewin->shaded)
      UnShadeEwin(ewin);
   FocusToEWin(ewin, FOCUS_SET);

   EDBUG_RETURN(0);
}

static int
doBackgroundSet(EWin * edummy, void *params)
{
   int                 desk;
   Background         *bg;
   char                view, s[1024];

   EDBUG(6, "doBackgroundSet");

   if (!params)
      EDBUG_RETURN(0);

   desk = desks.current;
   if (sscanf((char *)params, "%1000s %i", s, &desk) < 2)
      desk = desks.current;
   bg = (Background *) FindItem(s, 0, LIST_FINDBY_NAME, LIST_TYPE_BACKGROUND);
   if (!bg)
      EDBUG_RETURN(0);

   if (desks.desk[desk].bg != bg)
     {
	char                pq;

	if (desks.desk[desk].bg)
	   desks.desk[desk].bg->last_viewed = 0;
	view = desks.desk[desk].viewable;
	desks.desk[desk].viewable = 0;
	DesktopAccounting();
	desks.desk[desk].viewable = view;
	BGSettingsGoTo(bg);
	pq = queue_up;
	queue_up = 0;
	SetDesktopBg(desk, bg);
	RefreshDesktop(desk);
	RedrawPagersForDesktop(desk, 2);
	ForceUpdatePagersForDesktop(desk);
	queue_up = pq;
     }
   autosave();

   EDBUG_RETURN(0);
}

/* Area actions */

static int
doAreaSet(EWin * edummy, void *params)
{
   int                 a, b;

   EDBUG(6, "doAreaSet");
   if (!params)
      EDBUG_RETURN(0);
   sscanf((char *)params, "%i %i", &a, &b);
   SetCurrentArea(a, b);

   EDBUG_RETURN(0);
}

static int
doAreaMoveBy(EWin * edummy, void *params)
{
   int                 a, b;

   EDBUG(6, "doAreaMoveBy");
   if (!params)
      EDBUG_RETURN(0);

   sscanf((char *)params, "%i %i", &a, &b);
   MoveCurrentAreaBy(a, b);

   EDBUG_RETURN(0);
}

static int
doLinearAreaSet(EWin * edummy, void *params)
{
   int                 da;

   EDBUG(6, "doLinearAreaSet");
   if (params)
     {
	sscanf((char *)params, "%i", &da);
	SetCurrentLinearArea(da);
     }
   EDBUG_RETURN(0);
}

static int
doLinearAreaMoveBy(EWin * edummy, void *params)
{
   int                 da;

   EDBUG(6, "doLinearAreaMoveBy");
   if (params)
     {
	sscanf((char *)params, "%i", &da);
	MoveCurrentLinearAreaBy(da);
     }
   EDBUG_RETURN(0);
}

static int
doWarpPointer(EWin * edummy, void *params)
{
   int                 dx, dy;

   EDBUG(6, "doWarpPointer");

   if (params)
     {
	sscanf((char *)params, "%i %i", &dx, &dy);
	XWarpPointer(disp, None, None, 0, 0, 0, 0, dx, dy);
     }

   EDBUG_RETURN(0);
}

static int
doMoveWinToArea(EWin * ewin, void *params)
{
   int                 dx, dy;

   EDBUG(6, "doMoveWinToArea");
   if (params)
     {
	sscanf((char *)params, "%i %i", &dx, &dy);
	MoveEwinToArea(ewin, dx, dy);
     }
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

static int
doMoveWinByArea(EWin * ewin, void *params)
{
   int                 dx, dy;

   EDBUG(6, "doMoveWinByArea");
   if (params)
     {
	sscanf((char *)params, "%i %i", &dx, &dy);
	dx = ewin->area_x + dx;
	dy = ewin->area_y + dy;
	MoveEwinToArea(ewin, dx, dy);
     }
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

static int
doMoveWinToLinearArea(EWin * ewin, void *params)
{
   int                 da;

   EDBUG(6, "doMoveWinToLinearArea");
   if (params)
     {
	sscanf((char *)params, "%i", &da);
	MoveEwinToLinearArea(ewin, da);
     }
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

#if 0				/* Not used */
static int
doMoveWinByLinearArea(EWin * ewin, void *params)
{
   EWin               *ewin;
   int                 da;

   EDBUG(6, "doMoveWinByLinearArea");
   if (params)
     {
	sscanf((char *)params, "%i", &da);
	MoveEwinLinearAreaBy(ewin, da);
     }
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}
#endif

static int
DoSetWinBorder(EWin * ewin, void *params, int nogroup)
{
   EWin              **gwins = NULL;
   int                 i, num;
   char                buf[1024], has_shaded;
   Border             *b;
   char                shadechange = 0;

   EDBUG(6, "doSetWinBorder");

   if (!params)
      EDBUG_RETURN(0);

   sscanf((char *)params, "%1000s", buf);
   b = (Border *) FindItem(buf, 0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);
   if (!b)
      EDBUG_RETURN(0);

   has_shaded = 0;
   gwins = ListWinGroupMembersForEwin(ewin, ACTION_SET_WINDOW_BORDER, nogroup,
				      &num);
   for (i = 0; i < num; i++)
     {
	if (gwins[i]->shaded)
	  {
	     has_shaded = 1;
	     break;
	  }
     }
   if (has_shaded)
     {
	if ((b->border.left == 0) && (b->border.right == 0)
	    && (b->border.top == 0) && (b->border.bottom == 0))
	   EDBUG_RETURN(0);
     }
   for (i = 0; i < num; i++)
     {
	if (b != gwins[i]->border)
	  {
	     SoundPlay("SOUND_WINDOW_BORDER_CHANGE");
	     shadechange = 0;
	     if (gwins[i]->shaded)
	       {
		  shadechange = 1;
		  InstantUnShadeEwin(gwins[i]);
	       }
	     EwinSetBorder(gwins[i], b, 1);
	     if (shadechange)
		InstantShadeEwin(gwins[i], 0);
	  }
	RememberImportantInfoForEwin(gwins[i]);
     }
   if (gwins)
      Efree(gwins);
   EDBUG_RETURN(0);
}

static int
doSetWinBorder(EWin * ewin, void *params)
{
   return DoSetWinBorder(ewin, params, 0);
}

static int
doSetWinBorderNoGroup(EWin * ewin, void *params)
{
   return DoSetWinBorder(ewin, params, 1);
}

static int
doAbout(EWin * edummy, void *params)
{
   Dialog             *d;
   DItem              *table, *di;

   EDBUG(6, "doAbout");
   d = FindItem("ABOUT_ENLIGHTENMENT", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG);
   if (d)
     {
	ShowDialog(d);
	EDBUG_RETURN(0);
     }

   d = DialogCreate("ABOUT_ENLIGHTENMENT");
   {
      char                stuff[255];

      Esnprintf(stuff, sizeof(stuff), _("About Enlightenment %s"),
		ENLIGHTENMENT_VERSION);
      DialogSetTitle(d, stuff);
   }

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_IMAGE);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemImageSetFile(di, "pix/about.png");

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemTextSetText(di,
			 _("Welcome to the " ENLIGHTENMENT_VERSION " version\n"
			   "of the Enlightenment "
			   "window manager.\n Enlightenment is still under "
			   "development, but\n"
			   "we have tried to iron out all the bugs "
			   "that\nwe can find. If "
			   "you find a bug in the software,\n please do "
			   "not hesitate to send "
			   "in a bug report.\nSee \"Help\" for information "
			   "on joining the\n" "mailing list.\n" "\n"
			   "This code last updated on:\n" E_CHECKOUT_DATE "\n"
			   "\n" "Good luck. We hope you enjoy the software.\n"
			   "\n" "The Rasterman - raster@rasterman.com\n"
			   "Mandrake - mandrake@mandrake.net\n"));

   DialogAddButton(d, _("OK"), NULL, 1);
   ShowDialog(d);
   EDBUG_RETURN(0);
}

static int
doFX(EWin * edummy, void *params)
{
   EDBUG(6, "doFX");
   if (params)
      FX_Op((const char *)params, FX_OP_TOGGLE);
   autosave();
   EDBUG_RETURN(0);
}

static int
doSetPagerHiq(EWin * edummy, void *params)
{
   EDBUG(6, "doSetPagerHiq");
   if (params)
     {
	char                num;

	num = atoi((char *)params);
	PagerSetHiQ(num);
     }
   autosave();
   EDBUG_RETURN(0);
}

static int
doSetPagerSnap(EWin * edummy, void *params)
{
   EDBUG(6, "doSetPagerSnap");
   if (params)
     {
	char                num;

	num = atoi((char *)params);
	PagerSetSnap(num);
     }
   autosave();
   EDBUG_RETURN(0);
}

static int
doConfigure(EWin * edummy, void *params)
{
   char                s[1024];

   EDBUG(6, "doConfigure");
   if (InZoom())
      EDBUG_RETURN(0);
   sscanf((char *)params, "%1000s", s);
   if (params)
     {
	if (!strcmp(s, "pager"))
	   SettingsPager();
	else if (!strcmp(s, "focus"))
	   SettingsFocus();
	else if (!strcmp(s, "moveresize"))
	   SettingsMoveResize();
	else if (!strcmp(s, "desktops"))
	   SettingsDesktops();
	else if (!strcmp(s, "area"))
	   SettingsArea();
	else if (!strcmp(s, "placement"))
	   SettingsPlacement();
	else if (!strcmp(s, "icons"))
	   SettingsIcons();
	else if (!strcmp(s, "autoraise"))
	   SettingsAutoRaise();
	else if (!strcmp(s, "tooltips"))
	   SettingsTooltips();
	else if (!strcmp(s, "audio"))
	   SettingsAudio();
	else if (!strcmp(s, "fx"))
	   SettingsSpecialFX();
	else if (!strcmp(s, "bg"))
	   SettingsBackground(desks.desk[desks.current].bg);
	else if (!strcmp(s, "iconbox"))
	  {
	     sscanf((char *)params, "%*s %1000s", s);
	     SettingsIconbox(s);
	  }
	else if (!strcmp(s, "group"))
	  {
	     EWin               *ewin = GetFocusEwin();

	     if (ewin)
	       {
		  SettingsGroups(ewin);
	       }
	  }
	else if (!strcmp(s, "group_defaults"))
	  {
	     SettingsDefaultGroupControl();
	  }
	else if (!strcmp(s, "group_membership"))
	  {
	     EWin               *ewin = GetFocusEwin();

	     if (ewin)
	       {
		  ChooseGroupDialog(ewin,
				    _
				    ("  Pick the group the window will belong to:  \n"),
				    GROUP_SELECT_ALL_EXCEPT_EWIN,
				    ACTION_ADD_TO_GROUP);
	       }
	  }
	else if (!strcmp(s, "remember"))
	  {
	     SettingsRemember();
	  }
	else if (!strcmp(s, "miscellaneous"))
	  {
	     SettingsMiscellaneous();
	  }
     }
   EDBUG_RETURN(0);
}

struct _keyset
{
   char               *sym;
   int                 state;
   char               *ch;
};

static int
doInsertKeys(EWin * edummy, void *params)
{
   const struct _keyset ks[] = {
      {"a", 0, "a"},
      {"b", 0, "b"},
      {"c", 0, "c"},
      {"d", 0, "d"},
      {"e", 0, "e"},
      {"f", 0, "f"},
      {"g", 0, "g"},
      {"h", 0, "h"},
      {"i", 0, "i"},
      {"j", 0, "j"},
      {"k", 0, "k"},
      {"l", 0, "l"},
      {"m", 0, "m"},
      {"n", 0, "n"},
      {"o", 0, "o"},
      {"p", 0, "p"},
      {"q", 0, "q"},
      {"r", 0, "r"},
      {"s", 0, "s"},
      {"t", 0, "t"},
      {"u", 0, "u"},
      {"v", 0, "v"},
      {"w", 0, "w"},
      {"x", 0, "x"},
      {"y", 0, "y"},
      {"z", 0, "z"},
      {"a", ShiftMask, "A"},
      {"b", ShiftMask, "B"},
      {"c", ShiftMask, "C"},
      {"d", ShiftMask, "D"},
      {"e", ShiftMask, "E"},
      {"f", ShiftMask, "F"},
      {"g", ShiftMask, "G"},
      {"h", ShiftMask, "H"},
      {"i", ShiftMask, "I"},
      {"j", ShiftMask, "J"},
      {"k", ShiftMask, "K"},
      {"l", ShiftMask, "L"},
      {"m", ShiftMask, "M"},
      {"n", ShiftMask, "N"},
      {"o", ShiftMask, "O"},
      {"p", ShiftMask, "P"},
      {"q", ShiftMask, "Q"},
      {"r", ShiftMask, "R"},
      {"s", ShiftMask, "S"},
      {"t", ShiftMask, "T"},
      {"u", ShiftMask, "U"},
      {"v", ShiftMask, "V"},
      {"w", ShiftMask, "W"},
      {"x", ShiftMask, "X"},
      {"y", ShiftMask, "Y"},
      {"z", ShiftMask, "Z"},
      {"grave", 0, "`"},
      {"1", 0, "1"},
      {"2", 0, "2"},
      {"3", 0, "3"},
      {"4", 0, "4"},
      {"5", 0, "5"},
      {"6", 0, "6"},
      {"7", 0, "7"},
      {"8", 0, "8"},
      {"9", 0, "9"},
      {"0", 0, "0"},
      {"minus", 0, "-"},
      {"equal", 0, "="},
      {"bracketleft", 0, "["},
      {"bracketright", 0, "]"},
      {"backslash", 0, "\\\\"},
      {"semicolon", 0, "\\s"},
      {"apostrophe", 0, "\\a"},
      {"comma", 0, ","},
      {"period", 0, "."},
      {"slash", 0, "/"},
      {"grave", ShiftMask, "~"},
      {"1", ShiftMask, "!"},
      {"2", ShiftMask, "@"},
      {"3", ShiftMask, "#"},
      {"4", ShiftMask, "$"},
      {"5", ShiftMask, "%"},
      {"6", ShiftMask, "^"},
      {"7", ShiftMask, "&"},
      {"8", ShiftMask, "*"},
      {"9", ShiftMask, "("},
      {"0", ShiftMask, ")"},
      {"minus", ShiftMask, "_"},
      {"equal", ShiftMask, "+"},
      {"bracketleft", ShiftMask, "{"},
      {"bracketright", ShiftMask, "}"},
      {"backslash", ShiftMask, "|"},
      {"semicolon", ShiftMask, ":"},
      {"apostrophe", ShiftMask, "\\q"},
      {"comma", ShiftMask, "<"},
      {"period", ShiftMask, ">"},
      {"slash", ShiftMask, "?"},
      {"space", ShiftMask, " "},
      {"Return", ShiftMask, "\\n"},
      {"Tab", ShiftMask, "\\t"}
   };

   EDBUG(6, "doInsertKeys");
   if (params)
     {
	Window              win = 0;
	int                 i, rev;
	char               *s;
	XKeyEvent           ev;

	s = (char *)params;
	XGetInputFocus(disp, &win, &rev);
	if (win)
	  {
	     SoundPlay("SOUND_INSERT_KEYS");
	     ev.window = win;
	     for (i = 0; i < (int)strlen(s); i++)
	       {
		  int                 j;

		  ev.x = mode.x;
		  ev.y = mode.y;
		  ev.x_root = mode.x;
		  ev.y_root = mode.y;
		  for (j = 0; j < (int)(sizeof(ks) / sizeof(struct _keyset));
		       j++)
		    {
		       if (!strncmp(ks[j].ch, &(s[i]), strlen(ks[j].ch)))
			 {
			    i += (strlen(ks[j].ch) - 1);
			    ev.keycode =
			       XKeysymToKeycode(disp,
						XStringToKeysym(ks[j].sym));
			    ev.state = ks[j].state;
			    ev.type = KeyPress;
			    XSendEvent(disp, win, False, 0, (XEvent *) & ev);
			    ev.type = KeyRelease;
			    XSendEvent(disp, win, False, 0, (XEvent *) & ev);
			    j = 0x7fffffff;
			 }
		    }
	       }
	  }
     }
   EDBUG_RETURN(0);
}

static int
doCreateIconbox(EWin * edummy, void *params)
{
   Iconbox            *ib, **ibl;
   int                 num = 0;
   char                s[64];

   EDBUG(6, "doCreateIconbox");
   if (params == NULL)
     {
	ibl = ListAllIconboxes(&num);
	if (ibl)
	   Efree(ibl);
	Esnprintf(s, sizeof(s), "_IB_%i", num);
	params = s;
     }
   SoundPlay("SOUND_NEW_ICONBOX");
   ib = IconboxCreate(s);
   IconboxShow(ib);
   autosave();
   EDBUG_RETURN(0);
}

static int
doShowHideGroup(EWin * ewin, void *params)
{
   EDBUG(6, "doShowGroup");
   ShowHideWinGroups(ewin, NULL, SET_TOGGLE);
   EDBUG_RETURN(0);
}

static int
doStartGroup(EWin * ewin, void *params)
{
   EDBUG(6, "doStartGroup");
   BuildWindowGroup(&ewin, 1);
   SaveGroups();
   EDBUG_RETURN(0);
}

static int
doAddToGroup(EWin * ewin, void *params)
{
   EDBUG(6, "doAddToGroup");
   if (!current_group)
     {
	ChooseGroupDialog(ewin,
			  _("\n  There's no current group at the moment.  \n"
			    "  The current group is the last one you created,  \n"
			    "  and it exists until you create a new one or break  \n"
			    "  the latest one.  \n\n"
			    "  Pick another group that the window will belong to here:  \n\n"),
			  GROUP_SELECT_ALL_EXCEPT_EWIN, ACTION_ADD_TO_GROUP);
	EDBUG_RETURN(0);
     }
   else
      AddEwinToGroup(ewin, current_group);
   SaveGroups();
   EDBUG_RETURN(0);
}

static int
doRemoveFromGroup(EWin * ewin, void *params)
{
   EDBUG(6, "doRemoveFromGroup");
   ChooseGroupDialog(ewin,
		     _("   Select the group to remove the window from.  "),
		     GROUP_SELECT_EWIN_ONLY, ACTION_REMOVE_FROM_GROUP);

   SaveGroups();
   EDBUG_RETURN(0);
}

static int
doBreakGroup(EWin * ewin, void *params)
{
   EDBUG(6, "doBreakGroup");
   ChooseGroupDialog(ewin, _("  Select the group to break  "),
		     GROUP_SELECT_EWIN_ONLY, ACTION_BREAK_GROUP);
   SaveGroups();
   EDBUG_RETURN(0);
}

static int
doZoom(EWin * ewin, void *params)
{
   char                s[1024];

   EDBUG(6, "doZoom");

   if (!(CanZoom()))
      EDBUG_RETURN(0);

   Esnprintf(s, sizeof(s), "%s/.zoom_warn", EDirUser());
   if (!exists(s))
     {
	FILE               *f;

	f = fopen(s, "w");
	if (f)
	  {
	     fprintf(f,
		     _
		     ("You have been warned about the dangers of Zoom mode\n"));
	     fclose(f);
	  }
	DialogOK(_("Warning !!!"),
		 _("This feature is heavily reliant on a feature of your\n"
		   "X Server called the Vid Mode Extension. This feature exists\n"
		   "in XFree86 Servers, but is not a heavily used part of the\n"
		   "Server and thus isn't tested much.\n\n"
		   "It is possible your X Server does not deal well with being\n"
		   "asked to switch modes quickly and it may hang, glitch,\n"
		   "display artefacts or perhaps simply refuse to work.\n\n"
		   "This is a warning and will only be displayed this one time.\n"
		   "If your Server does not behave well then you will probably\n"
		   "have to avoid using this feature.\n"));
	EDBUG_RETURN(0);
     }

   if (InZoom())
      Zoom(NULL);
   else
      Zoom(ewin);
   EDBUG_RETURN(0);
}

static ActionFunction ActionFunctions[ACTION_NUMBEROF] = {
   {0, 1, 1, 0, doNothing},	/* ACTION_NONE */
   {0, 1, 1, 0, doExec},	/* ACTION_EXEC */
   {0, 0, 0, 0, doAlert},	/* ACTION_ALERT */
   {1, 0, 0, 1, doMove},	/* ACTION_MOVE */
   {1, 0, 0, 1, doResize},	/* ACTION_RESIZE */
   {1, 0, 0, 0, doRaise},	/* ACTION_RAISE */
   {1, 0, 0, 0, doLower},	/* ACTION_LOWER */
   {0, 0, 0, 0, doExit},	/* ACTION_EXIT */
   {0, 0, 0, 1, doCleanup},	/* ACTION_CLEANUP */
   {0, 1, 0, 0, spawnMenu},	/* ACTION_SHOW_MENU */
   {0, 1, 1, 0, hideMenu},	/* ACTION_HIDE_MENU */
   {1, 0, 0, 1, doResizeH},	/* ACTION_RESIZE_H */
   {1, 0, 0, 1, doResizeV},	/* ACTION_RESIZE_V */
   {1, 0, 0, 0, doKill},	/* ACTION_KILL */
   {1, 0, 0, 0, doKillNasty},	/* ACTION_KILL_NASTY */
   {0, 0, 1, 0, doNextDesktop},	/* ACTION_DESKTOP_NEXT */
   {0, 0, 1, 0, doPrevDesktop},	/* ACTION_DESKTOP_PREV */
   {0, 0, 0, 0, doRaiseDesktop},	/* ACTION_DESKTOP_RAISE */
   {0, 0, 0, 0, doLowerDesktop},	/* ACTION_DESKTOP_LOWER */
   {0, 0, 0, 0, doDragDesktop},	/* ACTION_DESKTOP_DRAG */
   {1, 0, 0, 0, doStick},	/* ACTION_STICK */
   {0, 0, 0, 0, doInplaceDesktop},	/* ACTION_DESKTOP_INPLACE */
   {0, 0, 0, 0, doDragButtonStart},	/* ACTION_DRAG_BUTTON */
   {0, 0, 0, 0, doFocusModeSet},	/* ACTION_FOCUSMODE_SET */
   {0, 0, 0, 0, doMoveModeSet},	/* ACTION_MOVEMODE_SET */
   {0, 0, 0, 0, doResizeModeSet},	/* ACTION_RESIZEMODE_SET */
   {0, 0, 0, 0, doSlideModeSet},	/* ACTION_SLIDEMODE_SET */
   {0, 0, 0, 0, doCleanupSlideSet},	/* ACTION_CLEANUPSILDE_SET */
   {0, 0, 0, 0, doMapSlideSet},	/* ACTION_MAPSLIDE_SET */
   {0, 0, 0, 0, doSoundSet},	/* ACTION_SOUND_SET */
   {0, 0, 0, 0, doButtonMoveResistSet},	/* ACTION_BUTTONMOVE_RESIST_SET */
   {0, 0, 0, 0, doDesktopBgTimeoutSet},	/* ACTION_DESKTOPBG_TIMEOUT_SET */
   {0, 0, 0, 0, doMapSlideSpeedSet},	/* ACTION_MAPSLIDE_SPEED_SET */
   {0, 0, 0, 0, doCleanupSlideSpeedSet},	/* ACTION_CLEANUPSLIDE_SPEED_SET */
   {0, 0, 0, 0, doDragdirSet},	/* ACTION_DRAGDIR_SET */
   {0, 0, 0, 0, doDragbarOrderSet},	/* ACTION_DRAGBAR_ORDER_SET */
   {0, 0, 0, 0, doDragbarWidthSet},	/* ACTION_DRAGBAR_WIDTH_SET */
   {0, 0, 0, 0, doDragbarLengthSet},	/* ACTION_DRAGBAR_LENGTH_SET */
   {0, 0, 0, 0, doDeskSlideSet},	/* ACTION_DESKSLIDE_SET */
   {0, 0, 0, 0, doDeskSlideSpeedSet},	/* ACTION_DESKSLIDE_SPEED_SET */
   {0, 0, 0, 0, doHiQualityBgSet},	/* ACTION_HIQUALITYBG_SET */
   {0, 0, 0, 0, doPlaySoundClass},	/* ACTION_PLAYSOUNDCLASS */
   {0, 0, 1, 0, doGotoDesktop},	/* ACTION_GOTO_DESK */
   {0, 0, 0, 0, doDeskray},	/* ACTION_DESKRAY */
   {0, 0, 0, 0, doAutosaveSet},	/* ACTION_AUTOSAVE_SET */
   {0, 0, 0, 0, doHideShowButton},	/* ACTION_HIDESHOW_BUTTON */
   {1, 0, 0, 0, doIconifyWindow},	/* ACTION_ICONIFY */
   {0, 0, 0, 0, doSlideout},	/* ACTION_SLIDEOUT */
   {0, 0, 0, 1, doScrollWindows},	/* ACTION_SCROLL_WINDOWS */
   {1, 0, 0, 0, doShade},	/* ACTION_SHADE */
   {1, 0, 0, 0, doMaxH},	/* ACTION_MAX_HEIGHT */
   {1, 0, 0, 0, doMaxW},	/* ACTION_MAX_WIDTH */
   {1, 0, 0, 0, doMax},		/* ACTION_MAX_SIZE */
   {1, 0, 0, 0, doSendToNextDesk},	/* ACTION_SEND_TO_NEXT_DESK */
   {1, 0, 0, 0, doSendToPrevDesk},	/* ACTION_SEND_TO_PREV_DESK */
   {1, 0, 0, 0, doSnapshot},	/* ACTION_SNAPSHOT */
   {0, 0, 0, 1, doScrollContainer},	/* ACTION_SCROLL_CONTAINER */
   {0, 0, 0, 0, doToolTipSet},	/* ACTION_TOOLTIP_SET */
   {0, 0, 0, 0, doFocusNext},	/* ACTION_FOCUS_NEXT */
   {0, 0, 0, 0, doFocusPrev},	/* ACTION_FOCUS_PREV */
   {1, 0, 0, 0, doFocusSet},	/* ACTION_FOCUS_SET */
   {0, 0, 0, 0, doBackgroundSet},	/* ACTION_BACKGROUND_SET */
   {0, 0, 1, 0, doAreaSet},	/* ACTION_AREA_SET */
   {0, 0, 1, 0, doAreaMoveBy},	/* ACTION_MOVE_BY */
   {0, 0, 0, 0, doToggleFixedPos},	/* ACTION_TOGGLE_FIXED */
   {1, 0, 0, 0, doSetLayer},	/* ACTION_SET_LAYER */
   {0, 0, 0, 0, doWarpPointer},	/* ACTION_WARP_POINTER */
   {1, 0, 0, 0, doMoveWinToArea},	/* ACTION_MOVE_WINDOW_TO_AREA */
   {1, 0, 0, 0, doMoveWinByArea},	/* ACTION_MOVE_WINDOW_BY_AREA */
   {1, 0, 0, 0, doSetWinBorder},	/* ACTION_SET_WINDOW_BORDER */
   {0, 0, 1, 0, doLinearAreaSet},	/* ACTION_LINEAR_AREA_SET */
   {0, 0, 1, 0, doLinearAreaMoveBy},	/* ACTION_LINEAR_MOVE_BY */
   {0, 0, 0, 0, doAbout},	/* ACTION_ABOUT */
   {0, 0, 0, 0, doFX},		/* ACTION_FX */
   {1, 0, 0, 0, doMoveWinToLinearArea},	/* ACTION_MOVE_WINDOW_TO_LINEAR_AREA */
   {1, 0, 0, 0, doMoveWinByArea},	/* ACTION_MOVE_WINDOW_BY_LINEAR_AREA */
   {0, 0, 0, 0, doSetPagerHiq},	/* ACTION_SET_PAGER_HIQ */
   {0, 0, 0, 0, doSetPagerSnap},	/* ACTION_SET_PAGER_SNAP */
   {0, 0, 0, 0, doConfigure},	/* ACTION_CONFIG */
   {1, 0, 0, 1, doMoveConstrained},	/* ACTION_MOVE_CONSTRAINED */
   {0, 0, 0, 0, doInsertKeys},	/* ACTION_INSERT_KEYS */
   {1, 0, 0, 0, doStartGroup},	/* ACTION_START_GROUP */
   {1, 0, 0, 0, doAddToGroup},	/* ACTION_ADD_TO_GROUP */
   {1, 0, 0, 0, doRemoveFromGroup},	/* ACTION_REMOVE_FROM_GROUP */
   {1, 0, 0, 0, doBreakGroup},	/* ACTION_BREAK_GROUP */
   {1, 0, 0, 0, doShowHideGroup},	/* ACTION_SHOW_HIDE_GROUP */
   {0, 0, 0, 0, doCreateIconbox},	/* ACTION_CREATE_ICONBOX */
   {1, 0, 0, 0, doRaiseLower},	/* ACTION_RAISE_LOWER */
   {1, 1, 0, 1, doZoom},	/* ACTION_ZOOM */
   {1, 0, 0, 0, doSetWinBorderNoGroup},	/* ACTION_SET_WINDOW_BORDER_NG */
   {1, 0, 0, 0, doIconifyWindowNoGroup},	/* ACTION_ICONIFY_NG */
   {1, 0, 0, 0, doKillNoGroup},	/* ACTION_KILL_NG */
   {1, 0, 0, 1, doMoveNoGroup},	/* ACTION_MOVE_NG */
   {1, 0, 0, 0, doRaiseNoGroup},	/* ACTION_RAISE_NG */
   {1, 0, 0, 0, doLowerNoGroup},	/* ACTION_LOWER_NG */
   {1, 0, 0, 0, doStickNoGroup},	/* ACTION_STICK_NG */
   {1, 0, 0, 0, doShadeNoGroup},	/* ACTION_SHADE_NG */
   {1, 0, 0, 0, doRaiseLowerNoGroup},	/* ACTION_RAISE_LOWER_NG */
   {1, 0, 0, 0, doSkipFocus},	/* ACTION_SKIPFOCUS */
   {1, 0, 0, 0, doSkipTask},	/* ACTION_SKIPTASK */
   {1, 0, 0, 0, doSkipWinList},	/* ACTION_SKIPWINLIST */
   {1, 0, 0, 0, doNeverFocus},	/* ACTION_NEVERFOCUS */
   {1, 0, 0, 0, doSkipLists},	/* ACTION_SKIPLISTS */
   {1, 0, 0, 1, doSwapMove}	/* ACTION_SWAPMOVE */
};
