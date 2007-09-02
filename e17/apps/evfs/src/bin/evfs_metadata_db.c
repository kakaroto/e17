#include "evfs.h"
#include <stdio.h>
#include <limits.h>
#include <sqlite3.h>
#include "evfs_metadata_db.h"
#include "evfs.h"


/* TODO
 * * Make a 'bulk-run' function to avoid the endless repeated sqlite3_exec
 */

#define EVFS_METADATA_DB_CONFIG_LATEST 7
static char metadata_db[PATH_MAX];
static int loc_init = 0;
static char* homedir;
static Ecore_Hash* db_upgrade_hash = NULL;


int evfs_metadata_db_upgrade_0_1(sqlite3* db)
{
	int ret;
	char* errMsg = 0;
	char query[1024];

	printf("Performing upgrade from v.0 to v.1\n");

	/*Seed statements*/
	ret = sqlite3_exec(db, 
	"CREATE TABLE File (id integer primary key AUTOINCREMENT, filename varchar(1024));", 
	NULL, 0,&errMsg);
	if( ret ){
	    fprintf(stderr, "Create error: %s\n", sqlite3_errmsg(db));
	    sqlite3_close(db);
	    exit(1);
	}

	ret = sqlite3_exec(db, 
	"CREATE TABLE FileGroup (id integer primary key AUTOINCREMENT, File int, MetaGroup int);", 
	NULL, 0,&errMsg);

	ret = sqlite3_exec(db, 
	"CREATE TABLE MetaGroup (id integer primary key AUTOINCREMENT, name varchar(255), parent int);", 
	NULL, 0,&errMsg);

	ret = sqlite3_exec(db, 
	"CREATE TABLE CustomValues (id integer primary key AUTOINCREMENT, name varchar(255), value varchar(255));", 
	NULL, 0,&errMsg);

	/*Inserts*/
	ret = sqlite3_exec(db, 
	"INSERT INTO \"MetaGroup\" (id,name,parent) VALUES(NULL, 'Pictures', 0);", 
	NULL, 0,&errMsg);

	ret = sqlite3_exec(db, 
	"INSERT INTO \"MetaGroup\" (id,name,parent) VALUES(NULL, 'Video', 0);", 
	NULL, 0,&errMsg);

	ret = sqlite3_exec(db, 
	"INSERT INTO \"MetaGroup\" (id,name,parent) VALUES(NULL, 'Audio', 0);", 
	NULL, 0,&errMsg);

	snprintf(query,sizeof(query), "INSERT INTO \"CustomValues\" VALUES(NULL, 'ConfigVersion', '%d');", 
			1);
	ret = sqlite3_exec(db, query, NULL, 0,&errMsg);

	return evfs_metadata_db_version_bump(db, "1");
}



/*---Upgrade functions*/
int evfs_metadata_db_upgrade_1_2(sqlite3* db) 
{
	int ret;
	char* errMsg = 0;

	printf("Performing upgrade from v.1 to v.2\n");

	ret = sqlite3_exec(db, 
	"alter table MetaGroup add visualHint varchar(255);", 
	NULL, 0,&errMsg);

	ret = sqlite3_exec(db, 
	"update MetaGroup set visualHint='video_hint' where name='Video' and parent=0", 
	NULL, 0,&errMsg);

	ret = sqlite3_exec(db, 
	"update MetaGroup set visualHint='audio_hint' where name='Audio' and parent=0", 
	NULL, 0,&errMsg);

	ret = sqlite3_exec(db, 
	"update MetaGroup set visualHint='image_hint' where name='Pictures' and parent=0", 
	NULL, 0,&errMsg);

	return evfs_metadata_db_version_bump(db, "2");
}

int evfs_metadata_db_upgrade_2_3(sqlite3* db)
{
	int ret;
	char* errMsg = 0;

	printf("Performing upgrade from v.2 to v.3\n");

	ret = sqlite3_exec(db, 
	"create table FileMeta (id integer primary key AUTOINCREMENT, File int, keyword varchar(255), value varchar(255));", 
	NULL, 0,&errMsg);

	ret = sqlite3_exec(db, 
	"create index FileMeta_idx_file on FileMeta(File);", 
	NULL, 0,&errMsg);

	ret = sqlite3_exec(db, 
	"create index FileMeta_idx_keyword_value on FileMeta(Keyword,Value);", 
	NULL, 0,&errMsg);


	return evfs_metadata_db_version_bump(db, "3");
}

