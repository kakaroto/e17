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

#include <efsd_macros.h>
#include <efsd_debug.h>
#include <efsd_misc.h>
#include <efsd_list.h>
#include <efsd_queue.h>


struct efsd_queue
{
  EfsdList      *q;
  int            size;
};

struct efsd_queue_iterator
{
  EfsdQueue     *q;
  EfsdList      *it;
};


EfsdQueue *
efsd_queue_new(void)
{
  EfsdQueue *q;

  D_ENTER;

  q = NEW(EfsdQueue);
  q->q = NULL;
  q->size = 0;

  D_RETURN_(q);
}


void       
efsd_queue_free(EfsdQueue *q, EfsdFunc free_func)
{
  D_ENTER;

  if (!q)
    D_RETURN;

  efsd_list_free(q->q, free_func);
  FREE(q);
  
  D_RETURN;
}


void       
efsd_queue_append_item(EfsdQueue *q, void *data)
{
  D_ENTER;

  if (!q || !data)
    D_RETURN;

  if (!q->q)
    q->q = efsd_list_new(data);
  else
    efsd_list_append(q->q, data);

  q->size++;
  
  D_RETURN;
}


void      *
efsd_queue_remove_item(EfsdQueue *q)
{
  void *data = NULL;

  D_ENTER;

  if (!q || !q->q)
    D_RETURN_(NULL);

  data = efsd_list_data(q->q);
  q->q = efsd_list_remove(q->q, q->q, NULL);
  q->size--;

  D_RETURN_(data);
}


void      *
efsd_queue_next_item(EfsdQueue *q)
{
  D_ENTER;

  if (!q || !q->q)
    D_RETURN_(NULL);

  D_RETURN_(efsd_list_data(q->q));
}


int        
efsd_queue_empty(EfsdQueue *q)
{
  int result;

  D_ENTER;

  result = (q->q == NULL);

  D_RETURN_(result);
}


int        
efsd_queue_size(EfsdQueue *q)
{
  D_ENTER;

  if (!q)
    D_RETURN_(0);

  D_RETURN_(q->size);
}


EfsdQueueIterator *
efsd_queue_it_new(EfsdQueue *q)
{
  EfsdQueueIterator *qit = NULL;

  D_ENTER;

  qit = NEW(EfsdQueueIterator);
  
  qit->q = q;
  qit->it = q->q;

  D_RETURN_(qit);
}


void               
efsd_queue_it_free(EfsdQueueIterator *it)
{
  D_ENTER;

  /* Nothing to free in the queue itself -- it's just an iterator */
  FREE(it);

  D_RETURN;
}


void *
efsd_queue_it_item(EfsdQueueIterator *it)
{
  D_ENTER;

  if (!it)
    D_RETURN_(NULL);  

  D_RETURN_(efsd_list_data(it->it));
}


int                
efsd_queue_it_remove(EfsdQueueIterator *it)
{
  D_ENTER;

  if (!it)
    D_RETURN_(FALSE);

  if (it->it)
    {
      EfsdList *next = NULL;

      next = efsd_list_next(it->it);
      it->q->q = efsd_list_remove(it->q->q, it->it, NULL);
      it->q->size--;
      it->it = next;

      D_RETURN_(TRUE);
    }

  D_RETURN_(FALSE);
}


int                
efsd_queue_it_next(EfsdQueueIterator *it)
{
  D_ENTER;

  if (!it)
    D_RETURN_(FALSE);

  if (it->it)
    {
      it->it = efsd_list_next(it->it);

      if (it->it)
	D_RETURN_(TRUE);
    }

  D_RETURN_(FALSE);
}


int                
efsd_queue_it_valid(EfsdQueueIterator *it)
{
  D_ENTER;

  if (!it)
    D_RETURN_(FALSE);

  if (it->it)
    D_RETURN_(TRUE);
  
  D_RETURN_(FALSE);
}

