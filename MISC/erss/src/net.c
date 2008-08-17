#include "erss.h"
#include "parse.h"             /* erss_parse() */
#include "parse_config.h"      /* rc */
#include "gui.h"               /* erss_set_time(), erss_gui_add_items() */

int erss_net_poll (void *data)
{
	Erss_Feed *f=(Erss_Feed *)data;

	if (f->waiting_for_reply) {
		fprintf (stderr, "%s warning: client has not received all information ", 
				PACKAGE);
		fprintf (stderr, "from the last connection attempt yet! \n");
		return TRUE;
	}
	
	f->server=NULL;
	
	if (f->last_time)
		free (f->last_time);
	else
		printf ("%s info: connecting to %s...\n", PACKAGE, f->cfg->hostname?f->cfg->hostname:"host");

	if (rc->proxy) {
		if (!strcasecmp (rc->proxy, ""))
			f->server = ecore_con_server_connect (ECORE_CON_REMOTE_SYSTEM,
							      f->cfg->hostname, 80, NULL);
		else {
			if (!rc->proxy_port)
				fprintf (stderr, "%s error: You need to define a proxy port!\n", PACKAGE);
			else
				f->server = ecore_con_server_connect (ECORE_CON_REMOTE_SYSTEM,
								      rc->proxy, rc->proxy_port, NULL);
		}
	} else
		f->server = ecore_con_server_connect (ECORE_CON_REMOTE_SYSTEM,
						      f->cfg->hostname, 80, NULL);

	if (!f->server)
		fprintf (stderr, "%s error: Could not connect to server ..\n", PACKAGE);
	else {
		f->total_connects++;
		f->last_time = erss_time_format ();
		erss_set_time (f);
	}

	return TRUE;
}

static int erss_net_server_add (void *data, int type, void *event)
{
	Erss_Feed *f=(Erss_Feed *)data;
	char       c[1024];

	/*
	 * We want to be connected before sending the request.
	 */
	if (f->total_connects == 1)
		printf ("%s info: requesting \"%s\"...\n", PACKAGE, f->cfg->url?f->cfg->url:"feed");
	
	snprintf (c, sizeof (c), "GET %s HTTP/1.0\r\n", f->cfg->url);
	ecore_con_server_send (f->server, c, strlen (c));
	snprintf (c, sizeof (c), "Host: %s\r\n", f->cfg->hostname);
	ecore_con_server_send (f->server, c, strlen (c));
	snprintf (c, sizeof (c), "User-Agent: %s/%s\r\n\r\n",
		  PACKAGE, VERSION);
	ecore_con_server_send (f->server, c, strlen (c));

	f->waiting_for_reply = TRUE;

	return 1;
}

static int erss_net_server_data (void *data, int type, void *event)
{
	Erss_Feed                   *f = (Erss_Feed *)data;
	Ecore_Con_Event_Server_Data *e = event;

	if (f->total_connects == 1)
		printf ("%s info: reading \"%s\" (%ld octets)...\n", PACKAGE, f->cfg->header?f->cfg->header:"feed", (long)e->size);

	/* 
	 * Read everything we receive into one big buffer, and handle
	 * that buffer when the server disconnects.
	 */
	f->main_buffer = realloc (f->main_buffer, f->main_bufsize + e->size);
	memcpy (f->main_buffer + f->main_bufsize, e->data, e->size);
	f->main_bufsize += e->size;

	return 1;
}

static int erss_net_server_del (void *data, int type, void *event)
{
	Erss_Feed                  *f = (Erss_Feed *)data;
	Ecore_Con_Event_Server_Del *e = event;
	char                       *buf = f->main_buffer;
	char                       *temp = NULL;

	if (f->total_connects == 1)
		printf ("%s info: disconnecting from %s...\n", PACKAGE, f->cfg->hostname?f->cfg->hostname:"host");

	/*
	 * Now split our buffer in each newline and then parse the line.
	 */

	/*
	 * Clean out the evas objects from the container to
	 * make room for the new items.
	 */
	erss_gui_items_drop(&f->list);

	f->item = NULL;
	f->list = ecore_list_new ();

	temp = strstr (buf, "<?xml");
	f->doc = xmlParseMemory (temp, f->main_bufsize - (temp - f->main_buffer));

	erss_parse (f);
	erss_gui_items_add (f);

	ecore_con_server_del (e->server);
	f->server = NULL;
	
	if (ecore_list_empty_is (f->list)) {
		if (buf && temp)
			printf ("%s\n", temp);
		else 
			printf ("%s error: could not connect to '%s'\n", PACKAGE, f->cfg->url);

		fprintf (stderr, "\n%s error: parsing data\n", PACKAGE);
		fprintf (stderr, "%s error: are you sure you have to correct input in your config file?\n", PACKAGE);
	}

	if (f->main_buffer) {
		free (f->main_buffer);
		f->main_buffer = NULL;
	}

	f->main_bufsize = 0;
	f->waiting_for_reply = FALSE;

	if (f->total_connects == 1)
		printf ("%s info: connection information only displays on the first connect.\n", 
				PACKAGE);

	return 1;
}

void erss_net_connect(Erss_Feed *f) {
	ecore_event_handler_add (ECORE_CON_EVENT_SERVER_ADD,
							 erss_net_server_add, f);
	ecore_event_handler_add (ECORE_CON_EVENT_SERVER_DEL,
							 erss_net_server_del, f);
	ecore_event_handler_add (ECORE_CON_EVENT_SERVER_DATA,
							 erss_net_server_data, f);
}
