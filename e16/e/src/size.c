
#include "E.h"

void
MaxHeight(EWin * ewin, char *resize_type)
{

   int                 x, y, w, h, y1, y2;

   if (!ewin)
      return;

   if (!ewin->toggle)
     {
	x = ewin->x;
	y = 0;
	w = ewin->client.w;
	h = root.h - ewin->border->border.top - ewin->border->border.bottom;
	if ((resize_type) && (!strcmp(resize_type, "available")))
	  {
	     EWin              **lst;
	     int                 i, num;

	     lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
	     if (lst)
	       {
		  y = ewin->y;
		  h = ewin->h;
		  y1 = 0;
		  y2 = root.h;
		  for (i = 0; i < num; i++)
		    {
		       if (((ewin->desktop == lst[i]->desktop) ||
			    (lst[i]->sticky)) &&
			   (!(lst[i]->floating)) &&
			   (lst[i] != ewin) &&
			   (!(lst[i]->ignorearrange)) &&
			 SPANS_COMMON(ewin->x, ewin->w, lst[i]->x, lst[i]->w))
			 {
			    if (((lst[i]->y + lst[i]->h) <= y) &&
				((lst[i]->y + lst[i]->h) >= y1))
			       y1 = lst[i]->y + lst[i]->h;
			    else if (((y + h) <= lst[i]->y) &&
				     (y2 >= lst[i]->y))
			       y2 = lst[i]->y;
			 }
		    }
		  Efree(lst);
		  y = y1;
		  h = y2 - y1 -
		     (ewin->border->border.top + ewin->border->border.bottom);
	       }
	  }
	else if ((resize_type) && (!strcmp(resize_type, "conservative")))
	  {
	     EWin              **lst;
	     int                 i, num;

	     lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
	     if (lst)
	       {
		  y = ewin->y;
		  h = ewin->h;
		  y1 = 0;
		  y2 = root.h;
		  for (i = 0; i < num; i++)
		    {
		       if (((ewin->desktop == lst[i]->desktop) ||
			    (lst[i]->sticky)) &&
			   (!(lst[i]->floating)) &&
			   (lst[i] != ewin) &&
			   (lst[i]->never_use_area) &&
			 SPANS_COMMON(ewin->x, ewin->w, lst[i]->x, lst[i]->w))
			 {
			    if (((lst[i]->y + lst[i]->h) <= y) &&
				((lst[i]->y + lst[i]->h) >= y1))
			       y1 = lst[i]->y + lst[i]->h;
			    else if (((y + h) <= lst[i]->y) &&
				     (y2 >= lst[i]->y))
			       y2 = lst[i]->y;
			 }
		    }
		  Efree(lst);
		  y = y1;
		  h = y2 - y1 -
		     (ewin->border->border.top + ewin->border->border.bottom);
	       }
	  }
	ewin->lx = ewin->x;
	ewin->ly = ewin->y;
	ewin->lw = ewin->client.w;
	ewin->lh = ewin->client.h;
	MoveResizeEwin(ewin, x, y, w, h);
	ewin->toggle = 1;
     }
   else
     {
	MoveResizeEwin(ewin, ewin->lx, ewin->ly, ewin->lw, ewin->lh);
	ewin->lx = ewin->x;
	ewin->ly = ewin->y;
	ewin->lw = ewin->client.w;
	ewin->lh = ewin->client.h;
	ewin->toggle = 0;
     }

   return;
}