int evfs_metadata_db_upgrade_3_4(sqlite3* db)
{
	int ret;
	char* errMsg = 0;

	printf("Performing upgrade from v.3 to v.4\n");

	ret = sqlite3_exec(db, 
	"create index FileMeta_idx_keyword_value_file on FileMeta(File,Keyword,Value);", 
	NULL, 0,&errMsg);

	return evfs_metadata_db_version_bump(db, "4");
}

int evfs_metadata_db_upgrade_4_5(sqlite3* db)
{
	int ret;
	char* errMsg = 0;

	printf("Performing upgrade from v.4 to v.5\n");

	ret = sqlite3_exec(db, 
	"create table FileTag (id integer primary key AUTOINCREMENT, File int, tag varchar(255));", 
	NULL, 0,&errMsg);

	return evfs_metadata_db_version_bump(db, "5");
}

int evfs_metadata_db_upgrade_5_6(sqlite3* db)
{
	int ret;
	char* errMsg = 0;

	printf("Performing upgrade from v.5 to v.6\n");

	ret = sqlite3_exec(db, 
	"create table VfolderSearch (id integer primary key AUTOINCREMENT, name varchar(255));",
	NULL, 0,&errMsg);

	ret = sqlite3_exec(db, 
	"create table VfolderSearchComponent (id integer primary key AUTOINCREMENT, vfolderSearch int, rtype char, rkey varchar(255), rvalue varchar(255));",
	NULL, 0,&errMsg);

	return evfs_metadata_db_version_bump(db, "6");
}

int evfs_metadata_db_upgrade_6_7(sqlite3* db)
{
	int ret;
	char* errMsg = 0;

	printf("Performing upgrade from v.6 to v.7\n");

	ret = sqlite3_exec(db, 
	"CREATE TABLE FileStat (id integer primary key AUTOINCREMENT, File int, tm_created int, tm_modified int, size int, fowner int, fgroup int);",
	NULL, 0,&errMsg);

	return evfs_metadata_db_version_bump(db, "7");
}


int evfs_metadata_db_version_bump(sqlite3* db, char* ver)
{
	int ret;
	char query[100];
	char* errMsg = 0;
	
	snprintf(query,sizeof(query), "update CustomValues set value = %s",ver);
	ret = sqlite3_exec(db, 
	query, NULL, 0,&errMsg);

	return atoi(ver);
}
/*---*/

void evfs_metadata_db_location_init()
{
	if (!loc_init) {
		loc_init =1 ;
		homedir = strdup(getenv("HOME"));
		snprintf(metadata_db, PATH_MAX, "%s/.e/evfs/evfs_metadata.db", homedir);
	}
}

void evfs_metadata_db_init(sqlite3** db)
{
	struct stat config_dir_stat;
	int ret;
	int ver;
	int runs = 0;

	evfs_metadata_db_location_init();

	db_upgrade_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
	ecore_hash_set(db_upgrade_hash, (int*)0, evfs_metadata_db_upgrade_0_1);
	ecore_hash_set(db_upgrade_hash, (int*)1, evfs_metadata_db_upgrade_1_2);
	ecore_hash_set(db_upgrade_hash, (int*)2, evfs_metadata_db_upgrade_2_3);
	ecore_hash_set(db_upgrade_hash, (int*)3, evfs_metadata_db_upgrade_3_4);
	ecore_hash_set(db_upgrade_hash, (int*)4, evfs_metadata_db_upgrade_4_5);
	ecore_hash_set(db_upgrade_hash, (int*)5, evfs_metadata_db_upgrade_5_6);
	ecore_hash_set(db_upgrade_hash, (int*)6, evfs_metadata_db_upgrade_6_7);
	
	/*Check if we need to seed the DB*/
	if (stat(metadata_db, &config_dir_stat)) {
		ret = sqlite3_open(metadata_db, db);
		if( ret ){
		    fprintf(stderr, "Can't open metadata database: %s\n", sqlite3_errmsg(*db));
		    sqlite3_close(*db);
		    exit(1);
		}

		ver = evfs_metadata_db_upgrade_check(*db,1);
		do {
			ver = evfs_metadata_db_upgrade_check(*db,0);
			runs += 1;
		} while (ver < EVFS_METADATA_DB_CONFIG_LATEST && runs < 100);

		if (runs == 100) {
			printf("Aborted upgrade of metadata db\n");
			exit(0);
		}	
	} else {

		ret = sqlite3_open(metadata_db, db);
		if( ret ){
		    fprintf(stderr, "Can't open metadata database: %s\n", sqlite3_errmsg(*db));
		    sqlite3_close(*db);
		    exit(1);
	
		}

		do {
			ver = evfs_metadata_db_upgrade_check(*db,0);
			runs += 1;
		} while (ver < EVFS_METADATA_DB_CONFIG_LATEST && runs < 100);

		if (runs == 100) {
			printf("Aborted upgrade of metadata db\n");
			exit(0);
		}
	}	

	printf("DB Init complete..\n");
}

