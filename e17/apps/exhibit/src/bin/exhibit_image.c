#include "exhibit.h"

void
_ex_image_mouse_down(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Mouse_Up_Down *ev;
   Exhibit *e;
   
   e = data;
   ev = event;
   
   if(ev->button == 1)
     {
	Ecore_X_Cursor cursor;
	
	e->mouse.down = 1;
	e->mouse.x = ev->canvas.x;
	e->mouse.y = ev->canvas.y;
	
	if ((cursor = ecore_x_cursor_shape_get(ECORE_X_CURSOR_FLEUR)))
	  ecore_x_window_cursor_set(ecore_evas_software_x11_window_get(ETK_WINDOW(e->win)->ecore_evas), cursor);
     }
   else if(ev->button == 3)
     {
	e->menu = etk_menu_new();
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Zoom in"), EX_IMAGE_ZOOM_IN, ETK_MENU_SHELL(e->menu), ETK_CALLBACK(_ex_menu_zoom_in_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Zoom out"), EX_IMAGE_ZOOM_OUT, ETK_MENU_SHELL(e->menu), ETK_CALLBACK(_ex_menu_zoom_out_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Zoom 1:1"), EX_IMAGE_ONE_TO_ONE, ETK_MENU_SHELL(e->menu), ETK_CALLBACK(_ex_menu_zoom_one_to_one_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Fit to window"), EX_IMAGE_FIT_TO_WINDOW, ETK_MENU_SHELL(e->menu), ETK_CALLBACK(_ex_menu_fit_to_window_cb), e);
	etk_menu_popup(e->menu);
     }
}

void
_ex_image_mouse_up(Etk_Object *object, void *event, void *data)
{
   Exhibit *e;
   Etk_Event_Mouse_Up_Down *ev;
      
   e = data;
   ev = event;
      
   if(ev->button == 1)
     {	
	Ecore_X_Cursor cursor;	
	
	e->mouse.down = 0;
	if ((cursor = ecore_x_cursor_shape_get(ECORE_X_CURSOR_LEFT_PTR)))
	  ecore_x_window_cursor_set(ecore_evas_software_x11_window_get(ETK_WINDOW(e->win)->ecore_evas), cursor);
     }
}

void
_ex_image_mouse_move(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Mouse_Move *ev;
   Exhibit *e;
   Etk_Widget *hs, *vs;
   int dx, dy;   
   
   e = data;
   ev = event;
   
   if(!e->mouse.down) return;
   
   hs = etk_scrolled_view_hscrollbar_get(ETK_SCROLLED_VIEW(e->scrolled_view));
   vs = etk_scrolled_view_vscrollbar_get(ETK_SCROLLED_VIEW(e->scrolled_view));
   
   dx = ev->prev.canvas.x - ev->cur.canvas.x;
   dy = ev->prev.canvas.y - ev->cur.canvas.y;   
   
   etk_range_value_set(ETK_RANGE(hs), etk_range_value_get(ETK_RANGE(hs)) + dx);
   etk_range_value_set(ETK_RANGE(vs), etk_range_value_get(ETK_RANGE(vs)) + dy);
}

void
_ex_image_flip_horizontal(Etk_Image *im)
{
   unsigned int *data;
   int           w, h;
   int           x, y;
   unsigned int *p1, *p2, *tmp;
   
   if(im->use_edje)
     return;
   
   etk_image_size_get(im, &w, &h);
   data = evas_object_image_data_get(im->image_object, TRUE);
   
   for (y = 0; y < h; y++)
     {
	p1 = data + (y * w);
	p2 = data + ((y + 1) * w) - 1;
	for (x = 0; x < (w >> 1); x++)
	  {
	     tmp = *p1;
	     *p1 = *p2;
	     *p2 = tmp;
	     p1++;
	     p2--;
	  }
     }
   
   evas_object_image_data_set(im->image_object, data);
   evas_object_image_data_update_add(im->image_object, 0, 0, w, h);
}

void
_ex_image_flip_vertical(Etk_Image *im)
{
   unsigned int *data;
   int           w, h;
   int           x, y;
   unsigned int *p1, *p2, *tmp;
   
   if(im->use_edje)
     return;
   
   etk_image_size_get(im, &w, &h);
   data = evas_object_image_data_get(im->image_object, TRUE);
   
   for (y = 0; y < (h >> 1); y++)
     {
	p1 = data + (y * w);
	p2 = data + ((h - 1 - y) * w);
	for (x = 0; x < w; x++)
	  {
	     tmp = *p1;
	     *p1 = *p2;
	     *p2 = tmp;
	     p1++;
	     p2++;
	  }
     }
      
   evas_object_image_data_set(im->image_object, data);
   evas_object_image_data_update_add(im->image_object, 0, 0, w, h);   
}

