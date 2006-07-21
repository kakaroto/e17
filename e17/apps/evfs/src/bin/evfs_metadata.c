#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "evfs.h"
#include "evfs_metadata.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <Eet.h>
#include <Evas.h>
#include <limits.h>

#include <sqlite3.h>

#include "evfs_metadata_db.h"

#define EVFS_METADATA_GROUP_LIST "/evfs/group/list"
#define EVFS_METADATA_BASE_DATA "block"

/*
 * TODO
 *
 * * Handle recursive (i.e. nested) files
 * * Make db access async - use callback properly, and don't block
 */

static int evfs_metadata_state = 0;

static Eet_Data_Descriptor* Evfs_Metadata_String_Edd;
static Eet_Data_Descriptor* Evfs_Metadata_Group_Edd;
static Eet_Data_Descriptor* Evfs_Metadata_File_Groups_Edd;
static Eet_Data_Descriptor* Evfs_Metadata_Root_Edd;

static evfs_metadata_root* metadata_root;

static char* homedir;
static char metadata_file[PATH_MAX];
static char metadata_db[PATH_MAX];
static Eet_File* _evfs_metadata_eet;

static sqlite3 *db;
static int _evfs_metadata_db_wait = 0;
static Ecore_DList* evfs_metdata_db_results = NULL;

/*DB Helper types*/
typedef struct {
	char* name;
	char* value;
} evfs_metadata_db_item;

typedef struct {
	int fieldcount;
	evfs_metadata_db_item** fields;
} evfs_metadata_db_result;




/*DB Helper functions*/
void evfs_metadata_db_results_init()
{
	if (evfs_metdata_db_results) {
		ecore_dlist_destroy(evfs_metdata_db_results);
		evfs_metdata_db_results = NULL;
	}

	evfs_metdata_db_results = ecore_dlist_new();
}

void evfs_metadata_db_results_free()
{
	evfs_metadata_db_result* result;
	ecore_dlist_goto_first(evfs_metdata_db_results);

	while ( (result = ecore_dlist_remove_first(evfs_metdata_db_results))) {
		int i;
		evfs_metadata_db_item* item;
		
		for (i=0;i<result->fieldcount;i++) {
			item = result->fields[i];
			if (item->name) free(item->name);
			if (item->value) free(item->value);
			free(item);
		}
		free(result->fields);
		free(result);
	}
	ecore_dlist_destroy(evfs_metdata_db_results);
	evfs_metdata_db_results = NULL;
	
}

void evfs_metadata_db_response_block()
{
	while (_evfs_metadata_db_wait) {
		usleep(2000);
	}
}

void evfs_metadata_db_response_setup()
{
	_evfs_metadata_db_wait = 1;
}

static int evfs_metadata_db_callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  evfs_metadata_db_result* result;

  result = calloc(1, sizeof(evfs_metadata_db_result));
  result->fields = calloc(argc, sizeof(evfs_metadata_db_item*));
  result->fieldcount = argc;
  
  for(i=0; i<argc; i++){
	evfs_metadata_db_item* item = calloc(1, sizeof(evfs_metadata_db_item));
	item->name = strdup(azColName[i]);
	item->value = strdup(argv[i]);
	result->fields[i] = item;
  }

  ecore_dlist_append(evfs_metdata_db_results, result);

   _evfs_metadata_db_wait = 0;
  return 0;
}
/*-------------*/

Eet_Data_Descriptor* _evfs_metadata_edd_create(char* desc, int size) 
{
	Eet_Data_Descriptor* edd;
	
	edd= eet_data_descriptor_new(desc, size,
                              (void *(*)(void *))evas_list_next,
                              (void *(*)(void *, void *))evas_list_append,
                              (void *(*)(void *))evas_list_data,
                              (void *(*)(void *))evas_list_free,
                              (void (*)
                               (void *,
                                int (*)(void *, const char *, void *, void *),
                                void *))evas_hash_foreach, (void *(*)(void *,
                                                                      const char
                                                                      *,
                                                                      void *))
                              evas_hash_add, (void (*)(void *))evas_hash_free);

	return edd;
}

void evfs_metadata_debug_group_list_print()
{
	Evas_List* l;
	evfs_metadata_group_header* g;

	printf("Printing group list:\n");
	for (l = metadata_root->group_list; l; ) {
		g = l->data;
	

		printf("Name: %s, Desc: %s\n", g->name, g->description);
		
		l = l->next;
	}
	printf("Done..\n");
}

void evfs_metadata_debug_file_groups_print(evfs_metadata_file_groups* groups)
{
	Evas_List* l;
	evfs_metadata_group_header* g;

	printf("Printing group list:\n");
	for (l = groups->groups; l; ) {
		g = l->data;
		
		printf("Name: %s, Desc: %s\n", g->name, g->description);
		
		l = l->next;
	}
	printf("Done..\n");
}


