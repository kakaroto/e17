#include "E.h"

Group              *
CreateGroup()
{
   static int          index = 0;
   Group              *g;

   g = Emalloc(sizeof(Group));
   if (!g)
      EDBUG_RETURN(NULL);

   g->index = index++;
   g->iconify = 1;
   g->kill = 0;
   g->move = 1;
   g->raise = 0;
   g->set_border = 1;
   g->stick = 1;
   g->shade = 1;
   g->ref_count = 0;

   EDBUG_RETURN(g);
}

void
BreakWindowGroup(EWin * ewin)
{

   int                 i, num;
   EWin              **gwins;

   if (ewin)
     {
	gwins = ListWinGroupMembers(ewin->group, &num);

	if (gwins)
	  {
	     for (i = 0; i < num; i++)
		RemoveEwinFromGroup(gwins[i]);

	     Efree(gwins);
	  }
     }
}

void
BuildWindowGroup(EWin ** ewins, int num)
{

   int                 i;
   Group              *g;

   g = CreateGroup();
   current_group = g;

   for (i = 0; i < num; i++)
     {
	RemoveEwinFromGroup(ewins[i]);
	g->ref_count++;
	ewins[i]->group = g;
     }
}

void
AddEwinToGroup(EWin * ewin, Group * g)
{
   if (ewin && g)
     {
	RemoveEwinFromGroup(ewin);
	ewin->group = g;
	ewin->group->ref_count++;
     }
}

void
RemoveEwinFromGroup(EWin * ewin)
{
   if (ewin)
     {
	if (ewin->group)
	  {
	     if (--(ewin->group->ref_count) <= 0)
		Efree(ewin->group);
	  }
	ewin->group = NULL;
     }
}
