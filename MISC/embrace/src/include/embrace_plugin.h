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

#ifndef __EMBRACE_PLUGIN_H
#define __EMBRACE_PLUGIN_H

#include "mailbox.h"
#include "embrace.h"

#define EMBRACE_PLUGIN(x) ((EmbracePlugin *) (x))

typedef struct {
	void *handle;
	char name[64];

	bool (*check) (MailBox *mb);
	bool (*load_config) (MailBox *mb, E_DB_File *edb, const char *root);

	bool (*add_mailbox) (MailBox *mb);
	bool (*remove_mailbox) (MailBox *mb);

	void (*shutdown) ();
} EmbracePlugin;

EmbracePlugin *embrace_plugin_new (const char *name);
void embrace_plugin_free (EmbracePlugin *ep);

#endif
