/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2008 Kim Woelders
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
#include "e16-ecore_list.h"
#include "emodule.h"
#include "iclass.h"
#include "tclass.h"
#include "xwin.h"

#define ENABLE_DESTROY 0	/* Broken */

static Ecore_List  *tclass_list = NULL;

static TextClass   *TextclassGetFallback(void);

static char        *
TextstateFontLookup(const char *name)
{
   const char         *font;

   if (*name == '*')
     {
	font = FontLookup(name + 1);
	if (font)
	   name = font;
     }
   return Estrdup(name);
}

static TextState   *
TextstateCreate(const char *font)
{
   TextState          *ts;

   ts = ECALLOC(TextState, 1);
   if (!ts)
      return NULL;

   ts->style.mode = MODE_WRAP_CHAR;
   ts->style.orientation = FONT_TO_RIGHT;

   if (font)
     {
	ts->fontname = TextstateFontLookup(font);
	ts->style.mode = MODE_VERBATIM;
     }

   return ts;
}

#if ENABLE_DESTROY
static void
TextStateDestroy(TextState * ts)
{
   if (!ts)
      return;

   Efree(ts->fontname);
   if (ts->ops)
      ts->ops->Destroy(ts);

   Efree(ts);
}
#endif

static TextClass   *
TextclassCreate(const char *name)
{
   TextClass          *tc;

   tc = ECALLOC(TextClass, 1);
   if (!tc)
      return NULL;

   if (!tclass_list)
      tclass_list = ecore_list_new();
   ecore_list_prepend(tclass_list, tc);

   tc->name = Estrdup(name);
   tc->justification = 512;

   return tc;
}

#if ENABLE_DESTROY
static void
TextclassDestroy(TextClass * tc)
{
   if (tc->ref_count > 0)
     {
	DialogOK("TextClass Error!", _("%u references remain\n"),
		 tc->ref_count);
	return;
     }
   Efree(tc->name);
   TextStateDestroy(tc->norm.normal);
   TextStateDestroy(tc->norm.hilited);
   TextStateDestroy(tc->norm.clicked);
   TextStateDestroy(tc->norm.disabled);
   TextStateDestroy(tc->active.normal);
   TextStateDestroy(tc->active.hilited);
   TextStateDestroy(tc->active.clicked);
   TextStateDestroy(tc->active.disabled);
   TextStateDestroy(tc->sticky.normal);
   TextStateDestroy(tc->sticky.hilited);
   TextStateDestroy(tc->sticky.clicked);
   TextStateDestroy(tc->sticky.disabled);
   TextStateDestroy(tc->sticky_active.normal);
   TextStateDestroy(tc->sticky_active.hilited);
   TextStateDestroy(tc->sticky_active.clicked);
   TextStateDestroy(tc->sticky_active.disabled);

   Efree(tc);
}
#endif /* ENABLE_DESTROY */

TextClass          *
TextclassAlloc(const char *name, int fallback)
{
   TextClass          *tc;

   if (!name || !name[0])
      return NULL;

   tc = TextclassFind(name, fallback);
   if (tc)
      tc->ref_count++;

   return tc;
}

void
TextclassFree(TextClass * tc)
{
   if (tc)
      tc->ref_count--;
}

int
TextclassGetJustification(TextClass * tc)
{
   return tc->justification;
}

void
TextclassSetJustification(TextClass * tc, int just)
{
   tc->justification = just;
}

static void
TextclassPopulate(TextClass * tclass)
{
   if (!tclass)
      return;

   if (!tclass->norm.normal)
      return;

   if (!tclass->norm.hilited)
      tclass->norm.hilited = tclass->norm.normal;
   if (!tclass->norm.clicked)
      tclass->norm.clicked = tclass->norm.normal;
   if (!tclass->norm.disabled)
      tclass->norm.disabled = tclass->norm.normal;

   if (!tclass->active.normal)
      tclass->active.normal = tclass->norm.normal;
   if (!tclass->active.hilited)
      tclass->active.hilited = tclass->active.normal;
   if (!tclass->active.clicked)
      tclass->active.clicked = tclass->active.normal;
   if (!tclass->active.disabled)
      tclass->active.disabled = tclass->active.normal;

   if (!tclass->sticky.normal)
      tclass->sticky.normal = tclass->norm.normal;
   if (!tclass->sticky.hilited)
      tclass->sticky.hilited = tclass->sticky.normal;
   if (!tclass->sticky.clicked)
      tclass->sticky.clicked = tclass->sticky.normal;
   if (!tclass->sticky.disabled)
      tclass->sticky.disabled = tclass->sticky.normal;

   if (!tclass->sticky_active.normal)
      tclass->sticky_active.normal = tclass->norm.normal;
   if (!tclass->sticky_active.hilited)
      tclass->sticky_active.hilited = tclass->sticky_active.normal;
   if (!tclass->sticky_active.clicked)
      tclass->sticky_active.clicked = tclass->sticky_active.normal;
   if (!tclass->sticky_active.disabled)
      tclass->sticky_active.disabled = tclass->sticky_active.normal;
}

