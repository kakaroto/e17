#include "E.h"

void
MakeWindowUnSticky(EWin * ewin)
{

   EDBUG(5, "MakeWindowUnSticky");
   if (!ewin)
      EDBUG_RETURN_;

   FloatEwinAt(ewin, ewin->x, ewin->y);
   DrawEwinShape(ewin, 0, ewin->x, ewin->y,
		 ewin->client.w, ewin->client.h, 0);
   MoveEwinToDesktopAt(ewin, desks.current, ewin->x, ewin->y);
   ewin->sticky = 0;
   RaiseEwin(ewin);
   DrawEwin(ewin);

   if (mode.kde_support)
      KDE_UpdateClient(ewin);

   ApplySclass(FindItem("SOUND_WINDOW_UNSTICK", 0, LIST_FINDBY_NAME,
			LIST_TYPE_SCLASS));

   EDBUG_RETURN_;

}

void
MakeWindowSticky(EWin * ewin)
{
   EDBUG(5, "MakeWindowSticky");
   if (!ewin)
      EDBUG_RETURN_;
   ewin->sticky = 1;
   MoveEwinToDesktopAt(ewin, desks.current, ewin->x, ewin->y);
   RaiseEwin(ewin);
   DrawEwin(ewin);

   if (mode.kde_support)
      KDE_UpdateClient(ewin);

   ApplySclass(FindItem("SOUND_WINDOW_STICK", 0,
			LIST_FINDBY_NAME, LIST_TYPE_SCLASS));

   EDBUG_RETURN_;
}
