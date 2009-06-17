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

#include <Esmart/Esmart_Container.h>
#include <Esmart/Esmart_Draggies.h>
#include <Esmart/Esmart_Trans_X11.h>
#include <ctype.h>
#include <assert.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "embrace.h"
#include "mailbox.h"
#include "embrace_plugin.h"

#ifdef SIGRTMIN
static int last_signal = 0;
#endif

/**
 * Copies one string to another, but '~' is expanded.
 */
void embrace_expand_path (char *str, char *dest, int destlen)
{
	char *home = getenv ("HOME"), *ptr;
	int cur = 0, home_len = strlen (home);

	if (!strchr (str, '~')) {
		strncpy (dest, str, destlen);
		dest[destlen - 1] = 0;
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
	Eina_List *l;

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
		e->plugins = eina_list_append (e->plugins, ep);
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

		if (!strcmp (&entry->d_name[len - 3], ".la")) {
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

	return (eina_list_count (e->plugins) > 0);
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

	if (!(mb = mailbox_new (e->gui.evas, e->cfg.theme)))
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
			esmart_container_element_append (e->gui.container, edje);

			e->mailboxes = eina_list_append (e->mailboxes, mailbox);

			h += mailbox_height_get (mailbox);
			w = mailbox_width_get (mailbox);
		}

	if (e->gui.ee) {
		ecore_evas_resize (e->gui.ee, w, h);
		ecore_evas_size_max_set (e->gui.ee, w, h);
		ecore_evas_size_min_set (e->gui.ee, w, h);
	}

	return eina_list_count (e->mailboxes);
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
	if (!*e->cfg.evas_engine
	    && (str = e_db_str_get (edb, "/" PACKAGE "/evas_engine"))) {
		snprintf (e->cfg.evas_engine, sizeof (e->cfg.evas_engine),
		          "%s", str);
		free (str);
		str = NULL;
	}

	if (!*e->cfg.theme
	    && !(str = e_db_str_get (edb, "/" PACKAGE "/theme"))) {
		fprintf (stderr, "'theme' not specified, "
		         "falling back to the default theme!\n");
		str = strdup ("default");
	}

	if ((theme = find_theme (str ? str : e->cfg.theme))) {
		snprintf (e->cfg.theme, sizeof (e->cfg.theme), "%s", theme);
		ret = true;
	} else
		fprintf (stderr, "Cannot find theme '%s'!\n", e->cfg.theme);

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

static void on_pre_render (Ecore_Evas *ee)
{
	edje_thaw ();
}

static void on_post_render (Ecore_Evas *ee)
{
	edje_freeze ();
}

static void on_resize (Ecore_Evas *ee)
{
	Evas *evas = ecore_evas_get (ee);
	Evas_Object *edje = evas_object_name_find (evas, "main");
	Evas_Object *dragger = evas_object_name_find (evas, "dragger");
	Evas_Object *trans = evas_object_name_find (evas, "trans");
	int x = 0, y = 0, w = 0, h = 0;

	ecore_evas_geometry_get (ee, &x, &y, &w, &h);
	evas_object_resize (edje, (Evas_Coord) w, (Evas_Coord) h);
	evas_object_resize (dragger, (Evas_Coord) w, (Evas_Coord) h);

	if (trans) {
		evas_object_resize (trans, (Evas_Coord) w, (Evas_Coord) h);
		esmart_trans_x11_freshen (trans, x, y, w, h);
	}
}

static void on_move (Ecore_Evas *ee)
{
	Evas *evas = ecore_evas_get (ee);
	Evas_Object *trans = evas_object_name_find (evas, "trans");
	int x = 0, y = 0, w = 0, h = 0;

	assert (trans);

	ecore_evas_geometry_get (ee, &x, &y, &w, &h);
	esmart_trans_x11_freshen (trans, x, y, w, h);
}

static void on_delete_request (Ecore_Evas *ee)
{
	ecore_main_loop_quit ();
}

static bool ui_load_edje (Embrace *e)
{
	Evas_Coord w = 0, h = 0;

	assert (e);

	if (!(e->gui.edje = edje_object_add (e->gui.evas)))
		return false;

	evas_object_name_set (e->gui.edje, "main");

	if (!edje_object_file_set (e->gui.edje, e->cfg.theme,
	                           "Embrace")) {
		fprintf (stderr, "Cannot load theme '%s'!\n", e->cfg.theme);
		return false;
	}

	if (e->gui.ee) {
		/* set min/max sizes */
		edje_object_size_max_get (e->gui.edje, &w, &h);
		ecore_evas_size_max_set (e->gui.ee, w, h);

		edje_object_size_min_get (e->gui.edje, &w, &h);
		ecore_evas_size_min_set (e->gui.ee, (int) w, (int) h);
		evas_object_resize (e->gui.edje, w, h);
		ecore_evas_resize (e->gui.ee, (int) w, (int) h);
	}

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

	if (!(e->gui.container = esmart_container_new (e->gui.evas)))
		return false;

	esmart_container_direction_set (e->gui.container, 1);
	esmart_container_spacing_set (e->gui.container, 0);
	esmart_container_fill_policy_set (e->gui.container,
	                                  CONTAINER_FILL_POLICY_FILL_X);

	edje_object_part_swallow (e->gui.edje, "Container",
	                          e->gui.container);

	evas_object_pass_events_set (e->gui.container, 1);
	evas_object_show (e->gui.container);

	return true;
}

static bool ui_load_trans_obj (Embrace *e)
{
	Evas_Object *trans;
	Ecore_X_Window win;
	const char *val;
	int w = 0, h = 0;

	assert (e);

	if (e->cfg.module)
		return true;

	if (!(val = edje_object_data_get (e->gui.edje, "trans_bg")))
		return true;

	if (strcmp (val, "1"))
		return true;

	if (!(trans = esmart_trans_x11_new (e->gui.evas)))
		return false;

	win = ecore_evas_software_x11_window_get (e->gui.ee);
	esmart_trans_x11_window_set (trans, win);

	ecore_evas_geometry_get (e->gui.ee, NULL, NULL, &w, &h);

	evas_object_move (trans, 0, 0);
	evas_object_resize (trans, w, h);
	evas_object_layer_set (trans, 0);
	evas_object_name_set (trans, "trans");
	evas_object_show (trans);

	return true;
}

static void on_dragger_mouse_up (void *data, Evas *evas,
                                 Evas_Object *o, void *ev)
{
	Embrace *e = data;

	ecore_evas_raise(e->gui.ee);
}

static bool ui_load_dragger (Embrace *e)
{
	Evas_Object *dragger;

	assert (e);

	if (e->cfg.module)
		return true;

	if (!(dragger = esmart_draggies_new (e->gui.ee)))
		return false;

	esmart_draggies_button_set (dragger, 1);

	evas_object_name_set (dragger, "dragger");
	evas_object_move (dragger, 0, 0);
	evas_object_show (dragger);

	esmart_draggies_event_callback_add (dragger,
	                                    EVAS_CALLBACK_MOUSE_UP,
	                                    on_dragger_mouse_up, e);

	return true;
}

static bool embrace_load_ui (Embrace *e)
{
	char path[PATH_MAX + 1];
	assert (e);

	if (!e->cfg.module) {
#ifdef HAVE_ECORE_EVAS_GL
		if (!strcasecmp (e->cfg.evas_engine, "gl"))
			e->gui.ee = ecore_evas_gl_x11_new (NULL, 0, 0, 0, 0, 0);
		else
#endif
			e->gui.ee = ecore_evas_software_x11_new (NULL, 0, 0, 0, 0, 0);

		if (!e->gui.ee)
			return false;

		ecore_evas_title_set (e->gui.ee, "Embrace");
		ecore_evas_name_class_set (e->gui.ee, "embrace", "Embrace");
		ecore_evas_borderless_set (e->gui.ee, true);

		e->gui.evas = ecore_evas_get (e->gui.ee);
	}

	embrace_expand_path ("~/.fonts", path, sizeof (path));
	evas_font_path_append (e->gui.evas, path);
	evas_font_path_append (e->gui.evas, "/usr/share/fonts");
	evas_font_path_append (e->gui.evas, "/usr/X11R6/lib/X11/fonts");

	if (!ui_load_dragger (e))
		return false;

	if (!ui_load_edje (e))
		return false;

	if (!ui_load_trans_obj (e))
		return false;

	if (!ui_load_container (e))
		return false;

	if (e->gui.ee) {
		ecore_evas_callback_pre_render_set (e->gui.ee, on_pre_render);
		ecore_evas_callback_post_render_set (e->gui.ee, on_post_render);
		ecore_evas_callback_delete_request_set (e->gui.ee, on_delete_request);
		ecore_evas_callback_resize_set (e->gui.ee, on_resize);

		if (evas_object_name_find (e->gui.evas, "trans"))
			ecore_evas_callback_move_set (e->gui.ee, on_move);
	}

	return true;
}

static int on_sighup (void *udata, int type, void *event)
{
	Embrace *e = udata;

	assert (e);

	embrace_stop (e);
	embrace_deinit (e);

	if (embrace_init (e))
		embrace_run (e);
	else
		ecore_main_loop_quit ();

	return 0;
}

Embrace *embrace_new (void)
{
	return calloc (1, sizeof (Embrace));
}

static void free_mailboxes (Embrace *e)
{
	MailBox *mb;

	assert (e);

	while (e->mailboxes) {
		mb = e->mailboxes->data;

		e->mailboxes = eina_list_remove (e->mailboxes, mb);
		mailbox_free (mb);
	}
}

static void free_plugins (Embrace *e)
{
	EmbracePlugin *ep;

	assert (e);

	while (e->plugins) {
		ep = e->plugins->data;

		e->plugins = eina_list_remove (e->plugins, ep);
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

	if (e->gui.ee) {
		ecore_evas_free (e->gui.ee);
		e->gui.ee = NULL;
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

	*e->cfg.evas_engine = 0;
	*e->cfg.theme = 0;
}

static bool handle_args (Embrace *e)
{
	int argc = 0, o;
	char **argv = NULL;
	bool ret = true;
	struct option opts[] = {{"help", no_argument, 0, 'h'},
	                        {"version", no_argument, 0, 'v'},
	                        {"engine", required_argument, 0, 'e'},
	                        {"theme", required_argument, 0, 't'},
	                        {NULL, 0, NULL, 0}};

	assert (e);
	if (e->cfg.module)
		return true;

	ecore_app_args_get (&argc, &argv);

	while ((o = getopt_long (argc, (char **) argv, "hve:t:",
	                         opts, NULL)) != -1) {
		switch (o) {
			case 'h':
				printf ("Usage: embrace [options]\n"
				        "Options:\n"
				        "  -v, --version       print version and exit\n"
				        "  -h, --help          print help and exit\n"
				        "  -e, --engine=name   specifiy engine to use\n"
				        "  -t, --theme=name    specify theme to use\n");
				ret = false;
				break;
			case 'v':
				printf ("Embrace " VERSION "\n");
				ret = false;
				break;
			case 'e':
				snprintf (e->cfg.evas_engine, sizeof (e->cfg.evas_engine),
				          "%s", optarg);
				break;
			case 't':
				snprintf (e->cfg.theme, sizeof (e->cfg.theme),
				          "%s", optarg);
				break;
		}
	}

	return ret;
}

bool embrace_init (Embrace *e)
{
	assert (e);

#ifdef SIGRTMIN
	last_signal = SIGRTMIN;
#endif

	embrace_config_init (e);

	if (!handle_args (e))
		return false;

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
	Eina_List *l;

	for (l = e->mailboxes; l; l = l->next)
		mailbox_check (l->data);

	/* only exec once */
	return 0;
}

void embrace_run (Embrace *e)
{
	assert (e);

	if (e->gui.ee)
		ecore_evas_show (e->gui.ee);

	ecore_idler_add (check_mailboxes, e);

	if (!e->cfg.module) {
		e->evt_hup = ecore_event_handler_add (ECORE_EVENT_SIGNAL_HUP,
		                                      on_sighup, e);
		assert (e->evt_hup);
	}
}

void embrace_stop (Embrace *e)
{
	assert (e);

	if (e->evt_hup)
		ecore_event_handler_del (e->evt_hup);

	e->evt_hup = NULL;
}

int embrace_signal_get (void)
{
#ifdef SIGRTMIN
	assert (last_signal >= SIGRTMIN);
	assert (last_signal < SIGRTMAX);

	return ++last_signal;
#else
	return -1;
#endif
}
