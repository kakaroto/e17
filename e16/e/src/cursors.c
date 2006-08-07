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
#include "X11/cursorfont.h"
#include "conf.h"
#include "e16-ecore_list.h"
#include "emodule.h"
#include "xwin.h"

struct _ecursor
{
   char               *name;
   char               *file;
   Cursor              cursor;
   unsigned int        ref_count;
#if 0				/* Not used */
   char                inroot;
#endif
};

static Ecore_List  *cursor_list = NULL;

static ECursor     *
ECursorCreate(const char *name, const char *image, int native_id, XColor * fg,
	      XColor * bg)
{
   Cursor              curs;
   Pixmap              pmap, mask;
   int                 xh, yh;
   unsigned int        w, h, ww, hh;
   char               *img, msk[FILEPATH_LEN_MAX];
   ECursor            *ec;

   if ((!name) || (!image && native_id == -1))
      return NULL;

   if (image)
     {
	img = FindFile(image, Mode.theme.path, 0);
	if (!img)
	   return NULL;

	Esnprintf(msk, sizeof(msk), "%s.mask", img);
	pmap = 0;
	mask = 0;
	xh = 0;
	yh = 0;
	XReadBitmapFile(disp, VRoot.xwin, msk, &w, &h, &mask, &xh, &yh);
	XReadBitmapFile(disp, VRoot.xwin, img, &w, &h, &pmap, &xh, &yh);
	XQueryBestCursor(disp, VRoot.xwin, w, h, &ww, &hh);
	curs = None;
	if ((w <= ww) && (h <= hh) && (pmap))
	  {
	     EAllocColor(VRoot.cmap, fg);
	     EAllocColor(VRoot.cmap, bg);
	     if (xh < 0 || xh >= (int)w)
		xh = (int)w / 2;
	     if (yh < 0 || yh >= (int)h)
		yh = (int)h / 2;
	     curs = XCreatePixmapCursor(disp, pmap, mask, fg, bg, xh, yh);
	  }

	if (!curs)
	   Eprintf("*** Failed to create cursor \"%s\" from %s,%s\n",
		   name, img, msk);

	if (pmap)
	   EFreePixmap(pmap);
	if (mask)
	   EFreePixmap(mask);
	Efree(img);

	if (!curs)
	   return NULL;
     }
   else
     {
	curs = (native_id == 999) ? None : XCreateFontCursor(disp, native_id);
     }

   ec = Emalloc(sizeof(ECursor));
   ec->name = Estrdup(name);
   ec->file = Estrdup(image);
   ec->cursor = curs;
   ec->ref_count = 0;
#if 0				/* Not used */
   ec->inroot = 0;
#endif

   if (!cursor_list)
      cursor_list = ecore_list_new();
   ecore_list_prepend(cursor_list, ec);

   return ec;
}

static void
ECursorDestroy(ECursor * ec)
{
   if (!ec)
      return;

   if (ec->ref_count > 0)
     {
	DialogOK(_("ECursor Error!"), _("%u references remain\n"),
		 ec->ref_count);
	return;
     }

   ecore_list_remove_node(cursor_list, ec);

   if (ec->name)
      Efree(ec->name);
   if (ec->file)
      Efree(ec->file);
   Efree(ec);
}

static int
_ECursorMatchName(const void *data, const void *match)
{
   return strcmp(((const ECursor *)data)->name, match);
}

ECursor            *
ECursorFind(const char *name)
{
   return ecore_list_find(cursor_list, _ECursorMatchName, name);
}

static int
ECursorConfigLoad(FILE * fs)
{
   int                 err = 0;
   XColor              xclr, xclr2;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1, i2, r, g, b;
   char               *file = NULL, *name = NULL;
   int                 native_id = -1;
   int                 fields;

   while (GetLine(s, sizeof(s), fs))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

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
		  continue;
	       }
	  }

	switch (i1)
	  {
	  case CONFIG_CURSOR:
	     err = -1;
	     i2 = atoi(s2);
	     if (i2 != CONFIG_OPEN)
		goto done;
	     ESetColor(&xclr, 0, 0, 0);
	     ESetColor(&xclr2, 255, 255, 255);
	     _EFREE(file);
	     _EFREE(name);
	     native_id = -1;
	     break;
	  case CONFIG_CLOSE:
	     ECursorCreate(name, file, native_id, &xclr, &xclr2);
	     err = 0;
	     break;

	  case CONFIG_CLASSNAME:
	     if (ECursorFind(s2))
	       {
		  SkipTillEnd(fs);
		  goto done;
	       }
	     _EFDUP(name, s2);
	     break;
	  case CURS_BG_RGB:
	     r = g = b = 0;
	     sscanf(s, "%4000s %d %d %d", s2, &r, &g, &b);
	     ESetColor(&xclr, r, g, b);
	     break;
	  case CURS_FG_RGB:
	     r = g = b = 255;
	     sscanf(s, "%4000s %d %d %d", s2, &r, &g, &b);
	     ESetColor(&xclr2, r, g, b);
	     break;
	  case XBM_FILE:
	     _EFDUP(file, s2);
	     break;
	  case NATIVE_ID:
	     sscanf(s, "%4000s %d", s2, &native_id);
	     break;
	  default:
	     break;
	  }
     }

 done:
   if (err)
      ConfigAlertLoad(_("Cursor"));

   _EFREE(name);
   _EFREE(file);

   return err;
}

