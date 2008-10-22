/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "exhibit.h"

#define EX_DND_MAX_NUM 25
#define EX_DND_COL_NUM 5

static void _ex_tab_tree_drag_begin_cb(Etk_Object *object, void *data);
static void _ex_tab_dtree_item_clicked_cb(Etk_Object *object, Etk_Tree_Row *row, void *event, void *data);
static void _ex_tab_itree_item_clicked_cb(Etk_Object *object, Etk_Tree_Row *row, void *data);
static void _ex_tab_itree_key_down_cb(Etk_Object *object, void *event, void *data);

Ex_Tab *
_ex_tab_new(Exhibit *e, char *dir)
{
   Ex_Tab *tab;
   Etk_Tree_Model *imodel;
   char *file;

   file = NULL;
   tab = calloc(1, sizeof(Ex_Tab));
   //tab->num = eina_list_count(e->tabs);
   tab->dirs = NULL;

   if (!dir) {
	D(("NO DIR\n"));
	exit(-1);
   }

   D(("Creating new tab with %s\n", dir));

   if (e->options->default_view == EX_IMAGE_FIT_TO_WINDOW)
     {
	tab->fit_window = ETK_TRUE;
	D(("Setting EX_IMAGE_FIT_TO_WINDOW\n"));
     }
   else
      tab->fit_window = ETK_FALSE;

   tab->comment.visible = ETK_FALSE;
   tab->image_loaded = ETK_FALSE;

   tab->dtree = etk_tree_new();
   etk_widget_size_request_set(tab->dtree, 180, 120);
   etk_signal_connect("row-clicked", ETK_OBJECT(tab->dtree), ETK_CALLBACK(_ex_tab_dtree_item_clicked_cb), e);
   tab->dcol = etk_tree_col_new(ETK_TREE(tab->dtree), "Directories", 10, 0.0);
   etk_tree_col_model_add(tab->dcol, etk_tree_model_image_new());
   etk_tree_col_model_add(tab->dcol, etk_tree_model_text_new());
   etk_tree_headers_visible_set(ETK_TREE(tab->dtree), 0);
   etk_scrolled_view_policy_set(etk_tree_scrolled_view_get(ETK_TREE(tab->dtree)), ETK_POLICY_AUTO, ETK_POLICY_SHOW);
   etk_tree_build(ETK_TREE(tab->dtree));

   tab->itree = etk_tree_new();
   etk_scrolled_view_dragable_set(ETK_SCROLLED_VIEW(etk_tree_scrolled_view_get(ETK_TREE(tab->itree))), ETK_TRUE);
   etk_scrolled_view_drag_bouncy_set(ETK_SCROLLED_VIEW(etk_tree_scrolled_view_get(ETK_TREE(tab->itree))), ETK_FALSE);
   etk_scrolled_view_drag_damping_set(ETK_SCROLLED_VIEW(etk_tree_scrolled_view_get(ETK_TREE(tab->itree))), 300);



   etk_widget_dnd_source_set(ETK_WIDGET(tab->itree), ETK_TRUE);
//   etk_signal_connect("drag_begin", ETK_OBJECT(tab->itree), ETK_CALLBACK(_ex_tab_tree_drag_begin_cb), tab);
   etk_widget_size_request_set(tab->itree, 180, 220);
   etk_tree_multiple_select_set(ETK_TREE(tab->itree), ETK_TRUE);
   etk_signal_connect("row-selected", ETK_OBJECT(tab->itree), ETK_CALLBACK(_ex_tab_itree_item_clicked_cb), e);
   etk_signal_connect("key-down", ETK_OBJECT(tab->itree), ETK_CALLBACK(_ex_tab_itree_key_down_cb), e);
   imodel = etk_tree_model_wobbly_new();
   tab->imodel = imodel;
   //etk_tree_model_image_width_set(imodel, 80, 0.0);
   tab->icol = etk_tree_col_new(ETK_TREE(tab->itree), "Files", 10, 0.0);
   etk_tree_col_model_add(tab->icol, imodel);
   etk_tree_col_model_add(tab->icol, etk_tree_model_text_new());
   etk_tree_headers_visible_set(ETK_TREE(tab->itree), 0);
   etk_tree_rows_height_set(ETK_TREE(tab->itree), 60);
   etk_scrolled_view_policy_set(etk_tree_scrolled_view_get(ETK_TREE(tab->itree)), ETK_POLICY_AUTO, ETK_POLICY_SHOW);
   etk_tree_build(ETK_TREE(tab->itree));

   tab->dir = _ex_path_normalize(ecore_file_realpath(dir));
   tab->alignment = etk_alignment_new(0.5, 0.5, 1.0, 1.0);

   tab->image = etk_image_new();
   etk_widget_theme_file_set(tab->image, PACKAGE_DATA_DIR"/gui.edj");
   etk_widget_theme_group_set(tab->image, "image_bg");
   etk_signal_connect("mouse-down", ETK_OBJECT(tab->image), ETK_CALLBACK(_ex_image_mouse_down), e);
   etk_signal_connect("mouse-up", ETK_OBJECT(tab->image), ETK_CALLBACK(_ex_image_mouse_up), e);
   etk_signal_connect("mouse-move", ETK_OBJECT(tab->image), ETK_CALLBACK(_ex_image_mouse_move), e);
   etk_signal_connect("mouse-wheel", ETK_OBJECT(tab->image), ETK_CALLBACK(_ex_image_mouse_wheel), e);

   etk_image_keep_aspect_set(ETK_IMAGE(tab->image), ETK_TRUE);

   etk_container_add(ETK_CONTAINER(tab->alignment), tab->image);

   tab->scrolled_view = etk_scrolled_view_new();
   etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(tab->scrolled_view), ETK_POLICY_HIDE, ETK_POLICY_HIDE);
   etk_scrolled_view_dragable_set(ETK_SCROLLED_VIEW(tab->scrolled_view), ETK_TRUE);
   etk_scrolled_view_drag_bouncy_set((ETK_SCROLLED_VIEW(tab->scrolled_view)), ETK_FALSE);
   etk_scrolled_view_add_with_viewport(ETK_SCROLLED_VIEW(tab->scrolled_view), tab->alignment);
   etk_widget_has_event_object_set(tab->scrolled_view, ETK_TRUE);
   etk_signal_connect("mouse-wheel", ETK_OBJECT(tab->scrolled_view), ETK_CALLBACK(_ex_image_mouse_wheel), e);

   return tab;
}

