#ifndef __EVFS_METADATA_DB_H_
#define __EVFS_METADATA_DB_H_

int evfs_metadata_db_upgrade_check(sqlite3* db,int startmode);
void evfs_metadata_db_init(sqlite3** db);
int evfs_metadata_db_version_bump(sqlite3* db, char* ver);

int evfs_metadata_db_id_for_file(sqlite3* db, evfs_filereference* ref, int create);
void evfs_metadata_db_file_keyword_add(sqlite3* db, int file, char* key, char* value);

#endif
