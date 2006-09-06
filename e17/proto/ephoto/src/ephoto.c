#include "ephoto.h"
Main *m;
char *current_directory;

int
main(int argc, char **argv)
{
 FILE *file;
 char *home;
 const char *icon_theme_path;
 char ephoto_path[PATH_MAX];
 char ephoto_complete[PATH_MAX];
 m = NULL;
 
 if (!ewl_init(&argc, argv))
 {
  printf("Unable to init ewl\n");
  return 1;
 }

 home = getenv("HOME");
 current_directory = strdup(home);
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
 ewl_window_title_set(EWL_WINDOW(m->win), "Ephoto");
 ewl_window_name_set(EWL_WINDOW(m->win), "Ephoto");
 ewl_object_size_request(EWL_OBJECT(m->win), 615, 470);
 ewl_callback_append(m->win, EWL_CALLBACK_DELETE_WINDOW, destroy_cb, NULL);
 ewl_widget_show(m->win);

 m->vbox = ewl_vbox_new();
 ewl_box_spacing_set(EWL_BOX(m->vbox), 1);
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

 m->hseparator = ewl_hseparator_new();
 ewl_object_alignment_set(EWL_OBJECT(m->hseparator), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->vbox), m->hseparator);
 ewl_object_fill_policy_set(EWL_OBJECT(m->hseparator), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->hseparator);
 
 m->hbox = ewl_hbox_new();
 ewl_box_spacing_set(EWL_BOX(m->hbox), 5);
 ewl_container_child_append(EWL_CONTAINER(m->vbox), m->hbox);
 ewl_object_fill_policy_set(EWL_OBJECT(m->hbox), EWL_FLAG_FILL_HFILL |
		 				 EWL_FLAG_FILL_VSHRINK);
 ewl_widget_show(m->hbox);

 m->image = ewl_image_new();
 ewl_container_child_append(EWL_CONTAINER(m->hbox), m->image);
 icon_theme_path = ewl_icon_theme_icon_path_get(EWL_ICON_GO_UP, 
		 				EWL_ICON_SIZE_MEDIUM);
 ewl_image_file_set(EWL_IMAGE(m->image), icon_theme_path, NULL);
 ewl_callback_append(m->image, EWL_CALLBACK_CLICKED, go_up, NULL);
 ewl_widget_show(m->image);

 m->image = ewl_image_new();
 ewl_container_child_append(EWL_CONTAINER(m->hbox), m->image);
 icon_theme_path = ewl_icon_theme_icon_path_get(EWL_ICON_GO_HOME,
                                                EWL_ICON_SIZE_MEDIUM);
 ewl_image_file_set(EWL_IMAGE(m->image), icon_theme_path, NULL);
 ewl_callback_append(m->image, EWL_CALLBACK_CLICKED, go_home, NULL);
 ewl_widget_show(m->image);

 m->text = ewl_text_new();
 ewl_text_text_set(EWL_TEXT(m->text), "Location:");
 ewl_object_alignment_set(EWL_OBJECT(m->text), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->hbox), m->text);
 ewl_widget_show(m->text);

 m->entry = ewl_entry_new();
 ewl_text_text_set(EWL_TEXT(m->entry), current_directory);
 ewl_container_child_append(EWL_CONTAINER(m->hbox), m->entry);
 ewl_callback_append(m->entry, EWL_CALLBACK_VALUE_CHANGED, entry_change, NULL);
 ewl_widget_show(m->entry);
 
 m->hseparator = ewl_hseparator_new();
 ewl_object_alignment_set(EWL_OBJECT(m->hseparator), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->vbox), m->hseparator);
 ewl_object_fill_policy_set(EWL_OBJECT(m->hseparator), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->hseparator);
 
 m->hpaned = ewl_hbox_new();
 ewl_object_alignment_set(EWL_OBJECT(m->hpaned), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->vbox), m->hpaned);
 ewl_object_fill_policy_set(EWL_OBJECT(m->hpaned), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->hpaned);

 m->groups = ewl_vbox_new();
 ewl_container_child_append(EWL_CONTAINER(m->hpaned), m->groups);
 ewl_object_fill_policy_set(EWL_OBJECT(m->groups), EWL_FLAG_FILL_ALL);
 ewl_object_size_request(EWL_OBJECT(m->groups), 220, 250);
 ewl_object_maximum_size_set(EWL_OBJECT(m->groups), 195, 999999);
 ewl_widget_show(m->groups);

 m->browser = ewl_tree_new(1);
 ewl_container_child_append(EWL_CONTAINER(m->groups), m->browser);
 ewl_object_fill_policy_set(EWL_OBJECT(m->browser), EWL_FLAG_FILL_ALL);
 ewl_tree_headers_visible_set(EWL_TREE(m->browser), 0);
 ewl_tree_expandable_rows_set(EWL_TREE(m->browser), FALSE);
 ewl_object_size_request(EWL_OBJECT(m->groups), 220, 250);
 ewl_widget_show(m->browser);

 m->hseparator = ewl_hseparator_new();
 ewl_object_alignment_set(EWL_OBJECT(m->hseparator), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->groups), m->hseparator);
 ewl_object_fill_policy_set(EWL_OBJECT(m->hseparator), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->hseparator);

 m->albums_border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(m->albums_border), "Albums");
 ewl_border_label_alignment_set(EWL_BORDER(m->albums_border), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->groups), m->albums_border);
 ewl_object_alignment_set(EWL_OBJECT(m->albums_border), EWL_FLAG_ALIGN_CENTER);
 ewl_object_fill_policy_set(EWL_OBJECT(m->albums_border), EWL_FLAG_FILL_ALL);
 ewl_object_size_request(EWL_OBJECT(m->albums_border), 220, 250);
 ewl_widget_show(m->albums_border);

 m->albums = ewl_tree_new(1);
 ewl_container_child_append(EWL_CONTAINER(m->albums_border), m->albums);
 ewl_object_fill_policy_set(EWL_OBJECT(m->albums), EWL_FLAG_FILL_ALL);
 ewl_tree_headers_visible_set(EWL_TREE(m->albums), 0);
 ewl_tree_expandable_rows_set(EWL_TREE(m->albums), FALSE);
 ewl_object_size_request(EWL_OBJECT(m->albums), 220, 250);
 ewl_widget_show(m->albums);
 
 m->vsep = ewl_vseparator_new();
 ewl_container_child_append(EWL_CONTAINER(m->hpaned), m->vsep);
 ewl_widget_show(m->vsep);
 
 m->viewer = ewl_scrollpane_new();
 ewl_container_child_append(EWL_CONTAINER(m->hpaned), m->viewer);
 ewl_object_fill_policy_set(EWL_OBJECT(m->viewer), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->viewer);

 m->viewer_freebox = ewl_hfreebox_new();
 ewl_freebox_layout_type_set(EWL_FREEBOX(m->viewer_freebox), EWL_FREEBOX_LAYOUT_AUTO);
 ewl_container_child_append(EWL_CONTAINER(m->viewer), m->viewer_freebox);
 ewl_object_fill_policy_set(EWL_OBJECT(m->viewer_freebox), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->viewer_freebox);

 ewl_callback_append(m->albums, EWL_CALLBACK_SHOW, populate_albums, NULL);
 ewl_callback_append(m->browser, EWL_CALLBACK_SHOW, populate_browser, home);
 ewl_widget_name_set(m->browser, current_directory);
 populate_images(m->browser, NULL, NULL);
 
 ewl_main();
 return 0;
}

