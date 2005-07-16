/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2005 Kim Woelders
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
#include "emodule.h"
#include "ewins.h"
#include "ewin-ops.h"
#include "xwin.h"
#include <ctype.h>

#define SS(s) ((s) ? (s) : NoText)
static const char   NoText[] = "-NONE-";

static size_t       bufsiz;
static char        *bufptr;

static void
IpcPrintInit(void)
{
   bufsiz = 0;
   bufptr = NULL;
}

static void
IpcPrintFlush(Client * c)
{
   if (bufptr == NULL)
      return;

   if (c)
      CommsSend(c, bufptr);
   Efree(bufptr);
   bufsiz = 0;
   bufptr = NULL;
}

void
IpcPrintf(const char *fmt, ...)
{
   char                tmp[FILEPATH_LEN_MAX];
   int                 len;
   va_list             args;

   va_start(args, fmt);
   len = Evsnprintf(tmp, sizeof(tmp), fmt, args);
   va_end(args);

   bufptr = Erealloc(bufptr, bufsiz + len + 1);
   strcpy(bufptr + bufsiz, tmp);
   bufsiz += len;
}

static EWin        *
IpcFindEwin(const char *windowid)
{
   unsigned int        win;

   if (!strcmp(windowid, "*") || !strcmp(windowid, "%")
       || !strcmp(windowid, "current"))
      return GetContextEwin();

   if (isdigit(windowid[0]))
     {
	sscanf(windowid, "%x", &win);
	return EwinFindByChildren(win);
     }

   if (windowid[0] == '+')
      return EwinFindByString(windowid + 1, '+');

   if (windowid[0] == '=')
      return EwinFindByString(windowid + 1, '=');

   return EwinFindByString(windowid, '=');
}

static int
SetEwinBoolean(const char *txt, char *item, const char *value, int set)
{
   int                 old, new;

   new = old = *item != 0;	/* Remember old value */

   if (value == NULL || value[0] == '\0')
      new = !old;
   else if (!strcmp(value, "on"))
      new = 1;
   else if (!strcmp(value, "off"))
      new = 0;
   else if (!strcmp(value, "?"))
      IpcPrintf("%s: %s", txt, (old) ? "on" : "off");
   else
      IpcPrintf("Error: %s", value);

   if (new != old)
     {
	if (set)
	   *item = new;
	return 1;
     }

   return 0;
}

/* The IPC functions */

static void
IPC_Screen(const char *params __UNUSED__, Client * c __UNUSED__)
{
#ifdef HAS_XINERAMA
   if (Mode.display.xinerama_active)
     {
	XineramaScreenInfo *screens;
	int                 num, i;

	screens = XineramaQueryScreens(disp, &num);

	IpcPrintf("Xinerama active:\n");
	IpcPrintf("Head  Screen  X-Origin  Y-Origin     Width    Height\n");
	for (i = 0; i < num; i++)
	  {
	     IpcPrintf(" %2d     %2d       %5d     %5d     %5d     %5d\n",
		       i, screens[i].screen_number,
		       screens[i].x_org, screens[i].y_org, screens[i].width,
		       screens[i].height);
	  }
	XFree(screens);
	return;
     }
   else
     {
	IpcPrintf("Xinerama is not active\n");
     }
#endif

   IpcPrintf("Head  Screen  X-Origin  Y-Origin     Width    Height\n");
   IpcPrintf(" %2d     %2d       %5d     %5d     %5d     %5d\n",
	     0, VRoot.scr, 0, 0, VRoot.w, VRoot.h);
}

static void
IPC_Nop(const char *params __UNUSED__, Client * c __UNUSED__)
{
   IpcPrintf("nop");
}

/* Should be elsewhere */
static void
IPC_Border(const char *params, Client * c __UNUSED__)
{
   if (!params)
     {
	IpcPrintf("Please specify...\n");
	return;
     }

   if (!strncmp(params, "list", 2))
     {
	Border            **lst;
	int                 num, i;

	lst = (Border **) ListItemType(&num, LIST_TYPE_BORDER);
	for (i = 0; i < num; i++)
	   IpcPrintf("%s\n", lst[i]->name);
	if (lst)
	   Efree(lst);
	return;
     }
}

static void
IPC_DialogOK(const char *params, Client * c __UNUSED__)
{
   if (params)
      DialogOKstr(_("Message"), params);
   else
      IpcPrintf("Error: No text for dialog specified\n");
}

static int
CfgStrlistIndex(const char **list, const char *str)
{
   int                 i;

   for (i = 0; list[i]; i++)
      if (!strcmp(list[i], str))
	 return i;
   return -1;
}

static const char  *MovResCfgMoveModes[] = {
   "opaque", "lined", "box", "shaded", "semi-solid", "translucent", NULL
};

static const char  *MovResCfgResizeModes[] = {
   "opaque", "lined", "box", "shaded", "semi-solid", NULL
};

static const char  *MovResCfgInfoModes[] = {
   "never", "center", "corner", NULL
};

static void
IPC_MoveResize(const char *params, Client * c __UNUSED__)
{
   char                param1[32];
   char                param2[32];
   int                 i;

   if (!params)
     {
	IpcPrintf("Please specify...\n");
	return;
     }

   param1[0] = param2[0] = '\0';
   sscanf(params, "%31s %31s", param1, param2);

   if (!strncmp(param1, "move", 2))
     {
	if (param2[0] == '\n' || param2[0] == '?')
	  {
	     if (Conf.movres.mode_move < 0 || Conf.movres.mode_move > 5)
		Conf.movres.mode_move = 0;
	     IpcPrintf("Move mode: %s\n",
		       MovResCfgMoveModes[Conf.movres.mode_move]);
	     return;
	  }

	i = CfgStrlistIndex(MovResCfgMoveModes, param2);
	if (i >= 0)
	  {
	     Conf.movres.mode_move = i;
	  }
	else
	  {
	     IpcPrintf("Move mode not found: %s\n", param2);
	  }

     }
   else if (!strncmp(param1, "resize", 2))
     {
	if (param2[0] == '\n' || param2[0] == '?')
	  {
	     if (Conf.movres.mode_resize < 0 || Conf.movres.mode_resize > 4)
		Conf.movres.mode_resize = 0;
	     IpcPrintf("Resize mode: %s\n",
		       MovResCfgResizeModes[Conf.movres.mode_resize]);
	     return;
	  }

	i = CfgStrlistIndex(MovResCfgResizeModes, param2);
	if (i >= 0)
	  {
	     Conf.movres.mode_resize = i;
	  }
	else
	  {
	     IpcPrintf("Resize mode not found: %s\n", param2);
	  }

     }
   else if (!strncmp(param1, "info", 2))
     {
	if (param2[0] == '\n' || param2[0] == '?')
	  {
	     if (Conf.movres.mode_info < 0 || Conf.movres.mode_info > 2)
		Conf.movres.mode_info = 1;
	     IpcPrintf("Info mode: %s\n",
		       MovResCfgInfoModes[Conf.movres.mode_info]);
	     return;
	  }

	i = CfgStrlistIndex(MovResCfgInfoModes, param2);
	if (i >= 0)
	  {
	     Conf.movres.mode_info = i;
	  }
	else
	  {
	     IpcPrintf("Info mode not found: %s\n", param2);
	  }

     }
}

