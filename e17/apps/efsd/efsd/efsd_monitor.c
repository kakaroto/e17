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

#ifdef DEBUG
#undef DEBUG
#endif

#include <efsd.h>
#include <efsd_debug.h>
#include <efsd_fam_r.h>
#include <efsd_filetype.h>
#include <efsd_globals.h>
#include <efsd_hash.h>
#include <efsd_io.h>
#include <efsd_lock.h>
#include <efsd_macros.h>
#include <efsd_main.h>
#include <efsd_misc.h>
#include <efsd_monitor.h>
#include <efsd_event_queue.h>
#include <efsd_statcache.h>

static EfsdHash *monitors = NULL;
static EfsdLock *monitors_lock;

typedef struct efsd_monitor_key
{
  char        *filename;
  int          dir_mode;
}
EfsdMonitorKey;

/* Allocator and deallocator for a Monitor */
static EfsdMonitor        *monitor_new(EfsdCommand *com, Ecore_Ipc_Client* client,
				       int dir_mode, int is_temporary,
				       int is_sorted);
static void                monitor_free(EfsdMonitor *m);


/* Increment use count for file monitor, or start
   new monitor if file is not monitored yet.
 */
static int                 monitor_add_client(EfsdMonitor *m, EfsdCommand *com, Ecore_Ipc_Client* client);

/* Returns list item containing monitoring request as data,
   if monitor contains request by client CLIENT:
 */
static EfsdList           *monitor_has_client(EfsdMonitor *m, int client);

/* Decreases use count on file. If count drops to
   zero, monitoring is stopped.
*/
static int                 monitor_remove_client(EfsdCommand *com, int client, int dir_mode);

static void                monitor_hash_item_free(EfsdHashItem *it);

static EfsdMonitorKey     *monitor_key_new(char *filename, int dir_mode);
static void                monitor_key_init(EfsdMonitorKey *emk);
static void                monitor_key_free(EfsdMonitorKey *emk);
static int                 monitor_key_cmp(EfsdMonitorKey *k1, EfsdMonitorKey *k2);
static unsigned int        monitor_key_hash(EfsdHash *monitors, EfsdMonitorKey *emk);


static EfsdMonitorKey *
monitor_key_new(char *filename, int dir_mode)
{
  EfsdMonitorKey *emk = NULL;

  D_ENTER;

  emk = NEW(EfsdMonitorKey);
  memset(emk, 0, sizeof(EfsdMonitorKey));

  emk->filename = filename;
  emk->dir_mode = dir_mode;

  D_RETURN_(emk);
}


static void             
monitor_key_init(EfsdMonitorKey *emk)
{
  D_ENTER;

  if (!emk)
    D_RETURN;

  memset(emk, 0, sizeof(EfsdMonitorKey));

  D_RETURN;
}


static void             
monitor_key_free(EfsdMonitorKey *emk)
{
  D_ENTER;

  if (!emk)
    D_RETURN;

  FREE(emk->filename);
  FREE(emk);

  D_RETURN;
}


static unsigned int     
monitor_key_hash(EfsdHash *monitors, EfsdMonitorKey *emk)
{
  unsigned int hash;

  D_ENTER;

  hash = efsd_hash_string(monitors, emk->filename);

  D_RETURN_(hash);
}


static int              
monitor_key_cmp(EfsdMonitorKey *k1, EfsdMonitorKey *k2)
{
  D_ENTER;

  if (!k1 || !k2)
    D_RETURN_(1);

  D("Comparing %s %i -- %s %i\n",
    k1->filename, k1->dir_mode,
    k2->filename, k2->dir_mode);

  D("Returning %i\n",
    (strcmp(k1->filename, k2->filename) || !(k1->dir_mode == k2->dir_mode)));

  /* Return 0 when filenames and dirmodes match, 1 otherwise: */

  D_RETURN_((strcmp(k1->filename, k2->filename) || !(k1->dir_mode == k2->dir_mode)));
}


