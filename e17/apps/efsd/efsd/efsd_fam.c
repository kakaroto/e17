/*

Copyright (C) 2000, 2001 Christian Kreibich <kreibich@aciri.org>.

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
#include <efsd_fam.h>
#include <efsd_misc.h>



extern FAMConnection    famcon;
extern int              clientnums[];

EfsdList *monitors = NULL;

static EfsdFamRequest * efsd_fam_new_request(int client, EfsdCmdId id);
static void             efsd_fam_free_request(EfsdFamRequest *efr);

/* Increment use count for file monitor, or start
   new monitor if file is not monitored yet.
 */
static void             efsd_fam_add_monitor(EfsdCommand *com, int client);

/* Decreases use count on file. If count drops to
   zero, monitoring is stopped.
*/
static int              efsd_fam_del_monitor(EfsdCommand *com, int client);


static EfsdFamRequest *
efsd_fam_new_request(int client, EfsdCmdId id)
{
  EfsdFamRequest   *efr;
  
  D_ENTER;

  efr = NEW(EfsdFamRequest);
  efr->client = client;
  efr->id     = id;

  D_RETURN_(efr);
}


static void             
efsd_fam_free_request(EfsdFamRequest *efr)
{
  D_ENTER;
  FREE(efr);
  D_RETURN;
}

void         
efsd_fam_init(void)
{
  D_ENTER;
  D_RETURN;
}


void         
efsd_fam_cleanup(void)
{  
  D_ENTER;
  efsd_list_free(efsd_list_head(monitors), (EfsdFunc)efsd_fam_free_monitor);
  D_RETURN;
}


int
efsd_fam_start_monitor(EfsdCommand *com, int client)
{
  D_ENTER;

  if (!efsd_fam_is_monitored(com->efsd_file_cmd.file))
    {
      EfsdFamMonitor  *m;
      
      m = efsd_fam_new_monitor(com, client, FULL);
      monitors = efsd_list_prepend(monitors, m);

      if (efsd_misc_file_is_dir(m->filename))
	{
	  D(("Starting monitoring dir %s.\n", m->filename));
	  FAMMonitorDirectory(&famcon, m->filename, m->fam_req, m);
	}
      else
	{
	  D(("Starting monitoring file %s.\n", m->filename));
	  FAMMonitorFile(&famcon, m->filename, m->fam_req, m);
	}      
    }
  else
    {
      efsd_fam_add_monitor(com, client);
    }

  D_RETURN_(0);
}


int         
efsd_fam_stop_monitor(EfsdCommand *com, int client)
{
  D_ENTER;
  D_RETURN_(efsd_fam_del_monitor(com, client));
}


int          
efsd_fam_is_monitored(char *filename)
{
  EfsdList *l;

  D_ENTER;

  l = efsd_list_head(monitors);
  
  while (l)
    {
      EfsdFamMonitor *m;

      m = (EfsdFamMonitor *)efsd_list_data(l);
      if (!strcmp(m->filename, filename))
	D_RETURN_(1);
    }

  D_RETURN_(0);
}


void
efsd_fam_force_startstop_monitor(EfsdCommand *com, int client)
{
  EfsdFamMonitor   *m;

  D_ENTER;

  m = efsd_fam_new_monitor(com, client, SIMPLE);

  if (efsd_misc_file_is_dir(m->filename))
    FAMMonitorDirectory(&famcon, m->filename, m->fam_req, m);
  else
    FAMMonitorFile(&famcon, m->filename, m->fam_req, m);

  FAMCancelMonitor(&famcon, m->fam_req);

  D_RETURN;
}


EfsdFamMonitor *         
efsd_fam_new_monitor(EfsdCommand *com, int client, EfsdFamMonType type)
{
  EfsdFamMonitor   *m;

  D_ENTER;

  m = NEW(EfsdFamMonitor);

  m->filename  = strdup(com->efsd_file_cmd.file);
  m->fam_req   = NEW(FAMRequest);
  m->clients   = NULL;
  m->clients   = efsd_list_prepend(m->clients,
				   efsd_fam_new_request(client, com->efsd_file_cmd.id));
  m->use_count = 1;
  m->type      = type;

  D_RETURN_(m);
}


