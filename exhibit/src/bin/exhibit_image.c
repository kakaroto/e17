/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "exhibit.h"
#include <Ecore_File.h>
#include <Ecore_Evas.h>
#define R_CMOD(r) \
   red_mapping[(int)(r)]
#define G_CMOD(g) \
   green_mapping[(int)(g)]
#define B_CMOD(b) \
   blue_mapping[(int)(b)]
#define A_CMOD(a) \
   alpha_mapping[(int)(a)]

#define A_VAL(p) ((DATA8 *)(p))[3]
#define R_VAL(p) ((DATA8 *)(p))[2]
#define G_VAL(p) ((DATA8 *)(p))[1]
#define B_VAL(p) ((DATA8 *)(p))[0]

static void _ex_image_delete_dialog_response(Etk_Object *obj, int response_id, void *data);
static void _ex_image_delete_cb(void *data);

Etk_Bool
_ex_image_mouse_wheel(Etk_Object *object, void *event, void *data)
{
   Exhibit *e;
   Etk_Event_Mouse_Wheel *ev;
   Evas *evas;
   
   ev = event;
   e = data;
   evas = etk_widget_toplevel_evas_get(ETK_WIDGET(e->win));   
   
   if(evas_key_modifier_is_set(evas_key_modifier_get(evas), "Control"))
     {
	if (ev->z > 0)
	  _ex_tab_current_zoom_in(data);
	else
	  _ex_tab_current_zoom_out(data);
     }
   else
     {	
	Etk_Tree_Row *row;
	
	if (ev->z > 0)
	  {
	     row = etk_tree_row_next_get(
		   etk_tree_selected_row_get(ETK_TREE(e->cur_tab->itree)));
	     etk_tree_row_select(row);
	     etk_tree_row_scroll_to(row, ETK_FALSE);
	  }
	else
	  {
	     row = etk_tree_row_prev_get(
		       etk_tree_selected_row_get(ETK_TREE(e->cur_tab->itree)));
	     etk_tree_row_select(row);
	     etk_tree_row_scroll_to(row, ETK_FALSE);
	  }
     }
   
   return ETK_FALSE;
}

void
_ex_image_mouse_down(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Mouse_Down *ev;
   Exhibit *e;
   
   e = data;
   ev = event;
   
   if(ev->button == 1)
     {
	if (ev->flags != ETK_MOUSE_DOUBLE_CLICK)
	  {
	     e->mouse.down = 1;
	     e->mouse.x = ev->canvas.x;
	     e->mouse.y = ev->canvas.y;
	
             etk_toplevel_pointer_push(ETK_TOPLEVEL(e->win), ETK_POINTER_MOVE);
	  }
	else
          _ex_main_window_fullscreen_toggle();
     }
   else if(ev->button == 2)
     {
	if (e->cur_tab->fit_window)
	   _ex_tab_current_zoom_one_to_one(e);
	else   
	   _ex_tab_current_fit_to_window(e);
     }
   else if(ev->button == 3)
     {
	Etk_Widget *menu_item;
	Etk_Widget *submenu;
#if 0
	/* do this better */
	if(e->menu)
	  {
	     etk_menu_popup(ETK_MENU(e->menu));
	     return;
	  }
#endif	
	e->menu = etk_menu_new();
	
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Undo"), ETK_STOCK_EDIT_UNDO, ETK_MENU_SHELL(e->menu), ETK_CALLBACK(_ex_menu_undo_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPARATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(e->menu), NULL, NULL);

	 menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Open With..."), ETK_STOCK_APPLICATION_X_EXECUTABLE, ETK_MENU_SHELL(e->menu), ETK_CALLBACK(_ex_menu_run_in_cb), e);
	 submenu = etk_menu_new();
	 etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(submenu));
	 _ex_menu_build_run_menu(submenu);

	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Favorites"), ETK_STOCK_EMBLEM_FAVORITE, ETK_MENU_SHELL(e->menu), ETK_CALLBACK(_ex_menu_run_in_cb), e);
	submenu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(submenu));
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Add to favorites"), ETK_STOCK_EMBLEM_PHOTOS, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_menu_add_to_fav_cb), e);
	if(_ex_image_is_favorite(e))
	  _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Remove from favorites"), ETK_STOCK_LIST_REMOVE, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_menu_remove_from_fav_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPARATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(e->menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Rotate clockwise"), ETK_STOCK_GO_NEXT, ETK_MENU_SHELL(e->menu), ETK_CALLBACK(_ex_menu_rot_clockwise_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Rotate counterclockwise"), ETK_STOCK_GO_PREVIOUS, ETK_MENU_SHELL(e->menu), ETK_CALLBACK(_ex_menu_rot_counter_clockwise_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Flip horizontally"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(e->menu), ETK_CALLBACK(_ex_menu_flip_horizontal_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Flip vertically"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(e->menu), ETK_CALLBACK(_ex_menu_flip_vertical_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPARATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(e->menu), NULL, NULL);
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Effects"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(e->menu), ETK_CALLBACK(_ex_menu_run_in_cb), e);
	submenu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(submenu));
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Blur"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_menu_blur_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Sharpen"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_menu_sharpen_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Brighten"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_menu_brighten_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPARATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(e->menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Toggle Comments"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(e->menu), ETK_CALLBACK(_ex_menu_comments_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Set as wallpaper"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(e->menu), ETK_CALLBACK(_ex_menu_set_wallpaper_cb), e);
	etk_menu_popup(ETK_MENU(e->menu));
     }
}

