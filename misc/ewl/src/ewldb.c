#include "ewldb.h"

char    *ewl_db_strip_path(char *path)
{
	int   len = 0;
	char *np = NULL;
	FUNC_BGN("ewl_db_strip_path");
	if (!path||!strlen(path))	{
		ewl_debug("ewl_db_strip_path", EWL_NULL_ERROR, "path");
		FUNC_END("ewl_db_strip_path");
		return path;
	}
	len = strlen(path);
	if (len<3)	{
		FUNC_END("ewl_db_strip_path");
		return ewl_string_dup(path);
	}
	if (path[len-3]!='.'||
	    path[len-2]!='d'||
	    path[len-1]!='b')	{
		FUNC_END("ewl_db_strip_path");
		return ewl_string_dup(path);
	}

	/* FIXME -- fprintf(stderr,"WARNING: Stripping .db suffix.\n");*/

	path[len-3]=0;
	np = malloc(len-2);
	if (!np)	{
		ewl_debug("ewl_db_strip_path", EWL_NULL_ERROR, "np");
		path[len-3] = '.';
		return ewl_string_dup(path);
	}
	strcpy(np,path);
	path[len-3] = '.';
	FUNC_END("ewl_db_strip_path");
	return np;
}

EwlDB   *ewl_db_new()
{
	EwlDB *db = NULL;
	FUNC_BGN("ewl_db_new");
	db = malloc(sizeof(EwlDB));
	if (!db)	{
		ewl_debug("ewl_db_new", EWL_NULL_ERROR, "db");
	} else {
		/* initialize db here */
		db->node.data = NULL;
		db->node.next = NULL;
		db->db = NULL;
		db->error = 0;
		db->writeable = 0;
	}
	FUNC_END("ewl_db_new");
	return db;
}

void     ewl_db_free(EwlDB *db)
{
	FUNC_BGN("ewl_db_free");
	if (db)	{
		if (db->path) free (db->path);
		free(db);
		db = NULL;
	} else {
		ewl_debug("ewl_db_free", EWL_NULL_ERROR, "db");
	}
	FUNC_BGN("ewl_db_free");
	return;
}

EwlDB   *ewl_db_open(char *file)
{
	EwlDB *db = NULL;
	FUNC_BGN("ewl_db_open");
	if (!file)	{
		ewl_debug("ewl_db_open", EWL_NULL_ERROR, "file");
		FUNC_END("ewl_db_open");
		return NULL;
	}
	db = ewl_db_new();
	if (!db)	{
		ewl_debug("ewl_db_open", EWL_NULL_ERROR, "db");
		FUNC_END("ewl_db_open");
		return NULL;
	}

	db->path = ewl_db_strip_path(file);
	db->db = dbm_open(db->path, O_RDONLY, 0644);
	if (!db->db)	{
		ewl_debug("ewl_db_open", EWL_FILE_NOT_FOUND_ERROR, file);
		ewl_db_free(db);
	} else {
		db->writeable = 0;
		db->ref_count = 1;
		db->error = 0;
	}

	FUNC_END("ewl_db_open");
	return db;
}

EwlDB   *ewl_db_open_writeable(char *file)
{
	EwlDB *db = NULL;
	FUNC_BGN("ewl_db_open_writeable");
	if (!file)	{
		ewl_debug("ewl_db_open_writeable", EWL_NULL_ERROR, "file");
		FUNC_END("ewl_db_open_writeable");
		return NULL;
	}
	db = ewl_db_new();
	if (!db)	{
		ewl_debug("ewl_db_open_writeable", EWL_NULL_ERROR, "db");
		FUNC_END("ewl_db_open_writeable");
		return NULL;
	}

	db->path = ewl_db_strip_path(file);
	db->db = dbm_open(db->path, O_RDWR|O_CREAT, 0644);
	if (!db->db)	{
		ewl_debug("ewl_db_open_writeable", EWL_FILE_NOT_FOUND_ERROR, file);
		ewl_db_free(db);
	} else {
		db->writeable = 1;
		db->ref_count = 1;
		db->error = 0;
	}

	FUNC_END("ewl_db_open_writeable");
	return db;
}

void     ewl_db_flush(EwlDB *db)
{
	UNUSED(db);
	FUNC_BGN("ewl_db_flush");
	/* do I even need this badboy? */
	FUNC_END("ewl_db_flush");
	return;
}

void     ewl_db_close(EwlDB *db)
{
	FUNC_BGN("ewl_db_close");
	if (!db)	{
		ewl_debug("ewl_db_close", EWL_NULL_ERROR, "db");
	} else {
		db->ref_count--;
		if (db->ref_count<1)	{
			dbm_close(db->db);
			ewl_db_free(db);
		}
	}
	FUNC_END("ewl_db_close");
	return;
}