static EfsdMonitor *         
monitor_new(EfsdCommand *com, Ecore_Ipc_Client* client, int dir_mode, int is_temporary, int is_sorted)
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

  emr = efsd_monitor_request_new(client, &com->efsd_file_cmd);
  
  m->clients = efsd_list_prepend(m->clients, emr);				   

  if (client == EFSD_CLIENT_INTERNAL)
    m->internal_use_count = 1;
  else
    m->client_use_count = 1;

#if USE_THREADS
  pthread_mutex_init(&m->use_count_mutex, NULL);
#endif

  m->is_dir = dir_mode;
  m->is_temporary = is_temporary;
  m->is_sorted = is_sorted;
  m->is_receiving_exist_events = TRUE;

  efsd_lock_get_write_access(monitors_lock);
  efsd_hash_insert(monitors, monitor_key_new(m->filename, dir_mode), m);
  efsd_lock_release_write_access(monitors_lock);

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
     Also pass FALSE for the monitor updating, because
     we're already freeing the monitor that is responsible
     for the file.
  */
  efsd_stat_remove(m->filename, FALSE);


  FREE(m->filename);
  FREE(m->fam_req);

  efsd_list_free(m->clients, (EfsdFunc)efsd_monitor_request_free);
  efsd_dca_free(m->files);

#if USE_THREADS
  pthread_mutex_destroy(&m->use_count_mutex);
#endif

  FREE(m);

  D_RETURN;
}


static void             
monitor_hash_item_free(EfsdHashItem *it)
{
  EfsdMonitor *m = NULL;
  EfsdMonitorKey *key = NULL;

  D_ENTER;

  if (!it)
    D_RETURN;

  /* Key is a EfsdMonitorKey, the filename inside the key is a string
     also contained inside the monitor, so we must not free it separately,
     because it also gets freed below.
  */

  key = (EfsdMonitorKey*)it->key;
  key->filename = NULL;
  monitor_key_free(key);  

  /* Set the key to NULL, to make sure no hash table lookup
     can possibly succeed on this monitor any more.
  */

  it->key = NULL;

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


EfsdMonitorRequest *
efsd_monitor_request_new(Ecore_Ipc_Client* client, EfsdFileCmd *cmd)
{
  EfsdMonitorRequest   *emr;
  
  D_ENTER;

  emr = NEW(EfsdMonitorRequest);
  memset(emr, 0, sizeof(EfsdMonitorRequest));

  printf("Seting client id to %p\n", client);
  emr->client      = client;
  emr->id          = cmd->id;

  /* Unhook (not free) the options  from the command, so
     that they don't get freed in the normal command
     cleanup. They will get cleaned up when we see the
     acknowledge event and the EfsdMonitorRequest is freed.
  */
     
  if (cmd->num_options > 0)
    {
      emr->num_options = cmd->num_options;
      emr->options     = cmd->options;
      cmd->num_options = 0;
      cmd->options = NULL;
    }

  D_RETURN_(emr);
}


void             
efsd_monitor_request_free(EfsdMonitorRequest *emr)
{
  int i;

  D_ENTER;

  for (i = 0; i < emr->num_options; i++)
    efsd_option_cleanup(&emr->options[i]);
  
  FREE(emr->options);
  FREE(emr);
  
  D_RETURN;
}


void
efsd_monitor_send_filechange_events(EfsdMonitor *m, EfsdMonitorRequest *emr)
{
  char list_all_files = FALSE, sort_files = FALSE;
  int  i;

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
  

  /* For external clients, send file-exists events. On directories,
     sort files first if so requested.
  */

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
	      printf("4\n");
	      efsd_monitor_send_filechange_event(m, emr, EFSD_FILE_EXISTS, filename);
	    }
	  
	  index++;
	}
      
      /* Send FILE_END_EXISTS so that the client knows that the end is reached. */
      printf("5\n");
      efsd_monitor_send_filechange_event(m, emr, EFSD_FILE_END_EXISTS, m->filename);
    }
  else
    {
      printf("6\n");
      efsd_monitor_send_filechange_event(m, emr, EFSD_FILE_EXISTS, m->filename);
      efsd_monitor_send_filechange_event(m, emr, EFSD_FILE_END_EXISTS, m->filename);
    }
}


