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
#include "timestamp.h"
#ifdef __EMX__
#include <process.h>
#endif

static char         mode_action_destroy = 0;

static int          handleAction(ActionType * Action);

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
		       if (pager_hi_win)
			  XGrabButton(disp, button, mod, pager_hi_win,
				      False, mask, GrabModeSync, GrabModeAsync,
				      None, None);
		       XGrabButton(disp, button, mod, ewin->win, False, mask,
				   GrabModeSync, GrabModeAsync, None, None);
		    }
		  else
		    {
		       int                 i;

		       for (i = 0; i < 8; i++)
			 {
			    if (pager_hi_win)
			       XGrabButton(disp, button,
					   mod | mask_mod_combos[i],
					   pager_hi_win, False, mask,
					   GrabModeSync, GrabModeAsync, None,
					   None);
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

void
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

void
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
EventAclass(XEvent * ev, ActionClass * a)
{
   KeyCode             key;
   int                 i, type, button, modifiers, ok, mouse, mask, val = 0;
   Action             *act;
   char                reset_ewin;

   EDBUG(5, "EventAclass");
   reset_ewin = key = type = button = modifiers = mouse = 0;
   if (!mode.ewin)
     {
	mode.ewin = mode.focuswin;
	if (!mode.ewin)
	   mode.ewin = mode.mouse_over_win;
	reset_ewin = 1;
     }
   {
      EWin               *ewin;

      ewin = mode.ewin;
      if ((mode.movemode == 0) && (ewin) && (mode.mode == MODE_MOVE))
	 DetermineEwinFloat(ewin, 0, 0);
   }

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
		       handleAction(act->action);
		       val = 1;
		    }
	       }
	  }
	if (mode_action_destroy)
	   break;
     }
   if (reset_ewin)
      mode.ewin = NULL;
   mode_action_destroy = 0;
   EDBUG_RETURN(val);
}

static int
handleAction(ActionType * Action)
{

   /* This function will handle any type of action that is passed into
    * it.  ALL internal events should be passed through this function.
    * No exceptions.  --Mandrake (02/26/98)
    */

   int                 error;

   EDBUG(5, "handleAction");
   error = (*(ActionFunctions[Action->Type])) (Action->params);

   /* Did we just hose ourselves?
    * if so, we'd best not stick around here 
    */

   if (mode_action_destroy)
      EDBUG_RETURN(0);
   /* If there is another action in this series, (now that
    * we're sure we didn't already die) perform it
    */
   if (!error)
      if (Action->Next)
	 error = handleAction(Action->Next);
   EDBUG_RETURN(error);
}

int
spawnMenu(void *params)
{
   char                s[1024];
   char                s2[1024];
   int                 x, y, di;
   Window              dw;
   unsigned int        w, h, d;
   EWin               *ewin;
   char                desk_click = 0;
   int                 i;

   EDBUG(6, "spawnMenu");

   if (!params)
      EDBUG_RETURN(0);

   if (mode.cur_menu_depth > 0)
      EDBUG_RETURN(0);

   ewin = mode.ewin = GetFocusEwin();
   for (i = 0; i < mode.numdesktops; i++)
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

int
hideMenu(void *params)
{
   EDBUG(6, "hideMenu");
   params = NULL;
   EDBUG_RETURN(0);
}

int
doNothing(void *params)
{
   EDBUG(6, "doNothing");
   params = NULL;
   EDBUG_RETURN(0);
}

int
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

int
alert(void *params)
{
   char               *pp;
   int                 i;

   EDBUG(6, "alert");
   if (InZoom())
      Zoom(NULL);
   if ((mode.mode == MODE_MOVE) || (mode.mode == MODE_RESIZE_H)
       || (mode.mode == MODE_RESIZE_V) || (mode.mode == MODE_RESIZE))
      EDBUG_RETURN(0);
   pp = duplicate((char *)params);
   i = 1;
   if (!pp)
      EDBUG_RETURN(1);
   if (strlen(pp) <= 0)
      EDBUG_RETURN(1);
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

int
doExit(void *params)
{
   EDBUG(6, "doExit");
   if ((mode.mode == MODE_MOVE) || (mode.mode == MODE_RESIZE_H)
       || (mode.mode == MODE_RESIZE_V) || (mode.mode == MODE_RESIZE))
      EDBUG_RETURN(0);
   /* This function is now handled in session.c */
   if (InZoom())
      Zoom(NULL);
   doSMExit(params);
   EDBUG_RETURN(0);
}

int
doResize(void *params)
{
   EWin               *ewin;
   int                 x, y, w, h;

   EDBUG(6, "doResize");
   if (InZoom())
      EDBUG_RETURN(0);
   if ((mode.mode == MODE_MOVE) || (mode.mode == MODE_RESIZE_H)
       || (mode.mode == MODE_RESIZE_V) || (mode.mode == MODE_RESIZE))
      EDBUG_RETURN(0);
   if (mode.slideout)
      HideSlideout(mode.slideout, mode.context_win);
   ewin = mode.ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);
   if (ewin->shaded)
      EDBUG_RETURN(0);
   if (mode.resizemode > 0)
     {
	FX_Pause();
	GrabX();
     }
   queue_up = 0;
   SoundPlay("SOUND_RESIZE_START");
   UnGrabTheButtons();
   GrabConfineThePointer(root.win);
   mode.mode = MODE_RESIZE;
   x = mode.x - ewin->x;
   y = mode.y - ewin->y;
   w = ewin->w >> 1;
   h = ewin->h >> 1;
   if ((x < w) && (y < h))
      mode.resize_detail = 0;
   if ((x >= w) && (y < h))
      mode.resize_detail = 1;
   if ((x < w) && (y >= h))
      mode.resize_detail = 2;
   if ((x >= w) && (y >= h))
      mode.resize_detail = 3;
   mode.start_x = mode.x;
   mode.start_y = mode.y;
   mode.win_x = ewin->x;
   mode.win_y = ewin->y;
   mode.win_w = ewin->client.w;
   mode.win_h = ewin->client.h;
   mode.firstlast = 0;
   DrawEwinShape(ewin, mode.resizemode, ewin->x, ewin->y, ewin->client.w,
		 ewin->client.h, mode.firstlast);
   mode.firstlast = 1;
   params = NULL;
   EDBUG_RETURN(0);
}

int
doResizeH(void *params)
{
   EWin               *ewin;
   int                 x, w;

   EDBUG(6, "doResizeH");
   if (InZoom())
      EDBUG_RETURN(0);
   if ((mode.mode == MODE_MOVE) || (mode.mode == MODE_RESIZE_H)
       || (mode.mode == MODE_RESIZE_V) || (mode.mode == MODE_RESIZE))
      EDBUG_RETURN(0);
   if (mode.slideout)
      HideSlideout(mode.slideout, mode.context_win);
   ewin = mode.ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);
   if (ewin->shaded)
      EDBUG_RETURN(0);
   if (mode.resizemode > 0)
     {
	FX_Pause();
	GrabX();
     }
   queue_up = 0;
   SoundPlay("SOUND_RESIZE_START");
   UnGrabTheButtons();
   GrabConfineThePointer(root.win);
   mode.mode = MODE_RESIZE_H;
   x = mode.x - ewin->x;
   w = ewin->w >> 1;
   if (x < w)
      mode.resize_detail = 0;
   else
      mode.resize_detail = 1;
   mode.start_x = mode.x;
   mode.start_y = mode.y;
   mode.win_x = ewin->x;
   mode.win_y = ewin->y;
   mode.win_w = ewin->client.w;
   mode.win_h = ewin->client.h;
   mode.firstlast = 0;
   DrawEwinShape(ewin, mode.resizemode, ewin->x, ewin->y, ewin->client.w,
		 ewin->client.h, mode.firstlast);
   mode.firstlast = 1;
   params = NULL;
   EDBUG_RETURN(0);
}

