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
#include <string.h>
#include <errno.h>

#include <efsd.h>
#include <efsd_debug.h>
#include <efsd_fam_r.h>
#include <efsd_filetype.h>
#include <efsd_globals.h>
#include <efsd_hash.h>
#include <efsd_io.h>
#include <efsd_macros.h>
#include <efsd_main.h>
#include <efsd_misc.h>
#include <efsd_monitor.h>
#include <efsd_queue.h>
#include <efsd_statcache.h>


EfsdHash *monitors = NULL;

/* Allocator and deallocator for a Monitor */
static EfsdMonitor     *monitor_new(EfsdCommand *com, int client,
				    int dir_mode, int is_temporary,
				    int is_sorted);
static void             monitor_free(EfsdMonitor *m);


static EfsdMonitorRequest *monitor_request_new(int client, EfsdCmdId id,
					       int num_options, EfsdOption *options);

static void             monitor_request_free(EfsdMonitorRequest *emr);

/* Increment use count for file monitor, or start
   new monitor if file is not monitored yet.
 */
static int              monitor_add_client(EfsdMonitor *m, EfsdCommand *com, int client);

/* Decreases use count on file. If count drops to
   zero, monitoring is stopped.
*/
static int              monitor_remove_client(EfsdCommand *com, int client);

static void             monitor_hash_item_free(EfsdHashItem *it);



static EfsdMonitor *         
monitor_new(EfsdCommand *com, int client, int dir_mode, int is_temporary, int is_sorted)
{
  EfsdMonitorRequest   *emr;
  EfsdMonitor   *m;

  D_ENTER;

  m = NEW(EfsdMonitor);
  memset(m, 0, sizeof(EfsdMonitor));

  m->filename   = strdup(com->efsd_file_cmd.files[0]);
  m->fam_req    = NEW(FAMRequest);
  m->clients    = NULL;
  m->files      = efsd_dca_new();

  emr = monitor_request_new(client, com->efsd_file_cmd.id,
			    com->efsd_file_cmd.num_options,
			    com->efsd_file_cmd.options);

  /* If we have options for a listdir events, unhook (not free)
     them from the command, so that they don't get freed in the
     normal command cleanup. They will get cleaned up when we
     see the acknowledge event and the EfsdMonitorRequest is freed. */
     
  if (com->efsd_file_cmd.num_options > 0)
    {
      com->efsd_file_cmd.num_options = 0;
      com->efsd_file_cmd.options = NULL;
    }
  
  m->clients = efsd_list_prepend(m->clients, emr);				   

  if (client == EFSD_CLIENT_INTERNAL)
    m->internal_use_count = 1;
  else
    m->client_use_count = 1;

  m->is_dir = dir_mode;
  m->is_temporary = is_temporary;
  m->is_sorted = is_sorted;

  efsd_hash_insert(monitors, m->filename, m);

  D_RETURN_(m);
}


static void             
monitor_free(EfsdMonitor *m)
{
  D_ENTER;

  if (!m)
    D_RETURN;

  /* We need to make sure that if the monitored file is in
     the statcache, it gets removed from the cache now.
     Otherwise, 
  */
  efsd_stat_remove(m->filename);


  FREE(m->filename);
  FREE(m->fam_req);

  efsd_list_free(m->clients, (EfsdFunc)monitor_request_free);
  efsd_dca_free(m->files);

  FREE(m);

  D_RETURN;
}


static EfsdMonitorRequest *
monitor_request_new(int client, EfsdCmdId id,
		    int num_options, EfsdOption *options)
{
  EfsdMonitorRequest   *emr;
  
  D_ENTER;

  emr = NEW(EfsdMonitorRequest);

  emr->client      = client;
  emr->id          = id;
  emr->num_options = num_options;
  emr->options     = options;

  D_RETURN_(emr);
}