void evfs_metadata_group_header_free(evfs_metadata_group_header* g)
{
	if (g->name) free(g->name);
	if (g->description) free(g->description);
	free(g);
}

void evfs_metadata_file_groups_free(evfs_metadata_file_groups* groups) {
	Evas_List* l;
	evfs_metadata_group_header* g;

	for (l = groups->groups; l; ) {
		g = l->data;
	
		evfs_metadata_group_header_free(g);
		
		l = l->next;
	}

}

int evfs_metadata_file_groups_group_check(evfs_metadata_file_groups* groups, char* group) {
	Evas_List* l;
	evfs_metadata_group_header* g;
	int ret = 0;

	for (l = groups->groups; l; ) {
		g = l->data;
	
		if (!strncmp(group, g->name, strlen(g->name))) {
			ret = 1;
			goto JUMP_OUT;
		}
			
		l = l->next;
	}	

	JUMP_OUT:
	return ret;
}

evfs_metadata_group_header*
evfs_metadata_group_header_new(char* name, char* desc) 
{
	evfs_metadata_group_header* group;

	group = calloc(1, sizeof(evfs_metadata_group_header));

	if (name) group->name = strdup(name);
	if (desc) group->description = strdup(desc);

	return group;
}

int evfs_metadata_group_header_exists(char* group)
{
	char query[1024];
	int exists = 0;
	int ret;
	sqlite3_stmt *pStmt;

	evfs_metadata_db_response_setup();
	evfs_metadata_db_results_init();

	snprintf(query, sizeof(query), "select id from MetaGroup where name='%s'", group);
	ret = sqlite3_prepare(db, query, 
			-1, &pStmt, 0);
	if (ret == SQLITE_OK) {
		ret = sqlite3_step(pStmt);
		if (ret == SQLITE_ROW) exists = sqlite3_column_int(pStmt,0);
	} else {
		printf("header_exists: sqlite_error\n");
	}
		
	sqlite3_reset(pStmt);
	sqlite3_finalize(pStmt);

	return exists;
}

void evfs_metadata_initialise()
{
	struct stat config_dir_stat;
	Evas_List* group;
	char* data;
	int size;
	int ret;
	
	if (!evfs_metadata_state) {
		evfs_metadata_state++;
	} else return;

	group = NULL;

	

	if (!evfs_object_client_is_get()) {
		printf(". EVFS metadata initialise..\n");
			
		/*String edd*/
		Evfs_Metadata_String_Edd = _evfs_metadata_edd_create("evfs_metadata_string", sizeof(evfs_metadata_object));
		EET_DATA_DESCRIPTOR_ADD_BASIC(Evfs_Metadata_String_Edd, evfs_metadata_object,
	                                 "description", description, EET_T_STRING);
		EET_DATA_DESCRIPTOR_ADD_BASIC(Evfs_Metadata_String_Edd, evfs_metadata_object,
                                 "key", key, EET_T_STRING);
		EET_DATA_DESCRIPTOR_ADD_BASIC(Evfs_Metadata_String_Edd, evfs_metadata_object,
                                 "value", value, EET_T_STRING);

		/*Group edd*/
		Evfs_Metadata_Group_Edd = _evfs_metadata_edd_create("evfs_metadata_group_header", sizeof(evfs_metadata_group_header));
		EET_DATA_DESCRIPTOR_ADD_BASIC(Evfs_Metadata_Group_Edd, evfs_metadata_group_header,
	                                 "description", description, EET_T_STRING);
		EET_DATA_DESCRIPTOR_ADD_BASIC(Evfs_Metadata_Group_Edd, evfs_metadata_group_header,
	                                 "name", name, EET_T_STRING);

		/*Metadata group root*/
		Evfs_Metadata_Root_Edd = _evfs_metadata_edd_create("evfs_metadata_root", sizeof(evfs_metadata_root));
		EET_DATA_DESCRIPTOR_ADD_LIST(Evfs_Metadata_Root_Edd, evfs_metadata_root,
	                                 "group_list", group_list, Evfs_Metadata_Group_Edd);

		/*Metadata group root*/
		Evfs_Metadata_File_Groups_Edd = _evfs_metadata_edd_create("evfs_metadata_file_groups", sizeof(evfs_metadata_file_groups));
		EET_DATA_DESCRIPTOR_ADD_LIST(Evfs_Metadata_File_Groups_Edd, evfs_metadata_file_groups,
	                                 "groups", groups, Evfs_Metadata_Group_Edd);

		homedir = strdup(getenv("HOME"));
		snprintf(metadata_file, PATH_MAX, "%s/.e/evfs", homedir);
	
		if (stat(metadata_file, &config_dir_stat)) {
			mkdir(metadata_file, 0700);
		}

		snprintf(metadata_file, PATH_MAX, "%s/.e/evfs/evfs_metadata.eet", homedir);

		if (stat(metadata_file, &config_dir_stat)) {
			printf("Making new metadata file..\n");
				
			/*Open/close the file*/
			_evfs_metadata_eet = eet_open(metadata_file, EET_FILE_MODE_WRITE);
	
			metadata_root = calloc(1, sizeof(evfs_metadata_root));
	
			/*Create a starting 'group' list*/
			metadata_root->group_list = evas_list_append(metadata_root->group_list, 
					evfs_metadata_group_header_new("Pictures", "Pictures"));
			metadata_root->group_list = evas_list_append(metadata_root->group_list, 
					evfs_metadata_group_header_new("Video", "Video"));
			metadata_root->group_list = evas_list_append(metadata_root->group_list, 
					evfs_metadata_group_header_new("Audio", "Audio"));

			data = eet_data_descriptor_encode(Evfs_Metadata_Root_Edd, metadata_root, &size);
			ret = eet_write(_evfs_metadata_eet, EVFS_METADATA_GROUP_LIST, data, size, 0);
	
			free(data);
	
			eet_close(_evfs_metadata_eet);
			
		} else {
			printf("Loading pre-existing metadata root..\n");
		
			_evfs_metadata_eet = eet_open(metadata_file, EET_FILE_MODE_READ);	
			data = eet_read(_evfs_metadata_eet, EVFS_METADATA_GROUP_LIST, &size);
			if (data) {
				metadata_root = 
					eet_data_descriptor_decode(Evfs_Metadata_Root_Edd, data, size);
				free(data);

				printf("..Loaded group list..\n");

				evfs_metadata_debug_group_list_print();
			} else {
				printf("Error loading group list..\n");
			}
			eet_close(_evfs_metadata_eet);
		}

	
		evfs_metadata_db_init(&db);
	}

}

