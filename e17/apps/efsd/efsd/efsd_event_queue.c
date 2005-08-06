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

typedef struct efsd_event_q_item
{
 #if HAVE_ECORE
 Ecore_Ipc_Client* client;
 #else
 int          client;
 #endif
  EfsdEvent   *ee;
}
EfsdEventQItem;

#if HAVE_ECORE
static EfsdEventQItem *
event_queue_item_new(Ecore_Ipc_Client* client, EfsdEvent *ee);
#else
static EfsdEventQItem *
event_queue_item_new(int client, EfsdEvent *ee);
#endif

static void            event_queue_item_free(EfsdEventQItem *eqi);


#if HAVE_ECORE
static EfsdEventQItem *
event_queue_item_new(Ecore_Ipc_Client* client, EfsdEvent *ee)
#else
static EfsdEventQItem *
event_queue_item_new(int client, EfsdEvent *ee)
#endif
	
{
  EfsdEventQItem *result;

  D_ENTER;

  result = NEW(EfsdEventQItem);
  result->client = client;
  result->ee = ee;
  
  D_RETURN_(result);
}


static void
event_queue_item_free(EfsdEventQItem *eqi)
{
  D_ENTER;

  if (!eqi)
    D_RETURN;

  efsd_event_free(eqi->ee);
  FREE(eqi);
  
  D_RETURN;
}
		     

void
efsd_event_queue_fill_fdset(EfsdQueue *q, fd_set *fdset, int *fdsize)
{
  EfsdQueueIterator *eqi;
  EfsdEventQItem    *qi;
  int                fd;

  D_ENTER;

  for (eqi = efsd_queue_it_new(q); efsd_queue_it_valid(eqi); efsd_queue_it_next(eqi))
    {      
      qi = (EfsdEventQItem*) efsd_queue_it_item(eqi);
      fd = efsd_main_get_fd(qi->client); 

      if (fd > 0)
	{
	  FD_SET(fd, fdset);
	  if (fd > *fdsize)
	    *fdsize = fd;
	}
    }

  efsd_queue_it_free(eqi);

  D_RETURN;
}


int 
efsd_event_queue_process(EfsdQueue *q, fd_set *fdset)
{
  EfsdQueueIterator *eqi;
  EfsdEventQItem    *qi;
  int                fd, done = 0;

  D_ENTER;

  /* We need to look at each item in the queue and test
     if an item's file descriptor is ready to be written to.
     In that case, try to write the command and remove it
     from the queue.
  */

  if (!q || !fdset)
    D_RETURN_(FALSE);

  for (eqi = efsd_queue_it_new(q); efsd_queue_it_valid(eqi); efsd_queue_it_next(eqi))
    {
      qi = (EfsdEventQItem*) efsd_queue_it_item(eqi);
      

        #if HAVE_ECORE
      
        /*ecore_ipc_client_send() */
      
        #else

        fd = efsd_main_get_fd(qi->client);
        if (fd < 0)
	{
	  efsd_queue_it_remove(eqi);
	  event_queue_item_free(qi);	  
	  done++;
	  continue;
	}


      if (FD_ISSET(fd, fdset))
	{
	  if (efsd_io_write_event(fd, qi->ee) < 0)
	    {
	      if (errno == EPIPE)
		{
		  D("Client %i died -- closing connection\n", qi->client);
		  efsd_main_close_connection(qi->client);
		  done++;
		}
	      else
		{
		  D("Couldn't write queued command.\n");
		  continue;
		}

	    }
	  else
	    {
	      D("One queue item processed.\n");
	      done++;
	    }  

	  efsd_queue_it_remove(eqi);
	  event_queue_item_free(qi);	  
	}
        #endif
    }

  efsd_queue_it_free(eqi);

  D_RETURN_(done);
}


#if HAVE_ECORE
efsd_event_queue_add_event(EfsdQueue *q, Ecore_Ipc_Client* client, EfsdEvent *ee)
#else
void
efsd_event_queue_add_event(EfsdQueue *q, int client, EfsdEvent *ee)
#endif
	
{
  EfsdEvent *ee_copy;

  D_ENTER;

  ee_copy = NEW(EfsdEvent);
  efsd_event_duplicate(ee, ee_copy);
  efsd_queue_append_item(q, event_queue_item_new(client, ee_copy));

  D_RETURN;
}