static int
monitor_add_client(EfsdMonitor *m, EfsdCommand *com, Ecore_Ipc_Client* client)
{
  EfsdList           *l = NULL;
  EfsdMonitorRequest *emr;

  D_ENTER;

  if (!m)
    D_RETURN_(FALSE);

  for (l = efsd_list_head(m->clients); l; l = efsd_list_next(l))
    {
      if (((EfsdMonitorRequest*)efsd_list_data(l))->client == client)
	{
	  LOCK(&m->use_count_mutex);
	  if (client == EFSD_CLIENT_INTERNAL)
	    {
	      
	      m->internal_use_count++;
	      printf("No event raised..\n");
	      D("Incrementing internal use count for monitoring file %s, now (%i/%i).\n",
		m->filename, m->internal_use_count, m->client_use_count);
	    }
	  else
	    {
	      printf("event raised..\n");
	      printf("7\n");
	      efsd_monitor_send_filechange_events(m, (EfsdMonitorRequest*)efsd_list_data(l)); 
	      D("Client %i already monitors %s\n", client, m->filename);
	    }
	  UNLOCK(&m->use_count_mutex);

	  D_RETURN_(TRUE);
	}
    }
  
      
  LOCK(&m->use_count_mutex);

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
  
  emr = efsd_monitor_request_new(client, &com->efsd_file_cmd);  
  m->clients = efsd_list_prepend(m->clients, emr);
  
  UNLOCK(&m->use_count_mutex);

  if (client == EFSD_CLIENT_INTERNAL)
    D_RETURN_(TRUE);
 
  printf("8\n");
  efsd_monitor_send_filechange_events(m, emr);

  D_RETURN_(TRUE);
}


static EfsdList *
monitor_has_client(EfsdMonitor *m, int client)
{
  EfsdMonitorRequest    *emr;
  EfsdList              *l;

  D_ENTER;

  if (!m)
    D_RETURN_(FALSE);

  LOCK(&m->use_count_mutex);
      
  for (l = efsd_list_head(m->clients); l; l = efsd_list_next(l))
    {
      emr = (EfsdMonitorRequest*)efsd_list_data(l);
      
      if (emr->client == client)
	{
	  UNLOCK(&m->use_count_mutex);
	  D_RETURN_(l);
	}
    }
	  
  UNLOCK(&m->use_count_mutex);
  D_RETURN_(NULL);
}


static int
monitor_remove_client(EfsdCommand *com, int client, int dir_mode)
{
  EfsdMonitor  *m = NULL;
  char         *filename;

  D_ENTER;

  if (!com)
    {
      errno = EINVAL;
      D_RETURN_(-1);
    }

  filename = com->efsd_file_cmd.files[0];
  efsd_misc_remove_trailing_slashes(filename);

  if ((client == EFSD_CLIENT_INTERNAL)                      &&
      ((!strcmp(filename, efsd_filetype_get_system_file())) ||
       (!strcmp(filename, efsd_filetype_get_user_file()))))
    {
      /* It's an internal monitor, and it's monitoring
	 one of our filetype databases -- never stop
	 monitoring those ... */
      D_RETURN_(0);
    }

  m = efsd_monitored(filename, dir_mode);

  if (!m)
    {
      D("%s not monitored?\n", filename);
      errno = ENOENT;
      D_RETURN_(-1);
    }
  
  LOCK(&m->use_count_mutex);

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
      EfsdList              *l;
      
      /* Use count not zero -- remove given client
	 from list of monitoring clients. */
      for (l = efsd_list_head(m->clients); l; l = efsd_list_next(l))
	{
	  emr = (EfsdMonitorRequest*)efsd_list_data(l);

	  if (emr->client != client)
	    continue;

	  if (client == EFSD_CLIENT_INTERNAL && m->internal_use_count > 0)
	    break;
	  
	  D("Use count on %s is (%i/%i) -- removing client %i.\n",
	    m->filename, m->internal_use_count, m->client_use_count, client);
	  
	  if (!m->is_receiving_exist_events)
	    {
	      m->clients = efsd_list_remove(m->clients, l,
					    (EfsdFunc)efsd_monitor_request_free);
	    }
	  else
	    {
	      D("Client %i is currently receiving exist events. Marking as finished.\n", client);
	      emr->is_finished = TRUE;
	    }
	  
	  break;
	}
    }

  UNLOCK(&m->use_count_mutex);

  D_RETURN_(0);
}

