#include "ephoto.h"
Main *m = NULL;
const char *current_album;
const char *current_slideshow;

int
main(int argc, char **argv)
{
 char *home;
 char *name;
 char ephoto_path[PATH_MAX];
 char database[PATH_MAX];
 int argint = 1;
 int rc = SQLITE_OK;
 int i = 0;
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
 snprintf(database, PATH_MAX, "%s/ephoto_database", ephoto_path);
 
 m = calloc(1, sizeof(Main));
 
 if (!ecore_file_exists(ephoto_path))
 {
  ecore_file_mkdir(ephoto_path);
 }

 if (!ecore_file_exists(database))
 {
  sqlite3_open(database, &db);
  sqlite3_exec(db, "CREATE TABLE images(id INTEGER PRIRMARY KEY AUTOINCREMENT,"
			"name VARCHAR(255));", NULL, NULL, NULL);
  sqlite3_exec(db, "CREATE TABLE albums(id INTEGER PRIMARY KEY AUTOINCREMENT," 
			"name VARCHAR(255));", NULL, NULL, NULL);
  sqlite3_exec(db, "CREATE TABLE albums_full(id INTEGER PRIMARY KEY AUTOINCREMENT," 
			"album_id INTEGER, image_id INTEGER);", NULL, NULL, NULL);
  sqlite3_exec(db, "CREATE TABLE slideshows(id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"name VARCHAR(255));", NULL, NULL, NULL);
  sqlite3_exec(db, "CREATE TABLE slideshows_images_full(id INTEGER PRIMARY KEY AUTOINCREMENT," 
			"slideshows_id INTEGER, images_id INTEGER);", NULL, NULL, NULL);
  sqlite3_exec(db, "CREATE TABLE settings(id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"name VARCHAR(255));", NULL, NULL, NULL);
  sqlite3_exec(db, "CREATE TABLE slideshows_settings_full(id INTEGER PRIMARY KEY AUTOINCREMENT,"
 			"slideshows_id INTEGER, settings_id INTEGER);", NULL, NULL, NULL);
  sqlite3_exec(db, "INSERT OR IGNORE INTO albums (name) VALUES ('Complete Library');", 
			NULL, NULL, NULL);
  sqlite3_exec(db, "INSERT OR IGNORE INTO slideshows (name) VALUES ('Complete Library Slideshow');",
			NULL, NULL, NULL);
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
 ewl_callback_append(m->menu_item, EWL_CALLBACK_CLICKED, add_album_image_cb, NULL);
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

 m->albums = ewl_scrollpane_new();
 ewl_widget_state_set(EWL_WIDGET(m->albums), "nobg", EWL_STATE_PERSISTENT);
 ewl_container_child_append(EWL_CONTAINER(m->albums_border), m->albums);
 ewl_object_fill_policy_set(EWL_OBJECT(m->albums), EWL_FLAG_FILL_ALL);
 ewl_box_spacing_set(EWL_BOX(EWL_SCROLLPANE(m->albums)->box), 8);
 ewl_widget_show(m->albums);

 m->hseparator = ewl_hseparator_new();
 ewl_object_alignment_set(EWL_OBJECT(m->hseparator), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->groups), m->hseparator);
 ewl_object_fill_policy_set(EWL_OBJECT(m->hseparator), EWL_FLAG_FILL_ALL);
 ewl_widget_show(m->hseparator);

 m->slideshows_border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(m->slideshows_border), "Slideshows");
 ewl_border_label_alignment_set(EWL_BORDER(m->slideshows_border), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->groups), m->slideshows_border);
 ewl_object_alignment_set(EWL_OBJECT(m->slideshows_border), EWL_FLAG_ALIGN_CENTER);
 ewl_object_fill_policy_set(EWL_OBJECT(m->slideshows_border), EWL_FLAG_FILL_ALL);
 ewl_object_size_request(EWL_OBJECT(m->slideshows_border), 30, 250);
 ewl_widget_show(m->slideshows_border);

 m->slideshows = ewl_scrollpane_new();
 ewl_widget_state_set(EWL_WIDGET(m->slideshows), "nobg", EWL_STATE_PERSISTENT);
 ewl_container_child_append(EWL_CONTAINER(m->slideshows_border), m->slideshows);
 ewl_object_fill_policy_set(EWL_OBJECT(m->slideshows), EWL_FLAG_FILL_ALL);
 ewl_box_spacing_set(EWL_BOX(EWL_SCROLLPANE(m->slideshows)->box), 8);
 ewl_widget_show(m->slideshows);

 m->viewer_border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(m->viewer_border), "Viewer");
 ewl_border_label_alignment_set(EWL_BORDER(m->viewer_border), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(m->hpaned), m->viewer_border);
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

 sqlite3_open(database, &db);
 rc = sqlite3_exec(db, "SELECT name FROM albums;", populate_album_cb, 0, 0);
 sqlite3_close(db);

 sqlite3_open(database, &db);
 rc = sqlite3_exec(db, "SELECT name FROM slideshows;", populate_slideshow_cb, 0, 0);
 sqlite3_close(db);


 ewl_main();
 return 0;
}