static void             
monitor_request_free(EfsdMonitorRequest *emr)
{
  int i;

  D_ENTER;

  for (i = 0; i < emr->num_options; i++)
    efsd_option_cleanup(&emr->options[i]);

  FREE(emr->options);
  FREE(emr);

  D_RETURN;
}


static void             
monitor_hash_item_free(EfsdHashItem *it)
{
  EfsdMonitor *m = NULL;

  D_ENTER;

  if (!it)
    D_RETURN;

  /* Key is a string inside the monitor,
     so we don't need to free it separately.
  */

  /* Data is an EfsdMonitor -- and this is a problem:
     If we free the monitor right away, we'll segfault
     if an event is reported for this monitor. Thus
     we need to stop the monitor, thus causing it to be
     freed when we see the corresponding FAMAcknowledge.
  */

  m = (EfsdMonitor*)it->data;

  if (m->is_finished)
    monitor_free(m);
  else
    FAMCancelMonitor_r(&famcon, m->fam_req);

  it->data = NULL;
  FREE(it);
  
  D_RETURN;
}


static int
monitor_add_client(EfsdMonitor *m, EfsdCommand *com, int client)
{
  EfsdList           *l2 = NULL;
  EfsdMonitorRequest *emr;
  char                list_all_files = FALSE, sort_files = FALSE;
  int                 i;

  D_ENTER;

  if (!m)
    D_RETURN_(FALSE);

  for (l2 = efsd_list_head(m->clients); l2; l2 = efsd_list_next(l2))
    {
      if (((EfsdMonitorRequest*)efsd_list_data(l2))->client == client)
	{
	  if (client == EFSD_CLIENT_INTERNAL)
	    {
	      m->internal_use_count++;
	      D("Incrementing internal use count for monitoring file %s, now (%i/%i).\n",
		 m->filename, m->internal_use_count, m->client_use_count);
	    }
	  else
	    {
	      D("Client %i already monitors %s\n", client, m->filename);
	    }

	  D_RETURN_(TRUE);
	}
    }
  
      
  if (client == EFSD_CLIENT_INTERNAL)
    {
      m->internal_use_count++;
      D("Incrementing internal use count for monitoring file %s, now (%i/%i).\n",
	m->filename, m->internal_use_count, m->client_use_count);
    }
  else
    {
      m->client_use_count++;
      D("Incrementing client use count for monitoring file %s, now (%i/%i).\n",
	m->filename, m->internal_use_count, m->client_use_count);
    }
  
  emr = monitor_request_new(client, com->efsd_file_cmd.id,
			    com->efsd_file_cmd.num_options,
			    com->efsd_file_cmd.options);
  
  m->clients = efsd_list_prepend(m->clients, emr);

  for (i = 0; i < emr->num_options; i++)
    {
      switch (emr->options[i].type)
	{
	case EFSD_OP_ALL:
	  list_all_files = TRUE;
	  break;
	case EFSD_OP_SORT:
	  sort_files = TRUE;
	  break;
	default:
	}
    }
  
  if (client != EFSD_CLIENT_INTERNAL)
    {
      if (m->is_dir)
	{
	  char *filename;
	  int index = 0;
	  
	  /* Sort the files in the monitor on request,
	     it's a no-op if they're still sorted.
	   */
	  if (sort_files)
	    efsd_dca_sort(m->files);
	  
	  /* ... and handle each of them. */
	  while ((filename = efsd_dca_get(m->files, index)) != NULL)
	    {
	      if ((list_all_files || !efsd_misc_file_is_dotfile(filename)))
		{
		  efsd_monitor_send_filechange_event(m, emr, EFSD_FILE_EXISTS, filename);
		}
	      
	      index++;
	    }
	  
	  /* Send FILE_END_EXISTS so that the client knows that the end is reached. */
	  efsd_monitor_send_filechange_event(m, emr, EFSD_FILE_END_EXISTS, m->filename);
	}
      else
	{
	  efsd_monitor_send_filechange_event(m, emr, EFSD_FILE_EXISTS, m->filename);
	  efsd_monitor_send_filechange_event(m, emr, EFSD_FILE_END_EXISTS, m->filename);
	}
    }

  D_RETURN_(TRUE);
}