static int
_TextclassMatchName(const void *data, const void *match)
{
   return strcmp(((const TextClass *)data)->name, (const char *)match);
}

TextClass          *
TextclassFind(const char *name, int fallback)
{
   TextClass          *tc = NULL;

   if (name)
      tc = (TextClass *) ecore_list_find(tclass_list, _TextclassMatchName,
					 name);
   if (tc || !fallback)
      return tc;

#if 0
   Eprintf("%s: Get fallback (%s)\n", __func__, name);
#endif
   return TextclassGetFallback();
}

int
TextclassConfigLoad(FILE * fs)
{
   int                 err = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1, r, g, b;
   TextClass          *tc = NULL;
   TextState          *ts = NULL;

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = ConfigParseline1(s, s2, NULL, NULL);
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     TextclassPopulate(tc);
	     goto done;
	  case CONFIG_CLASSNAME:
	     if (TextclassFind(s2, 0))
	       {
		  SkipTillEnd(fs);
		  goto done;
	       }
	     tc = TextclassCreate(s2);
	     break;
	  case TEXT_ORIENTATION:
	     if (ts)
		ts->style.orientation = atoi(s2);
	     break;
	  case TEXT_JUSTIFICATION:
	     if (tc)
		tc->justification = atoi(s2);
	     break;
	  case CONFIG_DESKTOP:
	  case ICLASS_NORMAL:
	     if (tc)
		tc->norm.normal = ts = TextstateCreate(s2);
	     break;
	  case ICLASS_CLICKED:
	     if (tc)
		tc->norm.clicked = ts = TextstateCreate(s2);
	     break;
	  case ICLASS_HILITED:
	     if (tc)
		tc->norm.hilited = ts = TextstateCreate(s2);
	     break;
	  case ICLASS_DISABLED:
	     if (tc)
		tc->norm.disabled = ts = TextstateCreate(s2);
	     break;
	  case ICLASS_STICKY_NORMAL:
	     if (tc)
		tc->sticky.normal = ts = TextstateCreate(s2);
	     break;
	  case ICLASS_STICKY_CLICKED:
	     if (tc)
		tc->sticky.clicked = ts = TextstateCreate(s2);
	     break;
	  case ICLASS_STICKY_HILITED:
	     if (tc)
		tc->sticky.hilited = ts = TextstateCreate(s2);
	     break;
	  case ICLASS_STICKY_DISABLED:
	     if (tc)
		tc->sticky.disabled = ts = TextstateCreate(s2);
	     break;
	  case ICLASS_ACTIVE_NORMAL:
	     if (tc)
		tc->active.normal = ts = TextstateCreate(s2);
	     break;
	  case ICLASS_ACTIVE_CLICKED:
	     if (tc)
		tc->active.clicked = ts = TextstateCreate(s2);
	     break;
	  case ICLASS_ACTIVE_HILITED:
	     if (tc)
		tc->active.hilited = ts = TextstateCreate(s2);
	     break;
	  case ICLASS_ACTIVE_DISABLED:
	     if (tc)
		tc->active.disabled = ts = TextstateCreate(s2);
	     break;
	  case ICLASS_STICKY_ACTIVE_NORMAL:
	     if (tc)
		tc->sticky_active.normal = ts = TextstateCreate(s2);
	     break;
	  case ICLASS_STICKY_ACTIVE_CLICKED:
	     if (tc)
		tc->sticky_active.clicked = ts = TextstateCreate(s2);
	     break;
	  case ICLASS_STICKY_ACTIVE_HILITED:
	     if (tc)
		tc->sticky_active.hilited = ts = TextstateCreate(s2);
	     break;
	  case ICLASS_STICKY_ACTIVE_DISABLED:
	     if (tc)
		tc->sticky_active.disabled = ts = TextstateCreate(s2);
	     break;
	  case TEXT_MODE:
	     if (ts)
		ts->style.mode = atoi(s2);
	     break;
	  case TEXT_EFFECT:
	     if (ts)
		ts->style.effect = atoi(s2);
	     break;
	  case TEXT_FG_COL:
	     if (ts)
	       {
		  r = g = b = 0;
		  sscanf(s, "%*s %i %i %i", &r, &g, &b);
		  SET_COLOR(&ts->fg_col, r, g, b);
	       }
	     break;
	  case TEXT_BG_COL:
	     if (ts)
	       {
		  r = g = b = 0;
		  sscanf(s, "%*s %i %i %i", &r, &g, &b);
		  SET_COLOR(&ts->bg_col, r, g, b);
	       }
	     break;
	  default:
	     ConfigParseError("TextClass", s);
	     break;
	  }
     }
   err = -1;

 done:
   return err;
}

