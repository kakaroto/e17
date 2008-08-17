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
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <embrace_plugin.h>

static void read_file (FILE *fp, int *unseen, int *total)
{
	char buf[1024];
	bool in_header = false;

	assert (fp);

	while (fgets (buf, sizeof (buf), fp)) {
		if (buf[0] == '\n')
			in_header = false;
		else if (!strncmp (buf, "From ", 5)) {
			in_header = true;
			(*total)++;
			(*unseen)++;
		} else if (in_header && !strncmp (buf, "Status: ", 7)
		           && strchr (buf, 'R'))
			(*unseen)--;
	}
}

static bool check_file (MailBox *mb, int *unseen, int *total)
{
	FILE *fp;

	assert (mb);

	if (!(fp = fopen (mailbox_property_get (mb, "path"), "r")))
		return false;

	read_file (fp, unseen, total);
	fclose (fp);

	return true;
}

static bool has_changed (MailBox *mb)
{
	char *path;
	struct stat st;
	time_t *mtime;
	off_t *size;
	bool ret;

	assert (mb);

	path = mailbox_property_get (mb, "path");

	if (stat (path, &st))
		return true;

	mtime = mailbox_property_get (mb, "mtime");
	size = mailbox_property_get (mb, "size");

	/* check whether mtime or size have changed */
	ret = (st.st_mtime != *mtime || st.st_size != *size);

	*mtime = st.st_mtime;
	*size = st.st_size;

	return ret;
}

static bool mbox_check (MailBox *mb)
{
	int total = 0, unseen = 0;

	assert (mb);

	if (!has_changed (mb))
		return false;

	if (!check_file (mb, &unseen, &total))
		return false;

	mailbox_unseen_set (mb, unseen);
	mailbox_total_set (mb, total);

	return true;
}

static int on_timer (void *udata)
{
	mbox_check (udata);

	return 1;
}

static bool mbox_add_mailbox (MailBox *mb)
{
	Ecore_Timer *timer;
	int interval;
	time_t *tzero;
	off_t *ozero;

	assert (mb);

	interval = mailbox_poll_interval_get (mb);

	if (!(timer = ecore_timer_add (interval, on_timer, mb)))
		return false;

	mailbox_property_set (mb, "timer", timer);

	/* init the mtime and size properties */
	if (!(tzero = calloc (1, sizeof (time_t)))) {
		mailbox_property_set (mb, "timer", NULL);
		ecore_timer_del (timer);

		return false;
	}

	mailbox_property_set (mb, "mtime", tzero);

	if (!(ozero = calloc (1, sizeof (off_t)))) {
		mailbox_property_set (mb, "timer", NULL);
		mailbox_property_set (mb, "mtime", NULL);

		ecore_timer_del (timer);
		free (tzero);

		return false;
	}

	mailbox_property_set (mb, "size", ozero);

	return true;
}

static bool mbox_remove_mailbox (MailBox *mb)
{
	Ecore_Timer *timer;
	assert (mb);

	free (mailbox_property_get (mb, "path"));

	if ((timer = mailbox_property_get (mb, "timer")))
		ecore_timer_del (timer);

	free (mailbox_property_get (mb, "mtime"));
	free (mailbox_property_get (mb, "size"));

	return true;
}

static bool mbox_load_config (MailBox *mb, E_DB_File *edb,
                              const char *root)
{
	char key[32], *str, *path;

	assert (mb);
	assert (edb);
	assert (root);

	if (!(path = malloc ((PATH_MAX + 1) * sizeof (char))))
		return false;

	snprintf (key, sizeof (key), "%s/path", root);

	if (!(str = e_db_str_get (edb, key))) {
		fprintf (stderr, "[mbox] 'path' not specified!\n");
		return false;
	}

	embrace_expand_path (str, path, PATH_MAX + 1);
	free (str);

	mailbox_property_set (mb, "path", path);

	return true;
}

bool embrace_plugin_init (EmbracePlugin *ep)
{
	snprintf (ep->name, sizeof (ep->name), "%s", "mbox");

	ep->check = mbox_check;
	ep->load_config = mbox_load_config;

	ep->add_mailbox = mbox_add_mailbox;
	ep->remove_mailbox = mbox_remove_mailbox;

	return true;
}
