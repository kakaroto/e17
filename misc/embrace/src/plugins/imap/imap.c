/*
 * $Id$
 * vim:noexpandtab:ts=4:sw=4:sts=4
 *
 * Copyright (C) 2004 Embrace project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Ecore.h>
#include <Ecore_Con.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <embrace_plugin.h>

typedef enum {
	STATE_DISCONNECTED,
	STATE_CONNECTED,
	STATE_SERVER_READY,
	STATE_LOGGED_IN,
	STATE_STATUS_OK
} State;

typedef struct {
	char *host;
	int port;
	bool use_ssl;
	char *user;
	char *pass;

	Evas_List *clients;
	Evas_List *current;

	Ecore_Timer *timer;
	double interval;

	Ecore_Con_Server *server;
	State state;
	int command_no;
} ImapServer;

#define MIN_INTERVAL 60
#define LOGOUT() \
	if (server->state >= STATE_LOGGED_IN) { \
		len = snprintf (outbuf, sizeof (outbuf), \
		                "A%03i LOGOUT", ++server->command_no); \
		ecore_con_server_send (server->server, outbuf, len); \
	} \
\
	ecore_con_server_del (server->server); \
	server->server = NULL;

static EmbracePlugin *plugin = NULL;
static Evas_List *servers = NULL;
static Ecore_Event_Handler *ev_hdl[3];

static ImapServer *find_server (Ecore_Con_Server *server)
{
	Evas_List *l;

	for (l = servers; l; l = l->next) {
		ImapServer *current = l->data;

		if (current->server == server)
			return current;
	}

	return NULL;
}

static ImapServer *find_server_by_mailbox (MailBox *mb)
{
	Evas_List *l;
	char *host;
	int port;
	int use_ssl;
	char *user;
	char *pass;

	assert (mb);

	host = mailbox_property_get (mb, "host");
	port = (int) mailbox_property_get (mb, "port");
	use_ssl = (int) mailbox_property_get (mb, "ssl");
	user = mailbox_property_get (mb, "user");
	pass = mailbox_property_get (mb, "pass");

	for (l = servers; l; l = l->next) {
		ImapServer *current = l->data;

		if (port == current->port && use_ssl == current->use_ssl &&
		    !strcmp (host, current->host) &&
		    !strcmp (user, current->user) &&
		    !strcmp (pass, current->pass))
			return current;
	}

	return NULL;
}

static int on_server_add (void *udata, int type, void *event)
{
	Ecore_Con_Event_Server_Add *ev = event;
	ImapServer *server;

	/* Make sure this event is for us */
	server = find_server (ev->server);
	if (!server)
		return 1;

	server->state = STATE_CONNECTED;
	server->command_no = 0;

	return 0;
}

static int on_server_data (void *udata, int type, void *event)
{
	Ecore_Con_Event_Server_Data *ev = event;
	ImapServer *server;
	MailBox *mb;
	char inbuf[1024], outbuf[256], *spc;
	int total = 0, unseen = 0, len;
	size_t slen;

	/* Make sure this event is for us */
	server = find_server (ev->server);
	if (!server)
		return 1;

	/* take the data and make a NUL-terminated string out of it */
	len = sizeof (inbuf) - 1;
	len = MIN (len, ev->size);

	memcpy (inbuf, ev->data, len);
	inbuf[len] = 0;
	embrace_strstrip (inbuf);

	assert (server->state != STATE_DISCONNECTED);

	if ((spc = strchr (inbuf, ' '))) {
		slen = strlen (spc);

		if (slen > 5 && !strncmp (spc + 1, "NO ", 3)) {
			LOGOUT ();
			fprintf (stderr, "[imap] failure: %s\n", spc + 4);
			return 0;
		} else if (slen > 6 && !strncmp (spc + 1, "BAD ", 4)) {
			LOGOUT ();
			fprintf (stderr, "[imap] bad command: %s\n", spc + 5);
			return 0;
		}
	}

	mb = server->current->data;
	server->state++;

	switch (server->state) {
		case STATE_SERVER_READY:
			len = snprintf (outbuf, sizeof (outbuf),
			                "A%03i LOGIN %s %s\r\n",
			                ++server->command_no,
			                server->user, server->pass);
			ecore_con_server_send (ev->server, outbuf, len);
			break;
		case STATE_STATUS_OK:
			if (sscanf (inbuf, "* STATUS %*s (MESSAGES %i UNSEEN %i)",
			            &total, &unseen) == 2) {
				mailbox_unseen_set (mb, unseen);
				mailbox_total_set (mb, total);

				server->current = server->current->next;

				if (server->current) {
					mb = server->current->data;
					server->state = STATE_LOGGED_IN;
				} else {
					mb = NULL;
					LOGOUT ();
				}
			} else {
				assert (false);
			}

			/* Fall through if we got another mailbox to check */
			if (!mb)
				break;
		case STATE_LOGGED_IN:
			len = snprintf (outbuf, sizeof (outbuf),
			                "A%03i STATUS %s (MESSAGES UNSEEN)\r\n",
			                ++server->command_no,
			                (char *) mailbox_property_get (mb, "path"));
			ecore_con_server_send (ev->server, outbuf, len);
			break;
		default:
			assert (false);
	}

	return 0;
}

