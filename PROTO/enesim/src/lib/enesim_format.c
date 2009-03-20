/* ENESIM - Direct Rendering Library
 * Copyright (C) 2007-2008 Jorge Luis Zapata
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
#include "Enesim.h"
#include "enesim_private.h"
/**
 * In Enesim every intermediate surface format is argb8888_pre, so all surface
 * modes should know how to convert to/from argb8888_pre
 *
 * To add a new surface format you should follow the next steps:
 * 1. Add a new entry to Enesim_Format enumaration in enesim_surface.h
 *    the format is TODO
 * 2. Create a new ENESIM_SURFACE_FORMAT(name, [yes | no]) entry into
 *    configure.in
 * 3. Create a data struct for that format in enesim_surface.h, every plane
 *    should be named as planeX, in case the pixel length is smaller than the
 *    data type and contiguous you should also create a planeX_pixel variable.
 * 4. Add your new data type to the Enesim_Surface_Data union, it should be of
 *    the same name as the format type
 * 5. Go to enesim_generator.c and follow the instructions on the beginning of
 *    the file
 * 6. Add a rule to generate the core headers in src/include/Makefile.am
 * 7. Add a rule to generate the drawer source files in src/lib/drawer/Makefile.am
 * 8. Add the drawer to the array of drawers in enesim_drawer.c, with the
 *    conditional building of course
 * 9. Add your includes in enesim_private.h in a similar way
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static int _num_formats = 0;
static Enesim_Format **_formats = NULL;

typedef struct _Enesim_Format_Iterator
{
	Eina_Iterator iterator;
	int curr;
} Enesim_Format_Iterator;

static inline Eina_Bool _component_compare(Enesim_Format_Component *a, Enesim_Format_Component *b)
{
	if (!a)
	{
		if ((b->length == 0) && (b->offset == 0) && (b->plane == 0))
			return EINA_TRUE;
		else
			return EINA_FALSE;
	}
	else
	{
		if ((b->length == a->length) && (b->offset == a->offset)
				&& (b->plane == a->plane))
			return EINA_TRUE;
		else
			return EINA_FALSE;
	}
}
static Eina_Bool _iterator_next(Enesim_Format_Iterator *it, void **data)
{
	Enesim_Format **f = (Enesim_Format **)data;

	if (!f)
		return EINA_FALSE;
	if (it->curr >= _num_formats)
		return EINA_FALSE;
	*f = _formats[it->curr++];
	return EINA_TRUE;
}
static void * _iterator_get_container(Enesim_Format_Iterator *it)
{
	return NULL;
}

static void _iterator_free(Enesim_Format_Iterator *it)
{
	free(it);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool enesim_format_register(Enesim_Format *f)
{
	Enesim_Format **tmp;

	/* check the correctness of the format */
	if (!f->name)
	{
		EINA_ERROR_PERR("Format hasn't set the name\n");
		return EINA_FALSE;
	}
	if (!f->create)
	{
		EINA_ERROR_PERR("Format %s doesn't implement the create() function\n", f->name);
		return EINA_FALSE;
	}
	if (!f->delete)
	{
		EINA_ERROR_PERR("Format %s doesn't implement the delete() function\n", f->name);
		return EINA_FALSE;
	}
	if (!f->argb_from)
	{
		EINA_ERROR_PERR("Format %s doesn't implement the argb_from() function\n", f->name);
		return EINA_FALSE;
	}
	if (!f->argb_to)
	{
		EINA_ERROR_PERR("Format %s doesn't implement the argb_to() function\n", f->name);
		return EINA_FALSE;
	}
	if (!f->increment)
	{
		EINA_ERROR_PERR("Format %s doesn't implement the increment() function\n", f->name);
		return EINA_FALSE;
	}
	if (!f->pixel_argb_from)
	{
		EINA_ERROR_PERR("Format %s doesn't implement the pixel_argb_from() function\n", f->name);
		return EINA_FALSE;
	}
	if (!f->pixel_argb_to)
	{
		EINA_ERROR_PERR("Format %s doesn't implement the pixel_argb_to() function\n", f->name);
		return EINA_FALSE;
	}
	if (!f->pixel_get)
	{
		EINA_ERROR_PERR("Format %s doesn't implement the pixel_get() function\n", f->name);
		return EINA_FALSE;
	}
	if (!f->pixel_set)
	{
		EINA_ERROR_PERR("Format %s doesn't implement the pixel_set() function\n", f->name);
		return EINA_FALSE;
	}
	tmp = realloc(_formats, (_num_formats + 1) * sizeof(Enesim_Format *));
	if (!tmp)
		return EINA_FALSE;
	_formats = tmp;
	_formats[_num_formats] = f;
	_num_formats++;

	EINA_ERROR_PINFO("Format %s registered\n", f->name);
	return EINA_TRUE;
}

void enesim_format_init(void)
{
	/* First initialize the argb8888 */
	enesim_format_argb8888_init();
	enesim_format_a8_init();
	/* Now the optional ones */
#ifdef BUILD_SURFACE_ARGB8888_UNPRE
	enesim_format_argb8888_unpre_init();
#endif

#ifdef BUILD_SURFACE_RGB565_XA5

#endif

#ifdef BUILD_SURFACE_RGB565_B1A3
#endif
}
void enesim_format_shutdown(void)
{
	/* free the _formats */
	free(_formats);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Enesim_Format ENESIM_FORMAT_ARGB8888;
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Format * enesim_format_get(Enesim_Format_Component *a,
		Enesim_Format_Component *r, Enesim_Format_Component *g,
		Enesim_Format_Component *b, Eina_Bool premul)
{
	int i;
	/* iterate over the list of formats and get one that matches
	 * the specified one
	 */
	for (i = 0; i < _num_formats; i++)
	{
		Enesim_Format *f = _formats[i];

		if ((_component_compare(a, &f->alpha)) &&
				(_component_compare(r, &f->red)) &&
				(_component_compare(g, &f->green)) &&
				(_component_compare(b, &f->blue)) &&
				(premul == f->premul))
			return f;
	}
	return NULL;
}

/**
 * Debug routine that returns the format of a surface
 * as a string
 */
EAPI const char * enesim_format_name_get(Enesim_Format *f)
{
	return f->name;
}

EAPI Eina_Iterator * enesim_format_iterator_new(void)
{
	Enesim_Format_Iterator *it;

	it = malloc(sizeof(Enesim_Format_Iterator));
	it->curr = 0;
	it->iterator.next = FUNC_ITERATOR_NEXT(_iterator_next);
	it->iterator.get_container = FUNC_ITERATOR_GET_CONTAINER(_iterator_get_container);
	it->iterator.free = FUNC_ITERATOR_FREE(_iterator_free);

	EINA_MAGIC_SET(&it->iterator, EINA_MAGIC_ITERATOR);
	return &it->iterator;
}
