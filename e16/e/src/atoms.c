/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
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

void               *
AtomGet(Window win, Atom to_get, Atom type, int *size)
{
   unsigned char      *retval;
   Atom                type_ret;
   unsigned long       bytes_after, num_ret;
   int                 format_ret;
   long                length;
   void               *data;

   EDBUG(5, "AtomGet");

   retval = NULL;
   length = 0x7fffffff;
   XGetWindowProperty(disp, win, to_get, 0, length, False, type, &type_ret,
		      &format_ret, &num_ret, &bytes_after, &retval);
   if ((retval) && (num_ret > 0) && (format_ret > 0))
     {
	if (format_ret == 32)
	  {
	     int                 i;

	     *size = num_ret * sizeof(unsigned long);

	     data = Emalloc(*size);
	     for (i = 0; i < (int)num_ret; i++)
		((unsigned long *)data)[i] = ((unsigned long *)retval)[i];
	  }
	else if (format_ret == 16)
	  {
	     int                 i;

	     *size = num_ret * sizeof(unsigned short);

	     data = Emalloc(*size);
	     for (i = 0; i < (int)num_ret; i++)
		((unsigned short *)data)[i] = ((unsigned short *)retval)[i];
	  }
	else
	  {
	     /* format_ret == 8 */
	     *size = num_ret;
	     data = Emalloc(num_ret);
	     if (data)
		memcpy(data, retval, num_ret);
	  }
	XFree(retval);
	EDBUG_RETURN(data);
     }
   if (retval)
      XFree(retval);
   EDBUG_RETURN(NULL);

}

void
EPropWindowSet(Window win, Atom a, Window propwin)
{
   CARD32              val;

   val = propwin;
   XChangeProperty(disp, win, a, XA_WINDOW, 32, PropModeReplace,
		   (unsigned char *)&val, 1);
}

Window
EPropWindowGet(Window win, Atom a)
{
   Window              propwin;
   Atom                aa;
   int                 format_ret;
   unsigned long       bytes_after, num_ret;
   unsigned char      *puc;

   puc = NULL;
   XGetWindowProperty(disp, win, a, 0, 0x7fffffff, True,
		      XA_WINDOW, &aa, &format_ret, &num_ret,
		      &bytes_after, &puc);

   if (!puc)
      return None;

   propwin = *((Window *) puc);
   XFree(puc);

   return propwin;
}
