#include "E.h"

void
DockIt(EWin * ewin)
{
   ActionClass        *ac;
   ImageClass         *ic;
   Button             *bt;
   char                id[32];

   EDBUG(3, "DockIt");
   Esnprintf(id, sizeof(id), "%i", ewin->client.win);
   ac = 0;
   ic = FindItem("DEFAULT_DOCK_BUTTON", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (mode.dockstartx > 0)
     {
	bt = CreateButton(id, ic, ac, NULL, NULL, 1, 0, 64, 64, 64, 64, 0, 0,
			  mode.dockstartx, 0, mode.dockstarty, 0, 0, 0, 0, 0,
			  1, 0, 1);
     }
   else
     {
	bt = CreateButton(id, ic, ac, NULL, NULL, 1, 0, 64, 64, 64, 64, 0,
			  2046, 0, 0, 0, 1023, 0, 0, 0, 0, 1, 0, 1);
     }
   UngrabX();

   if (!bt)
      EDBUG_RETURN_;

   ShowButton(bt);
   FindEmptySpotForButton(bt, "DOCK_APP_BUTTON", mode.dockdirmode);

   AddItem(ewin, "EWIN", ewin->client.win, LIST_TYPE_EWIN);
   AddItem(bt, "DOCK_APP_BUTTON", ewin->client.win, LIST_TYPE_BUTTON);

   EmbedWindowIntoButton(bt, ewin->client.win);
   ShowEwin(ewin);

   EDBUG_RETURN_;
}

void
DockDestroy(EWin * ewin)
{

   Button             *bt;

   EDBUG(3, "DockDestroy");
   bt = FindItem(NULL, ewin->client.win, LIST_FINDBY_ID, LIST_TYPE_BUTTON);
   if (bt)
      DestroyButton(RemoveItem(NULL, ewin->client.win,
			       LIST_FINDBY_ID, LIST_TYPE_BUTTON));
   EDBUG_RETURN_;
}
