/*
 Copyright (C) 2000-2003 Carsten Haitzler, Geoff Harrison and various contributors

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to
 deal in the Software without restriction, including without limitation the
 rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies of the Software, its documentation and marketing & publicity
 materials, and acknowledgment shall be given in the documentation, materials
 and software packages that this Software was used.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"

void
SetupX()
{
   EDBUG(6, "SetupX");
   /* Open a connection to the diplay nominated by the DISPLAY variable */
   /* Or set with the -display option */
   disp = XOpenDisplay(display_name);
   /* if cannot connect to display */
   if (!disp)
     {
	Alert("Eesh cannot connect to the display nominated by\n"
	      "your shell's DISPLAY environment variable. You may set this\n"
	      "variable to indicate which display name Enlightenment is to\n"
	      "connect to. It may be that you do not have an Xserver already\n"
	      "running to serve that Display connection, or that you do not\n"
	      "have permission to connect to that display. Please make sure\n"
	      "all is correct before trying again. Run an Xserver by running\n"
	      "xdm or startx first, or contact your local system\n"
	      "administrator, or Xserver vendor, or read the X, xdm and\n"
	      "startx manual pages before proceeding.\n");
	exit(1);
     }

   root.win = DefaultRootWindow(disp);
   root.scr = DefaultScreen(disp);
   root.w = DisplayWidth(disp, root.scr);
   root.h = DisplayHeight(disp, root.scr);

   /* warn, if necessary about X version problems */
   if (ProtocolVersion(disp) != 11)
     {
	Alert("WARNING:\n"
	      "This is not an X11 Xserver. It infact talks the X%i protocol.\n"
	      "This may mean Enlightenment will either not function, or\n"
	      "function incorrectly. If it is later than X11, then your\n"
	      "server is one the author(s) of Enlightenment neither have\n"
	      "access to, nor have heard of.\n", ProtocolVersion(disp));
     }
   EDBUG_RETURN_;
}