static void
IPC_WinList(const char *params, Client * c __UNUSED__)
{
   char                param1[FILEPATH_LEN_MAX];
   EWin               *const *lst, *e;
   int                 num, i;

   param1[0] = '\0';
   word(params, 1, param1);

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	e = lst[i];
	switch (param1[0])
	  {
	  case '\0':
	     IpcPrintf("%#lx : %s\n", e->client.win, SS(e->icccm.wm_name));
	     break;

	  default:
	     IpcPrintf("%#lx : %s :: %d : %d %d : %d %d %dx%d\n",
		       e->client.win, SS(e->icccm.wm_name),
		       (EoIsSticky(e)) ? -1 : EoGetDesk(e), e->area_x,
		       e->area_y, EoGetX(e), EoGetY(e), EoGetW(e), EoGetH(e));
	     break;

	  case 'a':
	     IpcPrintf("%#10lx : %5d %5d %4dx%4d :: %2d : %d %d : %s\n",
		       e->client.win, EoGetX(e), EoGetY(e), EoGetW(e),
		       EoGetH(e), (EoIsSticky(e)) ? -1 : EoGetDesk(e),
		       e->area_x, e->area_y, SS(e->icccm.wm_name));
	     break;

	  case 'p':
	     IpcPrintf
		("%#10lx : %5d %5d %4dx%4d :: %2d : \"%s\" \"%s\" \"%s\"\n",
		 e->client.win, EoGetX(e), EoGetY(e), EoGetW(e), EoGetH(e),
		 (EoIsSticky(e)) ? -1 : EoGetDesk(e), SS(e->icccm.wm_res_name),
		 SS(e->icccm.wm_res_class), SS(e->icccm.wm_name));
	     break;
	  }
     }
   if (num <= 0)
      IpcPrintf("No windows\n");
}

#if 0				/* TBD */
static int
doMoveConstrained(EWin * ewin, const char *params)
{
   return ActionMoveStart(ewin, params, 1, 0);
}

static int
doMoveNoGroup(EWin * ewin, const char *params)
{
   return ActionMoveStart(ewin, params, 0, 1);
}

static int
doSwapMove(EWin * ewin, const char *params)
{
   Mode.move.swap = 1;
   return ActionMoveStart(ewin, params, 0, 0);
}

static int
doMoveConstrainedNoGroup(EWin * ewin, const char *params)
{
   return ActionMoveStart(ewin, params, 1, 1);
}
#endif

