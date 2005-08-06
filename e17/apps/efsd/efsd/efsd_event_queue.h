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
#ifndef efsd_event_queue_h
#define efsd_event_queue_h

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <efsd.h>
#include <efsd_queue.h>

/* Tries to process as many items in the queue as possible.
   Returns number of items processed, 0 if none got processed.
*/
int  efsd_event_queue_process(EfsdQueue *q, fd_set *fdset);

/* Fills an fd_set with the file descriptors of events that
   are waiting to be sent in the queue.
*/
void efsd_event_queue_fill_fdset(EfsdQueue *q, fd_set *fdset, int *fdsize);

/* Adds an event that is supposed to be delivered to SOCKFD,
   making a copy of event EE.
*/

#if HAVE_ECORE
void efsd_event_queue_add_event(EfsdQueue *q, Ecore_Ipc_Client* client, EfsdEvent *ee);
#else
void efsd_event_queue_add_event(EfsdQueue *q, int client, EfsdEvent *ee);
#endif

#endif 
