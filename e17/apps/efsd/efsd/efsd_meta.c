/*

Copyright (C) 2000, 2001 Christian Kreibich <cK@whoop.org>.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <Edb.h>

#include <efsd.h>
#include <efsd_debug.h>
#include <efsd_macros.h>
#include <efsd_misc.h>
#include <efsd_statcache.h>

static unsigned int   hash_filename(char *filename);
static char          *meta_db_get_file(char *filename);
static int            meta_db_set_data(EfsdSetMetadataCmd *esmc, char *dbfile);
static void          *meta_db_get_data(EfsdGetMetadataCmd *egmc,
				       char *dbfile, int *data_len);
static char          *get_full_key(char *filename, char* key);


static unsigned int   
hash_filename(char *s)
{
  unsigned int hash;

  D_ENTER;

  for (hash = 0; *s != '\0'; s++)
    hash = (32*hash + *s);

  D_RETURN_(hash);
}


static char*
meta_db_get_file(char *filename)
{
  /* FIXME: mutex this when adding threads! */
  static char    dbfile[MAXPATHLEN];
  char           s[MAXPATHLEN];
  char          *path, *file;
  int            use_home_dir = FALSE;
  unsigned int   h;
  struct stat   *st;

  D_ENTER;

  if (!filename || filename[0] == '\0' || filename[0] != '/')
    D_RETURN_(NULL);
  
  path = filename;

  file = strrchr(filename, '/');
  if (!file)
    {
      /* Something's wrong -- this is supposed to be
	 a chanonical path ...
      */
      D(("Couldn't find '/' in filename '%s'\n", filename));
      D_RETURN_(NULL);
    }

  *file = '\0';
  file++;

  if (*path == '\0')
    path = "/";

  snprintf(s, MAXPATHLEN, "%s/%s", path, EFSD_META_DIR_NAME);

  if (efsd_misc_file_exists(s))
    {
      if (efsd_misc_file_writeable(s) &&
	  efsd_misc_file_execable(s))
	use_home_dir = FALSE;
      else
	use_home_dir = TRUE;
    }
  else
    {
      if (efsd_misc_file_writeable(path) &&
	  efsd_misc_file_execable(path))
	{
	  if ((st = efsd_stat(path)) == NULL)
	    {
	      use_home_dir = TRUE;
	    }
	  else
	    {
	      umask(000);

	      if (mkdir(s, st->st_mode) < 0)
		use_home_dir = TRUE;
	      else
		use_home_dir = FALSE;

	      umask(077);
	    }
	}
      else
	{
	  use_home_dir = TRUE;
	}
    }

  *(file-1) = '/';
  h = hash_filename(filename);

  if (use_home_dir)
    {
      snprintf(dbfile, MAXPATHLEN, "%s/%u.db",
	       efsd_misc_get_user_dir(), h);
    }
  else
    {
      snprintf(dbfile, MAXPATHLEN, "%s/%u.db",
	       s, h);
    }  

  D_RETURN_(dbfile);
}


static int   
meta_db_set_data(EfsdSetMetadataCmd *esmc, char *dbfile)
{
  char      *key;
  E_DB_File *db;

  D_ENTER;

  if ( (db = e_db_open(dbfile)) == NULL)
    D_RETURN_(0);

  key = get_full_key(esmc->file, esmc->key);

  switch (esmc->datatype)
    {
    case EFSD_INT:
      D(("Setting metadata key '%s' to int value %i\n",
	 key, *((int*)esmc->data)));
      e_db_int_set(db, key, *((int*)esmc->data));
      break;
    case EFSD_FLOAT:
      e_db_float_set(db, key, *((float*)esmc->data));
      break;
    case EFSD_STRING:
      e_db_str_set(db, key, esmc->data);
      break;
    case EFSD_RAW:
      e_db_data_set(db, key, esmc->data, esmc->data_len);
      break;
    default:
      D(("Unknown data type!\n"));
      e_db_close(db);
      e_db_flush();
      D_RETURN_(0);
    }

  e_db_close(db);
  e_db_flush();

  D_RETURN_(1);
}


static void *
meta_db_get_data(EfsdGetMetadataCmd *egmc,
		 char *dbfile, int *data_len)
{
  void        *result;
  char        *key;
  int          success = FALSE;
  E_DB_File   *db;

  D_ENTER;

  if ( (db = e_db_open_read(dbfile)) == NULL)
    D_RETURN_(0);

  key = get_full_key(egmc->file, egmc->key);

  switch (egmc->datatype)
    {
    case EFSD_INT:
      {
	result = NEW(int);
	*data_len = sizeof(int);

	success = e_db_int_get(db, key, (int*)result);
      }
      break;
    case EFSD_FLOAT:
      {
	result = NEW(float);
	*data_len = sizeof(float);

	success = e_db_float_get(db, key, (float*)result);
      }
      break;
    case EFSD_STRING:
      {
	result = e_db_str_get(db, key);
	*data_len = strlen(result) + 1;

	if (result)
	  success = TRUE;
	else
	  success = FALSE;
      }
      break;
    case EFSD_RAW:
      {
	result = e_db_data_get(db, key, data_len);
      }
      break;
    default:
      D(("Unknown data type!\n"));
      e_db_close(db);
      D_RETURN_(0);
    }

  e_db_close(db);

  if (!success)
    D_RETURN_(NULL);

  D_RETURN_(result);
}


static char *
get_full_key(char *filename, char *key)
{
  static char s[MAXPATHLEN];

  D_ENTER;
  
  /* FIXME: mutex this when adding threads! */

  snprintf(s, MAXPATHLEN, "%s:%i:%s", filename, geteuid(), key);

  D_RETURN_(s);
}


int 
efsd_meta_set(EfsdCommand *ec)
{
  char               *dbfile = NULL;
  EfsdSetMetadataCmd *esmc;

  D_ENTER;

  if ((!ec) || (ec->type != EFSD_CMD_SETMETA))
    D_RETURN_(0);

  esmc = &(ec->efsd_set_metadata_cmd);
  dbfile = meta_db_get_file(esmc->file);

  D_RETURN_(meta_db_set_data(esmc, dbfile));
}


void *
efsd_meta_get(EfsdCommand *ec, int *data_len)
{
  char               *dbfile = NULL;
  EfsdGetMetadataCmd *egmc;

  D_ENTER;

  if ((!ec) || (ec->type != EFSD_CMD_GETMETA) || (!data_len))
    D_RETURN_(NULL);

  egmc = &(ec->efsd_get_metadata_cmd);
  dbfile = meta_db_get_file(egmc->file);

  D_RETURN_(meta_db_get_data(egmc, dbfile, data_len));
}