static void
IPC_WinOps(const char *params, Client * c __UNUSED__)
{
   EWin               *ewin;
   char                windowid[FILEPATH_LEN_MAX];
   char                operation[FILEPATH_LEN_MAX];
   char                param1[FILEPATH_LEN_MAX];
   const char         *p;
   const WinOp        *wop;
   unsigned int        val;
   char                on;
   int                 a, b;

   if (params == NULL)
     {
	IpcPrintf("Error: no window specified");
	goto done;
     }

   operation[0] = 0;
   param1[0] = 0;

   windowid[0] = 0;
   word(params, 1, windowid);
   ewin = IpcFindEwin(windowid);
   if (!ewin)
     {
	IpcPrintf("Error: no such window: %s", windowid);
	goto done;
     }

   word(params, 2, operation);
   word(params, 3, param1);

   if (!operation[0])
     {
	IpcPrintf("Error: no operation specified");
	goto done;
     }

   wop = EwinOpFind(operation);
   if (!wop)
     {
	IpcPrintf("Error: unknown operation");
	goto done;
     }

   switch (wop->op)
     {
     default:
	/* We should not get here */
	IpcPrintf("Error: unknown operation");
	return;

     case EWIN_OP_CLOSE:
	EwinOpClose(ewin);
	break;

     case EWIN_OP_KILL:
	EwinOpKill(ewin);
	break;

     case EWIN_OP_ICONIFY:
	if (SetEwinBoolean
	    ("window iconified", &ewin->state.iconified, param1, 0))
	   EwinOpIconify(ewin, !ewin->state.iconified);
	break;

     case EWIN_OP_OPACITY:
	if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("opacity: %u", ewin->ewmh.opacity >> 24);
	     goto done;
	  }
	val = 0xff;
	sscanf(param1, "%i", &val);
	EwinOpSetOpacity(ewin, val);
	break;

#if USE_COMPOSITE
     case EWIN_OP_SHADOW:
	on = EoGetShadow(ewin);
	if (SetEwinBoolean("shadow", &on, param1, 0))
	   EoSetShadow(ewin, !on);
	break;
#endif

     case EWIN_OP_SHADE:
	if (SetEwinBoolean("shaded", &ewin->state.shaded, param1, 0))
	   EwinOpShade(ewin, !ewin->state.shaded);
	break;

     case EWIN_OP_STICK:
	on = EoIsSticky(ewin);
	if (SetEwinBoolean("sticky", &on, param1, 0))
	   EwinOpStick(ewin, !on);
	break;

     case EWIN_OP_FIXED_POS:
	SetEwinBoolean("fixedpos", &ewin->props.fixedpos, param1, 1);
	break;

     case EWIN_OP_NEVER_USE_AREA:
	SetEwinBoolean("never_use_area", &ewin->props.never_use_area, param1,
		       1);
	break;

     case EWIN_OP_FOCUS_CLICK:
	SetEwinBoolean("focusclick", &ewin->props.focusclick, param1, 1);
	break;

     case EWIN_OP_FOCUS_NEVER:
	SetEwinBoolean("neverfocus", &ewin->props.never_focus, param1, 1);
	break;

     case EWIN_OP_NO_BUTTON_GRABS:
	if (SetEwinBoolean
	    ("no_button_grabs", &ewin->props.no_button_grabs, param1, 1))
	  {
	     if (ewin->props.no_button_grabs)
		UnGrabButtonGrabs(ewin);
	     else
		GrabButtonGrabs(ewin);
	  }
	break;

     case EWIN_OP_TITLE:
	p = atword(params, 3);
	if (!p)
	  {
	     IpcPrintf("Error: no title specified");
	     goto done;
	  }
	if (!strcmp(p, "?"))
	  {
	     IpcPrintf("title: %s", ewin->icccm.wm_name);
	     goto done;
	  }
	_EFREE(ewin->icccm.wm_name);
	ewin->icccm.wm_name = Estrdup(p);
	XStoreName(disp, ewin->client.win, ewin->icccm.wm_name);
	EwinBorderUpdateInfo(ewin);
	break;

     case EWIN_OP_MAX_WIDTH:
	MaxWidth(ewin, param1);
	break;

     case EWIN_OP_MAX_HEIGHT:
	MaxHeight(ewin, param1);
	break;

     case EWIN_OP_MAX_SIZE:
	MaxSize(ewin, param1);
	break;

     case EWIN_OP_RAISE:
	EwinOpRaise(ewin);
	break;

     case EWIN_OP_LOWER:
	EwinOpLower(ewin);
	break;

     case EWIN_OP_LAYER:
	if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("window layer: %d", EoGetLayer(ewin));
	     goto done;
	  }
	val = atoi(param1);
	EwinOpSetLayer(ewin, val);
	break;

     case EWIN_OP_BORDER:
	if (!param1[0])
	  {
	     IpcPrintf("Error: no border specified");
	     goto done;
	  }
	if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("window border: %s", BorderGetName(ewin->border));
	     goto done;
	  }
	EwinOpSetBorder(ewin, param1);
	break;

     case EWIN_OP_DESK:
	if (!param1[0])
	  {
	     IpcPrintf("Error: no desktop supplied");
	     goto done;
	  }
	if (!strncmp(param1, "next", 1))
	  {
	     EwinOpMoveToDesk(ewin, EoGetDesk(ewin) + 1);
	  }
	else if (!strncmp(param1, "prev", 1))
	  {
	     EwinOpMoveToDesk(ewin, EoGetDesk(ewin) - 1);
	  }
	else if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("window desk: %d", EoGetDesk(ewin));
	  }
	else
	  {
	     EwinOpMoveToDesk(ewin, atoi(param1));
	  }
	break;

     case EWIN_OP_AREA:
	if (!param1[0])
	  {
	     IpcPrintf("Error: no area supplied");
	     goto done;
	  }
	if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("window area: %d %d", ewin->area_x, ewin->area_y);
	  }
	else
	  {
	     sscanf(params, "%*s %*s %i %i", &a, &b);
	     EwinMoveToArea(ewin, a, b);
	  }
	break;

     case EWIN_OP_MOVE:
	if (!param1[0])
	  {
	     IpcPrintf("Error: no coords supplied");
	     goto done;
	  }
	if (!strcmp(param1, "ptr"))
	  {
	     ActionMoveStart(ewin, 1, 0, 0);
	  }
	else if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("window location: %d %d", EoGetX(ewin), EoGetY(ewin));
	  }
	else if (!strcmp(param1, "??"))
	  {
	     IpcPrintf("client location: %d %d",
		       EoGetX(ewin) + ewin->border->border.left,
		       EoGetY(ewin) + ewin->border->border.top);
	  }
	else
	  {
	     sscanf(params, "%*s %*s %i %i", &a, &b);
	     EwinMove(ewin, a, b);
	  }
	break;

     case EWIN_OP_SIZE:
	if (!param1[0])
	   goto done;

	if (!strcmp(param1, "ptr"))
	  {
	     ActionResizeStart(ewin, 0, MODE_RESIZE);
	  }
	else if (!strcmp(param1, "ptr-h"))
	  {
	     ActionResizeStart(ewin, 0, MODE_RESIZE_H);
	  }
	else if (!strcmp(param1, "ptr-v"))
	  {
	     ActionResizeStart(ewin, 0, MODE_RESIZE_V);
	  }
	else if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("window size: %d %d", ewin->client.w, ewin->client.h);
	  }
	else if (!strcmp(param1, "??"))
	  {
	     IpcPrintf("frame size: %d %d", EoGetW(ewin), EoGetH(ewin));
	  }
	else
	  {
	     sscanf(params, "%*s %*s %i %i", &a, &b);
	     EwinResize(ewin, a, b);
	  }
	break;

     case EWIN_OP_MOVE_REL:
	if (!param1[0])
	   goto done;

	sscanf(params, "%*s %*s %i %i", &a, &b);
	a += EoGetX(ewin);
	b += EoGetY(ewin);
	EwinMove(ewin, a, b);
	break;

     case EWIN_OP_SIZE_REL:
	if (!param1[0])
	   goto done;

	sscanf(params, "%*s %*s %i %i", &a, &b);
	a += ewin->client.w;
	b += ewin->client.h;
	EwinResize(ewin, a, b);
	break;

     case EWIN_OP_FOCUS:
	if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("focused: %s", (ewin == GetFocusEwin())? "yes" : "no");
	     goto done;
	  }
	DeskGotoByEwin(ewin);
	if (ewin->state.iconified)
	   EwinOpIconify(ewin, 0);
	if (ewin->state.shaded)
	   EwinOpShade(ewin, 0);
	EwinOpRaise(ewin);
	FocusToEWin(ewin, FOCUS_SET);
	break;

     case EWIN_OP_FULLSCREEN:
	on = ewin->state.fullscreen;
	if (SetEwinBoolean("fullscreen", &on, param1, 0))
	   EwinSetFullscreen(ewin, !on);
	break;

     case EWIN_OP_SKIP_LISTS:
	if (SetEwinBoolean("skiplists", &ewin->props.skip_ext_task, param1, 1))
	   EwinOpSkipLists(ewin, ewin->props.skip_ext_task);
	break;

     case EWIN_OP_ZOOM:
	if (InZoom())
	   Zoom(NULL);
	else
	   Zoom(ewin);
	break;

     case EWIN_OP_SNAP:
	SnapshotEwinParse(ewin, atword(params, 3));
	break;
     }

 done:
   return;
}

static void
IPC_Remember(const char *params, Client * c __UNUSED__)
{
   int                 window;
   EWin               *ewin;

   if (!params)
     {
	IpcPrintf("Error: no parameters\n");
	goto done;
     }

   window = 0;
   sscanf(params, "%x", &window);
   ewin = EwinFindByClient(window);
   if (!ewin)
     {
	IpcPrintf("Error: Window not found: %#x\n", window);
	goto done;
     }

   SnapshotEwinParse(ewin, atword(params, 2));

 done:
   return;
}

static void
IPC_ForceSave(const char *params __UNUSED__, Client * c __UNUSED__)
{
   autosave();
}

