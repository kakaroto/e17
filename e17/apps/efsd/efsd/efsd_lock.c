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
#include <unistd.h>

#include <efsd.h>
#include <efsd_debug.h>
#include <efsd_macros.h>
#include <efsd_misc.h>
#include <efsd_lock.h>

struct efsd_lock
{
  int             readers;
  pthread_mutex_t readers_mutex;
  pthread_cond_t  readers_cond;
  pthread_mutex_t writer_mutex;
};


EfsdLock *
efsd_lock_new(void)
{
  EfsdLock *l = NULL;

  D_ENTER;

  l = NEW(EfsdLock);

  l->readers = 0;
  pthread_mutex_init(&l->readers_mutex, NULL);
  pthread_cond_init(&l->readers_cond, NULL);
  pthread_mutex_init(&l->writer_mutex, NULL);

  D_RETURN_(l);
}


void
efsd_lock_free(EfsdLock *l)
{
  D_ENTER;

  if (!l)
    D_RETURN;
  
  pthread_mutex_destroy(&l->readers_mutex);
  pthread_cond_destroy(&l->readers_cond);
  pthread_mutex_destroy(&l->writer_mutex);

  FREE(l);
 
  D_RETURN;
}


void    
efsd_lock_get_write_access(EfsdLock *l)
{
  D_ENTER;

  pthread_mutex_lock(&l->readers_mutex);

  while (l->readers > 0)
    pthread_cond_wait(&l->readers_cond, &l->readers_mutex);

  pthread_mutex_lock(&l->writer_mutex);

  D_RETURN;
}


void    
efsd_lock_release_write_access(EfsdLock *l)
{
  D_ENTER;

  pthread_mutex_unlock(&l->writer_mutex);
  pthread_mutex_unlock(&l->readers_mutex);

  D_RETURN;
}


void    
efsd_lock_get_read_access(EfsdLock *l)
{
  D_ENTER;

  pthread_mutex_lock(&l->readers_mutex);
  l->readers++;
  pthread_mutex_unlock(&l->readers_mutex);

  D_RETURN;
}


void    
efsd_lock_release_read_access(EfsdLock *l)
{
  D_ENTER;

  pthread_mutex_lock(&l->readers_mutex);  
  if (--l->readers == 0)
    pthread_cond_broadcast(&l->readers_cond);
  pthread_mutex_unlock(&l->readers_mutex);

  D_RETURN;
}

