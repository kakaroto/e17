#include "evfs.h"
#include <stdio.h>
#include <limits.h>
#include <sqlite3.h>
#include "evfs_metadata_db.h"


/* TODO
 * * Make a 'bulk-run' function to avoid the endless repeated sqlite3_exec
 */

#define EVFS_METADATA_DB_CONFIG_LATEST 3
static char metadata_db[PATH_MAX];
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
	char query[1024];

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

void evfs_metadata_db_init(sqlite3** db)
{
	struct stat config_dir_stat;
	int ret;
	int ver;
	int runs = 0;
	
	homedir = strdup(getenv("HOME"));
	snprintf(metadata_db, PATH_MAX, "%s/.e/evfs/evfs_metadata.db", homedir);

	db_upgrade_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
	ecore_hash_set(db_upgrade_hash, (int*)0, evfs_metadata_db_upgrade_0_1);
	ecore_hash_set(db_upgrade_hash, (int*)1, evfs_metadata_db_upgrade_1_2);
	ecore_hash_set(db_upgrade_hash, (int*)2, evfs_metadata_db_upgrade_2_3);
	
	/*Check if we need to seed the DB*/
	if (stat(metadata_db, &config_dir_stat)) {
		char* errMsg = 0;
		
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
