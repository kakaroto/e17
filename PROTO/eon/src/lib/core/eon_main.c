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
static Eina_Hash *_types = NULL;

static int _emage_idler_cb(void *data)
{
	emage_dispatch();
	return 1;
}

/* register the common objects */
static void _objects_init(void)
{
	/* layouts */
	eon_canvas_init();
#if 0
	eon_stack_init();
#endif
	/* shapes */
#if 0
	eon_circle_init();
	eon_rect_init();
#endif
	/* paints */
	eon_checker_init();
#if 0
	eon_buffer_init();
	eon_compound_init();
	eon_compound_layer_init();
	eon_fade_init();
	eon_grid_init();
	eon_image_init();
	eon_stripes_init();
#endif
	/* core */
#if 0
	eon_style_init();
	eon_setter_init();
#endif
}

/* unregister the common objects */
static void _objects_shutdown(void)
{
#if 0
	/* layout */
	eon_canvas_shutdown();
	eon_stack_shutdown();
	/* shapes */
	eon_circle_shutdown();
	eon_rect_shutdown();
	/* paints */
	eon_buffer_shutdown();
	eon_checker_shutdown();
	eon_compound_shutdown();
	eon_compound_layer_shutdown();
	eon_fade_shutdown();
	eon_grid_shutdown();
	eon_image_shutdown();
	eon_stripes_shutdown();
	/* core */
	eon_setter_shutdown();
	eon_style_shutdown();
#endif
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* Initial code for the document object factory */
void eon_type_register(Ekeko_Type *t, char *name)
{
	eina_hash_add(_types, name, t);
}

void eon_type_unregister(Ekeko_Type *t)
{
	/* TODO */
}

Ekeko_Type * eon_type_get(char *name)
{
	return eina_hash_find(_types, name);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI int eon_init(void)
{
	if (_count) goto done;

	eina_init();
	ecore_init();
	etch_init();
	ekeko_init();
	emage_init();
	eon_value_init();
	eon_engine_init();
	eon_script_init();
	/* create the Emage idler */
	_idler = ecore_idle_enterer_add(_emage_idler_cb, NULL);
	/* the type registry */
	_types = eina_hash_string_superfast_new(NULL);
	_objects_init();
done:
	return ++_count;
}

EAPI int eon_shutdown(void)
{
	if (_count != 1) goto done;

	/* TODO delete the types hash */
	_objects_shutdown();
	ecore_idle_enterer_del(_idler);
	eon_script_shutdown();
	eon_engine_shutdown();
	eon_value_shutdown();
	emage_shutdown();
	ekeko_shutdown();
	etch_shutdown();
	ecore_shutdown();
	eina_shutdown();
done:
	return --_count;
}

EAPI void eon_loop(void)
{
	ecore_main_loop_begin();
}
