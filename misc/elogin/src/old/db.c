#include "first.h"
#include "db.h"
#include "mem.h"
#include "util.h"
#include "str.h"
#include <gdbm.h>
#include <netinet/in.h>

typedef struct _e_db_file _E_DB_File;

struct _e_db_file
{
   char               *file;
   GDBM_FILE           dbf;
   char                writeable;
   int                 references;
   _E_DB_File         *next;
};

static _E_DB_File  *e_db_find(char *file, char writeable);
static void         _e_db_close(E_DB_File * db);

static _E_DB_File  *dbs = NULL;
static int          max_db_count = 32;
static pid_t        main_process_pid = 0;
static double       last_db_call = 0.0;
static int          flush_pending = 0;

static _E_DB_File  *
e_db_find(char *file, char writeable)
{
   _E_DB_File         *ptr;
   static int          db_init = 0;

   if (!db_init)
     {
	main_process_pid = getpid();
	atexit(e_db_flush);
	db_init = 1;
     }
   for (ptr = dbs; ptr; ptr = ptr->next)
     {
	if ((e_string_cmp(file, ptr->file)) &&
	    ((!writeable) || (ptr->writeable)))
	  {
	     ptr->references++;
	     return ptr;
	  }
     }
   if (writeable)
     {
	_E_DB_File         *dbf;

	dbf = dbs;
	while (dbf)
	  {
	     _E_DB_File         *dbf2;

	     dbf2 = dbf;
	     dbf = dbf->next;
	     if ((e_string_cmp(file, dbf2->file)) && (dbf2->references == 0))
	       {
		  dbf2->references = -1;
		  _e_db_close((E_DB_File *) dbf2);
	       }
	  }
     }
   return NULL;
}

E_DB_File          *
e_db_open(char *file)
{
   _E_DB_File         *dbf;
   GDBM_FILE           db;
   int                 dbs_count = 0;

   dbf = e_db_find(file, 1);
   if (dbf)
      return (E_DB_File *) dbf;
   for (dbf = dbs; dbf; dbf = dbf->next)
     {
	if (dbf->references == 0)
	   dbs_count++;
     }
   while (dbs_count > max_db_count)
     {
	_E_DB_File         *dbf_last;

	dbf_last = NULL;
	for (dbf = dbs; dbf; dbf = dbf->next)
	  {
	     if (dbf->references == 0)
		dbf_last = dbf;
	  }
	if (dbf_last)
	  {
	     dbf_last->references = -1;
	     _e_db_close((E_DB_File *) dbf_last);
	     dbs_count--;
	  }
     }
   db = gdbm_open(file, 0, GDBM_WRCREAT | GDBM_FAST, 0664, NULL);
   if (!db)
      db = gdbm_open(file, 0, GDBM_WRCREAT, 0664, NULL);
   while ((!db) && (e_file_can_write(file)))
     {
	db = gdbm_open(file, 0, GDBM_WRCREAT | GDBM_FAST, 0664, NULL);
	usleep(200000);
     }
   if (db)
     {
	int                 cache_size = 32;
	int                 fastmode = 1;

	gdbm_setopt(db, GDBM_CACHESIZE, &cache_size, sizeof(int));
	gdbm_setopt(db, GDBM_FASTMODE, &fastmode, sizeof(int));

	dbf = NEW(_E_DB_File, 1);
	dbf->file = e_string_dup(file);
	dbf->dbf = db;
	dbf->writeable = 1;
	dbf->references = 1;
	dbf->next = dbs;
	dbs = dbf;
	last_db_call = e_get_time();
	flush_pending = 1;
	return (E_DB_File *) dbf;
     }
   return NULL;
}

E_DB_File          *
e_db_open_read(char *file)
{
   _E_DB_File         *dbf;
   GDBM_FILE           db;
   int                 dbs_count = 0;

   dbf = e_db_find(file, 0);
   if (dbf)
      return (E_DB_File *) dbf;
   dbf = e_db_find(file, 1);
   if (dbf)
      return (E_DB_File *) dbf;

   for (dbf = dbs; dbf; dbf = dbf->next)
     {
	if (dbf->references == 0)
	   dbs_count++;
     }
   while (dbs_count > max_db_count)
     {
	_E_DB_File         *dbf_last;

	dbf_last = NULL;
	for (dbf = dbs; dbf; dbf = dbf->next)
	  {
	     if (dbf->references == 0)
		dbf_last = dbf;
	  }
	if (dbf_last)
	  {
	     dbf_last->references = -1;
	     _e_db_close((E_DB_File *) dbf_last);
	     dbs_count--;
	  }
     }
   db = gdbm_open(file, 0, GDBM_READER, 0664, NULL);
   while ((!db) && (e_file_can_read(file)))
     {
	db = gdbm_open(file, 0, GDBM_READER, 0664, NULL);
	usleep(200000);
     }
   if (db)
     {
	int                 cache_size = 32;
	int                 fastmode = 1;

	gdbm_setopt(db, GDBM_CACHESIZE, &cache_size, sizeof(int));
	gdbm_setopt(db, GDBM_FASTMODE, &fastmode, sizeof(int));

	dbf = NEW(_E_DB_File, 1);
	dbf->file = e_string_dup(file);
	dbf->dbf = db;
	dbf->writeable = 0;
	dbf->references = 1;
	dbf->next = dbs;
	dbs = dbf;
	last_db_call = e_get_time();
	flush_pending = 1;
	return (E_DB_File *) dbf;
     }
   return NULL;
}