void
_ex_image_mouse_up(Etk_Object *object, void *event, void *data)
{
   Exhibit *e;
   Etk_Event_Mouse_Down *ev;
      
   e = data;
   ev = event;
      
   if(ev->button == 1)
     {
	e->mouse.down = 0;
        etk_toplevel_pointer_pop(ETK_TOPLEVEL(e->win), ETK_POINTER_MOVE);
     }
}

void
_ex_image_mouse_move(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Mouse_Move *ev;
   Exhibit *e;
   Etk_Range *hs, *vs;
   int dx, dy;   
   
   e = data;
   ev = event;
   
   if(!e->mouse.down) return;
   
   hs = etk_scrolled_view_hscrollbar_get(ETK_SCROLLED_VIEW(e->cur_tab->scrolled_view));
   vs = etk_scrolled_view_vscrollbar_get(ETK_SCROLLED_VIEW(e->cur_tab->scrolled_view));
   
   dx = ev->prev.canvas.x - ev->cur.canvas.x;
   dy = ev->prev.canvas.y - ev->cur.canvas.y;   
   
   etk_range_value_set(hs, etk_range_value_get(ETK_RANGE(hs)) + dx);
   etk_range_value_set(vs, etk_range_value_get(ETK_RANGE(vs)) + dy);
}

unsigned int *
_ex_image_data_copy(Etk_Image *im, unsigned int *data, int w, int h) 
{
   unsigned int *data2;
   
   data2 = etk_object_data_get(ETK_OBJECT(im), "undo");
   if(data2)
     E_FREE(data2);
   
   data2 = malloc(w * h * sizeof(unsigned int));
   memcpy(data2, data, w * h * sizeof(unsigned int));
   etk_object_data_set(ETK_OBJECT(im), "undo", data2);
   D(("Undo: setting data %p size %d, image %p\n", data,
	 w * h * sizeof(unsigned int), im));
   
   return data2;
}

void
_ex_image_undo(Etk_Image *im)
{
   unsigned int *data;
   int           w, h;
   char *edje;
   char *group;
   
   etk_image_edje_get(im, &edje, &group);
   if(edje != NULL)
     {
	free(edje);
	free(group);
	return;
     }
	
   etk_image_size_get(im, &w, &h);
   
   data = etk_object_data_get(ETK_OBJECT(im), "undo");
   
   if (data) 
     {
	D(("Undo: getting data %p, image %p\n", data, im));
	evas_object_image_data_set(etk_image_evas_object_get(im), data);
	evas_object_image_data_update_add(etk_image_evas_object_get(im), 0, 0, w, h);
	etk_object_data_set(ETK_OBJECT(im), "undo", NULL);
     }
}
	
void
_ex_image_flip_horizontal(Etk_Image *im)
{
   unsigned int *data;
   int           w, h;
   int           x, y;
   unsigned int *p1, *p2, tmp;
   char *edje;
   char *group;
   
   etk_image_edje_get(im, &edje, &group);
   if(edje != NULL)
     {
	free(edje);
	free(group);
	return;
     }   
   
   etk_image_size_get(im, &w, &h);
   data = evas_object_image_data_get(etk_image_evas_object_get(im), ETK_TRUE);
   _ex_image_data_copy(im, data, w, h); /* for undo */
   
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
   
   evas_object_image_data_set(etk_image_evas_object_get(im), data);
   evas_object_image_data_update_add(etk_image_evas_object_get(im), 0, 0, w, h);
}

