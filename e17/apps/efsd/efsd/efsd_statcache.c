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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <efsd_debug.h>
#include <efsd_fam.h>
#include <efsd_hash.h>
#include <efsd_macros.h>
#include <efsd_misc.h>
#include <efsd_lock.h>

static EfsdHash  *stat_cache;
static EfsdHash  *lstat_cache;
EfsdLock         *stat_lock;

static void         stat_hash_item_free(EfsdHashItem *it);
static int          stat_internal(char *filename, struct stat *st, char use_lstat);

static void         
stat_hash_item_free(EfsdHashItem *it)
{
  D_ENTER;

  if (!it)
    D_RETURN;

  /* An item is removed or falling out of the hash --
     stop the monitor. If it has already been stopped,
     the hashtable lookup will fail anyway.
  */
  efsd_fam_stop_monitor_internal(it->key);

  /* Key is a string -- simple to free: */
  FREE(it->key);

  /* Data is a struct stat -- also easy: */
  FREE(it->data);
  FREE(it);

  D_RETURN;
}


static int
stat_internal(char *filename, struct stat *st, char use_lstat)
{
  struct stat *stp;

  D_ENTER;

  efsd_lock_get_read_access(stat_lock);

  /* Check if info is still in cache: */
  if (use_lstat)
    {
      D(("Looking up %s in lstat cache\n", filename));
      stp = (struct stat*)efsd_hash_find(lstat_cache, filename);
    }
  else
    {
      D(("Looking up %s in stat cache\n", filename));
      stp = (struct stat*)efsd_hash_find(stat_cache, filename);
    }

  if (stp)
    {
      D(("Cached stat for %s\n", filename));
      *st = *stp;
      efsd_lock_release_read_access(stat_lock);
      D_RETURN_(TRUE);
    }

  efsd_lock_release_read_access(stat_lock);

  /* No -- we have to stat: */

  stp = NEW(struct stat);

  if (use_lstat)
    {
      if (lstat(filename, stp) < 0)
	{
	  D(("lstat() on %s failed.\n", filename));
	  FREE(stp);
	  D_RETURN_(FALSE);
	}
    }
  else
    {
      if (stat(filename, stp) < 0)
	{
	  if (lstat(filename, stp) < 0)
	    {
	      D(("stat and lstat() on %s failed.\n", filename));
	      FREE(stp);
	      D_RETURN_(FALSE);
	    }
	}
    }

  /* Insert in cache and monitor file to be
     informed about updates:
  */

  efsd_lock_get_write_access(stat_lock);

  if (use_lstat)
    efsd_hash_insert(lstat_cache, (void*)strdup(filename), (void*)stp);
  else
    efsd_hash_insert(stat_cache, (void*)strdup(filename), (void*)stp);

  *st = *stp;

  efsd_lock_release_write_access(stat_lock);
  efsd_fam_start_monitor_internal(filename);

  D_RETURN_(TRUE);
}



void         
efsd_stat_init(void)
{
  D_ENTER;

  stat_cache = efsd_hash_new(1023, 10, (EfsdHashFunc)efsd_hash_string,
			     (EfsdCmpFunc)strcmp, stat_hash_item_free);

  lstat_cache = efsd_hash_new(1023, 10, (EfsdHashFunc)efsd_hash_string,
			      (EfsdCmpFunc)strcmp, stat_hash_item_free);

  stat_lock = efsd_lock_new();
  
  D_RETURN;
}


void         
efsd_stat_cleanup(void)
{
  D_ENTER;

  efsd_hash_free(stat_cache);
  efsd_hash_free(lstat_cache);
  efsd_lock_free(stat_lock);

  stat_cache = NULL;
  lstat_cache = NULL;
  stat_lock = NULL;

  D_RETURN;
}


int
efsd_stat(char *filename, struct stat *st)
{
  int result;

  D_ENTER;

  result = stat_internal(filename, st, FALSE);
  
  D_RETURN_(result);
}


int
efsd_lstat(char *filename, struct stat *st)
{
  int result;

  D_ENTER;

  result = stat_internal(filename, st, TRUE);

  D_RETURN_(result);
}


void         
efsd_stat_update(char *filename)
{
  struct stat *st;

  D_ENTER;

  /* Make sure no cache-reads are currently in progress: */
  efsd_lock_get_write_access(stat_lock);

  if ((st = (struct stat*)efsd_hash_find(stat_cache, filename)))
    {
      D(("Old timestamp: %i\n", (int)st->st_mtime));
      if (stat(filename, st) < 0)
	{
	  D(("stat() on %s failed.\n", filename));
	}
      D(("New timestamp: %i\n", (int)st->st_mtime));
    }

  if ((st = (struct stat*)efsd_hash_find(lstat_cache, filename)))
    {
      D(("Old timestamp: %i\n", (int)st->st_mtime));
      if (lstat(filename, st) < 0)
	{
	  D(("stat() on %s failed.\n", filename));
	}
      D(("New timestamp: %i\n", (int)st->st_mtime));
    }

  efsd_lock_release_write_access(stat_lock);

  D_RETURN;
}


void         
efsd_stat_remove(char *filename)
{
  D_ENTER;
  
  efsd_lock_get_write_access(stat_lock);

  efsd_hash_remove(stat_cache, filename);
  efsd_hash_remove(lstat_cache, filename);

  efsd_lock_release_write_access(stat_lock);

  D_RETURN;
}


void         
efsd_stat_change_filename(char *file1, char *file2)
{
  D_ENTER;

  efsd_lock_get_write_access(stat_lock);

  efsd_hash_change_key(stat_cache, file1, file2);
  efsd_hash_change_key(lstat_cache, file1, file2);

  efsd_lock_release_write_access(stat_lock);

  D_RETURN;
}
