/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2005 Kim Woelders
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
#include "conf.h"
#include "desktops.h"
#include "emodule.h"
#include "ewins.h"
#include "ewin-ops.h"

static int          WindowMatchEwinOpsParse(EWin * ewin, const char *ops);

struct _windowmatch
{
   char               *name;
   /* Match criteria */
   char                match;
   char                op;
   char                prop;
   char                qual;
   char               *value;
   Constraints         width;
   Constraints         height;
   /* Match actions */
   char               *args;
   Border             *border;
};

#define MATCH_TYPE_TITLE        1
#define MATCH_TYPE_WM_NAME      2
#define MATCH_TYPE_WM_CLASS     3
#define MATCH_TYPE_SIZE         4
#define MATCH_TYPE_SIZE_H       5
#define MATCH_TYPE_SIZE_V       6
#define MATCH_TYPE_PROP         7

#define MATCH_PROP_TRANSIENT    1
#define MATCH_PROP_SHAPED       2
#define MATCH_PROP_FIXEDSIZE    3
#define MATCH_PROP_FIXEDSIZE_H  4
#define MATCH_PROP_FIXEDSIZE_V  5

#define MATCH_OP_BORDER         1
#define MATCH_OP_ICON           2
#define MATCH_OP_WINOP          3

const char         *MatchType[] = {
   NULL, "Title", "Name", "Class", "Size", "Width", "Height", "Prop", NULL
};

const char         *MatchProp[] = {
   NULL, "Transient", "Shaped", "FixedSize", "FixedWidth", "FixedHeight", NULL
};

const char         *MatchOp[] = {
   NULL, "Border", "Icon", "Winop", NULL
};

static int
MatchFind(const char **list, const char *str)
{
   int                 i;

   for (i = 1; list[i]; i++)
      if (!strcmp(str, list[i]))
	 return i;

   return 0;
}

static WindowMatch *
WindowMatchCreate(const char *name)
{
   WindowMatch        *b;

   b = Ecalloc(1, sizeof(WindowMatch));
   if (!b)
      return NULL;

   b->name = Estrdup(name);
   b->width.max = 99999;
   b->height.max = 99999;

   return b;
}

static void
WindowMatchDestroy(WindowMatch * wm)
{
   if (!wm)
      return;

   while (RemoveItemByPtr(wm, LIST_TYPE_WINDOWMATCH));

   if (wm->name)
      Efree(wm->name);
   if (wm->value)
      Efree(wm->value);
   if (wm->border)
      BorderDecRefcount(wm->border);
   if (wm->args)
      Efree(wm->args);

   Efree(wm);
}

