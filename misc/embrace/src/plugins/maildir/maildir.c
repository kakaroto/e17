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

#include <limits.h>
#include <assert.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>

#ifdef __linux__
# define USE_DNOTIFY
#else
# error Linux 2.4+ is required at the moment (yeah, this sucks)
#endif

#ifdef USE_DNOTIFY
# include <fcntl.h>
# include <signal.h>
#endif

#include <embrace_plugin.h>

#ifdef USE_DNOTIFY
typedef struct {
	MailBox *mailbox;
	int fd;
	bool is_unseen;
} NotifyData;

static Evas_List *notify_data = NULL;
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
static void on_notify (int sig, siginfo_t *si, void *whatever)
{
	NotifyData *data = NULL;
	char *path;
	Evas_List *l;
	int num;

	/* find the set we need to work with */
	for (l = notify_data; l; l = l->next) {
		data = l->data;

		if (si->si_fd == data->fd)
			break;
	}

	assert (data->mailbox);

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
}

static bool monitor_dir (MailBox *mb, const char *path, bool is_unseen)
{
	NotifyData *data;
	struct sigaction act;
	int fd, signal;

	assert (mb);
	assert (path);

	signal = embrace_signal_get ();

	/* setup the signal handler */
	act.sa_sigaction = on_notify;
	sigemptyset (&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	sigaction (signal, &act, NULL);

	fd = open (path, O_RDONLY);

	if (fd == -1)
		return false;

	/* tell the kernel what events we are interested in */
	fcntl (fd, F_SETSIG, signal);
	fcntl (fd, F_NOTIFY,
	       DN_MODIFY | DN_CREATE | DN_DELETE | DN_MULTISHOT);

	/* now store additional data so we can access it from within
	 * the signal handler.
	 */
	data = malloc (sizeof (NotifyData));

	data->fd = fd;
	data->is_unseen = is_unseen;
	data->mailbox = mb;

	notify_data = evas_list_append (notify_data, data);

	return true;
}

static bool maildir_add_mailbox (MailBox *mb)
{
	char *str, *prop[] = {"path_cur", "path_new"};
	int i;

	assert (mb);

	for (i = 0; i < 2; i++) {
		str = mailbox_property_get (mb, prop[i]);
		monitor_dir (mb, str, !!i);
	}

	return true;
}

static Evas_List *find_notify_data (MailBox *mb)
{
	NotifyData *data;
	Evas_List *l;

	for (l = notify_data; l; l = l->next) {
		data = l->data;

		if (data->mailbox == mb)
			return l;
	}

	return NULL;
}
#endif

static bool maildir_remove_mailbox (MailBox *mb)
{
#ifdef USE_DNOTIFY
	NotifyData *data;
	Evas_List *l;
#endif

	assert (mb);

	/* free plugin data */
	free (mailbox_property_get (mb, "path_cur"));
	free (mailbox_property_get (mb, "path_new"));

#ifdef USE_DNOTIFY
	while ((l = find_notify_data (mb))) {
		data = l->data;

		close (data->fd);
		free (data);

		notify_data = evas_list_remove_list (notify_data, l);
	}
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
                                 char *root)
{
	char key[256], *str, *path[2], expanded[PATH_MAX + 1];
	char *prop[2] = {"path_cur", "path_new"};
	int i;

	assert (mb);
	assert (edb);
	assert (root);

	/* read path */
	snprintf (key, sizeof (key), "%s/path", root);

	if (!(str = e_db_str_get (edb, key)))
		return false;

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
	ep->add_mailbox = maildir_add_mailbox;
#endif

	ep->remove_mailbox = maildir_remove_mailbox;

	return true;
}
