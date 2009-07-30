/* EON - Canvas and Toolkit library
 * Copyright (C) 2008-2009 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ecore_Idle_Enterer *_idler = NULL;
static int _count = 0;

static int _emage_idler_cb(void *data)
{
	emage_dispatch();
	return 1;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI int eon_init(void)
{
	if (_count) goto done;
		eina_init();
		ecore_init();
		ekeko_init();
		emage_init();
		eon_value_init();
		eon_engine_init();
		eon_parser_init();
		eon_script_init();
		/* create the Emage idler */
		_idler = ecore_idle_enterer_add(_emage_idler_cb, NULL);
done:
	return ++_count;
}

EAPI int eon_shutdown(void)
{
	if (_count != 1) goto done;
		ecore_idle_enterer_del(_idler);
		eon_script_shutdown();
		eon_parser_shutdown();
		eon_engine_shutdown();
		eon_value_shutdown();
		emage_shutdown();
		ekeko_shutdown();
		ecore_shutdown();
		eina_shutdown();
done:
	return --_count;
}

EAPI void eon_loop(void)
{
	ecore_main_loop_begin();
}
