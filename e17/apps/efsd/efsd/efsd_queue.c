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
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <efsd_debug.h>
#include <efsd_globals.h>
#include <efsd_macros.h>
#include <efsd_io.h>
#include <efsd_main.h>
#include <efsd_misc.h>
#include <efsd_list.h>
#include <efsd_types.h>
#include <efsd_queue.h>

typedef struct efsd_queue_item
{
  int          client;
  EfsdEvent   *ee;
}
EfsdQueueItem;

EfsdList *queue      = NULL;
EfsdList *queue_last = NULL;

static EfsdQueueItem *queue_item_new(int client, EfsdEvent *ee);
static void           queue_item_free(EfsdQueueItem *eqi);


static EfsdQueueItem *
queue_item_new(int client, EfsdEvent *ee)
{
  EfsdQueueItem *result;

  D_ENTER;

  result = NEW(EfsdQueueItem);
  result->client = client;
  result->ee = ee;
  
  D_RETURN_(result);
}


static void
queue_item_free(EfsdQueueItem *eqi)
{
  D_ENTER;

  if (!eqi)
    {
      D_RETURN;
    }

  efsd_event_free(eqi->ee);
  FREE(eqi);
  
  D_RETURN;
}
		     
int 
efsd_queue_empty(void)
{
  D_ENTER;
  D_RETURN_(queue == NULL);
}


void
efsd_queue_fill_fdset(fd_set *fdset, int *fdsize)
{
  EfsdList      *q;
  EfsdQueueItem *eqi;

  D_ENTER;

  for (q = queue; q; q = efsd_list_next(q))
    {
      eqi = (EfsdQueueItem*) efsd_list_data(q);
      
      FD_SET(clientfd[eqi->client], fdset);
      if (clientfd[eqi->client] > *fdsize)
	*fdsize = clientfd[eqi->client];
    }

  D_RETURN;
}


int 
efsd_queue_process(fd_set *fdset)
{
  EfsdList      *q, *qtemp;
  EfsdQueueItem *eqi;
  int            done = 0;

  for (q = queue; q; )
    {
      eqi = (EfsdQueueItem*) efsd_list_data(q);

      if (eqi->client < 0)
	{
	  if (q == queue_last)
	    queue_last = NULL;

	  qtemp = q;
	  q = efsd_list_next(q);
	  queue = efsd_list_remove(queue, qtemp, (EfsdFunc)queue_item_free);
	  done++;
	  continue;
	}

      if (FD_ISSET(clientfd[eqi->client], fdset))
	{
	  if (efsd_io_write_event(clientfd[eqi->client], eqi->ee) < 0)
	    {
	      if (errno == EPIPE)
		{
		  D("Client %i died -- closing connection\n", eqi->client);
		  efsd_main_close_connection(eqi->client);

		  qtemp = q;
		  q = efsd_list_next(q);
		  queue = efsd_list_remove(queue, qtemp, (EfsdFunc)queue_item_free);
		  done++;
		}
	      else
		{
		  D("Couldn't write queued command.\n");
		}
	    }
	  else
	    {
	      D("One queue item processed.\n");

	      if (q == queue_last)
		queue_last = NULL;
	      
	      qtemp = q;
	      q = efsd_list_next(q);
	      queue = efsd_list_remove(queue, qtemp, (EfsdFunc)queue_item_free);
	      done++;
	    }
	}
    }

  D_RETURN_(done);
}


void
efsd_queue_add_event(int sockfd, EfsdEvent *ee)
{
  EfsdEvent *ee_copy;

  D_ENTER;

  ee_copy = NEW(EfsdEvent);
  efsd_event_duplicate(ee, ee_copy);

  if (!queue)
    {
      D("Creating job queue\n");
      queue = efsd_list_new(queue_item_new(sockfd, ee_copy));
      queue_last = queue;
    }
  else
    {
      D("Appending to job queue\n");
      queue_last = efsd_list_append(queue_last, queue_item_new(sockfd, ee_copy));
    }

  D_RETURN;
}

