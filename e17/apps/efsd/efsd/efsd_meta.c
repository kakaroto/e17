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
#include <dirent.h>
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
#include <efsd_meta.h>
#include <efsd_fs.h>



EfsdLock              *meta_lock;

static char *          meta_get_user_metadata_filename(void);
static int             meta_db_get_file(char *filename,
					char *dbfile, int db_len,
					char *key, int key_len, int create);

static int             meta_db_set_data(EfsdSetMetadataCmd *esmc, char *key_base, char *dbfile);

static void           *meta_db_get_data(EfsdGetMetadataCmd *egmc, char *key_base,
					char *dbfile, int *data_len);

/*
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
*/

static char *
meta_get_user_metadata_filename(void)
{
  static char   s[MAXPATHLEN] = "\0";
  
  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);

  snprintf(s, MAXPATHLEN, "efsd_meta_%u.db", geteuid());

  D_RETURN_(s);
}

static int
meta_db_get_file(char *filename,
		 char *dbfile, int db_len,
		 char *key, int key_len, int create)
{
  char           s[MAXPATHLEN], filename_copy[MAXPATHLEN];
  char          *path, *file;
  int            use_home_dir = FALSE;
  struct stat    st;

  D_ENTER;

  if (!filename || filename[0] != '/')
    {
      D("Invalid filename\n");
      errno = ENOENT;
      D_RETURN_(FALSE);
    }

  snprintf(filename_copy, MAXPATHLEN, "%s", filename);
  path = filename_copy;

  file = strrchr(filename_copy, '/');
  if (!file)
    {
      /* Something's wrong -- this is supposed to be
	 a chanonical path ...
      */
      D("Couldn't find '/' in filename '%s'\n", filename);
      errno = EINVAL;
      D_RETURN_(FALSE);
    }

  /* terminate the string where the path ends,
     cutting of the filename...
  */
  *file = '\0';
  file++;

  /* file is now the filename only, path is now the path only. */

  if (*path == '\0')
    {
      path = "/";
      snprintf(s, MAXPATHLEN, "/%s", EFSD_META_DIR_NAME);
    }
  else
    {
      snprintf(s, MAXPATHLEN, "%s/%s", path, EFSD_META_DIR_NAME);
    }

  /* s is now the metadata directory for the given file, out
     in the filesystem. Check if the directory exists
     and is accessible.
  */
   
  if (efsd_misc_file_exists(s))
    {
      if (efsd_misc_file_writeable(s) &&
	  efsd_misc_file_execable(s))
	/* Can access and manipulate --> use it. */	
	use_home_dir = FALSE;
      else
	/* No access --> need to put metadata in home dir */
	use_home_dir = TRUE;  
    }
  else
    {
      if (efsd_misc_file_writeable(path) &&
	  efsd_misc_file_execable(path))
	{
	  if (!efsd_stat(path, &st))
	    {
	      use_home_dir = TRUE;
	    }
	  else
	    {
	      if (!create)
		{
		  D("Not existant metadata dir %s, no create -- returning false.\n", s);
		  D_RETURN_(FALSE);
		}

	      /* We're told to create the metadata directory, so try it ... */
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

  /* We use different keys to access metadata, depending on
     whether we're in the global directory in the user's home
     or locally out in the filesystem. When we're in the home
     dir, we use the full path and filename. When we're local,
     we just use the file name, without the path.

     By doing this we can rename an entire directory
     branch without having to adjust all the metadata
     file names in the subtree.
  */

  if (use_home_dir)
    {
      snprintf(dbfile, db_len, "%s/%s",
	       efsd_misc_get_user_dir(), EFSD_META_FILE_NAME);

      snprintf(key, key_len, "/meta%s", filename);	       
    }
  else
    {
      snprintf(dbfile, db_len, "%s/%s",
	       s, meta_get_user_metadata_filename());

      snprintf(key, key_len, "/meta/%s", file);
    }  
  D("Returning success.\n");

  D_RETURN_(TRUE);
}


int
efsd_meta_copy_data(char *from_file, char *to_file)
{
  char from_db_file[MAXPATHLEN];
  char to_db_file[MAXPATHLEN];
  char from_key[MAXPATHLEN];
  char to_key[MAXPATHLEN];
  E_DB_File   *from_db;
  E_DB_File   *to_db;
  char        **keys = NULL;
  int          i = 0, from_key_len = 0, to_key_len = 0, num_keys = 0;

  if (!from_file || !to_file ||
      *from_file != '/' || *to_file != '/')
    D_RETURN_(FALSE);

  D("Copying metadata from file %s to %s\n", from_file, to_file);

  if (!meta_db_get_file(from_file,
			from_db_file, MAXPATHLEN,
			from_key, MAXPATHLEN,
			FALSE))
    D_RETURN_(FALSE);
  
  if (!meta_db_get_file(to_file,
			to_db_file, MAXPATHLEN,
			to_key, MAXPATHLEN,
			TRUE))
    D_RETURN_(FALSE);

  D("From %s %s to %s %s\n", from_db_file, from_key, to_db_file, to_key);

  efsd_lock_get_write_access(meta_lock);

  if ((from_db = e_db_open(from_db_file)) == NULL)
    {
      efsd_lock_release_write_access(meta_lock);
      D_RETURN_(FALSE);
    }

  if ((to_db = e_db_open(to_db_file)) == NULL)
    {
      e_db_close(from_db);
      efsd_lock_release_write_access(meta_lock);
      D_RETURN_(FALSE);
    }

  from_key_len = strlen(from_key);
  to_key_len = strlen(to_key);

  /* append a "*" to the end to make the key match all metadata entries */
  snprintf(from_key + from_key_len, MAXPATHLEN - from_key_len, "/*"); 
	   
  keys = e_db_match_keys(from_db, from_key, &num_keys);

  for (i = 0; i < num_keys; i++)
    {
      char *type = NULL;
      void *data = NULL;
      int   data_len = 0;

      snprintf(to_key + to_key_len, MAXPATHLEN - to_key_len, "%s", keys[i] + from_key_len);

      data = e_db_data_get(from_db, keys[i], &data_len);
      e_db_data_set(to_db, to_key, data, data_len);

      if ((type = e_db_type_get(from_db, keys[i])))
	{
	  e_db_type_set(to_db, to_key, type);
	  FREE(type);
	}
      
      D("Copied item from %s to %s\n", keys[i], to_key);
      FREE(data);
    }
  
  FREE(keys);
  efsd_lock_release_write_access(meta_lock);

  e_db_close(from_db);
  e_db_close(to_db);

  D_RETURN_(TRUE);  
}


int        
efsd_meta_move_data(char *from_file, char *to_file)
{
  int result;

  if (efsd_meta_copy_data(from_file, to_file) == FALSE)
    D_RETURN_(FALSE);

  result = efsd_meta_remove_data(from_file);

  D_RETURN_(result);
}



int
efsd_meta_remove_data(char *file)
{
  char db_file[MAXPATHLEN];
  char key[MAXPATHLEN];
  E_DB_File   *db;
  char        **keys = NULL;
  int          i = 0, key_len = 0, num_keys = 0;

  if (!file || *file != '/')
    D_RETURN_(FALSE);

  D("Removing metadata of file %s\n", file);

  if (!meta_db_get_file(file,
			db_file, MAXPATHLEN,
			key, MAXPATHLEN,
			FALSE))
    D_RETURN_(FALSE);
  
  efsd_lock_get_write_access(meta_lock);

  if ((db = e_db_open(db_file)) == NULL)
    {
      efsd_lock_release_write_access(meta_lock);
      D_RETURN_(FALSE);
    }

  key_len = strlen(key);

  /* append a "*" to the end to make the key match all metadata entries */
  snprintf(key + key_len, MAXPATHLEN - key_len, "/*"); 
	   
  keys = e_db_match_keys(db, key, &num_keys);

  for (i = 0; i < num_keys; i++)
    e_db_data_del(db, keys[i]);
  
  FREE(keys);
  efsd_lock_release_write_access(meta_lock);

  e_db_close(db);

  D_RETURN_(TRUE);  
}



static int   
meta_db_set_data(EfsdSetMetadataCmd *esmc, char *key_base, char *dbfile)
{
  E_DB_File *db;
  char       key[MAXPATHLEN];
   
  D_ENTER;

  efsd_lock_get_write_access(meta_lock);

  if ( (db = e_db_open(dbfile)) == NULL)
    {
      efsd_lock_release_write_access(meta_lock);
      D_RETURN_(0);
    }

  snprintf(key, MAXPATHLEN, "%s%s", key_base, esmc->key);
  
  switch (esmc->datatype)
    {
    case EFSD_INT:
      D("Setting metadata key '%s' to int value %i\n",
	 key, *((int*)esmc->data));
      e_db_int_set(db, key, *((int*)esmc->data));
      break;
    case EFSD_FLOAT:
      D("Setting metadata key '%s' to float value %f\n",
	 key, *((float*)esmc->data));
      e_db_float_set(db, key, *((float*)esmc->data));
      break;
    case EFSD_STRING:
      e_db_str_set(db, key, esmc->data);
      break;
    case EFSD_RAW:
      e_db_data_set(db, key, esmc->data, esmc->data_len);
      break;
    default:
      D("Unknown data type!\n");
      e_db_close(db);
/*      e_db_flush(); */
      efsd_lock_release_write_access(meta_lock);
      D_RETURN_(0);
    }

  e_db_close(db);
/* e_db_flush(); */
  efsd_lock_release_write_access(meta_lock);

  D_RETURN_(1);
}


static void *
meta_db_get_data(EfsdGetMetadataCmd *egmc, char *key_base,
		 char *dbfile, int *data_len)
{
  void        *result = NULL;
  int          success = FALSE;
  E_DB_File   *db;
  char         key[MAXPATHLEN];

  D_ENTER;

  efsd_lock_get_read_access(meta_lock);

   /* changed this to open read/write - not harmful as we control */
   /* the meta data entirely */
  if ( (db = e_db_open(dbfile)) == NULL)
    {
      errno = ENODATA;
      efsd_lock_release_read_access(meta_lock);
      D_RETURN_(NULL);
    }

  snprintf(key, MAXPATHLEN, "%s%s", key_base, egmc->key);

  D("Getting metadata %s from %s\n",
     key, dbfile);
   
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
	result = e_db_data_get(db, key, data_len);
	
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
  char                db_file[MAXPATHLEN];
  char                key_base[MAXPATHLEN];
  EfsdSetMetadataCmd *esmc;
  int                 success = FALSE;

  D_ENTER;

  if ((!ec) || (ec->type != EFSD_CMD_SETMETA))
    D_RETURN_(0);

  esmc = &(ec->efsd_set_metadata_cmd);
  efsd_misc_remove_trailing_slashes(esmc->file);
  meta_db_get_file(esmc->file, db_file, MAXPATHLEN,
		   key_base, MAXPATHLEN, TRUE /* create */);

  D("Writing to %s for %s\n", db_file, esmc->file);

  success = meta_db_set_data(esmc, key_base, db_file);

  D_RETURN_(success);
}


void *
efsd_meta_get(EfsdCommand *ec, int *data_len)
{
  char                db_file[MAXPATHLEN];
  char                key_base[MAXPATHLEN];
  EfsdGetMetadataCmd *egmc;
  void               *success = NULL;

  D_ENTER;

  if ((!ec) || (ec->type != EFSD_CMD_GETMETA) || (!data_len))
    D_RETURN_(NULL);

  egmc = &(ec->efsd_get_metadata_cmd);
  efsd_misc_remove_trailing_slashes(egmc->file);

  if (meta_db_get_file(egmc->file, db_file, MAXPATHLEN,
		       key_base, MAXPATHLEN, FALSE) == FALSE)
    D_RETURN_(NULL);

  success = meta_db_get_data(egmc, key_base, db_file, data_len);

  D_RETURN_(success);
}


void
efsd_meta_dir_cleanup(char *file)
{
  DIR           *dir;
  struct dirent  de, *de_ptr;
  int            dir_empty = TRUE;

  if ( (dir = opendir(file)) == NULL)
    D_RETURN;

  for (READDIR(dir, de, de_ptr); de_ptr; READDIR(dir, de, de_ptr))
    {
      if (!strcmp(de_ptr->d_name, ".")  ||
	  !strcmp(de_ptr->d_name, "..") ||
	  !strcmp(de_ptr->d_name, EFSD_META_DIR_NAME))	  
	continue;

      dir_empty = FALSE;
      break;
    }

  if (dir_empty)
    {
      char meta_dir[MAXPATHLEN];

      snprintf(meta_dir, MAXPATHLEN, "%s/%s", file, EFSD_META_DIR_NAME);
      efsd_fs_rm(meta_dir, EFSD_FS_OP_FORCE|EFSD_FS_OP_RECURSIVE);     
    }
}


void
efsd_meta_idle(void)
{
  D_ENTER;
  
  efsd_lock_get_write_access(meta_lock);
  e_db_flush();
  efsd_lock_release_write_access(meta_lock);
  
  D_RETURN;
}