Evas_List* evfs_metadata_groups_get() {
	int ret;
	Evas_List* ret_list = NULL;
	sqlite3_stmt *pStmt;
	evfs_metadata_group_header* g;
	char* str;

	ret = sqlite3_prepare(db, "select name,visualHint from MetaGroup where parent = 0", 
			-1, &pStmt, 0);

	do {
		ret = sqlite3_step(pStmt);
		

		if (ret == SQLITE_ROW) {
			g = calloc(1, sizeof(evfs_metadata_group_header));
			g->name = strdup(sqlite3_column_text(pStmt,0));
			if (sqlite3_column_text(pStmt, 1)) {
				g->visualhint = strdup(sqlite3_column_text(pStmt,1));
				printf("Loaded visualhint %s\n", g->visualhint);
			}
			
			ret_list = evas_list_append(ret_list, g);
		}
	} while (ret == SQLITE_ROW);

	sqlite3_reset(pStmt);
	sqlite3_finalize(pStmt);
	
	return ret_list;
}

void evfs_metadata_file_set_key_value_edd(evfs_filereference* ref, char* key, 
		void* value, Eet_Data_Descriptor* edd) 
{

}

Ecore_List*
evfs_metadata_file_group_list(char* group) 
{
	Ecore_List* ret_list;
	char query[PATH_MAX];
	sqlite3_stmt *pStmt;
	int ret;

	ret_list = ecore_list_new();

	snprintf(query, sizeof(query), "select f.filename from File f join FileGroup fg on f.id = fg.file join MetaGroup mg on mg.id = fg.metagroup where mg.name= '%s'", group);
	ret = sqlite3_prepare(db, query, 
			-1, &pStmt, 0);

	do {
		ret = sqlite3_step(pStmt);

		if (ret == SQLITE_ROW) 
			ecore_list_append(ret_list, strdup(sqlite3_column_text(pStmt,0)));
	} while (ret == SQLITE_ROW);

	sqlite3_reset(pStmt);
	sqlite3_finalize(pStmt);
	

	return ret_list;
}

