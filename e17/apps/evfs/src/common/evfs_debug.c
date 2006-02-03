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

#include "evfs.h"

#if USE_THREADS
#include <pthread.h>

/* This is a thread-specific call depth register,
   and a mutex to lock output for a single thread
   at a time.
*/

static pthread_mutex_t debug_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t debug_once = PTHREAD_ONCE_INIT;
static pthread_key_t debug_calldepth_key;

static void debug_once_init(void);
static void debug_destructor(void *item);
#else

/* We're not using threads -- here's a simple calldepth
   variable, and a pointer to it to be code-compatible
   to the threaded case.
*/
static int calldepth = 0;
static void *calldepth_ptr = &calldepth;
#endif

static void debug_whitespace(int calldepth);
static void debug_print_thread_info(void);

static void
debug_whitespace(int calldepth)
{
   int i;

   for (i = 0; i < 2 * calldepth; i++)
      printf("-");
}

static void
debug_print_thread_info(void)
{
   printf("evfs ");

#if USE_THREADS
   printf("[%li]: ", pthread_self());
#else
   printf("[%i]: ", getpid());
#endif
}

#if USE_THREADS
static void
debug_once_init(void)
{
   pthread_key_create(&debug_calldepth_key, debug_destructor);
}

static void
debug_destructor(void *item)
{
   free(item);
}
#endif

void
evfs_debug_output_start(void)
{
#if USE_THREADS
   pthread_mutex_lock(&debug_mutex);
#endif

   debug_print_thread_info();
}

void
evfs_debug_output_end(void)
{
   fflush(stdout);

#if USE_THREADS
   pthread_mutex_unlock(&debug_mutex);
#endif
}

void
evfs_debug_enter(const char *file, const char *func)
{
#if USE_THREADS
   void *calldepth_ptr;

   pthread_once(&debug_once, debug_once_init);
   if ((calldepth_ptr = pthread_getspecific(debug_calldepth_key)) == NULL)
     {
        calldepth_ptr = malloc(sizeof(int));
        *((int *)calldepth_ptr) = 0;
        pthread_setspecific(debug_calldepth_key, calldepth_ptr);
     }

   pthread_mutex_lock(&debug_mutex);
#endif

   (*((int *)calldepth_ptr))++;
   printf("ENTER  ");
   debug_print_thread_info();
   debug_whitespace(*((int *)calldepth_ptr));
   printf("%s, %s()\n", file, func);
   fflush(stdout);

#if USE_THREADS
   pthread_mutex_unlock(&debug_mutex);
#endif
}

void
evfs_debug_return(const char *file, const char *func)
{
#if USE_THREADS
   void *calldepth_ptr;

   calldepth_ptr = pthread_getspecific(debug_calldepth_key);
   pthread_mutex_lock(&debug_mutex);
#endif

   printf("RETURN ");
   debug_print_thread_info();
   debug_whitespace(*((int *)calldepth_ptr));
   printf("%s, %s()\n", file, func);
   fflush(stdout);

   (*((int *)calldepth_ptr))--;

#if USE_THREADS
   pthread_mutex_unlock(&debug_mutex);
#endif
}