sqlite3* evfs_metadata_db_connect()
{
	sqlite3* db;
	int ret;
	
	evfs_metadata_db_location_init();
	
	ret = sqlite3_open(metadata_db, &db);
	if( ret ){
	    fprintf(stderr, "Can't open metadata database: %s\n", sqlite3_errmsg(db));
	    sqlite3_close(db);
	    exit(1);
	}

	return db;
}

void evfs_metadata_db_close(sqlite3* db)
{
	sqlite3_close(db);
}

int evfs_metadata_db_upgrade_check(sqlite3* db, int startmode) 
{
	char query[1024];
	int ret;
	sqlite3_stmt *pStmt;
	int version = 0;
	int (*upgrade_func)(sqlite3*);

	if (!startmode) {
		snprintf(query, sizeof(query), "select value from CustomValues where name='ConfigVersion'");
		ret = sqlite3_prepare(db, query, 
				-1, &pStmt, 0);
		if (ret == SQLITE_OK) {
			ret = sqlite3_step(pStmt);
			if (ret == SQLITE_ROW) version = sqlite3_column_int(pStmt,0);
		} else {
			printf("cannot check metadata version: sqlite_error\n");
			exit(0);
		}
		sqlite3_reset(pStmt);
		sqlite3_finalize(pStmt);
	} else {
		version = 0;
	}

	printf("Current version is: %d\n", version);

	if (version < EVFS_METADATA_DB_CONFIG_LATEST) {
		printf("DB upgrade required..\n");
		upgrade_func = ecore_hash_get(db_upgrade_hash, (int*)version);

		if (upgrade_func) {
			return (*upgrade_func)(db);
		} else {
			/*No upgrade function? Argh..*/
			return 0;
		}
	} else {
		return EVFS_METADATA_DB_CONFIG_LATEST;
	}
}


int evfs_metadata_db_id_for_file(sqlite3* db, EvfsFilereference* ref, int create)
{
	char* file_path;
	char query[PATH_MAX];
	int ret;
	int file = 0;
	sqlite3_stmt *pStmt;

	
	/*Build a path*/
	file_path = EvfsFilereference_to_string(ref);
	/*printf("File path is: %s\n", file_path);*/


	snprintf(query, sizeof(query), "select id from File where filename = ?");
	ret = sqlite3_prepare(db, query, -1, &pStmt, 0);

	if (ret == SQLITE_OK) {
		sqlite3_bind_text(pStmt, 1, file_path, strlen(file_path), SQLITE_STATIC);
		
		ret = sqlite3_step(pStmt);
		if (ret == SQLITE_ROW)  {
			file = sqlite3_column_int(pStmt,0);

			sqlite3_reset(pStmt);
			sqlite3_finalize(pStmt);
		} else {
			sqlite3_reset(pStmt);
			sqlite3_finalize(pStmt);
			
			if (create) {
				snprintf(query, sizeof(query), "insert into File (filename) values(?);");
				ret = sqlite3_prepare(db, query, -1, &pStmt, 0);
				sqlite3_bind_text(pStmt, 1, file_path, strlen(file_path), SQLITE_STATIC);

				if (sqlite3_step(pStmt) == SQLITE_DONE) {
					file = (int)sqlite3_last_insert_rowid(db);
				} else {
					file = 0;
				}
				
				sqlite3_reset(pStmt);
				sqlite3_finalize(pStmt);
			} else {
				file = 0;
			}
		}
	} else {
		sqlite3_reset(pStmt);
		sqlite3_finalize(pStmt);
		
		printf("id_for_file: sqlite error (%s)\n", file_path);
		file = 0;
	}
	free(file_path);

	return file;
}

