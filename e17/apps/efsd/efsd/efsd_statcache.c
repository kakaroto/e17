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

static EfsdHash  *stat_cache;
static EfsdHash  *lstat_cache;

static void         stat_hash_item_free(EfsdHashItem *it);
static struct stat *stat_internal(char *filename, char use_lstat);


static void         
stat_hash_item_free(EfsdHashItem *it)
{
  D_ENTER;

  if (!it)
    D_RETURN;

  /* Key is a string -- simple to free: */
  FREE(it->key);

  /* Data is a struct stat -- also easy: */
  FREE(it->data);
  FREE(it);

  D_RETURN;
}


static struct stat *
stat_internal(char *filename, char use_lstat)
{
  struct stat *st;

  D_ENTER;

  /* Check if info is still in cache: */
  if (use_lstat)
    st = (struct stat*)efsd_hash_find(lstat_cache, filename);
  else
    st = (struct stat*)efsd_hash_find(stat_cache, filename);
  
  if (st)
    {
      /* D(("Cached stat for %s\n", filename)); */
      D_RETURN_(st);
    }

  /* No -- we have to stat: */

  st = NEW(struct stat);

  if (use_lstat)
    {
      if (lstat(filename, st) < 0)
	{
	  D(("lstat() on %s failed.\n", filename));
	  FREE(st);
	  D_RETURN_(NULL);
	}
    }
  else
    {
      if (stat(filename, st) < 0)
	{
	  if (lstat(filename, st) < 0)
	    {
	      D(("stat and lstat() on %s failed.\n", filename));
	      FREE(st);
	      D_RETURN_(NULL);
	    }
	}
    }

  /* Insert in cache and monitor file to be
     informed about updates:
  */
  if (use_lstat)
    efsd_hash_insert(lstat_cache, (void*)strdup(filename), (void*)st);
  else
    efsd_hash_insert(stat_cache, (void*)strdup(filename), (void*)st);
  
  efsd_fam_start_monitor_internal(filename);

  /* D(("New lstat() on %s\n", filename)); */

  D_RETURN_(st);
}



void         
efsd_stat_init(void)
{
  D_ENTER;

  stat_cache = efsd_hash_new(1023, 10, (EfsdHashFunc)efsd_hash_string,
			     (EfsdCmpFunc)strcmp, stat_hash_item_free);

  lstat_cache = efsd_hash_new(1023, 10, (EfsdHashFunc)efsd_hash_string,
			      (EfsdCmpFunc)strcmp, stat_hash_item_free);
  
  D_RETURN;
}


struct stat *
efsd_stat(char *filename)
{
  D_ENTER;
  D_RETURN_(stat_internal(filename, FALSE));
}


struct stat *
efsd_lstat(char *filename)
{
  D_ENTER;
  D_RETURN_(stat_internal(filename, TRUE));
}


void         
efsd_stat_update(char *filename)
{
  struct stat *st;

  D_ENTER;

  if ((st = (struct stat*)efsd_hash_find(stat_cache, filename)))
    {
      if (stat(filename, st) < 0)
	{
	  D(("stat() on %s failed.\n", filename));
	}
    }

  if ((st = (struct stat*)efsd_hash_find(lstat_cache, filename)))
    {
      if (lstat(filename, st) < 0)
	{
	  D(("stat() on %s failed.\n", filename));
	}
    }

  D_RETURN;
}


void         
efsd_stat_remove(char *filename)
{
  D_ENTER;
  
  efsd_hash_remove(stat_cache, filename);
  efsd_hash_remove(lstat_cache, filename);

  D_RETURN;
}