int
WindowMatchConfigLoad(FILE * fs)
{
   int                 err = 0;
   WindowMatch        *wm = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   int                 fields;

   while (GetLine(s, sizeof(s), fs))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	   i1 = CONFIG_INVALID;
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
		Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
	  }

	switch (i1)
	  {
	  case CONFIG_VERSION:
	  case CONFIG_WINDOWMATCH:
	     err = -1;
	     break;

	  case CONFIG_CLASSNAME:
	     wm = WindowMatchCreate(s2);
	     break;

	  case CONFIG_CLOSE:
	     if (wm)
	       {
		  if (wm->match && wm->op)
		     AddItemEnd(wm, wm->name, 0, LIST_TYPE_WINDOWMATCH);
		  else
		    {
		       WindowMatchDestroy(wm);
		    }
		  wm = NULL;
		  err = 0;
	       }
	     goto done;

	  case WINDOWMATCH_MATCHTITLE:
	     if (!wm)
		break;
	     wm->match = MATCH_TYPE_TITLE;
	     wm->value = Estrdup(atword(s, 2));
	     break;
	  case WINDOWMATCH_MATCHNAME:
	     if (!wm)
		break;
	     wm->match = MATCH_TYPE_WM_NAME;
	     wm->value = Estrdup(atword(s, 2));
	     break;
	  case WINDOWMATCH_MATCHCLASS:
	     if (!wm)
		break;
	     wm->match = MATCH_TYPE_WM_CLASS;
	     wm->value = Estrdup(atword(s, 2));
	     break;

	  case WINDOWMATCH_WIDTH:
	     if (!wm)
		break;
	     if (wm->match == MATCH_TYPE_SIZE_V)
		wm->match = MATCH_TYPE_SIZE;
	     else
		wm->match = MATCH_TYPE_SIZE_H;
	     sscanf(s, "%*s %u %u", &(wm->width.min), &(wm->width.max));
	     break;
	  case WINDOWMATCH_HEIGHT:
	     if (!wm)
		break;
	     if (wm->match == MATCH_TYPE_SIZE_H)
		wm->match = MATCH_TYPE_SIZE;
	     else
		wm->match = MATCH_TYPE_SIZE_V;
	     sscanf(s, "%*s %u %u", &(wm->height.min), &(wm->height.max));
	     break;

	  case WINDOWMATCH_TRANSIENT:
	     if (!wm)
		break;
	     wm->match = MATCH_TYPE_PROP;
	     wm->prop = MATCH_PROP_TRANSIENT;
	     wm->qual = !atoi(s2);
	     break;
	  case WINDOWMATCH_SHAPED:
	     if (!wm)
		break;
	     wm->match = MATCH_TYPE_PROP;
	     wm->prop = MATCH_PROP_SHAPED;
	     wm->qual = !atoi(s2);
	     break;
	  case WINDOWMATCH_NO_RESIZE_H:
	     if (!wm)
		break;
	     wm->match = MATCH_TYPE_PROP;
	     if (wm->prop == MATCH_PROP_FIXEDSIZE_V)
		wm->prop = MATCH_PROP_FIXEDSIZE;
	     else
		wm->prop = MATCH_PROP_FIXEDSIZE_H;
	     wm->qual = !atoi(s2);
	     break;
	  case WINDOWMATCH_NO_RESIZE_V:
	     if (!wm)
		break;
	     wm->match = MATCH_TYPE_PROP;
	     if (wm->prop == MATCH_PROP_FIXEDSIZE_H)
		wm->prop = MATCH_PROP_FIXEDSIZE;
	     else
		wm->prop = MATCH_PROP_FIXEDSIZE_V;
	     wm->qual = !atoi(s2);
	     break;

	  case CONFIG_BORDER:
	  case WINDOWMATCH_USEBORDER:
	     if (!wm)
		break;
	     wm->border = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);
	     if (!wm->border)
		break;
	     wm->op = MATCH_OP_BORDER;
	     BorderIncRefcount(wm->border);
	     break;

	  case WINDOWMATCH_ICON:
	  case CONFIG_ICONBOX:
#if 0				/* This has not been active since at least 0.16.5 */
	     if (!wm)
		break;
	     wm->icon = ImageclassFind(s2, 0);
	     if (!wm->icon)
		break;
	     wm->op = MATCH_OP_ICON;
	     wm->icon->ref_count++;
#endif
	     break;

	  case WINDOWMATCH_DESKTOP:
	  case CONFIG_DESKTOP:
#if 0				/* This has not been active since at least 0.16.5 */
	     wm->desk = atoi(s2);
#endif
	     break;

	  case WINDOWMATCH_MAKESTICKY:
	     if (!wm)
		break;
	     wm->args = Estrdupcat2(wm->args, ":", "stick");
	     break;

	  default:
	     Alert(_("Warning: unable to determine what to do with\n"
		     "the following text in the middle of current "
		     "WindowMatch definition:\n"
		     "%s\nWill ignore and continue...\n"), s);
	     break;
	  }
     }

 done:
   return err;
}

