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

#include <Ecore_Con.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <embrace_plugin.h>

#define MAX_INTERVAL 300

static EmbracePlugin *plugin = NULL;

static int on_server_add (void *udata, int type, void *event)
{
	Ecore_Con_Event_Server_Add *ev = event;
	MailBox *mb;
	char buf[256];
	int len;

	mb = ecore_con_server_data_get (ev->server);
	assert (mb);

	if (mailbox_plugin_get (mb) != plugin)
		return 1;

	/* now login to the server */
	len = snprintf (buf, sizeof (buf), "A001 LOGIN %s %s\r\n",
	                (char *) mailbox_property_get (mb, "user"),
	                (char *) mailbox_property_get (mb, "pass"));

	ecore_con_server_send (ev->server, buf, len);

	return 0;
}

static int on_server_data (void *udata, int type, void *event)
{
	Ecore_Con_Event_Server_Data *ev = event;
	MailBox *mb;
	char realbuf[1024], *buf = realbuf;
	int num = 0, new = 0, len;

	mb = ecore_con_server_data_get (ev->server);
	assert (mb);

	if (mailbox_plugin_get (mb) != plugin)
		return 1;

	/* take the data and make a NUL-terminated string out of it */
	len = sizeof (realbuf) - 1;
	len = MIN(len, ev->size);

	memcpy (buf, ev->data, len);
	buf[len] = 0;

	/* STATUS the correct directory on successful login */
	if (!strncmp(buf, "A001 OK LOGIN", 13)) {
		len = snprintf (buf, sizeof (realbuf),
		                "A002 STATUS %s (MESSAGES UNSEEN)\r\n",
		                (char *) mailbox_property_get (mb, "path"));
		ecore_con_server_send (ev->server, buf, len);

		return 0;
	}

	/* check for STATUS response, fetch numbers */
	while (*buf != '\0' && *buf != '(')
		buf++;

	if (sscanf (buf, "(MESSAGES %i UNSEEN %i)", &num, &new) == 2) {
		mailbox_total_set (mb, num);
		mailbox_unseen_set (mb, new);

		ecore_con_server_send (ev->server, "A003 LOGOUT", 11);
		ecore_con_server_del (ev->server);
	}

	return 0;
}

static bool imap_check (MailBox *mb)
{
	Ecore_Con_Type type = ECORE_CON_REMOTE_SYSTEM;
	char *host;
	int port;

	host = mailbox_property_get (mb, "host"),
	port = (int) mailbox_property_get (mb, "port");

	assert (host);
	assert (port);

#ifdef HAVE_OPENSSL
	if ((int) mailbox_property_get (mb, "ssl"))
		type |= ECORE_CON_USE_SSL;
#endif

	ecore_con_server_connect (type, host, port, mb);

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

	interval = MAX (mailbox_poll_interval_get (mb), MAX_INTERVAL);

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

#ifdef HAVE_OPENSSL
	/* check whether OpenSSL should be used */
	snprintf (key, sizeof (key), "%s/ssl", root);

	if (!e_db_int_get (edb, key, &use_ssl))
		use_ssl = 0;

	mailbox_property_set (mb, "ssl", (int *) use_ssl);
#endif

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

	return true;
}
