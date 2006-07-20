#include "ephoto.h"
Main *m = NULL;

int
main(int argc, char **argv)
{
 char *home;
 char *name;
 char ephoto_path[PATH_MAX];
 char album_path[PATH_MAX];
 char slideshow_path[PATH_MAX];
 char album_library[PATH_MAX];
 char slideshow_library[PATH_MAX];
 int argint = 1;
 Ecore_List *files; 
 sqlite3 *db;

 if (!ewl_init(&argc, argv))
 {
  printf("Unable to init ewl\n");
  return 1;
 }

 while ( argint < argc ) 
 {
  argint++;
 }

 home = getenv("HOME");
 snprintf(ephoto_path, PATH_MAX, "%s/.ephoto", home);
 snprintf(album_path, PATH_MAX, "%s/.ephoto/albums", home);
 snprintf(slideshow_path, PATH_MAX, "%s/.ephoto/slideshows", home);
 snprintf(album_library, PATH_MAX, "%s/Complete Library", album_path);
 snprintf(slideshow_library, PATH_MAX, "%s/Complete Library Slideshow", slideshow_path);
 m = calloc(1, sizeof(Main));
 
 if (!ecore_file_exists(ephoto_path))
 {
  ecore_file_mkdir(ephoto_path);
 }

 if (!ecore_file_exists(album_path))
 {
  ecore_file_mkdir(album_path);
 }

 if (!ecore_file_exists(slideshow_path))
 {
  ecore_file_mkdir(slideshow_path);
 }

 if (!ecore_file_exists(album_library))
 {
  sqlite3_open(album_library, &db);
  sqlite3_exec(db, "create table images(images varchar(255));", NULL, 0, 0);
  sqlite3_close(db);
 }

 if (!ecore_file_exists(slideshow_library))
 {
  sqlite3_open(slideshow_library, &db);
  sqlite3_exec(db, "create table slideshows(images varchar(255), settings varchar(255));", NULL, 0, 0);
  sqlite3_close(db);
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

 m->menu = ewl_menu_new();
 ewl_button_label_set(EWL_BUTTON(m->menu), "Albums");
 ewl_container_child_append(EWL_CONTAINER(m->menubar), m->menu);
 ewl_object_fill_policy_set(EWL_OBJECT(m->menu), EWL_FLAG_FILL_NONE);
 ewl_widget_show(m->menu);

 m->menu_item = ewl_menu_item_new();
 ewl_button_label_set(EWL_BUTTON(m->menu_item), "Create new album");
 ewl_container_child_append(EWL_CONTAINER(m->menu), m->menu_item);
 ewl_callback_append(m->menu_item, EWL_CALLBACK_CLICKED, add_album_cb, NULL);
 ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->menu_item);

 m->menu_item = ewl_menu_item_new();
 ewl_button_label_set(EWL_BUTTON(m->menu_item), "Add images to album");
 ewl_container_child_append(EWL_CONTAINER(m->menu), m->menu_item);
 ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->menu_item);

 m->menu_item = ewl_menu_item_new();
 ewl_button_label_set(EWL_BUTTON(m->menu_item), "Remove album");
 ewl_container_child_append(EWL_CONTAINER(m->menu), m->menu_item);
 ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->menu_item);

 m->menu = ewl_menu_new();
 ewl_button_label_set(EWL_BUTTON(m->menu), "Slideshows");
 ewl_container_child_append(EWL_CONTAINER(m->menubar), m->menu);
 ewl_object_fill_policy_set(EWL_OBJECT(m->menu), EWL_FLAG_FILL_NONE);
 ewl_widget_show(m->menu);

 m->menu_item = ewl_menu_item_new();
 ewl_button_label_set(EWL_BUTTON(m->menu_item), "Create new slideshow");
 ewl_container_child_append(EWL_CONTAINER(m->menu), m->menu_item);
 ewl_callback_append(m->menu_item, EWL_CALLBACK_CLICKED, add_slideshow_cb, NULL);
 ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->menu_item);

 m->menu_item = ewl_menu_item_new();
 ewl_button_label_set(EWL_BUTTON(m->menu_item), "Add images to slideshow");
 ewl_container_child_append(EWL_CONTAINER(m->menu), m->menu_item);
 ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->menu_item);

 m->menu_item = ewl_menu_item_new();
 ewl_button_label_set(EWL_BUTTON(m->menu_item), "Remove slideshow");
 ewl_container_child_append(EWL_CONTAINER(m->menu), m->menu_item);
 ewl_object_fill_policy_set(EWL_OBJECT(m->menu_item), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->menu_item);

 m->paned = ewl_hpaned_new();
 ewl_object_alignment_set(EWL_OBJECT(m->paned), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->vbox), m->paned);
 ewl_object_fill_policy_set(EWL_OBJECT(m->paned), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->paned);

 m->groups = ewl_vbox_new();
 ewl_container_child_append(EWL_CONTAINER(m->paned), m->groups);
 ewl_object_fill_policy_set(EWL_OBJECT(m->groups), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->groups);

 m->albums_border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(m->albums_border), "Albums");
 ewl_border_label_alignment_set(EWL_BORDER(m->albums_border), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->groups), m->albums_border);
 ewl_object_alignment_set(EWL_OBJECT(m->albums_border), EWL_FLAG_ALIGN_CENTER);
 ewl_object_fill_policy_set(EWL_OBJECT(m->albums_border), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->albums_border);

 m->albums = ewl_scrollpane_new();
 ewl_widget_state_set(EWL_WIDGET(m->albums), "nobg", EWL_STATE_PERSISTENT);
 ewl_container_child_append(EWL_CONTAINER(m->albums_border), m->albums);
 ewl_object_fill_policy_set(EWL_OBJECT(m->albums), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->albums);

 files = ecore_list_new();
 files = ecore_file_ls(album_path);

 m->icon = ewl_icon_new();
 ewl_icon_label_set(EWL_ICON(m->icon), "Complete Library");
 ewl_object_alignment_set(EWL_OBJECT(m->icon), EWL_FLAG_ALIGN_CENTER);
 ewl_callback_append(m->icon, EWL_CALLBACK_CLICKED, album_clicked_cb, NULL);
 ewl_container_child_append(EWL_CONTAINER(m->albums), m->icon);
 ewl_widget_show(m->icon);

 while (!ecore_list_is_empty(files))
 { 
  name = ecore_list_remove_first(files);

  if (strcmp(name, "Complete Library") != 0)
  {
   m->icon = ewl_icon_new();
   ewl_icon_label_set(EWL_ICON(m->icon), name);
   ewl_object_alignment_set(EWL_OBJECT(m->icon), EWL_FLAG_ALIGN_CENTER);
   ewl_callback_append(m->icon, EWL_CALLBACK_CLICKED, album_clicked_cb, NULL);
   ewl_container_child_append(EWL_CONTAINER(m->albums), m->icon);
   ewl_widget_show(m->icon);
  }
 }

 ecore_list_destroy(files);

 m->slideshows_border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(m->slideshows_border), "Slideshows");
 ewl_border_label_alignment_set(EWL_BORDER(m->slideshows_border), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->groups), m->slideshows_border);
 ewl_object_alignment_set(EWL_OBJECT(m->slideshows_border), EWL_FLAG_ALIGN_CENTER);
 ewl_object_fill_policy_set(EWL_OBJECT(m->slideshows_border), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->slideshows_border);

 m->slideshows = ewl_scrollpane_new();
 ewl_widget_state_set(EWL_WIDGET(m->slideshows), "nobg", EWL_STATE_PERSISTENT);
 ewl_container_child_append(EWL_CONTAINER(m->slideshows_border), m->slideshows);
 ewl_object_fill_policy_set(EWL_OBJECT(m->slideshows), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->slideshows);

 files = ecore_list_new();
 files = ecore_file_ls(slideshow_path);

 m->icon = ewl_icon_new();
 ewl_icon_label_set(EWL_ICON(m->icon), "Complete Library Slideshow");
 ewl_object_alignment_set(EWL_OBJECT(m->icon), EWL_FLAG_ALIGN_CENTER);
 ewl_callback_append(m->icon, EWL_CALLBACK_CLICKED, slideshow_clicked_cb, NULL);
 ewl_container_child_append(EWL_CONTAINER(m->slideshows), m->icon);
 ewl_widget_show(m->icon);

 while (!ecore_list_is_empty(files))
 {
  name = ecore_list_remove_first(files);
  
  if (strcmp(name, "Complete Library Slideshow") != 0)
  {
   m->icon = ewl_icon_new();
   ewl_icon_label_set(EWL_ICON(m->icon), name);
   ewl_object_alignment_set(EWL_OBJECT(m->icon), EWL_FLAG_ALIGN_CENTER);
   ewl_callback_append(m->icon, EWL_CALLBACK_CLICKED, slideshow_clicked_cb, NULL);
   ewl_container_child_append(EWL_CONTAINER(m->slideshows), m->icon);
   ewl_widget_show(m->icon);
  }
 }

 ecore_list_destroy(files);

 m->viewer_border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(m->viewer_border), "Viewer");
 ewl_border_label_alignment_set(EWL_BORDER(m->viewer_border), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->paned), m->viewer_border);
 ewl_object_alignment_set(EWL_OBJECT(m->viewer_border), EWL_FLAG_ALIGN_CENTER);
 ewl_object_fill_policy_set(EWL_OBJECT(m->viewer_border), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->viewer_border);

 m->viewer = ewl_scrollpane_new();
 ewl_widget_state_set(EWL_WIDGET(m->viewer), "nobg", EWL_STATE_PERSISTENT);
 ewl_container_child_append(EWL_CONTAINER(m->viewer_border), m->viewer);
 ewl_object_fill_policy_set(EWL_OBJECT(m->viewer), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->viewer);

 m->viewer_freebox = ewl_hfreebox_new();
 ewl_freebox_layout_type_set(EWL_FREEBOX(m->viewer_freebox), EWL_FREEBOX_LAYOUT_AUTO);
 ewl_container_child_append(EWL_CONTAINER(m->viewer), m->viewer_freebox);
 ewl_object_fill_policy_set(EWL_OBJECT(m->viewer_freebox), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->viewer_freebox);

 ewl_main();
 return 0;
}

