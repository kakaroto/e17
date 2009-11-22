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
#ifndef EON_OBJECT_H_
#define EON_OBJECT_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EKEKO_OBJECT_ID_CHANGED "idChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_OBJECT_ID;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
#define EON_OBJECT(o) EKEKO_CAST(o, eon_object_type_get(), Eon_Object)
typedef struct _Eon_Object_Private Eon_Object_Private;
struct _Eon_Object
{
	Ekeko_Object base;
	Eon_Object_Private *prv;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type * eon_object_type_get(void);

EAPI void eon_object_id_set(Eon_Object *o, const char *id);
EAPI const char *eon_object_id_get(Eon_Object *o);

EAPI Eon_Document * eon_object_document_get(Eon_Object *o);

#endif /* EON_OBJECT_H_ */