void
_ex_image_flip_vertical(Etk_Image *im)
{
   unsigned int *data;
   int           w, h;
   int           x, y;
   unsigned int *p1, *p2, tmp;
   char *edje;
   char *group;
   
   etk_image_edje_get(im, &edje, &group);
   if(edje != NULL)
     {
	free(edje);
	free(group);
	return;
     }
   
   etk_image_size_get(im, &w, &h);
   data = evas_object_image_data_get(etk_image_evas_object_get(im), ETK_TRUE);
   _ex_image_data_copy(im, data, w, h); /* for undo */
   
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
      
   evas_object_image_data_set(etk_image_evas_object_get(im), data);
   evas_object_image_data_update_add(etk_image_evas_object_get(im), 0, 0, w, h);
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
   int             x, y, w, hw, iw, ih;
   char *edje;
   char *group;
   
   etk_image_edje_get(im, &edje, &group);
   if(edje != NULL)
     {
	free(edje);
	free(group);
	return;
     }
   
   etk_image_size_get(im, &iw, &ih);
   data2 = evas_object_image_data_get(etk_image_evas_object_get(im), ETK_FALSE);
	 
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
   
   evas_object_image_size_set(etk_image_evas_object_get(im), iw, ih);   
   evas_object_image_data_set(etk_image_evas_object_get(im), data);
   evas_object_image_data_update_add(etk_image_evas_object_get(im), 0, 0, iw, ih);
   etk_widget_size_request_set(ETK_WIDGET(im), iw, ih);


   if (e->options->rotate_autosave)
      _ex_image_save(ETK_IMAGE(e->cur_tab->image));
}

void
_ex_image_blur(Etk_Image *im)
{
   unsigned int *data, *data2;
   int           w, h;
   int           x, y, mx, my, mw, mh, mt, xx, yy;
   int           a, r, g, b;
   int          *as, *rs, *gs, *bs;   
   int           rad = 2; 
   unsigned int *p1, *p2;
   char *edje;
   char *group;
   
   etk_image_edje_get(im, &edje, &group);
   if(edje != NULL)
     {
	free(edje);
	free(group);
	return;
     }

   etk_image_size_get(im, &w, &h);
   data2 = evas_object_image_data_get(etk_image_evas_object_get(im), ETK_TRUE);
   _ex_image_data_copy(im, data2, w, h); /* for undo */

   rad = e->options->blur_thresh;
   D(("_ex_image_blur: using rad: %d\n", rad));
   
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
   E_FREE(as);
   E_FREE(rs);
   E_FREE(gs);
   E_FREE(bs);
   
   evas_object_image_data_set(etk_image_evas_object_get(im), data);
   evas_object_image_data_update_add(etk_image_evas_object_get(im), 0, 0, w, h);   
}

void
_ex_image_sharpen(Etk_Image *im)
{
   unsigned int *data, *data2;
   int           w, h;
   int           x, y;
   int           a, r, g, b;
   int           rad = 2;
   unsigned int *p1, *p2;
   char *edje;
   char *group;
   
   etk_image_edje_get(im, &edje, &group);
   if(edje != NULL)
     {
	free(edje);
	free(group);
	return;
     }

   etk_image_size_get(im, &w, &h);
   data2 = evas_object_image_data_get(etk_image_evas_object_get(im), ETK_TRUE);
   _ex_image_data_copy(im, data2, w, h); /* for undo */
      
   data = malloc(w * h * sizeof(unsigned int));

   /* FIXME I cant see any difference with this changed?? -- Balony */ 
   rad = e->options->sharpen_thresh;
   D(("_ex_image_sharpen: using rad: %d\n", rad));
   
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
   
   evas_object_image_data_set(etk_image_evas_object_get(im), data);
   evas_object_image_data_update_add(etk_image_evas_object_get(im), 0, 0, w, h);      
}

void
_ex_image_save(Etk_Image *im)
{
   pid_t pid;
   char *filename;
   
   etk_image_file_get(im, &filename, NULL);
   if(!filename)
     return;
   
   if(!ecore_file_exists(filename))
     return;
   
   pid = fork();
   if(!pid)
     {
	evas_object_image_save(etk_image_evas_object_get(im), filename, NULL, NULL);
	exit(0);
     }
}

void
_ex_image_move_entry_cb(Etk_Object *object, Etk_Event_Key_Down *ev, void *data)
{
   if (!strcmp(ev->key, "Return") || !strcmp(ev->key, "KP_Enter"))
     _ex_image_move_cb(data);   
}

void
_ex_image_move_cb(void *data)
{
   Ex_Filedialog *fd = data;
   char *basename;
   int ret;

   basename = strdup(etk_filechooser_widget_selected_file_get
      (ETK_FILECHOOSER_WIDGET(fd->filechooser)));
   ret = _ex_image_save_as_cb(data);
   if (basename)
     {
	if (ret)
	  ecore_file_unlink(basename);
	free(basename);
     }
}

