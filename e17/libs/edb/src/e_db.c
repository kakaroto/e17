#include "Edb.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>

#define DB_DBM_HSEARCH 1
#include "db.h"

#define NEW(dat, num) malloc(sizeof(dat) * (num))
#define FREE(dat) {if (dat) {free(dat); dat = NULL; } else {printf("eek - NULL free\n");sleep(30); } }
#define IF_FREE(dat) {if (dat) FREE(dat);}
#define MEMCPY(src, dst, type, num) memcpy(dst, src, sizeof(type) * (num))
#define REALLOC_PTR(dat, num) {if (dat) dat = realloc(dat, sizeof(void *) * (num)); else dat = malloc(sizeof(void *) * (num));}
#define NEW_PTR(num) malloc(sizeof(void *) * (num))


typedef struct _e_db_file _E_DB_File;

struct _e_db_file
{
   char               *file;
   DBM                *dbf;
   char                writeable;
   int                 references;
   _E_DB_File         *next;
};

static _E_DB_File  *_e_db_find(char *file, char writeable);
static void         _e_db_close(E_DB_File * db);

static _E_DB_File  *dbs = NULL;
static int          max_db_count = 32;
static double       last_db_call = 0.0;
static int          flush_pending = 0;

/* internal routines */

static double
_e_get_time(void)
{
   struct timeval      timev;
   
   gettimeofday(&timev, NULL);
   return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}


static _E_DB_File  *
_e_db_find(char *file, char writeable)
{
   _E_DB_File         *ptr;
   static int          db_init = 0;

   if (!db_init)
     {
	atexit(e_db_flush);
	db_init = 1;
     }
   for (ptr = dbs; ptr; ptr = ptr->next)
     {
	if ((!strcmp(file, ptr->file)) &&
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
	     if ((!strcmp(file, dbf2->file)) && (dbf2->references == 0))
	       {
		  dbf2->references = -1;
		  _e_db_close((E_DB_File *) dbf2);
	       }
	  }
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
		  dbm_close(dbf->dbf);
		  FREE(dbf);
		  return;
	       }
	     pptr = ptr;
	  }
     }
}

/* public routines in api */

E_DB_File          *
e_db_open(char *file)
{
   _E_DB_File         *dbf;
   DBM                *db = NULL;
   int                 dbs_count = 0;

   dbf = _e_db_find(file, 1);
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
   {
      char               *newfile = NULL;
      int                 newfilelen;

      newfile = strdup(file);
      if (newfile)
	{
	   newfilelen = strlen(newfile);
	   if ((newfilelen > 3) &&
	       (newfile[newfilelen - 3] == '.') &&
	       (newfile[newfilelen - 2] == 'd') &&
	       (newfile[newfilelen - 1] == 'b'))
	      newfile[newfilelen - 3] = 0;
	   
	   db = dbm_open(newfile, O_RDWR | O_CREAT, 0664);
	   FREE(newfile);
	}
   }
   if (db)
     {
	dbf = NEW(_E_DB_File, 1);
	dbf->file = strdup(file);
	dbf->dbf = db;
	dbf->writeable = 1;
	dbf->references = 1;
	dbf->next = dbs;
	dbs = dbf;
	last_db_call = _e_get_time();
	flush_pending = 1;
	return (E_DB_File *) dbf;
     }
   return NULL;
}

E_DB_File          *
e_db_open_read(char *file)
{
   _E_DB_File         *dbf;
   DBM                *db = NULL;
   int                 dbs_count = 0;

   dbf = _e_db_find(file, 0);
   if (dbf)
      return (E_DB_File *) dbf;
   dbf = _e_db_find(file, 1);
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
   {
      char               *newfile = NULL;
      int                 newfilelen;

      newfile = strdup(file);
      if (newfile)
	{
	   newfilelen = strlen(newfile);
	   if ((newfilelen > 3) &&
	       (newfile[newfilelen - 3] == '.') &&
	       (newfile[newfilelen - 2] == 'd') &&
	       (newfile[newfilelen - 1] == 'b'))
	      newfile[newfilelen - 3] = 0;

	   db = dbm_open(newfile, O_RDONLY, 0664);
	   FREE(newfile);
	}
   }
   if (db)
     {
	dbf = NEW(_E_DB_File, 1);
	dbf->file = strdup(file);
	dbf->dbf = db;
	dbf->writeable = 0;
	dbf->references = 1;
	dbf->next = dbs;
	dbs = dbf;
	last_db_call = _e_get_time();
	flush_pending = 1;
	return (E_DB_File *) dbf;
     }
   return NULL;
}

int
e_db_usage(void)
{
   _E_DB_File *dbf;
   int dbs_count = 0;
   
   for (dbf = dbs; dbf; dbf = dbf->next)
      dbs_count++;
   return dbs_count;
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
      return 0;
   now = _e_get_time();
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
   dkey.dsize = strlen(key);
   ret.dptr = NULL;
   ret = dbm_fetch(dbf->dbf, dkey);
   last_db_call = _e_get_time();
   flush_pending = 1;
   if (ret.dptr)
     {
	void *data;
	
	*size_ret = ret.dsize;
	data = malloc(ret.dsize);
	if (data)
	  {
	     memcpy(data, ret.dptr, ret.dsize);
	     return data;
	  }
     }
   *size_ret = 0;
   return NULL;
}

void
e_db_data_set(E_DB_File * db, char *key, void *data, int size)
{
   _E_DB_File         *dbf;
   datum               dkey, dat;

   dbf = (_E_DB_File *) db;
   dkey.dptr = key;
   dkey.dsize = strlen(key);
   dat.dptr = data;
   dat.dsize = size;
   dbm_store(dbf->dbf, dkey, dat, DBM_REPLACE);
   last_db_call = _e_get_time();
   flush_pending = 1;
}

