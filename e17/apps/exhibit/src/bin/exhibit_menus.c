/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "exhibit.h"
#include "exhibit_main.h"
#include "exhibit_image.h"

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
		  Etk_Signal_Callback_Function callback,
		  void *data)
{
   Etk_Widget *menu_item = NULL;
   
   switch (item_type)
     {
      case EX_MENU_ITEM_NORMAL:
	menu_item = etk_menu_item_image_new_with_label(label);
	break;
      case EX_MENU_ITEM_SEPERATOR:
	menu_item = etk_menu_item_separator_new();
	break;
      default:
	return NULL;
     }
   
   if (stock_id > ETK_STOCK_NO_STOCK)
     {
	Etk_Widget *image;

	image = etk_image_new_from_stock(stock_id, ETK_STOCK_SMALL);
	etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
     }
   else if (stock_id < ETK_STOCK_NO_STOCK)
     {	
	stock_id = abs(stock_id) - 1;
	if(stock_id >= 0 && stock_id < sizeof(ex_images) / sizeof(char*))
	  {
	     Etk_Widget *image;
	     
	     image = etk_image_new_from_edje(PACKAGE_DATA_DIR
					     "/images/images.edj",
					     ex_images[stock_id]);
	     etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
	  }
     }
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));
   
   if(callback)
     etk_signal_connect("activated", ETK_OBJECT(menu_item), callback, data);
   
   return menu_item;
}

void
_ex_menu_new_window_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   
   D(("new window\n"));
}

void
_ex_menu_new_tab_cb(Etk_Object *obj, void *data)
{
   Ex_Tab *tab;

   EX_MENU_ITEM_GET_RETURN(obj);

   tab = _ex_tab_new(e, e->cur_tab->cur_path);
   _ex_main_window_tab_append(e, tab);
   _ex_main_populate_files(e, NULL);
}

void
_ex_menu_delete_tab_cb(Etk_Object *obj, void *data)
{
   if(evas_list_count(e->tabs) <= 1)
     {
	 _ex_main_dialog_show("No tabs open! Create new with Ctrl^t", 
	       ETK_MESSAGE_DIALOG_INFO);
	return;
     }
     
   EX_MENU_ITEM_GET_RETURN(obj);

   _ex_main_window_tab_remove(e->cur_tab);
   _ex_tab_delete(e->cur_tab);
}

void
_ex_menu_save_image_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e;
   Etk_Tree_Row *r;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   r = etk_tree_selected_row_get(ETK_TREE(e->cur_tab->itree));

   if(!r) return;
   _ex_image_save(ETK_IMAGE(e->cur_tab->image));
}

void
_ex_menu_save_image_as_cb(Etk_Object *obj, void *data)
{   
   Exhibit      *e = data;
   EX_MENU_ITEM_GET_RETURN(obj);

   if (!e->cur_tab->image_loaded)
     return;

   _ex_image_save_as(e);
}

void
_ex_menu_search_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   D(("search\n"));
}

void
_ex_menu_rename_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   D(("rename\n"));
}

void
_ex_menu_delete_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e = data;
   EX_MENU_ITEM_GET_RETURN(obj);

   if (!e->cur_tab->image_loaded)
     return;

   _ex_image_delete(e);
}

void
_ex_menu_options_cb(Etk_Object *obj, void *data)
{
   Exhibit *e = data;
   
   EX_MENU_ITEM_GET_RETURN(obj);
   _ex_options_window_show(e);
}

void
_ex_menu_close_window_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   etk_main_quit();   
}

void
_ex_menu_quit_cb(Etk_Object *obj, void *data)
{
   Exhibit *e;

   EX_MENU_ITEM_GET_RETURN(obj);

   e = (Exhibit *)data;
   if (e)     
     free(e);     

   etk_main_quit();   
}

void
_ex_menu_run_in_cb(Etk_Object *obj, void *data)
{
   const char *app;
   EX_MENU_ITEM_GET_RETURN(obj);

   if (!e->cur_tab->image_loaded)
     return;

   app = etk_menu_item_label_get(ETK_MENU_ITEM(obj));
   _ex_image_run(app);
}

void
_ex_menu_undo_cb(Etk_Object *obj, void *data)
{
   Exhibit *e = data;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   if (!e->cur_tab->image_loaded)
     return;

   _ex_image_undo(ETK_IMAGE(e->cur_tab->image));   
}


void
_ex_menu_rot_clockwise_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e = data;
   int           w, h;
   char          size[30];
   EX_MENU_ITEM_GET_RETURN(obj);
   
   if (!e->cur_tab->image_loaded)
     return;

   _ex_image_flip_diagonal(ETK_IMAGE(e->cur_tab->image), 1);
   etk_image_size_get(ETK_IMAGE(e->cur_tab->image), &w, &h);
   snprintf(size, sizeof(size), "( %d x %d )", w, h);
   etk_statusbar_pop(ETK_STATUSBAR(e->statusbar[1]), 0);
   etk_statusbar_push(ETK_STATUSBAR(e->statusbar[1]), size, 0);
}

void
_ex_menu_rot_counter_clockwise_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e = data;
   int           w, h;
   char          size[30];   
   EX_MENU_ITEM_GET_RETURN(obj);
   
   if (!e->cur_tab->image_loaded)
     return;
   
   _ex_image_flip_diagonal(ETK_IMAGE(e->cur_tab->image), 2);
   etk_image_size_get(ETK_IMAGE(e->cur_tab->image), &w, &h);
   snprintf(size, sizeof(size), "( %d x %d )", w, h);
   etk_statusbar_pop(ETK_STATUSBAR(e->statusbar[1]), 0);
   etk_statusbar_push(ETK_STATUSBAR(e->statusbar[1]), size, 0);   
}

