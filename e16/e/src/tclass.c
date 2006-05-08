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
#include "conf.h"
#include "e16-ecore_list.h"
#include "emodule.h"
#include "iclass.h"
#include "tclass.h"
#include "xwin.h"

static Ecore_List  *tclass_list = NULL;

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
TextstateCreate(void)
{
   TextState          *ts;

   ts = Ecalloc(1, sizeof(TextState));
   if (!ts)
      return NULL;

   ts->fontname = NULL;
   ts->style.mode = MODE_WRAP_CHAR;
   ts->style.orientation = FONT_TO_RIGHT;
   ts->efont = NULL;
   ts->xfont = NULL;
   ts->xfontset = 0;

   return ts;
}

static void
TextStateDestroy(TextState * ts)
{
   if (ts->fontname)
      Efree(ts->fontname);
   if (ts->xfont)
      XFreeFont(disp, ts->xfont);
   if (ts->efont)
      Efont_free(ts->efont);
   Efree(ts);
}

static TextClass   *
TextclassCreate(const char *name)
{
   TextClass          *tc;

   tc = Ecalloc(1, sizeof(TextClass));
   if (!tc)
      return NULL;

   if (!tclass_list)
      tclass_list = ecore_list_new();
   ecore_list_prepend(tclass_list, tc);

   tc->name = Estrdup(name);
   tc->justification = 512;

   return tc;
}

static void
TextclassDestroy(TextClass * tc)
{
   if (tc->ref_count > 0)
     {
	DialogOK(_("Textclass Error!"), _("%u references remain\n"),
		 tc->ref_count);
	return;
     }
   if (tc->name)
      Efree(tc->name);
   if (tc->norm.normal)
      TextStateDestroy(tc->norm.normal);
   if (tc->norm.hilited)
      TextStateDestroy(tc->norm.hilited);
   if (tc->norm.clicked)
      TextStateDestroy(tc->norm.clicked);
   if (tc->norm.disabled)
      TextStateDestroy(tc->norm.disabled);
   if (tc->active.normal)
      TextStateDestroy(tc->active.normal);
   if (tc->active.hilited)
      TextStateDestroy(tc->active.hilited);
   if (tc->active.clicked)
      TextStateDestroy(tc->active.clicked);
   if (tc->active.disabled)
      TextStateDestroy(tc->active.disabled);
   if (tc->sticky.normal)
      TextStateDestroy(tc->sticky.normal);
   if (tc->sticky.hilited)
      TextStateDestroy(tc->sticky.hilited);
   if (tc->sticky.clicked)
      TextStateDestroy(tc->sticky.clicked);
   if (tc->sticky.disabled)
      TextStateDestroy(tc->sticky.disabled);
   if (tc->sticky_active.normal)
      TextStateDestroy(tc->sticky_active.normal);
   if (tc->sticky_active.hilited)
      TextStateDestroy(tc->sticky_active.hilited);
   if (tc->sticky_active.clicked)
      TextStateDestroy(tc->sticky_active.clicked);
   if (tc->sticky_active.disabled)
      TextStateDestroy(tc->sticky_active.disabled);
   Efree(tc);
}

void
TextclassIncRefcount(TextClass * tc)
{
   tc->ref_count++;
}

void
TextclassDecRefcount(TextClass * tc)
{
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
   return strcmp(((const TextClass *)data)->name, match);
}

