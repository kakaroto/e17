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

/*
 * void
 * EmbedContainerInEwin(Container * MyContainer)
 * {
 * 
 * }
 */

Container          *
InitializeContainer(char *name, ImageClass * iclass, int width,
		    int height, char orientation)
{
   Container          *bc;

   EDBUG(5, "InitializeContainer");
   bc = Emalloc(sizeof(Container));

   bc->name = duplicate(name);
   bc->iclass = iclass;
   if (bc->iclass)
      bc->iclass->ref_count++;
   bc->state = 0;
   bc->expose = 0;
   bc->w = width;
   bc->h = height;
   bc->orientation = orientation;
   bc->ButtonList = NULL;
   bc->numofbuttonsinlist = 0;

   bc->win = ECreateWindow(root.win, -100, -100, width, height, 0);

   EDBUG_RETURN(bc);

}

void
AddButtonToContainer(Container * bc, Button * b)
{
   EDBUG(5, "AddButtonToContainer");

   if (!bc->ButtonList)
     {
	bc->ButtonList = Emalloc(sizeof(Container *));
     }
   else
     {
	bc->ButtonList =
	   Erealloc(bc->ButtonList,
		    (bc->numofbuttonsinlist + 1) * sizeof(Container *));
     }
   bc->ButtonList[bc->numofbuttonsinlist++] = b;
   b->ref_count++;

   EDBUG_RETURN_;
}

void
RemoveButtonFromContainer(Container * bc, Button * b)
{

   int                 i, j, found;
   Button            **MyButtonList;

   EDBUG(5, "RemoveButtonFromContainer");

   MyButtonList = Emalloc(bc->numofbuttonsinlist * sizeof(Container *));
   j = 0;
   found = 0;
   for (i = 0; i < bc->numofbuttonsinlist; i++)
      MyButtonList[i] = bc->ButtonList[i];

   for (i = 0; i < bc->numofbuttonsinlist; i++)
     {
	if (bc->ButtonList[i] == b)
	  {
	     j = i;
	     found = 1;
	  }
     }
   if (found && ((bc->numofbuttonsinlist - 1) > 0))
     {
	bc->ButtonList =
	   Erealloc(bc->ButtonList,
		    (bc->numofbuttonsinlist * sizeof(Container *)));
	for (i = 0; i <= bc->numofbuttonsinlist; i++)
	  {
	     if (i != j)
		bc->ButtonList[j++] = MyButtonList[i];
	  }
	bc->numofbuttonsinlist--;
     }
   else if (found && (bc->numofbuttonsinlist - 1) == 0)
     {
	Efree(bc->ButtonList);
	bc->ButtonList = NULL;
	bc->numofbuttonsinlist = 0;
     }
   Efree(MyButtonList);
   b->ref_count--;
   EDBUG_RETURN_;
}

void
RemoveContainer(Container * bc)
{
   EDBUG(5, "RemoveContainer");
   if (bc)
     {
	if (bc->name)
	   Efree(bc->name);
	Efree(bc);
     }
   EDBUG_RETURN_;
}