void
e_db_data_del(E_DB_File * db, char *key)
{
   char               *key2;
   _E_DB_File         *dbf;
   datum               dkey;
   int                 len;

   dbf = (_E_DB_File *) db;
   dkey.dptr = key;
   dkey.dsize = len = strlen(key);
   dbm_delete(dbf->dbf, dkey);
   
   key2 = malloc(strlen(key) + 2);
   if (!key2) return;
   key2[0] = 0;
   strcpy(&(key2[1]), key);
   dkey.dptr = key2;
   dkey.dsize = len + 1;
   dbm_delete(dbf->dbf, dkey);
   free(key2);
   
   last_db_call = _e_get_time();
   flush_pending = 1;
}

void
e_db_int_set(E_DB_File * db, char *key, int val)
{
   int                 v;

   v = htonl(val);
   e_db_data_set(db, key, &v, sizeof(int));
   e_db_type_set(db, key, "int");
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

   sprintf(buf, "%f", val);
   e_db_str_set(db, key, buf);
   e_db_type_set(db, key, "float");
   return;
   val = 0.0;
}

int
e_db_float_get(E_DB_File * db, char *key, float *val)
{
   char               *dat;

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
   e_db_data_set(db, key, str, strlen(str));
   e_db_type_set(db, key, "str");
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

void
e_db_type_set(E_DB_File * db, char *key, char *type)
{
   char               *key2;
   _E_DB_File         *dbf;
   datum               dkey, dat;
   
   key2 = malloc(strlen(key) + 2);
   if (!key2) return;
   key2[0] = 0;
   strcpy(&(key2[1]), key);
   dbf = (_E_DB_File *) db;
   dkey.dptr = key2;
   dkey.dsize = strlen(key) + 1;
   dat.dptr = type;
   dat.dsize = strlen(type);
   dbm_store(dbf->dbf, dkey, dat, DBM_REPLACE);
   free(key2);
   last_db_call = _e_get_time();
   flush_pending = 1;
}

char *
e_db_type_get(E_DB_File * db, char *key)
{
   char               *key2;
   _E_DB_File         *dbf;
   datum               dkey, ret;

   key2 = malloc(strlen(key) + 2);
   if (!key2) return NULL;
   key2[0] = 0;
   strcpy(&(key2[1]), key);
   dbf = (_E_DB_File *) db;
   dkey.dptr = key2;
   dkey.dsize = strlen(key) + 1;
   ret.dptr = NULL;
   ret = dbm_fetch(dbf->dbf, dkey);
   free(key2);
   last_db_call = _e_get_time();
   flush_pending = 1;
   if (ret.dptr)
     {
	char *data;
	
	data = malloc(ret.dsize + 1);
	if (data)
	  {
	     memcpy(data, ret.dptr, ret.dsize);
	     data[ret.dsize] = 0;
	     return (char *)data;
	  }
     }
   return NULL;
}

char              **
e_db_dump_multi_field(char *file, char *file2, int *num_ret)
{
   E_DB_File          *db1 = NULL, *db2 = NULL;
   _E_DB_File         *dbf1, *dbf2;
   datum               ret, key;
   char              **list = NULL;

   *num_ret = 0;

   db1 = e_db_open_read(file);
   dbf1 = db1;
   /* load prioirty database in first */
   if (dbf1)
     {
	key = dbm_firstkey(dbf1->dbf);
	while (key.dptr)
	  {
	     if (*((char *)(key.dptr)) != 0)
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
		  ret = dbm_fetch(dbf1->dbf, key);
		  (*num_ret)++;
		  REALLOC_PTR(list, *num_ret);
		  list[*num_ret - 1] = NEW(char, ret.dsize + 1);
		  MEMCPY(ret.dptr, list[*num_ret - 1], char, ret.dsize);
		  
		  list[*num_ret - 1][ret.dsize] = 0;
		  key = dbm_nextkey(dbf1->dbf);
	       }
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

	key = dbm_firstkey(dbf2->dbf);
	while (key.dptr)
	  {
	     char               *s;
	     int                 ok = 1;

	     if (*((char *)(key.dptr)) != 0)
	       {
		  s = NEW(char, key.dsize + 1);
		  MEMCPY(key.dptr, s, char, key.dsize);
		  
		  s[key.dsize] = 0;
		  for (i = 0; (i < *num_ret) && (ok); i++)
		    {
		       if (strcasecmp(s, list[i]))
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
		       ret = dbm_fetch(dbf1->dbf, key);
		       (*num_ret)++;
		       REALLOC_PTR(list, *num_ret);
		       list[*num_ret - 1] = NEW(char, ret.dsize + 1);
		       MEMCPY(ret.dptr, list[*num_ret - 1], char, ret.dsize);
		       
		       list[*num_ret - 1][ret.dsize] = 0;
		       key = dbm_nextkey(dbf1->dbf);
		    }
		  FREE(s);
	       }
	  }
	e_db_close(db2);
     }
   last_db_call = _e_get_time();
   flush_pending = 1;
   return list;
}

char              **
e_db_dump_key_list(char *file, int *num_ret)
{
   E_DB_File          *db;
   _E_DB_File         *dbf;
   datum               key;
   char              **list = NULL;

   *num_ret = 0;

   db = e_db_open_read(file);
   dbf = db;
   if (dbf)
     {
	key = dbm_firstkey(dbf->dbf);
	while (key.dptr)
	  {
	     if (*((char *)(key.dptr)) != 0)
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
	       }
	     key = dbm_nextkey(dbf->dbf);
	  }
	e_db_close(db);
     }
   last_db_call = _e_get_time();
   flush_pending = 1;
   return list;
}
