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
#include <fnmatch.h>

#define DB_DBM_HSEARCH 1
#include "edb_common.h"

#define NEW(dat, num) malloc(sizeof(dat) * (num))
#define FREE(dat) {if (dat) {free(dat); dat = NULL; } else {printf("eek - NULL free(%s @ %u)\n", __FILE__, __LINE__);sleep(30); } }
#define IF_FREE(dat) {if (dat) FREE(dat);}
#define MEMCPY(src, dst, type, num) memcpy(dst, src, sizeof(type) * (num))
#define REALLOC_PTR(dat, num) {if (dat) dat = realloc(dat, sizeof(void *) * (num)); else dat = malloc(sizeof(void *) * (num));}
#define NEW_PTR(num) malloc(sizeof(void *) * (num))


typedef struct _e_db_file _E_DB_File;

struct _e_db_file
{
   char               *file;
   DBM                *edbf;
   char                writeable;
   int                 references;
   _E_DB_File         *next;
};

static _E_DB_File  *_e_db_find(const char *file, char writeable);
static void         _e_db_close(E_DB_File * edb);

void        *_e_db_data_get(E_DB_File * edb, datum dkey, int *size_ret);
void         _e_db_data_set(E_DB_File * edb, datum dkey, const void *data, int size);


static _E_DB_File  *edbs = NULL;
static int          max_edb_count = 32;
static double       last_edb_call = 0.0;
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
_e_db_find(const char *file, char writeable)
{
   _E_DB_File         *ptr;
   static int          edb_init = 0;

   if (!edb_init)
     {
//	atexit(e_db_flush);
	edb_init = 1;
     }
   for (ptr = edbs; ptr; ptr = ptr->next)
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
	_E_DB_File         *edbf;

	edbf = edbs;
	while (edbf)
	  {
	     _E_DB_File         *edbf2;

	     edbf2 = edbf;
	     edbf = edbf->next;
	     if ((!strcmp(file, edbf2->file)) && (edbf2->references == 0))
	       {
		  edbf2->references = -1;
		  _e_db_close((E_DB_File *) edbf2);
	       }
	  }
     }
   return NULL;
}


static void
_e_db_close(E_DB_File * edb)
{
   _E_DB_File         *edbf;

   edbf = (_E_DB_File *) edb;
   if (edbf->references > 0)
      edbf->references--;
   if (edbf->references < 0)
     {
	_E_DB_File         *ptr, *pptr;

	pptr = NULL;
	for (ptr = edbs; ptr; ptr = ptr->next)
	  {
	     if (ptr == edbf)
	       {
		  if (pptr)
		     pptr->next = ptr->next;
		  else
		     edbs = ptr->next;

		  IF_FREE(edbf->file);
		  edbm_close(edbf->edbf);
		  FREE(edbf);
		  return;
	       }
	     pptr = ptr;
	  }
     }
}

