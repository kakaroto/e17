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
#ifndef efsd_queue_h
#define efsd_queue_h

#include <efsd_list.h>

typedef struct efsd_queue EfsdQueue;
typedef struct efsd_queue_iterator EfsdQueueIterator;


/* Creates new empty queue.
 */
EfsdQueue *efsd_queue_new(void);

/* Cleans up queue using the given destructor function
   for the remaining items.
*/
void       efsd_queue_free(EfsdQueue *q, EfsdFunc free_func);

/* Appends an item to the end of the queue.
 */
void       efsd_queue_append_item(EfsdQueue *q, void *data);

/* Removes and returns the next item from the queue.
 */
void      *efsd_queue_remove_item(EfsdQueue *q);

/* Returns next item to be processed in the queue,
   without removing it from the queue.
*/
void      *efsd_queue_next_item(EfsdQueue *q);

/* Returns value > 0 when items are in the queue.
 */
int        efsd_queue_empty(EfsdQueue *q);


/* Returns number of items currently queued.
 */
int        efsd_queue_size(EfsdQueue *q);

/* Queue iterator -- iterates over all items
   that are currently queued */

EfsdQueueIterator *efsd_queue_it_new(EfsdQueue *h);
void               efsd_queue_it_free(EfsdQueueIterator *it);
void              *efsd_queue_it_item(EfsdQueueIterator *it);
int                efsd_queue_it_remove(EfsdQueueIterator *it);
int                efsd_queue_it_next(EfsdQueueIterator *it);
int                efsd_queue_it_valid(EfsdQueueIterator *it);

#endif 