void
_ex_image_save_as_entry_cb(Etk_Object *object, Etk_Event_Key_Down *ev, void *data) 
{
   if (!strcmp(ev->key, "Return") || !strcmp(ev->key, "KP_Enter"))
     _ex_image_save_as_cb(data);   
}

int
_ex_image_save_as_cb(void *data)
{
   Ex_Filedialog *fd = data;
   char file[1024];
   const char *basename;
   const char *dir;
   Etk_Image *im = ETK_IMAGE(fd->e->cur_tab->image);
   int res = 0;

   basename = etk_filechooser_widget_selected_file_get
      (ETK_FILECHOOSER_WIDGET(fd->filechooser));
   dir = etk_filechooser_widget_current_folder_get
      (ETK_FILECHOOSER_WIDGET(fd->filechooser));

   if (!dir || !basename)
      goto destroy;

   {
      int ret;

      snprintf(file, sizeof(file), "%s/%s", dir, basename);
      ret = ecore_file_exists(file);
      if (ret)
          goto destroy;
   }
  
   D(("Saving: %s\n", file));
   /* Dont fork for the tree polulating to work */
   evas_object_image_save(etk_image_evas_object_get(im), file, NULL, NULL);
   res = 1;

destroy:
   _ex_image_file_dialog_destroy(fd);
   return res;
}

Ex_Filedialog *
_ex_image_file_dialog_new(void)
{
   Ex_Filedialog *fd;
   Etk_Widget *vbox;
   
   fd = calloc(1, sizeof(Ex_Filedialog));
   if (!fd) return NULL;
   fd->e = e;
   fd->win = NULL;
   
   fd->win = etk_window_new();
   etk_signal_connect("delete-event", ETK_OBJECT(fd->win), 
		      ETK_CALLBACK(etk_window_hide_on_delete), fd->win);
   
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(fd->win), vbox);
   
   fd->filechooser = etk_filechooser_widget_new();
   etk_container_add(ETK_CONTAINER(vbox), fd->filechooser);
   
   //D(("Selected original filename: %s\n", e->cur_tab->cur_file));
   
   fd->hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(vbox), fd->hbox);

   return fd;
}

void
_ex_image_file_dialog_destroy(Ex_Filedialog *fd)
{
   if (!fd)
     return;

   if (fd->win)
     etk_object_destroy(ETK_OBJECT(fd->win));

   free(fd);
}

void
_ex_image_move(void)
{
   Ex_Filedialog *fd;
   Etk_Widget *btn;

   fd = _ex_image_file_dialog_new();
   etk_filechooser_widget_is_save_set(ETK_FILECHOOSER_WIDGET(fd->filechooser), ETK_TRUE);
   etk_filechooser_widget_selected_file_set(ETK_FILECHOOSER_WIDGET(fd->filechooser), fd->e->cur_tab->cur_file);
   etk_window_title_set(ETK_WINDOW(fd->win), "Exhibit - Move image to...");


   btn = etk_button_new_with_label("Move");
   etk_box_append(ETK_BOX(fd->hbox), btn, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(btn), 
	 ETK_CALLBACK(_ex_image_move_cb), fd);
   
   btn = etk_button_new_with_label("Cancel");
   etk_box_append(ETK_BOX(fd->hbox), btn, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(btn), 
	 ETK_CALLBACK(_ex_image_file_dialog_destroy), fd);

   etk_widget_show_all(fd->win);
}

void
_ex_image_save_as(void)
{
   Ex_Filedialog *fd;
   Etk_Widget *btn;

   fd = _ex_image_file_dialog_new();
   etk_filechooser_widget_is_save_set(ETK_FILECHOOSER_WIDGET(fd->filechooser), ETK_TRUE);
   etk_filechooser_widget_selected_file_set(ETK_FILECHOOSER_WIDGET(fd->filechooser), fd->e->cur_tab->cur_file);
   etk_window_title_set(ETK_WINDOW(fd->win), "Exhibit - Save image as...");


   btn = etk_button_new_with_label("Save");
   etk_box_append(ETK_BOX(fd->hbox), btn, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(btn), 
			      ETK_CALLBACK(_ex_image_save_as_cb), fd);
   
   btn = etk_button_new_with_label("Cancel");
   etk_box_append(ETK_BOX(fd->hbox), btn, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(btn), 
	 ETK_CALLBACK(_ex_image_file_dialog_destroy), fd);

   etk_widget_show_all(fd->win);
}