void               *
_e_db_data_get(E_DB_File * edb, datum dkey, int *size_ret)
{
   _E_DB_File         *edbf;
   datum               ret;

   edbf = (_E_DB_File *) edb;
   ret.dptr = NULL;
   ret = edbm_fetch(edbf->edbf, dkey);
   last_edb_call = _e_get_time();
   flush_pending = 1;
   if (ret.dptr)
     {
	void *data;
	
	if (ret.dsize == 0) return NULL;
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
_e_db_data_set(E_DB_File * edb, datum dkey, const void *data, int size)
{
   _E_DB_File         *edbf;
   datum               dat;

   edbf = (_E_DB_File *) edb;
   dat.dptr = (char*)data;
   dat.dsize = size;
   edbm_store(edbf->edbf, dkey, dat, DBM_REPLACE);
   last_edb_call = _e_get_time();
   flush_pending = 1;
}


/* public routines in api */

E_DB_File          *
e_db_open(const char *file)
{
  return e_db_open_mode(file, O_RDWR | O_CREAT);
}


E_DB_File          *
e_db_open_mode(const char *file, int flags)
{
   _E_DB_File         *edbf;
   DBM                *edb = NULL;
   int                 edbs_count = 0;

   edbf = _e_db_find(file, 1);
   if (edbf)
      return (E_DB_File *) edbf;
   for (edbf = edbs; edbf; edbf = edbf->next)
     {
	if (edbf->references == 0)
	   edbs_count++;
     }
   while (edbs_count > max_edb_count)
     {
	_E_DB_File         *edbf_last;

	edbf_last = NULL;
	for (edbf = edbs; edbf; edbf = edbf->next)
	  {
	     if (edbf->references == 0)
		edbf_last = edbf;
	  }
	if (edbf_last)
	  {
	     edbf_last->references = -1;
	     _e_db_close((E_DB_File *) edbf_last);
	     edbs_count--;
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
	   
	   edb = edbm_open(newfile, flags, 0664);
	   FREE(newfile);
	}
   }
   if (edb)
     {
	edbf = NEW(_E_DB_File, 1);
	edbf->file = strdup(file);
	edbf->edbf = edb;
	edbf->writeable = 1;
	edbf->references = 1;
	edbf->next = edbs;
	edbs = edbf;
	last_edb_call = _e_get_time();
	flush_pending = 1;
	return (E_DB_File *) edbf;
     }
   return NULL;
}

E_DB_File          *
e_db_open_read(const char *file)
{
   _E_DB_File         *edbf;
   DBM                *edb = NULL;
   int                 edbs_count = 0;

   edbf = _e_db_find(file, 0);
   if (edbf)
      return (E_DB_File *) edbf;
   edbf = _e_db_find(file, 1);
   if (edbf)
      return (E_DB_File *) edbf;

   for (edbf = edbs; edbf; edbf = edbf->next)
     {
	if (edbf->references == 0)
	   edbs_count++;
     }
   while (edbs_count > max_edb_count)
     {
	_E_DB_File         *edbf_last;

	edbf_last = NULL;
	for (edbf = edbs; edbf; edbf = edbf->next)
	  {
	     if (edbf->references == 0)
		edbf_last = edbf;
	  }
	if (edbf_last)
	  {
	     edbf_last->references = -1;
	     _e_db_close((E_DB_File *) edbf_last);
	     edbs_count--;
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

	   edb = edbm_open(newfile, O_RDONLY, 0664);
	   FREE(newfile);
	}
   }
   if (edb)
     {
	edbf = NEW(_E_DB_File, 1);
	edbf->file = strdup(file);
	edbf->edbf = edb;
	edbf->writeable = 0;
	edbf->references = 1;
	edbf->next = edbs;
	edbs = edbf;
	last_edb_call = _e_get_time();
	flush_pending = 1;
	return (E_DB_File *) edbf;
     }
   return NULL;
}

int
e_db_usage(void)
{
   _E_DB_File *edbf;
   int edbs_count = 0;
   
   for (edbf = edbs; edbf; edbf = edbf->next)
      edbs_count++;
   return edbs_count;
}

void
e_db_close(E_DB_File * edb)
{
   _e_db_close(edb);
}

void
e_db_flush(void)
{
   _E_DB_File         *edbf;

   edbf = edbs;
   while (edbf)
     {
	_E_DB_File         *edbf2;

	edbf2 = edbf;
	edbf = edbf->next;
	if (edbf2->references == 0)
	  {
	     edbf2->references = -1;
	     _e_db_close((E_DB_File *) edbf2);
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
   if (now - last_edb_call > 0.5)
     {
	e_db_flush();
	flush_pending = 0;
	return 1;
     }
   return 0;
}

void
e_db_property_set(E_DB_File *edb, const char *property, const char *value)
{
   char *pname;
   datum               dkey;

   pname = malloc(strlen(property) + (sizeof(char)*2));
   sprintf(pname, "%c%c%s", 0, 0, property); 

   dkey.dptr = pname;
   dkey.dsize = strlen(property)+2;

   _e_db_data_set(edb, dkey, value, strlen(value));

   free(pname);
}

char *
e_db_property_get(E_DB_File *edb, const char *property)
{
   char *pname;
   char *getdata, *returndata = NULL;
   int size = 0;
   datum dkey;

   pname = malloc(strlen(property) + (sizeof(char)*2));
   sprintf(pname, "%c%c%s", 0, 0, property);
   
   dkey.dptr = pname;
   dkey.dsize = strlen(property) + 2;

   getdata = _e_db_data_get(edb, dkey, &size);
   if (!getdata)
      return NULL;
   returndata = NEW(char, size + 1);
   MEMCPY(getdata, returndata, char, size);

   returndata[size] = 0;
   FREE(getdata);
   return returndata;
}

void
e_db_set_type(E_DB_File * edb, const char *value) {
   char *type;
   char *existing;

   if (!e_db_is_type(edb, value)) 
   {
      existing = e_db_property_get(edb, "E_DBType");
      if (existing != NULL) 
      {
         type = NEW(char, strlen(existing)+strlen(value)+2);
         sprintf(type, "%s%s/", existing, value);
         FREE(existing);
      }
      else
      {
         type = NEW(char, strlen(value)+2);
         sprintf(type, "/%s/", value);
      }
      
      e_db_property_set(edb, "E_DBType", type);
      FREE(type);
   }
}

int
e_db_is_type(E_DB_File * edb, const char *type) {
   char *existing;
   char *tok;

   existing = e_db_property_get(edb, "E_DBType");
   if (existing) 
   {
      tok = strtok(existing, "/");
      while (tok != NULL) {
         if (!strcasecmp(tok, type)) {
            FREE(existing);
            return 1;
         }
         tok = strtok(NULL, "/");
      }
      
      FREE(existing);
   }
   return 0;
}

void               *
e_db_data_get(E_DB_File * edb, const char *key, int *size_ret)
{
   datum dkey;

   dkey.dptr = (char*)key;
   dkey.dsize = strlen(key);

   return _e_db_data_get(edb, dkey, size_ret);
}

void
e_db_data_set(E_DB_File * edb, const char *key, const void *data, int size)
{
   datum               dkey;

   dkey.dptr = (char*)key;
   dkey.dsize = strlen(key);
   _e_db_data_set(edb, dkey, data, size);
}

void
e_db_data_del(E_DB_File * edb, const char *key)
{
   char               *key2;
   _E_DB_File         *edbf;
   datum               dkey;
   int                 len;

   edbf = (_E_DB_File *) edb;
   dkey.dptr = (char*)key;
   dkey.dsize = len = strlen(key);
   edbm_delete(edbf->edbf, dkey);
   
   key2 = malloc(strlen(key) + 2);
   if (!key2) return;
   key2[0] = 0;
   strcpy(&(key2[1]), key);
   dkey.dptr = key2;
   dkey.dsize = len + 1;
   edbm_delete(edbf->edbf, dkey);
   free(key2);
   
   last_edb_call = _e_get_time();
   flush_pending = 1;
}

void
e_db_int_set(E_DB_File * edb, const char *key, int val)
{
   int                 v;

   v = htonl(val);
   e_db_data_set(edb, key, &v, sizeof(int));
   e_db_type_set(edb, key, "int");
}

int
e_db_int_get(E_DB_File * edb, const char *key, int *val)
{
   int                *dat;
   int                 size;
   int                 v;

   dat = e_db_data_get(edb, key, &size);
   if (!dat)
      return 0;
   v = ntohl(*dat);
   *val = v;
   FREE(dat);
   return 1;
}

void
e_db_float_set(E_DB_File * edb, const char *key, float val)
{
   char                buf[256];

   sprintf(buf, "%f", val);

   if (1.5 == atof("1,5")) {
       char *p;
       p = buf;
       while ((++p)[0])
          if (p[0] == ',')
             p[0] = '.';
   }

   e_db_str_set(edb, key, buf);
   e_db_type_set(edb, key, "float");
   return;
   val = 0.0;
}

int
e_db_float_get(E_DB_File * edb, const char *key, float *val)
{
   char               *dat;

   dat = e_db_str_get(edb, key);
   if (!dat)
      return 0;

   if (1.5 == atof("1,5")) {
       char *p;
       p = dat;
       while ((++p)[0])
          if (p[0] == '.')
             p[0] = ',';
   }

   *val = atof(dat);
   FREE(dat);
   return 1;
}

void
e_db_str_set(E_DB_File * edb, const char *key, const char *str)
{
   e_db_data_set(edb, key, str, strlen(str));
   e_db_type_set(edb, key, "str");
}

char               *
e_db_str_get(E_DB_File * edb, const char *key)
{
   char               *dat, *s;
   int                 size;

   dat = e_db_data_get(edb, key, &size);
   if (!dat)
      return NULL;
   s = NEW(char, size + 1);
   MEMCPY(dat, s, char, size);

   s[size] = 0;
   FREE(dat);
   return s;
}

void
e_db_type_set(E_DB_File * edb, const char *key, const char *type)
{
   char               *key2;
   _E_DB_File         *edbf;
   datum               dkey, dat;
   
   key2 = malloc(strlen(key) + 2);
   if (!key2) return;
   key2[0] = 0;
   strcpy(&(key2[1]), key);
   edbf = (_E_DB_File *) edb;
   dkey.dptr = key2;
   dkey.dsize = strlen(key) + 1;
   dat.dptr = (char*)type;
   dat.dsize = strlen(type);
   edbm_store(edbf->edbf, dkey, dat, DBM_REPLACE);
   free(key2);
   last_edb_call = _e_get_time();
   flush_pending = 1;
}

char *
e_db_type_get(E_DB_File * edb, const char *key)
{
   char               *key2;
   _E_DB_File         *edbf;
   datum               dkey, ret;

   key2 = malloc(strlen(key) + 2);
   if (!key2) return NULL;
   key2[0] = 0;
   strcpy(&(key2[1]), key);
   edbf = (_E_DB_File *) edb;
   dkey.dptr = key2;
   dkey.dsize = strlen(key) + 1;
   ret.dptr = NULL;
   ret = edbm_fetch(edbf->edbf, dkey);
   free(key2);
   last_edb_call = _e_get_time();
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
e_db_dump_multi_field(const char *file, const char *file2, int *num_ret)
{
   E_DB_File          *edb1 = NULL, *edb2 = NULL;
   _E_DB_File         *edbf1, *edbf2;
   datum               ret, key;
   char              **list = NULL;
   int                 buf_count;

   *num_ret = 0;
   buf_count = 0;
   
   edb1 = e_db_open_read(file);
   edbf1 = edb1;
   /* load prioirty database in first */
   if (edbf1)
     {
	key = edbm_firstkey(edbf1->edbf);
	while (key.dptr)
	  {
	     if (*((char *)(key.dptr)) != 0)
	       {
		  (*num_ret)++;
		  if (*num_ret > buf_count)
		    {
		       buf_count += 256;
		       REALLOC_PTR(list, buf_count);
		    }
		  list[*num_ret - 1] = NEW(char, key.dsize + 1);
		  MEMCPY(key.dptr, list[*num_ret - 1], char, key.dsize);
		  
		  list[*num_ret - 1][key.dsize] = 0;
		  ret = edbm_fetch(edbf1->edbf, key);
		  (*num_ret)++;
		  REALLOC_PTR(list, *num_ret);
		  list[*num_ret - 1] = NEW(char, ret.dsize + 1);
		  MEMCPY(ret.dptr, list[*num_ret - 1], char, ret.dsize);
		  
		  list[*num_ret - 1][ret.dsize] = 0;
	       }
	     key = edbm_nextkey(edbf1->edbf);
	  }
	e_db_close(edb1);
     }
   /* go through database 2 and if the keys dont exist alreayd in list - add */
   if (file2)
     {
	edb2 = e_db_open_read(file2);
	edbf2 = edb2;
     }
   else
      edbf2 = NULL;
   if (edbf2)
     {
	int                 i;

	key = edbm_firstkey(edbf2->edbf);
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
		       if (*num_ret > buf_count)
			 {
			    buf_count += 256;
			    REALLOC_PTR(list, buf_count);
			 }
		       list[*num_ret - 1] = NEW(char, key.dsize + 1);
		       MEMCPY(key.dptr, list[*num_ret - 1], char, key.dsize);
		       
		       list[*num_ret - 1][key.dsize] = 0;
		       ret = edbm_fetch(edbf1->edbf, key);
		       (*num_ret)++;
		       REALLOC_PTR(list, *num_ret);
		       list[*num_ret - 1] = NEW(char, ret.dsize + 1);
		       MEMCPY(ret.dptr, list[*num_ret - 1], char, ret.dsize);
		       
		       list[*num_ret - 1][ret.dsize] = 0;
		    }
		  FREE(s);
	       }
	     key = edbm_nextkey(edbf1->edbf);
	  }
	e_db_close(edb2);
     }
   last_edb_call = _e_get_time();
   flush_pending = 1;
   return list;
}

char              **
e_db_dump_key_list(const char *file, int *num_ret)
{
   E_DB_File          *edb;
   _E_DB_File         *edbf;
   datum               key;
   char              **list = NULL;
   int                 buf_count;
   
   *num_ret = 0;
   buf_count = 0;
   
   edb = e_db_open_read(file);
   edbf = edb;
   if (edbf)
     {
	key = edbm_firstkey(edbf->edbf);
	while (key.dptr)
	  {
	     if (*((char *)(key.dptr)) != 0)
	       {
		  (*num_ret)++;
		  if (*num_ret > buf_count)
		    {
		       buf_count += 256;
		       REALLOC_PTR(list, buf_count);
		    }
		  list[*num_ret - 1] = NEW(char, key.dsize + 1);
		  MEMCPY(key.dptr, list[*num_ret - 1], char, key.dsize);
		  
		  list[*num_ret - 1][key.dsize] = 0;
	       }
	     key = edbm_nextkey(edbf->edbf);
	  }
	e_db_close(edb);
     }
   last_edb_call = _e_get_time();
   flush_pending = 1;
   return list;
}

char              **
e_db_match_keys(E_DB_File *edb, const char *pattern, int *num_ret)
{
   _E_DB_File         *edbf;
   datum               key;
   char              **list = NULL;
   int                 buf_count;
   char                s[8192];
   
   *num_ret = 0;
   buf_count = 0;
   
   edbf = edb;
   if (edbf)
     {
	key = edbm_firstkey(edbf->edbf);
	while (key.dptr)
	  {	    
	     MEMCPY(key.dptr, s, char, key.dsize);
	     s[key.dsize] = 0;

	     if ((*((char *)(key.dptr)) != 0) &&
		 (fnmatch(pattern, s, 0) == 0))
	       {
		  (*num_ret)++;
		  if (*num_ret > buf_count)
		    {
		       buf_count += 256;
		       REALLOC_PTR(list, buf_count);
		    }
		  list[*num_ret - 1] = NEW(char, key.dsize + 1);
		  MEMCPY(key.dptr, list[*num_ret - 1], char, key.dsize);
		  
		  list[*num_ret - 1][key.dsize] = 0;
	       }
	     key = edbm_nextkey(edbf->edbf);
	  }
     }
   last_edb_call = _e_get_time();
   flush_pending = 1;
   return list;
}
