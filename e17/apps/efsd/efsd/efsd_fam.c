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

#include <efsd.h>
#include <efsd_debug.h>
#include <efsd_globals.h>
#include <efsd_macros.h>
#include <efsd_misc.h>
#include <efsd_hash.h>
#include <efsd_fam_r.h>
#include <efsd_fam.h>


EfsdHash *monitors = NULL;

/* Allocator and deallocator for a Monitor */
static EfsdFamMonitor  *fam_new_monitor(EfsdFamMonType type, EfsdCommand *com,
					int client, int registered);
static void             fam_free_monitor(EfsdFamMonitor *m);


static EfsdFamRequest * fam_new_request(EfsdFamMonType type, int client, EfsdCmdId id,
					     int num_options, EfsdOption *options);

static void             fam_free_request(EfsdFamRequest *efr);

/* Increment use count for file monitor, or start
   new monitor if file is not monitored yet.
 */
static void             fam_add_client_to_monitor(EfsdFamMonType type, EfsdCommand *com, int client);

/* Decreases use count on file. If count drops to
   zero, monitoring is stopped.
*/
static int              fam_del_monitor(EfsdCommand *com, int client);

static void             fam_hash_item_free(EfsdHashItem *it);

static int              fam_is_monitored_internally(char *filename);

static EfsdFamMonitor *         
fam_new_monitor(EfsdFamMonType type, EfsdCommand *com, int client, int registered)
{
  EfsdFamRequest   *efr;
  EfsdFamMonitor   *m;

  D_ENTER;

  m = NEW(EfsdFamMonitor);
  memset(m, 0, sizeof(EfsdFamMonitor));

  m->filename   = strdup(com->efsd_file_cmd.file);
  m->fam_req    = NEW(FAMRequest);
  m->clients    = NULL;
  m->registered = registered;

  efr = fam_new_request(type, client, com->efsd_file_cmd.id,
			com->efsd_file_cmd.num_options,
			com->efsd_file_cmd.options);

  /* If we have options for a listdir events, unhook (not free)
     them from the command, so that they don't get freed in the
     normal command cleanup. They will get cleaned up when we
     see the acknowledge event and the EfsdFamRequest is freed. */
     
  if (com->efsd_file_cmd.num_options > 0)
    {
      com->efsd_file_cmd.num_options = 0;
      com->efsd_file_cmd.options = NULL;
    }
  
  m->clients   = efsd_list_prepend(m->clients, efr);				   
  m->use_count = 1;

  if (registered)
    efsd_hash_insert(monitors, m->filename, m);

  D_RETURN_(m);
}


static void             
fam_free_monitor(EfsdFamMonitor *m)
{
  D_ENTER;

  if (!m)
    D_RETURN;

  FREE(m->filename);
  FREE(m->fam_req);

  efsd_list_free(m->clients, (EfsdFunc)fam_free_request);
  FREE(m);

  D_RETURN;
}


static EfsdFamRequest *
fam_new_request(EfsdFamMonType type, int client, EfsdCmdId id,
		int num_options, EfsdOption *options)
{
  EfsdFamRequest   *efr;
  
  D_ENTER;

  efr = NEW(EfsdFamRequest);
  efr->type        = type;

  /* Okay -- this is tricky -- only the NORMAL monitor type
     actually causes file monitoring events to be sent to
     clients. So the question is what to fill in as client
    numbers for the others. We just use the monitor types,
     because they're all defined as negative ints.

     We can then check whether we're already monitoring a
     file using a specific type by simply comparing the
     client numbers in fam_add_client_to_monitor()...
  */

  if (type != EFSD_FAM_MONITOR_NORMAL)
    efr->client    = type;
  else
    efr->client    = client;

  efr->id          = id;
  efr->num_options = num_options;
  efr->options     = options;

  D_RETURN_(efr);
}


static void             
fam_free_request(EfsdFamRequest *efr)
{
  int i;

  D_ENTER;

  for (i = 0; i < efr->num_options; i++)
    efsd_option_cleanup(&efr->options[i]);

  FREE(efr->options);
  FREE(efr);

  D_RETURN;
}


static void             
fam_hash_item_free(EfsdHashItem *it)
{
  D_ENTER;

  if (!it)
    D_RETURN;

  /* Key is a string inside the monitor,
     so we don't need to free it separately.
  */

  /* Data is an EfsdFamMonitor: */
  fam_free_monitor(it->data);
  FREE(it);
  
  D_RETURN;
}