static void
_e_db_close(E_DB_File * db)
{
   _E_DB_File         *dbf;

   dbf = (_E_DB_File *) db;
   if (dbf->references > 0)
      dbf->references--;
   if (dbf->references < 0)
     {
	_E_DB_File         *ptr, *pptr;

	pptr = NULL;
	for (ptr = dbs; ptr; ptr = ptr->next)
	  {
	     if (ptr == dbf)
	       {
		  if (pptr)
		     pptr->next = ptr->next;
		  else
		     dbs = ptr->next;
		  IF_FREE(dbf->file);
		  gdbm_close(dbf->dbf);
		  FREE(dbf);
		  return;
	       }
	     pptr = ptr;
	  }
     }
}

void
e_db_close(E_DB_File * db)
{
   _e_db_close(db);
}

void
e_db_flush(void)
{
   _E_DB_File         *dbf;

   if ((main_process_pid) && (main_process_pid != getpid()))
      return;
   dbf = dbs;
   while (dbf)
     {
	_E_DB_File         *dbf2;

	dbf2 = dbf;
	dbf = dbf->next;
	if (dbf2->references == 0)
	  {
	     dbf2->references = -1;
	     _e_db_close((E_DB_File *) dbf2);
	  }
     }
}

int
e_db_runtime_flush(void)
{
   double              now;

   if (!flush_pending)
      return;
   now = e_get_time();
   if (now - last_db_call > 0.5)
     {
	e_db_flush();
	flush_pending = 0;
	return 1;
     }
   return 0;
}

void               *
e_db_data_get(E_DB_File * db, char *key, int *size_ret)
{
   _E_DB_File         *dbf;
   datum               dkey, ret;

   dbf = (_E_DB_File *) db;
   dkey.dptr = key;
   dkey.dsize = e_string_length(key);
   ret.dptr = NULL;
   ret = gdbm_fetch(dbf->dbf, dkey);
   if (ret.dptr)
     {
	*size_ret = ret.dsize;
	return ret.dptr;
     }
   *size_ret = 0;
   last_db_call = e_get_time();
   flush_pending = 1;
   return NULL;
}

void
e_db_data_set(E_DB_File * db, char *key, void *data, int size)
{
   _E_DB_File         *dbf;
   datum               dkey, dat;

   dbf = (_E_DB_File *) db;
   dkey.dptr = key;
   dkey.dsize = e_string_length(key);
   dat.dptr = data;
   dat.dsize = size;
   gdbm_store(dbf->dbf, dkey, dat, GDBM_REPLACE);
   last_db_call = e_get_time();
   flush_pending = 1;
}

void
e_db_data_del(E_DB_File * db, char *key)
{
   _E_DB_File         *dbf;
   datum               dkey, dat;

   dbf = (_E_DB_File *) db;
   dkey.dptr = key;
   dkey.dsize = e_string_length(key);
   gdbm_delete(dbf->dbf, dkey);
   last_db_call = e_get_time();
   flush_pending = 1;
}

void
e_db_int_set(E_DB_File * db, char *key, int val)
{
   int                 v;

   v = htonl(val);
   e_db_data_set(db, key, &v, sizeof(int));
}

int
e_db_int_get(E_DB_File * db, char *key, int *val)
{
   int                *dat;
   int                 size;
   int                 v;

   dat = e_db_data_get(db, key, &size);
   if (!dat)
      return 0;
   v = ntohl(*dat);
   *val = v;
   FREE(dat);
   return 1;
}

void
e_db_float_set(E_DB_File * db, char *key, float val)
{
   char                buf[256];

   e_db_str_set(db, key, buf);
}

int
e_db_float_get(E_DB_File * db, char *key, float *val)
{
   char               *dat;
   int                 size;

   dat = e_db_str_get(db, key);
   if (!dat)
      return 0;
   *val = atof(dat);
   FREE(dat);
   return 1;
}