int
doResizeV(void *params)
{
   EWin               *ewin;
   int                 y, h;

   EDBUG(6, "doResizeV");
   if (InZoom())
      EDBUG_RETURN(0);
   if ((mode.mode == MODE_MOVE) || (mode.mode == MODE_RESIZE_H)
       || (mode.mode == MODE_RESIZE_V) || (mode.mode == MODE_RESIZE))
      EDBUG_RETURN(0);
   if (mode.slideout)
      HideSlideout(mode.slideout, mode.context_win);
   ewin = mode.ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);
   if (ewin->shaded)
      EDBUG_RETURN(0);
   if (mode.resizemode > 0)
     {
	FX_Pause();
	GrabX();
     }
   queue_up = 0;
   SoundPlay("SOUND_RESIZE_START");
   UnGrabTheButtons();
   GrabConfineThePointer(root.win);
   mode.mode = MODE_RESIZE_V;
   y = mode.y - ewin->y;
   h = ewin->h >> 1;
   if (y < h)
      mode.resize_detail = 0;
   else
      mode.resize_detail = 1;
   mode.start_x = mode.x;
   mode.start_y = mode.y;
   mode.win_x = ewin->x;
   mode.win_y = ewin->y;
   mode.win_w = ewin->client.w;
   mode.win_h = ewin->client.h;
   mode.firstlast = 0;
   DrawEwinShape(ewin, mode.resizemode, ewin->x, ewin->y, ewin->client.w,
		 ewin->client.h, mode.firstlast);
   mode.firstlast = 1;
   params = NULL;
   EDBUG_RETURN(0);
}

int
doResizeEnd(void *params)
{
   EWin               *ewin;
   int                 i;

   EDBUG(0, "doResizeEnd");
   ewin = GetFocusEwin();
   UnGrabTheButtons();
   SoundPlay("SOUND_RESIZE_STOP");
   if (!ewin)
     {
	if (mode.resizemode > 0)
	   UngrabX();
	ForceUpdatePagersForDesktop(desks.current);
	EDBUG_RETURN(0);
     }
   queue_up = DRAW_QUEUE_ENABLE;
   mode.mode = MODE_NONE;
   if (mode.noewin)
      mode.ewin = NULL;
   mode.noewin = 0;
   mode.firstlast = 2;
   DrawEwinShape(ewin, mode.resizemode, ewin->x, ewin->y, ewin->client.w,
		 ewin->client.h, mode.firstlast);
   for (i = 0; i < ewin->border->num_winparts; i++)
      ewin->bits[i].no_expose = 1;
   ICCCM_Configure(ewin);
   HideCoords();
   XSync(disp, False);
   if (mode.resizemode > 0)
     {
	FX_Pause();
	UngrabX();
     }
   mode.firstlast = 0;
   params = NULL;
   ForceUpdatePagersForDesktop(desks.current);
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

static int          start_move_desk = 0;
static int          start_move_x = 0;
static int          start_move_y = 0;
static int          real_move_mode = 0;

static int
doMoveImpl(void *params, char constrained)
{
   EWin              **gwins;
   EWin               *ewin;
   int                 i, num;

   EDBUG(6, "doMove");
   if (InZoom())
      EDBUG_RETURN(0);
   if ((mode.mode == MODE_MOVE) || (mode.mode == MODE_RESIZE_H)
       || (mode.mode == MODE_RESIZE_V) || (mode.mode == MODE_RESIZE))
      EDBUG_RETURN(0);
   if (mode.slideout)
      HideSlideout(mode.slideout, mode.context_win);
   ewin = mode.ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);
   if (ewin->fixedpos)
      EDBUG_RETURN(0);
   mode.moveresize_pending_ewin = ewin;
   real_move_mode = mode.movemode;
   if (((ewin->groups) || (ewin->has_transients)) && (mode.movemode > 0))
      mode.movemode = 0;
   if (mode.movemode > 0)
     {
	FX_Pause();
	GrabX();
     }
   UnGrabTheButtons();
   GrabConfineThePointer(root.win);
   SoundPlay("SOUND_MOVE_START");
   mode.mode = MODE_MOVE;
   mode.constrained = constrained;
   mode.start_x = mode.x;
   mode.start_y = mode.y;
   mode.win_x = ewin->x;
   mode.win_y = ewin->y;
   mode.win_w = ewin->client.w;
   mode.win_h = ewin->client.h;
   mode.firstlast = 0;
   start_move_desk = ewin->desktop;

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_MOVE, mode.nogroup
				      || mode.swapmovemode, &num);
   for (i = 0; i < num; i++)
     {
	FloatEwinAt(gwins[i], gwins[i]->x, gwins[i]->y);
	if (!mode.moveresize_pending_ewin)
	   DrawEwinShape(gwins[i], mode.movemode, gwins[i]->x, gwins[i]->y,
			 gwins[i]->client.w, gwins[i]->client.h,
			 mode.firstlast);
     }
   Efree(gwins);
   mode.firstlast = 1;
   params = NULL;
   mode.swapcoord_x = start_move_x = ewin->x;
   mode.swapcoord_y = start_move_y = ewin->y;
   EDBUG_RETURN(0);
}

int
doMove(void *params)
{
   return doMoveImpl(params, 0);
}

int
doMoveConstrained(void *params)
{
   return doMoveImpl(params, 1);
}

int
doMoveNoGroup(void *params)
{
   mode.nogroup = 1;
   return doMoveImpl(params, 0);
}

int
doSwapMove(void *params)
{
   mode.swapmovemode = 1;
   return doMoveImpl(params, 0);
}

int
doMoveConstrainedNoGroup(void *params)
{
   mode.nogroup = 1;
   return doMoveImpl(params, 1);
}

int
doMoveEnd(void *params)
{
   EWin              **gwins;
   EWin               *ewin;
   int                 d, wasresize = 0, num, i;

   EDBUG(6, "doMoveEnd");
   ewin = GetFocusEwin();
   UnGrabTheButtons();
   SoundPlay("SOUND_MOVE_STOP");
   if (!ewin)
     {
	if (mode.movemode > 0)
	   UngrabX();
	if (!mode.moveresize_pending_ewin)
	   ForceUpdatePagersForDesktop(desks.current);
	mode.movemode = real_move_mode;
	EDBUG_RETURN(0);
     }
   mode.mode = MODE_NONE;
   if (mode.noewin)
      mode.ewin = NULL;
   mode.noewin = 0;
   mode.firstlast = 2;
   d = DesktopAt(mode.x, mode.y);

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_MOVE, mode.nogroup
				      || mode.swapmovemode, &num);

   if (!mode.moveresize_pending_ewin)
     {
	wasresize = 1;
	for (i = 0; i < num; i++)
	   DrawEwinShape(gwins[i], mode.movemode, gwins[i]->x, gwins[i]->y,
			 gwins[i]->client.w, gwins[i]->client.h,
			 mode.firstlast);
	for (i = 0; i < num; i++)
	   MoveEwin(gwins[i], gwins[i]->x, gwins[i]->y);
     }
   mode.moveresize_pending_ewin = NULL;
   for (i = 0; i < num; i++)
     {
	if ((gwins[i]->floating) || (mode.movemode > 0))
	  {
	     if (gwins[i]->floating)
		MoveEwinToDesktopAt(gwins[i], d,
				    gwins[i]->x - (desks.desk[d].x -
						   desks.
						   desk[gwins[i]->desktop].x),
				    gwins[i]->y - (desks.desk[d].y -
						   desks.
						   desk[gwins[i]->desktop].y));
	     else
		MoveEwinToDesktopAt(gwins[i], d, gwins[i]->x, gwins[i]->y);
	     gwins[i]->floating = 0;
	  }
	if ((mode.movemode > 0) && (gwins[i]->has_transients))
	  {
	     EWin              **lst;
	     int                 j, num2;
	     int                 dx, dy;

	     dx = ewin->x - start_move_x;
	     dy = ewin->y - start_move_y;

	     lst = ListTransientsFor(gwins[i]->client.win, &num2);
	     if (lst)
	       {
		  for (j = 0; j < num2; j++)
		     MoveEwin(lst[j], lst[j]->x + dx, lst[j]->y + dy);
		  Efree(lst);
	       }
	  }
	RaiseEwin(gwins[i]);
	ICCCM_Configure(gwins[i]);
     }
   mode.firstlast = 0;
   HideCoords();
   XSync(disp, False);
   if (mode.movemode > 0)
     {
	FX_Pause();
	UngrabX();
     }
   RememberImportantInfoForEwins(ewin);
   if (wasresize)
      ForceUpdatePagersForDesktop(desks.current);
   Efree(gwins);
   mode.movemode = real_move_mode;
   params = NULL;
   mode.nogroup = 0;
   mode.swapmovemode = 0;
   EDBUG_RETURN(0);
}