static void
_ex_image_delete_cb(void *data)
{
   Exhibit *e = data;
   Ex_Tab *tab = e->cur_tab;
   char string[PATH_MAX];
   int ret;

   snprintf(string, sizeof(string), "%s%s", tab->set_img_path, tab->cur_file);

   ret = remove(string);
   if (ret == -1) 
     {
	_ex_main_dialog_show("Error deleting file!", ETK_MESSAGE_DIALOG_ERROR);
	etk_object_destroy(ETK_OBJECT(tab->dialog));
	return;
     }

   _ex_main_image_unset();
   etk_object_destroy(ETK_OBJECT(tab->dialog));
   D(("Deleted %s\n", string));
}

static void
_ex_image_delete_dialog_response(Etk_Object *obj, int response_id, void *data)
{
   switch(response_id)
     {
      case ETK_RESPONSE_OK:
	 _ex_image_delete_cb(e);
	 break;
      case ETK_RESPONSE_CANCEL:
	 etk_object_destroy(ETK_OBJECT(obj));
	 break;
      default:
	 break;
     }
}

void
_ex_image_delete(Exhibit *e)
{
   Ex_Tab *tab = e->cur_tab;
   char string[PATH_MAX];

   snprintf(string, sizeof(string),
	 "Are you sure you want to delete this image? <br>%s<br> ", 
	 tab->cur_file);

   tab->dialog = etk_message_dialog_new(ETK_MESSAGE_DIALOG_QUESTION, 
	 ETK_MESSAGE_DIALOG_OK_CANCEL, 
	 string);

   etk_signal_connect("response", ETK_OBJECT(tab->dialog), 
	 ETK_CALLBACK(_ex_image_delete_dialog_response), e);
   
   etk_container_border_width_set(ETK_CONTAINER(tab->dialog), 4);
   etk_window_title_set(ETK_WINDOW(tab->dialog), 
	 _("Exhibit - Confirm delete"));

   etk_widget_show_all(tab->dialog);
}


static void
_ex_image_rename_dialog_response(Etk_Object *obj, int response_id, void *data)
{
   const char *string;
   char *newpath;
   char *oldpath;
   int ret;
   Ex_Tab *tab = e->cur_tab;
   
   switch(response_id)
     {
      case ETK_RESPONSE_OK:
	 string = etk_entry_text_get(ETK_ENTRY(data));
	 newpath = malloc(PATH_MAX);
	 oldpath = malloc(PATH_MAX);
	 snprintf(newpath, PATH_MAX, "%s%s", tab->set_img_path, string);
	 snprintf(oldpath, PATH_MAX, "%s", tab->cur_file);
	 D(("Renaming from %s -> %s\n", oldpath, newpath));

printf("Rename '%s' to '%s' \n", oldpath, newpath);
	 ret = rename(oldpath, newpath);
	 if (ret == -1) 
	   _ex_main_dialog_show("Error renaming file!", ETK_MESSAGE_DIALOG_ERROR);
	 else
	   _ex_main_image_unset();

	 E_FREE(newpath);
	E_FREE(oldpath);
	 break;
      default:
	 break;
     }
   
   etk_object_destroy(ETK_OBJECT(obj));
}

void
_ex_image_rename(void) 
{
   Ex_Tab *tab = e->cur_tab;
   Etk_Widget *dialog;
   Etk_Widget *entry;
   Etk_Widget *label;
   char string[PATH_MAX];
   char labeltext[PATH_MAX + 100];

   snprintf(string, sizeof(string), "%s", basename(tab->cur_file));
   dialog = etk_dialog_new();

   snprintf(labeltext, sizeof(labeltext), "Rename file '%s'", string);

   label = etk_label_new(labeltext);

   etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), ETK_WIDGET(label), 
	 ETK_BOX_START, ETK_BOX_NONE, 5);

   label = etk_label_new("New filename");
   etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), ETK_WIDGET(label), 
	 ETK_BOX_START, ETK_BOX_NONE, 2);

   entry = etk_entry_new();
   etk_entry_text_set(ETK_ENTRY(entry), string);
   
   etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), ETK_WIDGET(entry), 
	 ETK_BOX_START, ETK_BOX_NONE, 0);
   
   etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_OK, ETK_RESPONSE_OK);
   etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_CANCEL, ETK_RESPONSE_CANCEL);
   etk_signal_connect("response", ETK_OBJECT(dialog),
	 ETK_CALLBACK(_ex_image_rename_dialog_response), entry);
   
   etk_container_border_width_set(ETK_CONTAINER(dialog), 15);
   etk_window_title_set(ETK_WINDOW(dialog), 
	 _("Exhibit - rename"));

   etk_widget_show_all(dialog);
}