void
_ex_menu_flip_horizontal_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e = data;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   if (!e->cur_tab->image_loaded)
     return;

   _ex_image_flip_horizontal(ETK_IMAGE(e->cur_tab->image));
}

void
_ex_menu_flip_vertical_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e = data;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   if (!e->cur_tab->image_loaded)
     return;

   _ex_image_flip_vertical(ETK_IMAGE(e->cur_tab->image));   
}

void
_ex_menu_blur_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e = data;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   if (!e->cur_tab->image_loaded)
     return;

   _ex_image_blur(ETK_IMAGE(e->cur_tab->image));   
}

void
_ex_menu_sharpen_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e = data;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   if (!e->cur_tab->image_loaded)
     return;

   _ex_image_sharpen(ETK_IMAGE(e->cur_tab->image));   
}

void
_ex_menu_brighten_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e = data;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   if (!e->cur_tab->image_loaded)
     return;

   e->brightness = e->options->brighten_thresh;
   
   /* What is this for? FIXME -- Balony */
   e->brightness += 10; 
   
   if(e->brightness > 255)
     e->brightness = 255;

   D(("Using brightness %d\n", e->brightness));
   _ex_image_brightness(ETK_IMAGE(e->cur_tab->image), e->brightness);
}

void
_ex_menu_darken_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e = data;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   if (!e->cur_tab->image_loaded)
     return;
   
   e->brightness -= 10;
   if(e->brightness < 0)
     e->brightness = 0;   
   _ex_image_brightness(ETK_IMAGE(e->cur_tab->image), e->brightness);
}

void
_ex_menu_set_wallpaper_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e = data;
   EX_MENU_ITEM_GET_RETURN(obj);

   if (!e->cur_tab->image_loaded)
     return;
   
   _ex_image_wallpaper_set(ETK_IMAGE(e->cur_tab->image));
}

void
_ex_menu_zoom_in_cb(Etk_Object *obj, void *data)
{
   _ex_tab_current_zoom_in(data);  
}

void
_ex_menu_zoom_out_cb(Etk_Object *obj, void *data)
{
   _ex_tab_current_zoom_out(data);
}

void
_ex_menu_zoom_one_to_one_cb(Etk_Object *obj, void *data)
{
   _ex_tab_current_zoom_one_to_one(data);    
}

void
_ex_menu_fit_to_window_cb(Etk_Object *obj, void *data)
{
   _ex_tab_current_fit_to_window(data);
}

void
_ex_menu_toggle_slideshow_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e;   
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;   
   _ex_main_window_slideshow_toggle(e);
}

void
_ex_menu_refresh_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   D(("refresh\n"));
}

void
_ex_menu_comments_cb(Etk_Object *obj, void *data)
{   
   Exhibit      *e;
   Etk_Tree_Row *r;
   char         *icol_string;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   r = etk_tree_selected_row_get(ETK_TREE(e->cur_tab->itree));
   if(!r) return;
   
   etk_tree_row_fields_get(r, etk_tree_nth_col_get(ETK_TREE(e->cur_tab->itree), 0), NULL, &icol_string, etk_tree_nth_col_get(ETK_TREE(e->cur_tab->itree), 1),NULL);   
   
   if(!e->cur_tab->comment.visible)
     {
	_ex_comment_show(e);
	_ex_comment_load(e);	  
     }
   else
     _ex_comment_hide(e);
}

void
_ex_menu_add_to_fav_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e;
   Etk_Tree_Row *r;
   char         *icol_string;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   r = etk_tree_selected_row_get(ETK_TREE(e->cur_tab->itree));
      if(!r) return;
   
   etk_tree_row_fields_get(r, etk_tree_nth_col_get(ETK_TREE(e->cur_tab->itree), 0), NULL, &icol_string, etk_tree_nth_col_get(ETK_TREE(e->cur_tab->itree), 1),NULL);
   _ex_favorites_add(e, icol_string);
   //free(icol_string);
}

void
_ex_menu_remove_from_fav_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e;
   Etk_Tree_Row *r;
   char         *icol_string;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   r = etk_tree_selected_row_get(ETK_TREE(e->cur_tab->itree));
      if(!r) return;
   
   etk_tree_row_fields_get(r, etk_tree_nth_col_get(ETK_TREE(e->cur_tab->itree), 0), NULL, &icol_string, etk_tree_nth_col_get(ETK_TREE(e->cur_tab->itree), 1),NULL);
   _ex_favorites_del(e, icol_string);
   //free(icol_string);
}

void
_ex_menu_go_to_fav_cb(Etk_Object *obj, void *data)
{    
   Exhibit      *e;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   e = data;
   
   _ex_slideshow_stop(e);
   E_FREE(e->cur_tab->dir);
   e->cur_tab->dir = strdup(e->options->fav_path);
   etk_tree_clear(ETK_TREE(e->cur_tab->itree));
   etk_tree_clear(ETK_TREE(e->cur_tab->dtree));
   _ex_main_populate_files(e, NULL);   
}

void
_ex_menu_release_notes_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   D(("release notes\n"));
}

void
_ex_menu_about_cb(Etk_Object *obj, void *data)
{
   EX_MENU_ITEM_GET_RETURN(obj);
   D(("about\n"));
}