void
MaxWidth(EWin * ewin, char *resize_type)
{

   int                 x, y, w, h, x1, x2;

   if (!ewin)
      return;

   if (!ewin->toggle)
     {
	x = 0;
	y = ewin->y;
	w = root.w - ewin->border->border.left - ewin->border->border.right;
	h = ewin->client.h;
	if ((resize_type) && (!strcmp(resize_type, "available")))
	  {
	     EWin              **lst;
	     int                 i, num;

	     lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
	     if (lst)
	       {
		  x = ewin->x;
		  w = ewin->w;
		  x1 = 0;
		  x2 = root.w;
		  for (i = 0; i < num; i++)
		    {
		       if (((ewin->desktop == lst[i]->desktop) ||
			    (lst[i]->sticky)) &&
			   (!(lst[i]->floating)) &&
			   (lst[i] != ewin) &&
			   (!(lst[i]->ignorearrange)) &&
			 SPANS_COMMON(ewin->y, ewin->h, lst[i]->y, lst[i]->h))
			 {
			    if (((lst[i]->x + lst[i]->w) <= x) &&
				((lst[i]->x + lst[i]->w) >= x1))
			       x1 = lst[i]->x + lst[i]->w;
			    else if (((x + w) <= lst[i]->x) &&
				     (x2 >= lst[i]->x))
			       x2 = lst[i]->x;
			 }
		    }
		  Efree(lst);
		  x = x1;
		  w = x2 - x1 -
		     (ewin->border->border.left + ewin->border->border.right);
	       }
	  }
	else if ((resize_type) && (!strcmp(resize_type, "conservative")))
	  {
	     EWin              **lst;
	     int                 i, num;

	     lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
	     if (lst)
	       {
		  x = ewin->x;
		  w = ewin->w;
		  x1 = 0;
		  x2 = root.w;
		  for (i = 0; i < num; i++)
		    {
		       if ((lst[i] != ewin) && (!(lst[i]->ignorearrange)) &&
			   (!(lst[i]->floating)) &&
			   (((ewin->desktop == lst[i]->desktop) &&
			     (lst[i]->fixedpos)) ||
			    (lst[i]->sticky)) &&
			 SPANS_COMMON(ewin->y, ewin->h, lst[i]->y, lst[i]->h))
			 {
			    if (((lst[i]->x + lst[i]->w) <= x) &&
				((lst[i]->x + lst[i]->w) >= x1))
			       x1 = lst[i]->x + lst[i]->w;
			    else if (((x + w) <= lst[i]->x) &&
				     (x2 >= lst[i]->x))
			       x2 = lst[i]->x;
			 }
		    }
		  Efree(lst);
		  x = x1;
		  w = x2 - x1 -
		     (ewin->border->border.left + ewin->border->border.right);
	       }
	  }
	ewin->lx = ewin->x;
	ewin->ly = ewin->y;
	ewin->lw = ewin->client.w;
	ewin->lh = ewin->client.h;
	MoveResizeEwin(ewin, x, y, w, h);
	ewin->toggle = 1;
     }
   else
     {
	MoveResizeEwin(ewin, ewin->lx, ewin->ly, ewin->lw, ewin->lh);
	ewin->lx = ewin->x;
	ewin->ly = ewin->y;
	ewin->lw = ewin->client.w;
	ewin->lh = ewin->client.h;
	ewin->toggle = 0;
     }

   return;
}