void evfs_metadata_db_file_keyword_add(sqlite3* db, int file, char* key, char* value)
{
	char query[1024];
	int ret;
	char* errMsg = 0;
	int cnt;
	sqlite3_stmt *pStmt;

	/*Check we don't already have a match for this file/keyword/value triplet*/
	/*FIXME: This is bad - what if the filename is >1024 chars*/
	snprintf(query, sizeof(query), "select count(*) from FileMeta where file=%d and keyword='%s' and value='%s'", file,key,value);
	ret = sqlite3_prepare(db, query, -1, &pStmt, 0);
	if (ret == SQLITE_OK) {
		ret = sqlite3_step(pStmt);
		if (ret == SQLITE_ROW)  {
			cnt = sqlite3_column_int(pStmt,0);
	
			sqlite3_reset(pStmt);
			sqlite3_finalize(pStmt);
			
			if (cnt == 0 &&key&&value) {
				snprintf(query,sizeof(query),
					"insert into FileMeta (File, keyword, value) select %d, '%s', '%s';", file,key,value);
				
				/*printf("Running %p:%s\n", db,query);*/
				ret = sqlite3_exec(db, 
				query, 
				NULL, 0,&errMsg);
				if (errMsg) printf("ERROR: %s\n", errMsg);
			} else {
					/*printf("db_file_keyword_add: key or value is null\n");*/
			}
		}
	}

}

void evfs_metadata_db_delete_file(sqlite3* db, int file)
{
	char query[1024];
	int ret;
	char* errMsg = 0;

	/*First delete the file parent reference*/
	snprintf(query, sizeof(query), "delete from File where id = %d", file);
	ret = sqlite3_exec(db, query, NULL,0,&errMsg);
	if (errMsg) printf("ERROR: %s\n", errMsg);

	/*Now delete any meta for this file*/
	snprintf(query, sizeof(query), "delete from FileMeta where File = %d", file);
	ret = sqlite3_exec(db, query, NULL,0,&errMsg);
	if (errMsg) printf("ERROR: %s\n", errMsg);

	snprintf(query, sizeof(query), "delete from FileStat where File = %d", file);
	ret = sqlite3_exec(db, query, NULL,0,&errMsg);
	if (errMsg) printf("ERROR: %s\n", errMsg);	

	/*...And any group memberships*/
	snprintf(query, sizeof(query), "delete from FileGroup where File = %d", file);
	ret = sqlite3_exec(db, query, NULL,0,&errMsg);
	if (errMsg) printf("ERROR: %s\n", errMsg);		
}


int evfs_metadata_db_vfolder_search_create(sqlite3* db, char* name)
{
	char query[PATH_MAX];
	int ret;
	int vfo = 0;
	sqlite3_stmt *pStmt;

	snprintf(query, sizeof(query), "select id from VFolderSearch where name = ?");
	ret = sqlite3_prepare(db, query, -1, &pStmt, 0);

	if (ret == SQLITE_OK) {
		sqlite3_bind_text(pStmt, 1, name, strlen(name), SQLITE_STATIC);
		
		ret = sqlite3_step(pStmt);
		if (ret == SQLITE_ROW)  {
			vfo = sqlite3_column_int(pStmt,0);

			sqlite3_reset(pStmt);
			sqlite3_finalize(pStmt);
		} else {
			sqlite3_reset(pStmt);
			sqlite3_finalize(pStmt);
			
			snprintf(query, sizeof(query), "insert into VFolderSearch (name) values(?);");
			ret = sqlite3_prepare(db, query, -1, &pStmt, 0);
			sqlite3_bind_text(pStmt, 1, name, strlen(name), SQLITE_STATIC);

			if (sqlite3_step(pStmt) == SQLITE_DONE) {
				vfo = (int)sqlite3_last_insert_rowid(db);
			} else {
				vfo = 0;
			}
				
			sqlite3_reset(pStmt);
			sqlite3_finalize(pStmt);
		}
	} else {
		sqlite3_reset(pStmt);
		sqlite3_finalize(pStmt);
		
		printf("id_for_file: sqlite error (%s)\n", name);
		vfo = 0;
	}

	return vfo;
}