/* Directions (source is right/down):
 * 0 = down/right (flip over ul-dr diagonal)
 * 1 = down/left  (rotate 90 degrees clockwise)
 * 2 = up/right   (rotate 90 degrees counterclockwise)
 * 3 = up/left    (flip over ur-ll diagonal)
 */
void
_ex_image_flip_diagonal(Etk_Image *im, int direction)
{
   unsigned int   *data, *data2, *to, *from;
   int             x, y, w, hw, tmp, iw, ih;
     
   if(im->use_edje)
     return;
   
   etk_image_size_get(im, &iw, &ih);
   data2 = evas_object_image_data_get(im->image_object, FALSE);
      
   data = malloc(iw * ih * sizeof(unsigned int));
   from = data2;
   w = ih;
   ih = iw;
   iw = w;
   hw = w * ih;
   switch (direction)
     {
      default:
      case 0:                 /*\ DOWN_RIGHT \ */
	to = data;
	hw = -hw + 1;
	break;
      case 1:                 /*\ DOWN_LEFT \ */
	to = data + w - 1;
	hw = -hw - 1;
	break;
      case 2:                 /*\ UP_RIGHT \ */
	to = data + hw - w;
	w = -w;
	hw = hw + 1;
	break;
      case 3:                 /*\ UP_LEFT \ */
	to = data + hw - 1;
	w = -w;
	hw = hw - 1;
	break;
     }
   from = data2;
   for (x = iw; --x >= 0;)
     {
	for (y = ih; --y >= 0;)
	  {
	     *to = *from;
	     from++;
	     to += w;
	  }
	to += hw;
     }
   
   evas_object_image_size_set(im->image_object, iw, ih);   
   evas_object_image_data_set(im->image_object, data);
   evas_object_image_data_update_add(im->image_object, 0, 0, iw, ih);
   etk_widget_size_recalc_queue(im);
}

void
_ex_image_blur(Etk_Image *im)
{
   unsigned int *data, *data2;
   int           w, h;
   int           x, y, mx, my, mw, mh, mt, xx, yy;
   int           a, r, g, b;
   int          *as, *rs, *gs, *bs;   
   int           rad = 2; /* maybe make this configurable later */
   unsigned int *p1, *p2, *tmp;
   
   if(im->use_edje)
     return;
   
   etk_image_size_get(im, &w, &h);
   data2 = evas_object_image_data_get(im->image_object, TRUE);
   
   if (rad < 1)
     return;
   data = malloc(w * h * sizeof(unsigned int));
   as = malloc(sizeof(int) * w);
   rs = malloc(sizeof(int) * w);
   gs = malloc(sizeof(int) * w);
   bs = malloc(sizeof(int) * w);
   
   for (y = 0; y < h; y++)
     {
	my = y - rad;
	mh = (rad << 1) + 1;
	if (my < 0)
	  {
	     mh += my;
	     my = 0;
	  }
	if ((my + mh) > h)
	  mh = h - my;
	
	p1 = data + (y * w);
	memset(as, 0, w * sizeof(int));
	memset(rs, 0, w * sizeof(int));
	memset(gs, 0, w * sizeof(int));
	memset(bs, 0, w * sizeof(int));
	
	for (yy = 0; yy < mh; yy++)
	  {
	     p2 = data2 + ((yy + my) * w);
	     for (x = 0; x < w; x++)
	       {
		  as[x] += (*p2 >> 24) & 0xff;
		  rs[x] += (*p2 >> 16) & 0xff;
		  gs[x] += (*p2 >> 8) & 0xff;
		  bs[x] += *p2 & 0xff;
		  p2++;
	       }
	  }
	if (w > ((rad << 1) + 1))
	  {
	     for (x = 0; x < w; x++)
	       {
		  a = 0;
		  r = 0;
		  g = 0;
		  b = 0;
		  mx = x - rad;
		  mw = (rad << 1) + 1;
		  if (mx < 0)
		    {
		       mw += mx;
		       mx = 0;
		    }
		  if ((mx + mw) > w)
		    mw = w - mx;
		  mt = mw * mh;
		  for (xx = mx; xx < (mw + mx); xx++)
		    {
		       a += as[xx];
		       r += rs[xx];
		       g += gs[xx];
		       b += bs[xx];
		    }
		  a = a / mt;
		  r = r / mt;
		  g = g / mt;
		  b = b / mt;
		  *p1 = (a << 24) | (r << 16) | (g << 8) | b;
		  p1++;
	       }
	  }
	else
	  {
	  }
     }
   free(as);
   free(rs);
   free(gs);
   free(bs);
   
   evas_object_image_data_set(im->image_object, data);
   evas_object_image_data_update_add(im->image_object, 0, 0, w, h);   
}