static WindowMatch *
WindowMatchDecode(const char *line)
{
   char                match[32], value[1024], op[32];
   const char         *args;
   WindowMatch        *wm = NULL;
   int                 err, num, w1, w2, h1, h2;

   match[0] = value[0] = op[0] = '\0';
   num = sscanf(line, "%32s %1024s %32s %n", match, value, op, &w1);
   if (num < 3)
      return NULL;
   args = line + w1;
   if (*args == '\0')
      return NULL;

   err = 0;

   num = MatchFind(MatchType, match);
   if (num <= 0)
     {
	Eprintf("WindowMatchDecode: Error (%s): %s\n", match, line);
	err = 1;
	goto done;
     }

   wm = WindowMatchCreate(NULL);
   if (!wm)
      return NULL;

   wm->match = num;

   switch (wm->match)
     {
     case MATCH_TYPE_TITLE:
     case MATCH_TYPE_WM_NAME:
     case MATCH_TYPE_WM_CLASS:
	wm->value = Estrdup(value);
	break;

     case MATCH_TYPE_SIZE:
	num = sscanf(value, "%u-%ux%u-%u", &w1, &w2, &h1, &h2);
	if (num < 4)
	   goto case_error;
	wm->width.min = w1;
	wm->width.max = w2;
	wm->height.min = h1;
	wm->height.max = h2;
	break;
     case MATCH_TYPE_SIZE_H:
	num = sscanf(value, "%u-%u", &w1, &w2);
	if (num < 2)
	   goto case_error;
	wm->width.min = w1;
	wm->width.max = w2;
	break;
     case MATCH_TYPE_SIZE_V:
	num = sscanf(value, "%u-%u", &h1, &h2);
	if (num < 2)
	   goto case_error;
	wm->height.min = h1;
	wm->height.max = h2;
	break;

     case MATCH_TYPE_PROP:
	num = 0;
	if (*value == '!')
	  {
	     wm->qual = 1;
	     num = 1;
	  }
	wm->prop = MatchFind(MatchProp, value + num);
	if (wm->prop <= 0)
	   goto case_error;
	break;

      case_error:
	Eprintf("WindowMatchDecode: Error (%s): %s\n", value, line);
	err = 1;
	goto done;
     }

   wm->op = MatchFind(MatchOp, op);
   if (wm->op <= 0)
     {
	Eprintf("WindowMatchDecode: Error (%s): %s\n", op, line);
	err = 1;
	goto done;
     }

   switch (wm->op)
     {
     case MATCH_OP_BORDER:
	wm->border = FindItem(args, 0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);
	if (!wm->border)
	  {
	     err = 1;
	     goto done;
	  }
	BorderIncRefcount(wm->border);
	break;

     case MATCH_OP_ICON:
	/* FIXME - Check if exists */
	wm->args = Estrdup(args);
	break;

     case MATCH_OP_WINOP:
	if (WindowMatchEwinOpsParse(NULL, args))
	  {
	     Eprintf("WindowMatchDecode: Error (%s): %s\n", args, line);
	     err = 1;
	     goto done;
	  }
	wm->args = Estrdup(args);
	break;
     }

 done:
   if (err)
     {
	if (wm)
	   WindowMatchDestroy(wm);
     }
   else
     {
	AddItemEnd(wm, wm->name, 0, LIST_TYPE_WINDOWMATCH);
     }
   return wm;
}

