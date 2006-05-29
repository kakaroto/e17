#include "exhibit.h"

#define EX_DND_MAX_NUM 25
#define EX_DND_COL_NUM 5

static void _ex_tab_tree_drag_begin_cb(Etk_Object *object, void *data);

Ex_Tab *
_ex_tab_new(Exhibit *e, char *dir)
{
   Ex_Tab *tab;
   Etk_Tree_Model *imodel;
   char *file;
   
   file = NULL;
   tab = calloc(1, sizeof(Ex_Tab));   
   tab->num = evas_list_count(e->tabs);
   tab->dirs = NULL;
   tab->images = NULL;
   tab->e = e;
   tab->fit_window = ETK_FALSE;
   
   tab->dtree = etk_tree_new();
   etk_widget_size_request_set(tab->dtree, 180, 120);
   etk_signal_connect("row_selected", ETK_OBJECT(tab->dtree), ETK_CALLBACK(_ex_main_dtree_item_clicked_cb), e);
   tab->dcol = etk_tree_col_new(ETK_TREE(tab->dtree), "Directories", etk_tree_model_icon_text_new(ETK_TREE(tab->dtree), ETK_TREE_FROM_EDJE), 10);
   etk_tree_headers_visible_set(ETK_TREE(tab->dtree), 0);
   etk_tree_build(ETK_TREE(tab->dtree));

   tab->itree = etk_tree_new();
   etk_widget_dnd_source_set(ETK_WIDGET(tab->itree), ETK_TRUE);
   etk_signal_connect("drag_begin", ETK_OBJECT(tab->itree), ETK_CALLBACK(_ex_tab_tree_drag_begin_cb), tab);
   etk_widget_size_request_set(tab->itree, 180, 120);
   etk_tree_multiple_select_set(ETK_TREE(tab->itree), ETK_TRUE);
   etk_signal_connect("row_selected", ETK_OBJECT(tab->itree), ETK_CALLBACK(_ex_main_itree_item_clicked_cb), e);
   etk_signal_connect("key_down", ETK_OBJECT(tab->itree), ETK_CALLBACK(_ex_main_itree_key_down_cb), e);
   imodel = etk_tree_model_icon_text_new(ETK_TREE(tab->itree), ETK_TREE_FROM_FILE);
   etk_tree_model_icon_text_icon_width_set(imodel, 80);
   tab->icol = etk_tree_col_new(ETK_TREE(tab->itree), "Files", imodel, 10);
   etk_tree_headers_visible_set(ETK_TREE(tab->itree), 0);
   etk_tree_row_height_set(ETK_TREE(tab->itree), 60);
   etk_tree_build(ETK_TREE(tab->itree));

   if(dir)
     tab->dir = strdup(dir);
   else
     tab->dir = strdup(".");

   tab->alignment = etk_alignment_new(0.5, 0.5, 0.0, 0.0);   
   
   tab->image = etk_image_new();
   etk_widget_theme_file_set(tab->image, PACKAGE_DATA_DIR"/images/images.edj");
   etk_widget_theme_group_set(tab->image, "image_bg");
   etk_signal_connect("mouse_down", ETK_OBJECT(tab->image), ETK_CALLBACK(_ex_image_mouse_down), e);
   etk_signal_connect("mouse_up", ETK_OBJECT(tab->image), ETK_CALLBACK(_ex_image_mouse_up), e);
   etk_signal_connect("mouse_move", ETK_OBJECT(tab->image), ETK_CALLBACK(_ex_image_mouse_move), e);
   etk_signal_connect("mouse_wheel", ETK_OBJECT(tab->image), ETK_CALLBACK(_ex_image_mouse_wheel), e);
   etk_image_keep_aspect_set(ETK_IMAGE(tab->image), ETK_TRUE);
   etk_container_add(ETK_CONTAINER(tab->alignment), tab->image);   
      
   tab->scrolled_view = etk_scrolled_view_new();
   etk_scrolled_view_add_with_viewport(ETK_SCROLLED_VIEW(tab->scrolled_view), tab->alignment);
      
   return tab;
}

void
_ex_tab_dir_set(Ex_Tab *tab, char *path)
{
   
}

void
_ex_tab_delete(Ex_Tab *tab)
{
   
}