static int on_server_del (void *udata, int type, void *event)
{
	Ecore_Con_Event_Server_Del *ev = event;
	ImapServer *server;

	/* Make sure this event is for us */
	server = find_server (ev->server);
	if (!server)
		return 1;

	if (server->state == STATE_DISCONNECTED)
		fprintf (stderr, "[imap] cannot connect to '%s'\n", server->host);
	else {
		server->state = STATE_DISCONNECTED;
		fprintf (stderr, "[imap] lost connection to '%s'\n", server->host);
	}

	ecore_con_server_del (server->server);
	server->server = NULL;

	return 0;
}

static bool imap_server_check (ImapServer *server)
{
	Ecore_Con_Type type = ECORE_CON_REMOTE_SYSTEM;

	assert (server);

	if (server->server) {
		fprintf (stderr, "[imap] already connected to '%s'\n", server->host);
		return false;
	}

	if (ecore_con_ssl_available_get () &&
	    server->use_ssl)
		type |= ECORE_CON_USE_SSL;

	server->server = ecore_con_server_connect (type, server->host,
	                                           server->port, NULL);
	server->state = STATE_DISCONNECTED;
	server->current = server->clients;

	return true;
}

static bool imap_check (MailBox *mb)
{
	ImapServer *server;
	bool ret = true;

	assert (mb);

	server = (ImapServer *) mailbox_property_get (mb, "server");
	if (server)
		ret = imap_server_check (server);

	return ret;
}

static int on_timer (void *udata)
{
	imap_server_check (udata);

	return 1;
}

static ImapServer *create_server (MailBox *mb)
{
	ImapServer *s;

	s = calloc (1, sizeof (ImapServer));
	if (!s)
		return NULL;

	s->interval = MAX (mailbox_poll_interval_get (mb), MIN_INTERVAL);
	s->timer = ecore_timer_add (s->interval, on_timer, s);
	if (!s->timer) {
		free (s);
		return NULL;
	}

	s->host = strdup (mailbox_property_get (mb, "host"));
	s->port = (int) mailbox_property_get (mb, "port");
	s->use_ssl = (bool) mailbox_property_get (mb, "ssl");
	s->user = strdup (mailbox_property_get (mb, "user"));
	s->pass = strdup (mailbox_property_get (mb, "pass"));
	s->state = STATE_DISCONNECTED;

	return s;
}

static bool destroy_server (ImapServer *server)
{
	assert (server);

	free (server->host);
	free (server->user);
	free (server->pass);

	if (server->timer)
		ecore_timer_del (server->timer);

	servers = evas_list_remove (servers, server);
	free (server);
	
	return true;
}

static bool imap_add_server (MailBox *mb)
{
	ImapServer *server;
	double interval;

	assert (mb);

	/* did we already create a server for this mailbox? */
	server = find_server_by_mailbox (mb);
	if (!server) {
		server = create_server (mb);
		if (!server)
			return false;

		servers = evas_list_append (servers, server);
	} else {
		interval = MAX (mailbox_poll_interval_get (mb), MIN_INTERVAL);
		if (interval < server->interval) {
			server->interval = interval;
			ecore_timer_del (server->timer);

			server->timer = ecore_timer_add (server->interval,
			                                 on_timer, server);
			if (!server->timer) {
				destroy_server (server);
				return false;
			}
		}
	}

	server->clients = evas_list_append (server->clients, mb);
	mailbox_property_set (mb, "server", server);

	return true;
}

