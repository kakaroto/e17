#include "E.h"

static int          calls = 0;

Clone              *
CloneEwin(EWin * ewin)
{
   Clone              *c;
   Pixmap              pmap;
   static GC           gc = 0;
   XGCValues           gcv;
   XSetWindowAttributes attr;

   if (!gc)
     {
	gcv.subwindow_mode = IncludeInferiors;
	gc = XCreateGC(disp, root.win, GCSubwindowMode, &gcv);
     }
   c = Emalloc(sizeof(Clone));
   c->name = NULL;
   c->ewin = ewin;
   attr.backing_store = NotUseful;
   attr.override_redirect = True;
   attr.colormap = root.cmap;
   attr.border_pixel = 0;
   attr.background_pixel = 0;
   attr.background_pixmap = None;
   attr.save_under = False;
   c->win = XCreateWindow(disp, ewin->parent, ewin->x, ewin->y,
			  ewin->w, ewin->h, 0, root.depth,
			  InputOutput, root.vis,
			  CWOverrideRedirect | CWSaveUnder |
			  CWBackingStore | CWColormap |
			  CWBackPixmap | CWBackPixel |
			  CWBorderPixel,
			  &attr);
   pmap = XCreatePixmap(disp, c->win, ewin->w, ewin->h, root.depth);
   XSetWindowBackgroundPixmap(disp, c->win, pmap);
   XCopyArea(disp, ewin->win, pmap, gc, 0, 0, ewin->w, ewin->h, 0, 0);
   XFreePixmap(disp, pmap);
   XShapeCombineShape(disp, c->win, ShapeBounding, 0, 0, ewin->win,
		      ShapeBounding, ShapeSet);
   return c;
}

void
FreeClone(Clone * c)
{
   XDestroyWindow(disp, c->win);
   Efree(c);
}

void
RemoveClones(void)
{
   Clone              *c;

   calls--;
   if (calls <= 0)
     {
	while ((c = RemoveItem("CLONE", 0, LIST_FINDBY_NAME, LIST_TYPE_CLONE)))
	   FreeClone(c);
	calls = 0;
     }
}

void
CloneDesktop(int d)
{
   int                 i, num;
   Clone             **clist = NULL;

   if (calls > 0)
      return;
   calls++;
   for (i = 0; i < desks.desk[d].num; i++)
     {
	EWin               *ewin;
	Clone              *c;

	ewin = desks.desk[d].list[i];
	if ((ewin) && (ewin->sticky) && (!ewin->iconified))
	  {
	     if (!FindItem("CLONE", ewin->client.win, LIST_FINDBY_ID,
			   LIST_TYPE_CLONE))
	       {
		  c = CloneEwin(ewin);
		  AddItem(c, "CLONE", ewin->client.win, LIST_TYPE_CLONE);
	       }
	  }
     }
   clist = (Clone **) ListItemType(&num, LIST_TYPE_CLONE);
   if (clist)
     {
	Window             *wl;
	int                 k, j = 0;

	wl = Emalloc(sizeof(Window) * (desks.desk[d].num + num));
	if (wl)
	  {
	     for (i = 0; i < desks.desk[d].num; i++)
	       {
		  for (k = 0; k < num; k++)
		    {
		       if (clist[k]->ewin == desks.desk[d].list[i])
			  wl[j++] = clist[k]->win;
		    }
		  wl[j++] = desks.desk[d].list[i]->win;
	       }
	     XRestackWindows(disp, wl, j);
	     Efree(wl);
	  }
	for (i = 0; i < num; i++)
	  {
	     XMapWindow(disp, clist[i]->win);
	  }
	Efree(clist);
     }
}
