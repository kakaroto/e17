/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2006 Kim Woelders
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
#include "aclass.h"
#include "borders.h"		/* FIXME - Should not be here */
#include "desktops.h"
#include "emodule.h"
#include "eobj.h"
#include "ewins.h"
#include "ewin-ops.h"
#include "hints.h"		/* FIXME - Should not be here */
#include "snaps.h"
#include "timers.h"
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
   ScreenShowInfo();
}

static void
IPC_Nop(const char *params __UNUSED__, Client * c __UNUSED__)
{
   IpcPrintf("nop");
}

/* Should be elsewhere */
static void
IPC_Border_CB_List(Border * b, void *data __UNUSED__)
{
   IpcPrintf("%s\n", BorderGetName(b));
}

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
	BordersForeach(IPC_Border_CB_List, NULL);
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
   static const char  *const TxtPG[] = { "NW", "NE", "SW", "SE" };
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
	     IpcPrintf("%#lx : %s\n", _EwinGetClientXwin(e),
		       SS(e->icccm.wm_name));
	     break;

	  default:
	     IpcPrintf("%#lx : %s :: %d : %d %d : %d %d %dx%d\n",
		       _EwinGetClientXwin(e), SS(e->icccm.wm_name),
		       (EoIsSticky(e)) ? -1 : (int)EoGetDeskNum(e), e->area_x,
		       e->area_y, EoGetX(e), EoGetY(e), EoGetW(e), EoGetH(e));
	     break;

	  case 'a':
	     IpcPrintf("%#10lx : %5d %5d %4dx%4d :: %2d : %d %d : %s\n",
		       _EwinGetClientXwin(e), EoGetX(e), EoGetY(e), EoGetW(e),
		       EoGetH(e), (EoIsSticky(e)) ? -1 : (int)EoGetDeskNum(e),
		       e->area_x, e->area_y, SS(e->icccm.wm_name));
	     break;

	  case 'g':
	     IpcPrintf
		("%#10lx : %5d %5d %4dx%4d :: %2d : %s %4d,%4d %2d,%2d : %s\n",
		 _EwinGetClientXwin(e), EoGetX(e), EoGetY(e), EoGetW(e),
		 EoGetH(e), (EoIsSticky(e)) ? -1 : (int)EoGetDeskNum(e),
		 TxtPG[e->place.gravity & 3], e->place.gx, e->place.gy,
		 e->place.ax, e->place.ay, SS(e->icccm.wm_name));
	     break;

	  case 'p':
	     IpcPrintf
		("%#10lx : %5d %5d %4dx%4d :: %2d : \"%s\" \"%s\" \"%s\"\n",
		 _EwinGetClientXwin(e), EoGetX(e), EoGetY(e), EoGetW(e),
		 EoGetH(e), (EoIsSticky(e)) ? -1 : (int)EoGetDeskNum(e),
		 SS(e->icccm.wm_res_name), SS(e->icccm.wm_res_class),
		 SS(e->icccm.wm_name));
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
	EwinOpSetBorder(ewin, OPSRC_USER, param1);
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
	XStoreName(disp, _EwinGetClientXwin(ewin), ewin->icccm.wm_name);
	EwinBorderUpdateInfo(ewin);
	break;

     case EWIN_OP_CLOSE:
	EwinOpClose(ewin, OPSRC_USER);
	break;

     case EWIN_OP_KILL:
	EwinOpKill(ewin, OPSRC_USER);
	break;

     case EWIN_OP_ICONIFY:
	on = ewin->state.iconified;
	if (SetEwinBoolean("window iconified", &on, param1, 1))
	   EwinOpIconify(ewin, OPSRC_USER, on);
	break;

     case EWIN_OP_SHADE:
	on = ewin->state.shaded;
	if (SetEwinBoolean(wop->name, &on, param1, 1))
	   EwinOpShade(ewin, OPSRC_USER, on);
	break;

     case EWIN_OP_STICK:
	on = EoIsSticky(ewin);
	if (SetEwinBoolean(wop->name, &on, param1, 1))
	   EwinOpStick(ewin, OPSRC_USER, on);
	break;

     case EWIN_OP_FOCUS:
	if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("focused: %s", (ewin == GetFocusEwin())? "yes" : "no");
	     goto done;
	  }
	EwinOpActivate(ewin, OPSRC_USER);
	break;

     case EWIN_OP_DESK:
	if (!param1[0])
	  {
	     IpcPrintf("Error: no desktop supplied");
	     goto done;
	  }
	if (!strncmp(param1, "next", 1))
	  {
	     EwinOpMoveToDesk(ewin, OPSRC_USER, EoGetDesk(ewin), 1);
	  }
	else if (!strncmp(param1, "prev", 1))
	  {
	     EwinOpMoveToDesk(ewin, OPSRC_USER, EoGetDesk(ewin), -1);
	  }
	else if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("window desk: %d", EoGetDeskNum(ewin));
	  }
	else
	  {
	     EwinOpMoveToDesk(ewin, OPSRC_USER, NULL, atoi(param1));
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
	else if (!strcmp(param1, "move"))
	  {
	     a = b = 0;
	     sscanf(params, "%*s %*s %*s %i %i", &a, &b);
	     EwinMoveToArea(ewin, ewin->area_x + a, ewin->area_y + b);
	  }
	else
	  {
	     a = ewin->area_x;
	     b = ewin->area_y;
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
	     EwinOpMove(ewin, OPSRC_USER, a, b);
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
	     EwinOpResize(ewin, OPSRC_USER, a, b);
	  }
	break;

     case EWIN_OP_MOVE_REL:
	if (!param1[0])
	   goto done;

	sscanf(params, "%*s %*s %i %i", &a, &b);
	a += EoGetX(ewin);
	b += EoGetY(ewin);
	EwinOpMove(ewin, OPSRC_USER, a, b);
	break;

     case EWIN_OP_SIZE_REL:
	if (!param1[0])
	   goto done;

	sscanf(params, "%*s %*s %i %i", &a, &b);
	a += ewin->client.w;
	b += ewin->client.h;
	EwinOpResize(ewin, OPSRC_USER, a, b);
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

     case EWIN_OP_FULLSCREEN:
	on = ewin->state.fullscreen;
	if (SetEwinBoolean(wop->name, &on, param1, 1))
	   EwinOpFullscreen(ewin, OPSRC_USER, on);
	break;

     case EWIN_OP_ZOOM:
	if (InZoom())
	   Zoom(NULL);
	else
	   Zoom(ewin);
	break;

     case EWIN_OP_LAYER:
	if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("window layer: %d", EoGetLayer(ewin));
	     goto done;
	  }
	val = atoi(param1);
	EwinOpSetLayer(ewin, OPSRC_USER, val);
	break;

     case EWIN_OP_RAISE:
	EwinOpRaise(ewin, OPSRC_USER);
	break;

     case EWIN_OP_LOWER:
	EwinOpLower(ewin, OPSRC_USER);
	break;

     case EWIN_OP_OPACITY:
	val = OpacityToPercent(ewin->ewmh.opacity);
	if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("opacity: %u", val);
	     goto done;
	  }
	if (!strcmp(param1, "opaque_when_focused"))
	  {
	     ewin->props.opaque_when_focused = !ewin->props.opaque_when_focused;
	  }
	else
	  {
	     sscanf(param1, "%i", &val);
	     val = OpacityFix(val);
	  }
	EwinOpSetOpacity(ewin, OPSRC_USER, val);
	break;

     case EWIN_OP_SNAP:
	SnapshotEwinParse(ewin, atword(params, 3));
	break;

     case EWIN_OP_SKIP_LISTS:
	on = ewin->props.skip_ext_task;
	if (SetEwinBoolean(wop->name, &on, param1, 1))
	   EwinOpSkipLists(ewin, OPSRC_USER, on);
	break;

     case EWIN_OP_NEVER_USE_AREA:
	on = ewin->props.never_use_area;
	SetEwinBoolean(wop->name, &on, param1, 1);
	ewin->props.never_use_area = on;
	break;

     case EWIN_OP_FOCUS_CLICK:
	on = ewin->props.focusclick;
	SetEwinBoolean(wop->name, &on, param1, 1);
	ewin->props.focusclick = on;
	break;

     case EWIN_OP_NO_BUTTON_GRABS:
	on = ewin->props.no_button_grabs;
	if (SetEwinBoolean(wop->name, &on, param1, 1))
	  {
	     ewin->props.no_button_grabs = on;
	     if (ewin->props.no_button_grabs)
		UnGrabButtonGrabs(ewin);
	     else
		GrabButtonGrabs(ewin);
	  }
	break;

     case EWIN_OP_INH_APP_FOCUS:
	on = EwinInhGetApp(ewin, focus);
	SetEwinBoolean(wop->name, &on, param1, 1);
	EwinInhSetApp(ewin, focus, on);
	break;

     case EWIN_OP_INH_APP_MOVE:
	on = EwinInhGetApp(ewin, move);
	SetEwinBoolean(wop->name, &on, param1, 1);
	EwinInhSetApp(ewin, move, on);
	break;

     case EWIN_OP_INH_APP_SIZE:
	on = EwinInhGetApp(ewin, size);
	SetEwinBoolean(wop->name, &on, param1, 1);
	EwinInhSetApp(ewin, size, on);
	break;

     case EWIN_OP_INH_USER_CLOSE:
	on = EwinInhGetUser(ewin, close);
	SetEwinBoolean(wop->name, &on, param1, 1);
	EwinInhSetUser(ewin, close, on);
	EwinStateUpdate(ewin);
	HintsSetWindowState(ewin);
	break;

     case EWIN_OP_INH_USER_MOVE:
	on = EwinInhGetUser(ewin, move);
	SetEwinBoolean(wop->name, &on, param1, 1);
	EwinInhSetUser(ewin, move, on);
	EwinStateUpdate(ewin);
	HintsSetWindowState(ewin);
	break;

     case EWIN_OP_INH_USER_SIZE:
	on = EwinInhGetUser(ewin, size);
	SetEwinBoolean(wop->name, &on, param1, 1);
	EwinInhSetUser(ewin, size, on);
	EwinStateUpdate(ewin);
	HintsSetWindowState(ewin);
	break;

     case EWIN_OP_INH_WM_FOCUS:
	on = EwinInhGetWM(ewin, focus);
	SetEwinBoolean(wop->name, &on, param1, 1);
	EwinInhSetWM(ewin, focus, on);
	EwinStateUpdate(ewin);
	break;

