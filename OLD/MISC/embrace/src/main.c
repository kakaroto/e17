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

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <ltdl.h>
#include <stdio.h>

#include "embrace.h"

#define BAIL_OUT() { \
	if (e) {                \
		embrace_deinit (e); \
		embrace_free (e);   \
	}                       \
	shutdown();             \
	return 1;               \
}

static bool init ()
{
	ecore_init ();
	ecore_evas_init ();
	edje_init ();
	edje_frametime_set (1.0 / 60.0);

	if (lt_dlinit ()) {
		fprintf (stderr, "Cannot initialize LTDL!\n");
		return false;
	}

	return true;
}

static void shutdown ()
{
	lt_dlexit ();

	edje_shutdown ();
	ecore_evas_shutdown ();
	ecore_shutdown ();
}

int main (int argc, const char **argv)
{
	Embrace *e = NULL;

	if (!init ())
		BAIL_OUT ();

	ecore_app_args_set (argc, argv);

	if (!(e = embrace_new ())) {
		fprintf (stderr, "Out of memory!\n");
		BAIL_OUT ();
	}

	if (!embrace_init (e))
		BAIL_OUT ();

	embrace_run (e);
	ecore_main_loop_begin ();

	/* we don't need to call embrace_stop() here, since the timers
	 * and the event handlers are free'd by ecore
	 */
	embrace_deinit (e);
	embrace_free (e);

	shutdown ();

	return 0;
}