static int
monitor_remove_client(EfsdCommand *com, int client)
{
  EfsdMonitor  *m = NULL;
  char         *filename;

  D_ENTER;

  if (!com)
    D_RETURN_(-1);

  filename = com->efsd_file_cmd.files[0];
  efsd_misc_remove_trailing_slashes(filename);

  if ((client == EFSD_CLIENT_INTERNAL)                          &&
      ((!strcmp(filename, efsd_filetype_get_magic_db()))        ||
       (!strcmp(filename, efsd_filetype_get_sys_patterns_db())) ||
       (!strcmp(filename, efsd_filetype_get_user_patterns_db()))))
    {
      /* It's an internal monitor, and it's monitoring
	 one of our filetype databases -- never stop
	 monitoring those ... */
      D_RETURN_(0);
    }

  m = efsd_monitored(filename);

  if (!m)
    D_RETURN_(-1);

  if (client == EFSD_CLIENT_INTERNAL)
    m->internal_use_count--;
  else
    m->client_use_count--;

  D("Decrementing usecount for file %s on monitor %s, now (%i/%i).\n",
    filename, m->filename, m->internal_use_count, m->client_use_count);

  if (m->internal_use_count == 0 && m->client_use_count == 0)
    {
      D("Use count is (%i/%i) -- stopping monitoring of %s.\n",
	m->internal_use_count, m->client_use_count, filename);
      
      FAMCancelMonitor_r(&famcon, m->fam_req);
      
      /* Actual cleanup happens when FAMAcknowledge is seen ... */
    }
  else
    {
      EfsdMonitorRequest    *emr;
      EfsdList              *l2;
      
      /* Use count not zero -- remove given client
	 from list of monitoring clients. */
      for (l2 = efsd_list_head(m->clients); l2; l2 = efsd_list_next(l2))
	{
	  emr = (EfsdMonitorRequest*)efsd_list_data(l2);

	  if (emr->client != client)
	    continue;
	  
	  if (client == EFSD_CLIENT_INTERNAL)
	    {
	      if (m->internal_use_count == 0)
		{
		  D("Use count on %s is (%i/%i) -- removing internal.\n",
		    m->filename, m->internal_use_count, m->client_use_count);
		  m->clients = efsd_list_remove(m->clients, l2,
						(EfsdFunc)monitor_request_free);
		}
	    }
	  else
	    {
	      D("Use count on %s is (%i/%i) -- removing client %i.\n",
		m->filename, m->internal_use_count, m->client_use_count, client);
	      m->clients = efsd_list_remove(m->clients, l2,
					    (EfsdFunc)monitor_request_free);
	    }
	  
	  l2 = NULL;
	  break;
	}
    }

  D_RETURN_(0);
}

/*
static EfsdMonitor*
monitored_internally(char *filename)
{
  EfsdMonitor   *m;
  EfsdList         *l;

  D_ENTER;
  
  m = efsd_hash_find(monitors, filename);
      
  if (m)
    {
      for (l = efsd_list_head(m->clients); l; l = efsd_list_next(l))
	{
	  if (((EfsdMonitorRequest*)efsd_list_data(l))->client == EFSD_CLIENT_INTERNAL)
	    D_RETURN_(m);
	}
    }

  D_RETURN_(NULL);
}
*/

void         
efsd_monitor_init(void)
{
  D_ENTER;

  if ((FAMOpen_r(&famcon)) < 0)
    {
      fprintf(stderr, "Can not connect to fam -- exiting.\n");
      exit(-1);
    }

  monitors = efsd_hash_new(1023, 10, (EfsdHashFunc)efsd_hash_string,
			   (EfsdCmpFunc)strcmp, monitor_hash_item_free);

  D_RETURN;
}