static void
fam_add_client_to_monitor(EfsdFamMonType type, EfsdCommand *com, int client)
{
  EfsdList         *l2 = NULL;
  EfsdFamMonitor   *m;

  D_ENTER;

  m = efsd_hash_find(monitors, com->efsd_file_cmd.file);
      
  if (m)
    {
      for (l2 = efsd_list_head(m->clients); l2; l2 = efsd_list_next(l2))
	{
	  if (((EfsdFamRequest*)efsd_list_data(l2))->client == client)
	    break;
	}

      if (!l2)
	{	      
	  EfsdFamRequest *efr;
	  
	  m->use_count++;

	  D(("Incrementing usecount for monitoring file %s, now %i.\n",
	     com->efsd_file_cmd.file, m->use_count));
	  
	  efr = fam_new_request(type, client, com->efsd_file_cmd.id,
				com->efsd_file_cmd.num_options,
				com->efsd_file_cmd.options);
	  
	  m->clients = efsd_list_prepend(m->clients, efr);
	  efsd_fam_force_startstop_monitor(com, client);
	}
    }

  D_RETURN;
}


static int
fam_del_monitor(EfsdCommand *com, int client)
{
  EfsdFamMonitor  *m = NULL;

  D_ENTER;

  if (!com)
    D_RETURN_(-1);

  efsd_misc_remove_trailing_slashes(com->efsd_file_cmd.file);
  m = efsd_hash_find(monitors, com->efsd_file_cmd.file);

  if (m)
    {
      D(("Decrementing usecount for monitoring file %s.\n",
	 com->efsd_file_cmd.file));

      if (--(m->use_count) == 0)
	{
	  D(("Use count is zero -- stopping monitoring of %s.\n",
	     com->efsd_file_cmd.file));
	  
	  FAMCancelMonitor_r(&famcon, m->fam_req);
	  
	  /* Actual cleanup happens when FAMAcknowledge is seen ... */
	}
      else
	{
	  EfsdFamRequest    *efr;
	  EfsdList          *l2;
	  
	  /* Use count not zero -- remove given client
	     from list of monitoring clients. */
	  for (l2 = efsd_list_head(m->clients); l2; l2 = efsd_list_next(l2))
	    {
	      efr = (EfsdFamRequest*)efsd_list_data(l2);
	      
	      if (efr->client == client)
		{
		  m->clients = efsd_list_remove(m->clients, l2, (EfsdFunc)fam_free_request);
		  l2 = NULL;
		  break;
		}
	    }
	}

      D_RETURN_(0);
    }
  
  D_RETURN_(-1);
}


static int
fam_is_monitored_internally(char *filename)
{
  EfsdFamMonitor   *m;
  EfsdList         *l;

  D_ENTER;
  
  m = efsd_hash_find(monitors, filename);
      
  if (m)
    {
      for (l = efsd_list_head(m->clients); l; l = efsd_list_next(l))
	{
	  if (((EfsdFamRequest*)efsd_list_data(l))->client == EFSD_FAM_MONITOR_INTERNAL)
	    D_RETURN_(TRUE);
	}
    }

  D_RETURN_(FALSE);

}


void         
efsd_fam_init(void)
{
  D_ENTER;

  if ((FAMOpen_r(&famcon)) < 0)
    {
      fprintf(stderr, "Can not connect to fam -- exiting.\n");
      exit(-1);
    }

  monitors = efsd_hash_new(1023, 10, (EfsdHashFunc)efsd_hash_string,
			   (EfsdCmpFunc)strcmp, fam_hash_item_free);

  D_RETURN;
}


void         
efsd_fam_cleanup(void)
{  
  D_ENTER;

  efsd_hash_free(monitors);
  monitors = NULL;
  FAMClose_r(&famcon);

  D_RETURN;
}


int
efsd_fam_start_monitor(EfsdFamMonType type, EfsdCommand *com, int client)
{
  D_ENTER;

  if (!com)
    D_RETURN_(-1);

  efsd_misc_remove_trailing_slashes(com->efsd_file_cmd.file);

  if (!efsd_fam_is_monitored(com->efsd_file_cmd.file))
    {
      EfsdFamMonitor  *m;
      
      m = fam_new_monitor(type, com, client, TRUE);

      if (type == EFSD_FAM_MONITOR_INTERNAL)
	{
	  /* Placing this in an extra case saves us the stat ... */
	  D(("Starting monitoring file %s.\n", m->filename));
	  if (FAMMonitorFile_r(&famcon, m->filename, m->fam_req, m) < 0)
	    {
	      D(("Starting monitoring %s FAILED.\n", m->filename));
	      efsd_fam_remove_monitor(m);	      
	    }
	}
      else if (efsd_misc_file_is_dir(m->filename))
	{
	  D(("Starting monitoring dir %s.\n", m->filename));
	  if (FAMMonitorDirectory_r(&famcon, m->filename, m->fam_req, m) < 0)
	    {
	      D(("Starting monitoring %s FAILED.\n", m->filename));
	      efsd_fam_remove_monitor(m);	      
	    }
	}
      else
	{
	  D(("Starting monitoring file %s.\n", m->filename));
	  if (FAMMonitorFile_r(&famcon, m->filename, m->fam_req, m) < 0)
	    {
	      D(("Starting monitoring %s FAILED.\n", m->filename));
	      efsd_fam_remove_monitor(m);	      
	    }
	}      
    }
  else
    {
      fam_add_client_to_monitor(type, com, client);
    }

  D_RETURN_(0);
}