void
_ex_image_refresh(void) 
{
   char file[PATH_MAX];

   snprintf(file, sizeof(file), "%s%s", e->cur_tab->set_img_path, e->cur_tab->cur_file);
   _ex_main_image_set(e, file);
}

void
_ex_image_run(const char *app)
{
   Ecore_Exe *exe = NULL;
   char *tmp = NULL;
   char str[PATH_MAX];

   tmp = malloc(PATH_MAX + 10);
   memset(tmp, 0, PATH_MAX + 10);

   if (!tmp)
     return;
   
   snprintf(str, sizeof(str), "%s%s", e->cur_tab->set_img_path, e->cur_tab->cur_file);
   snprintf(tmp, PATH_MAX, app, str);

   if (strlen(tmp) <= 0) 
     return;
   
   exe = ecore_exe_run(tmp, NULL);
   if (exe) 
     {
      D(("ecore_exe_run: %s\n", tmp));
      return;
     } 
   else
     _ex_main_dialog_show("Error runnng command", ETK_MESSAGE_DIALOG_ERROR);

   E_FREE(tmp);
   E_FREE(exe);
}

void
_ex_image_zoom(Etk_Image *im, int zoom)
{
   int           w, h;
   char *edje;
   char *group;
   
   etk_image_edje_get(im, &edje, &group);
   if(edje != NULL)
     {
	free(edje);
	free(group);
	return;
     }   

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

   etk_widget_size_request_set(ETK_WIDGET(im), w, h);
   etk_widget_redraw_queue(ETK_WIDGET(im));
}

void
_ex_image_brightness(Etk_Image *im, int brightness)
{
   DATA32 *data, *p;
   int     w, h;
   int     i, j;
   int     val2;
   DATA8   red_mapping[256];
   DATA8   green_mapping[256];   
   DATA8   blue_mapping[256];
   DATA8   alpha_mapping[256];
   char *edje;
   char *group;
   
   etk_image_edje_get(im, &edje, &group);
   if(edje != NULL)
     {
	free(edje);
	free(group);
	return;
     }
   
   for (i = 0; i < 256; i++)
     {
	red_mapping[i] = (DATA8) i;
	green_mapping[i] = (DATA8) i;
	blue_mapping[i] = (DATA8) i;
	alpha_mapping[i] = (DATA8) i;
     }
      
   etk_image_size_get(im, &w, &h);
   data = evas_object_image_data_get(etk_image_evas_object_get(im), ETK_TRUE);
   _ex_image_data_copy(im, data, w, h); /* for undo */
   
   for (i = 0; i < 256; i++)
     {
	val2 = (int)red_mapping[i] + brightness;
	if (val2 < 0)
	  val2 = 0;
	if (val2 > 255)
	  val2 = 255;
	red_mapping[i] = (DATA8) val2;
	
	val2 = (int)green_mapping[i] + brightness;
	if (val2 < 0)
	  val2 = 0;
	if (val2 > 255)
	  val2 = 255;
	green_mapping[i] = (DATA8) val2;
	
	val2 = (int)blue_mapping[i] + brightness;
	if (val2 < 0)
	  val2 = 0;
	if (val2 > 255)
	  val2 = 255;
	blue_mapping[i] = (DATA8) val2;
	
	val2 = (int)alpha_mapping[i] + brightness;
	if (val2 < 0)
	  val2 = 0;
	if (val2 > 255)
	  val2 = 255;
	alpha_mapping[i] = (DATA8) val2;
     }
   
   for(i = 0; i < w - 1; i++) 
     {
      for(j = 0; j < h - 1; j++) 
	  {
	     p = &data[i + (w * j)];
	     
	     R_VAL(p) = R_CMOD(R_VAL(p));
	     G_VAL(p) = G_CMOD(G_VAL(p));
	     B_VAL(p) = B_CMOD(B_VAL(p));
	     A_VAL(p) = A_CMOD(A_VAL(p));
	  }
     }   
   
   evas_object_image_data_set(etk_image_evas_object_get(im), data);
   evas_object_image_data_update_add(etk_image_evas_object_get(im), 0, 0, w, h);      
}

