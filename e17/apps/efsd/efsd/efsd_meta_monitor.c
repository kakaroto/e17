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

#include <efsd_debug.h>
#include <efsd_macros.h>
#include <efsd_hash.h>
#include <efsd_event_queue.h>
#include <efsd_io.h>
#include <efsd_lock.h>
#include <efsd_monitor.h>
#include <efsd_meta.h>
#include <efsd_meta_monitor.h>
#include <efsd_main.h>

/* A Hastable that registers the metadata entries that
   are monitored by clients. The hash stores key/value
   pairs, keys are char*s, values are EfsdMetaMonitors */
EfsdHash              *meta_monitors;

/* A lock to manage concurrent access to the hashtable */
EfsdLock              *meta_monitors_lock;

struct efsd_meta_monitor
{
  char         *filename;
  char         *key;

  EfsdList     *clients;   /* list<EfsdMonitorRequest*> */
  EfsdLock     *clients_lock;
};

static EfsdMetaMonitor *meta_monitor_new(char *filename, char *key);
static void             meta_monitor_free(EfsdMetaMonitor *m);
static void             meta_monitor_add_client(EfsdMetaMonitor *m, EfsdFileCmd *cmd, Ecore_Ipc_Client* client);
static void             meta_monitor_del_client(EfsdMetaMonitor *m, Ecore_Ipc_Client* client, int do_lock);
static EfsdMetaMonitor *meta_monitor_find(char *file, char *key);
static void             meta_monitor_get_key(char *file, char *key,
					     char *result, int result_len);
static void             meta_monitor_hash_item_free(EfsdHashItem *it);
static void             meta_monitor_send_change_event(EfsdMetaMonitor *monitor, EfsdDatatype type,
						       int data_len, void *data);

static EfsdMetaMonitor       *
meta_monitor_new(char *filename, char *key)
{
  EfsdMetaMonitor *monitor = NULL;
  char             full_key[MAXPATHLEN];

  D_ENTER;

  if (!filename || !*filename || !key || !*key)
    D_RETURN_(NULL);

  monitor = NEW(EfsdMetaMonitor);

  if (monitor == NULL)
    D_RETURN_(NULL);

  memset(monitor, 0, sizeof(EfsdMetaMonitor));

  monitor->filename = strdup(filename);
  monitor->key = strdup(key);
  monitor->clients_lock = efsd_lock_new();

  meta_monitor_get_key(filename, key, full_key, MAXPATHLEN);
  efsd_lock_get_write_access(meta_monitors_lock);
  efsd_hash_insert(meta_monitors, strdup(full_key), monitor);
  efsd_lock_release_write_access(meta_monitors_lock);

  D("New metadata monitor created.\n");

  D_RETURN_(monitor);
}


static void                   
meta_monitor_free(EfsdMetaMonitor *monitor)
{
  D_ENTER;

  if (!monitor)
    D_RETURN;

  D("Freeing metadata monitor for %s:%s.\n", monitor->filename, monitor->key);

  FREE(monitor->filename);
  FREE(monitor->key);

  efsd_lock_get_write_access(monitor->clients_lock);
  efsd_list_free(monitor->clients, (EfsdFunc)efsd_monitor_request_free);
  efsd_lock_release_write_access(monitor->clients_lock);

  efsd_lock_free(monitor->clients_lock);

  FREE(monitor);

  D_RETURN;
}


static void                   
meta_monitor_add_client(EfsdMetaMonitor *monitor, EfsdFileCmd *cmd, Ecore_Ipc_Client* client)
{
  EfsdList *l = NULL;
  EfsdMonitorRequest *emr = NULL;

  D_ENTER;

  if (!monitor)
    D_RETURN;

  efsd_lock_get_read_access(monitor->clients_lock);

  D("Adding client %i to metadata monitor for %s:%s\n", client, monitor->filename, monitor->key);

  for (l = efsd_list_head(monitor->clients); l; l = efsd_list_next(l))
    {
      emr = (EfsdMonitorRequest*) efsd_list_data(l);

      /* If client monitors this metadata already,
	 don't do anything, just return. */
      if (emr->client == client)
	{
	  D("Metadata monitor already monitors client %i\n", client);
	  efsd_lock_release_read_access(monitor->clients_lock);
	  D_RETURN;
	}
    }

  efsd_lock_release_read_access(monitor->clients_lock);

  /* If we get here, this client doesn't yet monitor this metadata. */

  emr = efsd_monitor_request_new(client, cmd);

  efsd_lock_get_write_access(monitor->clients_lock);
  monitor->clients = efsd_list_prepend(monitor->clients, emr);
  efsd_lock_release_write_access(monitor->clients_lock);

  D_RETURN;
}


