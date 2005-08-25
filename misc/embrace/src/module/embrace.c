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

#include <assert.h>
#include <ctype.h>

#include <e.h>
#include "embrace.h"
#include "embrace_plugin.h"

/**
 * Copies one string to another, but '~' is expanded.
 */
void embrace_expand_path (char *str, char *dest, int destlen)
{
	char *home = getenv ("HOME"), *ptr;
	int cur = 0, home_len = strlen (home);

	if (!strchr (str, '~')) {
		strncat (dest, str, destlen);
		return;
	}

	dest[0] = 0;

	for (ptr = str; *ptr; ptr++) {
		if (*ptr == '~') {
			strncat (dest, home, destlen - cur);
			dest += home_len;
			cur += home_len;
		} else {
			*dest++ = *ptr;
			cur++;
		}
	}

	*dest++ = 0;
}

/**
 * Remove leading and trailing whitespace
 *
 * @param str
 * @return Trimmed string.
 */
char *embrace_strstrip (char *str)
{
	char *start, *ptr = str;

	assert (str);

	if (!strlen (str))
		return str;

	/* step over leading whitespace */
	for (start = str; isspace (*start); start++);

	if (str != start) {
		while ((*ptr++ = *start++));
		*ptr = 0;
	}

	if (!strlen (str))
		return str;

	ptr = &str[strlen (str) - 1];

	if (!isspace (*ptr))
		return str;

	while (isspace (*ptr) && ptr > str)
		ptr--;

	ptr[1] = 0;

	return str;
}

static EmbracePlugin *find_plugin (Embrace *e, const char *name)
{
	EmbracePlugin *ep;
	Evas_List *l;

	assert (e);
	assert (name);

	for (l = e->plugins; l; l = l->next) {
		ep = l->data;

		if (!strcasecmp (ep->name, name))
			return ep;
	}

	return NULL;
}

static void load_plugin (Embrace *e, const char *file)
{
	EmbracePlugin *ep;

	assert (e);
	assert (file);

	if (!(ep = embrace_plugin_new (file)))
		return;

	/* only add this plugin if it hasn't been added yet */
	if (find_plugin (e, ep->name))
		embrace_plugin_free (ep);
	else
		e->plugins = evas_list_append (e->plugins, ep);
}

static void load_plugins (Embrace *e, const char *path)
{
	DIR *dir;
	struct dirent *entry;
	char buf[PATH_MAX + 1];
	int len;

	assert (e);
	assert (path);

	if (!(dir = opendir (path)))
		return;

	while ((entry = readdir (dir))) {
		if (!strcmp (entry->d_name, ".")
		    || !strcmp (entry->d_name, ".."))
			continue;

		if ((len = strlen (entry->d_name)) < 4)
			continue;

		if (!strcmp (&entry->d_name[len - 3], ".so")) {
			snprintf (buf, sizeof (buf), "%s/%s", path, entry->d_name);
			load_plugin (e, buf);
		}
	}

	closedir (dir);
}

static bool embrace_load_plugins (Embrace *e)
{
	char path[PATH_MAX + 1];

	assert (e);

	embrace_expand_path ("~/.e/apps/" PACKAGE "/plugins",
	                     path, sizeof (path));

	load_plugins (e, path);
	load_plugins (e, PLUGIN_DIR);

	return (evas_list_count (e->plugins) > 0);
}

static MailBox *load_mailbox (Embrace *e, E_DB_File *edb, int i)
{
	MailBox *mb;
	EmbracePlugin *p;
	char key[32], *str;
	int val;

	assert (edb);

	/* read title */
	snprintf (key, sizeof (key), "/" PACKAGE "/mailbox%i/title", i);

	if (!(str = e_db_str_get (edb, key))) {
		fprintf (stderr, "mailbox %i: 'title' not specified!\n", i);
		return NULL;
	}

	if (!(mb = mailbox_new (e->gui.evas, e->conf->theme)))
		return NULL;

	mailbox_title_set (mb, str);
	free (str);

	/* read poll interval */
	snprintf (key, sizeof (key), "/" PACKAGE "/mailbox%i/interval", i);

	if (e_db_int_get (edb, key, &val))
		mailbox_poll_interval_set (mb, val);

	/* read mailbox type */
	snprintf (key, sizeof (key), "/" PACKAGE "/mailbox%i/type", i);

	if (!(str = e_db_str_get (edb, key))) {
		fprintf (stderr, "mailbox %i: 'type' not specified!\n", i);
		mailbox_free (mb);
		return NULL;
	}

	/* try to find the plugin for this mailbox type */
	if (!(p = find_plugin (e, str))) {
		fprintf (stderr,
		         "mailbox %i: no plugin found for type '%s'!\n",
		         i, str);
		free (str);
		mailbox_free (mb);
		return NULL;
	}

	mailbox_plugin_set (mb, p);
	free (str);

	/* load plugin-specific config */
	snprintf (key, sizeof (key), "/" PACKAGE "/mailbox%i", i);

	if (!mailbox_load_config (mb, edb, key)) {
		fprintf (stderr,
		         "mailbox %i: "
		         "loading plugin-specific configuration failed!\n", i);
		mailbox_free (mb);
		return NULL;
	}

	return mb;
}

