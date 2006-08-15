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

void 
_ex_menu_build_run_menu(Etk_Widget *submenu) 
{
   Etk_Widget *w;
   
   if (!submenu)
     w = e->submenu;
   else
     w = submenu;

   if (e->options->app1 && e->options->app1_cmd)
     e->app1_menu = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _(e->options->app1), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(w), ETK_CALLBACK(_ex_menu_run_in_cb), e->options->app1_cmd);
   if (e->options->app2 && e->options->app2_cmd)
     e->app2_menu = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _(e->options->app2), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(w), ETK_CALLBACK(_ex_menu_run_in_cb), e->options->app2_cmd);
   if (e->options->app3 && e->options->app3_cmd)
     e->app3_menu = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _(e->options->app3), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(w), ETK_CALLBACK(_ex_menu_run_in_cb), e->options->app3_cmd);
   if (e->options->app4 && e->options->app4_cmd)
     e->app4_menu = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _(e->options->app4), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(w), ETK_CALLBACK(_ex_menu_run_in_cb), e->options->app4_cmd);
   
}

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
_ex_menu_rename_cb(Etk_Object *obj, void *data)
{
   Exhibit      *e = data;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   if (!e->cur_tab->image_loaded)
     return;
   
   _ex_image_rename();
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
   const char *cmd = data;
   EX_MENU_ITEM_GET_RETURN(obj);

   if (!e->cur_tab->image_loaded)
     return;

   _ex_image_run(cmd);
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

   etk_tree_clear(ETK_TREE(e->cur_tab->itree));
   etk_tree_clear(ETK_TREE(e->cur_tab->dtree));
   _ex_main_populate_files(e, NULL);
   
   if (!e->cur_tab->image_loaded)
     return;

   _ex_image_refresh();
}

void
_ex_menu_comments_cb(Etk_Object *obj, void *data)
{   
   Exhibit      *e = data;
   EX_MENU_ITEM_GET_RETURN(obj);
   
   if (!e->cur_tab->image_loaded)
     return;

   if(!e->cur_tab->comment.visible)
     {
	e->options->comments_visible = EX_DEFAULT_COMMENTS_VISIBLE;
	_ex_comment_show(e);
	_ex_comment_load(e);
	
	if(e->opt_dialog) 
	  {
	     /* If the options dialog is open, toggle the checkbox  */
	     if (!etk_toggle_button_active_get
		   (ETK_TOGGLE_BUTTON(e->opt_dialog->comments_visible)))
	       etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(e->opt_dialog->comments_visible));
	  }
     }
   else
     {
	e->options->comments_visible = EX_DEFAULT_COMMENTS_HIDDEN;
	_ex_comment_hide(e);
	
	if(e->opt_dialog) 
	  {
	     /* If the options dialog is open, toggle the checkbox  */
	     if (etk_toggle_button_active_get
		   (ETK_TOGGLE_BUTTON(e->opt_dialog->comments_visible)))
	       etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(e->opt_dialog->comments_visible));
	  }
     }

   /* Save this as settings since we want "remember state" for this */
   _ex_options_save(e);
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
_ex_menu_about_cb(Etk_Object *obj, void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *frame;
   Etk_Widget *vbox;
   Etk_Widget *desctext; 
   Etk_Widget *abouttext;
   Etk_Widget *helptext; 
   
   EX_MENU_ITEM_GET_RETURN(obj);

   if(win)
     {
	etk_widget_show_all(win);
	return;
     }

   win = etk_dialog_new();
   etk_window_title_set(ETK_WINDOW(win), "About Exhibit");
   etk_signal_connect_swapped("delete_event", ETK_OBJECT(win),
	 ETK_CALLBACK(etk_window_hide_on_delete), win);
   etk_signal_connect_swapped("response", ETK_OBJECT(win),
	 ETK_CALLBACK(etk_window_hide_on_delete), win);
   etk_widget_size_request_set(win, 290, 405);
   
   vbox = etk_vbox_new(ETK_FALSE, 0);
   frame = etk_frame_new("What is Exhibit?");
   desctext = etk_text_view_new();
   etk_widget_size_request_set(desctext, -1, 150);
   etk_object_properties_set(ETK_OBJECT(desctext),
	 "focusable", ETK_FALSE, NULL);
   etk_textblock_text_set(ETK_TEXT_VIEW(desctext)->textblock,
	 "<b>Exhibit</b> is an imageviewer that uses Etk as its toolkit. "
	 "Exhibit supports image previews for image types supported by "
	 "Evas and allows for directory changing using a point "
	 "and click interface or a text input box with "
	 "tab autocompletion support.\n\n"
	 "<p align=\"center\"><style effect=glow color1=#fa14 color2=#fe87>"
	 "<b>"VERSION"</b>"
	 "</style></p>",
	 ETK_TRUE);
   etk_container_add(ETK_CONTAINER(frame), desctext);
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 0);

   frame = etk_frame_new("Authors");
   abouttext = etk_text_view_new();
   etk_widget_size_request_set(abouttext, -1, 75);
   etk_object_properties_set(ETK_OBJECT(abouttext),
	 "focusable", ETK_FALSE, NULL);
   etk_textblock_text_set(ETK_TEXT_VIEW(abouttext)->textblock,
	 "<b>Code:</b>\n"
	 "Hisham '<b>CodeWarrior</b>' Mardam Bey\n"
	 "Martin '<b>balony</b>' Sarajervi\n"
	 "Simon '<b>MoOm</b>' Treny",
	 ETK_TRUE);
   etk_container_add(ETK_CONTAINER(frame), abouttext);
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   frame = etk_frame_new("Common shortcuts");
   helptext = etk_text_view_new();
   etk_widget_size_request_set(helptext, -1,100);
   etk_object_properties_set(ETK_OBJECT(helptext),
	 "focusable", ETK_FALSE, NULL);
   etk_textblock_text_set(ETK_TEXT_VIEW(helptext)->textblock,
	 "<b>control-t:</b> create a new tab\n"
	 "<b>control-w:</b> close active tab\n"
	 "<b>control-d:</b> add to favorites\n"
	 "<b>control-x:</b> toggle view\n"
	 "<b>control-q:</b> exit program\n"
	 "<b>control-s:</b> toggle slideshow\n",
	 ETK_TRUE);
   etk_container_add(ETK_CONTAINER(frame), helptext);
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_dialog_pack_in_main_area(ETK_DIALOG(win), vbox, ETK_FALSE, ETK_FALSE,
	 0, ETK_FALSE);
   etk_dialog_button_add(ETK_DIALOG(win), "Close", ETK_RESPONSE_CLOSE);
   etk_container_border_width_set(ETK_CONTAINER(win), 7);
   etk_widget_show_all(win);

   etk_textblock_object_cursor_visible_set(ETK_TEXT_VIEW(abouttext)->textblock_object,
	 ETK_FALSE);
   etk_textblock_object_cursor_visible_set(ETK_TEXT_VIEW(desctext)->textblock_object,
	 ETK_FALSE);
   etk_textblock_object_cursor_visible_set(ETK_TEXT_VIEW(helptext)->textblock_object,
	 ETK_FALSE);

}