void
_ex_tab_delete(void)
{
   if (!e->cur_tab)
     {
	D(("No currently selected TAB!!\n"));
	return;
     }

   D(("Number of tabs: %d\n", eina_list_count(e->tabs)));

   if (eina_list_count(e->tabs) < 1)
     {
	D(("Cannot remove the last tab\n"));
	return;
     }

   D(("Delete tab %d\n", e->cur_tab->num));

   D(("Remove from list\n"));
   e->tabs = eina_list_remove(e->tabs, e->cur_tab);

   D(("Free\n"));
   etk_notebook_page_remove(ETK_NOTEBOOK(e->notebook), e->cur_tab->num);
   /* Set the cur_tab on the new one */
   e->cur_tab = eina_list_nth(e->tabs, etk_notebook_current_page_get(ETK_NOTEBOOK(e->notebook)));
//   E_FREE(e->cur_tab);

   if (eina_list_count(e->tabs) == 1)
     etk_notebook_tabs_visible_set(ETK_NOTEBOOK(e->notebook), ETK_FALSE);
}

void
_ex_tab_select(Ex_Tab *tab)
{
   //chdir(tab->dir);

   D(("_ex_tab_select: changed dir to %s\n", tab->dir));
   D(("_ex_tab_select: selecting tab num %d\n", e->cur_tab->num));

   if (tab->comment.visible)
     etk_notebook_page_child_set(ETK_NOTEBOOK(e->notebook), tab->num,
				 tab->comment.vbox);
   else if (tab->fit_window)
     etk_notebook_page_child_set(ETK_NOTEBOOK(e->notebook), tab->num,
				 tab->alignment);

   etk_table_attach(ETK_TABLE(e->table), tab->dtree,
		    0, 3, 3, 3,
		    ETK_TABLE_VEXPAND | ETK_TABLE_FILL, 0, 0);
   etk_widget_show(tab->dtree);

   etk_paned_child2_set(ETK_PANED(e->vpaned), tab->itree, ETK_TRUE);
   etk_widget_show(tab->itree);

   etk_widget_show(tab->image);
   etk_widget_show(tab->alignment);
   etk_widget_show(tab->scrolled_view);

   etk_widget_show_all(e->win);
}