void
ECursorApply(ECursor * ec, Win win)
{
   if (!ec)
      return;
   XDefineCursor(disp, WinGetXwin(win), ec->cursor);
#if 0				/* Not used */
   if (win == VRoot.xwin)
      ec->inroot = 1;
#endif
}

static              Cursor
ECursorGetByName(const char *name, unsigned int fallback)
{
   ECursor            *ec;

   ec = ECursorFind(name);
   if (!ec)
      return XCreateFontCursor(disp, fallback);

   ECursorIncRefcount(ec);

   return ec->cursor;
}

void
ECursorIncRefcount(ECursor * ec)
{
   if (ec)
      ec->ref_count++;
}

void
ECursorDecRefcount(ECursor * ec)
{
   if (ec)
      ec->ref_count--;
}

#if 0				/* Not used */
static int
ECursorGetRefcount(ECursor * ec)
{
   return (ec) ? ec->ref_count : 0;
}
#endif

static Cursor       ECsrs[ECSR_COUNT];

Cursor
ECsrGet(int which)
{
   return (which >= 0 && which < ECSR_COUNT) ? ECsrs[which] : None;
}

void
ECsrApply(int which, Window win)
{
   XDefineCursor(disp, win, ECsrGet(which));
}

/*
 * Set up some basic cursors
 */
static void
CursorsInit(void)
{
   ECsrs[ECSR_NONE] = None;
   ECsrs[ECSR_ROOT] = ECursorGetByName("DEFAULT", XC_left_ptr);
   ECsrs[ECSR_GRAB] = ECursorGetByName("GRAB", XC_crosshair);
   ECsrs[ECSR_PGRAB] = ECursorGetByName("PGRAB", XC_X_cursor);
   ECsrs[ECSR_ACT_MOVE] = ECursorGetByName("GRAB_MOVE", XC_fleur);
   ECsrs[ECSR_ACT_RESIZE] = ECursorGetByName("GRAB_RESIZE", XC_sizing);
}

/*
 * Cursor module
 */

static void
CursorSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_INIT:
	ConfigFileLoad("cursors.cfg", Mode.theme.path, ECursorConfigLoad, 1);
	CursorsInit();
	break;
     }
}

static void
CursorsIpc(const char *params, Client * c __UNUSED__)
{
   const char         *p;
   char                cmd[128], prm[4096];
   int                 len;
   ECursor            *ec;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %4000s %n", cmd, prm, &len);
	p += len;
     }

   if (!strncmp(cmd, "add", 3))
     {
	/* TBD */
	IpcPrintf("Not implemented\n");
     }
   else if (!strncmp(cmd, "del", 3))
     {
	ECursorDestroy(ECursorFind(prm));
     }
   else if (!strncmp(cmd, "list", 2))
     {
	ECORE_LIST_FOR_EACH(cursor_list, ec) IpcPrintf("%s\n", ec->name);
     }
}

static const IpcItem CursorIpcArray[] = {
   {
    CursorsIpc,
    "cursor", "csr",
    "Cursor functions",
    "  cursor add <classname> ...        Create cursor\n"
    "  cursor del <classname>            Delete cursor\n"
    "  cursor list                       Show all cursors\n"}
};
#define N_IPC_FUNCS (sizeof(CursorIpcArray)/sizeof(IpcItem))

/*
 * Module descriptor
 */
const EModule       ModCursors = {
   "cursor", "csr",
   CursorSighan,
   {N_IPC_FUNCS, CursorIpcArray}
   ,
   {0, NULL}
};
