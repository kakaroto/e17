#include "ephoto.h"

/*Ephoto databasing callbacks*/
static int get_album_id(void *notused, int argc, char **argv, char **col);
static int get_image_id(void *notused, int argc, char **argv, char **col);
static int list_albums(void *notused, int argc, char **argv, char **col);
static int list_images(void *notused, int argc, char **argv, char **col);
static int list_image_ids(void *notused, int argc, char **argv, char **col);

/*Ephoto databasing global variables*/
static int image_id, album_id;

/*Ephoto databasing ecore global variables*/
static Ecore_List *albums, *images, *image_ids;

/*Open the sqlite3 database. Create the database if it does not already exits.*/
sqlite3 *ephoto_db_init(void)
{
	char path[PATH_MAX];
	sqlite3 *db;

	snprintf(path, PATH_MAX, "%s/.ephoto/.ephoto_database", getenv("HOME"));

	if (!ecore_file_exists(path))
	{
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
		ephoto_db_add_album(db, "Complete Library", "Contains every tagged image!");
	}
	else 
 	{
		sqlite3_open(path, &db);
 	}
	return db;
}

/*Get the id of an image in the images table*/
static int get_image_id(void *notused, int argc, char **argv, char **col)
{
	int i;

	notused = 0;
	i = 0;

	image_id = atoi(argv[i] ? argv[i] : "NULL");

	return 0;
}

/*Get the id of an album in the albums table*/
static int get_album_id(void *notused, int argc, char **argv, char **col)
{
	int i;

	notused = 0;
	i = 0;

	album_id = atoi(argv[i] ? argv[i] : "NULL");

	return 0;
}

/*Add a new album to the album table*/
void ephoto_db_add_album(sqlite3 *db, char *name, char *description)
{
	char command[PATH_MAX];

	snprintf(command, PATH_MAX, "INSERT or IGNORE INTO albums(name, description) "
                                    "VALUES('%s', '%s');", name, description);
	sqlite3_exec(db, command, 0, 0, 0);

	return;
}

/*Add a new image to a particular album*/
void ephoto_db_add_image(sqlite3 *db, char *album, char *name, char *path)
{
	char command[PATH_MAX];
	
	snprintf(command, PATH_MAX, "INSERT or IGNORE INTO images(name, path) "
			            "VALUES('%s', '%s');", name, path);
	sqlite3_exec(db, command, 0, 0, 0);
	snprintf(command, PATH_MAX, "SELECT id FROM images WHERE path = '%s';", path);
	sqlite3_exec(db, command, get_image_id, 0, 0);
	snprintf(command, PATH_MAX, "SELECT id FROM albums WHERE name = '%s';", album);
	sqlite3_exec(db, command, get_album_id, 0, 0);
	snprintf(command, PATH_MAX, "INSERT or IGNORE into album_images(image_id, album_id) "
				    "VALUES('%d', '%d');", image_id, album_id);
	sqlite3_exec(db, command, 0, 0, 0);

	/*This is to make sure the complete library has all images*/
	snprintf(command, PATH_MAX, "SELECT id FROM albums WHERE name = 'Complete Library';");
	sqlite3_exec(db, command, get_album_id, 0, 0);
	snprintf(command, PATH_MAX, "INSERT into album_images(image_id, album_id) "
				    "VALUES('%d', '%d');", image_id, album_id);
	sqlite3_exec(db, command, 0, 0, 0);

	return;
}

/*Populate a list the albums in the albums table*/
static int list_albums(void *notused, int argc, char **argv, char **col)
{
	int i;
 
	notused = 0;

	for(i = 0; i < argc; i++)
	{
		ecore_list_append(albums, strdup(argv[i] ? argv[i] : "NULL"));
	}

	return 0;
}

/*Return a list of albums in the albums table*/ 
Ecore_List *ephoto_db_list_albums(sqlite3 *db)
{
	if(albums)
	{
		ecore_list_destroy(albums);
	}
	albums = ecore_list_new();
	sqlite3_exec(db, "SELECT name FROM albums;", list_albums, 0, 0);
	
	return albums;
}

/*Populate a list of images belonging to a certain album*/
static int list_images(void *notused, int argc, char **argv, char **col)
{
	int i;

	notused = 0;

	for(i = 0; i < argc; i++)
	{
		ecore_list_append(images, strdup(argv[i] ? argv[i] : "NULL"));
	}

	return 0;
}

/*Populate a list of the id's of images belonging to a certain album*/
static int list_image_ids(void *notused, int argc, char **argv, char **col)
{
	char command[PATH_MAX];
	int i;

	notused = 0;

	for(i = 0; i < argc; i++)
	{
		ecore_list_append(image_ids, strdup(argv[i] ? argv[i] : "NULL"));
	}

	return 0;
}

/*Return a list of images belonging to a certain album*/
Ecore_List *ephoto_db_list_images(sqlite3 *db, char *album)
{
	char command[PATH_MAX];
	char *id;

	if(images)
	{
		ecore_list_destroy(images);
	}
	if(image_ids)
	{
		ecore_list_destroy(image_ids);
	}
	images = ecore_list_new();
	image_ids = ecore_list_new();
	
	snprintf(command, PATH_MAX, "SELECT id FROM albums WHERE name = '%s';", album);
        sqlite3_exec(db, command, get_album_id, 0, 0);
	snprintf(command, PATH_MAX, "SELECT image_id FROM album_images WHERE album_id = '%d';", album_id);
	sqlite3_exec(db, command, list_image_ids, 0, 0);

	while(!ecore_list_is_empty(image_ids))
	{
		id = ecore_list_remove_first(image_ids);
		snprintf(command, PATH_MAX, "SELECT path FROM images WHERE id"
					    " = '%s'", id);			            
	    	sqlite3_exec(db, command, list_images, 0, 0);
	}

	return images;
}

/*Close the sqlite3 database*/
void ephoto_db_close(sqlite3 *db)
{
	sqlite3_close(db);
	return;
} 

