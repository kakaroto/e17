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
#ifndef EON_EXTERNAL_H_
#define EON_EXTERNAL_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_EXTERNAL_FILE_CHANGED "fileChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_EXTERNAL_FILE;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_External_Private Eon_External_Private;
struct _Eon_External
{
	Ekeko_Object parent;
	Eon_External_Private *private;
};

typedef struct _Eon_Parser
{
	Eina_Bool (*file_load)(Eon_Canvas *c, const char *file);
	Eina_Bool (*tree_get)(Eon_External *e, const char *file);
	Eina_Bool (*subtree_get)(Eon_External *e, const char *file);
} Eon_Parser;
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_external_type_get(void);
EAPI Eon_External * eon_external_new(Eon_Canvas *c);
EAPI void eon_external_file_set(Eon_External *e, const char *file);


#endif /* EON_EXTERNAL_H_ */