int
doRaise(void *params)
{
   EWin               *ewin;
   EWin              **gwins = NULL;
   int                 i, num;

   EDBUG(6, "doRaise");

   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
      ewin =
	 FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
   else
      ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);

   SoundPlay("SOUND_RAISE");

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_RAISE, mode.nogroup, &num);
   for (i = 0; i < num; i++)
      RaiseEwin(gwins[i]);
   Efree(gwins);
   EDBUG_RETURN(0);
}

int
doRaiseNoGroup(void *params)
{
   int                 result;

   mode.nogroup = 1;
   result = doRaise(params);
   mode.nogroup = 0;
   return result;
}

int
doLower(void *params)
{
   EWin               *ewin;
   EWin              **gwins = NULL;
   int                 i, num;

   EDBUG(6, "doLower");

   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
      ewin =
	 FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
   else
      ewin = GetFocusEwin();

   if (!ewin)
      EDBUG_RETURN(0);

   SoundPlay("SOUND_LOWER");

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_LOWER, mode.nogroup, &num);
   for (i = 0; i < num; i++)
      LowerEwin(gwins[i]);
   Efree(gwins);
   EDBUG_RETURN(0);
}

int
doLowerNoGroup(void *params)
{
   int                 result;

   mode.nogroup = 1;
   result = doLower(params);
   mode.nogroup = 0;
   return result;
}

int
doCleanup(void *params)
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

   if (InZoom())
      EDBUG_RETURN(0);
   if ((mode.mode == MODE_MOVE) || (mode.mode == MODE_RESIZE_H)
       || (mode.mode == MODE_RESIZE_V) || (mode.mode == MODE_RESIZE))
      EDBUG_RETURN(0);

   type = (char *)params;
   method = ARRANGE_BY_SIZE;
   speed = mode.slidespeedcleanup;
   doslide = mode.cleanupslide;

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

int
doKill(void *params)
{
   EWin               *ewin;

   EDBUG(6, "doKill");
   if (params)
     {
	ewin =
	   FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
     }
   else
     {
	ewin = GetFocusEwin();
     }

   KillEwin(ewin);

   EDBUG_RETURN(0);
}

int
doKillNoGroup(void *params)
{
   int                 result;

   mode.nogroup = 1;
   result = doKill(params);
   mode.nogroup = 0;
   return result;
}

int
doKillNasty(void *params)
{
   EWin               *ewin;

   EDBUG(6, "doKillNasty");

   if (params)
      ewin =
	 FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
   else
      ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);

   SoundPlay("SOUND_WINDOW_CLOSE");
   EDestroyWindow(disp, ewin->client.win);
   EDBUG_RETURN(0);
}

int
doNextDesktop(void *params)
{
   int                 pd, nd;

   EDBUG(6, "doNextDesktop");

   if (InZoom())
      EDBUG_RETURN(0);

   pd = desks.current;
   nd = desks.current + 1;
   if (mode.desktop_wraparound && (nd >= mode.numdesktops))
      nd = 0;
   GotoDesktop(nd);

   if (desks.current != pd)
      SoundPlay("SOUND_DESKTOP_SHUT");

   EDBUG_RETURN(0);
}

int
doPrevDesktop(void *params)
{
   int                 pd, nd;

   EDBUG(6, "doPrevDesktop");

   if (InZoom())
      EDBUG_RETURN(0);

   pd = desks.current;
   nd = desks.current - 1;
   if (mode.desktop_wraparound && (nd < 0))
      nd = mode.numdesktops - 1;
   GotoDesktop(nd);

   if (desks.current != pd)
      SoundPlay("SOUND_DESKTOP_SHUT");

   EDBUG_RETURN(0);
}

int
doRaiseDesktop(void *params)
{
   int                 d = 0;

   EDBUG(6, "doRaiseDesktop");

   if (InZoom())
      EDBUG_RETURN(0);
   if ((mode.mode == MODE_MOVE) || (mode.mode == MODE_RESIZE_H)
       || (mode.mode == MODE_RESIZE_V) || (mode.mode == MODE_RESIZE))
      EDBUG_RETURN(0);

   if (!params)
      d = desks.current;
   else
      d = atoi((char *)params);
   SoundPlay("SOUND_DESKTOP_RAISE");
   RaiseDesktop(d);
   EDBUG_RETURN(0);
}

int
doLowerDesktop(void *params)
{
   int                 d = 0;

   EDBUG(6, "doLowerDesktop");

   if (InZoom())
      EDBUG_RETURN(0);
   if ((mode.mode == MODE_MOVE) || (mode.mode == MODE_RESIZE_H)
       || (mode.mode == MODE_RESIZE_V) || (mode.mode == MODE_RESIZE))
      EDBUG_RETURN(0);

   if (!params)
      d = desks.current;
   else
      d = atoi((char *)params);
   SoundPlay("SOUND_DESKTOP_LOWER");
   LowerDesktop(d);
   EDBUG_RETURN(0);
}

int
doDragDesktop(void *params)
{
   int                 d = 0;

   EDBUG(6, "doDragDesktop");

   if (InZoom())
      EDBUG_RETURN(0);
   if ((mode.mode == MODE_MOVE) || (mode.mode == MODE_RESIZE_H)
       || (mode.mode == MODE_RESIZE_V) || (mode.mode == MODE_RESIZE))
      EDBUG_RETURN(0);

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

int
doStick(void *params)
{
   EWin               *ewin;
   EWin              **gwins = NULL;
   Group              *curr_group = NULL;
   int                 i, num;
   char                sticky;

   EDBUG(6, "doStick");

   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
      ewin =
	 FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
   else
      ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_STICK, mode.nogroup, &num);
   sticky = ewin->sticky;

   for (i = 0; i < num; i++)
     {
	curr_group = EwinsInGroup(ewin, gwins[i]);
	if (gwins[i]->sticky
	    && ((curr_group && !curr_group->cfg.mirror) || sticky))
	   MakeWindowUnSticky(gwins[i]);
	else if (!gwins[i]->sticky
		 && ((curr_group && !curr_group->cfg.mirror) || !sticky))
	   MakeWindowSticky(gwins[i]);
	params = NULL;
	RememberImportantInfoForEwin(gwins[i]);
     }
   Efree(gwins);
   EDBUG_RETURN(0);
}

int
doStickNoGroup(void *params)
{
   int                 result;

   mode.nogroup = 1;
   result = doStick(params);
   mode.nogroup = 0;
   return result;
}

int
doSkipLists(void *params)
{
   EWin               *ewin;
   char                skip;

   EDBUG(6, "doSkipLists");

   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
      ewin =
	 FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
   else
      ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);

   skip = ewin->skipfocus;

   ewin->skiptask = !(skip);
   ewin->skipwinlist = !(skip);
   ewin->skipfocus = !(skip);
   params = NULL;
   HintsSetWindowState(ewin);
   HintsSetClientList();
   RememberImportantInfoForEwin(ewin);

   EDBUG_RETURN(0);
}

int
doSkipTask(void *params)
{
   EWin               *ewin;
   char                skiptask;

   EDBUG(6, "doSkipTask");

   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
      ewin =
	 FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
   else
      ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);

   skiptask = ewin->skiptask;

   ewin->skiptask = !(skiptask);
   params = NULL;
   HintsSetWindowState(ewin);
   HintsSetClientList();
   RememberImportantInfoForEwin(ewin);

   EDBUG_RETURN(0);
}