#if USE_COMPOSITE
     case EWIN_OP_SHADOW:
	on = EoGetShadow(ewin);
	if (SetEwinBoolean(wop->name, &on, param1, 1))
	   EoSetShadow(ewin, on);
	break;

     case EWIN_OP_NO_REDIRECT:
	on = EoGetNoRedirect(ewin);
	on = ewin->o.noredir;
	if (SetEwinBoolean(wop->name, &on, param1, 1))
	   EoSetNoRedirect(ewin, on);
	break;
#endif
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
EwinShowInfo(const EWin * ewin)
{
   int                 bl, br, bt, bb;

   EwinBorderGetSize(ewin, &bl, &br, &bt, &bb);

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
	     "NeverUseArea %i   FixedPos     %i   FixedSize    %i\n"
	     "Desktop      %i   Layer        %i(%i)\n"
	     "Iconified    %i   Sticky       %i   Shaded       %i   Docked       %i\n"
	     "State        %i   Shown        %i   Visibility   %i   Active       %i\n"
	     "Member of groups        %i\n"
#if USE_COMPOSITE
	     "Opacity    %3i(%x)  Shadow       %i   NoRedirect   %i\n"
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
	     _EwinGetClientXwin(ewin),
	     ewin->client.x, ewin->client.y, ewin->client.w, ewin->client.h,
	     _EwinGetContainerXwin(ewin),
	     EoGetXwin(ewin),
	     EoGetX(ewin), EoGetY(ewin), EoGetW(ewin), EoGetH(ewin),
#if USE_COMPOSITE
	     EoGetPixmap(ewin),
#endif
	     EwinBorderGetName(ewin), bl, br, bt, bb,
	     ewin->icccm.icon_win,
	     ewin->icccm.icon_pmap, ewin->icccm.icon_mask,
	     EwinIsWindowGroupLeader(ewin), EwinGetWindowGroup(ewin),
	     ewin->icccm.client_leader, EwinGetTransientCount(ewin),
	     EwinIsTransient(ewin), EwinGetTransientFor(ewin),
	     ewin->props.no_resize_h, ewin->props.no_resize_v,
	     ewin->state.shaped, ewin->icccm.base_w, ewin->icccm.base_h,
	     ewin->icccm.width.min, ewin->icccm.height.min,
	     ewin->icccm.width.max, ewin->icccm.height.max,
	     ewin->icccm.w_inc, ewin->icccm.h_inc,
	     ewin->icccm.aspect_min, ewin->icccm.aspect_max,
	     ewin->strut.left, ewin->strut.right,
	     ewin->strut.top, ewin->strut.bottom,
	     ewin->mwm.decor_border, ewin->mwm.decor_resizeh,
	     ewin->mwm.decor_title, ewin->mwm.decor_menu,
	     ewin->mwm.decor_minimize, ewin->mwm.decor_maximize,
	     ewin->icccm.need_input, ewin->icccm.take_focus,
	     EwinInhGetWM(ewin, focus), ewin->props.focusclick,
	     ewin->props.never_use_area, EwinInhGetUser(ewin, move),
	     EwinInhGetUser(ewin, size), EoGetDeskNum(ewin),
	     EoGetLayer(ewin), ewin->o.ilayer,
	     ewin->state.iconified, EoIsSticky(ewin), ewin->state.shaded,
	     ewin->state.docked, ewin->state.state, EoIsShown(ewin),
	     ewin->state.visibility, ewin->state.active, ewin->num_groups,
	     OpacityToPercent(ewin->ewmh.opacity)
#if USE_COMPOSITE
	     , EoGetOpacity(ewin), EoGetShadow(ewin), EoGetNoRedirect(ewin)
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
	   EwinShowInfo(lst[i]);
     }
   else
     {
	ewin = IpcFindEwin(param1);
	if (ewin)
	   EwinShowInfo(ewin);
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

   IpcPrintf
      ("Num    window T V Sh  Dsk S  F   L     pos       size    C R Name\n");
   for (i = 0; i < num; i++)
     {
	eo = lst[i];
	IpcPrintf
	   (" %2d %#9lx %d %d %2d  %3d %d  %d %3d %5d,%5d %4dx%4d %d %d %s\n",
	    i, EobjGetXwin(eo), eo->type, eo->shown, eo->shaped, eo->desk->num,
	    eo->sticky, eo->floating, eo->ilayer, eo->x, eo->y, eo->w, eo->h,
#if USE_COMPOSITE
	    (eo->cmhook) ? 1 : 0, !eo->noredir
#else
	    0, 0
#endif
	    , eo->name);
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
      EwinReparent(ewin, _EwinGetClientXwin(enew));
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
	EQueryPointer(None, &x, &y, NULL, NULL);
	IpcPrintf("Pointer location: %d %d\n", x, y);
     }
   else if (!strncmp(params, "abs", 3))
     {
	sscanf(params, "%*s %i %i", &x, &y);
	EWarpPointer(VRoot.win, x, y);
     }
   else if (!strncmp(params, "rel", 3))
     {
	sscanf(params, "%*s %i %i", &x, &y);
	EWarpPointer(None, x, y);
     }
   else if (!strncmp(params, "scr", 3))
     {
	x = (VRoot.scr + 1) % ScreenCount(disp);
	sscanf(params, "%*s %i", &x);
	if (x >= 0 && x < ScreenCount(disp))
	   EWarpPointer(RootWindow(disp, x), DisplayWidth(disp, x) / 2,
			DisplayHeight(disp, x) / 2);
     }
   else
     {
	sscanf(params, "%i %i", &x, &y);
	EWarpPointer(None, x, y);
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
	   IpcPrintf("Current Desktop: %d\n", DesksGetCurrentNum());
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

static const IpcItem IPCArray[] = {
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
    "  win_op <windowid> border <BORDERNAME>\n"
    "  win_op <windowid> title <title>\n"
    "  win_op <windowid> <close/kill>\n"
    "  win_op <windowid> <focus/iconify/shade/stick>\n"
    "  win_op <windowid> desk <desktochangeto/next/prev>\n"
    "  win_op <windowid> area <x> <y>\n"
    "  win_op <windowid> <move/size> <x> <y>\n"
    "          (you can use ? and ?? to retreive client and frame locations)\n"
    "  win_op <windowid> <mr/sr> <x> <y>   (incremental move/size)\n"
    "  win_op <windowid> toggle_<width/height/size> <conservative/available/xinerama>\n"
    "  win_op <windowid> <fullscreen/zoom>\n"
    "  win_op <windowid> layer <0-100,4=normal>\n"
    "  win_op <windowid> <raise/lower>\n"
    "  win_op <windowid> opacity <1-100(100=opaque),opaque_when_focused>\n"
    "  win_op <windowid> snap <what>\n"
    "         <what>: all, none, border, command, desktop, dialog, group, icon,\n"
    "                 layer, location, opacity, shade, shadow, size, sticky\n"
    "  win_op <windowid> <focusclick/never_use_area/no_button_grabs/skiplists>\n"
    "  win_op <windowid> <no_app_focus/move/size>\n"
    "  win_op <windowid> <no_user_close/move/size>\n"
    "  win_op <windowid> <no_wm_focus>\n"
    "  win_op <windowid> noshadow\n"
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
    IPC_Border, "border", NULL, "List available borders", NULL},
   {
    IPC_Screen, "screen", NULL, "Return screen information", NULL},
   {
    SnapIpcFunc,
    "list_remember", "rl",
    "Retrieve a list of remembered windows and their attributes",
    SnapIpcText},
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
    IPC_EwinInfo, "win_info", "wi", "Show client window info", NULL},
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

   w[0] = 0;
   word(params, 1, w);

   ok = 0;
   for (i = 0; i < num; i++)
     {
	ipc = lst[i];
	if (!(ipc->nick && !strcmp(w, ipc->nick)) && strcmp(w, ipc->name))
	   continue;

	w[0] = 0;
	word(params, 2, w);
	if (w[0])
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

static void
doEFuncDeferred(int val __UNUSED__, void *data)
{
   void              **prm = (void **)data;
   EWin               *ewin;

   ewin = prm[0];
   if (ewin && !EwinFindByPtr(ewin))
      return;

   EFunc(ewin, prm[1]);

   Efree(prm[1]);
   Efree(data);
}

void
EFuncDefer(EWin * ewin, const char *cmd)
{
   static int          seqn = 0;
   char                s[32];
   void              **prm;

   prm = Emalloc(2 * sizeof(void *));
   if (!prm)
      return;
   prm[0] = ewin;
   prm[1] = Estrdup(cmd);

   Esnprintf(s, sizeof(s), "EFunc-%d", seqn++);
   DoIn(s, 0.0, doEFuncDeferred, 0, prm);
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