void
_ex_tab_current_zoom_in(Exhibit *e)
{
   if (e->cur_tab->fit_window)
     {
	if (e->cur_tab->comment.visible)
	  {
	     etk_notebook_page_child_set(ETK_NOTEBOOK(e->notebook), e->cur_tab->num, e->cur_tab->comment.vbox);
	     etk_container_remove(e->cur_tab->alignment);
	     etk_container_remove(e->cur_tab->comment.frame);
	     etk_box_append(ETK_BOX(e->cur_tab->comment.vbox), e->cur_tab->scrolled_view, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	     etk_box_append(ETK_BOX(e->cur_tab->comment.vbox), e->cur_tab->comment.frame, ETK_BOX_START, ETK_BOX_NONE, 3);
	  }
	else
	  etk_notebook_page_child_set(ETK_NOTEBOOK(e->notebook), e->cur_tab->num, e->cur_tab->scrolled_view);

        etk_widget_size_request_set(e->cur_tab->alignment, -1, -1);
        etk_scrolled_view_add_with_viewport(ETK_SCROLLED_VIEW(e->cur_tab->scrolled_view), e->cur_tab->alignment);
        e->cur_tab->fit_window = ETK_FALSE;
     }

   if (e->zoom == ZOOM_MAX)
     e->zoom = ZOOM_MAX;
   else
     e->zoom += 2;

   _ex_image_zoom(ETK_IMAGE(e->cur_tab->image), e->zoom);
   _ex_main_statusbar_zoom_update(e);
}

void
_ex_tab_current_zoom_out(Exhibit *e)
{
   if (e->cur_tab->fit_window)
     {
	if (e->cur_tab->comment.visible)
	  {
	     etk_notebook_page_child_set(ETK_NOTEBOOK(e->notebook), e->cur_tab->num, e->cur_tab->comment.vbox);
	     etk_container_remove(e->cur_tab->alignment);
	     etk_container_remove(e->cur_tab->comment.frame);
	     etk_box_append(ETK_BOX(e->cur_tab->comment.vbox), e->cur_tab->scrolled_view, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	     etk_box_append(ETK_BOX(e->cur_tab->comment.vbox), e->cur_tab->comment.frame, ETK_BOX_START, ETK_BOX_NONE, 3);
	  }
	     else
	  etk_notebook_page_child_set(ETK_NOTEBOOK(e->notebook), e->cur_tab->num, e->cur_tab->scrolled_view);

        etk_widget_size_request_set(e->cur_tab->alignment, -1, -1);
        etk_scrolled_view_add_with_viewport(ETK_SCROLLED_VIEW(e->cur_tab->scrolled_view), e->cur_tab->alignment);
        e->cur_tab->fit_window = ETK_FALSE;
     }

   if (e->zoom <= ZOOM_MIN)
     e->zoom = ZOOM_MIN;
   else
     e->zoom -= 2;

   _ex_image_zoom(ETK_IMAGE(e->cur_tab->image), e->zoom);
   _ex_main_statusbar_zoom_update(e);
}

void
_ex_tab_current_zoom_one_to_one(Exhibit *e)
{
   if (e->cur_tab->fit_window)
     {
	if (e->cur_tab->comment.visible)
	  {
	     etk_notebook_page_child_set(ETK_NOTEBOOK(e->notebook), e->cur_tab->num, e->cur_tab->comment.vbox);
	     etk_container_remove(e->cur_tab->alignment);
	     etk_container_remove(e->cur_tab->comment.frame);
	     etk_box_append(ETK_BOX(e->cur_tab->comment.vbox), e->cur_tab->scrolled_view, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	     etk_box_append(ETK_BOX(e->cur_tab->comment.vbox), e->cur_tab->comment.frame, ETK_BOX_START, ETK_BOX_NONE, 3);
	  }
	else
	  etk_notebook_page_child_set(ETK_NOTEBOOK(e->notebook), e->cur_tab->num, e->cur_tab->scrolled_view);

        etk_widget_size_request_set(e->cur_tab->alignment, -1, -1);
        etk_scrolled_view_add_with_viewport(ETK_SCROLLED_VIEW(e->cur_tab->scrolled_view), e->cur_tab->alignment);
        e->cur_tab->fit_window = ETK_FALSE;
     }

   e->zoom = 0;
   e->brightness = 128;
   e->contrast = 0;

   _ex_image_zoom(ETK_IMAGE(e->cur_tab->image), e->zoom);
   _ex_main_statusbar_zoom_update(e);
}

void
_ex_tab_current_fit_to_window(Exhibit *e)
{
   etk_widget_size_request_set(e->cur_tab->alignment, 10, 10);

   if (e->cur_tab->comment.visible)
     {
	etk_notebook_page_child_set(ETK_NOTEBOOK(e->notebook), e->cur_tab->num, e->cur_tab->comment.vbox);
	etk_container_remove(e->cur_tab->scrolled_view);
	etk_container_remove(e->cur_tab->comment.frame);
	etk_box_append(ETK_BOX(e->cur_tab->comment.vbox), e->cur_tab->alignment, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	etk_box_append(ETK_BOX(e->cur_tab->comment.vbox), e->cur_tab->comment.frame, ETK_BOX_START, ETK_BOX_NONE, 3);
     }
   else
     etk_notebook_page_child_set(ETK_NOTEBOOK(e->notebook), e->cur_tab->num, e->cur_tab->alignment);

   etk_widget_size_request_set(e->cur_tab->image, -1, -1);

   e->cur_tab->fit_window = ETK_TRUE;
   _ex_main_statusbar_zoom_update(e);
}

Ex_Tab *
_ex_tab_find_by_itree(Etk_Tree *itree)
{
   Eina_List *l;
   Ex_Tab *tab;

   EINA_LIST_FOREACH(e->tabs, l , tab)
     if (ETK_TREE(tab->itree) == itree)
       return tab;
   return NULL;
}

static void _ex_tab_tree_drag_begin_cb(Etk_Object *object, void *data)
{
   Ex_Tab       *tab;
   Etk_Tree     *tree;
   Etk_Tree_Row *row;
   Etk_Widget   *drag;
   Etk_Widget   *image;
   Eina_List    *rows;
   char *icol1_string;
   char *icol2_string;
   char *drag_data;
   const char **types;
   unsigned int num_types;

   tab = data;
   tree = ETK_TREE(object);
   /* TODO: reimplement dnd when it is reimplemented in Etk
   drag = (ETK_WIDGET(tree))->drag; */
   drag = NULL;

   rows = etk_tree_selected_rows_get(tree);

   types = calloc(1, sizeof(char*));
   num_types = 1;
   types[0] = strdup("text/uri-list");

   if (eina_list_count(rows) > 1)
     {
	Eina_List *ll;
	Etk_Widget *table;
	int i = 0, l = 0, r = 0, t = 0, b = 0, row_num;

	if (eina_list_count(rows) >= EX_DND_COL_NUM)
	  row_num = eina_list_count(rows) / EX_DND_COL_NUM;
	else
	  row_num = 1;

	table = etk_table_new(EX_DND_COL_NUM, row_num + 1, ETK_TABLE_HOMOGENEOUS);
	drag_data = calloc(PATH_MAX * eina_list_count(rows), sizeof(char));
	EINA_LIST_FOREACH(rows, ll, row)
	  {
	     char tmp[PATH_MAX];

	     etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), &icol1_string, &icol2_string, etk_tree_nth_col_get(tree, 1),NULL);
	     snprintf(tmp, PATH_MAX * sizeof(char), "file://%s%s\r\n", tab->dir, icol2_string);
	     strncat(drag_data, tmp, PATH_MAX * eina_list_count(rows));
	     if (i <= EX_DND_MAX_NUM * EX_DND_MAX_NUM)
	       {
		  image = etk_image_new_from_file(icol1_string, NULL);
		  etk_image_keep_aspect_set(ETK_IMAGE(image), ETK_TRUE);
		  etk_widget_size_request_set(image, 48, 48);
		  etk_table_attach(ETK_TABLE(table), image, l, r, t, b, ETK_TABLE_NONE, 3, 3);

		  ++l; ++r;

		  if (l == EX_DND_COL_NUM)
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
	snprintf(drag_data, PATH_MAX * sizeof(char), "file://%s%s\r\n", tab->dir, icol2_string);
	image = etk_image_new_from_file(icol1_string, NULL);
	etk_image_keep_aspect_set(ETK_IMAGE(image), ETK_TRUE);
	etk_widget_size_request_set(image, 96, 96);
	etk_container_add(ETK_CONTAINER(drag), image);
     }

   etk_drag_types_set(ETK_DRAG(drag), types, num_types);
   etk_drag_data_set(ETK_DRAG(drag), drag_data, strlen(drag_data) + 1);
}

static void
_ex_tab_dtree_item_clicked_cb(Etk_Object *object, Etk_Tree_Row *row, void *event, void *data)
{
   Etk_Tree *tree;
   char *dcol_string;
   Exhibit *e;
   char *path, *real_path;

   e = data;
   _ex_slideshow_stop();
   _ex_thumb_abort();

   tree = ETK_TREE(object);
   etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), NULL, NULL, &dcol_string, NULL);

   path = calloc(PATH_MAX, sizeof(char));
   snprintf(path, PATH_MAX, "%s/%s", e->cur_tab->dir, dcol_string);

   real_path = ecore_file_realpath(path);
   E_FREE(path);

   E_FREE(e->cur_tab->dir);
   e->cur_tab->dir = _ex_path_normalize(real_path);
   E_FREE(real_path);

   etk_tree_clear(ETK_TREE(e->cur_tab->itree));
   etk_tree_clear(ETK_TREE(e->cur_tab->dtree));
   etk_combobox_entry_clear(ETK_COMBOBOX_ENTRY(e->combobox_entry));
   _ex_main_populate_files(NULL, EX_TREE_UPDATE_ALL);
   etk_notebook_page_tab_label_set(ETK_NOTEBOOK(e->notebook), etk_notebook_current_page_get(ETK_NOTEBOOK(e->notebook)), _ex_file_get(e->cur_tab->dir));
}

static void
_ex_tab_itree_item_clicked_cb(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   Exhibit *e;
   char *path;

   e = data;
   e->zoom = 0;
   _ex_main_statusbar_zoom_update(e);

   path = (char *)etk_tree_row_data_get(row);

   _ex_main_image_set(e, path);
}

static void
_ex_tab_itree_key_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Down *ev;
   Exhibit *e;

   e = data;
   ev = event;

   if (!strcmp(ev->key, "Return") || !strcmp(ev->key, "KP_Enter"))
     {
        e->cur_tab->dir = strdup((char*)etk_entry_text_get(ETK_ENTRY(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(e->combobox_entry)))));
        etk_tree_clear(ETK_TREE(e->cur_tab->itree));
        etk_tree_clear(ETK_TREE(e->cur_tab->dtree));
	etk_combobox_entry_clear(ETK_COMBOBOX_ENTRY(e->combobox_entry));
        _ex_main_populate_files(NULL, EX_TREE_UPDATE_ALL);
     }
}

