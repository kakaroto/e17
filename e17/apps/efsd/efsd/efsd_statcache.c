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
#include <efsd_monitor.h>
#include <efsd_hash.h>
#include <efsd_macros.h>
#include <efsd_misc.h>
#include <efsd_monitor.h>
#include <efsd_lock.h>

static EfsdHash  *stat_cache;
static EfsdHash  *lstat_cache;
static EfsdLock  *stat_lock;


/* Statcache entries, hashed by the filename: */
typedef struct efsd_statcache_item
{
  /* Stat info itself: */
  struct stat      st;

  /* The monitor that is responsible for the
     file whose stat info this entry contains: */
  EfsdMonitor     *monitor;
}
EfsdStatcacheItem;

static void         stat_hash_item_free(EfsdHashItem *it);
static void         stat_hash_item_free_no_monitor_update(EfsdHashItem *it);
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
  D("Item %s falling out of statcache.\n", (char*)it->key);
  efsd_monitor_stop_internal(it->key, FALSE);

  /* Key is a string -- simple to free: */
  FREE(it->key);

  /* Data is an EfsdStatcacheItem -- also easy,
     the monitor is freed elsewhere: */
  FREE(it->data);
  FREE(it);

  D_RETURN;
}


static void         
stat_hash_item_free_no_monitor_update(EfsdHashItem *it)
{
  D_ENTER;

  if (!it)
    D_RETURN;

  /* An item is removed or falling out of the hash --
     stop the monitor. If it has already been stopped,
     the hashtable lookup will fail anyway.
  */
  D("Item %s falling out of statcache.\n", (char*)it->key);

  /* Key is a string -- simple to free: */
  FREE(it->key);

  /* Data is an EfsdStatcacheItem -- also easy,
     the monitor is freed elsewhere: */
  FREE(it->data);
  FREE(it);

  D_RETURN;
}


static int
stat_internal(char *filename, struct stat *st, char use_lstat)
{
  EfsdStatcacheItem *it = NULL;

  D_ENTER;

  efsd_lock_get_read_access(stat_lock);

  /* Check if info is still in cache: */
  if (use_lstat)
    {
      D("Looking up %s in lstat cache\n", filename);
      it = (EfsdStatcacheItem*)efsd_hash_find(lstat_cache, filename);
    }
  else
    {
      D("Looking up %s in stat cache\n", filename);
      it = (EfsdStatcacheItem*)efsd_hash_find(stat_cache, filename);
    }

  if (it)
    {
      D("Cached stat for %s\n", filename);
      *st = it->st;
      efsd_lock_release_read_access(stat_lock);
      D_RETURN_(TRUE);
    }

  efsd_lock_release_read_access(stat_lock);

  /* No -- we have to stat: */

  it = NEW(EfsdStatcacheItem);
  memset(it, 0, sizeof(EfsdStatcacheItem));

  if (use_lstat)
    {
      if (lstat(filename, &(it->st)) < 0)
	{
	  D("lstat() on %s failed.\n", filename);
	  FREE(it);
	  D_RETURN_(FALSE);
	}
    }
  else
    {
      if (stat(filename, &(it->st)) < 0)
	{
	  if (lstat(filename, &(it->st)) < 0)
	    {
	      D("stat and lstat() on %s failed.\n", filename);
	      FREE(it);
	      D_RETURN_(FALSE);
	    }
	}
    }

  /* Insert item in cache and monitor file to be
     informed about updates:
  */

  efsd_lock_get_write_access(stat_lock);

  if (use_lstat)
    efsd_hash_insert(lstat_cache, (void*)strdup(filename), (void*)it);
  else
    efsd_hash_insert(stat_cache, (void*)strdup(filename), (void*)it);

  *st = it->st;

  efsd_lock_release_write_access(stat_lock);

  it->monitor = efsd_monitor_start_internal(filename, FALSE);

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

  efsd_hash_free_with_func(stat_cache, stat_hash_item_free_no_monitor_update);
  efsd_hash_free_with_func(lstat_cache, stat_hash_item_free_no_monitor_update);
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
efsd_stat_remove(char *filename, int monitor_update)
{
  D_ENTER;
  
  efsd_lock_get_write_access(stat_lock);

  if (monitor_update)
    {
      efsd_hash_remove(stat_cache, filename, NULL);
      efsd_hash_remove(lstat_cache, filename, NULL);
    }
  else
    {
      efsd_hash_remove(stat_cache, filename, (EfsdFunc)stat_hash_item_free_no_monitor_update);
      efsd_hash_remove(lstat_cache, filename, (EfsdFunc)stat_hash_item_free_no_monitor_update);
    }

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