static bool imap_remove_server (ImapServer *server, MailBox *mb)
{
	assert (server);
	assert (mb);

	/* FIXME: reschedule server timer */
	server->clients = evas_list_remove (server->clients, mb);
	if (!server->clients)
		destroy_server (server);

	return true;
}

static bool imap_add_mailbox (MailBox *mb)
{
	assert (mb);

	return imap_add_server (mb);
}

static bool imap_remove_mailbox (MailBox *mb)
{
	ImapServer *server;

	assert (mb);

	if ((server = mailbox_property_get (mb, "server")))
		imap_remove_server (server, mb);

	free (mailbox_property_get (mb, "host"));
	free (mailbox_property_get (mb, "user"));
	free (mailbox_property_get (mb, "pass"));
	free (mailbox_property_get (mb, "path"));

	return true;
}

static bool imap_load_config (MailBox *mb, E_DB_File *edb,
                              const char *root)
{
	char key[32], *str;
	int val, use_ssl = 0;

	assert (mb);
	assert (edb);
	assert (root);

	if (ecore_con_ssl_available_get ()) {
		/* check whether OpenSSL should be used */
		snprintf (key, sizeof (key), "%s/ssl", root);

		if (!e_db_int_get (edb, key, &use_ssl))
			use_ssl = 0;

		mailbox_property_set (mb, "ssl", (int *) use_ssl);
	}

	/* read server */
	snprintf (key, sizeof (key), "%s/host", root);

	if (!(str = e_db_str_get (edb, key))) {
		fprintf (stderr, "[imap] 'host' not specified!\n");
		return false;
	}

	mailbox_property_set (mb, "host", str);

	/* read port */
	snprintf (key, sizeof (key), "%s/port", root);

	if (!e_db_int_get (edb, key, &val))
		val = use_ssl ? 993 : 143;

	mailbox_property_set (mb, "port", (int *) val);

	/* read username */
	snprintf (key, sizeof (key), "%s/user", root);

	if (!(str = e_db_str_get (edb, key))) {
		fprintf (stderr, "[imap] 'user' not specified!\n");
		return false;
	}

	mailbox_property_set (mb, "user", str);

	/* read password */
	snprintf (key, sizeof (key), "%s/pass", root);

	if (!(str = e_db_str_get (edb, key))) {
		fprintf (stderr, "[imap] 'user' not specified!\n");
		return false;
	}

	mailbox_property_set (mb, "pass", str);

	/* read mailbox path */
	snprintf (key, sizeof (key), "%s/path", root);

	if (!(str = e_db_str_get (edb, key))) {
		fprintf (stderr, "[imap] 'path' not specified!\n");
		return false;
	}

	mailbox_property_set (mb, "path", str);

	return true;
}

static void imap_shutdown ()
{
	int i;

	for (i = 0; i < 3; i++) {
		ecore_event_handler_del (ev_hdl[i]);
		ev_hdl[i] = NULL;
	}

	ecore_con_shutdown ();
}

bool embrace_plugin_init (EmbracePlugin *ep)
{
	plugin = ep;
	snprintf (ep->name, sizeof (ep->name), "%s", "imap");

	ep->check = imap_check;
	ep->load_config = imap_load_config;
	ep->shutdown = imap_shutdown;

	ep->add_mailbox = imap_add_mailbox;
	ep->remove_mailbox = imap_remove_mailbox;

	ecore_con_init ();

	ev_hdl[0] = ecore_event_handler_add (ECORE_CON_EVENT_SERVER_ADD,
	                                     on_server_add, NULL);
	ev_hdl[1] = ecore_event_handler_add (ECORE_CON_EVENT_SERVER_DATA,
	                                     on_server_data, NULL);
	ev_hdl[2] = ecore_event_handler_add (ECORE_CON_EVENT_SERVER_DEL,
	                                     on_server_del, NULL);

	return true;
}