int
doSkipFocus(void *params)
{
   EWin               *ewin;
   char                skipfocus;

   EDBUG(6, "doSkipFocus");

   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
      ewin =
	 FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
   else
      ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);

   skipfocus = ewin->skipfocus;

   ewin->skipfocus = !(skipfocus);

   params = NULL;
   HintsSetWindowState(ewin);
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

int
doSkipWinList(void *params)
{
   EWin               *ewin;
   char                skipwinlist;

   EDBUG(6, "doSkipWinList");

   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
      ewin =
	 FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
   else
      ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);

   skipwinlist = ewin->skipwinlist;

   ewin->skipwinlist = !(skipwinlist);
   params = NULL;
   HintsSetWindowState(ewin);
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

int
doNeverFocus(void *params)
{
   EWin               *ewin;
   char                neverfocus;

   EDBUG(6, "doSkipWinList");

   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
      ewin =
	 FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
   else
      ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);

   neverfocus = ewin->neverfocus;

   ewin->neverfocus = !(neverfocus);
   params = NULL;
   HintsSetWindowState(ewin);
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

int
doInplaceDesktop(void *params)
{
   int                 d, pd;

   EDBUG(6, "doInplaceDesktop");

   if (InZoom())
      EDBUG_RETURN(0);
   if (!params)
      d = desks.current;
   else
      d = atoi((char *)params);

   pd = desks.current;
   GotoDesktop(d);
   if (desks.current != pd)
     {
	SoundPlay("SOUND_DESKTOP_SHUT");
     }
   EDBUG_RETURN(0);
}

