#include "ephoto.h"
int image_id;
int album_id;

sqlite3 *ephoto_db_init(void)
{
 char path[PATH_MAX];
 sqlite3 *db;

 snprintf(path, PATH_MAX, "%s/.ephoto/.ephoto_database", getenv("HOME"));

 if (!ecore_file_exists(path))
 {
  sqlite3_open(path, &db);

  sqlite3_exec(db, "CREATE TABLE images( "
	  	   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
		   "name VARCHAR( 255 ) NULL, "
		   "path VARCHAR( 255 ) NOT NULL);", 0, 0, 0);
  sqlite3_exec(db, "CREATE TABLE 'albums'( "
	  	   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
		   "name VARCHAR( 255 ) NOT NULL, "
		   "description VARCHAR( 255 ) NULL);", 0, 0, 0);
  sqlite3_exec(db, "CREATE TABLE album_image( "
		   "image_id INTEGER NOT NULL, "
		   "album_id INTEGER NOT NULL);", 0, 0, 0);
  ephoto_db_add_album(db, "Complete Library", "Contains every tagged image!");
 }

 else 
 {
  sqlite3_open(path, &db);
 }

 return db;
}

void ephoto_db_add_album(sqlite3 *db, char *name, char *description)
{
 char command[PATH_MAX];
 Ewl_Widget *hbox, *image, *text, *children[2], *row;

 snprintf(command, PATH_MAX, "INSERT or IGNORE INTO albums(name, description) "
		             "VALUES('%s', '%s');", name, description);
 sqlite3_exec(db, command, 0, 0, 0);

 return;
}

int list_albums(void *notused, int argc, char **argv, char **col)
{
 int i;
 Ewl_Widget *hbox, *image, *text, *children[2], *row;
 
 notused = 0;

 for(i = 0; i < argc; i++)
 {
  hbox = ewl_hbox_new();
  ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
  ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);
  ewl_box_spacing_set(EWL_BOX(hbox), 5);
  ewl_widget_show(hbox);

  image = ewl_image_new();
  ewl_image_file_set(EWL_IMAGE(image),
		     PACKAGE_DATA_DIR "/images/image.png", NULL);
  ewl_container_child_append(EWL_CONTAINER(hbox), image);
  ewl_widget_show(image);

  text = ewl_text_new();
  ewl_text_text_set(EWL_TEXT(text), argv[i] ? argv[i] : "NULL");
  ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
  ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_SHRINK);
  ewl_container_child_append(EWL_CONTAINER(hbox), text);
  ewl_widget_show(text);

  children[0] = hbox;  
  children[1] = NULL;

  row = ewl_tree_row_add(EWL_TREE(m->albums), NULL, children);
 }

 return 0;
}

void ephoto_db_list_albums(sqlite3 *db)
{
 sqlite3_exec(db, "SELECT name FROM albums;", list_albums, 0, 0);
 return;
}

void ephoto_db_add_image(sqlite3 *db, char *name, char *path)
{
 char command[PATH_MAX];
 
 snprintf(command, PATH_MAX, "INSERT or IGNORE INTO images(name, path) "
		             "VALUES(%s, %s);", name, path);
 sqlite3_exec(db, command, 0, 0, 0);
 snprintf(command, PATH_MAX, "SELECT id FROM images WHERE path = %s;", path);
 sqlite3_exec(db, command, 0, 0, 0);
 return;
}

void ephoto_db_close(sqlite3 *db)
{
 sqlite3_close(db);
 return;
} 