void
_ex_image_brightness2(Etk_Image *im, int brightness)
{
   DATA32 *data, *data2, *pixel;
   int           w, h;
   int           i, j;
   DATA8 a, r, g, b;
   int light_transform[256];
   char *edje;
   char *group;
   
   etk_image_edje_get(im, &edje, &group);
   if(edje != NULL)
     {
	free(edje);
	free(group);
	return;
     }
   
   D(("brightness = %d\n", brightness));
   for(i=0; i<256; i++){
      light_transform[i] = i + brightness;
      if(light_transform[i] > 255)
	light_transform[i] = 255;
      if(light_transform[i] < 0)
	light_transform[i] = 0;
   }   
   
   etk_image_size_get(im, &w, &h);
   data = evas_object_image_data_get(etk_image_evas_object_get(im), ETK_TRUE);
   data2 = malloc(w * h * sizeof(DATA32));
   memcpy(data2, data, w * h * sizeof(DATA32));
   
   for(i = 0; i < w - 1; i++) {
      for(j = 0; j < h - 1; j++) {
	 pixel = &data[i + (w * j)];
	 a = (*pixel >> 24) &0xff;
	 r = (*pixel >> 16) &0xff;
	 g = (*pixel >> 8) &0xff;
	 b = (*pixel) & 0xff;
	 
	 pixel = &data2[i + (w * j)];
	 *pixel |= (light_transform[r] << 16);
	 *pixel |= (light_transform[g] << 8);
	 *pixel |= (light_transform[b]);
      }
   }
   
   evas_object_image_data_set(etk_image_evas_object_get(im), data2);
   evas_object_image_data_update_add(etk_image_evas_object_get(im), 0, 0, w, h);      
}

void
_ex_image_contrast(Etk_Image *im, int contrast)
{
   
}