TextClass          *
TextclassFind(const char *name, int fallback)
{
   TextClass          *tc;

   if (name)
     {
	tc = ecore_list_find(tclass_list, _TextclassMatchName, name);
	if (tc || !fallback)
	   return tc;
     }

   tc = ecore_list_find(tclass_list, _TextclassMatchName, "__FALLBACK_TCLASS");

   return tc;
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
   int                 fields;

   while (GetLine(s, sizeof(s), fs))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000[^=]", &i1, s2);

	if (fields < 1)
	  {
	     i1 = CONFIG_INVALID;
	  }
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
		  i1 = CONFIG_INVALID;
	       }
	  }
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
		tc->norm.normal = ts = TextstateCreate();
	     if (ts)
	       {
		  ts->fontname = TextstateFontLookup(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_CLICKED:
	     if (tc)
		tc->norm.clicked = ts = TextstateCreate();
	     if (ts)
	       {
		  ts->fontname = TextstateFontLookup(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_HILITED:
	     if (tc)
		tc->norm.hilited = ts = TextstateCreate();
	     if (ts)
	       {
		  ts->fontname = TextstateFontLookup(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_DISABLED:
	     if (tc)
		tc->norm.disabled = ts = TextstateCreate();
	     if (ts)
	       {
		  ts->fontname = TextstateFontLookup(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_STICKY_NORMAL:
	     if (tc)
		tc->sticky.normal = ts = TextstateCreate();
	     if (ts)
	       {
		  ts->fontname = TextstateFontLookup(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_STICKY_CLICKED:
	     if (tc)
		tc->sticky.clicked = ts = TextstateCreate();
	     if (ts)
	       {
		  ts->fontname = TextstateFontLookup(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_STICKY_HILITED:
	     if (tc)
		tc->sticky.hilited = ts = TextstateCreate();
	     if (ts)
	       {
		  ts->fontname = TextstateFontLookup(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_STICKY_DISABLED:
	     if (tc)
		tc->sticky.disabled = ts = TextstateCreate();
	     if (ts)
	       {
		  ts->fontname = TextstateFontLookup(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_ACTIVE_NORMAL:
	     if (tc)
		tc->active.normal = ts = TextstateCreate();
	     if (ts)
	       {
		  ts->fontname = TextstateFontLookup(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_ACTIVE_CLICKED:
	     if (tc)
		tc->active.clicked = ts = TextstateCreate();
	     if (ts)
	       {
		  ts->fontname = TextstateFontLookup(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_ACTIVE_HILITED:
	     if (tc)
		tc->active.hilited = ts = TextstateCreate();
	     if (ts)
	       {
		  ts->fontname = TextstateFontLookup(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_ACTIVE_DISABLED:
	     if (tc)
		tc->active.disabled = ts = TextstateCreate();
	     if (ts)
	       {
		  ts->fontname = TextstateFontLookup(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_STICKY_ACTIVE_NORMAL:
	     if (tc)
		tc->sticky_active.normal = ts = TextstateCreate();
	     if (ts)
	       {
		  ts->fontname = TextstateFontLookup(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_STICKY_ACTIVE_CLICKED:
	     if (tc)
		tc->sticky_active.clicked = ts = TextstateCreate();
	     if (ts)
	       {
		  ts->fontname = TextstateFontLookup(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_STICKY_ACTIVE_HILITED:
	     if (tc)
		tc->sticky_active.hilited = ts = TextstateCreate();
	     if (ts)
	       {
		  ts->fontname = TextstateFontLookup(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_STICKY_ACTIVE_DISABLED:
	     if (tc)
		tc->sticky_active.disabled = ts = TextstateCreate();
	     if (ts)
	       {
		  ts->fontname = TextstateFontLookup(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case TEXT_MODE:
	     if (ts)
		ts->style.mode = atoi(s2);
	     break;
	  case TEXT_EFFECT:
	     if (ts)
		ts->effect = atoi(s2);
	     break;
	  case TEXT_FG_COL:
	     if (ts)
	       {
		  r = g = b = 0;
		  sscanf(s, "%*s %i %i %i", &r, &g, &b);
		  ESetColor(&ts->fg_col, r, g, b);
	       }
	     break;
	  case TEXT_BG_COL:
	     if (ts)
	       {
		  r = g = b = 0;
		  sscanf(s, "%*s %i %i %i", &r, &g, &b);
		  ESetColor(&ts->bg_col, r, g, b);
	       }
	     break;
	  default:
	     Alert(_("Warning: unable to determine what to do with\n"
		     "the following text in the middle of current Text"
		     " definition:\n" "%s\nWill ignore and continue...\n"), s);
	  }

     }
   err = -1;

 done:
   return err;
}

/*
 * Textclass Module
 */

static void
TextclassSighan(int sig, void *prm __UNUSED__)
{
   TextClass          *tc;

   switch (sig)
     {
     case ESIGNAL_INIT:
	/* create a fallback textclass in case no textclass is found */
	tc = TextclassCreate("__FALLBACK_TCLASS");
	tc->norm.normal = TextstateCreate();
	tc->norm.normal->fontname =
	   Estrdup("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-*-*");
	ESetColor(&(tc->norm.normal->fg_col), 0, 0, 0);
	break;
     }
}

static void
TextclassIpc(const char *params, Client * c __UNUSED__)
{
   char                param1[FILEPATH_LEN_MAX];
   char                param2[FILEPATH_LEN_MAX];
   char                param3[FILEPATH_LEN_MAX];
   TextClass          *tc;

   if (!params)
     {
	IpcPrintf("Please specify...\n");
	return;
     }

   param1[0] = 0;
   param2[0] = 0;
   param3[0] = 0;

   word(params, 1, param1);
   word(params, 2, param2);

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

   tc = TextclassFind(param1, 0);
   if (!tc)
     {
	IpcPrintf("TextClass not found: %s\n", param1);
	return;
     }

   if (!strcmp(param2, "create"))
     {
     }
   else if (!strcmp(param2, "delete"))
     {
	if (tc)
	   TextclassDestroy(tc);
     }
   else if (!strcmp(param2, "modify"))
     {
     }
   else if (!strcmp(param2, "apply"))
     {
	if (tc)
	  {
	     Window              xwin;
	     Win                 win;
	     int                 state;
	     int                 x, y;
	     const char         *txt;

	     word(params, 3, param3);
	     xwin = (Window) strtoul(param3, NULL, 0);

	     word(params, 4, param3);
	     x = atoi(param3);
	     word(params, 5, param3);
	     y = atoi(param3);

	     word(params, 6, param3);
	     state = STATE_NORMAL;
	     if (!strcmp(param3, "normal"))
		state = STATE_NORMAL;
	     else if (!strcmp(param3, "hilited"))
		state = STATE_HILITED;
	     else if (!strcmp(param3, "clicked"))
		state = STATE_CLICKED;
	     else if (!strcmp(param3, "disabled"))
		state = STATE_DISABLED;

	     txt = atword(params, 7);
	     if (!txt)
		return;

	     win = ECreateWinFromXwin(xwin);
	     if (!win)
		return;

	     TextDraw(tc, win, None, 0, 0, state, txt, x, y, 99999, 99999, 17,
		      0);
	     EDestroyWin(win);
	  }
     }
   else if (!strcmp(param2, "query_size"))
     {
	if (tc)
	  {
	     int                 w, h;
	     const char         *txt;

	     w = h = 0;
	     txt = atword(params, 3);
	     if (txt)
		TextSize(tc, 0, 0, STATE_NORMAL, txt, &w, &h, 17);
	     IpcPrintf("%i %i\n", w, h);
	  }
     }
   else if (!strcmp(param2, "query"))
     {
	if (tc)
	   IpcPrintf("TextClass %s found\n", tc->name);
     }
   else if (!strcmp(param2, "ref_count"))
     {
	if (tc)
	   IpcPrintf("%u references remain.\n", tc->ref_count);
     }
   else
     {
	IpcPrintf("Error: Unknown operation specified\n");
     }
}

static const IpcItem TextclassIpcArray[] = {
   {
    TextclassIpc,
    "textclass", NULL,
    "List textclasses, create/delete/modify/apply a textclass",
    NULL}
   ,
};
#define N_IPC_FUNCS (sizeof(TextclassIpcArray)/sizeof(IpcItem))

/*
 * Module descriptor
 */
const EModule       ModTextclass = {
   "textclass", "tc",
   TextclassSighan,
   {N_IPC_FUNCS, TextclassIpcArray}
   ,
   {0, NULL}
};
