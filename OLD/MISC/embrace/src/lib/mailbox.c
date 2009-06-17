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
#include <Evas.h>

typedef struct {
	Evas_Object *edje;
	Evas_Coord width;
	Evas_Coord height;

	void *plugin;

	Evas_Hash *properties;
	int poll_interval;

	char title[64];
	int total;
	int unseen;
} MailBox;

#define __MAILBOX_C
#include "mailbox.h"
#include "embrace_plugin.h"

#if 0
static void on_edje_signal_exec (void *udata, Evas_Object *o,
                                 const char *emission, const char *src)
{
	MailBox *mb = udata;
	const char *cmd, *path;
	char tmp[PATH_MAX + 1];

	if (!(cmd = mailbox_command_get (mb)))
		return;

	if (!(path = mailbox_property_get (mb, "path")))
		return;

	snprintf (tmp, sizeof (tmp), cmd, path);

	ecore_exe_run (tmp, NULL);
}
#endif

static void on_edje_signal_force_check (void *udata, Evas_Object *o,
                                        const char *e, const char *s)
{
	mailbox_check (udata);
}

MailBox *mailbox_new (Evas *evas, const char *theme)
{
	MailBox *mb;

	if (!(mb = calloc (1, sizeof (MailBox))))
		return NULL;

	if (!(mb->edje = edje_object_add (evas))) {
		mailbox_free (mb);
		return NULL;
	}

	if (!edje_object_file_set (mb->edje, theme, "MailBox")) {
		fprintf (stderr, "Cannot load theme '%s'!\n", theme);
		mailbox_free (mb);
		return NULL;
	}

	mb->total = mb->unseen = -1;
	mailbox_total_set (mb, 0);
	mailbox_unseen_set (mb, 0);

	mailbox_poll_interval_set (mb, 60);

	edje_object_size_min_get (mb->edje, &mb->width, &mb->height);
	evas_object_resize (mb->edje, mb->width, mb->height);

#if 0
	edje_object_signal_callback_add (mb->edje, "EXEC", "*",
	                                 on_edje_signal_exec, mb);
#endif

	edje_object_signal_callback_add (mb->edje,
	                                 "MAILBOX_FORCE_CHECK", "*",
	                                 on_edje_signal_force_check, mb);

	evas_object_show (mb->edje);

	return mb;
}

void mailbox_free (MailBox *mb)
{
	assert (mb);

	if (mb->plugin)
		EMBRACE_PLUGIN(mb->plugin)->remove_mailbox (mb);

	if (mb->properties)
		evas_hash_free (mb->properties);

	if (mb->edje)
		evas_object_del (mb->edje);

	free (mb);
}

bool mailbox_load_config (MailBox *mb, E_DB_File *edb, char *key)
{
	assert (mb);
	assert (edb);
	assert (key);
	assert (mb->plugin);

	if (!EMBRACE_PLUGIN(mb->plugin)->load_config)
		return true;

	return (EMBRACE_PLUGIN(mb->plugin)->load_config (mb, edb, key));
}

void mailbox_emit_add (MailBox *mb)
{
	assert (mb);

	if (mb->plugin)
		EMBRACE_PLUGIN(mb->plugin)->add_mailbox (mb);
}

bool mailbox_check (MailBox *mb)
{
	assert (mb);
	assert (mb->plugin);
	assert (EMBRACE_PLUGIN(mb->plugin)->check);

	return EMBRACE_PLUGIN(mb->plugin)->check (mb);
}

const char *mailbox_title_get (MailBox *mb)
{
	assert (mb);

	return mb->title;
}

void mailbox_title_set (MailBox *mb, const char *title)
{
	assert (mb);

	snprintf (mb->title, sizeof (mb->title), "%s", title);
	edje_object_part_text_set (mb->edje, "MailBoxTitle", mb->title);
}

int mailbox_unseen_get (MailBox *mb)
{
	assert (mb);

	return mb->unseen;
}

void mailbox_unseen_set (MailBox *mb, int unseen)
{
	char buf[32], *sig;

	assert (mb);

	if (mb->unseen == unseen)
		return;

	mb->unseen = unseen;

	snprintf (buf, sizeof (buf), "%i unread", mb->unseen);
	edje_object_part_text_set (mb->edje, "MailBoxCountUnseen", buf);

	sig = unseen ? "MAILBOX_SET_DIRTY" : "MAILBOX_SET_DEFAULT";
	edje_object_signal_emit (mb->edje, sig, "Embrace");
}

int mailbox_total_get (MailBox *mb)
{
	assert (mb);

	return mb->total;
}

void mailbox_total_set (MailBox *mb, int total)
{
	char buf[32];

	assert (mb);

	if (mb->total == total)
		return;

	mb->total = total;

	snprintf (buf, sizeof (buf), "%i total", mb->total);
	edje_object_part_text_set (mb->edje, "MailBoxCountTotal", buf);
}

void *mailbox_plugin_get (MailBox *mb)
{
	assert (mb);

	return mb->plugin;
}

void mailbox_plugin_set (MailBox *mb, void *data)
{
	assert (mb);

	if (mb->plugin == data)
		return;

	mb->plugin = data;
}

Evas_Object *mailbox_edje_get (MailBox *mb)
{
	assert (mb);

	return mb->edje;
}

int mailbox_width_get (MailBox *mb)
{
	assert (mb);

	return mb->width;
}

int mailbox_height_get (MailBox *mb)
{
	assert (mb);

	return mb->height;
}

void *mailbox_property_get (MailBox *mb, char *key)
{
	assert (mb);
	assert (key);

	return evas_hash_find (mb->properties, key);
}

void mailbox_property_set (MailBox *mb, char *key, void *data)
{
	assert (mb);
	assert (key);

	/* if there's already data stored with the same key,
	 * remove that entry first.
	 */
	if (mb->properties && (evas_hash_find (mb->properties, key)))
		mb->properties = evas_hash_del (mb->properties, key, NULL);

	mb->properties = evas_hash_add (mb->properties, key, data);
}

int mailbox_poll_interval_get (MailBox *mb)
{
	assert (mb);

	return mb->poll_interval;
}

void mailbox_poll_interval_set (MailBox *mb, int interval)
{
	assert (mb);

	mb->poll_interval = interval;
}

void mailbox_is_checking_set (MailBox *mb, bool checking)
{
	char *sig[] = {"MAILBOX_SET_CHECKING", "MAILBOX_UNSET_CHECKING"};

	assert (mb);

	edje_object_signal_emit (mb->edje, sig[checking], "Embrace");
}
