/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2007 Kim Woelders
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
#include "dialog.h"

static void
_DlgFillAbout(Dialog * d, DItem * table, void *data __UNUSED__)
{
   DItem              *di;
   char                buf[1024];

   Esnprintf(buf, sizeof(buf), _("About Enlightenment %s"), e_wm_version);
   DialogSetTitle(d, buf);

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_IMAGE);
   DialogItemImageSetFile(di, "pix/about.png");

   di = DialogAddItem(table, DITEM_TEXT);
   Esnprintf(buf, sizeof(buf),
	     _("Welcome to the %s version\n"
	       "of the Enlightenment window manager.\n"
	       "Enlightenment is still under development, but\n"
	       "we have tried to iron out all the bugs that\n"
	       "we can find. If you find a bug in the software,\n"
	       "please do not hesitate to send in a bug report.\n"
	       "See \"Help\" for information on joining the\n"
	       "mailing list.\n" "\n"
	       "This code was last updated on:\n%s\n" "\n"
	       "Good luck. We hope you enjoy the software.\n" "\n"
	       "The Rasterman - raster@rasterman.com\n"
	       "Mandrake - mandrake@mandrake.net\n"
	       "Kim Woelders - kim@woelders.dk\n"), e_wm_version, e_wm_date);
   DialogItemSetText(di, buf);

   DialogBindKey(d, "Return", DialogCallbackClose, 0, NULL);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0, NULL);
}

static const DialogDef DlgAbout = {
   "ABOUT_ENLIGHTENMENT",
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   _DlgFillAbout,
   DLG_OK | DLG_NO_SEPARATOR, DialogCallbackClose,
};

void
About(void)
{
   DialogShowSimple(&DlgAbout, NULL);
}
