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

GList *monitors = NULL;

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
  
  efr = (EfsdFamRequest*)malloc(sizeof(EfsdFamRequest));
  efr->client = client;
  efr->id     = id;

  return efr;
}


static void             
efsd_fam_free_request(EfsdFamRequest *efr)
{
  if (efr)
    free(efr);
}

void         
efsd_fam_init(void)
{
}


void         
efsd_fam_cleanup(void)
{  
  GList *l;
  
  for (l = g_list_first(monitors); l; l = g_list_next(l))
    {
      efsd_fam_free_monitor((EfsdFamMonitor*)l->data);
      l->data = NULL;
    }
  g_list_free(monitors);
}


int
efsd_fam_start_monitor(EfsdCommand *com, int client)
{
  if (!efsd_fam_is_monitored(com->efsd_file_cmd.file))
    {
      EfsdFamMonitor  *m;
      
      m = efsd_fam_new_monitor(com, client, FULL);
      monitors = g_list_append(monitors, m);

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

  return (0);
}


int         
efsd_fam_stop_monitor(EfsdCommand *com, int client)
{
  return efsd_fam_del_monitor(com, client);
}


int          
efsd_fam_is_monitored(char *filename)
{
  GList *l;

  l = g_list_first(monitors);
  
  while (l)
    {
      EfsdFamMonitor *m;

      m = (EfsdFamMonitor *)l->data;
      if (!strcmp(m->filename, filename))
	return (1);
    }

  return (0);
}


void
efsd_fam_force_startstop_monitor(EfsdCommand *com, int client)
{
  EfsdFamMonitor   *m;

  m = efsd_fam_new_monitor(com, client, SIMPLE);

  if (efsd_misc_file_is_dir(m->filename))
    FAMMonitorDirectory(&famcon, m->filename, m->fam_req, m);
  else
    FAMMonitorFile(&famcon, m->filename, m->fam_req, m);

  FAMCancelMonitor(&famcon, m->fam_req);
}


EfsdFamMonitor *         
efsd_fam_new_monitor(EfsdCommand *com, int client, EfsdFamMonType type)
{
  EfsdFamMonitor   *m;

  m = (EfsdFamMonitor*)malloc(sizeof(EfsdFamMonitor));

  m->filename  = strdup(com->efsd_file_cmd.file);
  m->fam_req   = (FAMRequest*)malloc(sizeof(FAMRequest));
  m->clients   = NULL;
  m->clients   = g_list_append(m->clients,
			      efsd_fam_new_request(client, com->efsd_file_cmd.id));
  m->use_count = 1;
  m->type      = type;

  return m;
}


void             
efsd_fam_free_monitor(EfsdFamMonitor *m)
{
  if (!m)
    return;

  if (m->filename)
    free (m->filename);
  if (m->fam_req)
    free (m->fam_req);

  g_list_free(m->clients);

  free (m);
}


static void
efsd_fam_add_monitor(EfsdCommand *com, int client)
{
  GList            *l;
  GList            *l2;
  EfsdFamMonitor   *m;
  char             *f;

  f = com->efsd_file_cmd.file;
  l = g_list_first(monitors);
  
  while (l)
    {
      m = (EfsdFamMonitor *)l->data;
      if (!strcmp(m->filename, f))
	{
	  D(("Incrementing usecount for monitoring file %s.\n", f));
	  m->use_count++;

	  for (l2 = g_list_first(m->clients); l2; l2 = g_list_next(l2))
	    {
	      if (((EfsdFamRequest*)l2->data)->client == client)
		break;
	    }

	  if (!l2)
	    {	      
	      m->clients =
		g_list_append(m->clients, efsd_fam_new_request(client, com->efsd_file_cmd.id));
	    }

	  return;
	}

      l = g_list_next(l);
    }
}


static int
efsd_fam_del_monitor(EfsdCommand *com, int client)
{
  FAMRequest      *fam_req = NULL;
  GList           *l;
  int              success;
  char            *f;
  EfsdFamMonitor  *m = NULL;

  l = g_list_first(monitors);
  f = com->efsd_file_cmd.file;
  success = 0;

  while (l)
    {
      m = (EfsdFamMonitor *)l->data;

      if (!strcmp(m->filename, f))
	{
	  D(("Decrementing usecount for monitoring file %s.\n", f));
	  if (--(m->use_count) == 0)
	    {
	      fam_req = m->fam_req;
	      monitors = g_list_remove_link(monitors, l);
	      l->data = NULL;
	      g_list_free_1(l);
	      l = NULL;
	    }
	  else
	    {
	      EfsdFamRequest    *efr;
	      GList             *l2;

	      /* Use count not zero -- remove given client
		 from list of monitoring clients. */
	      for (l2 = g_list_first(m->clients); l2; l2 = g_list_next(l2))
		{
		  efr = (EfsdFamRequest*)l2->data;

		  if (efr->client == client)
		    {
		      m->clients = g_list_remove_link(m->clients, l2);
		      efsd_fam_free_request(efr);
		      l2->data = NULL;		      
		      g_list_free_1(l2);
		      break;
		    }
		}
	    }
	  success = 1;
	  break;
	}
      
      l = g_list_next(l);
    }

  if (fam_req)
    {
      /* Use count dropped to zero -- stop monitoring. */
      D(("Use count is zero -- stopping monitoring of %s.\n", f));
      FAMCancelMonitor(&famcon, fam_req);
      efsd_fam_free_monitor(m);
    }

  if (success)
    return (0);

  return (-1);
}


int          
efsd_fam_cleanup_client(int client)
{
  GList      *l, *c;

  l = g_list_first(monitors);

  while (l)
    {
      EfsdFamMonitor *m;
      m = (EfsdFamMonitor *)l->data;

      c = g_list_first(m->clients);
      while (c)
	{
	  if (*((int*)c->data) == client)
	    break;

	  c = g_list_next(c);
	}

      if (c)
	{
	  if (--(m->use_count) == 0)
	    {
	      /* Use count dropped to zero -- stop monitoring. */
	      D(("Stopping monitoring %s.\n", m->filename));
	      FAMCancelMonitor(&famcon, m->fam_req);

	      monitors = g_list_remove_link(monitors, l);
	      g_list_free_1(l);
	      l = NULL;
	    }
	  else
	    {
	      /* Use count not zero, but remove client from list of users */
	      g_list_remove_link(m->clients, c);
	      if (!c->prev && !c->next)
		{
		  /* This cannot happen -- use count is not zero! */
		  fprintf(stderr, "FAM connection handling error -- ouch.\n");
		  exit(-1);
		}
	      g_list_free_1(c);
	    }
	}

      l = g_list_next(l);
    }

  return (0);
}


GList*
efsd_fam_get_clients_for_event(FAMEvent *fe)
{
  GList *l;

  if (!fe)
    return NULL;

  l = g_list_first(monitors);

  while (l)
    {
      EfsdFamMonitor *m;
      m = (EfsdFamMonitor *)l->data;

      if (fe->fr.reqnum == m->fam_req->reqnum)
	{
	  return m->clients;
	}

      l = g_list_next(l);
    }

  return NULL;
}

