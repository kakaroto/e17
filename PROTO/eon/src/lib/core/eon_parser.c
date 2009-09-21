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
static Eina_Hash *_parsers = NULL;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_parser_init(void)
{
	_parsers = eina_hash_string_superfast_new(NULL);
#ifdef BUILD_PARSER_EXPAT
	parser_expat_init();
#endif
#ifdef BUILD_PARSER_EXML
	parser_exml_init();
#endif
}

void eon_parser_shutdown(void)
{
	/* TODO remove the hash */
}

void eon_parser_register(const char *name, Eon_Parser *p)
{
	printf("Parser with name %s registered\n", name);
	eina_hash_add(_parsers, name, p);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool eon_parser_load(Eon_Document *doc, Ekeko_Object **o, const char *file)
{
	Eina_Iterator *it;
	Eon_Parser *p;

	if (!_parsers)
		return;
	if (!o)
		return;

	it = eina_hash_iterator_data_new(_parsers);
	while (eina_iterator_next(it, (void **)&p))
	{
		if (p->file_load(doc, o, file))
		{
			break;
		}
	}
	eina_iterator_free(it);

	if (!o || !*o)
		return EINA_FALSE;

	ekeko_object_dump(*o, ekeko_object_dump_printf);

	return EINA_TRUE;
}