static void
IPC_Restart(const char *params __UNUSED__, Client * c __UNUSED__)
{
   SessionExit(EEXIT_RESTART, NULL);
}

static void
IPC_Exit(const char *params, Client * c __UNUSED__)
{
   char                param1[FILEPATH_LEN_MAX];

   param1[0] = 0;
   word(params, 1, param1);

   if (!param1[0])
      SessionExit(EEXIT_EXIT, NULL);
   else if (!strcmp(param1, "logout"))
      SessionExit(EEXIT_LOGOUT, NULL);
   else if (!strcmp(param1, "restart"))
      SessionExit(EEXIT_RESTART, NULL);
   else if (!strcmp(param1, "theme"))
      SessionExit(EEXIT_THEME, atword(params, 2));
   else if (!strcmp(param1, "exec"))
      SessionExit(EEXIT_EXEC, atword(params, 2));
}

static void
IPC_Copyright(const char *params __UNUSED__, Client * c __UNUSED__)
{
   IpcPrintf("Copyright (C) 2000-2005 Carsten Haitzler and Geoff Harrison,\n"
	     "with various contributors (Isaac Richards, Sung-Hyun Nam, "
	     "Kimball Thurston,\n"
	     "Michael Kellen, Frederic Devernay, Felix Bellaby, "
	     "Michael Jennings,\n"
	     "Christian Kreibich, Peter Kjellerstedt, Troy Pesola, Owen Taylor, "
	     "Stalyn,\n" "Knut Neumann, Nathan Heagy, Simon Forman, "
	     "Brent Nelson,\n"
	     "Martin Tyler, Graham MacDonald, Jessse Michael, "
	     "Paul Duncan, Daniel Erat,\n"
	     "Tom Gilbert, Peter Alm, Ben Frantzdale, "
	     "Hallvar Helleseth, Kameran Kashani,\n"
	     "Carl Strasen, David Mason, Tom Christiansen, and others\n"
	     "-- please see the AUTHORS file for a complete listing)\n\n"
	     "Permission is hereby granted, free of charge, to "
	     "any person obtaining a copy\n"
	     "of this software and associated documentation files "
	     "(the \"Software\"), to\n"
	     "deal in the Software without restriction, including "
	     "without limitation the\n"
	     "rights to use, copy, modify, merge, publish, distribute, "
	     "sub-license, and/or\n"
	     "sell copies of the Software, and to permit persons to "
	     "whom the Software is\n"
	     "furnished to do so, subject to the following conditions:\n\n"
	     "The above copyright notice and this permission notice "
	     "shall be included in\n"
	     "all copies or substantial portions of the Software.\n\n"
	     "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF "
	     "ANY KIND, EXPRESS OR\n"
	     "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF "
	     "MERCHANTABILITY,\n"
	     "FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. "
	     "IN NO EVENT SHALL\n"
	     "THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
	     "LIABILITY, WHETHER\n"
	     "IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, "
	     "OUT OF OR IN\n"
	     "CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS "
	     "IN THE SOFTWARE.\n");
}

static void
IPC_About(const char *params __UNUSED__, Client * c __UNUSED__)
{
   About();
}

static void
IPC_Version(const char *params __UNUSED__, Client * c __UNUSED__)
{
   IpcPrintf(_("Enlightenment Version : %s\n" "code is current to    : %s\n"),
	     e_wm_version, e_wm_date);
}

static void
IPC_Hints(const char *params, Client * c __UNUSED__)
{
   char                param1[FILEPATH_LEN_MAX];
   char                param2[FILEPATH_LEN_MAX];

   param1[0] = 0;
   param2[0] = 0;

   word(params, 1, param1);
   word(params, 2, param2);

   if (!strcmp(param1, "xroot"))
     {
	if (!strncmp(param2, "norm", 4))
	   Conf.hints.set_xroot_info_on_root_window = 0;
	else if (!strncmp(param2, "root", 4))
	   Conf.hints.set_xroot_info_on_root_window = 1;
	autosave();
     }

   IpcPrintf("Set _XROOT* hints: %s\n",
	     (Conf.hints.set_xroot_info_on_root_window) ? "root" : "normal");
}

static void
IPC_Debug(const char *params, Client * c __UNUSED__)
{
   char                param[1024];
   int                 l;
   const char         *p;

   if (!params)
      return;

   p = params;
   l = 0;
   sscanf(p, "%1000s %n", param, &l);
   p += l;

   if (!strncmp(param, "event", 2))
     {
	EventDebugInit(p);
     }
   else if (!strncmp(param, "grab", 2))
     {
	Window              win;

	l = 0;
	sscanf(p, "%1000s %n", param, &l);
	p += l;

	if (!strcmp(param, "?"))
	  {
	     IpcPrintf("Pointer grab on=%d win=%#lx\n",
		       Mode.grabs.pointer_grab_active,
		       Mode.grabs.pointer_grab_window);
	  }
	else if (!strncmp(param, "allow", 2))
	  {
	     l = 0;
	     sscanf(p, "%d", &l);
	     XAllowEvents(disp, l, CurrentTime);
	     IpcPrintf("XAllowEvents\n");
	  }
	else if (!strncmp(param, "unset", 2))
	  {
	     GrabPointerRelease();
	     IpcPrintf("Ungrab\n");
	  }
	else
	  {
	     sscanf(param, "%li", &win);
	     GrabPointerSet(win, ECSR_ACT_RESIZE, 1);
	     IpcPrintf("Grab %#lx\n", win);
	  }
     }
   else if (!strncmp(param, "sync", 2))
     {
	l = 0;
	sscanf(p, "%1000s %n", param, &l);
	p += l;
	if (!strncmp(param, "on", 2))
	  {
	     XSynchronize(disp, True);
	     IpcPrintf("Sync on\n");
	  }
	else if (!strncmp(param, "off", 2))
	  {
	     XSynchronize(disp, False);
	     IpcPrintf("Sync off\n");
	  }
     }
}

static void
IPC_Set(const char *params, Client * c __UNUSED__)
{
   ConfigurationSet(params);
}

static void
IPC_Show(const char *params, Client * c __UNUSED__)
{
   ConfigurationShow(params);
}

