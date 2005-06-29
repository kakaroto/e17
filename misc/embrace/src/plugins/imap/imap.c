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

#include <embrace_plugin.h>

typedef enum {
	STATE_DISCONNECTED,
	STATE_CONNECTED,
	STATE_SERVER_READY,
	STATE_LOGGED_IN,
	STATE_STATUS_OK
} State;

#define MIN_INTERVAL 60
#define LOGOUT() \
	if (state >= STATE_LOGGED_IN) { \
		len = snprintf (outbuf, sizeof (outbuf), \
		                "A%03i LOGOUT", command_no); \
		ecore_con_server_send (ev->server, outbuf, len); \
	} \
\
	ecore_con_server_del (ev->server); \
	mailbox_property_set (mb, "server", NULL);

static EmbracePlugin *plugin = NULL;

static int on_server_add (void *udata, int type, void *event)
{
	Ecore_Con_Event_Server_Add *ev = event;
	MailBox *mb;

	mb = ecore_con_server_data_get (ev->server);
	assert (mb);

	if (mailbox_plugin_get (mb) != plugin)
		return 1;

	mailbox_property_set (mb, "state", (void *) STATE_CONNECTED);
	mailbox_property_set (mb, "command_no", (void *) 0);

	return 0;
}

static int on_server_data (void *udata, int type, void *event)
{
	Ecore_Con_Event_Server_Data *ev = event;
	MailBox *mb;
	State state;
	char inbuf[1024], outbuf[256], *spc;
	int total = 0, unseen = 0, len, command_no;
	size_t slen;

	mb = ecore_con_server_data_get (ev->server);
	assert (mb);

	if (mailbox_plugin_get (mb) != plugin)
		return 1;

	/* take the data and make a NUL-terminated string out of it */
	len = sizeof (inbuf) - 1;
	len = MIN (len, ev->size);

	memcpy (inbuf, ev->data, len);
	inbuf[len] = 0;
	embrace_strstrip (inbuf);

	state = (State) mailbox_property_get (mb, "state");
	assert (state != STATE_DISCONNECTED);

	command_no = (int) mailbox_property_get (mb, "command_no");
	mailbox_property_set (mb, "command_no", (void *) ++command_no);

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

	mailbox_property_set (mb, "state", (void *) ++state);

	switch (state) {
		case STATE_SERVER_READY:
			len = snprintf (outbuf, sizeof (outbuf),
			                "A%03i LOGIN %s %s\r\n",
			                command_no,
			                (char *) mailbox_property_get (mb, "user"),
			                (char *) mailbox_property_get (mb, "pass"));
			ecore_con_server_send (ev->server, outbuf, len);
			break;
		case STATE_LOGGED_IN:
			len = snprintf (outbuf, sizeof (outbuf),
			                "A%03i STATUS %s (MESSAGES UNSEEN)\r\n",
			                command_no,
			                (char *) mailbox_property_get (mb, "path"));
			ecore_con_server_send (ev->server, outbuf, len);
			break;
		case STATE_STATUS_OK:
			if (sscanf (inbuf, "* STATUS %*s (MESSAGES %i UNSEEN %i)",
			            &total, &unseen) == 2) {
				mailbox_unseen_set (mb, unseen);
				mailbox_total_set (mb, total);

				LOGOUT ();
			}

			break;
		default:
			assert (false);
	}

	return 0;
}

static int on_server_del (void *udata, int type, void *event)
{
	Ecore_Con_Event_Server_Del *ev = event;
	MailBox *mb;
	char *host;

	mb = ecore_con_server_data_get (ev->server);
	assert (mb);

	if (mailbox_plugin_get (mb) != plugin)
		return 1;

	host = (char *) mailbox_property_get (mb, "host");

	if (mailbox_property_get (mb, "state") == STATE_DISCONNECTED)
		fprintf (stderr, "[imap] cannot connect to '%s'\n", host);
	else {
		mailbox_property_set (mb, "state", STATE_DISCONNECTED);
		fprintf (stderr, "[imap] lost connection to '%s'\n", host);
	}

	ecore_con_server_del (ev->server);
	mailbox_property_set (mb, "server", NULL);

	return 0;
}

static bool imap_check (MailBox *mb)
{
	Ecore_Con_Type type = ECORE_CON_REMOTE_SYSTEM;
	Ecore_Con_Server *server;
	char *host;
	int port;

	host = mailbox_property_get (mb, "host"),
	port = (int) mailbox_property_get (mb, "port");

	assert (host);
	assert (port);

	if (mailbox_property_get (mb, "server")) {
		fprintf (stderr, "[imap] already connected to '%s'\n", host);
		return false;
	}

	if (ecore_con_ssl_available_get () &&
	    mailbox_property_get (mb, "ssl"))
		type |= ECORE_CON_USE_SSL;

	server = ecore_con_server_connect (type, host, port, mb);

	mailbox_property_set (mb, "state", STATE_DISCONNECTED);
	mailbox_property_set (mb, "server", server);

	return true;
}

static int on_timer (void *udata)
{
	imap_check (udata);

	return 1;
}

static bool imap_add_mailbox (MailBox *mb)
{
	Ecore_Timer *timer;
	int interval;

	assert (mb);

	interval = MAX (mailbox_poll_interval_get (mb), MIN_INTERVAL);

	if (!(timer = ecore_timer_add (interval, on_timer, mb)))
		return false;

	mailbox_property_set (mb, "timer", timer);

	return true;
}

static bool imap_remove_mailbox (MailBox *mb)
{
	Ecore_Timer *timer;

	assert (mb);

	if ((timer = mailbox_property_get (mb, "timer")))
		ecore_timer_del (timer);

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

	ecore_event_handler_add (ECORE_CON_EVENT_SERVER_ADD,
	                         on_server_add, NULL);
	ecore_event_handler_add (ECORE_CON_EVENT_SERVER_DATA,
	                         on_server_data, NULL);
	ecore_event_handler_add (ECORE_CON_EVENT_SERVER_DEL,
	                         on_server_del, NULL);

	return true;
}
