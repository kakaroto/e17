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
#ifndef efsd_lock_h
#define efsd_lock_h

#if HAVE_CONFIG_H
# include <config.h>
#endif

/* This is a lock mechanism that implements an
   abstraction of the multiple-read/single-write
   paradigm. It can protect a resource by allowing
   multiple readers of a resource to run in parallel,
   but allowing only one writer. It also guarantees
   that no reads are in progress when a write begins
   and no writes are in progress when a read begins.
*/

#  if USE_THREADS

#  include <pthread.h>

#define LOCK(X)    (pthread_mutex_lock(X))
#define UNLOCK(X)  (pthread_mutex_unlock(X))

typedef struct efsd_lock EfsdLock;

EfsdLock *efsd_lock_new(void);
void      efsd_lock_free(EfsdLock *l);

void      efsd_lock_get_write_access(EfsdLock *l);
void      efsd_lock_release_write_access(EfsdLock *l);
void      efsd_lock_get_read_access(EfsdLock *l);
void      efsd_lock_release_read_access(EfsdLock *l);

#  else

/* We're not using threads -- reduce EfsdLock* to
   void pointers and hide the function calls to
   minimize #ifdef clutter...
*/

#define LOCK(X)
#define UNLOCK(X)

typedef void EfsdLock;

#  define efsd_lock_new() NULL
#  define efsd_lock_free(X)
#  define efsd_lock_get_write_access(X)
#  define efsd_lock_release_write_access(X)
#  define efsd_lock_get_read_access(X)
#  define efsd_lock_release_read_access(X)
#  endif


#endif