static char        *
WindowMatchEncode(WindowMatch * wm, char *buf, int len)
{
   char                s[1024];
   const char         *qual, *value, *args;

   qual = " ";

   switch (wm->match)
     {
     default:
	value = wm->value;
	break;

     case MATCH_TYPE_SIZE:
	value = s;
	sprintf(s, "%u-%ux%u-%u", wm->width.min, wm->width.max,
		wm->height.min, wm->height.max);
	break;
     case MATCH_TYPE_SIZE_H:
	value = s;
	sprintf(s, "%u-%u", wm->width.min, wm->width.max);
	break;
     case MATCH_TYPE_SIZE_V:
	value = s;
	sprintf(s, "%u-%u", wm->height.min, wm->height.max);
	break;

     case MATCH_TYPE_PROP:
	qual = (wm->qual) ? "!" : " ";
	value = MatchProp[(int)wm->prop];
	break;
     }

   switch (wm->op)
     {
     default:
	args = wm->args;
	break;

     case MATCH_OP_BORDER:
	args = BorderGetName(wm->border);
	break;
     }

   Esnprintf(buf, len, "%-8s %s%-16s %s %s", MatchType[(int)wm->match],
	     qual, value, MatchOp[(int)wm->op], args);

   return buf;
}

static void
WindowMatchConfigLoad2(FILE * fs)
{
   char                s[FILEPATH_LEN_MAX], *ss;
   int                 len;

   for (;;)
     {
	ss = fgets(s, sizeof(s), fs);
	if (!ss)
	   break;

	len = strcspn(s, "#\r\n");
	if (len <= 0)
	   continue;
	s[len] = '\0';

	WindowMatchDecode(s);
     }
}

static void
WindowMatchConfigLoadConfig(void)
{
   char               *file;
   FILE               *fs;

   file = ConfigFileFind("matches.cfg", NULL, 0);
   if (!file)
      return;

   fs = fopen(file, "r");
   Efree(file);
   if (!fs)
      return;

   WindowMatchConfigLoad2(fs);

   fclose(fs);
}

static int
WindowMatchTest(const EWin * ewin, const WindowMatch * wm)
{
   int                 match;

   match = 0;

   switch (wm->match)
     {
     case MATCH_TYPE_TITLE:
	return matchregexp(wm->value, ewin->icccm.wm_name);

     case MATCH_TYPE_WM_NAME:
	return matchregexp(wm->value, ewin->icccm.wm_res_name);

     case MATCH_TYPE_WM_CLASS:
	return matchregexp(wm->value, ewin->icccm.wm_res_class);

     case MATCH_TYPE_SIZE:
	match = (ewin->client.w >= wm->width.min &&
		 ewin->client.w <= wm->width.max &&
		 ewin->client.h >= wm->height.min &&
		 ewin->client.h <= wm->height.max);
	break;
     case MATCH_TYPE_SIZE_H:
	match = (ewin->client.w >= wm->width.min &&
		 ewin->client.w <= wm->width.max);
	break;
     case MATCH_TYPE_SIZE_V:
	match = (ewin->client.h >= wm->height.min &&
		 ewin->client.h <= wm->height.max);
	break;

     case MATCH_TYPE_PROP:
	switch (wm->prop)
	  {
	  case MATCH_PROP_TRANSIENT:
	     match = EwinIsTransient(ewin);
	     break;

	  case MATCH_PROP_SHAPED:
	     match = ewin->state.shaped;
	     break;

	  case MATCH_PROP_FIXEDSIZE:
	     match = ewin->props.no_resize_h && ewin->props.no_resize_v;
	     break;
	  case MATCH_PROP_FIXEDSIZE_H:
	     match = ewin->props.no_resize_h;
	     break;
	  case MATCH_PROP_FIXEDSIZE_V:
	     match = ewin->props.no_resize_v;
	     break;
	  }
     }

   if (wm->qual)
      match = !match;
   return match;
}

static WindowMatch *
WindowMatchType(const EWin * ewin, int type)
{
   WindowMatch       **lst, *wm;
   int                 i, num;

   lst = (WindowMatch **) ListItemType(&num, LIST_TYPE_WINDOWMATCH);
   for (i = 0; i < num; i++)
     {
	wm = lst[i];

	if (wm->op != type)
	   continue;

	if (!WindowMatchTest(ewin, lst[i]))
	   continue;
	goto done;
     }
   wm = NULL;

 done:
   if (lst)
      Efree(lst);

   return wm;
}