static TextClass   *
TextclassGetFallback(void)
{
   TextClass          *tc;

   tc = TextclassFind("__fb_tc", 0);
   if (tc)
      return tc;

   /* Create fallback textclass */
   tc = TextclassCreate("__fb_tc");
   if (!tc)
      return tc;

   tc->norm.normal =
      TextstateCreate("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-*-*");
   SET_COLOR(&(tc->norm.normal->fg_col), 0, 0, 0);
   TextclassPopulate(tc);

   return tc;
}

/*
 * Textclass Module
 */

static void
TextclassIpc(const char *params)
{
   char                param1[1024];
   char                param2[1024];
   int                 l;
   const char         *p;
   TextClass          *tc;

   if (!params)
     {
	IpcPrintf("Please specify...\n");
	return;
     }

   p = params;
   l = 0;
   param1[0] = param2[0] = '\0';
   sscanf(p, "%1000s %1000s %n", param1, param2, &l);
   p += l;

   if (!strncmp(param1, "list", 2))
     {
	ECORE_LIST_FOR_EACH(tclass_list, tc) IpcPrintf("%s\n", tc->name);
	return;
     }

   if (!param1[0])
     {
	IpcPrintf("TextClass not specified\n");
	return;
     }

   if (!strcmp(param2, "create"))
     {
	/* Not implemented */
	return;
     }

   tc = TextclassFind(param1, 0);
   if (!tc)
     {
	IpcPrintf("TextClass not found: %s\n", param1);
	return;
     }

   if (!strcmp(param2, "delete"))
     {
#if ENABLE_DESTROY
	TextclassDestroy(tc);
#endif
     }
   else if (!strcmp(param2, "modify"))
     {
	/* Not implemented */
     }
   else if (!strcmp(param2, "apply"))
     {
	Window              xwin;
	Win                 win;
	char                state[20];
	int                 x, y, st;

	/* 3:xwin 4:x 5:y 6:state 7-:txt */
	xwin = None;
	x = y = 0;
	state[0] = '\0';
	l = 0;
	sscanf(p, "%lx %d %d %16s %n", &xwin, &x, &y, state, &l);
	p += l;

	if (!strcmp(state, "normal"))
	   st = STATE_NORMAL;
	else if (!strcmp(state, "hilited"))
	   st = STATE_HILITED;
	else if (!strcmp(state, "clicked"))
	   st = STATE_CLICKED;
	else if (!strcmp(state, "disabled"))
	   st = STATE_DISABLED;
	else
	   st = STATE_NORMAL;

	if (l == 0)
	   return;

	win = ECreateWinFromXwin(xwin);
	if (!win)
	   return;

	TextDraw(tc, win, None, 0, 0, st, p, x, y, 99999, 99999, 17, 0);
	EDestroyWin(win);
     }
   else if (!strcmp(param2, "query_size"))
     {
	int                 w, h;

	/* 3-:txt */

	if (l == 0)
	   return;

	w = h = 0;
	TextSize(tc, 0, 0, STATE_NORMAL, p, &w, &h, 17);
	IpcPrintf("%i %i\n", w, h);
     }
   else if (!strcmp(param2, "query"))
     {
	IpcPrintf("TextClass %s found\n", tc->name);
     }
   else if (!strcmp(param2, "ref_count"))
     {
	IpcPrintf("%u references remain\n", tc->ref_count);
     }
   else
     {
	IpcPrintf("Error: unknown operation specified\n");
     }
}

static const IpcItem TextclassIpcArray[] = {
   {
    TextclassIpc,
    "textclass", "tc",
    "List textclasses, apply a textclass",
    NULL}
   ,
};
#define N_IPC_FUNCS (sizeof(TextclassIpcArray)/sizeof(IpcItem))

/*
 * Module descriptor
 */
extern const EModule ModTextclass;
const EModule       ModTextclass = {
   "textclass", "tc",
   NULL,
   {N_IPC_FUNCS, TextclassIpcArray}
   ,
   {0, NULL}
};