static void
EwinShowInfo1(const EWin * ewin)
{
   Border              NoBorder;
   const Border       *border;

   border = ewin->border;
   if (border == NULL)
     {
	border = &NoBorder;
	memset(&NoBorder, 0, sizeof(Border));
     }

   IpcPrintf("***CLIENT***\n"
	     "CLIENT_WIN_ID:          %#10lx\n"
	     "FRAME_WIN_ID:           %#10lx\n"
	     "CONTAINER_WIN_ID:       %#10lx\n"
	     "FRAME_X,Y:              %5i , %5i\n"
	     "FRAME_WIDTH,HEIGHT:     %5i , %5i\n"
	     "BORDER_NAME:            %s\n"
	     "BORDER_BORDER:          %5i , %5i , %5i , %5i\n"
	     "DESKTOP_NUMBER:         %5i\n"
	     "MEMBER_OF_GROUPS:       %5i\n"
	     "DOCKED:                 %5i\n"
	     "STICKY:                 %5i\n"
	     "VISIBLE:                %5i\n"
	     "ICONIFIED:              %5i\n"
	     "SHADED:                 %5i\n"
	     "ACTIVE:                 %5i\n"
	     "LAYER:                  %5i\n"
	     "NEVER_USE_AREA:         %5i\n"
	     "FLOATING:               %5i\n"
	     "CLIENT_WIDTH,HEIGHT:    %5i , %5i\n"
	     "ICON_WIN_ID:            %#10lx\n"
	     "ICON_PIXMAP,MASK_ID:    %#10lx , %#10lx\n"
	     "CLIENT_GROUP_LEADER_ID: %#10lx\n"
	     "CLIENT_NEEDS_INPUT:     %5i\n"
	     "TRANSIENT:              %5i\n"
	     "TITLE:                  %s\n"
	     "CLASS:                  %s\n"
	     "NAME:                   %s\n"
	     "COMMAND:                %s\n"
	     "MACHINE:                %s\n"
	     "ICON_NAME:              %s\n"
	     "IS_GROUP_LEADER:        %5i\n"
	     "NO_RESIZE_HORIZONTAL:   %5i\n"
	     "NO_RESIZE_VERTICAL:     %5i\n"
	     "SHAPED:                 %5i\n"
	     "MIN_WIDTH,HEIGHT:       %5i , %5i\n"
	     "MAX_WIDTH,HEIGHT:       %5i , %5i\n"
	     "BASE_WIDTH,HEIGHT:      %5i , %5i\n"
	     "WIDTH,HEIGHT_INC:       %5i , %5i\n"
	     "ASPECT_MIN,MAX:         %5.5f , %5.5f\n"
	     "MWM_BORDER:             %5i\n"
	     "MWM_RESIZEH:            %5i\n"
	     "MWM_TITLE:              %5i\n"
	     "MWM_MENU:               %5i\n"
	     "MWM_MINIMIZE:           %5i\n"
	     "MWM_MAXIMIZE:           %5i\n",
	     ewin->client.win, EoGetWin(ewin), ewin->win_container,
	     EoGetX(ewin), EoGetY(ewin), EoGetW(ewin), EoGetH(ewin),
	     border->name,
	     border->border.left, border->border.right,
	     border->border.top, border->border.bottom,
	     EoGetDesk(ewin),
	     ewin->num_groups, ewin->state.docked, EoIsSticky(ewin),
	     EoIsShown(ewin), ewin->state.iconified, ewin->state.shaded,
	     ewin->state.active, EoGetLayer(ewin), ewin->props.never_use_area,
	     EoIsFloating(ewin), ewin->client.w, ewin->client.h,
	     ewin->client.icon_win,
	     ewin->client.icon_pmap, ewin->client.icon_mask,
	     ewin->client.group,
	     ewin->client.need_input, ewin->client.transient,
	     SS(ewin->icccm.wm_name), SS(ewin->icccm.wm_res_class),
	     SS(ewin->icccm.wm_res_name), SS(ewin->icccm.wm_command),
	     SS(ewin->icccm.wm_machine), SS(ewin->icccm.wm_icon_name),
	     ewin->client.is_group_leader,
	     ewin->client.no_resize_h, ewin->client.no_resize_v,
	     ewin->state.shaped,
	     ewin->client.width.min, ewin->client.height.min,
	     ewin->client.width.max, ewin->client.height.max,
	     ewin->client.base_w, ewin->client.base_h,
	     ewin->client.w_inc, ewin->client.h_inc,
	     ewin->client.aspect_min, ewin->client.aspect_max,
	     ewin->mwm.decor_border, ewin->mwm.decor_resizeh,
	     ewin->mwm.decor_title, ewin->mwm.decor_menu,
	     ewin->mwm.decor_minimize, ewin->mwm.decor_maximize);
}

static void
EwinShowInfo2(const EWin * ewin)
{
   Border              NoBorder;
   const Border       *border;

   border = ewin->border;
   if (border == NULL)
     {
	border = &NoBorder;
	memset(&NoBorder, 0, sizeof(Border));
     }

   IpcPrintf("WM_NAME                 %s\n"
	     "WM_ICON_NAME            %s\n"
	     "WM_CLASS name.class     %s.%s\n"
	     "WM_WINDOW_ROLE          %s\n"
	     "WM_COMMAND              %s\n"
	     "WM_CLIENT_MACHINE       %s\n"
	     "Client window           %#10lx   x,y %4i,%4i   wxh %4ix%4i\n"
	     "Container window        %#10lx\n"
	     "Frame window            %#10lx   x,y %4i,%4i   wxh %4ix%4i\n"
#if USE_COMPOSITE
	     "Named pixmap            %#10lx\n"
#endif
	     "Border                  %s   lrtb %i,%i,%i,%i\n"
	     "Icon window, pixmap, mask %#10lx, %#10lx, %#10lx\n"
	     "Is group leader  %i  Window group leader %#lx   Client leader %#10lx\n"
	     "Has transients   %i  Transient type  %i  Transient for %#10lx\n"
	     "No resize H/V    %i/%i       Shaped      %i\n"
	     "Base, min, max, inc w/h %ix%i, %ix%i, %ix%i %ix%i\n"
	     "Aspect min, max         %5.5f, %5.5f\n"
	     "Struts                  lrtb %i,%i,%i,%i\n"
	     "MWM border %i resizeh %i title %i menu %i minimize %i maximize %i\n"
	     "NeedsInput   %i   TakeFocus    %i   FocusNever   %i   FocusClick   %i\n"
	     "NeverUseArea %i   FixedPos     %i\n"
	     "Desktop      %i   Layer        %i(%i)\n"
	     "Iconified    %i   Sticky       %i   Shaded       %i   Docked       %i\n"
	     "State        %i   Shown        %i   Visibility   %i   Active       %i\n"
	     "Member of groups        %i\n"
#if USE_COMPOSITE
	     "Opacity    %3i(%x)  Shadow       %i\n"
#else
	     "Opacity    %3i\n"
#endif
	     ,
	     SS(ewin->icccm.wm_name),
	     SS(ewin->icccm.wm_icon_name),
	     SS(ewin->icccm.wm_res_name), SS(ewin->icccm.wm_res_class),
	     SS(ewin->icccm.wm_role),
	     SS(ewin->icccm.wm_command),
	     SS(ewin->icccm.wm_machine),
	     ewin->client.win,
	     ewin->client.x, ewin->client.y, ewin->client.w, ewin->client.h,
	     ewin->win_container,
	     EoGetWin(ewin),
	     EoGetX(ewin), EoGetY(ewin), EoGetW(ewin), EoGetH(ewin),
#if USE_COMPOSITE
	     EoGetPixmap(ewin),
#endif
	     border->name, border->border.left, border->border.right,
	     border->border.top, border->border.bottom,
	     ewin->client.icon_win,
	     ewin->client.icon_pmap, ewin->client.icon_mask,
	     ewin->client.is_group_leader, ewin->client.group,
	     ewin->client.client_leader, ewin->has_transients,
	     ewin->client.transient, ewin->client.transient_for,
	     ewin->client.no_resize_h, ewin->client.no_resize_v,
	     ewin->state.shaped, ewin->client.base_w, ewin->client.base_h,
	     ewin->client.width.min, ewin->client.height.min,
	     ewin->client.width.max, ewin->client.height.max,
	     ewin->client.w_inc, ewin->client.h_inc,
	     ewin->client.aspect_min, ewin->client.aspect_max,
	     ewin->strut.left, ewin->strut.right,
	     ewin->strut.top, ewin->strut.bottom,
	     ewin->mwm.decor_border, ewin->mwm.decor_resizeh,
	     ewin->mwm.decor_title, ewin->mwm.decor_menu,
	     ewin->mwm.decor_minimize, ewin->mwm.decor_maximize,
	     ewin->client.need_input, ewin->client.take_focus,
	     ewin->props.never_focus, ewin->props.focusclick,
	     ewin->props.never_use_area, ewin->props.fixedpos, EoGetDesk(ewin),
	     EoGetLayer(ewin), ewin->o.ilayer,
	     ewin->state.iconified, EoIsSticky(ewin), ewin->state.shaded,
	     ewin->state.docked, ewin->state.state, EoIsShown(ewin),
	     ewin->state.visibility, ewin->state.active, ewin->num_groups,
	     ewin->ewmh.opacity
#if USE_COMPOSITE
	     , EoGetOpacity(ewin), EoGetShadow(ewin)
#endif
      );
}

