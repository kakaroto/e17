#include "exhibit.h"

#define EX_MENU_ITEM_GET_RETURN(o) \
   Etk_Menu_Item *item; \
   if (!(item = ETK_MENU_ITEM(o))) \
     return; 

char *ex_images[] = 
{
   "fit_to_window",
   "one_to_one",
   "zoom_in",
   "zoom_out"
};

Etk_Widget *
_ex_menu_item_new(Ex_Menu_Item_Type item_type, const char *label,
		  int stock_id, Etk_Menu_Shell *menu_shell,
		  void *func(Etk_Object *obj, void *data),
		  void *data)
{
   Etk_Widget *menu_item = NULL;
   
   switch (item_type)
     {
      case EX_MENU_ITEM_NORMAL:
	menu_item = etk_menu_item_new_with_label(label);
	break;
      case EX_MENU_ITEM_SEPERATOR:
	menu_item = etk_menu_separator_new();
	break;
      default:
	return NULL;
     }
   
   if (stock_id >= 0)
     {
	Etk_Widget *image;

	image = etk_image_new_from_stock(stock_id);
	etk_menu_item_image_set(ETK_MENU_ITEM(menu_item), ETK_IMAGE(image));
     }
   else
     {	
	stock_id = abs(stock_id) - 1;
	if(stock_id >= 0 && stock_id < sizeof(ex_images) / sizeof(char*))
	  {
	     Etk_Widget *image;
	     
	     image = etk_image_new_from_edje(PACKAGE_DATA_DIR
					     "/images/images.edj",
					     ex_images[stock_id]);
	     etk_menu_item_image_set(ETK_MENU_ITEM(menu_item), ETK_IMAGE(image));
	  }
     }
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));
   
   if(func)
     etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(func), data);
   
   return menu_item;
}

void
_ex_menu_new_window_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   printf("new window\n");
}

void
_ex_menu_save_image_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e;
   Etk_Tree_Row *r;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   r = etk_tree_selected_row_get(ETK_TREE(e->itree));
   if(!r) return;
   _ex_image_save(ETK_IMAGE(e->image));
}

void
_ex_menu_save_image_as_cb(Etk_Object *obj, void *data)
{   
   EX_MENU_ITEM_GET_RETURN(obj);
   printf("save image as\n");
}

void
_ex_menu_search_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   printf("search\n");
}

void
_ex_menu_rename_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   printf("rename\n");
}

void
_ex_menu_delete_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   printf("delete\n");
}

void
_ex_menu_close_window_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   printf("close window\n");
}

void
_ex_menu_quit_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   printf("quit\n");
}

void
_ex_menu_run_in_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   printf("run in\n");
}

void
_ex_menu_rot_clockwise_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e;
   Etk_Tree_Row *r;
   int           w, h;
   char         *size[30];
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   r = etk_tree_selected_row_get(ETK_TREE(e->itree));
   if(!r) return;   
   _ex_image_flip_diagonal(ETK_IMAGE(e->image), 1);
   etk_image_size_get(ETK_IMAGE(e->image), &w, &h);
   snprintf(size, sizeof(size), "( %d x %d )", w, h);
   etk_statusbar_pop(ETK_STATUSBAR(e->statusbar[1]), 0);
   etk_statusbar_push(ETK_STATUSBAR(e->statusbar[1]), size, 0);
}

void
_ex_menu_rot_counter_clockwise_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e;
   Etk_Tree_Row *r;   
   int           w, h;
   char         *size[30];   
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   r = etk_tree_selected_row_get(ETK_TREE(e->itree));
   if(!r) return;   
   _ex_image_flip_diagonal(ETK_IMAGE(e->image), 2);
   etk_image_size_get(ETK_IMAGE(e->image), &w, &h);
   snprintf(size, sizeof(size), "( %d x %d )", w, h);
   etk_statusbar_pop(ETK_STATUSBAR(e->statusbar[1]), 0);
   etk_statusbar_push(ETK_STATUSBAR(e->statusbar[1]), size, 0);   
}

void
_ex_menu_flip_horizontal_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e;
   Etk_Tree_Row *r;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   r = etk_tree_selected_row_get(ETK_TREE(e->itree));
   if(!r) return;   
   _ex_image_flip_horizontal(ETK_IMAGE(e->image));
}

void
_ex_menu_flip_vertical_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e;
   Etk_Tree_Row *r;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   r = etk_tree_selected_row_get(ETK_TREE(e->itree));
   if(!r) return;   
   _ex_image_flip_vertical(ETK_IMAGE(e->image));   
}

void
_ex_menu_blur_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e;
   Etk_Tree_Row *r;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   r = etk_tree_selected_row_get(ETK_TREE(e->itree));
   if(!r) return;   
   _ex_image_blur(ETK_IMAGE(e->image));   
}

void
_ex_menu_sharpen_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e;
   Etk_Tree_Row *r;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   r = etk_tree_selected_row_get(ETK_TREE(e->itree));
   if(!r) return;   
   _ex_image_sharpen(ETK_IMAGE(e->image));   
}

void
_ex_menu_brighten_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e;
   Etk_Tree_Row *r;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   r = etk_tree_selected_row_get(ETK_TREE(e->itree));
   if(!r) return;   
   
   e->brightness += 10;
   if(e->brightness > 255)
     e->brightness = 255;
   _ex_image_brightness(ETK_IMAGE(e->image), e->brightness);
}

void
_ex_menu_darken_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e;
   Etk_Tree_Row *r;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   r = etk_tree_selected_row_get(ETK_TREE(e->itree));
   if(!r) return;   
   
   e->brightness -= 10;
   if(e->brightness < 0)
     e->brightness = 0;   
   _ex_image_brightness(ETK_IMAGE(e->image), e->brightness);
}

void
_ex_menu_set_wallpaper_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   printf("set as wallpaper\n");
}

void
_ex_menu_zoom_in_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e;
   Etk_Tree_Row *r;   
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   r = etk_tree_selected_row_get(ETK_TREE(e->itree));
   if(!r) return;
   
   if(e->zoom == ZOOM_MAX)
     e->zoom = ZOOM_MAX;
   else
     e->zoom += 2;
   
   _ex_image_zoom(ETK_IMAGE(e->image), e->zoom);
   _ex_main_statusbar_zoom_update(e);   
}

void
_ex_menu_zoom_out_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e;
   Etk_Tree_Row *r;   
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   r = etk_tree_selected_row_get(ETK_TREE(e->itree));
   if(!r) return;
   
   if(e->zoom <= ZOOM_MIN)
     e->zoom = ZOOM_MIN;
   else
     e->zoom -= 2;
   
   _ex_image_zoom(ETK_IMAGE(e->image), e->zoom);
   _ex_main_statusbar_zoom_update(e);
}

void
_ex_menu_zoom_one_to_one_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e;
   Etk_Tree_Row *r;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   r = etk_tree_selected_row_get(ETK_TREE(e->itree));
   if(!r) return;
   
   e->zoom = 0;
   
   _ex_image_zoom(ETK_IMAGE(e->image), e->zoom);
   _ex_main_statusbar_zoom_update(e);     
}

void
_ex_menu_fit_to_window_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   printf("fit to window\n");
}

void
_ex_menu_toggle_slideshow_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   printf("toggle slideshow\n");
}

void
_ex_menu_refresh_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   printf("refresh\n");
}

void
_ex_menu_release_notes_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   printf("release notes\n");
}

void
_ex_menu_about_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   printf("about\n");
}
