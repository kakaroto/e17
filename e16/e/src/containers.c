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
	bc->ButtonList = Erealloc(bc->ButtonList,
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
	bc->ButtonList = Erealloc(bc->ButtonList,
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
