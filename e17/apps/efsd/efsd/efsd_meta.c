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
#include <errno.h>
#include <Edb.h>

#include <efsd.h>
#include <efsd_debug.h>
#include <efsd_macros.h>
#include <efsd_misc.h>
#include <efsd_statcache.h>
#include <efsd_lock.h>

EfsdLock              *meta_lock;

static unsigned int    meta_hash_filename(char *filename);
static int             meta_db_get_file(char *filename, char *dbfile, int len, int create);
static int             meta_db_set_data(EfsdSetMetadataCmd *esmc, char *dbfile);
static void           *meta_db_get_data(EfsdGetMetadataCmd *egmc,
					char *dbfile, int *data_len);


static unsigned int   
meta_hash_filename(char *filename)
{
  char str[MAXPATHLEN];
  char *s;
  unsigned int hash;

  D_ENTER;

  snprintf(str, MAXPATHLEN, "%i:%s", geteuid(), filename);
  s = str + strlen(str) - 1;

  for (hash = 0; s != str; s--)
    hash = (7*hash + *s);

  D("Hashed %s to %u\n", filename, hash);

  D_RETURN_(hash);
}


static int
meta_db_get_file(char *filename, char *dbfile, int len, int create)
{
  char           s[MAXPATHLEN];
  char          *path, *file, *newfilename, *dir;
  int            use_home_dir = FALSE;
  unsigned int   h;
  struct stat    st;

  D_ENTER;

  if (!filename || filename[0] == '\0' || filename[0] != '/')
    {
      errno = ENOENT;
      D_RETURN_(FALSE);
    }

  newfilename = strdup(filename);
  /* eww - changed code that modified the filename buffer passed into this */
  /* func itself */
  path = newfilename;

  file = strrchr(newfilename, '/');
  if (!file)
    {
      /* Something's wrong -- this is supposed to be
	 a chanonical path ...
      */
      D("Couldn't find '/' in filename '%s'\n", newfilename);
      errno = EINVAL;
      if (newfilename) free(newfilename);
      D_RETURN_(FALSE);
    }

  *file = '\0';
  file++;

  if (*path == '\0')
    {
      path = "/";
      snprintf(s, MAXPATHLEN, "/%s", EFSD_META_DIR_NAME);
    }
  else
    {
      snprintf(s, MAXPATHLEN, "%s/%s", path, EFSD_META_DIR_NAME);
    }
   
  dir = strdup(path);


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
      if (!create)
	{
	  if (newfilename) free(newfilename);
	  if (dir) free(dir);
	  D_RETURN_(FALSE);
	}

      if (efsd_misc_file_writeable(path) &&
	  efsd_misc_file_execable(path))
	{
	  if (!efsd_stat(path, &st))
	    {
	      use_home_dir = TRUE;
	    }
	  else
	    {
	      umask(000);

	      if (mkdir(s, st.st_mode) < 0)
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
  
  /* Okay -- we use different metadata file names
     depending on whether we're in the global directory
     in the user's home or locally out in the filesystem.
     When we're global, we use the full path and filename.
     When we're local, we just use the file name, without
     the path.

     By doing this we can rename an entire directory
     branch without having to adjust all the metadata
     file names in the subtree.
  */

  if (use_home_dir)
    {
      h = meta_hash_filename(dir);
      snprintf(dbfile, len, "%s/efsd_meta_%u.db",
	       efsd_misc_get_user_dir(), h);
    }
  else
    {
      snprintf(dbfile, len, "%s/efsd_meta.db",
	       s);
    }  

  if (dir) free(dir);
  if (newfilename) free(newfilename);
  D_RETURN_(TRUE);
}


static int   
meta_db_set_data(EfsdSetMetadataCmd *esmc, char *dbfile)
{
  E_DB_File *db;
  char       s[MAXPATHLEN];
  char      *last_slash;
   
  D_ENTER;

  efsd_lock_get_write_access(meta_lock);

  if ( (db = e_db_open(dbfile)) == NULL)
    {
      efsd_lock_release_write_access(meta_lock);
      D_RETURN_(0);
    }

  last_slash = strrchr(esmc->file, '/');
  if (last_slash)
    snprintf(s, sizeof(s), ".meta/%s/%s", last_slash + 1, esmc->key);
  else
    snprintf(s, sizeof(s), ".meta/%s/%s", esmc->file, esmc->key);
  switch (esmc->datatype)
    {
    case EFSD_INT:
      D("Setting metadata key '%s' to int value %i\n",
	 esmc->key, *((int*)esmc->data));
      e_db_int_set(db, s, *((int*)esmc->data));
      break;
    case EFSD_FLOAT:
      e_db_float_set(db, s, *((float*)esmc->data));
      break;
    case EFSD_STRING:
      e_db_str_set(db, s, esmc->data);
      break;
    case EFSD_RAW:
      e_db_data_set(db, s, esmc->data, esmc->data_len);
      break;
    default:
      D("Unknown data type!\n");
      e_db_close(db);
/* oooooh - no no. we can be much better here - don't flush. we don't really */
/* need to as well - wel will likely be the only opener of this meta db */
/* since we are the file system daemon... notice the MASSIVE speedups */
/* in accessing metadata for read/write... */
/*      e_db_flush(); */
      efsd_lock_release_write_access(meta_lock);
      D_RETURN_(0);
    }

  e_db_close(db);
/* oooooh - no no. we can be much better here - don't flush. we don't really */
/* need to as well - wel will likely be the only opener of this meta db */
/* since we are the file system daemon... notice the MASSIVE speedups */
/* in accessing metadata for read/write... */
/* e_db_flush(); */
  efsd_lock_release_write_access(meta_lock);

  D_RETURN_(1);
}


static void *
meta_db_get_data(EfsdGetMetadataCmd *egmc,
		 char *dbfile, int *data_len)
{
  void        *result = NULL;
  int          success = FALSE;
  E_DB_File   *db;
  char         s[MAXPATHLEN];
  char        *last_slash;

  D_ENTER;
  D("Getting metadata %s from %s\n",
     egmc->key, dbfile);

  efsd_lock_get_read_access(meta_lock);

   /* changed this to open read/write - not harmful as we control */
   /* the meta data entirely */
  if ( (db = e_db_open(dbfile)) == NULL)
    {
      errno = ENODATA;
      efsd_lock_release_read_access(meta_lock);
      D_RETURN_(NULL);
    }

  last_slash = strrchr(egmc->file, '/');
  if (last_slash)
    snprintf(s, sizeof(s), ".meta/%s/%s", last_slash + 1, egmc->key);
  else
    snprintf(s, sizeof(s), ".meta/%s/%s", egmc->file, egmc->key);
   
  switch (egmc->datatype)
    {
    case EFSD_INT:
      {
	result = NEW(int);
	*data_len = sizeof(int);

	success = e_db_int_get(db, s, (int*)result);
      }
      break;
    case EFSD_FLOAT:
      {
	result = NEW(float);
	*data_len = sizeof(float);

	success = e_db_float_get(db, s, (float*)result);
      }
      break;
    case EFSD_STRING:
      {
	result = e_db_str_get(db, s);

	if (result)
	  {
	    *data_len = strlen(result) + 1;
	    success = TRUE;
	  }
	else
	  success = FALSE;
      }
      break;
    case EFSD_RAW:
      {
	result = e_db_data_get(db, s, data_len);
	
	if (result)
	  success = TRUE;
	else
	  success = FALSE;
      }
      break;
    default:
      D("Unknown data type!\n");
      success = FALSE;
    }

  if (!success)
    errno = ENODATA;

  e_db_close(db);
/* oooooh - no no. we can be much better here - don't flush. we don't really */
/* need to as well - wel will likely be the only opener of this meta db */
/* since we are the file system daemon... notice the MASSIVE speedups */
/* in accessing metadata for read/write... */
/* e_db_flush(); */
  efsd_lock_release_read_access(meta_lock);

  if (!success)
    D_RETURN_(NULL);

  D_RETURN_(result);
}


void        
efsd_meta_init(void)
{
  D_ENTER;

  meta_lock = efsd_lock_new();

  D_RETURN;
}


void        
efsd_meta_cleanup(void)
{
  D_ENTER;

  efsd_lock_free(meta_lock);
  meta_lock = NULL;

  D_RETURN;
}


int 
efsd_meta_set(EfsdCommand *ec)
{
  char                dbfile[MAXPATHLEN];
  EfsdSetMetadataCmd *esmc;

  D_ENTER;

  if ((!ec) || (ec->type != EFSD_CMD_SETMETA))
    D_RETURN_(0);

  esmc = &(ec->efsd_set_metadata_cmd);
  efsd_misc_remove_trailing_slashes(esmc->file);
  meta_db_get_file(esmc->file, dbfile, MAXPATHLEN, TRUE /* create */);

  D("Writing to %s for %s\n", dbfile, esmc->file);

  D_RETURN_(meta_db_set_data(esmc, dbfile));
}


void *
efsd_meta_get(EfsdCommand *ec, int *data_len)
{
  char                dbfile[MAXPATHLEN];
  EfsdGetMetadataCmd *egmc;

  D_ENTER;

  if ((!ec) || (ec->type != EFSD_CMD_GETMETA) || (!data_len))
    D_RETURN_(NULL);

  egmc = &(ec->efsd_get_metadata_cmd);
  efsd_misc_remove_trailing_slashes(egmc->file);

  if (meta_db_get_file(egmc->file, dbfile, MAXPATHLEN, FALSE) == FALSE)
    D_RETURN_(NULL);

  D_RETURN_(meta_db_get_data(egmc, dbfile, data_len));
}


int         
efsd_meta_get_meta_file(char *filename, char *metafile, int len, int create)
{
  int result;

  D_ENTER;

  result = meta_db_get_file(filename, metafile, len, create);

  D_RETURN_(result);
}


void
efsd_meta_idle(void)
{
   efsd_lock_get_write_access(meta_lock);
   e_db_flush();
   efsd_lock_release_write_access(meta_lock);
}