void
_ex_tab_select(Ex_Tab *tab)
{   
   chdir(tab->cur_path);

   if(tab->fit_window)
     etk_notebook_page_child_set(ETK_NOTEBOOK(tab->e->notebook), tab->num, tab->alignment);
   
   etk_table_attach(ETK_TABLE(tab->e->table), tab->dtree,
		    0, 3, 3, 3,
		    0, 0, ETK_FILL_POLICY_VEXPAND|ETK_FILL_POLICY_VFILL|ETK_FILL_POLICY_HFILL);
   etk_widget_show(tab->dtree);
   
   etk_paned_child2_set(ETK_PANED(tab->e->vpaned), tab->itree, ETK_TRUE);
   etk_widget_show(tab->itree);
      
   etk_widget_show(tab->image);
   etk_widget_show(tab->alignment);   
   etk_widget_show(tab->scrolled_view);
      
   etk_widget_show_all(tab->e->win);
}

static void _ex_tab_tree_drag_begin_cb(Etk_Object *object, void *data)
{
   Ex_Tab       *tab;
   Etk_Tree     *tree;
   Etk_Tree_Row *row;
   Etk_Widget   *drag;
   Etk_Widget   *image;
   Evas_List    *rows;
   char *icol1_string;   
   char *icol2_string;
   char *drag_data;   
   const char **types;
   unsigned int num_types;

   tab = data;
   tree = ETK_TREE(object);
   drag = (ETK_WIDGET(tree))->drag;
   
   rows = etk_tree_selected_rows_get(tree);
   
   types = calloc(1, sizeof(char*));
   num_types = 1;
   types[0] = strdup("text/uri-list");   
   
   if(evas_list_count(rows) > 1)
     {
	Evas_List *ll;
	Etk_Widget *table;
	int i = 0, l = 0, r = 0, t = 0, b = 0, row_num;
		
	if(evas_list_count(rows) >= EX_DND_COL_NUM)
	  row_num = evas_list_count(rows) / EX_DND_COL_NUM;
	else
	  row_num = 1;
	
	table = etk_table_new(EX_DND_COL_NUM, row_num + 1, ETK_TRUE);
	drag_data = calloc(PATH_MAX * evas_list_count(rows), sizeof(char));
	for(ll = rows; ll; ll = ll->next)
	  {
	     char tmp[PATH_MAX];
	     
	     row = ll->data;
	     etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), &icol1_string, &icol2_string, etk_tree_nth_col_get(tree, 1),NULL);
	     snprintf(tmp, PATH_MAX * sizeof(char), "file://%s%s\r\n", tab->cur_path, icol2_string);
	     strncat(drag_data, tmp, PATH_MAX * evas_list_count(rows));
	     if(i <= EX_DND_MAX_NUM * EX_DND_MAX_NUM)
	       {
		  image = etk_image_new_from_file(icol1_string);
		  etk_image_keep_aspect_set(ETK_IMAGE(image), ETK_TRUE);
		  etk_widget_size_request_set(image, 48, 48);
		  etk_table_attach(ETK_TABLE(table), image, l, r, t, b, 3, 3,
				   ETK_FILL_POLICY_NONE);
		  
		  ++l; ++r;
		  
		  if(l == EX_DND_COL_NUM)
		    {
		       l = r = 0;
		       ++t; ++b;
		    }	     
	       }
	     ++i;
	  }
	
	etk_container_add(ETK_CONTAINER(drag), table);	
     }
   else
     {   
	row = etk_tree_selected_row_get(tree);      
	etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), &icol1_string, &icol2_string, etk_tree_nth_col_get(tree, 1),NULL);
	drag_data = calloc(PATH_MAX, sizeof(char));
	snprintf(drag_data, PATH_MAX * sizeof(char), "file://%s%s\r\n", tab->cur_path, icol2_string);
	image = etk_image_new_from_file(icol1_string);
	etk_image_keep_aspect_set(ETK_IMAGE(image), ETK_TRUE);
	etk_widget_size_request_set(image, 96, 96);
	etk_container_add(ETK_CONTAINER(drag), image);	
     }
   
   etk_drag_types_set(ETK_DRAG(drag), types, num_types);
   etk_drag_data_set(ETK_DRAG(drag), drag_data, strlen(drag_data) + 1);
}