static void
IPC_EwinInfo(const char *params, Client * c __UNUSED__)
{
   char                param1[FILEPATH_LEN_MAX];
   EWin               *ewin;

   if (params == NULL)
      return;

   sscanf(params, "%1000s", param1);

   if (!strcmp(param1, "all"))
     {
	EWin               *const *lst;
	int                 i, num;

	lst = EwinListGetAll(&num);
	for (i = 0; i < num; i++)
	   EwinShowInfo1(lst[i]);
     }
   else
     {
	ewin = IpcFindEwin(param1);
	if (ewin)
	   EwinShowInfo1(ewin);
	else
	   IpcPrintf("No matching EWin found\n");
     }
}

static void
IPC_EwinInfo2(const char *params, Client * c __UNUSED__)
{
   char                param1[FILEPATH_LEN_MAX];
   EWin               *ewin;

   if (params == NULL)
      return;

   sscanf(params, "%1000s", param1);

   if (!strcmp(param1, "all"))
     {
	EWin               *const *lst;
	int                 i, num;

	lst = EwinListGetAll(&num);
	for (i = 0; i < num; i++)
	   EwinShowInfo2(lst[i]);
     }
   else
     {
	ewin = IpcFindEwin(param1);
	if (ewin)
	   EwinShowInfo2(ewin);
	else
	   IpcPrintf("No matching EWin found\n");
     }
}

static void
IPC_ObjInfo(const char *params __UNUSED__, Client * c __UNUSED__)
{
   int                 i, num;
   EObj               *const *lst, *eo;

   lst = EobjListStackGet(&num);

   IpcPrintf("Num   window T V  D   L     pos       size    S F C Name\n");
   for (i = 0; i < num; i++)
     {
	eo = lst[i];
	IpcPrintf(" %2d %#lx %d %d %2d %3d %5d,%5d %4dx%4d %d %d %d %s\n", i,
		  eo->win, eo->type, eo->shown, eo->desk, eo->ilayer,
		  eo->x, eo->y, eo->w, eo->h, eo->sticky, eo->floating,
#if USE_COMPOSITE
		  (eo->cmhook) ? 1 : 0,
#else
		  0,
#endif
		  eo->name);
     }
}

static void
IPC_Reparent(const char *params, Client * c __UNUSED__)
{
   char                param1[FILEPATH_LEN_MAX];
   char                param2[FILEPATH_LEN_MAX];
   EWin               *ewin, *enew;

   if (params == NULL)
      return;

   sscanf(params, "%100s %100s", param1, param2);

   ewin = IpcFindEwin(param1);
   enew = IpcFindEwin(param2);
   if (!ewin || !enew)
      IpcPrintf("No matching client or target EWin found\n");
   else
      EwinReparent(ewin, enew->client.win);
}

static void
IPC_Warp(const char *params, Client * c __UNUSED__)
{
   int                 x, y;

   if (!params)
      return;

   x = y = 0;
   if (!strcmp(params, "?"))
     {
	PointerAt(&x, &y);
	IpcPrintf("Pointer location: %d %d\n", x, y);
     }
   else if (!strncmp(params, "abs", 3))
     {
	sscanf(params, "%*s %i %i", &x, &y);
	XWarpPointer(disp, None, VRoot.win, 0, 0, 0, 0, x, y);
     }
   else if (!strncmp(params, "rel", 3))
     {
	sscanf(params, "%*s %i %i", &x, &y);
	XWarpPointer(disp, None, None, 0, 0, 0, 0, x, y);
     }
   else if (!strncmp(params, "scr", 3))
     {
	x = (VRoot.scr + 1) % ScreenCount(disp);
	sscanf(params, "%*s %i", &x);
	if (x >= 0 && x < ScreenCount(disp))
	   XWarpPointer(disp, None, RootWindow(disp, x), 0, 0, 0, 0,
			DisplayWidth(disp, x) / 2, DisplayHeight(disp, x) / 2);
     }
   else
     {
	sscanf(params, "%i %i", &x, &y);
	XWarpPointer(disp, None, None, 0, 0, 0, 0, x, y);
     }
}

/*
 * Compatibility stuff - DO NOT USE
 */
