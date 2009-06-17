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
#include <ltdl.h>
#include <assert.h>

#include "embrace_plugin.h"

EmbracePlugin *embrace_plugin_new (const char *path)
{
	EmbracePlugin *ep;
	bool (*init)(EmbracePlugin *p);

	assert (path);

	if (!(ep = calloc (1, sizeof (EmbracePlugin))))
		return NULL;

	if (!(ep->handle = lt_dlopen (path))) {
		fprintf (stderr, "Cannot load plugin '%s': %s\n",
		         path, lt_dlerror ());
		embrace_plugin_free (ep);
		return NULL;
	}

	if (!(init = lt_dlsym (ep->handle, "embrace_plugin_init"))) {
		fprintf (stderr, "Cannot load plugin '%s': "
		         "cannot find init method!\n", path);
		embrace_plugin_free (ep);
		return NULL;
	}

	if (!(*init)(ep) || !ep->check) {
		fprintf (stderr, "Cannot load plugin '%s': "
		         "init method failed!\n", path);
		embrace_plugin_free (ep);
		return NULL;
	}

	return ep;
}

/**
 * Frees a plugin, calling the shutdown method if available.
 *
 * @param p
 */
void embrace_plugin_free (EmbracePlugin *ep)
{
	assert (ep);

	if (ep->shutdown)
		ep->shutdown ();

	if (ep->handle)
		lt_dlclose (ep->handle);

	free (ep);
}