void         
efsd_monitor_cleanup(void)
{  
  D_ENTER;

  efsd_hash_free(monitors);
  monitors = NULL;
  FAMClose_r(&famcon);

  D_RETURN;
}


EfsdMonitor *
efsd_monitor_start(EfsdCommand *com, int client, int dir_mode, int do_sort)
{
  EfsdMonitor  *m = NULL;

  D_ENTER;

  if (!com)
    D_RETURN_(NULL);

  efsd_misc_remove_trailing_slashes(com->efsd_file_cmd.files[0]);

  if ((m = efsd_monitored(com->efsd_file_cmd.files[0])) == NULL)
    {      
      m = monitor_new(com, client, dir_mode, FALSE, do_sort);

      if (efsd_misc_file_is_dir(m->filename) && dir_mode)
	{
	  D("Starting monitoring dir %s.\n", m->filename);
	  if (FAMMonitorDirectory_r(&famcon, m->filename, m->fam_req, m) < 0)
	    {
	      D("Starting monitoring %s FAILED.\n", m->filename);
	      efsd_monitor_remove(m);	      
	    }
	}
      else
	{
	  D("Starting monitoring file %s.\n", m->filename);
	  if (FAMMonitorFile_r(&famcon, m->filename, m->fam_req, m) < 0)
	    {
	      D("Starting monitoring %s FAILED.\n", m->filename);
	      efsd_monitor_remove(m);	      
	    }
	}      
    }
  else
    {
      monitor_add_client(m, com, client);
    }

  D_RETURN_(m);
}


int         
efsd_monitor_stop(EfsdCommand *com, int client)
{
  D_ENTER;
  D_RETURN_(monitor_remove_client(com, client));
}


EfsdMonitor *
efsd_monitor_start_internal(char *filename, int dir_mode)
{
  EfsdCommand       com;
  
  D_ENTER;

  if (!filename || filename[0] == '\0')
    D_RETURN_(NULL);

  memset(&com, 0, sizeof(EfsdCommand));
  com.efsd_file_cmd.files = &filename;

  D_RETURN_(efsd_monitor_start(&com, EFSD_CLIENT_INTERNAL, dir_mode, TRUE));
}


int              
efsd_monitor_stop_internal(char *filename)
{
  EfsdCommand       com;

  D_ENTER;

  if (!filename || filename[0] == '\0')
    D_RETURN_(-1);

  memset(&com, 0, sizeof(EfsdCommand));
  com.efsd_file_cmd.files = &filename;

  D_RETURN_(efsd_monitor_stop(&com, EFSD_CLIENT_INTERNAL));
}


EfsdMonitor *
efsd_monitored(char *filename)
{
  char path[MAXPATHLEN];
  EfsdMonitor *m = NULL;

  D_ENTER;

  efsd_misc_remove_trailing_slashes(filename);
  m = efsd_hash_find(monitors, filename);

  if (m)
    {
      D("%s is monitored, as dir: %i\n", filename, m->is_dir);
      D_RETURN_(m);
    }

  /* If it's not directly monitored, maybe it's treated
     indirectly by a monitor for the directory the file
     is in: */

  if (!efsd_misc_get_path_only(filename, path, MAXPATHLEN))
    D_RETURN_(NULL);

  m = efsd_hash_find(monitors, path);

  /* We found a monitor for a directory. Now it must be
     monitoring the directory and its contents, not just
     the directory file. */

  if (m && m->is_dir)
    {
      D("%s is dir-monitored, so %s is monitored.\n",
	 m->filename, filename);
      D_RETURN_(m);
    }
  
  D_RETURN_(NULL);
}


