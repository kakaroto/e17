/*
 * Copyright (C) 2000-2003 Carsten Haitzler, Geoff Harrison and various contributors
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

Strip              *
CreateStrip(char *name)
{
   EDBUG(5, "CreateStrip");
   name = NULL;
   EDBUG_RETURN(NULL);
}

void
DestroyStrip(Strip * s)
{
   EDBUG(5, "DestroyStrip");
   s = NULL;
   EDBUG_RETURN_;
}

void
RotateStripTo(Strip * s, char rot)
{
   EDBUG(3, "RotateStripTo");
   s = NULL;
   rot = 0;
   EDBUG_RETURN_;
}

void
MoveStripTo(Strip * s, int x, int y)
{
   EDBUG(3, "MoveStripTo");
   s = NULL;
   x = 0;
   y = 0;
   EDBUG_RETURN_;
}

void
ShowStrip(Strip * s)
{
   EDBUG(3, "ShowStrip");
   s = NULL;
   EDBUG_RETURN_;
}

void
HideStrip(Strip * s)
{
   EDBUG(3, "HideStrip");
   s = NULL;
   EDBUG_RETURN_;
}

void
RemoveButtonFromStrip(Strip * s, Button * b)
{
   EDBUG(3, "RemoveButtonFromStrip");
   s = NULL;
   b = NULL;
   EDBUG_RETURN_;
}

void
AddButtonToStrip(Strip * s, Button * b, int x, int y)
{
   EDBUG(3, "AddButtonToStrip");
   s = NULL;
   b = NULL;
   x = 0;
   y = 0;
   EDBUG_RETURN_;
}

void
RepackStrip(Strip * s)
{
   EDBUG(4, "RepackStrip");
   s = NULL;
   EDBUG_RETURN_;
}

Strip              *
IsInStrip(int x, int y)
{
   EDBUG(4, "IsInStrip");
   x = 0;
   y = 0;
   EDBUG_RETURN(NULL);
}
