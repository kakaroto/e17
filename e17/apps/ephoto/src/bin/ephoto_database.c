#include "ephoto.h"

static int get_album_id(void *notused, int argc, char **argv, char **col);
static int get_image_id(void *notused, int argc, char **argv, char **col);
static int list_albums(void *notused, int argc, char **argv, char **col);
static int list_images(void *notused, int argc, char **argv, char **col);
static int list_image_ids(void *notused, int argc, char **argv, char **col);

static int image_id, album_id;

static Ecore_List *albums, *images_list, *image_ids;

sqlite3 *
ephoto_db_init(void)
{
	char path[PATH_MAX];
	char path2[PATH_MAX];
	sqlite3 *db;

	albums = ecore_list_new();
	images_list = ecore_dlist_new();
	image_ids = ecore_list_new();

	snprintf(path, PATH_MAX, "%s/.ephoto/.ephoto_database", 
			getenv("HOME"));
	snprintf(path2, PATH_MAX, "%s/.ephoto", getenv("HOME"));

	if (!ecore_file_exists(path))
	{
		if (!ecore_file_exists(path2))
		{
			ecore_file_mkdir(path2);
		}
		sqlite3_open(path, &db);
		sqlite3_exec(db, "CREATE TABLE 'albums'( "
				 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
				 "name VARCHAR( 255 ) NOT NULL, "
				 "description VARCHAR( 255 ) NULL);", 0, 0, 0);
                sqlite3_exec(db, "CREATE TABLE images( "
				"id INTEGER PRIMARY KEY AUTOINCREMENT, "
				"name VARCHAR( 255 ) NULL, "
				"path VARCHAR( 255 ) NOT NULL);", 0, 0, 0);		  
		sqlite3_exec(db, "CREATE TABLE album_images( "
				 "image_id INTEGER NOT NULL, "
				 "album_id INTEGER NOT NULL);", 0, 0, 0);
		ephoto_db_add_album(db, "Complete Library", 
					"Contains every tagged image!");
	}
	else 
 	{
		sqlite3_open(path, &db);
 	}
	return db;
}

static int 
get_image_id(void *notused, int argc, char **argv, char **col)
{
	int i;

	notused = 0;
	i = 0;

	image_id = atoi(argv[i] ? argv[i] : "NULL");

	return 0;
}

static int 
get_album_id(void *notused, int argc, char **argv, char **col)
{
	int i;

	notused = 0;
	i = 0;

	album_id = atoi(argv[i] ? argv[i] : "NULL");

	return 0;
}

void 
ephoto_db_add_album(sqlite3 *db, const char *name, const char *description)
{
	char command[PATH_MAX];

	snprintf(command, PATH_MAX, 
			  "INSERT or IGNORE INTO albums(name, description) "
                          "VALUES('%s', '%s');", name, description);
	sqlite3_exec(db, command, 0, 0, 0);

	return;
}

void 
ephoto_db_delete_album(sqlite3 *db, const char *name)
{
	char command[PATH_MAX];

	snprintf(command, PATH_MAX, "DELETE FROM albums WHERE name = '%s';", 
									name);
	sqlite3_exec(db, command, 0, 0, 0);
	snprintf(command, PATH_MAX, "SELECT id FROM albums WHERE name = '%s';",
									name);
	sqlite3_exec(db, command, get_album_id, 0, 0);
	snprintf(command, PATH_MAX, 
			"DELETE FROM album_images WHERE album_id = '%d';", 
								album_id);
	sqlite3_exec(db, command, 0, 0, 0);

	return;
}