void evfs_metadata_db_vfolder_search_entry_add(sqlite3* db, int id, EvfsVfolderEntry* entry) 
{
	char query[PATH_MAX];
	int ret;
	sqlite3_stmt *pStmt;

	snprintf(query, sizeof(query),
		"insert into VfolderSearchComponent (vfolderSearch, rtype, rkey, rvalue) values (%d, '%c', ?, ?);", id, entry->type);
	ret = sqlite3_prepare(db, query, -1, &pStmt, 0);

	if (ret == SQLITE_OK) {
		sqlite3_bind_text(pStmt, 1, entry->name, strlen(entry->name), SQLITE_STATIC);
		sqlite3_bind_text(pStmt, 2, entry->value, strlen(entry->value), SQLITE_STATIC);
		
		if (sqlite3_step(pStmt) != SQLITE_DONE) {
			printf("evfs_metadata_db_vfolder_search_entry_add: sqlite3 error\n");
		}
		sqlite3_reset(pStmt);
		sqlite3_finalize(pStmt);
	}
}

Ecore_List* evfs_metadata_db_vfolder_search_list_get(sqlite3* db)
{
	Ecore_List* retlist;

	char query[PATH_MAX];
	int ret;
	sqlite3_stmt *pStmt;
	char* name;

	retlist = ecore_list_new();

	snprintf(query, sizeof(query), "select name from VFolderSearch");
	ret = sqlite3_prepare(db, query, -1, &pStmt, 0);

	if (ret == SQLITE_OK) {
		
		while ((ret = sqlite3_step(pStmt)) == SQLITE_ROW) {
			name = strdup((const char*)sqlite3_column_text(pStmt,0));
			ecore_list_append(retlist,name);
		}

		sqlite3_reset(pStmt);
		sqlite3_finalize(pStmt);
	}

	return retlist;
}

int evfs_metadata_db_vfolder_search_id_get(sqlite3* db, char* name)
{
	char query[PATH_MAX];
	int ret;
	int id;
	sqlite3_stmt *pStmt;


	snprintf(query, sizeof(query), "select id from VFolderSearch where name = ?");
	ret = sqlite3_prepare(db, query, -1, &pStmt, 0);

	if (ret == SQLITE_OK) {
		sqlite3_bind_text(pStmt, 1, name, strlen(name), SQLITE_STATIC);	
		if ((ret = sqlite3_step(pStmt)) == SQLITE_ROW) {
			id = sqlite3_column_int(pStmt,0);
		} else {
			id = 0;		
		}

		sqlite3_reset(pStmt);
		sqlite3_finalize(pStmt);
	} else {
		id = 0;
	}

	return id;
}

Ecore_List* evfs_metadata_db_vfolder_search_entries_get(sqlite3* db, int id)
{
	Ecore_List* entries;

	entries = ecore_list_new();
	char query[PATH_MAX];
	int ret;
	sqlite3_stmt *pStmt;

	int rtype;
	char* rkey = NULL;
	char* rvalue=NULL;
	EvfsVfolderEntry* entry;

	entries = ecore_list_new();

	snprintf(query, sizeof(query), "select rtype, rkey, rvalue from VFolderSearchComponent where vfolderSearch = ?");
	ret = sqlite3_prepare(db, query, -1, &pStmt, 0);

	if (ret == SQLITE_OK) {
		sqlite3_bind_int(pStmt, 1, id);
		while ((ret = sqlite3_step(pStmt)) == SQLITE_ROW) {
			entry = NEW(EvfsVfolderEntry);
			rtype = sqlite3_column_text(pStmt,0)[0];
			if (sqlite3_column_text(pStmt,1)) 
				rkey = strdup((const char*)sqlite3_column_text(pStmt,1));
			if (sqlite3_column_text(pStmt,2))
				rvalue = strdup((const char*)sqlite3_column_text(pStmt,2));
			entry->type = rtype;
			entry->name = rkey;
			entry->value = rvalue;
			ecore_list_append(entries,entry);
		}

		sqlite3_reset(pStmt);
		sqlite3_finalize(pStmt);
	}
	
	return entries;
}