void
MaxSize(EWin * ewin, char *resize_type)
{

   int                 x, y, w, h, x1, x2, y1, y2;

   if (!ewin)
      return;

   if (!ewin->toggle)
     {
	x = 0;
	y = 0;
	w = root.w - ewin->border->border.left - ewin->border->border.right;
	h = root.h - ewin->border->border.top - ewin->border->border.bottom;
	if ((resize_type) && (!strcmp(resize_type, "available")))
	  {
	     EWin              **lst;
	     int                 i, num;

	     lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
	     if (lst)
	       {
		  y = ewin->y;
		  h = ewin->h;
		  y1 = 0;
		  y2 = root.h;
		  x = ewin->x;
		  w = ewin->w;
		  x1 = 0;
		  x2 = root.w;
		  for (i = 0; i < num; i++)
		    {
		       if (((ewin->desktop == lst[i]->desktop) ||
			    (lst[i]->sticky)) &&
			   (!(lst[i]->floating)) &&
			   (lst[i] != ewin) &&
			   (!(lst[i]->ignorearrange)) &&
			 SPANS_COMMON(ewin->x, ewin->w, lst[i]->x, lst[i]->w))
			 {
			    if (((lst[i]->y + lst[i]->h) <= y) &&
				((lst[i]->y + lst[i]->h) >= y1))
			       y1 = lst[i]->y + lst[i]->h;
			    else if (((y + h) <= lst[i]->y) &&
				     (y2 >= lst[i]->y))
			       y2 = lst[i]->y;
			 }
		    }
		  y = y1;
		  h = y2 - y1 -
		     (ewin->border->border.top + ewin->border->border.bottom);
		  for (i = 0; i < num; i++)
		    {
		       if (((ewin->desktop == lst[i]->desktop) ||
			    (lst[i]->sticky)) &&
			   (!(lst[i]->floating)) &&
			   (lst[i] != ewin) &&
			   (!(lst[i]->ignorearrange)) &&
			   SPANS_COMMON(y, h, lst[i]->y, lst[i]->h))
			 {
			    if (((lst[i]->x + lst[i]->w) <= x) &&
				((lst[i]->x + lst[i]->w) >= x1))
			       x1 = lst[i]->x + lst[i]->w;
			    else if (((x + w) <= lst[i]->x) &&
				     (x2 >= lst[i]->x))
			       x2 = lst[i]->x;
			 }
		    }
		  x = x1;
		  w = x2 - x1 -
		     (ewin->border->border.left + ewin->border->border.right);
		  Efree(lst);
	       }
	  }
	else if ((resize_type) && (!strcmp(resize_type, "conservative")))
	  {
	     EWin              **lst;
	     int                 i, num;

	     lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
	     if (lst)
	       {
		  y = ewin->y;
		  h = ewin->h;
		  y1 = 0;
		  y2 = root.h;
		  x = ewin->x;
		  w = ewin->w;
		  x1 = 0;
		  x2 = root.w;
		  for (i = 0; i < num; i++)
		    {
		       if ((lst[i] != ewin) && (!(lst[i]->ignorearrange)) &&
			   (!(lst[i]->floating)) &&
			   (((ewin->desktop == lst[i]->desktop) &&
			     (lst[i]->fixedpos)) ||
			    (lst[i]->sticky)) &&
			 SPANS_COMMON(ewin->x, ewin->w, lst[i]->x, lst[i]->w))
			 {
			    if (((lst[i]->y + lst[i]->h) <= y) &&
				((lst[i]->y + lst[i]->h) >= y1))
			       y1 = lst[i]->y + lst[i]->h;
			    else if (((y + h) <= lst[i]->y) &&
				     (y2 >= lst[i]->y))
			       y2 = lst[i]->y;
			 }
		    }
		  y = y1;
		  h = y2 - y1 -
		     (ewin->border->border.top + ewin->border->border.bottom);
		  for (i = 0; i < num; i++)
		    {
		       if ((lst[i] != ewin) && (!(lst[i]->ignorearrange)) &&
			   (!(lst[i]->floating)) &&
			   (((ewin->desktop == lst[i]->desktop) &&
			     (lst[i]->fixedpos)) ||
			    (lst[i]->sticky)) &&
			   SPANS_COMMON(y, h, lst[i]->y, lst[i]->h))
			 {
			    if (((lst[i]->x + lst[i]->w) <= x) &&
				((lst[i]->x + lst[i]->w) >= x1))
			       x1 = lst[i]->x + lst[i]->w;
			    else if (((x + w) <= lst[i]->x) &&
				     (x2 >= lst[i]->x))
			       x2 = lst[i]->x;
			 }
		    }
		  x = x1;
		  w = x2 - x1 -
		     (ewin->border->border.left + ewin->border->border.right);
		  Efree(lst);
	       }
	  }
	ewin->lx = ewin->x;
	ewin->ly = ewin->y;
	ewin->lw = ewin->client.w;
	ewin->lh = ewin->client.h;
	MoveResizeEwin(ewin, x, y, w, h);
	ewin->toggle = 1;
     }
   else
     {
	MoveResizeEwin(ewin, ewin->lx, ewin->ly, ewin->lw, ewin->lh);
	ewin->lx = ewin->x;
	ewin->ly = ewin->y;
	ewin->lw = ewin->client.w;
	ewin->lh = ewin->client.h;
	ewin->toggle = 0;
     }

   return;
}
