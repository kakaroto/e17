/*
 * $Id$
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

#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#ifdef __linux__
# define USE_DNOTIFY
# include <fcntl.h>
#endif

#include <embrace_plugin.h>

#ifdef USE_DNOTIFY
typedef struct {
	MailBox *mailbox;
	int fd;
	int signal;
	bool is_unseen;
} NotifyData;

static Eina_List *find_notify_data (MailBox *mb, int signal);

static Eina_List *notify_data = NULL;
#endif

static int get_files (char *path)
{
	DIR *dir;
	struct dirent *entry;
	struct stat st;
	char buf[PATH_MAX + 1];
	int ret = 0;

	if (!(dir = opendir (path)))
		return -1;

	while ((entry = readdir (dir))) {
		if (!strcmp (entry->d_name, ".")
		    || !strcmp (entry->d_name, ".."))
			continue;

		snprintf (buf, sizeof (buf), "%s/%s",
		          path, entry->d_name);

		if (!stat (buf, &st) && S_ISREG(st.st_mode))
			ret++;
	}

	closedir (dir);

	return ret;
}

static bool maildir_check (MailBox *mb)
{
	char *path[2];
	int total = 0, unseen = 0;

	assert (mb);

	path[0] = mailbox_property_get (mb, "path_cur");
	path[1] = mailbox_property_get (mb, "path_new");
	assert (path[0]);
	assert (path[1]);

	/* get total mail count */
	total = get_files (path[0]);

	if (total == -1)
		return false;

	/* get new mail count */
	unseen = get_files (path[1]);

	if (unseen == -1)
		return false;

	mailbox_unseen_set (mb, unseen);
	mailbox_total_set (mb, unseen + total);

	return true;
}

#ifdef USE_DNOTIFY
static int on_notify (void *udata, int type, void *event)
{
	Ecore_Event_Signal_Realtime *ev = event;
	NotifyData *data = NULL;
	char *path;
	Eina_List *l;
	int num;

	/* find the set we need to work with */
	if (!(l = find_notify_data (NULL, ev->num + SIGRTMIN)))
		return 1;

	data = l->data;

	assert (data);
	assert (data->mailbox);
	assert (data->fd == ev->data.si_fd);

	if (data->is_unseen)
		path = mailbox_property_get (data->mailbox, "path_new");
	else
		path = mailbox_property_get (data->mailbox, "path_cur");

	assert (path);

	num = get_files (path);

	if (data->is_unseen)
		mailbox_unseen_set (data->mailbox, num);
	else {
		num += mailbox_unseen_get (data->mailbox);
		mailbox_total_set (data->mailbox, num);
	}

	return 0;
}

static bool monitor_dir (MailBox *mb, const char *path, bool is_unseen)
{
	NotifyData *data;
	int fd;

	assert (mb);
	assert (path);

	fd = open (path, O_RDONLY);

	if (fd == -1)
		return false;

	/* now store additional data so we can access it from within
	 * the signal handler.
	 */
	data = malloc (sizeof (NotifyData));

	data->fd = fd;
	data->signal = embrace_signal_get ();
	data->is_unseen = is_unseen;
	data->mailbox = mb;

	notify_data = eina_list_append (notify_data, data);

	/* tell the kernel what events we are interested in */
	fcntl (fd, F_SETSIG, data->signal);
	fcntl (fd, F_NOTIFY,
	       DN_MODIFY | DN_CREATE | DN_DELETE | DN_MULTISHOT);

	return true;
}
#endif

#ifndef USE_DNOTIFY
static int on_timer (void *udata)
{
	maildir_check (udata);

	return 1;
}
#endif

static bool maildir_add_mailbox (MailBox *mb)
{
#ifdef USE_DNOTIFY
	char *str, *prop[] = {"path_cur", "path_new"};
	int i;
#else
	Ecore_Timer *timer;
	int interval;
#endif

	assert (mb);

#ifdef USE_DNOTIFY
	for (i = 0; i < 2; i++) {
		str = mailbox_property_get (mb, prop[i]);
		monitor_dir (mb, str, !!i);
	}
#else
	interval = mailbox_poll_interval_get (mb);

	if (!(timer = ecore_timer_add (interval, on_timer, mb)))
		return false;

	mailbox_property_set (mb, "timer", timer);
#endif

	return true;
}

#ifdef USE_DNOTIFY
static Eina_List *find_notify_data (MailBox *mb, int signal)
{
	NotifyData *data;
	Eina_List *l;

	for (l = notify_data; l; l = l->next) {
		data = l->data;

		if (data->mailbox == mb || data->signal == signal)
			return l;
	}

	return NULL;
}
#endif

static bool maildir_remove_mailbox (MailBox *mb)
{
#ifdef USE_DNOTIFY
	NotifyData *data;
	Eina_List *l;
#else
	Ecore_Timer *timer;
#endif

	assert (mb);

	/* free plugin data */
	free (mailbox_property_get (mb, "path_cur"));
	free (mailbox_property_get (mb, "path_new"));

#ifdef USE_DNOTIFY
	while ((l = find_notify_data (mb, 0))) {
		data = l->data;

		close (data->fd);
		free (data);

		notify_data = eina_list_remove_list (notify_data, l);
	}
#else
	if ((timer = mailbox_property_get (mb, "timer")))
		ecore_timer_del (timer);
#endif

	return true;
}

#ifdef USE_DNOTIFY
static void maildir_shutdown ()
{
	assert (!notify_data);
}
#endif

static bool maildir_load_config (MailBox *mb, E_DB_File *edb,
                                 const char *root)
{
	char key[256], *str, *path[2], expanded[PATH_MAX + 1];
	char *prop[2] = {"path_cur", "path_new"};
	int i;

	assert (mb);
	assert (edb);
	assert (root);

	/* read path */
	snprintf (key, sizeof (key), "%s/path", root);

	if (!(str = e_db_str_get (edb, key))) {
		fprintf (stderr, "[maildir] 'path' not specified!\n");
		return false;
	}

	embrace_expand_path (str, expanded, sizeof (expanded));
	free (str);

	path[0] = malloc ((PATH_MAX + 1) * sizeof (char));
	path[1] = malloc ((PATH_MAX + 1) * sizeof (char));

	for (i = 0; i < 2; i++) {
		snprintf (path[i], PATH_MAX + 1, "%s/%s",
		          expanded, &prop[i][5]);
		mailbox_property_set (mb, prop[i], path[i]);
	}

	return true;
}

bool embrace_plugin_init (EmbracePlugin *ep)
{
	snprintf (ep->name, sizeof (ep->name), "%s", "maildir");

	ep->check = maildir_check;
	ep->load_config = maildir_load_config;

#ifdef USE_DNOTIFY
	ep->shutdown = maildir_shutdown;
#endif

	ep->add_mailbox = maildir_add_mailbox;
	ep->remove_mailbox = maildir_remove_mailbox;

#ifdef USE_DNOTIFY
	ecore_event_handler_add (ECORE_EVENT_SIGNAL_REALTIME,
	                         on_notify, NULL);
#endif

	return true;
}