Ecore_List* evfs_metadata_db_vfolder_search_entries_execute(sqlite3* db, Ecore_List* entries)
{
	char query[PATH_MAX];

	EvfsVfolderEntry* entry;
	ecore_list_first_goto(entries);
	Ecore_List* files;
	char* file;
	int ret;
	sqlite3_stmt *pStmt;

	files = ecore_list_new();

	printf("Building query from %d entries\n", ecore_list_count(entries));
	
	snprintf(query,sizeof(query),"select distinct f.filename from File f where 1 ");
	while ( (entry=ecore_list_next(entries))) {
		if (entry->type == 'M') {
			strcat(query, " and f.id in (select file from FileMeta where keyword='");
			strcat(query, entry->name);
			strcat(query, "'");
			strcat(query, " and value = '");
			strcat(query, entry->value);
			strcat(query, "') ");

		}
	}
	
	ret = sqlite3_prepare(db, query, -1, &pStmt, 0);
	if (ret == SQLITE_OK) {
		while ((ret = sqlite3_step(pStmt)) == SQLITE_ROW) {
			file = strdup((char*)sqlite3_column_text(pStmt,0));		
			ecore_list_append(files,file);
		}

		sqlite3_reset(pStmt);
		sqlite3_finalize(pStmt);

	}
	printf("%s\n", query);

	return files;
}

Evas_List* evfs_metadata_db_meta_list_get(sqlite3* db)
{
	char query[PATH_MAX];
	Evas_List* meta = NULL;
	int ret;
	sqlite3_stmt *pStmt;

	snprintf(query,sizeof(query),"select keyword, count(*) cnt from FileMeta group by keyword order by cnt desc");
	ret = sqlite3_prepare(db, query, -1, &pStmt, 0);
	if (ret == SQLITE_OK) {
		while ((ret = sqlite3_step(pStmt)) == SQLITE_ROW) {
			EvfsMetaObject* m =  NEW(EvfsMetaObject);
			char* mt = strdup((char*)sqlite3_column_text(pStmt,0));
			m->key = mt;

			meta = evas_list_append(meta, m);	
		}
		sqlite3_reset(pStmt);
		sqlite3_finalize(pStmt);

	}
	return meta;	
}

void evfs_metadata_db_record_stat(sqlite3* db, int file, struct stat* st)
{
	char query[1024];
	int ret;
	char* errMsg = 0;
	sqlite3_stmt *pStmt;
	
	snprintf(query, sizeof(query), "delete from FileStat where File = %d", file);
	ret = sqlite3_exec(db, query, NULL,0,&errMsg);
	if (errMsg) printf("ERROR: %s\n", errMsg);		

	snprintf(query, sizeof(query), 
		"insert into FileStat (file, tm_created, tm_modified, size, fowner, fgroup) select %d, ?, ?, ?, ?, ?", file);

	ret = sqlite3_prepare(db, query, -1, &pStmt, 0);

	if (ret == SQLITE_OK) {
		sqlite3_bind_int(pStmt, 1, st->st_ctime);
		sqlite3_bind_int(pStmt, 2, st->st_mtime);
		sqlite3_bind_int(pStmt, 3, st->st_size);
		sqlite3_bind_int(pStmt, 4, st->st_uid);
		sqlite3_bind_int(pStmt, 5, st->st_gid);

		if (sqlite3_step(pStmt) != SQLITE_DONE) {
			printf("evfs_metadata_db_record_stat: sqlite3 error\n");
		}
		sqlite3_reset(pStmt);
		sqlite3_finalize(pStmt);
	}
}