Border             *
WindowMatchEwinBorder(const EWin * ewin)
{
   WindowMatch        *wm;

   wm = WindowMatchType(ewin, MATCH_OP_BORDER);
#if 0
   Eprintf("WindowMatchEwinBorder %s %s\n", EwinGetName(ewin),
	   (wm) ? BorderGetName(wm->border) : "???");
#endif
   if (wm)
      return wm->border;
   return NULL;
}

const char         *
WindowMatchEwinIcon(const EWin * ewin)
{
   WindowMatch        *wm;

   wm = WindowMatchType(ewin, MATCH_OP_ICON);
#if 0
   Eprintf("WindowMatchEwinIcon %s %s\n", EwinGetName(ewin),
	   (wm) ? wm->args : "???");
#endif
   if (wm)
      return wm->args;
   return NULL;
}

static int
GetBoolean(const char *value)
{
   /* We set off if "0" or "off", otherwise on */
   if (!value)
      return 1;
   else if (!strcmp(value, "0") || !strcmp(value, "off"))
      return 0;
   return 1;
}

#define WINOP_GET_BOOL(item, val) item = GetBoolean(val)

static void
WindowMatchEwinOpsAction(EWin * ewin, int op, const char *args)
{
   int                 a, b;

   /* NB! This must only be used when a new client is being adopted */

   switch (op)
     {
     default:
	/* We should not get here */
	return;

     case EWIN_OP_ICONIFY:
	WINOP_GET_BOOL(ewin->icccm.start_iconified, args);
	break;

     case EWIN_OP_SHADE:
	WINOP_GET_BOOL(ewin->state.shaded, args);
	break;

     case EWIN_OP_STICK:
	WINOP_GET_BOOL(ewin->o.sticky, args);
	break;

     case EWIN_OP_FIXED_POS:
	WINOP_GET_BOOL(ewin->props.fixedpos, args);
	break;

     case EWIN_OP_FIXED_SIZE:
	WINOP_GET_BOOL(ewin->props.fixedsize, args);
	break;

     case EWIN_OP_NEVER_USE_AREA:
	WINOP_GET_BOOL(ewin->props.never_use_area, args);
	break;

     case EWIN_OP_FOCUS_CLICK:
	WINOP_GET_BOOL(ewin->props.focusclick, args);
	break;

     case EWIN_OP_FOCUS_NEVER:
	WINOP_GET_BOOL(ewin->props.never_focus, args);
	break;

     case EWIN_OP_NO_BUTTON_GRABS:
	WINOP_GET_BOOL(ewin->props.no_button_grabs, args);
	break;

     case EWIN_OP_FULLSCREEN:
	WINOP_GET_BOOL(ewin->state.fullscreen, args);
	break;

     case EWIN_OP_SKIP_LISTS:
	WINOP_GET_BOOL(ewin->props.skip_winlist, args);
	ewin->props.skip_focuslist = ewin->props.skip_ext_task =
	   ewin->props.skip_winlist;
	break;

     case EWIN_OP_OPACITY:
	ewin->ewmh.opacity = OpacityExt(atoi(args));
	break;

#if USE_COMPOSITE
     case EWIN_OP_SHADOW:
	WINOP_GET_BOOL(ewin->o.shadow, args);
	break;

     case EWIN_OP_NO_REDIRECT:
	WINOP_GET_BOOL(ewin->o.noredir, args);
	break;
#endif

     case EWIN_OP_TITLE:
	_EFREE(ewin->icccm.wm_name);
	ewin->icccm.wm_name = Estrdup(args);
	break;

     case EWIN_OP_LAYER:
	EoSetLayer(ewin, atoi(args));
	break;

     case EWIN_OP_DESK:
	EoSetDesk(ewin, DeskGet(atoi(args)));
	break;

     case EWIN_OP_AREA:
	a = b = 0;
	if (sscanf(args, "%u %u", &a, &b) < 2)
	   break;
	EwinMoveToArea(ewin, a, b);	/* FIXME - We should not move here */
	break;

     case EWIN_OP_MOVE:
	a = ewin->client.x;
	b = ewin->client.y;
	if (sscanf(args, "%i %i", &a, &b) < 2)
	   break;
	ewin->client.x = a;
	ewin->client.y = b;
	ewin->state.placed = 1;
	break;

     case EWIN_OP_SIZE:
	a = ewin->client.w;
	b = ewin->client.h;
	if (sscanf(args, "%u %u", &a, &b) < 2)
	   break;
	ewin->client.w = a;
	ewin->client.h = b;
	break;
     }
}