void             
efsd_monitor_remove(EfsdMonitor *m)
{
  D_ENTER;

  if (!m)
    D_RETURN;
  
  m->is_finished = TRUE;

  if (m->is_temporary)
    {
      monitor_free(m);
    }
  else
    {
      D("Freeing monitor for %s.\n", m->filename);
      efsd_hash_remove(monitors, m->filename);    
    }

  D_RETURN;
}


int          
efsd_monitor_cleanup_client(int client)
{
  EfsdList         *c;
  EfsdHashIterator *it;
  EfsdMonitor   *m;

  D_ENTER;

  for (it = efsd_hash_it_new(monitors); efsd_hash_it_valid(it); efsd_hash_it_next(it))
    {
      m = (EfsdMonitor *) ((efsd_hash_it_item(it))->data);
      
      c = efsd_list_head(m->clients);
      while (c)
	{
	  if (((EfsdMonitorRequest*)efsd_list_data(c))->client == client)
	    break;

	  c = efsd_list_next(c);
	}

      if (!c)
	continue;

      if (client == EFSD_CLIENT_INTERNAL)
	m->internal_use_count--;
      else
	m->client_use_count--;
      
      D("Client %i found monitoring %s, use count now (%i/%i)\n",
	client, m->filename, m->internal_use_count, m->client_use_count);

      if (m->client_use_count == 0 && m->internal_use_count == 0)
	{
	  /* Use count dropped to zero -- stop monitoring. */
	  D("Stopping monitoring %s.\n", m->filename);
	  FAMCancelMonitor_r(&famcon, m->fam_req);
	}
      else
	{
	  if ((client == EFSD_CLIENT_INTERNAL && m->internal_use_count >= 0) ||
	      (client != EFSD_CLIENT_INTERNAL && m->client_use_count >= 0))
	    {
	      if (!efsd_list_prev(c) && !efsd_list_next(c))
		{
		  /* This cannot happen -- use count is not zero! */
		  fprintf(stderr, "FAM connection handling error -- ouch.\n");
		  exit(-1);
		}
	      
	      D("Removing client %i from monitor for %s\n", client, m->filename);
	      /* Use count not zero, but remove client from list of users */
	      m->clients = efsd_list_remove(m->clients, c, (EfsdFunc)monitor_request_free);
	    }
	}
    }

  efsd_hash_it_free(it);
  D_RETURN_(FALSE);
}


int              
efsd_monitor_send_filechange_event(EfsdMonitor *m, EfsdMonitorRequest *emr,
				   EfsdFilechangeType type, char *filename)
{
  EfsdEvent   ee;
  int         result = TRUE;

  D_ENTER;

  memset(&ee, 0, sizeof(EfsdEvent));
  ee.type = EFSD_EVENT_FILECHANGE;
  ee.efsd_filechange_event.changetype = type;
  ee.efsd_filechange_event.file = filename;

  /* Register the command id in the reply event: */
  ee.efsd_filechange_event.id = emr->id;
			  
  /* D("Writing FAM event %i to client %i\n",
     famev.code, emr->client);
  */

  if (efsd_io_write_event(clientfd[emr->client], &ee) < 0)
    {
      if (errno == EPIPE)
	{
	  D("Client %i died -- closing connection.\n", emr->client);
	  efsd_main_close_connection(emr->client);
	}
      else
	{
	  efsd_queue_add_event(clientfd[emr->client], &ee);
	  D("write() error when writing FAM event.\n");
	}

      result = FALSE;
    }
	      
  if ((type == EFSD_FILE_EXISTS)  ||
      (type == EFSD_FILE_CHANGED) ||
      (type == EFSD_FILE_CREATED))
    {
      if (filename[0] != '/')
	{
	  char        s[MAXPATHLEN];

	  snprintf(s, MAXPATHLEN, "%s/%s", m->filename, filename); 
	  efsd_main_handle_file_exists_options(s, emr);
	}
      else
	{
	  efsd_main_handle_file_exists_options(filename, emr);
	}
    }

  D_RETURN_(result);
}