void             
efsd_fam_free_monitor(EfsdFamMonitor *m)
{
  D_ENTER;

  if (!m)
    D_RETURN;

  if (m->filename)
    free (m->filename);
  if (m->fam_req)
    free (m->fam_req);

  efsd_list_free(m->clients, (EfsdFunc)efsd_fam_free_request);
  free (m);

  D_RETURN;
}


static void
efsd_fam_add_monitor(EfsdCommand *com, int client)
{
  EfsdList         *l;
  EfsdList         *l2 = NULL;
  EfsdFamMonitor   *m;
  char             *f;

  D_ENTER;

  f = com->efsd_file_cmd.file;
  l = efsd_list_head(monitors);
  
  while (l)
    {
      m = (EfsdFamMonitor *)efsd_list_data(l);
      if (!strcmp(m->filename, f))
	{
	  D(("Incrementing usecount for monitoring file %s.\n", f));
	  m->use_count++;

	  for (l2 = efsd_list_head(m->clients); l2; l2 = efsd_list_next(l2))
	    {
	      if (((EfsdFamRequest*)efsd_list_data(l2))->client == client)
		break;
	    }

	  if (!l2)
	    {	      
	      m->clients =
		efsd_list_prepend(m->clients,
				  efsd_fam_new_request(client, com->efsd_file_cmd.id));
	      efsd_fam_force_startstop_monitor(com, client);
	    }

	  D_RETURN;
	}

      l = efsd_list_next(l);
    }
  D_RETURN;
}


static int
efsd_fam_del_monitor(EfsdCommand *com, int client)
{
  EfsdList        *l;
  int              success;
  char            *f;
  EfsdFamMonitor  *m = NULL;

  D_ENTER;

  l = efsd_list_head(monitors);
  f = com->efsd_file_cmd.file;
  success = 0;

  while (l)
    {
      m = (EfsdFamMonitor *)efsd_list_data(l);

      if (!strcmp(m->filename, f))
	{
	  D(("Decrementing usecount for monitoring file %s.\n", f));
	  if (--(m->use_count) == 0)
	    {
	      D(("Use count is zero -- stopping monitoring of %s.\n", f));
	      FAMCancelMonitor(&famcon, m->fam_req);
	      monitors = efsd_list_remove(monitors, l, (EfsdFunc)efsd_fam_free_monitor);
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
		      m->clients = efsd_list_remove(m->clients, l2, (EfsdFunc)efsd_fam_free_request);
		      l2 = NULL;
		      break;
		    }
		}
	    }
	  success = 1;
	  break;
	}
      
      l = efsd_list_next(l);
    }

  if (success)
    D_RETURN_(0);

  D_RETURN_(-1);
}


int          
efsd_fam_cleanup_client(int client)
{
  EfsdList      *l, *c;

  D_ENTER;

  l = efsd_list_head(monitors);

  while (l)
    {
      EfsdFamMonitor *m;
      m = (EfsdFamMonitor *)efsd_list_data(l);

      c = efsd_list_head(m->clients);
      while (c)
	{
	  if (((EfsdFamRequest*)efsd_list_data(c))->client == client)
	    break;

	  c = efsd_list_next(c);
	}

      if (c)
	{
	  if (--(m->use_count) == 0)
	    {
	      /* Use count dropped to zero -- stop monitoring. */
	      D(("Stopping monitoring %s.\n", m->filename));
	      FAMCancelMonitor(&famcon, m->fam_req);

	      monitors = efsd_list_remove(monitors, l, (EfsdFunc)efsd_fam_free_monitor);
	      l = NULL;
	    }
	  else
	    {
	      if (!efsd_list_prev(c) && !efsd_list_next(c))
		{
		  /* This cannot happen -- use count is not zero! */
		  fprintf(stderr, "FAM connection handling error -- ouch.\n");
		  exit(-1);
		}
	      
	      /* Use count not zero, but remove client from list of users */
	      m->clients = efsd_list_remove(m->clients, c, (EfsdFunc)efsd_fam_free_request);
	    }
	}

      l = efsd_list_next(l);
    }
  D_RETURN_(0);
}

