#include "E.h"

static char         coords_visible = 0;
static Window       c_win = 0;
static int          cx = 0, cy = 0, cw = 0, ch = 0;

void
SetCoords(int x, int y, int w, int h)
{
   TextClass          *tc = NULL;
   ImageClass         *ic = NULL;
   char                s[256], pq;
   EWin               *ewin;
   int                 md;

   if (!tc)
      tc = FindItem("COORDS", 0, LIST_FINDBY_NAME, LIST_TYPE_TCLASS);
   if (!ic)
      ic = FindItem("COORDS", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (!c_win)
      c_win = ECreateWindow(root.win, 0, 0, 1, 1, 2);
   if ((!ic) || (!tc))
      return;

   Esnprintf(s, sizeof(s), "%i x %i (%i, %i)", w, h, x, y);
   TextSize(tc, 0, 0, 0, s, &cw, &ch, 17);
   ewin = mode.ewin;
   cw += (ic->padding.left + ic->padding.right);
   ch += (ic->padding.top + ic->padding.bottom);
   if (ewin)
     {
	md = 0;
	if (mode.mode == MODE_MOVE)
	   md = mode.movemode;
	else
	   md = mode.resizemode;
	if ((md > 0) && ((cw >= (ewin->w)) || (ch >= (ewin->h))))
	  {
	     cx = 0;
	     cy = 0;
	  }
	else
	  {
	     switch (md)
	       {
	       case 0:
	       case 1:
	       case 2:
		  cx = ewin->x + ((ewin->w - cw) / 2) - desks.desk[ewin->desktop].x;
		  cy = ewin->y + ((ewin->h - ch) / 2) - desks.desk[ewin->desktop].y;
		  break;
	       case 3:
		  cx = 0;
		  cy = 0;
		  break;
	       case 4:
		  cx = 0;
		  cy = 0;
		  break;
	       case 5:
		  cx = 0;
		  cy = 0;
		  break;
	       default:
		  cx = 0;
		  cy = 0;
		  break;
	       }
	  }
     }
   else
     {
	cx = 0;
	cy = 0;
     }
   if (!coords_visible)
      EMapWindow(disp, c_win);
   XRaiseWindow(disp, c_win);
   EMoveResizeWindow(disp, c_win, cx, cy, cw, ch);
   pq = queue_up;
   queue_up = 0;
   IclassApply(ic, c_win, cw, ch, 1, 0, STATE_NORMAL, 0);
   TclassApply(ic, c_win, cw, ch, 0, 0, STATE_NORMAL, 0, tc, s);
   queue_up = pq;
   XFlush(disp);
   coords_visible = 1;
}

void
HideCoords(void)
{
   if (c_win)
      EUnmapWindow(disp, c_win);
   coords_visible = 0;
}