int         
efsd_fam_stop_monitor(EfsdCommand *com, int client)
{
  D_ENTER;
  D_RETURN_(fam_del_monitor(com, client));
}


int              
efsd_fam_start_monitor_internal(char *filename)
{
  EfsdCommand       com;

  D_ENTER;

  if (!filename || filename[0] == '\0')
    D_RETURN_(-1);

  efsd_misc_remove_trailing_slashes(filename);

  if (fam_is_monitored_internally(filename))
    D_RETURN_(TRUE);

  memset(&com, 0, sizeof(EfsdCommand));
  com.efsd_file_cmd.file = filename;

  D_RETURN_(efsd_fam_start_monitor(EFSD_FAM_MONITOR_INTERNAL, &com, 0));
}


int              
efsd_fam_stop_monitor_internal(char *filename)
{
  EfsdCommand       com;

  D_ENTER;

  if (!filename || filename[0] == '\0')
    D_RETURN_(-1);

  efsd_misc_remove_trailing_slashes(filename);
  memset(&com, 0, sizeof(EfsdCommand));
  com.efsd_file_cmd.file = filename;

  D_RETURN_(efsd_fam_stop_monitor(&com, EFSD_FAM_MONITOR_INTERNAL));
}


int          
efsd_fam_is_monitored(char *filename)
{
  D_ENTER;

  efsd_misc_remove_trailing_slashes(filename);

  D_RETURN_((efsd_hash_find(monitors, filename) != NULL));
}


void
efsd_fam_force_startstop_monitor(EfsdCommand *com, int client)
{
  EfsdFamMonitor   *m;

  D_ENTER;

  m = fam_new_monitor(EFSD_FAM_MONITOR_NORMAL, com, client, FALSE);

  if (efsd_misc_file_is_dir(m->filename))
    {
      if (FAMMonitorDirectory_r(&famcon, m->filename, m->fam_req, m) < 0)
	{
	  D(("Starting monitoring %s FAILED.\n", m->filename));
	  efsd_fam_remove_monitor(m);	      
	}
    }
  else
    {
      if (FAMMonitorFile_r(&famcon, m->filename, m->fam_req, m) < 0)
	{
	  D(("Starting monitoring %s FAILED.\n", m->filename));
	  efsd_fam_remove_monitor(m);	      
	}
    }

  /* This causes at least one or more file-exists events
     to be sent -- now cancel the monitor to generate
     them FAMAcknowledge event.
  */

  FAMCancelMonitor_r(&famcon, m->fam_req);

  D_RETURN;
}


void             
efsd_fam_remove_monitor(EfsdFamMonitor *m)
{
  D_ENTER;

  if (!m)
    D_RETURN;

  if (m->registered)
    {
      D(("Freeing registered monitor for %s.\n", m->filename));
      efsd_hash_remove(monitors, m->filename);    
    }
  else
    {
      D(("Freeing monitor for %s.\n", m->filename));
      fam_free_monitor(m);
    }

  D_RETURN;
}


int          
efsd_fam_cleanup_client(int client)
{
  EfsdList         *c;
  EfsdHashIterator *it;
  EfsdFamMonitor   *m;

  D_ENTER;

  for (it = efsd_hash_it_new(monitors); efsd_hash_it_valid(it); efsd_hash_it_next(it))
    {
      m = (EfsdFamMonitor *) ((efsd_hash_it_item(it))->data);
      
      c = efsd_list_head(m->clients);
      while (c)
	{
	  if (((EfsdFamRequest*)efsd_list_data(c))->client == client)
	    break;

	  c = efsd_list_next(c);
	}

      if (c)
	{
	  --(m->use_count);
	  D(("Use count: %i\n", m->use_count));

	  if (m->use_count == 0)
	    {
	      /* Use count dropped to zero -- stop monitoring. */
	      D(("Stopping monitoring %s.\n", m->filename));
	      FAMCancelMonitor_r(&famcon, m->fam_req);
	    }
	  else if (m->use_count > 0)
	    {
	      if (!efsd_list_prev(c) && !efsd_list_next(c))
		{
		  /* This cannot happen -- use count is not zero! */
		  fprintf(stderr, "FAM connection handling error -- ouch.\n");
		  exit(-1);
		}
	      
	      /* Use count not zero, but remove client from list of users */
	      m->clients = efsd_list_remove(m->clients, c, (EfsdFunc)fam_free_request);
	    }
	}
    }

  efsd_hash_it_free(it);
  D_RETURN_(FALSE);
}