void
e_db_str_set(E_DB_File * db, char *key, char *str)
{
   e_db_data_set(db, key, str, e_string_length(str));
}

char               *
e_db_str_get(E_DB_File * db, char *key)
{
   char               *dat, *s;
   int                 size;

   dat = e_db_data_get(db, key, &size);
   if (!dat)
      return NULL;
   s = NEW(char, size + 1);
   MEMCPY(dat, s, char, size);

   s[size] = 0;
   FREE(dat);
   return s;
}

char              **
e_db_dump_multi_field(char *file, char *file2, int *num_ret)
{
   E_DB_File          *db1, *db2;
   _E_DB_File         *dbf1, *dbf2;
   datum               ret, key;
   char              **list = NULL;

   *num_ret = 0;

   db1 = e_db_open_read(file);
   dbf1 = db1;
   /* load prioirty database in first */
   if (dbf1)
     {
	key = gdbm_firstkey(dbf1->dbf);
	while (key.dptr)
	  {
	     (*num_ret)++;
	     if (list)
	       {
		  REALLOC_PTR(list, *num_ret);
	       }
	     else
		list = NEW_PTR(1);
	     list[*num_ret - 1] = NEW(char, key.dsize + 1);
	     MEMCPY(key.dptr, list[*num_ret - 1], char, key.dsize);

	     list[*num_ret - 1][key.dsize] = 0;
	     ret = gdbm_fetch(dbf1->dbf, key);
	     (*num_ret)++;
	     REALLOC_PTR(list, *num_ret);
	     list[*num_ret - 1] = NEW(char, ret.dsize + 1);
	     MEMCPY(ret.dptr, list[*num_ret - 1], char, ret.dsize);

	     list[*num_ret - 1][ret.dsize] = 0;
	     key = gdbm_nextkey(dbf1->dbf, key);
	  }
	e_db_close(db1);
     }
   /* go through database 2 and if the keys dont exist alreayd in list - add */
   if (file2)
     {
	db2 = e_db_open_read(file2);
	dbf2 = db2;
     }
   else
      dbf2 = NULL;
   if (dbf2)
     {
	int                 i;

	key = gdbm_firstkey(dbf2->dbf);
	while (key.dptr)
	  {
	     char               *s;
	     int                 ok = 1;

	     s = NEW(char, key.dsize + 1);
	     MEMCPY(key.dptr, s, char, key.dsize);

	     s[key.dsize] = 0;
	     for (i = 0; (i < *num_ret) && (ok); i++)
	       {
		  if (e_string_case_cmp(s, list[i]))
		     ok = 0;
	       }
	     if (ok)
	       {
		  (*num_ret)++;
		  if (list)
		    {
		       REALLOC_PTR(list, *num_ret);
		    }
		  else
		     list = NEW_PTR(1);
		  list[*num_ret - 1] = NEW(char, key.dsize + 1);
		  MEMCPY(key.dptr, list[*num_ret - 1], char, key.dsize);

		  list[*num_ret - 1][key.dsize] = 0;
		  ret = gdbm_fetch(dbf1->dbf, key);
		  (*num_ret)++;
		  REALLOC_PTR(list, *num_ret);
		  list[*num_ret - 1] = NEW(char, ret.dsize + 1);
		  MEMCPY(ret.dptr, list[*num_ret - 1], char, ret.dsize);

		  list[*num_ret - 1][ret.dsize] = 0;
		  key = gdbm_nextkey(dbf1->dbf, key);
	       }
	     FREE(s);
	  }
	e_db_close(db2);
     }
   last_db_call = e_get_time();
   flush_pending = 1;
   return list;
}

char              **
e_db_dump_key_list(char *file, int *num_ret)
{
   E_DB_File          *db;
   _E_DB_File         *dbf;
   datum               ret, key;
   char              **list = NULL;

   *num_ret = 0;

   db = e_db_open_read(file);
   dbf = db;
   if (dbf)
     {
	key = gdbm_firstkey(dbf->dbf);
	while (key.dptr)
	  {
	     (*num_ret)++;
	     if (list)
	       {
		  REALLOC_PTR(list, *num_ret);
	       }
	     else
		list = NEW_PTR(1);
	     list[*num_ret - 1] = NEW(char, key.dsize + 1);
	     MEMCPY(key.dptr, list[*num_ret - 1], char, key.dsize);

	     list[*num_ret - 1][key.dsize] = 0;
	     key = gdbm_nextkey(dbf->dbf, key);
	  }
	e_db_close(db);
     }
   last_db_call = e_get_time();
   flush_pending = 1;
   return list;
}