static void                   
meta_monitor_del_client(EfsdMetaMonitor *monitor, Ecore_Ipc_Client* client, int do_lock)
{
  EfsdList *l = NULL;
  EfsdMonitorRequest *emr = NULL;

  D_ENTER;

  if (!monitor)
    D_RETURN;

  efsd_lock_get_write_access(monitor->clients_lock);

  /* Walk through list of monitoring clients and remove the
     one requested, if possible. */
  for (l = efsd_list_head(monitor->clients); l; l = efsd_list_next(l))
    {
      emr = (EfsdMonitorRequest*) efsd_list_data(l);

      if (emr->client == client)
	{
	  D("Removing client %i from metadata monitor %s:%s\n", client, monitor->filename, monitor->key);
	  monitor->clients = efsd_list_remove(monitor->clients, l,
					(EfsdFunc)efsd_monitor_request_free);

	  break;
	}
    }

  efsd_lock_release_write_access(monitor->clients_lock);

  /* When there are no clients left, the monitor is
     no longer needed, so get rid of it. */
  if (monitor->clients == NULL)
    {
      char full_key[MAXPATHLEN];

      D("Metadata monitor %s:%s no longer needed.\n", monitor->filename, monitor->key);
      meta_monitor_get_key(monitor->filename, monitor->key, full_key, MAXPATHLEN);

      if (do_lock)
	efsd_lock_get_write_access(meta_monitors_lock);

      efsd_hash_remove(meta_monitors, full_key, NULL);

      if (do_lock)
	efsd_lock_release_write_access(meta_monitors_lock);
    }


  D_RETURN;
}


static EfsdMetaMonitor       *
meta_monitor_find(char *file, char *key)
{
  EfsdMetaMonitor *monitor = NULL;
  char full_key[MAXPATHLEN];

  D_ENTER;

  meta_monitor_get_key(file, key, full_key, MAXPATHLEN);

  /* Look up monitor key in hashtable, returns NULL
     if monitor isn't found. */
  efsd_lock_get_read_access(meta_monitors_lock);
  monitor = efsd_hash_find(meta_monitors, &full_key);
  efsd_lock_release_read_access(meta_monitors_lock);

  D("Metadata monitor for %s: %p\n", full_key, monitor);

  D_RETURN_(monitor);
}

static void                   
meta_monitor_get_key(char *file, char *key, char *result, int result_len)
{
  D_ENTER;

  if (!file || !*file || !key || !*key)
    {
      result[0] = '\0';
      D_RETURN;
    }

  snprintf(result, result_len, "%s:%s", file, key);

  D_RETURN;
}


static void             
meta_monitor_hash_item_free(EfsdHashItem *it)
{
  EfsdMetaMonitor *monitor = NULL;
  char            *key = NULL;

  D_ENTER;

  if (!it)
    D_RETURN;

  key = (char*) it->key;
  monitor = (EfsdMetaMonitor*) it->data;

  /* Free the key and value for this hashtable
     item. The key is just a string, the value
     is an EfsdMetaMonitor. */

  FREE(key);
  meta_monitor_free(monitor);
  FREE(it);

  D_RETURN;
}


static void             
meta_monitor_send_change_event(EfsdMetaMonitor *monitor, EfsdDatatype type,
			       int data_len, void *data)
{
  EfsdList   *l = NULL;
  EfsdMonitorRequest *emr = NULL;
  EfsdEvent   ee;

  D_ENTER;

  if (!monitor || !data)
    D_RETURN;

  /* Initialize the reply event -- we'll just fill the
     efsd_metachange_event part. */
  memset(&ee, 0, sizeof(EfsdEvent));

  /* Fill in everything that remains unchanged for all
     the clients ... */
  ee.type = EFSD_EVENT_METADATA_CHANGE;
  ee.efsd_metachange_event.key = monitor->key;
  ee.efsd_metachange_event.file = monitor->filename;
  ee.efsd_metachange_event.datatype = type;
  ee.efsd_metachange_event.data_len = data_len;
  ee.efsd_metachange_event.data = data;

  efsd_lock_get_read_access(monitor->clients_lock);

  /* Walk through list of monitoring clients and fill
     event with remaining, client-specific data.
     Then send off the event to the client.
  */
  for (l = efsd_list_head(monitor->clients); l; l = efsd_list_next(l))
    {
      emr = (EfsdMonitorRequest*) efsd_list_data(l);

      ee.efsd_metachange_event.id = emr->id;      

      D("Sending metadata change event for %s:%s to %i\n", monitor->filename, monitor->key, emr->client);

      if (efsd_io_write_event(efsd_main_get_fd(emr->client), &ee) < 0)
	{
	  if (errno == EPIPE)
	    {
	      D("Client %i died -- closing connection.\n", emr->client);
	      efsd_main_close_connection(emr->client);
	    }
	  else
	    {
	      efsd_event_queue_add_event(ev_q, emr->client, &ee);
	      D("write() error when writing metadata change event.\n");
	    }
	}
    }

  efsd_lock_release_read_access(monitor->clients_lock);
  
  D_RETURN;
}