static int
WindowMatchEwinOpsParse(EWin * ewin, const char *ops)
{
   int                 err, len;
   const WinOp        *wop;
   char               *ops2, *s, *p, op[32];

   if (!ops)
      return -1;

   /* Parse ':' separated operations list, e.g. "layer 3:desk 1: shade" */
   p = ops2 = Estrdup(ops);

   err = 0;
   for (; p; p = s)
     {
	/* Break at ':' */
	s = strchr(p, ':');
	if (s)
	   *s++ = '\0';

	len = 0;
	sscanf(p, "%31s %n", op, &len);
	if (len <= 0)
	   break;
	p += len;

	wop = EwinOpFind(op);
	if (!wop || !wop->ok_match)
	  {
	     err = -1;
	     break;
	  }

	/* If ewin is NULL, we are validating the configuration */
	if (ewin)
	   WindowMatchEwinOpsAction(ewin, wop->op, p);
     }

   Efree(ops2);

   return err;
}

void
WindowMatchEwinOps(EWin * ewin)
{
   WindowMatch       **lst, *wm;
   int                 i, num;

   lst = (WindowMatch **) ListItemType(&num, LIST_TYPE_WINDOWMATCH);
   for (i = 0; i < num; i++)
     {
	wm = lst[i];

	if (wm->op != MATCH_OP_WINOP)
	   continue;

	if (!WindowMatchTest(ewin, lst[i]))
	   continue;

	/* Match found - do the ops */
	WindowMatchEwinOpsParse(ewin, wm->args);
     }

   if (lst)
      Efree(lst);
}

/*
 * Winmatch module
 */

static void
WindowMatchSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_CONFIGURE:
#if 0
	ConfigFileLoad("windowmatches.cfg", Mode.theme.path,
		       WindowMatchConfigLoad);
#endif
	WindowMatchConfigLoadConfig();
#if 0
	WindowMatchConfigLoadUser();
#endif
#if 0
	IcondefChecker(0, NULL);
#endif
	break;
     }
}

static void
WindowMatchIpc(const char *params, Client * c __UNUSED__)
{
   const char         *p;
   char                cmd[128], prm[4096], buf[4096];
   int                 i, len, num;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %4000s %n", cmd, prm, &len);
	p += len;
     }

   if (!p || cmd[0] == '?')
     {
     }
   else if (!strncmp(cmd, "list", 2))
     {
	WindowMatch       **lst;

	lst = (WindowMatch **) ListItemType(&num, LIST_TYPE_WINDOWMATCH);
	for (i = 0; i < num; i++)
	   IpcPrintf("%s\n", WindowMatchEncode(lst[i], buf, sizeof(buf)));
	if (lst)
	   Efree(lst);
     }
}

IpcItem             WindowMatchIpcArray[] = {
   {
    WindowMatchIpc,
    "wmatch", "wma",
    "Window match functions",
    "  wmatch list               List window matches\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(WindowMatchIpcArray)/sizeof(IpcItem))

/*
 * Module descriptor
 */
EModule             ModWindowMatch = {
   "winmatch", NULL,
   WindowMatchSighan,
   {N_IPC_FUNCS, WindowMatchIpcArray}
   ,
   {0, NULL}
};