int
doDragButtonStart(void *params)
{
   Button             *b;

   EDBUG(6, "doDragButtonStart");

   if (InZoom())
      EDBUG_RETURN(0);
   if ((mode.mode == MODE_MOVE) || (mode.mode == MODE_RESIZE_H)
       || (mode.mode == MODE_RESIZE_V) || (mode.mode == MODE_RESIZE))
      EDBUG_RETURN(0);

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

int
doFocusModeSet(void *params)
{
   EDBUG(6, "doFocusModeSet");
   if (params)
     {
	if (!strcmp("pointer", (char *)params))
	   mode.focusmode = FOCUS_POINTER;
	else if (!strcmp("sloppy", (char *)params))
	   mode.focusmode = FOCUS_SLOPPY;
	else if (!strcmp("click", (char *)params))
	   mode.focusmode = FOCUS_CLICK;
     }
   else
     {
	if (mode.focusmode == FOCUS_POINTER)
	   mode.focusmode = FOCUS_SLOPPY;
	else if (mode.focusmode == FOCUS_SLOPPY)
	   mode.focusmode = FOCUS_CLICK;
	else if (mode.focusmode == FOCUS_CLICK)
	   mode.focusmode = FOCUS_POINTER;
     }
   FixFocus();
   autosave();
   EDBUG_RETURN(0);
}

int
doMoveModeSet(void *params)
{
   EDBUG(6, "doMoveModeSet");
   if ((mode.mode == MODE_MOVE) || (mode.mode == MODE_RESIZE_H)
       || (mode.mode == MODE_RESIZE_V) || (mode.mode == MODE_RESIZE))
      EDBUG_RETURN(0);
   if (params)
     {
	mode.movemode = atoi((char *)params);
     }
   else
     {
	mode.movemode++;
	if (mode.movemode > 5)
	   mode.movemode = 0;
     }
#if !USE_IMLIB2
   if ((prImlib_Context) && (mode.movemode == 5))
      mode.movemode = 3;
#endif
   autosave();
   EDBUG_RETURN(0);
}

int
doResizeModeSet(void *params)
{
   EDBUG(6, "doResizeModeSet");
   if ((mode.mode == MODE_MOVE) || (mode.mode == MODE_RESIZE_H)
       || (mode.mode == MODE_RESIZE_V) || (mode.mode == MODE_RESIZE))
      EDBUG_RETURN(0);
   if (params)
     {
	mode.resizemode = atoi((char *)params);
     }
   else
     {
	mode.resizemode++;
	if (mode.resizemode > 4)
	   mode.resizemode = 0;
     }
   if (mode.resizemode == 5)
      mode.resizemode = 3;
   autosave();
   EDBUG_RETURN(0);
}

int
doSlideModeSet(void *params)
{
   EDBUG(6, "doSlideModeSet");
   if (params)
     {
	mode.slidemode = atoi((char *)params);
     }
   else
     {
	mode.slidemode++;
	if (mode.slidemode > 4)
	   mode.slidemode = 0;
     }
   autosave();
   EDBUG_RETURN(0);
}

int
doCleanupSlideSet(void *params)
{
   EDBUG(6, "doCleanupSlideSet");
   if (params)
     {
	mode.cleanupslide = atoi((char *)params);
     }
   else
     {
	if (mode.cleanupslide)
	   mode.cleanupslide = 0;
	else
	   mode.cleanupslide = 1;
     }
   autosave();
   EDBUG_RETURN(0);
}

int
doMapSlideSet(void *params)
{
   EDBUG(6, "doMapSlideSet");
   if (params)
      mode.mapslide = atoi((char *)params);
   else
     {
	if (mode.mapslide)
	   mode.mapslide = 0;
	else
	   mode.mapslide = 1;
     }
   autosave();
   EDBUG_RETURN(0);
}

int
doSoundSet(void *params)
{
   char                snd;

   EDBUG(6, "doSoundSet");
   snd = mode.sound;
   if (params)
      mode.sound = atoi((char *)params);
   else
     {
	if (mode.sound)
	   mode.sound = 0;
	else
	   mode.sound = 1;
     }
   if (mode.sound != snd)
     {
	if (mode.sound)
	   SoundInit();
	else
	   SoundExit();
     }
   autosave();
   EDBUG_RETURN(0);
}

int
doButtonMoveResistSet(void *params)
{
   EDBUG(6, "doButtonMoveResistSet");
   if (params)
      mode.button_move_resistance = atoi((char *)params);
   autosave();
   EDBUG_RETURN(0);
}

int
doDesktopBgTimeoutSet(void *params)
{
   EDBUG(6, "doDesktopBgTimeoutSet");
   if (params)
      mode.desktop_bg_timeout = atoi((char *)params);
   autosave();
   EDBUG_RETURN(0);
}

int
doMapSlideSpeedSet(void *params)
{
   EDBUG(6, "doMapSlideSpeedSet");
   if (params)
      mode.slidespeedmap = atoi((char *)params);
   autosave();
   EDBUG_RETURN(0);
}

int
doCleanupSlideSpeedSet(void *params)
{
   EDBUG(6, "doCleanupSlideSpeedSet");
   if (params)
      mode.slidespeedcleanup = atoi((char *)params);
   autosave();
   EDBUG_RETURN(0);
}

int
doDragdirSet(void *params)
{
   char                pd;
   Button             *b;
   int                 i;

   EDBUG(6, "doDragdirSet");
   pd = desks.dragdir;
   if (params)
      desks.dragdir = atoi((char *)params);
   else
     {
	desks.dragdir++;
	if (desks.dragdir > 3)
	   desks.dragdir = 0;
     }
   if (pd != desks.dragdir)
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

int
doDragbarOrderSet(void *params)
{
   char                pd;
   Button             *b;

   EDBUG(6, "doDragbarOrderSet");
   pd = desks.dragbar_ordering;
   if (params)
      desks.dragbar_ordering = atoi((char *)params);
   else
     {
	desks.dragbar_ordering++;
	if (desks.dragbar_ordering > 5)
	   desks.dragbar_ordering = 0;
     }
   if (pd != desks.dragbar_ordering)
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

int
doDragbarWidthSet(void *params)
{
   int                 pd;
   Button             *b;

   EDBUG(6, "doDragbarWidthSet");
   pd = desks.dragbar_width;
   if (params)
      desks.dragbar_width = atoi((char *)params);
   if (pd != desks.dragbar_width)
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

int
doDragbarLengthSet(void *params)
{
   int                 pd;
   Button             *b;

   EDBUG(6, "doDragbarLengthSet");
   pd = desks.dragbar_length;
   if (params)
      desks.dragbar_length = atoi((char *)params);
   if (pd != desks.dragbar_length)
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

int
doDeskSlideSet(void *params)
{
   EDBUG(6, "doDeskSlideSet");
   if (params)
      desks.slidein = atoi((char *)params);
   else
     {
	if (desks.slidein)
	   desks.slidein = 0;
	else
	   desks.slidein = 1;
     }
   autosave();
   EDBUG_RETURN(0);
}

int
doDeskSlideSpeedSet(void *params)
{
   EDBUG(6, "doDeskSlideSpeedSet");
   if (params)
      desks.slidespeed = atoi((char *)params);
   autosave();
   EDBUG_RETURN(0);
}

int
doHiQualityBgSet(void *params)
{
   EDBUG(6, "doHiQualityBgSet");
   if (params)
      desks.hiqualitybg = atoi((char *)params);
   else
     {
	if (desks.hiqualitybg)
	   desks.hiqualitybg = 0;
	else
	   desks.hiqualitybg = 1;
     }
   autosave();
   EDBUG_RETURN(0);
}

int
doPlaySoundClass(void *params)
{
   EDBUG(6, "doPlaySoundClass");

   if (!params)
      EDBUG_RETURN(0);

   SoundPlay((char *)params);

   EDBUG_RETURN(0);
}

int
doGotoDesktop(void *params)
{
   int                 d = 0;

   EDBUG(6, "doGotoDesktop");

   if (InZoom())
      EDBUG_RETURN(0);
   if (!params)
      EDBUG_RETURN(0);

   sscanf((char *)params, "%i", &d);
   GotoDesktop(d);
   SoundPlay("SOUND_DESKTOP_SHUT");
   EDBUG_RETURN(0);
}

int
doDeskray(void *params)
{
   EDBUG(6, "doDeskray");
   if (InZoom())
      EDBUG_RETURN(0);
   if ((mode.mode == MODE_MOVE) || (mode.mode == MODE_RESIZE_H)
       || (mode.mode == MODE_RESIZE_V) || (mode.mode == MODE_RESIZE))
      EDBUG_RETURN(0);
   if (params)
     {
	if (!atoi((char *)params))
	  {
	     HideDesktopTabs();
	     mode.deskmode = MODE_NONE;
	  }
	else
	  {
	     mode.deskmode = MODE_DESKRAY;
	     ShowDesktopTabs();
	  }
     }
   else
     {
	if (mode.deskmode == MODE_DESKRAY)
	  {
	     HideDesktopTabs();
	     mode.deskmode = MODE_NONE;
	  }
	else
	  {
	     mode.deskmode = MODE_DESKRAY;
	     ShowDesktopTabs();
	  }
     }
   EDBUG_RETURN(0);
}

int
doAutosaveSet(void *params)
{
   EDBUG(6, "doAutosaveSet");
   if (params)
      mode.autosave = atoi((char *)params);
   else
     {
	if (mode.autosave)
	   mode.autosave = 0;
	else
	   mode.autosave = 1;
     }
   EDBUG_RETURN(0);
}

int
doHideShowButton(void *params)
{
   Button            **lst, *b;
   char                s[1024], *ss;
   int                 num, i;

   /* This is unused - where did this come from? -Mandrake */
   /* static char         lasthide = 0; */

   EDBUG(6, "doHideShowButton");

   if (InZoom())
      EDBUG_RETURN(0);

   if ((mode.mode == MODE_MOVE) || (mode.mode == MODE_RESIZE_H)
       || (mode.mode == MODE_RESIZE_V) || (mode.mode == MODE_RESIZE))
      EDBUG_RETURN(0);

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

int
doIconifyWindow(void *params)
{
   EWin               *ewin;
   Group              *curr_group = NULL;
   char               *windowid = 0;
   char                iconified;
   EWin              **gwins = NULL;
   int                 i, num;

   EDBUG(6, "doIconifyWindow");

   if (params)
     {
	windowid = (char *)params;
	ewin =
	   FindItem("ICON", atoi(windowid), LIST_FINDBY_BOTH,
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

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_ICONIFY, mode.nogroup, &num);
   iconified = ewin->iconified;

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
   Efree(gwins);
   EDBUG_RETURN(0);
}

int
doIconifyWindowNoGroup(void *params)
{
   int                 result;

   mode.nogroup = 1;
   result = doIconifyWindow(params);
   mode.nogroup = 0;
   return result;
}

int
doSlideout(void *params)
{
   Slideout           *s;

   EDBUG(6, "doSlideout");
   if (InZoom())
      EDBUG_RETURN(0);
   if (!params)
      EDBUG_RETURN(0);

   s = FindItem((char *)params, 0, LIST_FINDBY_NAME, LIST_TYPE_SLIDEOUT);
   if (s)
     {
	SoundPlay("SOUND_SLIDEOUT_SHOW");
	ShowSlideout(s, mode.context_win);
	s->ref_count++;
     }
   EDBUG_RETURN(0);
}

int
doScrollWindows(void *params)
{

   int                 x, y, num, i;
   EWin              **lst;

   EDBUG(6, "doScrollWindows");
   if (InZoom())
      EDBUG_RETURN(0);
   if (!params)
      EDBUG_RETURN(0);

   if (mode.slideout)
      HideSlideout(mode.slideout, mode.context_win);

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

int
doShade(void *params)
{
   EWin               *ewin;
   EWin              **gwins = NULL;
   Group              *curr_group = NULL;
   int                 i, num;
   char                shaded;

   EDBUG(6, "doShade");
   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
      ewin =
	 FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
   else
      ewin = GetFocusEwin();

   if (!ewin)
      EDBUG_RETURN(0);

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_SHADE, mode.nogroup, &num);
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
	params = NULL;
	RememberImportantInfoForEwin(gwins[i]);
     }
   Efree(gwins);
   EDBUG_RETURN(0);
}

int
doShadeNoGroup(void *params)
{
   int                 result;

   mode.nogroup = 1;
   result = doShade(params);
   mode.nogroup = 0;
   return result;
}

int
doMaxH(void *params)
{
   EWin               *ewin;

   EDBUG(6, "doMaxH");
   if (InZoom())
      EDBUG_RETURN(0);
   ewin = GetFocusEwin();
   if (ewin)
     {
	if (ewin->shaded)
	   EDBUG_RETURN(0);
	MaxHeight(ewin, (char *)params);
	RememberImportantInfoForEwin(ewin);
     }
   EDBUG_RETURN(0);
}

int
doMaxW(void *params)
{
   EWin               *ewin;

   EDBUG(6, "doMaxW");
   if (InZoom())
      EDBUG_RETURN(0);
   ewin = GetFocusEwin();
   if (ewin)
     {
	if (ewin->shaded)
	   EDBUG_RETURN(0);
	MaxWidth(ewin, (char *)params);
	RememberImportantInfoForEwin(ewin);
     }
   EDBUG_RETURN(0);
}

int
doMax(void *params)
{
   EWin               *ewin;

   EDBUG(6, "doMax");
   if (InZoom())
      EDBUG_RETURN(0);
   ewin = GetFocusEwin();
   if (ewin)
     {
	if (ewin->shaded)
	   EDBUG_RETURN(0);
	MaxSize(ewin, (char *)params);
	RememberImportantInfoForEwin(ewin);
     }
   EDBUG_RETURN(0);
}

int
doSendToNextDesk(void *params)
{
   EWin               *ewin;

   EDBUG(6, "doSendToNextDesk");
   if (InZoom())
      EDBUG_RETURN(0);
   ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);

   MoveEwinToDesktop(ewin, ewin->desktop + 1);
   RaiseEwin(ewin);
   ICCCM_Configure(ewin);
   ewin->sticky = 0;
   params = NULL;
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

int
doSendToPrevDesk(void *params)
{
   EWin               *ewin;

   EDBUG(6, "doSendToPrevDesk");
   if (InZoom())
      EDBUG_RETURN(0);
   ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);

   MoveEwinToDesktop(ewin, ewin->desktop - 1);
   RaiseEwin(ewin);
   ICCCM_Configure(ewin);
   ewin->sticky = 0;
   params = NULL;
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

int
doSnapshot(void *params)
{
   EWin               *ewin;

   EDBUG(6, "doSnapshot");

   ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);

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

int
doScrollContainer(void *params)
{
   EDBUG(6, "doScrollContainer");

   if (!params)
      EDBUG_RETURN(0);
   if (mode.slideout)
      HideSlideout(mode.slideout, mode.context_win);
   EDBUG_RETURN(0);

}

int
doToolTipSet(void *params)
{
   EDBUG(6, "doToolTipSet");

   if (params)
      mode.tooltips = atoi((char *)params);
   else
     {
	mode.tooltips++;
	if (mode.tooltips > 1)
	   mode.tooltips = 0;
     }
   autosave();
   EDBUG_RETURN(0);
}

int
doFocusNext(void *params)
{
   EDBUG(6, "doFocusNext");

   GetNextFocusEwin();
   params = NULL;
   EDBUG_RETURN(0);
}

int
doFocusPrev(void *params)
{
   EDBUG(6, "doFocusPrev");

   GetPrevFocusEwin();
   params = NULL;
   EDBUG_RETURN(0);
}

int
doFocusSet(void *params)
{
   Window              win = 0;
   EWin               *ewin;

   EDBUG(6, "doFocusSet");

   if (!params)
      EDBUG_RETURN(0);
   sscanf((char *)params, "%li", &win);
   ewin = (EWin *) FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
     {
	if (!ewin->sticky)
	  {
	     GotoDesktop(ewin->desktop);
	     SetCurrentArea(ewin->area_x, ewin->area_y);
	  }
	if (ewin->iconified)
	   DeIconifyEwin(ewin);
	if (ewin->shaded)
	   UnShadeEwin(ewin);
	if (mode.raise_on_next_focus || mode.raise_after_next_focus)
	   RaiseEwin(ewin);
	if (mode.warp_on_next_focus)
	   XWarpPointer(disp, None, ewin->win, 0, 0, 0, 0, ewin->w / 2,
			ewin->h / 2);
	FocusToEWin(ewin);
     }
   EDBUG_RETURN(0);
}

int
doBackgroundSet(void *params)
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

int
doAreaSet(void *params)
{
   int                 a, b;

   EDBUG(6, "doAreaSet");
   if (InZoom())
      EDBUG_RETURN(0);
   if (!params)
      EDBUG_RETURN(0);
   sscanf((char *)params, "%i %i", &a, &b);
   SetCurrentArea(a, b);

   EDBUG_RETURN(0);
}

int
doAreaMoveBy(void *params)
{
   int                 a, b;

   EDBUG(6, "doAreaMoveBy");
   if (InZoom())
      EDBUG_RETURN(0);
   if (!params)
      EDBUG_RETURN(0);

   sscanf((char *)params, "%i %i", &a, &b);
   MoveCurrentAreaBy(a, b);

   EDBUG_RETURN(0);
}

int
doToggleFixedPos(void *params)
{
   EWin               *ewin;

   EDBUG(6, "doToggleFixedPos");

   ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);

   if (ewin->fixedpos)
      ewin->fixedpos = 0;
   else
      ewin->fixedpos = 1;

   params = NULL;
   EDBUG_RETURN(0);
}

int
doSetLayer(void *params)
{
   EWin               *ewin;
   int                 l;

   EDBUG(6, "doSetLayer");
   if (InZoom())
      EDBUG_RETURN(0);
   ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);
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

int
doWarpPointer(void *params)
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

int
doMoveWinToArea(void *params)
{
   EWin               *ewin;
   int                 dx, dy;

   EDBUG(6, "doMoveWinToArea");
   if (InZoom())
      EDBUG_RETURN(0);
   ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);

   if (params)
     {
	sscanf((char *)params, "%i %i", &dx, &dy);
	MoveEwinToArea(ewin, dx, dy);
     }
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

int
doMoveWinByArea(void *params)
{
   EWin               *ewin;
   int                 dx, dy;

   EDBUG(6, "doMoveWinByArea");
   if (InZoom())
      EDBUG_RETURN(0);
   ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);

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

int
doSetWinBorder(void *params)
{
   EWin               *ewin;
   EWin              **gwins = NULL;
   int                 i, num;
   char                buf[1024], has_shaded;
   Border             *b;
   char                shadechange = 0;

   EDBUG(6, "doSetWinBorder");

   ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);

   if (!params)
      EDBUG_RETURN(0);

   gwins =
      ListWinGroupMembersForEwin(ewin, ACTION_SET_WINDOW_BORDER, mode.nogroup,
				 &num);

   sscanf((char *)params, "%1000s", buf);
   b = (Border *) FindItem(buf, 0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);
   if (!b)
      EDBUG_RETURN(0);
   has_shaded = 0;
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
   Efree(gwins);
   EDBUG_RETURN(0);
}

int
doSetWinBorderNoGroup(void *params)
{
   int                 result;

   mode.nogroup = 1;
   result = doSetWinBorder(params);
   mode.nogroup = 0;
   return result;
}

int
doLinearAreaSet(void *params)
{
   int                 da;

   EDBUG(6, "doLinearAreaSet");
   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
     {
	sscanf((char *)params, "%i", &da);
	SetCurrentLinearArea(da);
     }
   EDBUG_RETURN(0);
}

int
doLinearAreaMoveBy(void *params)
{
   int                 da;

   EDBUG(6, "doLinearAreaMoveBy");
   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
     {
	sscanf((char *)params, "%i", &da);
	MoveCurrentLinearAreaBy(da);
     }
   EDBUG_RETURN(0);
}

int
doAbout(void *params)
{
   Dialog             *d;
   DItem              *table, *di;

   EDBUG(6, "doAbout");
   if (InZoom())
      EDBUG_RETURN(0);
   if ((d =
	FindItem("ABOUT_ENLIGHTENMENT", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
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

   params = NULL;
   EDBUG_RETURN(0);
}

int
doFX(void *params)
{
   EDBUG(6, "doFX");
   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
      FX_Op((const char *)params, FX_OP_TOGGLE);
   autosave();
   EDBUG_RETURN(0);
}

int
doMoveWinToLinearArea(void *params)
{
   EWin               *ewin;
   int                 da;

   EDBUG(6, "doMoveWinToLinearArea");
   if (InZoom())
      EDBUG_RETURN(0);
   ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);
   if (params)
     {
	sscanf((char *)params, "%i", &da);
	MoveEwinToLinearArea(ewin, da);
     }
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

int
doMoveWinByLinearArea(void *params)
{
   EWin               *ewin;
   int                 da;

   EDBUG(6, "doMoveWinByLinearArea");
   if (InZoom())
      EDBUG_RETURN(0);
   ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);
   if (params)
     {
	sscanf((char *)params, "%i", &da);
	MoveEwinLinearAreaBy(ewin, da);
     }
   RememberImportantInfoForEwin(ewin);
   EDBUG_RETURN(0);
}

int
doSetPagerHiq(void *params)
{
   EDBUG(6, "doSetPagerHiq");
   if (params)
     {
	char                num;

	num = atoi(params);
	PagerSetHiQ(num);
     }
   autosave();
   EDBUG_RETURN(0);
}

int
doSetPagerSnap(void *params)
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

int
doConfigure(void *params)
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

int
doInsertKeys(void *params)
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

int
doCreateIconbox(void *params)
{
   EDBUG(6, "doCreateIconbox");
   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
     {
	Iconbox            *ib;

	SoundPlay("SOUND_NEW_ICONBOX");
	ib = IconboxCreate(params);
	IconboxShow(ib);
     }
   else
     {
	Iconbox            *ib, **ibl;
	int                 num = 0;
	char                s[64];

	ibl = ListAllIconboxes(&num);
	if (ibl)
	   Efree(ibl);
	Esnprintf(s, sizeof(s), "_IB_%i", num);
	SoundPlay("SOUND_NEW_ICONBOX");
	ib = IconboxCreate(s);
	IconboxShow(ib);
     }
   autosave();
   EDBUG_RETURN(0);
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

int
doRaiseLower(void *params)
{
   EWin               *ewin;
   EWin              **gwins;
   int                 i, num, j, raise = 0;

   EDBUG(6, "doRaiseLower");
   if (InZoom())
      EDBUG_RETURN(0);

   if (params)
      ewin =
	 FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
   else
      ewin = GetFocusEwin();
   if (!ewin)
      EDBUG_RETURN(0);

   gwins =
      ListWinGroupMembersForEwin(ewin, ACTION_RAISE_LOWER, mode.nogroup, &num);
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

   Efree(gwins);

   EDBUG_RETURN(0);
}

int
doRaiseLowerNoGroup(void *params)
{
   int                 result;

   mode.nogroup = 1;
   result = doRaiseLower(params);
   mode.nogroup = 0;
   return result;
}

int
doShowHideGroup(void *params)
{
   EWin               *ewin;

   EDBUG(6, "doShowGroup");
   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
      ewin =
	 FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
   else
      ewin = GetFocusEwin();

   if (!ewin)
      EDBUG_RETURN(0);

   ShowHideWinGroups(ewin, NULL, SET_TOGGLE);
   EDBUG_RETURN(0);
}

int
doStartGroup(void *params)
{
   EWin               *ewin;

   EDBUG(6, "doStartGroup");
   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
      ewin =
	 FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
   else
      ewin = GetFocusEwin();

   if (!ewin)
      EDBUG_RETURN(0);

   BuildWindowGroup(&ewin, 1);

   SaveGroups();
   EDBUG_RETURN(0);
}

int
doAddToGroup(void *params)
{
   EWin               *ewin;

   EDBUG(6, "doAddToGroup");
   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
      ewin =
	 FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
   else
      ewin = GetFocusEwin();

   if (!(ewin))
      EDBUG_RETURN(0);
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

int
doRemoveFromGroup(void *params)
{
   EWin               *ewin;

   EDBUG(6, "doRemoveFromGroup");
   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
      ewin =
	 FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
   else
      ewin = GetFocusEwin();

   if (!ewin)
      EDBUG_RETURN(0);

   ChooseGroupDialog(ewin,
		     _("   Select the group to remove the window from.  "),
		     GROUP_SELECT_EWIN_ONLY, ACTION_REMOVE_FROM_GROUP);

   SaveGroups();
   EDBUG_RETURN(0);
}

int
doBreakGroup(void *params)
{
   EWin               *ewin;

   EDBUG(6, "doBreakGroup");
   if (InZoom())
      EDBUG_RETURN(0);
   if (params)
      ewin =
	 FindItem(NULL, atoi((char *)params), LIST_FINDBY_ID, LIST_TYPE_EWIN);
   else
      ewin = GetFocusEwin();

   if (!ewin)
      EDBUG_RETURN(0);

   ChooseGroupDialog(ewin, _("  Select the group to break  "),
		     GROUP_SELECT_EWIN_ONLY, ACTION_BREAK_GROUP);

   SaveGroups();
   EDBUG_RETURN(0);
}

int
doZoom(void *params)
{
   EWin               *ewin;
   char                s[1024];

   EDBUG(6, "doZoom");

   if (!(CanZoom()))
      EDBUG_RETURN(0);

   Esnprintf(s, sizeof(s), "%s/.zoom_warn", UserEDir());
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
   ewin = GetFocusEwin();

   if (!ewin)
      EDBUG_RETURN(0);
   if (InZoom())
      Zoom(NULL);
   else
      Zoom(ewin);
   EDBUG_RETURN(0);
   params = NULL;
}

int
initFunctionArray(void)
{
   EDBUG(5, "initFunctionArray");
   ActionFunctions[ACTION_NONE] = (int (*)(void *))(doNothing);
   ActionFunctions[ACTION_EXIT] = (int (*)(void *))(doExit);
   ActionFunctions[ACTION_EXEC] = (int (*)(void *))(execApplication);
   ActionFunctions[ACTION_ALERT] = (int (*)(void *))(alert);
   ActionFunctions[ACTION_SHOW_MENU] = (int (*)(void *))(spawnMenu);
   ActionFunctions[ACTION_HIDE_MENU] = (int (*)(void *))(hideMenu);
   ActionFunctions[ACTION_MOVE] = (int (*)(void *))(doMove);
   ActionFunctions[ACTION_RESIZE] = (int (*)(void *))(doResize);
   ActionFunctions[ACTION_RAISE] = (int (*)(void *))(doRaise);
   ActionFunctions[ACTION_LOWER] = (int (*)(void *))(doLower);
   ActionFunctions[ACTION_CLEANUP] = (int (*)(void *))(doCleanup);
   ActionFunctions[ACTION_RESIZE_H] = (int (*)(void *))(doResizeH);
   ActionFunctions[ACTION_RESIZE_V] = (int (*)(void *))(doResizeV);
   ActionFunctions[ACTION_KILL] = (int (*)(void *))(doKill);
   ActionFunctions[ACTION_KILL_NASTY] = (int (*)(void *))(doKillNasty);
   ActionFunctions[ACTION_DESKTOP_NEXT] = (int (*)(void *))(doNextDesktop);
   ActionFunctions[ACTION_DESKTOP_PREV] = (int (*)(void *))(doPrevDesktop);
   ActionFunctions[ACTION_DESKTOP_RAISE] = (int (*)(void *))(doRaiseDesktop);
   ActionFunctions[ACTION_DESKTOP_LOWER] = (int (*)(void *))(doLowerDesktop);
   ActionFunctions[ACTION_DESKTOP_DRAG] = (int (*)(void *))(doDragDesktop);
   ActionFunctions[ACTION_STICK] = (int (*)(void *))(doStick);
   ActionFunctions[ACTION_DESKTOP_INPLACE] =
      (int (*)(void *))(doInplaceDesktop);
   ActionFunctions[ACTION_DRAG_BUTTON] = (int (*)(void *))(doDragButtonStart);
   ActionFunctions[ACTION_FOCUSMODE_SET] = (int (*)(void *))(doFocusModeSet);
   ActionFunctions[ACTION_MOVEMODE_SET] = (int (*)(void *))(doMoveModeSet);
   ActionFunctions[ACTION_RESIZEMODE_SET] = (int (*)(void *))(doResizeModeSet);
   ActionFunctions[ACTION_SLIDEMODE_SET] = (int (*)(void *))(doSlideModeSet);
   ActionFunctions[ACTION_CLEANUPSILDE_SET] =
      (int (*)(void *))(doCleanupSlideSet);
   ActionFunctions[ACTION_MAPSLIDE_SET] = (int (*)(void *))(doMapSlideSet);
   ActionFunctions[ACTION_SOUND_SET] = (int (*)(void *))(doSoundSet);
   ActionFunctions[ACTION_BUTTONMOVE_RESIST_SET] =
      (int (*)(void *))(doButtonMoveResistSet);
   ActionFunctions[ACTION_DESKTOPBG_TIMEOUT_SET] =
      (int (*)(void *))(doDesktopBgTimeoutSet);
   ActionFunctions[ACTION_MAPSLIDE_SPEED_SET] =
      (int (*)(void *))(doMapSlideSpeedSet);
   ActionFunctions[ACTION_CLEANUPSLIDE_SPEED_SET] =
      (int (*)(void *))(doCleanupSlideSpeedSet);
   ActionFunctions[ACTION_DRAGDIR_SET] = (int (*)(void *))(doDragdirSet);
   ActionFunctions[ACTION_DRAGBAR_ORDER_SET] =
      (int (*)(void *))(doDragbarOrderSet);
   ActionFunctions[ACTION_DRAGBAR_WIDTH_SET] =
      (int (*)(void *))(doDragbarWidthSet);
   ActionFunctions[ACTION_DRAGBAR_LENGTH_SET] =
      (int (*)(void *))(doDragbarLengthSet);
   ActionFunctions[ACTION_DESKSLIDE_SET] = (int (*)(void *))(doDeskSlideSet);
   ActionFunctions[ACTION_DESKSLIDE_SPEED_SET] =
      (int (*)(void *))(doDeskSlideSpeedSet);
   ActionFunctions[ACTION_HIQUALITYBG_SET] =
      (int (*)(void *))(doHiQualityBgSet);
   ActionFunctions[ACTION_PLAYSOUNDCLASS] = (int (*)(void *))(doPlaySoundClass);
   ActionFunctions[ACTION_GOTO_DESK] = (int (*)(void *))(doGotoDesktop);
   ActionFunctions[ACTION_DESKRAY] = (int (*)(void *))(doDeskray);
   ActionFunctions[ACTION_AUTOSAVE_SET] = (int (*)(void *))(doAutosaveSet);
   ActionFunctions[ACTION_HIDESHOW_BUTTON] =
      (int (*)(void *))(doHideShowButton);
   ActionFunctions[ACTION_ICONIFY] = (int (*)(void *))(doIconifyWindow);
   ActionFunctions[ACTION_SLIDEOUT] = (int (*)(void *))(doSlideout);
   ActionFunctions[ACTION_SCROLL_WINDOWS] = (int (*)(void *))(doScrollWindows);
   ActionFunctions[ACTION_SHADE] = (int (*)(void *))(doShade);
   ActionFunctions[ACTION_MAX_HEIGHT] = (int (*)(void *))(doMaxH);
   ActionFunctions[ACTION_MAX_WIDTH] = (int (*)(void *))(doMaxW);
   ActionFunctions[ACTION_MAX_SIZE] = (int (*)(void *))(doMax);
   ActionFunctions[ACTION_SEND_TO_NEXT_DESK] =
      (int (*)(void *))(doSendToNextDesk);
   ActionFunctions[ACTION_SEND_TO_PREV_DESK] =
      (int (*)(void *))(doSendToPrevDesk);
   ActionFunctions[ACTION_SNAPSHOT] = (int (*)(void *))(doSnapshot);
   ActionFunctions[ACTION_SCROLL_CONTAINER] =
      (int (*)(void *))(doScrollContainer);
   ActionFunctions[ACTION_TOOLTIP_SET] = (int (*)(void *))(doToolTipSet);
   ActionFunctions[ACTION_FOCUS_NEXT] = (int (*)(void *))(doFocusNext);
   ActionFunctions[ACTION_FOCUS_PREV] = (int (*)(void *))(doFocusPrev);
   ActionFunctions[ACTION_FOCUS_SET] = (int (*)(void *))(doFocusSet);
   ActionFunctions[ACTION_BACKGROUND_SET] = (int (*)(void *))(doBackgroundSet);
   ActionFunctions[ACTION_AREA_SET] = (int (*)(void *))(doAreaSet);
   ActionFunctions[ACTION_MOVE_BY] = (int (*)(void *))(doAreaMoveBy);
   ActionFunctions[ACTION_TOGGLE_FIXED] = (int (*)(void *))(doToggleFixedPos);
   ActionFunctions[ACTION_SET_LAYER] = (int (*)(void *))(doSetLayer);
   ActionFunctions[ACTION_WARP_POINTER] = (int (*)(void *))(doWarpPointer);
   ActionFunctions[ACTION_MOVE_WINDOW_TO_AREA] =
      (int (*)(void *))(doMoveWinToArea);
   ActionFunctions[ACTION_MOVE_WINDOW_BY_AREA] =
      (int (*)(void *))(doMoveWinByArea);
   ActionFunctions[ACTION_SET_WINDOW_BORDER] =
      (int (*)(void *))(doSetWinBorder);
   ActionFunctions[ACTION_LINEAR_AREA_SET] = (int (*)(void *))(doLinearAreaSet);
   ActionFunctions[ACTION_LINEAR_MOVE_BY] =
      (int (*)(void *))(doLinearAreaMoveBy);
   ActionFunctions[ACTION_ABOUT] = (int (*)(void *))(doAbout);
   ActionFunctions[ACTION_FX] = (int (*)(void *))(doFX);
   ActionFunctions[ACTION_MOVE_WINDOW_TO_LINEAR_AREA] =
      (int (*)(void *))(doMoveWinToLinearArea);
   ActionFunctions[ACTION_MOVE_WINDOW_BY_LINEAR_AREA] =
      (int (*)(void *))(doMoveWinByArea);
   ActionFunctions[ACTION_SET_PAGER_HIQ] = (int (*)(void *))(doSetPagerHiq);
   ActionFunctions[ACTION_SET_PAGER_SNAP] = (int (*)(void *))(doSetPagerSnap);
   ActionFunctions[ACTION_CONFIG] = (int (*)(void *))(doConfigure);
   ActionFunctions[ACTION_MOVE_CONSTRAINED] =
      (int (*)(void *))(doMoveConstrained);
   ActionFunctions[ACTION_INSERT_KEYS] = (int (*)(void *))(doInsertKeys);
   ActionFunctions[ACTION_START_GROUP] = (int (*)(void *))(doStartGroup);
   ActionFunctions[ACTION_ADD_TO_GROUP] = (int (*)(void *))(doAddToGroup);
   ActionFunctions[ACTION_REMOVE_FROM_GROUP] =
      (int (*)(void *))(doRemoveFromGroup);
   ActionFunctions[ACTION_BREAK_GROUP] = (int (*)(void *))(doBreakGroup);
   ActionFunctions[ACTION_SHOW_HIDE_GROUP] = (int (*)(void *))(doShowHideGroup);
   ActionFunctions[ACTION_CREATE_ICONBOX] = (int (*)(void *))(doCreateIconbox);
   ActionFunctions[ACTION_RAISE_LOWER] = (int (*)(void *))(doRaiseLower);
   ActionFunctions[ACTION_ZOOM] = (int (*)(void *))(doZoom);
   ActionFunctions[ACTION_SET_WINDOW_BORDER_NG] =
      (int (*)(void *))(doSetWinBorderNoGroup);
   ActionFunctions[ACTION_ICONIFY_NG] =
      (int (*)(void *))(doIconifyWindowNoGroup);
   ActionFunctions[ACTION_KILL_NG] = (int (*)(void *))(doKillNoGroup);
   ActionFunctions[ACTION_MOVE_NG] = (int (*)(void *))(doMoveNoGroup);
   ActionFunctions[ACTION_RAISE_NG] = (int (*)(void *))(doRaiseNoGroup);
   ActionFunctions[ACTION_LOWER_NG] = (int (*)(void *))(doLowerNoGroup);
   ActionFunctions[ACTION_STICK_NG] = (int (*)(void *))(doStickNoGroup);
   ActionFunctions[ACTION_SHADE_NG] = (int (*)(void *))(doShadeNoGroup);
   ActionFunctions[ACTION_RAISE_LOWER_NG] =
      (int (*)(void *))(doRaiseLowerNoGroup);
   ActionFunctions[ACTION_SKIPFOCUS] = (int (*)(void *))(doSkipFocus);
   ActionFunctions[ACTION_SKIPTASK] = (int (*)(void *))(doSkipTask);
   ActionFunctions[ACTION_SKIPWINLIST] = (int (*)(void *))(doSkipWinList);
   ActionFunctions[ACTION_NEVERFOCUS] = (int (*)(void *))(doNeverFocus);
   ActionFunctions[ACTION_SKIPLISTS] = (int (*)(void *))(doSkipLists);
   ActionFunctions[ACTION_SWAPMOVE] = (int (*)(void *))(doSwapMove);

   EDBUG_RETURN(0);
}