void 
efsd_meta_monitor_init(void)
{
  D_ENTER;

  meta_monitors = efsd_hash_new(1023, 10, (EfsdHashFunc)efsd_hash_string,
				(EfsdCmpFunc)strcmp, (EfsdFunc)meta_monitor_hash_item_free);

  meta_monitors_lock = efsd_lock_new();
  
  D_RETURN;
}


int
efsd_meta_monitor_add(EfsdCommand *cmd, Ecore_Ipc_Client* client)
{
  EfsdFileCmd *fcmd = NULL;
  EfsdMetaMonitor *monitor = NULL;
  char             key_base[MAXPATHLEN];
  int              key_base_len;

  D_ENTER;

  if (!cmd)
    D_RETURN_(FALSE);

  if (cmd->efsd_file_cmd.num_files != 2)
    D_RETURN_(FALSE);

  fcmd = &cmd->efsd_file_cmd;

  key_base[0] = '\0';
  efsd_meta_get_file_info(fcmd->files[0], NULL, 0, key_base, MAXPATHLEN, FALSE);
  key_base_len = strlen(key_base);
  snprintf(key_base + key_base_len, MAXPATHLEN - key_base_len, "%s", fcmd->files[1]);

  FREE(fcmd->files[1]);
  fcmd->files[1] = strdup(key_base);

  /* Try to find a monitor for the file/metadata combination,
     if nothing is found, create a new monitor. */
  if (! (monitor = meta_monitor_find(fcmd->files[0], fcmd->files[1])))
    {
      monitor = meta_monitor_new(fcmd->files[0], fcmd->files[1]);
      if (!monitor)
	D_RETURN_(FALSE);
    }

  /* No matter where the monitor came from, add the requested
     client to the list of clients monitoring that metadata. */
  meta_monitor_add_client(monitor, fcmd, client);

  D_RETURN_(TRUE);
}


int
efsd_meta_monitor_del(EfsdCommand *cmd, Ecore_Ipc_Client* client)
{
  EfsdFileCmd *fcmd = NULL;
  EfsdMetaMonitor *monitor = NULL;
  char             key_base[MAXPATHLEN];
  int              key_base_len;

  D_ENTER;

  if (!cmd)
    D_RETURN_(FALSE);

  fcmd = &cmd->efsd_file_cmd;

  key_base[0] = '\0';
  efsd_meta_get_file_info(fcmd->files[0], NULL, 0, key_base, MAXPATHLEN, FALSE);
  key_base_len = strlen(key_base);
  snprintf(key_base + key_base_len, MAXPATHLEN - key_base_len, "%s", fcmd->files[1]);

  FREE(fcmd->files[1]);
  fcmd->files[1] = strdup(key_base);

  /* Try to find monitor that monitors the requested metadata.
     If it can't be found, just return. */
  monitor = meta_monitor_find(fcmd->files[0], fcmd->files[1]);

  if (!monitor)
    D_RETURN_(FALSE);

  /* We've found a monitor. Remove the requested client from
     its list of monitoring clients. */
  meta_monitor_del_client(monitor, client, TRUE);

  D_RETURN_(TRUE);
}


void
efsd_meta_monitor_notify(char *file, char *key, EfsdDatatype type,
			 int data_len, void *data)
{
  EfsdMetaMonitor *monitor = NULL;
  char             key_base[MAXPATHLEN];
  int              key_base_len;

  D_ENTER;

  if (!file || !*file || !key || !*key)
    D_RETURN;

  key_base[0] = '\0';
  efsd_meta_get_file_info(file, NULL, 0, key_base, MAXPATHLEN, FALSE);
  key_base_len = strlen(key_base);
  snprintf(key_base + key_base_len, MAXPATHLEN - key_base_len, "%s", key);

  if (! (monitor = meta_monitor_find(file, key_base)))
    D_RETURN;

  /* Tell the clients that the metadata changed ... */
  meta_monitor_send_change_event(monitor, type, data_len, data);
  
  D_RETURN;
}


void
efsd_meta_monitor_cleanup_client(Ecore_Ipc_Client* client)
{
  EfsdHashIterator *it;
  EfsdMetaMonitor  *monitor;

  D_ENTER;

  efsd_lock_get_write_access(meta_monitors_lock);

  for (it = efsd_hash_it_new(meta_monitors); efsd_hash_it_valid(it); efsd_hash_it_next(it))
    {
      monitor = (EfsdMetaMonitor *) ((efsd_hash_it_item(it))->data);
      
      meta_monitor_del_client(monitor, client, FALSE);
    }

  efsd_lock_release_write_access(meta_monitors_lock);
  efsd_hash_it_free(it);

  D_RETURN;
}

