
/*
 * Copyright (C) 2000 Carsten Haitzler, Geoff Harrison and various contributors
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
#if ENABLE_KDE
		  if (mode.kde_support)
		    {
		       y1 = mode.kde_y1;
		       y2 = mode.kde_y2;
		    }
		  else
		    {
#endif
		       y1 = 0;
		       y2 = root.h;
#if ENABLE_KDE
		    }
#endif
#ifdef HAS_XINERAMA
		  if (xinerama_active)
		    {
		       XineramaScreenInfo *screens;
		       int                 num_screens;

		       screens = XineramaQueryScreens(disp, &num_screens);
		       for (i = 0; i < num_screens; i++)
			 {
			    if (ewin->x >= screens[i].x_org)
			      {
				 if (ewin->x <=
				     (screens[i].width + screens[i].x_org))
				   {
				      if (ewin->y >= screens[i].y_org)
					{
					   if (ewin->y <=
					       (screens[i].height +
						screens[i].y_org))
					     {
						if (screens[i].y_org > y1)
						  {
						     y1 = screens[i].y_org;
						  }
						if (screens[i].y_org +
						    screens[i].height < y2)
						  {
						     y2 = screens[i].y_org +
							screens[i].height;
						  }
					     }
					}
				   }
			      }
			 }
		    }
#endif
		  for (i = 0; i < num; i++)
		    {
		       if ((((ewin->desktop == lst[i]->desktop)
			     && !(lst[i]->iconified)) || (lst[i]->sticky))
			   && (!(lst[i]->floating)) && (lst[i] != ewin)
			   && (!(lst[i]->ignorearrange))
			   && SPANS_COMMON(ewin->x, ewin->w, lst[i]->x,
					   lst[i]->w))
			 {
			    if (((lst[i]->y + lst[i]->h) <= y)
				&& ((lst[i]->y + lst[i]->h) >= y1))
			       y1 = lst[i]->y + lst[i]->h;
			    else if (((y + h) <= lst[i]->y)
				     && (y2 >= lst[i]->y))
			       y2 = lst[i]->y;
			 }
		    }
		  Efree(lst);
		  y = y1;
		  h = y2 - y1 - (ewin->border->border.top +
				 ewin->border->border.bottom);
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
#if ENABLE_KDE
		  if (mode.kde_support)
		    {
		       y1 = mode.kde_y1;
		       y2 = mode.kde_y2;
		    }
		  else
		    {
#endif
		       y1 = 0;
		       y2 = root.h;
#if ENABLE_KDE
		    }
#endif
#ifdef HAS_XINERAMA
		  if (xinerama_active)
		    {
		       XineramaScreenInfo *screens;
		       int                 num_screens;

		       screens = XineramaQueryScreens(disp, &num_screens);
		       for (i = 0; i < num_screens; i++)
			 {
			    if (ewin->x >= screens[i].x_org)
			      {
				 if (ewin->x <=
				     (screens[i].width + screens[i].x_org))
				   {
				      if (ewin->y >= screens[i].y_org)
					{
					   if (ewin->y <=
					       (screens[i].height +
						screens[i].y_org))
					     {
						if (screens[i].y_org > y1)
						  {
						     y1 = screens[i].y_org;
						  }
						if (screens[i].y_org +
						    screens[i].height < y2)
						  {
						     y2 = screens[i].y_org +
							screens[i].height;
						  }
					     }
					}
				   }
			      }
			 }
		    }
#endif
		  for (i = 0; i < num; i++)
		    {
		       if ((((ewin->desktop == lst[i]->desktop)
			     && !(lst[i]->iconified)) || (lst[i]->sticky))
			   && (!(lst[i]->floating)) && (lst[i] != ewin)
			   && (lst[i]->never_use_area)
			   && SPANS_COMMON(ewin->x, ewin->w, lst[i]->x,
					   lst[i]->w))
			 {
			    if (((lst[i]->y + lst[i]->h) <= y)
				&& ((lst[i]->y + lst[i]->h) >= y1))
			       y1 = lst[i]->y + lst[i]->h;
			    else if (((y + h) <= lst[i]->y)
				     && (y2 >= lst[i]->y))
			       y2 = lst[i]->y;
			 }
		    }
		  Efree(lst);
		  y = y1;
		  h = y2 - y1 - (ewin->border->border.top +
				 ewin->border->border.bottom);
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
#if ENABLE_KDE
		  if (mode.kde_support)
		    {
		       x1 = mode.kde_x1;
		       x2 = mode.kde_x2;
		    }
		  else
		    {
#endif
		       x1 = 0;
		       x2 = root.w;
#if ENABLE_KDE
		    }
#endif
#ifdef HAS_XINERAMA
		  if (xinerama_active)
		    {
		       XineramaScreenInfo *screens;
		       int                 num_screens;

		       screens = XineramaQueryScreens(disp, &num_screens);
		       for (i = 0; i < num_screens; i++)
			 {
			    if (ewin->x >= screens[i].x_org)
			      {
				 if (ewin->x <=
				     (screens[i].width + screens[i].x_org))
				   {
				      if (ewin->y >= screens[i].y_org)
					{
					   if (ewin->y <=
					       (screens[i].height +
						screens[i].y_org))
					     {
						if (screens[i].x_org > x1)
						  {
						     x1 = screens[i].x_org;
						  }
						if (screens[i].x_org +
						    screens[i].width < x2)
						  {
						     x2 = screens[i].x_org +
							screens[i].width;
						  }
					     }
					}
				   }
			      }
			 }
		    }
#endif
		  for (i = 0; i < num; i++)
		    {
		       if ((((ewin->desktop == lst[i]->desktop)
			     && !(lst[i]->iconified)) || (lst[i]->sticky))
			   && (!(lst[i]->floating)) && (lst[i] != ewin)
			   && (!(lst[i]->ignorearrange))
			   && SPANS_COMMON(ewin->y, ewin->h, lst[i]->y,
					   lst[i]->h))
			 {
			    if (((lst[i]->x + lst[i]->w) <= x)
				&& ((lst[i]->x + lst[i]->w) >= x1))
			       x1 = lst[i]->x + lst[i]->w;
			    else if (((x + w) <= lst[i]->x)
				     && (x2 >= lst[i]->x))
			       x2 = lst[i]->x;
			 }
		    }
		  Efree(lst);
		  x = x1;
		  w = x2 - x1 - (ewin->border->border.left +
				 ewin->border->border.right);
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
#if ENABLE_KDE
		  if (mode.kde_support)
		    {
		       x1 = mode.kde_x1;
		       x2 = mode.kde_x2;
		    }
		  else
		    {
#endif
		       x1 = 0;
		       x2 = root.w;
#if ENABLE_KDE
		    }
#endif
#ifdef HAS_XINERAMA
		  if (xinerama_active)
		    {
		       XineramaScreenInfo *screens;
		       int                 num_screens;

		       screens = XineramaQueryScreens(disp, &num_screens);
		       for (i = 0; i < num_screens; i++)
			 {
			    if (ewin->x >= screens[i].x_org)
			      {
				 if (ewin->x <=
				     (screens[i].width + screens[i].x_org))
				   {
				      if (ewin->y >= screens[i].y_org)
					{
					   if (ewin->y <=
					       (screens[i].height +
						screens[i].y_org))
					     {
						if (screens[i].x_org > x1)
						  {
						     x1 = screens[i].x_org;
						  }
						if (screens[i].x_org +
						    screens[i].width < x2)
						  {
						     x2 = screens[i].x_org +
							screens[i].width;
						  }
					     }
					}
				   }
			      }
			 }
		    }
#endif
		  for (i = 0; i < num; i++)
		    {
		       if ((lst[i] != ewin) && (!(lst[i]->ignorearrange))
			   && (!(lst[i]->floating)) && !(lst[i]->iconified)
			   &&
			   (((ewin->desktop
			      == lst[i]->desktop) && (lst[i]->fixedpos))
			    || (lst[i]->sticky))
			   && SPANS_COMMON(ewin->y, ewin->h, lst[i]->y,
					   lst[i]->h))
			 {
			    if (((lst[i]->x + lst[i]->w) <= x)
				&& ((lst[i]->x + lst[i]->w) >= x1))
			       x1 = lst[i]->x + lst[i]->w;
			    else if (((x + w) <= lst[i]->x)
				     && (x2 >= lst[i]->x))
			       x2 = lst[i]->x;
			 }
		    }
		  Efree(lst);
		  x = x1;
		  w = x2 - x1 - (ewin->border->border.left +
				 ewin->border->border.right);
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
		  x = ewin->x;
		  w = ewin->w;
#if ENABLE_KDE
		  if (mode.kde_support)
		    {
		       x1 = mode.kde_x1;
		       x2 = mode.kde_x2;
		       y1 = mode.kde_y1;
		       y2 = mode.kde_y2;
		    }
		  else
		    {
#endif
		       x1 = 0;
		       x2 = root.w;
		       y1 = 0;
		       y2 = root.h;
#if ENABLE_KDE
		    }
#endif
#ifdef HAS_XINERAMA
		  if (xinerama_active)
		    {
		       XineramaScreenInfo *screens;
		       int                 num_screens;

		       screens = XineramaQueryScreens(disp, &num_screens);
		       for (i = 0; i < num_screens; i++)
			 {
			    if (ewin->x >= screens[i].x_org)
			      {
				 if (ewin->x <=
				     (screens[i].width + screens[i].x_org))
				   {
				      if (ewin->y >= screens[i].y_org)
					{
					   if (ewin->y <=
					       (screens[i].height +
						screens[i].y_org))
					     {
						if (screens[i].x_org > x1)
						  {
						     x1 = screens[i].x_org;
						  }
						if (screens[i].x_org +
						    screens[i].width < x2)
						  {
						     x2 = screens[i].x_org +
							screens[i].width;
						  }
						if (screens[i].y_org > y1)
						  {
						     y1 = screens[i].y_org;
						  }
						if (screens[i].y_org +
						    screens[i].height < y2)
						  {
						     y2 = screens[i].y_org +
							screens[i].height;
						  }
					     }
					}
				   }
			      }
			 }
		    }
#endif
		  for (i = 0; i < num; i++)
		    {
		       if ((((ewin->desktop == lst[i]->desktop)
			     && !(lst[i]->iconified)) || (lst[i]->sticky))
			   && (!(lst[i]->floating)) && (lst[i] != ewin)
			   && (!(lst[i]->ignorearrange))
			   && SPANS_COMMON(ewin->x, ewin->w, lst[i]->x,
					   lst[i]->w))
			 {
			    if (((lst[i]->y + lst[i]->h) <= y)
				&& ((lst[i]->y + lst[i]->h) >= y1))
			       y1 = lst[i]->y + lst[i]->h;
			    else if (((y + h) <= lst[i]->y)
				     && (y2 >= lst[i]->y))
			       y2 = lst[i]->y;
			 }
		    }
		  y = y1;
		  h = y2 - y1 - (ewin->border->border.top +
				 ewin->border->border.bottom);
		  for (i = 0; i < num; i++)
		    {
		       if (((ewin->desktop == lst[i]->desktop)
			    || (lst[i]->sticky)) && (!(lst[i]->floating))
			   && (lst[i] != ewin) && (!(lst[i]->ignorearrange))
			   && SPANS_COMMON(y, h, lst[i]->y, lst[i]->h))
			 {
			    if (((lst[i]->x + lst[i]->w) <= x)
				&& ((lst[i]->x + lst[i]->w) >= x1))
			       x1 = lst[i]->x + lst[i]->w;
			    else if (((x + w) <= lst[i]->x)
				     && (x2 >= lst[i]->x))
			       x2 = lst[i]->x;
			 }
		    }
		  x = x1;
		  w = x2 - x1 - (ewin->border->border.left +
				 ewin->border->border.right);
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
		  x = ewin->x;
		  w = ewin->w;
#if ENABLE_KDE
		  if (mode.kde_support)
		    {
		       x1 = mode.kde_x1;
		       x2 = mode.kde_x2;
		       y1 = mode.kde_y1;
		       y2 = mode.kde_y2;
		    }
		  else
		    {
#endif
		       x1 = 0;
		       x2 = root.w;
		       y1 = 0;
		       y2 = root.h;
#if ENABLE_KDE
		    }
#endif
#ifdef HAS_XINERAMA
		  if (xinerama_active)
		    {
		       XineramaScreenInfo *screens;
		       int                 num_screens;

		       screens = XineramaQueryScreens(disp, &num_screens);
		       for (i = 0; i < num_screens; i++)
			 {
			    if (ewin->x >= screens[i].x_org)
			      {
				 if (ewin->x <=
				     (screens[i].width + screens[i].x_org))
				   {
				      if (ewin->y >= screens[i].y_org)
					{
					   if (ewin->y <=
					       (screens[i].height +
						screens[i].y_org))
					     {
						if (screens[i].x_org > x1)
						  {
						     x1 = screens[i].x_org;
						  }
						if (screens[i].x_org +
						    screens[i].width < x2)
						  {
						     x2 = screens[i].x_org +
							screens[i].width;
						  }
						if (screens[i].y_org > y1)
						  {
						     y1 = screens[i].y_org;
						  }
						if (screens[i].y_org +
						    screens[i].height < y2)
						  {
						     y2 = screens[i].y_org +
							screens[i].height;
						  }
					     }
					}
				   }
			      }
			 }
		    }
#endif
		  for (i = 0; i < num; i++)
		    {
		       if (((lst[i] != ewin) && (!(lst[i]->ignorearrange))
			    && !(lst[i]->iconified)) && (!(lst[i]->floating))
			   &&
			   (((ewin->desktop
			      == lst[i]->desktop) && (lst[i]->fixedpos))
			    || (lst[i]->sticky))
			   && SPANS_COMMON(ewin->x, ewin->w, lst[i]->x,
					   lst[i]->w))
			 {
			    if (((lst[i]->y + lst[i]->h) <= y)
				&& ((lst[i]->y + lst[i]->h) >= y1))
			       y1 = lst[i]->y + lst[i]->h;
			    else if (((y + h) <= lst[i]->y)
				     && (y2 >= lst[i]->y))
			       y2 = lst[i]->y;
			 }
		    }
		  y = y1;
		  h = y2 - y1 - (ewin->border->border.top +
				 ewin->border->border.bottom);
		  for (i = 0; i < num; i++)
		    {
		       if ((lst[i] != ewin) && (!(lst[i]->ignorearrange))
			   && (!(lst[i]->floating))
			   &&
			   (((ewin->desktop
			      == lst[i]->desktop) && (lst[i]->fixedpos))
			    || (lst[i]->sticky))
			   && SPANS_COMMON(y, h, lst[i]->y, lst[i]->h))
			 {
			    if (((lst[i]->x + lst[i]->w) <= x)
				&& ((lst[i]->x + lst[i]->w) >= x1))
			       x1 = lst[i]->x + lst[i]->w;
			    else if (((x + w) <= lst[i]->x)
				     && (x2 >= lst[i]->x))
			       x2 = lst[i]->x;
			 }
		    }
		  x = x1;
		  w = x2 - x1 - (ewin->border->border.left +
				 ewin->border->border.right);
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

#if ENABLE_KDE
   if (mode.kde_support)
      KDE_UpdateClient(ewin);
#endif

   return;
}