void
_ex_image_wallpaper_set(Etk_Image *im)
{
   pid_t pid;
   const char *file;
   char *filename;
   const char *dir;
   const char *filenoext;
   char *esetroot;
   char esetroot_opt[] = "-s";
#if HAVE_ENGRAVE
   char *edj_file;
   int w, h;
   Engrave_File *edj;
   Engrave_Image *image;
   Engrave_Group *grp;
   Engrave_Part *part;
   Engrave_Part_State *ps;
#endif   

   etk_image_file_get(im, &filename, NULL);
   pid = fork();
   if(!pid)
     {	
#if HAVE_E   
	if (!ecore_file_app_installed("enlightenment_remote"))
	  goto PSEUDO;	  
	/* make sure we got a file name */	 
          
	if (!filename) exit(0);
	if(strlen(filename) <= 4) exit(0);
	
	file = ecore_file_file_get(filename);
	dir = ecore_file_dir_get(filename);
	
	filenoext = _ex_file_strip_extention(filename);
	filenoext = ecore_file_file_get(filenoext);

	D(("Setting bg: dir: %s \tfile: %s\n", dir, file));

	/* 
	 * FIXME
	 *
	 * This doesnt set the bg if we choose another dir
	 * after having choosen the picture itself
	 *
	 */
	
	if (strcmp(filename + strlen(filename) - 4, ".edj") == 0) {
	   int w, h, num;
	   char static_bg[PATH_MAX];
	   char esetroot_s[PATH_MAX*2];
	   char e_bg_set[PATH_MAX*2];	   
	   char filename_s[PATH_MAX];
	   Ecore_X_Window *roots = NULL;

	   if (!ecore_x_init(NULL))
	     exit(0);
	   
	   num = 0;
	   roots = ecore_x_window_root_list(&num);
	   ecore_x_window_size_get(roots[0], &w, &h);
	   snprintf(filename_s, PATH_MAX, "/tmp/%s.png", filenoext);
	   snprintf(static_bg, PATH_MAX, "edje_thumb %s e/desktop/background %s -g %dx%d -og %dx%d", filename, filename_s, w, h, w, h);
	   snprintf(e_bg_set, PATH_MAX, "enlightenment_remote -default-bg-set %s", filename);
	   snprintf(esetroot_s, PATH_MAX, "Esetroot %s %s ", esetroot_opt, 
		 filename_s);	 
	   D(("Filename_s: %s\n", filename_s));
	   system(static_bg);
	   system(e_bg_set);	   
	   system(esetroot_s);
	   exit(0);
	}
   
#if HAVE_ENGRAVE   
	/* Set up edj path */
	edj_file = malloc(strlen(getenv("HOME")) +  strlen("/.e/e/backgrounds/")
			  + strlen(filenoext) + strlen(".edj") + 1);
	strcpy(edj_file, getenv("HOME"));
	strcat(edj_file, "/.e/e/backgrounds/");
	strcat(edj_file, filenoext);
	strcat(edj_file, ".edj");

	D(("Edje file: %s  FILENOEXT: %s\n", edj_file, filenoext));
	
	/* Determine image width / height */
	  {
	     Evas_Object *o;
	     Ecore_Evas *ee;
	     
	     ee = ecore_evas_buffer_new(0, 0);
	     o = evas_object_image_add(ecore_evas_get(ee));
	     evas_object_image_file_set(o, filename, NULL);
	     evas_object_image_size_get(o, &w, &h);
	     evas_object_del(o);
	     ecore_evas_free(ee);
	  }
	
	/* create the .edj */
	edj = engrave_file_new();
	engrave_file_image_dir_set(edj, dir);
	image = engrave_image_new(file, ENGRAVE_IMAGE_TYPE_COMP, 0);
	engrave_file_image_add(edj, image);
	
	grp = engrave_group_new();
	engrave_group_name_set(grp, "e/desktop/background");
	engrave_file_group_add(edj, grp);
	
	part = engrave_part_new(ENGRAVE_PART_TYPE_IMAGE);
	engrave_part_name_set(part, "background_image");
	engrave_group_part_add(grp, part);
	
	ps = engrave_part_state_new();
	engrave_part_state_name_set(ps, "default", 0.0);
	engrave_part_state_image_normal_set(ps, image);
	
	engrave_part_state_max_size_set(ps, w, h);
	
	/* for now, dont differentiate, just center */
# if 0   
	switch(e_bg_type) {
	 case E_BG_CENTER:
	   engrave_part_state_max_size_set(ps, w, h);
	   break;
	   
	 case E_BG_SCALE:
	   break;
	   
	 case E_BG_FIT:
	   break;
	   
	 case E_BG_TILE:
	   /* FIXME: This is a temp until dj2 fixes engrave */
	   //engrave_part_state_max_size_set(ps, w, h);
	   engrave_part_state_fill_size_relative_set(ps, 0.0, 0.0);
	   engrave_part_state_fill_size_offset_set(ps, w, h);
	   break;
	   
	 default:
	   /* FIXME: This is a temp until dj2 fixes engrave */
	   //engrave_part_state_max_size_set(ps, w, h);
	   engrave_part_state_fill_size_relative_set(ps, 0.0, 0.0);
	   engrave_part_state_fill_size_offset_set(ps, w, h);
	   break;
	}
# endif
	
	engrave_part_state_add(part, ps);
	
	engrave_edj_output(edj, edj_file);
	engrave_file_free(edj);   
	
	/* set the background */
	  {
	     char e_bg_set[PATH_MAX*2];
	     
	     snprintf(e_bg_set, PATH_MAX, "enlightenment_remote -default-bg-set %s", 
		   edj_file);
	     system(e_bg_set);
	  }
	
#endif

#endif
	
PSEUDO:
	
	/* If we're using pseudo-trans for eterm, then this will help */
	esetroot = malloc(strlen("Esetroot ") + strlen(esetroot_opt) + strlen(filename) + 2);
	snprintf(esetroot, strlen("Esetroot ") + strlen(esetroot_opt) + strlen(filename) + 2,
		 "Esetroot %s %s", esetroot_opt, filename);
	system(esetroot);
	E_FREE(esetroot);
	exit(0);
     }
}

Etk_Bool
_ex_image_is_favorite(Exhibit *e)
{
   Etk_Tree_Row *r;
   char         *icol_string;
   char          path[PATH_MAX];
      
   r = etk_tree_selected_row_get(ETK_TREE(e->cur_tab->itree));
   if(!r) return ETK_FALSE;
   
   etk_tree_row_fields_get(r, 
			    etk_tree_nth_col_get(ETK_TREE(e->cur_tab->itree),
						  0), 
			    NULL, NULL, &icol_string, 
			    NULL);
   snprintf(path, sizeof(path), "%s/%s", e->options->fav_path, icol_string);
   if(ecore_file_exists(path))     
     return ETK_TRUE;
   return ETK_FALSE;
}

Etk_Tree_Row *
_ex_image_find_row_from_file(Ex_Tab *tab, const char *filename)
{
   Etk_Tree_Row *iter;
   char *icol_string;
   char *base;
   
   for (iter = etk_tree_first_row_get(ETK_TREE(tab->itree));
	iter;
	iter = etk_tree_row_walk_next(iter, ETK_TRUE))
     {
	etk_tree_row_fields_get(
			  iter,
			  etk_tree_nth_col_get(ETK_TREE(tab->itree), 0),
			  NULL,
			  NULL,
			  &icol_string,
			  NULL);
	base = basename(filename);
	if (!strcmp(icol_string, base))
	  {
	     return iter;
	  }
     }
	
   return NULL;
}
