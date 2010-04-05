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
/**
 * @todo
 * * We need a way to pass the object or retrieve the object from
 *   yacc, instead of having global variables
 *
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
extern FILE *yyin;
int yyparse(void);
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Array *eon_parser_stack = NULL;
Ekeko_Property_Id eon_parser_property_id = NULL;
int eon_parser_property_type = 0;
Eon_Document *eon_parser_doc = NULL;
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * @param doc The Document
 * @param o The root object
 * @param file The file to parse
 */
EAPI Eina_Bool eon_parser_load(Eon_Document *doc, Ekeko_Object **o, const char *file)
{
	Eon_Parser *p;

	yyin = fopen(file, "r");
	if (!yyin) return EINA_FALSE;

	eon_parser_doc = doc;
	eon_parser_stack = eina_array_new(1);

	yyparse();
	fclose(yyin);
	ekeko_object_dump(*o, ekeko_object_dump_printf);

	eina_array_free(eon_parser_stack);
	return EINA_TRUE;
}