void 
ephoto_db_add_image(sqlite3 *db, const char *album, const char *name, const char *path)
{
	char command[PATH_MAX];
	
	snprintf(command, PATH_MAX, "INSERT or IGNORE INTO images(name, path) "
			            "VALUES('%s', '%s');", name, path);
	sqlite3_exec(db, command, 0, 0, 0);
	snprintf(command, PATH_MAX, "SELECT id FROM images WHERE path = '%s';",
									 path);
	sqlite3_exec(db, command, get_image_id, 0, 0);
	snprintf(command, PATH_MAX, "SELECT id FROM albums WHERE name = '%s';", 
									album);
	sqlite3_exec(db, command, get_album_id, 0, 0);
	snprintf(command, PATH_MAX, 
		"INSERT or IGNORE into album_images(image_id, album_id) "
		"VALUES('%d', '%d');", image_id, album_id);
	sqlite3_exec(db, command, 0, 0, 0);

	snprintf(command, PATH_MAX, 
		"SELECT id FROM albums WHERE name = 'Complete Library';");
	sqlite3_exec(db, command, get_album_id, 0, 0);
	snprintf(command, PATH_MAX, 
			  "INSERT into album_images(image_id, album_id) "
			  "VALUES('%d', '%d');", image_id, album_id);
	sqlite3_exec(db, command, 0, 0, 0);

	return;
}

void 
ephoto_db_delete_image(sqlite3 *db, const char *album, const char *path)
{
	char command[PATH_MAX];

	snprintf(command, PATH_MAX, "SELECT id FROM albums WHERE name = '%s';", 
									album);
	sqlite3_exec(db, command, get_album_id, 0, 0);
	snprintf(command, PATH_MAX, "SELECT id FROM images WHERE path = '%s';", 
									path);
	sqlite3_exec(db, command, get_image_id, 0, 0);
	snprintf(command, PATH_MAX, "DELETE FROM album_images WHERE "
		       	            "image_id = '%d' AND album_id = '%d';", 
				    image_id, album_id);
	sqlite3_exec(db, command, 0, 0, 0);
	return;
}	

static int 
list_albums(void *notused, int argc, char **argv, char **col)
{
	int i;
 
	notused = 0;

	for(i = 0; i < argc; i++)
	{
		ecore_list_append(albums, strdup(argv[i] ? argv[i] : "NULL"));
	}

	return 0;
}

Ecore_List *
ephoto_db_list_albums(sqlite3 *db)
{
	if(!ecore_list_empty_is(albums))
	{
		ecore_list_destroy(albums);
	}
	albums = ecore_list_new();
	sqlite3_exec(db, "SELECT name FROM albums;", list_albums, 0, 0);
	
	return albums;
}

static int 
list_images(void *notused, int argc, char **argv, char **col)
{
	int i;

	notused = 0;

	for(i = 0; i < argc; i++)
	{
		ecore_dlist_append(images_list, 
					strdup(argv[i] ? argv[i] : "NULL"));
	}

	return 0;
}

static int 
list_image_ids(void *notused, int argc, char **argv, char **col)
{
	int i;

	notused = 0;

	for(i = 0; i < argc; i++)
	{
		ecore_list_append(image_ids, 
					strdup(argv[i] ? argv[i] : "NULL"));
	}

	return 0;
}

Ecore_List *
ephoto_db_list_images(sqlite3 *db,  const char *album)
{
	char command[PATH_MAX];
	char *id;

	if(!ecore_list_empty_is(images_list))
	{
		ecore_dlist_destroy(images_list);
	}
	if(!ecore_list_empty_is(image_ids))
	{
		ecore_list_destroy(image_ids);
	}
	images_list = ecore_dlist_new();
	image_ids = ecore_list_new();

	snprintf(command, PATH_MAX, "SELECT id FROM albums WHERE name = '%s';", 
									album);
        sqlite3_exec(db, command, get_album_id, 0, 0);
	snprintf(command, PATH_MAX, 
		"SELECT image_id FROM album_images WHERE album_id = '%d';", 
								album_id);
	sqlite3_exec(db, command, list_image_ids, 0, 0);

	while(!ecore_list_empty_is(image_ids))
	{
		id = ecore_list_first_remove(image_ids);
		snprintf(command, PATH_MAX, "SELECT path FROM images WHERE id"
					    " = '%s'", id);			            
	    	sqlite3_exec(db, command, list_images, 0, 0);
	}

	return images_list;
}

void 
ephoto_db_close(sqlite3 *db)
{
	sqlite3_close(db);
	return;
} 