void evfs_metadata_group_header_file_add(evfs_filereference* ref, char* group)
{
	char* file_path;
	int ret = 0;

	char* errMsg = 0;
	char query[1024];

	int groupid = 0;
	int file = 0;
	sqlite3_stmt *pStmt;

	/*First make sure this group exists*/

	if ( (groupid = evfs_metadata_group_header_exists(group))) {
		printf("Group exists - proceed\n");
	} else {
		printf("Alert - group not found\n");
		return;
	}

	/*Build a path*/
	file_path = evfs_filereference_to_string(ref);
	printf("File path is: %s\n", file_path);


	snprintf(query, sizeof(query), "select id from File where filename ='%s'", file_path);
	ret = sqlite3_prepare(db, query, 
			-1, &pStmt, 0);

	if (ret == SQLITE_OK) {
		ret = sqlite3_step(pStmt);
		if (ret == SQLITE_ROW)  {
			file = sqlite3_column_int(pStmt,0);
		} else {
			snprintf(query, sizeof(query), "insert into File (filename) select '%s';", file_path);
			ret = sqlite3_exec(db, query, 
			NULL, 0,&errMsg);
	
			file = (int)sqlite3_last_insert_rowid(db);
		}
	} else {
		printf("header_file_add: sqlite error\n");
	}
	sqlite3_reset(pStmt);
	sqlite3_finalize(pStmt);

	if (file && groupid) {
		printf("File id: %d - Group id: %d\n", file, groupid);

		snprintf(query, sizeof(query), "insert into FileGroup (File,MetaGroup) values (%d,%d);",
				file, groupid);
		ret = sqlite3_exec (db, query, NULL, 0, &errMsg);
	}

	evfs_metadata_db_results_free();
	
}


void evfs_metadata_group_header_file_remove(evfs_filereference* ref, char* group)
{
	char* file_path;
	int ret = 0;

	char* errMsg = 0;
	char query[1024];

	int groupid = 0;
	int file = 0;
	sqlite3_stmt *pStmt;

	/*First make sure this group exists*/

	if ( (groupid = evfs_metadata_group_header_exists(group))) {
		printf("Group exists - proceed\n");
	} else {
		printf("Alert - group not found\n");
		return;
	}

	/*Build a path*/
	file_path = evfs_filereference_to_string(ref);
	printf("File path is: %s\n", file_path);


	snprintf(query, sizeof(query), "select id from File where filename ='%s'", file_path);
	ret = sqlite3_prepare(db, query, 
			-1, &pStmt, 0);

	if (ret == SQLITE_OK) {
		ret = sqlite3_step(pStmt);
		if (ret == SQLITE_ROW)  {
			file = sqlite3_column_int(pStmt,0);
		} else {
			printf("File does not exist - cannot remove from group\n");
			return;
		}
	} else {
		printf("header_file_add: sqlite error\n");
	}
	sqlite3_reset(pStmt);
	sqlite3_finalize(pStmt);

	if (file && groupid) {
		printf("File id: %d - Group id: %d\n", file, groupid);

		snprintf(query, sizeof(query), "delete from FileGroup where file = %d and metaGroup = %d",
				file, groupid);
		ret = sqlite3_exec (db, query, NULL, 0, &errMsg);
	}

	evfs_metadata_db_results_free();
	
}




void evfs_metadata_file_set_key_value_string(evfs_filereference* ref, char* key,
		char* value) 
{
	evfs_metadata_object obj;
	char path[PATH_MAX];
	char* file_path;
	char* data;
	int size;
	int ret;

	/*Build a path*/
	file_path = evfs_filereference_to_string(ref);

	snprintf(path, PATH_MAX, "/filedata/%s/custommeta/string/%s", file_path, key);
	_evfs_metadata_eet = eet_open(metadata_file, EET_FILE_MODE_READ_WRITE);
	ret = 0;

	obj.description = "string";
	obj.key = key;
	obj.value = (char*)value;

	data = eet_data_descriptor_encode(Evfs_Metadata_String_Edd, &obj, &size);
	
	if (data) {
		ret = eet_write(_evfs_metadata_eet, path, data, size, 0);
	}
	if (ret) {
		printf("Wrote %s for %s\n", value, path);
	}
	free(data);
	free(file_path);

	eet_close(_evfs_metadata_eet);
}

void evfs_metadata_file_get_key_value_string(evfs_filereference* ref, char* key,
		char* value) 
{
	evfs_metadata_object* obj = NULL;
	char path[PATH_MAX];
	char* data;
	char* file_path;
	int size;
	int ret;

	/*Build a path*/
	file_path = evfs_filereference_to_string(ref);

	snprintf(path, PATH_MAX, "/filedata/%s/custommeta/string/%s", file_path, key);
	_evfs_metadata_eet = eet_open(metadata_file, EET_FILE_MODE_READ);
	ret = 0;

	if (_evfs_metadata_eet) {
		data = eet_read(_evfs_metadata_eet, path, &size);
		if (data) {
			obj = eet_data_descriptor_decode(Evfs_Metadata_String_Edd, data, size);
		}

		if (obj) {
			printf ("Got %s -> %s\n", obj->key, (char*)obj->value);
			free(data);
			free(obj);
		}
	}	
	eet_close(_evfs_metadata_eet);

	free(file_path);
}
