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
#ifndef EON_DOCUMENT_H_
#define EON_DOCUMENT_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_DOCUMENT_SIZE_CHANGED "sizeChanged"
#define EON_DOCUMENT_OBJECT_NEW "ObjectNew"

typedef struct _Eon_Document_Object_New
{
	Ekeko_Event ev;
	char *name;
} Eon_Document_Object_New;
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_DOCUMENT_SIZE;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Eon_Document * eon_document_new(const char *name, int w, int h, const char *options);
EAPI Eon_Layout * eon_document_layout_get(Eon_Document *d);
EAPI Ekeko_Object * eon_document_object_get_by_id(Eon_Document *d, const char *id);
EAPI void eon_document_size_get(Eon_Document *d, int *w, int *h);
EAPI void eon_document_resize(Eon_Document *d, int w, int h);
EAPI void eon_document_pause(Eon_Document *d);
EAPI void eon_document_play(Eon_Document *d);
EAPI Ekeko_Object * eon_document_object_new(Eon_Document *d, const char *name);

#endif /* EON_DOCUMENT_H_ */