static int load_mailboxes (Embrace *e, E_DB_File *edb)
{
	MailBox *mailbox;
	Evas_Object *edje;
	int i, num = 0, val = 0, w = 0, h = 0;

	assert (e);
	assert (edb);

	if (e_db_int_get (edb, "/" PACKAGE "/num_mailboxes", &val))
		num = val;

	for (i = 1; i <= num; i++)
		if ((mailbox = load_mailbox (e, edb, i))) {
			mailbox_emit_add (mailbox);
			edje = mailbox_edje_get (mailbox);

			h = mailbox_height_get (mailbox);
			w = mailbox_width_get (mailbox);

			e_box_pack_end (e->gui.container, edje);
			e_box_pack_options_set (edje,
						1, 1, /* fill */
					       	0, 0, /* expand */
					       	0.5, 0.5, /* align */
					       	w, h, /* min */
					       	w, h /* max */
						);

			e->mailboxes = evas_list_append (e->mailboxes, mailbox);
		}

	e_box_min_size_get(e->gui.container, &w, &h);
	evas_object_resize(e->gui.container, w, h);
	edje_extern_object_min_size_set(e->gui.container, w, h);
	edje_object_part_swallow (e->gui.edje, "Container",
	                          e->gui.container);
	edje_object_size_min_calc(e->gui.edje, &w, &h);
	evas_object_resize (e->gui.edje, w, h);

	return evas_list_count (e->mailboxes);
}

/**
 * Finds the filename for the theme @name.
 * Looks in: ~/.e/apps/embrace/themes
 *           DATADIR/themes
 */
static char *find_theme (const char *name)
{
	static char eet[PATH_MAX + 1];
	struct stat st;

	assert (name);

	snprintf (eet, sizeof (eet),
			 "%s/.e/apps/" PACKAGE "/themes/%s.edj",
			 getenv ("HOME"), name);

	if (!stat (eet, &st))
		return eet;

	snprintf (eet, sizeof (eet), DATA_DIR "/themes/%s.edj", name);

	return stat (eet, &st) ? NULL : eet;
}

static bool config_load_misc (Embrace *e, E_DB_File *edb)
{
	char *str = NULL, *theme;
	bool ret = false;

	assert (e);
	assert (edb);

	/* only load the values from the EDB if they haven't been overriden
	 * in argv.
	 */
	if (!*e->conf->evas_engine
	    && (str = e_db_str_get (edb, "/" PACKAGE "/evas_engine"))) {
		snprintf (e->conf->evas_engine, 255,
		          "%s", str);
		free (str);
		str = NULL;
	}

	if (!*e->conf->theme
	    && !(str = e_db_str_get (edb, "/" PACKAGE "/theme"))) {
		fprintf (stderr, "'theme' not specified, "
		         "falling back to the default theme!\n");
		str = strdup ("default");
	}

	if ((theme = find_theme (str ? str : e->conf->theme))) {
		snprintf (e->conf->theme, PATH_MAX, "%s", theme);
		ret = true;
	} else
		fprintf (stderr, "Cannot find theme '%s'!\n", e->conf->theme);

	if (str)
		free (str);

	return ret;
}

static E_DB_File *open_edb (Embrace *e)
{
	char file[PATH_MAX + 1];

	assert (e);

	embrace_expand_path ("~/.e/apps/" PACKAGE "/" PACKAGE ".db",
	                     file, sizeof (file));

	/* sync changes */
	e_db_flush ();

	return e_db_open_read (file);
}

static bool embrace_load_config (Embrace *e)
{
	E_DB_File *edb;
	bool ret;

	if (!(edb = open_edb (e)))
		return false;

	ret = config_load_misc (e, edb);
	e_db_close (edb);

	return ret;
}