void    *ewl_db_get(EwlDB *db, char *key, int *size_ret)
{
	datum                    dkey, dval;
	FUNC_BGN("ewl_db_get");
	if (!db)	{
		ewl_debug("ewl_db_get", EWL_NULL_ERROR, "db");
	} else if (!key) {
		ewl_debug("ewl_db_get", EWL_NULL_ERROR, "key");
	} else if (!size_ret) {
		ewl_debug("ewl_db_get", EWL_NULL_ERROR, "size_ret");
	} else {
		dkey.dptr = key;
		dkey.dsize = strlen(key)+1;
		dval = dbm_fetch(db->db, dkey);
		*size_ret = dval.dsize;
		return ewl_string_dup(dval.dptr);
	}
	if (size_ret) *size_ret=0;
	FUNC_END("ewl_db_get");
	return NULL;
}

void     ewl_db_set(EwlDB *db, char *key, void *data, int size)
{
	datum                    dkey, dval;
	FUNC_BGN("ewl_db_set");
	if (!db)	{
		ewl_debug("ewl_db_set", EWL_NULL_ERROR, "db");
	} else if (!key) {
		ewl_debug("ewl_db_set", EWL_NULL_ERROR, "key");
	} else if (!data) {
		ewl_debug("ewl_db_set", EWL_NULL_ERROR, "data");
	} else if (!db->writeable) {
		ewl_debug("ewl_db_set", EWL_NULL_ERROR, "db->writeable");
	} else {
		dkey.dptr = key;
		dkey.dsize = strlen(key)+1;
		dval.dptr = data;
		dval.dsize = size;
		dbm_store(db->db, dkey, dval, DBM_REPLACE);
	}
	FUNC_END("ewl_db_set");
	return;
}

void     ewl_db_del(EwlDB *db, char *key)
{
	datum                    dkey;
	FUNC_BGN("ewl_db_set");
	if (!db)	{
		ewl_debug("ewl_db_set", EWL_NULL_ERROR, "db");
	} else if (!key) {
		ewl_debug("ewl_db_set", EWL_NULL_ERROR, "key");
	} else {
		dkey.dptr = key;
		dkey.dsize = strlen(key)+1;
		dbm_delete(db->db, dkey);
	}
	FUNC_END("ewl_db_set");
	return;
}

char     ewl_db_int_get(EwlDB *db, char *key, int *val)
{
	int      r = 0, *dval, size;
	FUNC_BGN("ewl_db_int_get");
	dval = ewl_db_get(db,key,&size);
	if (dval)	{
		*val = ntohl(*dval);
		r = 1;
	}
	FUNC_END("ewl_db_int_get");
	return r;
}

void     ewl_db_int_set(EwlDB *db, char *key, int val)
{
	int      lval;
	FUNC_BGN("ewl_db_int_set");
	lval = htonl(val);
	ewl_db_set(db,key,&lval,sizeof(int));
	FUNC_END("ewl_db_int_set");
	return;
}

char     ewl_db_float_get(EwlDB *db, char *key, float *val)
{
	int      r = 0, size;
	char    *dval;
	FUNC_BGN("ewl_db_float_get");
	dval = ewl_db_get(db,key,&size);
	if (dval)	{
		*val = atof(dval);
		r = 1;
	}
	FUNC_END("ewl_db_float_get");
	return r;
}

void     ewl_db_float_set(EwlDB *db, char *key, float val)
{
	char    buf[256] = "";
	FUNC_BGN("ewl_db_float_set");
	sprintf(buf, "%f", val);
	ewl_db_string_set(db,key,buf);
	FUNC_END("ewl_db_float_set");
	return;
}

char    *ewl_db_string_get(EwlDB *db, char *key)
{
	char  *s, *data;
	int    size = 0;
	FUNC_BGN("ewl_db_string_get");
	data = ewl_db_get(db,key,&size);
	if (!data)
		return NULL;
	s = malloc(size+1);
	if (!s)	
		return NULL;
	memcpy(s, data, size+1);
	s[size] = 0;
	FUNC_END("ewl_db_string_get");
	return s;
}

void     ewl_db_string_set(EwlDB *db, char *key, char *str)
{
	FUNC_BGN("ewl_db_string_set");
	ewl_db_set(db,key,str,strlen(str)+1);
	FUNC_END("ewl_db_string_set");
	return;
}

EwlList *ewl_db_dump(EwlDB *db)
{
	char    *buf = NULL;
	EwlList *ll = NULL;
	datum    key;
	FUNC_BGN("ewl_db_dump");
	if (!db)	{
		ewl_debug("ewl_db_dump", EWL_NULL_ERROR, "db");
		FUNC_END("ewl_db_dump");
		return NULL;
	}

	key = dbm_firstkey(db->db);
	while (key.dptr)	{
		buf = malloc(key.dsize+1);
		if (!buf)	{
			ewl_debug("ewl_db_dump", EWL_NULL_ERROR, "buf");
			FUNC_END("ewl_db_dump");
			return NULL;
		}
		memcpy(buf, key.dptr, key.dsize);
		buf[key.dsize] = 0;
		ewl_list_insert(ll, ewl_list_node_new(buf));
		key = dbm_nextkey(db->db);
	}
	FUNC_END("ewl_db_dump");
	return ll;
}
