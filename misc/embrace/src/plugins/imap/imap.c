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

#define IDENT "EMBRACE_IMAP_PLUGIN"
#define MAX_INTERVAL 300

static int on_server_add (void *udata, int type, void *event)
{
	Ecore_Con_Event_Server_Add *ev = event;
	MailBox *mb;
	char buf[256];
	int len;

	if (strcmp ((char *) udata, IDENT))
		return 1;

	mb = ecore_con_server_data_get (ev->server);

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

	if (strcmp ((char *) udata, IDENT))
		return 1;

	mb = ecore_con_server_data_get (ev->server);

	/* take the data and make a NUL-terminated string out of it */
	len = sizeof (realbuf) - 1;
	len = MIN(len, ev->size);

	memcpy (buf, ev->data, len);
	buf[len] = 0;

	/* STATUS the correct directory on successful login */
	if (!strncmp(buf, "A001 OK LOGIN", 13)) {
		len = snprintf (buf, sizeof (realbuf),
		                "A002 STATUS %s (MESSAGES UNSEEN)\r\n",
		                mailbox_property_get (mb, "path"));
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

bool imap_check (MailBox *mb)
{
	char *host;
	int port;

	host = mailbox_property_get (mb, "host"),
	port = (int) mailbox_property_get (mb, "port");

	assert (host);
	assert (port);

	ecore_con_server_connect (ECORE_CON_REMOTE_SYSTEM,
	                          host, port, mb);

	return true;
}

static int on_timer (void *udata)
{
	imap_check (udata);

	return 1;
}

bool imap_add_mailbox (MailBox *mb)
{
	Ecore_Timer *timer;
	int interval;

	assert (mb);

	interval = MAX(mailbox_poll_interval_get (mb), MAX_INTERVAL);

	if (!(timer = ecore_timer_add (interval, on_timer, mb)))
		return false;

	mailbox_property_set (mb, "timer", timer);

	return true;
}

bool imap_remove_mailbox (MailBox *mb)
{
	Ecore_Timer *timer;

	assert (mb);

	if (!(timer = mailbox_property_get (mb, "timer")))
		ecore_timer_del (timer);

	free (mailbox_property_get (mb, "host"));
	free (mailbox_property_get (mb, "user"));
	free (mailbox_property_get (mb, "pass"));
	free (mailbox_property_get (mb, "path"));

	return true;
}

bool imap_load_config (MailBox *mb, E_DB_File *edb, char *root)
{
	char key[32], *str;
	int val;

	assert (mb);
	assert (edb);
	assert (root);

	/* read server */
	snprintf (key, sizeof (key), "%s/host", root);

	if (!(str = e_db_str_get (edb, key)))
		return false;

	mailbox_property_set (mb, "host", str);

	/* read port */
	snprintf (key, sizeof (key), "%s/port", root);

	if (!e_db_int_get (edb, key, &val))
		val = 143;

	mailbox_property_set (mb, "port", (int *) val);

	/* read username */
	snprintf (key, sizeof (key), "%s/user", root);

	if ((str = e_db_str_get (edb, key)))
		mailbox_property_set (mb, "user", str);

	/* read password */
	snprintf (key, sizeof (key), "%s/pass", root);

	if ((str = e_db_str_get (edb, key)))
		mailbox_property_set (mb, "pass", str);
	
	/* read mailbox path */
	snprintf (key, sizeof (key), "%s/path", root);
	
	if ((str = e_db_str_get (edb, key)))
		mailbox_property_set (mb, "path", str);

	return true;
}

void imap_shutdown ()
{
	ecore_con_shutdown ();
}

bool embrace_plugin_init (EmbracePlugin *ep)
{
	snprintf (ep->name, sizeof (ep->name), "%s", "imap");

	ep->check = imap_check;
	ep->load_config = imap_load_config;
	ep->shutdown = imap_shutdown;

	ep->add_mailbox = imap_add_mailbox;
	ep->remove_mailbox = imap_remove_mailbox;

	ecore_con_init ();

	ecore_event_handler_add (ECORE_CON_EVENT_SERVER_ADD,
	                         on_server_add, IDENT);
	ecore_event_handler_add (ECORE_CON_EVENT_SERVER_DATA,
	                         on_server_data, IDENT);

	return true;
}