static bool embrace_load_mailboxes (Embrace *e)
{
	E_DB_File *edb;
	bool ret;

	if (!(edb = open_edb (e)))
		return false;

	ret = (load_mailboxes (e, edb) > 0);
	e_db_close (edb);

	return ret;
}

static bool ui_load_edje (Embrace *e)
{
	assert (e);

	if (!(e->gui.edje = edje_object_add (e->gui.evas)))
		return false;

	evas_object_name_set (e->gui.edje, "main");

	if (!edje_object_file_set (e->gui.edje, e->conf->theme,
	                           "Embrace")) {
		fprintf (stderr, "Cannot load theme '%s'!\n", e->conf->theme);
		return false;
	}

	evas_object_move (e->gui.edje, 0, 0);
	evas_object_resize (e->gui.edje, 400, 400);

	evas_object_pass_events_set (e->gui.edje, true);
	evas_object_show (e->gui.edje);

	return true;
}

static bool ui_load_container (Embrace *e)
{
	assert (e);

	if (!edje_object_part_exists (e->gui.edje, "Container")) {
		fprintf (stderr, "Container missing!\n");
		return false;
	}

	if (!(e->gui.container = e_box_add (e->gui.evas)))
		return false;

	e_box_orientation_set (e->gui.container, 0);
	e_box_align_set (e->gui.container, 0.0, 0.0);

	edje_object_part_swallow (e->gui.edje, "Container",
	                          e->gui.container);

	evas_object_pass_events_set (e->gui.container, 1);
	evas_object_move (e->gui.container, 0, 0);
	evas_object_resize (e->gui.container, 400, 400);
	evas_object_show (e->gui.container);

	return true;
}

bool embrace_load_ui (Embrace *e)
{
	char path[PATH_MAX + 1];
	assert (e);

	embrace_expand_path ("~/.fonts", path, sizeof (path));
	evas_font_path_append (e->gui.evas, path);
	evas_font_path_append (e->gui.evas, "/usr/share/fonts");
	evas_font_path_append (e->gui.evas, "/usr/X11R6/lib/X11/fonts");

	if (!ui_load_edje (e))
		return false;

	if (!ui_load_container (e))
		return false;

	return true;
}

Embrace *embrace_new ()
{
	return calloc (1, sizeof (Embrace));
}

static void free_mailboxes (Embrace *e)
{
	MailBox *mb;

	assert (e);

	while (e->mailboxes) {
		mb = e->mailboxes->data;

		e->mailboxes = evas_list_remove (e->mailboxes, mb);
		mailbox_free (mb);
	}
}

static void free_plugins (Embrace *e)
{
	EmbracePlugin *ep;

	assert (e);

	while (e->plugins) {
		ep = e->plugins->data;

		e->plugins = evas_list_remove (e->plugins, ep);
		embrace_plugin_free (ep);
	}
}

static void free_ui (Embrace *e)
{
	assert (e);

	if (e->gui.container) {
		if (e->gui.edje)
			edje_object_part_unswallow (e->gui.edje, e->gui.container);
		evas_object_del (e->gui.container);
		e->gui.container = NULL;
	}
	if (e->gui.edje) {
		evas_object_del (e->gui.edje);
		e->gui.edje = NULL;
	}
}

void embrace_deinit (Embrace *e)
{
	assert (e);

	free_mailboxes (e);
	free_plugins (e);
	free_ui (e);
}

void embrace_free (Embrace *e)
{
	free (e);
}

static void embrace_config_init (Embrace *e)
{
	assert (e);

	*e->conf->evas_engine = 0;
	*e->conf->theme = 0;
}

bool embrace_init (Embrace *e)
{
	assert (e);

	embrace_config_init (e);

	if (!embrace_load_config (e)) {
		fprintf (stderr, "Cannot load config!\n");
		return false;
	}

	if (!embrace_load_ui (e)) {
		fprintf (stderr, "Cannot load ui!\n");
		return false;
	}

	if (!embrace_load_plugins (e)) {
		fprintf (stderr, "Cannot load plugins!\n");
		return false;
	}

	if (!embrace_load_mailboxes (e)) {
		fprintf (stderr, "Cannot load mailboxes!\n");
		return false;
	}

	return true;
}

static int check_mailboxes (void *udata)
{
	Embrace *e = udata;
	Evas_List *l;

	for (l = e->mailboxes; l; l = l->next)
		mailbox_check (l->data);

	/* only exec once */
	return 0;
}

void embrace_run (Embrace *e)
{
	assert (e);

	ecore_idler_add (check_mailboxes, e);
}

void embrace_stop (Embrace *e)
{
}

int embrace_signal_get ()
{
	return -1;
}
