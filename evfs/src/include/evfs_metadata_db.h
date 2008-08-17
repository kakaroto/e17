#ifndef __EVFS_METADATA_DB_H_
#define __EVFS_METADATA_DB_H_

int evfs_metadata_db_upgrade_check(sqlite3* db,int startmode);
void evfs_metadata_db_init(sqlite3** db);
sqlite3* evfs_metadata_db_connect();
int evfs_metadata_db_version_bump(sqlite3* db, char* ver);

int evfs_metadata_db_id_for_file(sqlite3* db, EvfsFilereference* ref, int create);
void evfs_metadata_db_file_keyword_add(sqlite3* db, int file, char* key, char* value);
void evfs_metadata_db_delete_file(sqlite3* db, int file);
int evfs_metadata_db_vfolder_search_create(sqlite3* db, char* name);
void evfs_metadata_db_close(sqlite3* db);
void evfs_metadata_db_vfolder_search_entry_add(sqlite3* db, int id, EvfsVfolderEntry* entry);
Ecore_List* evfs_metadata_db_vfolder_search_list_get(sqlite3* db);
int evfs_metadata_db_vfolder_search_id_get(sqlite3* db, char* name);
Ecore_List* evfs_metadata_db_vfolder_search_entries_get(sqlite3* db, int id);
Ecore_List* evfs_metadata_db_vfolder_search_entries_execute(sqlite3* db, Ecore_List* entries);
Evas_List* evfs_metadata_db_meta_list_get(sqlite3* db);
void evfs_metadata_db_record_stat(sqlite3*, int, struct stat*);

#endif