void
_ex_image_sharpen(Etk_Image *im)
{
   unsigned int *data, *data2;
   int           w, h;
   int           x, y;
   int           a, r, g, b;
   int          *as, *rs, *gs, *bs;   
   int           rad = 2; /* maybe make this configurable later */
   unsigned int *p1, *p2, *tmp;
   
   if(im->use_edje)
     return;

   etk_image_size_get(im, &w, &h);
   data2 = evas_object_image_data_get(im->image_object, TRUE);
      
   data = malloc(w * h * sizeof(unsigned int));
   if (rad == 0)
     return;
   else
     {
	int                 mul, mul2, tot;
	
	mul = (rad * 4) + 1;
	mul2 = rad;
	tot = mul - (mul2 * 4);
	for (y = 1; y < (h - 1); y++)
	  {
	     p1 = data2 + 1 + (y * w);
	     p2 = data + 1 + (y * w);
	     for (x = 1; x < (w - 1); x++)
	       {
		  b = (int)((p1[0]) & 0xff) * 5;
		  g = (int)((p1[0] >> 8) & 0xff) * 5;
		  r = (int)((p1[0] >> 16) & 0xff) * 5;
		  a = (int)((p1[0] >> 24) & 0xff) * 5;
		  b -= (int)((p1[-1]) & 0xff);
		  g -= (int)((p1[-1] >> 8) & 0xff);
		  r -= (int)((p1[-1] >> 16) & 0xff);
		  a -= (int)((p1[-1] >> 24) & 0xff);
		  b -= (int)((p1[1]) & 0xff);
		  g -= (int)((p1[1] >> 8) & 0xff);
		  r -= (int)((p1[1] >> 16) & 0xff);
		  a -= (int)((p1[1] >> 24) & 0xff);
		  b -= (int)((p1[-w]) & 0xff);
		  g -= (int)((p1[-w] >> 8) & 0xff);
		  r -= (int)((p1[-w] >> 16) & 0xff);
		  a -= (int)((p1[-w] >> 24) & 0xff);
		  b -= (int)((p1[w]) & 0xff);
		  g -= (int)((p1[w] >> 8) & 0xff);
		  r -= (int)((p1[w] >> 16) & 0xff);
		  a -= (int)((p1[w] >> 24) & 0xff);
		  
		  a = (a & ((~a) >> 16));
		  a = ((a | ((a & 256) - ((a & 256) >> 8))));
		  r = (r & ((~r) >> 16));
		  r = ((r | ((r & 256) - ((r & 256) >> 8))));
		  g = (g & ((~g) >> 16));
		  g = ((g | ((g & 256) - ((g & 256) >> 8))));
		  b = (b & ((~b) >> 16));
		  b = ((b | ((b & 256) - ((b & 256) >> 8))));
		  
		  *p2 = (a << 24) | (r << 16) | (g << 8) | b;
		  p2++;
		  p1++;
	       }
	  }
     }
   
   evas_object_image_data_set(im->image_object, data);
   evas_object_image_data_update_add(im->image_object, 0, 0, w, h);      
}

void
_ex_image_save(Etk_Image *im)
{
   pid_t pid;
   
   if(!im->filename[0])
     return;
   
   if(!ecore_file_exists(im->filename))
     return;
   
   pid = fork();
   if(!pid)
     {
	evas_object_image_save(im->image_object, im->filename, NULL, NULL);
	exit(0);
     }
}

void
_ex_image_zoom(Etk_Image *im, int zoom)
{
   unsigned int *data;
   int           w, h;
   int           x, y;
   unsigned int *p1, *p2, *tmp;
   
   if(im->use_edje)
     return;

   etk_image_size_get(im, &w, &h);
   if(zoom > 0)
     {
	w *= zoom;
	h *= zoom;
     }
   else if(zoom < 0)
     {
	w /= abs(zoom);
	h /= abs(zoom);
     }

   evas_object_resize(im->image_object, w, h);
   evas_object_image_fill_set(im->image_object, 0, 0, w, h);
   etk_widget_size_request_set(im, w, h);
   etk_widget_redraw_queue(im);
}