void         
efsd_monitor_init(void)
{
  D_ENTER;

  if ((FAMOpen_r(&famcon)) < 0)
    {
      fprintf(stderr, "Can not connect to fam -- exiting.\n");
      exit(-1);
    }

  monitors = efsd_hash_new(1023, 10, (EfsdHashFunc)monitor_key_hash,
			   (EfsdCmpFunc)monitor_key_cmp, (EfsdFunc)monitor_hash_item_free);

  monitors_lock = efsd_lock_new();

  D_RETURN;
}


void         
efsd_monitor_cleanup(void)
{  
  D_ENTER;

  if (monitors)
    {
      efsd_hash_free(monitors);
      efsd_lock_free(monitors_lock);
      
      monitors = NULL;
      FAMClose_r(&famcon);
    }

  D_RETURN;
}


EfsdMonitor *
efsd_monitor_start(EfsdCommand *com, Ecore_Ipc_Client* client, int dir_mode, int do_sort)
{
  EfsdMonitor  *m = NULL;

  D_ENTER;

  if (!com)
    {
      errno = EINVAL;
      D_RETURN_(NULL);
    }

  efsd_misc_remove_trailing_slashes(com->efsd_file_cmd.files[0]);

  if ((m = efsd_monitored(com->efsd_file_cmd.files[0], dir_mode)) == NULL)
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
efsd_monitor_stop(EfsdCommand *com, int client, int dir_mode)
{
  D_ENTER;
  D_RETURN_(monitor_remove_client(com, client, dir_mode));
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
efsd_monitor_stop_internal(char *filename, int dir_mode)
{
  EfsdCommand       com;

  D_ENTER;

  if (!filename || filename[0] == '\0')
    D_RETURN_(-1);

  memset(&com, 0, sizeof(EfsdCommand));
  com.efsd_file_cmd.files = &filename;

  D_RETURN_(efsd_monitor_stop(&com, EFSD_CLIENT_INTERNAL, dir_mode));
}


EfsdMonitor *
efsd_monitored(char *filename, int dir_mode)
{
  char                   path[MAXPATHLEN];
  EfsdMonitor           *m = NULL;
  EfsdMonitorKey         key;

  D_ENTER;

  efsd_misc_remove_trailing_slashes(filename);

  monitor_key_init(&key);
  key.filename = filename;
  key.dir_mode = dir_mode;

  D("Looking up %s, as dir: %i\n", filename, dir_mode);

  efsd_lock_get_write_access(monitors_lock);
  m = efsd_hash_find(monitors, &key);
  efsd_lock_release_write_access(monitors_lock);

  if (m)
    {
      D("%s is monitored, dir requested: %i, monitored as dir: %i\n",
	filename, dir_mode, m->is_dir);
      D_RETURN_(m);
    }

  /* If it's not directly monitored, maybe it's treated
     indirectly by a monitor for the directory the file
     is in: */

  if (!efsd_misc_get_path_only(filename, path, MAXPATHLEN))
    {
      D("Couldn't get path only for %s\n", filename);
      D_RETURN_(NULL);
    }

  key.filename = path;
  key.dir_mode = TRUE;

  efsd_lock_get_write_access(monitors_lock);
  m = efsd_hash_find(monitors, &key);
  efsd_lock_release_write_access(monitors_lock);

  /* We found a monitor for a directory. Now it must be
     monitoring the directory and its contents, not just
     the directory file. */

  if (m && !dir_mode)
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
  EfsdMonitorKey key;

  D_ENTER;

  if (!m)
    D_RETURN;
  
  monitor_key_init(&key);
  key.filename = m->filename;
  key.dir_mode = m->is_dir;

  m->is_finished = TRUE;

  if (m->is_temporary)
    {
      monitor_free(m);
    }
  else
    {
      D("Freeing monitor for %s %i.\n", m->filename, m->is_dir);
      efsd_lock_get_write_access(monitors_lock);
      efsd_hash_remove(monitors, &key, NULL);
      efsd_lock_release_write_access(monitors_lock);
    }

  D_RETURN;
}


void
efsd_monitor_cleanup_client(int client)
{
  EfsdList         *l;
  EfsdHashIterator *it;
  EfsdMonitor      *m;

  D_ENTER;

  efsd_lock_get_read_access(monitors_lock);

  for (it = efsd_hash_it_new(monitors); efsd_hash_it_valid(it); efsd_hash_it_next(it))
    {
      m = (EfsdMonitor *) ((efsd_hash_it_item(it))->data);
      
      if (!m)
	continue;

      if ((l = monitor_has_client(m, client)) == NULL)
	continue;

      LOCK(&m->use_count_mutex);

      if (client == EFSD_CLIENT_INTERNAL)
	m->internal_use_count--;
      else
	m->client_use_count--;
      
      D("Client %i found monitoring %s, use count now (%i/%i)\n",
	client, m->filename, m->internal_use_count, m->client_use_count);

      D("Removing client %i from monitor for %s\n", client, m->filename);
      m->clients = efsd_list_remove(m->clients, l, (EfsdFunc)efsd_monitor_request_free);

      if (m->client_use_count == 0 && m->internal_use_count == 0)
	{
	  /* Use count dropped to zero -- stop monitoring. */
	  D("Stopping monitoring %s.\n", m->filename);
	  FAMCancelMonitor_r(&famcon, m->fam_req);
	}
      
      UNLOCK(&m->use_count_mutex);
    }

  efsd_lock_release_read_access(monitors_lock);
  efsd_hash_it_free(it);

  D_RETURN;
}


void
efsd_monitor_cleanup_requests(EfsdMonitor *m)
{
  EfsdMonitorRequest    *emr;
  EfsdList              *l;

  D_ENTER;

  if (!m)
    D_RETURN;

  for (l = efsd_list_head(m->clients); l; l = efsd_list_next(l))
    {
      emr = (EfsdMonitorRequest*)efsd_list_data(l);
      
      if (emr->is_finished)
	{
	  LOCK(&m->use_count_mutex);
	  m->clients = efsd_list_remove(m->clients, l,
					(EfsdFunc)efsd_monitor_request_free);
	  UNLOCK(&m->use_count_mutex);
	}
    }  

  D_RETURN;
}


int              
efsd_monitor_send_filechange_event(EfsdMonitor *m, EfsdMonitorRequest *emr,
				   EfsdFilechangeType type, char *filename)
{
  EfsdEvent   ee;
  int         result = TRUE;

  D_ENTER;

  printf("Running filechange on client %p\n", emr->client);

  memset(&ee, 0, sizeof(EfsdEvent));
  ee.type = EFSD_EVENT_FILECHANGE;
  ee.efsd_filechange_event.changetype = type;
  ee.efsd_filechange_event.file = filename;

  /* Register the command id in the reply event: */
  ee.efsd_filechange_event.id = emr->id;
			  
  /* D("Writing FAM event %i to client %i\n",
     famev.code, emr->client);
  */

  
  if (efsd_io_write_event(emr->client, &ee) < 0)
    {
      if (errno == EPIPE)
	{
	  D("Client %i died -- closing connection.\n", emr->client);
	  efsd_main_close_connection(emr->client);
	}
      else
	{
	  efsd_event_queue_add_event(ev_q, emr->client, &ee);
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

	  if (m->filename[strlen(m->filename) - 1] == '/')
	    snprintf(s, MAXPATHLEN, "%s%s", m->filename, filename); 
	  else
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

