#ifndef _EWL_DB_H_
#define _EWL_DB_H_ 1

#include "includes.h"
#include "list.h"
#include "util.h"

#define DB_DBM_HSEARCH  1
#include <db.h>
#include <netinet/in.h>

typedef struct _EwlDB EwlDB;
struct _EwlDB	{
	EwlListNode node;
	int         ref_count;
	DBM        *db;
	char       *path;

	char        error;
	char        writeable;
};

char    *ewl_db_strip_path(char *path);

EwlDB   *ewl_db_new();
void     ewl_db_free(EwlDB *db);
EwlDB   *ewl_db_open(char *file);
EwlDB   *ewl_db_open_writeable(char *file);
void     ewl_db_flush(EwlDB *db);
void     ewl_db_close(EwlDB *db);

void    *ewl_db_get(EwlDB *db, char *key, int *size_ret);
void     ewl_db_set(EwlDB *db, char *key, void *data, int size);
void     ewl_db_del(EwlDB *db, char *key);

char     ewl_db_int_get(EwlDB *db, char *key, int *val);
void     ewl_db_int_set(EwlDB *db, char *key, int val);
char     ewl_db_float_get(EwlDB *db, char *key, float *val);
void     ewl_db_float_set(EwlDB *db, char *key, float val);
char    *ewl_db_string_get(EwlDB *db, char *key);
void     ewl_db_string_set(EwlDB *db, char *key, char *str);

EwlList   *ewl_db_dump(EwlDB *db);

#endif /* _EWL_DB_H_ */