static int
IPC_Compat(const char *params)
{
   int                 ok = 0;
   char                param1[128];
   const char         *p;
   int                 len;

   if (!params)
      goto done;

   len = 0;
   param1[0] = '\0';
   sscanf(params, "%127s %n", param1, &len);
   p = params + len;

   ok = 1;
   if (!strcmp(param1, "goto_desktop"))
     {
	if (*p == '?')
	   IpcPrintf("Current Desktop: %d\n", DesksGetCurrent());
     }
   else if (!strcmp(param1, "num_desks"))
     {
	if (*p == '?')
	   IpcPrintf("Number of Desks: %d\n", DesksGetNumber());
     }
   else
     {
	ok = 0;
     }

 done:
   return ok;
}

/* the IPC Array */

/* the format of an IPC member of the IPC array is as follows:
 * {
 *    NameOfMyFunction,
 *    "command_name",
 *    "quick-help explanation",
 *    "extended help data"
 *    "may go on for several lines, be sure\n"
 *    "to add line feeds when you need them and to \"quote\"\n"
 *    "properly"
 * }
 *
 * when you add a function into this array, make sure you also add it into
 * the declarations above and also put the function in this file.  PLEASE
 * if you add a new function in, add help to it also.  since my end goal
 * is going to be to have this whole IPC usable by an end-user or to your
 * scripter, it should be easy to learn to use without having to crack
 * open the source code.
 * --Mandrake
 */
static void         IPC_Help(const char *params, Client * c);

IpcItem             IPCArray[] = {
   {
    IPC_Help,
    "help", "?",
    "Gives you this help screen",
    "Additional parameters will retrieve help on many topics - "
    "\"help <command>\"." "\n" "use \"help all\" for a list of commands.\n"},
   {
    IPC_Version,
    "version", "ver",
    "Displays the current version of Enlightenment running",
    NULL},
   {
    IPC_Nop,
    "nop", NULL,
    "IPC No-operation - returns nop",
    NULL},
   {
    IPC_Copyright,
    "copyright", NULL,
    "Displays copyright information for Enlightenment",
    NULL},
   {
    IPC_About, "about", NULL, "Show E info", NULL},
   {
    IPC_Restart,
    "restart", NULL,
    "Restart Enlightenment",
    NULL},
   {
    IPC_Exit,
    "exit", "q",
    "Exit Enlightenment",
    NULL},
   {
    IPC_ForceSave,
    "save_config", "s",
    "Force Enlightenment to save settings now",
    NULL},
   {
    IPC_WinOps,
    "win_op", "wop",
    "Change a property of a specific window",
    "Use \"win_op <windowid> <property> <value>\" to change the "
    "property of a window\nYou can use the \"window_list\" "
    "command to retrieve a list of available windows\n"
    "You can use ? after most of these commands to receive the current\n"
    "status of that flag\n"
    "available win_op commands are:\n"
    "  win_op <windowid> <close/kill>\n"
    "  win_op <windowid> <fixedpos/never_use_area>\n"
    "  win_op <windowid> <focus/focusclick/neverfocus>\n"
    "  win_op <windowid> <fullscreen/iconify/shade/stick>\n"
    "  win_op <windowid> no_button_grabs\n"
    "  win_op <windowid> <raise/lower>\n"
    "  win_op <windowid> skiplists\n"
    "  win_op <windowid> snap <what>\n"
    "         <what>: all, none, border, command, desktop, dialog, group, icon,\n"
    "                 layer, location, opacity, shade, shadow, size, sticky\n"
    "  win_op <windowid> noshadow\n"
    "  win_op <windowid> toggle_<width/height/size> <conservative/available/xinerama>\n"
    "          (or none for absolute)\n"
    "  win_op <windowid> border <BORDERNAME>\n"
    "  win_op <windowid> desk <desktochangeto/next/prev>\n"
    "  win_op <windowid> area <x> <y>\n"
    "  win_op <windowid> <move/resize> <x> <y>\n"
    "          (you can use ? and ?? to retreive client and frame locations)\n"
    "  win_op <windowid> title <title>\n"
    "  win_op <windowid> layer <0-100,4=normal>\n"
    "<windowid> may be substituted with \"current\" to use the current window\n"},
   {
    IPC_WinList,
    "window_list", "wl",
    "Get a list of currently open windows",
    "the list will be returned in the following "
    "format - \"window_id : title\"\n"
    "you can get an extended list using \"window_list extended\"\n"
    "returns the following format:\n\"window_id : title :: "
    "desktop : area_x area_y : x_coordinate y_coordinate\"\n"},
#if 0
   {
    IPC_FX,
    "fx", NULL,
    "Toggle various effects on/off",
    "Use \"fx <effect> <mode>\" to set the mode of a particular effect\n"
    "Use \"fx <effect> ?\" to get the current mode\n"
    "the following effects are available\n"
    "ripples <on/off> (ripples that act as a water effect on the screen)\n"
    "deskslide <on/off> (slide in desktops on desktop change)\n"
    "mapslide <on/off> (slide in new windows)\n"
    "raindrops <on/off> (raindrops will appear across your desktop)\n"
    "menu_animate <on/off> (toggles the animation of menus "
    "as they appear)\n"
    "animate_win_shading <on/off> (toggles the animation of "
    "window shading)\n"
    "window_shade_speed <#> (number of pixels/sec to shade a window)\n"
    "dragbar <on/off/left/right/top/bottom> (changes " "location of dragbar)\n"
    "tooltips <on/off/#> (changes state of tooltips and "
    "seconds till popup)\n"
    "autoraise <on/off/#> (changes state of autoraise and "
    "seconds till raise)\n"
    "edge_resistance <#/?/off> (changes the amount (in 1/100 seconds)\n"
    "   of time to push for resistance to give)\n"
    "edge_snap_resistance <#/?> (changes the number of pixels that "
    "a window will\n   resist moving against another window\n"
    "audio <on/off> (changes state of audio)\n"
    "-  seconds for tooltips and autoraise can have less than one second\n"
    "   (i.e. 0.5) or greater (1.3, 3.5, etc)\n"},
#endif
   {
    IPC_MoveResize,
    "movres", "mr",
    "Show/set Window move/resize/geometry info modes",
    "  movres move   <?/opaque/lined/box/shaded/semi-solid/translucent>\n"
    "  movres resize <?/opaque/lined/box/shaded/semi-solid>\n"
    "  movres info   <?/never/center/corner>\n"},
   {
    IPC_DialogOK,
    "dialog_ok", "dok",
    "Pop up a dialog box with an OK button",
    "use \"dialog_ok <message>\" to pop up a dialog box\n"},
   {
    IPC_Border, "border", NULL, "List available borders\n", NULL},
   {
    IPC_Screen, "screen", NULL, "Return screen information\n", NULL},
   {
    SnapIpcFunc,
    "list_remember", "rl",
    "Retrieve a list of remembered windows and their attributes",
    SnapIpcText},
   {
    IPC_Hints,
    "hints", NULL,
    "Set hint options",
    "  hints xroot <normal/root>\n"},
   {
    IPC_Debug,
    "debug", NULL,
    "Set debug options",
    "  debug events <EvNo>:<EvNo>...\n"},
   {
    IPC_Set, "set", NULL, "Set configuration parameter", NULL},
   {
    IPC_Show, "show", "sh", "Show configuration parameter(s)", NULL},
   {
    IPC_EwinInfo, "get_client_info", NULL, "Show client window info", NULL},
   {
    IPC_EwinInfo2, "win_info", "wi", "Show client window info", NULL},
   {
    IPC_ObjInfo, "obj_info", "oi", "Show window object info", NULL},
   {
    IPC_Reparent,
    "reparent", "rep",
    "Reparent window",
    "  reparent <windowid> <new parent>\n"},
   {
    IPC_Remember,
    "remember", NULL,
    "Remembers parameters for client windows (obsolete)",
    "  remember <windowid> <parameter>...\n"
    "For compatibility with epplets only. In stead use\n"
    "  wop <windowid> snap <parameter>...\n"},
   {
    IPC_Warp,
    "warp", NULL,
    "Warp/query pointer",
    "  warp ?               Get pointer position\n"
    "  warp abs <x> <y>     Set pointer position\n"
    "  warp rel <x> <y>     Move pointer relative to current position\n"
    "  warp scr [<i>]       Move pointer to other screen (default next)\n"
    "  warp <x> <y>         Same as \"warp rel\"\n"},
};

