#include "ephoto.h"
Main *m;

int
main(int argc, char **argv)
{
 FILE *file;
 char *home;
 char ephoto_path[PATH_MAX];
 char ephoto_complete[PATH_MAX];
 m = NULL;
 
 if (!ewl_init(&argc, argv))
 {
  printf("Unable to init ewl\n");
  return 1;
 }

 home = getenv("HOME");
 snprintf(ephoto_path, PATH_MAX, "%s/.ephoto", home);
 snprintf(ephoto_complete, PATH_MAX, "%s/Complete Library", ephoto_path);
 m = calloc(1, sizeof(Main));
 
 if (!ecore_file_exists(ephoto_path)) ecore_file_mkdir(ephoto_path);
 
 if (!ecore_file_exists(ephoto_complete))
 {
  file = fopen(ephoto_complete, "w");
  if (file != NULL)
  {
   fputs("#Ephoto Complete Library", file);
   fclose(file);
  }
 }
	 
 m->win = ewl_window_new();
 ewl_window_title_set(EWL_WINDOW(m->win), "ephoto");
 ewl_window_name_set(EWL_WINDOW(m->win), "ephoto");
 ewl_object_size_request(EWL_OBJECT(m->win), 580, 480);
 ewl_callback_append(m->win, EWL_CALLBACK_DELETE_WINDOW, destroy_cb, NULL);
 ewl_widget_show(m->win);

 m->vbox = ewl_vbox_new();
 ewl_container_child_append(EWL_CONTAINER(m->win), m->vbox);
 ewl_object_fill_policy_set(EWL_OBJECT(m->vbox), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->vbox);

 m->menubar = ewl_hmenubar_new();
 ewl_container_child_append(EWL_CONTAINER(m->vbox), m->menubar);
 ewl_object_fill_policy_set(EWL_OBJECT(m->menubar), EWL_FLAG_FILL_HFILL);
 ewl_widget_show(m->menubar);

 m->menu = ewl_menu_new();
 ewl_button_label_set(EWL_BUTTON(m->menu), "File");
 ewl_container_child_append(EWL_CONTAINER(m->menubar), m->menu);
 ewl_object_fill_policy_set(EWL_OBJECT(m->menu), EWL_FLAG_FILL_NONE);
 ewl_widget_show(m->menu);

 m->menu_item = ewl_menu_item_new();
 ewl_button_label_set(EWL_BUTTON(m->menu_item), "Exit");
 ewl_container_child_append(EWL_CONTAINER(m->menu), m->menu_item);
 ewl_callback_append(m->menu_item, EWL_CALLBACK_CLICKED, destroy_cb, NULL);
 ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->menu_item);

 m->hpaned = ewl_hpaned_new();
 ewl_object_alignment_set(EWL_OBJECT(m->hpaned), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->vbox), m->hpaned);
 ewl_object_fill_policy_set(EWL_OBJECT(m->hpaned), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->hpaned);

 m->groups = ewl_vbox_new();
 ewl_container_child_append(EWL_CONTAINER(m->hpaned), m->groups);
 ewl_object_fill_policy_set(EWL_OBJECT(m->groups), EWL_FLAG_FILL_ALL);
 ewl_object_size_request(EWL_OBJECT(m->groups), 30, 250);
 ewl_widget_show(m->groups);

 m->albums_border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(m->albums_border), "Albums");
 ewl_border_label_alignment_set(EWL_BORDER(m->albums_border), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->groups), m->albums_border);
 ewl_object_alignment_set(EWL_OBJECT(m->albums_border), EWL_FLAG_ALIGN_CENTER);
 ewl_object_fill_policy_set(EWL_OBJECT(m->albums_border), EWL_FLAG_FILL_ALL);
 ewl_object_size_request(EWL_OBJECT(m->albums_border), 30, 250);
 ewl_widget_show(m->albums_border);

 m->albums = ewl_tree_new(1);
 ewl_container_child_append(EWL_CONTAINER(m->albums_border), m->albums);
 ewl_object_fill_policy_set(EWL_OBJECT(m->albums), EWL_FLAG_FILL_ALL);
 ewl_tree_headers_visible_set(EWL_TREE(m->albums), 0);
 ewl_tree_expandable_rows_set(EWL_TREE(m->albums), FALSE);
 ewl_widget_show(m->albums);

 m->hseparator = ewl_hseparator_new();
 ewl_object_alignment_set(EWL_OBJECT(m->hseparator), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->groups), m->hseparator);
 ewl_object_fill_policy_set(EWL_OBJECT(m->hseparator), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->hseparator);

 m->browser_border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(m->browser_border), "Browser");
 ewl_border_label_alignment_set(EWL_BORDER(m->browser_border), 
		 		EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->groups), m->browser_border);
 ewl_object_alignment_set(EWL_OBJECT(m->browser_border), EWL_FLAG_ALIGN_CENTER);
 ewl_object_fill_policy_set(EWL_OBJECT(m->browser_border), EWL_FLAG_FILL_ALL);
 ewl_object_size_request(EWL_OBJECT(m->browser_border), 30, 250);
 ewl_widget_show(m->browser_border);
 
 m->browser = ewl_tree_new(1);
 ewl_container_child_append(EWL_CONTAINER(m->browser_border), m->browser);
 ewl_object_fill_policy_set(EWL_OBJECT(m->browser), EWL_FLAG_FILL_ALL);
 ewl_tree_headers_visible_set(EWL_TREE(m->browser), 0);
 ewl_tree_expandable_rows_set(EWL_TREE(m->browser), FALSE);
 ewl_widget_show(m->browser);
 
 m->viewer_border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(m->viewer_border), "Viewer");
 ewl_border_label_alignment_set(EWL_BORDER(m->viewer_border), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->hpaned), m->viewer_border);
 ewl_object_alignment_set(EWL_OBJECT(m->viewer_border), EWL_FLAG_ALIGN_CENTER);
 ewl_object_fill_policy_set(EWL_OBJECT(m->viewer_border), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->viewer_border);

 m->viewer = ewl_scrollpane_new();
 ewl_container_child_append(EWL_CONTAINER(m->viewer_border), m->viewer);
 ewl_object_fill_policy_set(EWL_OBJECT(m->viewer), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->viewer);

 m->viewer_freebox = ewl_hfreebox_new();
 ewl_freebox_layout_type_set(EWL_FREEBOX(m->viewer_freebox), EWL_FREEBOX_LAYOUT_AUTO);
 ewl_container_child_append(EWL_CONTAINER(m->viewer), m->viewer_freebox);
 ewl_object_fill_policy_set(EWL_OBJECT(m->viewer_freebox), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->viewer_freebox);

 ewl_callback_append(m->albums, EWL_CALLBACK_SHOW, populate_albums, NULL);
 ewl_callback_append(m->browser, EWL_CALLBACK_SHOW, populate_browser, home);
 
 ewl_main();
 return 0;
}

