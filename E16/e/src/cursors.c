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
#include "X11/cursorfont.h"
#include "conf.h"
#include "cursors.h"
#include "e16-ecore_list.h"
#include "emodule.h"
#include "xwin.h"

struct _ecursor {
   char               *name;
   char               *file;
   Cursor              cursor;
   unsigned int        ref_count;
};

static Ecore_List  *cursor_list = NULL;

static ECursor     *
ECursorCreate(const char *name, const char *image, int native_id, EColor * fg,
	      EColor * bg)
{
   Cursor              curs;
   Pixmap              pmap, mask;
   XColor              fgxc, bgxc;
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
	XReadBitmapFile(disp, WinGetXwin(VROOT), msk, &w, &h, &mask, &xh, &yh);
	XReadBitmapFile(disp, WinGetXwin(VROOT), img, &w, &h, &pmap, &xh, &yh);
	XQueryBestCursor(disp, WinGetXwin(VROOT), w, h, &ww, &hh);
	curs = None;
	if ((w <= ww) && (h <= hh) && (pmap))
	  {
	     EAllocXColor(WinGetCmap(VROOT), &fgxc, fg);
	     EAllocXColor(WinGetCmap(VROOT), &bgxc, bg);
	     if (xh < 0 || xh >= (int)w)
		xh = (int)w / 2;
	     if (yh < 0 || yh >= (int)h)
		yh = (int)h / 2;
	     curs = XCreatePixmapCursor(disp, pmap, mask, &fgxc, &bgxc, xh, yh);
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

   ec = EMALLOC(ECursor, 1);
   ec->name = Estrdup(name);
   ec->file = Estrdup(image);
   ec->cursor = curs;
   ec->ref_count = 0;

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
	DialogOK("ECursor Error!", _("%u references remain\n"), ec->ref_count);
	return;
     }

   ecore_list_node_remove(cursor_list, ec);

   Efree(ec->name);
   Efree(ec->file);

   Efree(ec);
}

static int
_ECursorMatchName(const void *data, const void *match)
{
   return strcmp(((const ECursor *)data)->name, (const char *)match);
}

static ECursor     *
ECursorFind(const char *name)
{
   if (!name || !name[0])
      return NULL;
   return (ECursor *) ecore_list_find(cursor_list, _ECursorMatchName, name);
}

ECursor            *
ECursorAlloc(const char *name)
{
   ECursor            *ec;

   if (!name)
      return NULL;

   ec = ECursorFind(name);
   if (ec)
      ec->ref_count++;

   return ec;
}

void
ECursorFree(ECursor * ec)
{
   if (ec)
      ec->ref_count--;
}

static int
ECursorConfigLoad(FILE * fs)
{
   int                 err = 0;
   EColor              clr, clr2;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   char               *p2;
   int                 i1, i2, r, g, b;
   char                name[FILEPATH_LEN_MAX], *pname;
   char                file[FILEPATH_LEN_MAX], *pfile;
   int                 native_id = -1;

   pname = pfile = NULL;

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = ConfigParseline1(s, s2, &p2, NULL);
	switch (i1)
	  {
	  case CONFIG_CURSOR:
	     err = -1;
	     i2 = atoi(s2);
	     if (i2 != CONFIG_OPEN)
		goto done;
	     SET_COLOR(&clr, 0, 0, 0);
	     SET_COLOR(&clr2, 255, 255, 255);
	     pname = pfile = NULL;
	     native_id = -1;
	     break;
	  case CONFIG_CLOSE:
	     ECursorCreate(pname, pfile, native_id, &clr, &clr2);
	     err = 0;
	     break;

	  case CONFIG_CLASSNAME:
	     if (ECursorFind(s2))
	       {
		  SkipTillEnd(fs);
		  goto done;
	       }
	     strcpy(name, s2);
	     pname = name;
	     break;
	  case CURS_BG_RGB:
	     r = g = b = 0;
	     sscanf(p2, "%d %d %d", &r, &g, &b);
	     SET_COLOR(&clr, r, g, b);
	     break;
	  case CURS_FG_RGB:
	     r = g = b = 255;
	     sscanf(p2, "%d %d %d", &r, &g, &b);
	     SET_COLOR(&clr2, r, g, b);
	     break;
	  case XBM_FILE:
	     strcpy(file, s2);
	     pfile = file;
	     break;
	  case NATIVE_ID:
	     native_id = atoi(s2);
	     break;
	  default:
	     break;
	  }
     }

 done:
   if (err)
      ConfigAlertLoad("Cursor");

   return err;
}

void
ECursorApply(ECursor * ec, Win win)
{
   if (!ec)
      return;
   XDefineCursor(disp, WinGetXwin(win), ec->cursor);
}

static              Cursor
ECursorGetByName(const char *name, const char *name2, unsigned int fallback)
{
   ECursor            *ec;

   ec = ECursorAlloc(name);
   if (!ec && name2)
      ec = ECursorAlloc(name2);
   if (ec)
      return ec->cursor;

   return XCreateFontCursor(disp, fallback);
}

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
   ECsrs[ECSR_ROOT] = ECursorGetByName("DEFAULT", NULL, XC_left_ptr);
   ECsrs[ECSR_GRAB] = ECursorGetByName("GRAB", NULL, XC_crosshair);
   ECsrs[ECSR_PGRAB] = ECursorGetByName("PGRAB", NULL, XC_X_cursor);
   ECsrs[ECSR_ACT_MOVE] = ECursorGetByName("GRAB_MOVE", NULL, XC_fleur);
   ECsrs[ECSR_ACT_RESIZE] = ECursorGetByName("GRAB_RESIZE", NULL, XC_sizing);
   ECsrs[ECSR_ACT_RESIZE_H] =
      ECursorGetByName("RESIZE_H", NULL, XC_sb_h_double_arrow);
   ECsrs[ECSR_ACT_RESIZE_V] =
      ECursorGetByName("RESIZE_V", NULL, XC_sb_v_double_arrow);
   ECsrs[ECSR_ACT_RESIZE_TL] =
      ECursorGetByName("RESIZE_TL", "RESIZE_BR", XC_top_left_corner);
   ECsrs[ECSR_ACT_RESIZE_TR] =
      ECursorGetByName("RESIZE_TR", "RESIZE_BL", XC_top_right_corner);
   ECsrs[ECSR_ACT_RESIZE_BL] =
      ECursorGetByName("RESIZE_BL", "RESIZE_TR", XC_bottom_left_corner);
   ECsrs[ECSR_ACT_RESIZE_BR] =
      ECursorGetByName("RESIZE_BR", "RESIZE_TL", XC_bottom_right_corner);
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
CursorsIpc(const char *params)
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
extern const EModule ModCursors;
const EModule       ModCursors = {
   "cursor", "csr",
   CursorSighan,
   {N_IPC_FUNCS, CursorIpcArray}
   ,
   {0, NULL}
};