static int          ipc_item_count = 0;
static const IpcItem **ipc_item_list = NULL;

static const IpcItem **
IPC_GetList(int *pnum)
{
   int                 i, num;
   const IpcItem     **lst;

   if (ipc_item_list)
     {
	/* Must be re-generated if modules are ever added/removed */
	*pnum = ipc_item_count;
	return ipc_item_list;
     }

   num = sizeof(IPCArray) / sizeof(IpcItem);
   lst = (const IpcItem **)Emalloc(num * sizeof(IpcItem *));
   for (i = 0; i < num; i++)
      lst[i] = &IPCArray[i];

   ModulesGetIpcItems(&lst, &num);

   ipc_item_count = num;
   ipc_item_list = lst;
   *pnum = num;
   return lst;
}

/* The IPC Handler */
/* this is the function that actually loops through the IPC array
 * and finds the command that you were trying to run, and then executes it.
 * you shouldn't have to touch this function
 * - Mandrake
 */
int
HandleIPC(const char *params, Client * c)
{
   int                 i, num, ok;
   char                w[FILEPATH_LEN_MAX];
   const IpcItem     **lst, *ipc;

   if (EventDebug(EDBUG_TYPE_IPC))
      Eprintf("HandleIPC: %s\n", params);

   IpcPrintInit();

   lst = IPC_GetList(&num);

   word(params, 1, w);

   ok = 0;
   for (i = 0; i < num; i++)
     {
	ipc = lst[i];
	if (!(ipc->nick && !strcmp(w, ipc->nick)) && strcmp(w, ipc->name))
	   continue;

	word(params, 2, w);
	if (w)
	   ipc->func(atword(params, 2), c);
	else
	   ipc->func(NULL, c);

	ok = 1;
	break;
     }

   if (!ok)
      ok = IPC_Compat(params);

   IpcPrintFlush(c);
   CommsFlush(c);
   return ok;
}

int
EFunc(EWin * ewin, const char *params)
{
   int                 err;

   SetContextEwin(ewin);
   err = HandleIPC(params, NULL);
   SetContextEwin(NULL);

   return err;
}

static int
ipccmp(void *p1, void *p2)
{
   return strcmp(((IpcItem *) p1)->name, ((IpcItem *) p2)->name);
}

static void
IPC_Help(const char *params, Client * c __UNUSED__)
{
   int                 i, num;
   const IpcItem     **lst, *ipc;
   const char         *nick;

   lst = IPC_GetList(&num);

   IpcPrintf(_("Enlightenment IPC Commands Help\n"));

   if (!params)
     {
	IpcPrintf(_("Use \"help all\" for descriptions of each command\n"
		    "Use \"help <command>\" for an individual description\n\n"));
	IpcPrintf(_("Commands currently available:\n"));

	Quicksort((void **)lst, 0, num - 1, ipccmp);

	for (i = 0; i < num; i++)
	  {
	     ipc = lst[i];
	     nick = (ipc->nick) ? ipc->nick : "";
	     IpcPrintf("  %-16s %-4s ", ipc->name, nick);
	     if ((i % 3) == 2)
		IpcPrintf("\n");
	  }
	if (i % 3)
	   IpcPrintf("\n");
     }
   else if (!strcmp(params, "all"))
     {
	IpcPrintf(_
		  ("Use \"help full\" for full descriptions of each command\n"));
	IpcPrintf(_("Use \"help <command>\" for an individual description\n"));
	IpcPrintf(_("Commands currently available:\n"));
	IpcPrintf(_("         <command>     : <description>\n"));

	for (i = 0; i < num; i++)
	  {
	     ipc = lst[i];
	     nick = (ipc->nick) ? ipc->nick : "";
	     IpcPrintf("%18s %4s: %s\n", ipc->name, nick, ipc->help_text);
	  }
     }
   else if (!strcmp(params, "full"))
     {
	IpcPrintf(_("Commands currently available:\n"));
	IpcPrintf(_("         <command>     : <description>\n"));

	for (i = 0; i < num; i++)
	  {
	     IpcPrintf("----------------------------------------\n");
	     ipc = lst[i];
	     nick = (ipc->nick) ? ipc->nick : "";
	     IpcPrintf("%18s %4s: %s\n", ipc->name, nick, ipc->help_text);
	     if (ipc->extended_help_text)
		IpcPrintf(ipc->extended_help_text);
	  }
     }
   else
     {
	for (i = 0; i < num; i++)
	  {
	     ipc = lst[i];
	     if (strcmp(params, ipc->name) &&
		 (ipc->nick == NULL || strcmp(params, ipc->nick)))
		continue;

	     nick = (ipc->nick) ? ipc->nick : "";
	     IpcPrintf("----------------------------------------\n");
	     IpcPrintf("%18s %4s: %s\n", ipc->name, nick, ipc->help_text);
	     IpcPrintf("----------------------------------------\n");
	     if (ipc->extended_help_text)
		IpcPrintf(ipc->extended_help_text);
	  }
     }
}
