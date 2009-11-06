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
#ifndef EON_BUFFER_H_
#define EON_BUFFER_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_BUFFER_FORMAT_CHANGED "formatChanged"
#define EON_BUFFER_DATA_CHANGED "dataChanged"
#define EON_BUFFER_WIDTH_CHANGED "dwidthChanged"
#define EON_BUFFER_HEIGHT_CHANGED "dheightChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_BUFFER_FORMAT;
extern Ekeko_Property_Id EON_BUFFER_DATA;
extern Ekeko_Property_Id EON_BUFFER_WIDTH;
extern Ekeko_Property_Id EON_BUFFER_HEIGHT;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Buffer_Private Eon_Buffer_Private;
struct _Eon_Buffer
{
	Eon_Paint_Square parent;
	Eon_Buffer_Private *prv;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Eon_Buffer * eon_buffer_new(Eon_Document *d);

EAPI void eon_buffer_format_set(Eon_Buffer *b, Enesim_Converter_Format f);
EAPI Enesim_Converter_Format eon_buffer_format_get(Eon_Buffer *b);

EAPI void eon_buffer_data_set(Eon_Buffer *b, Enesim_Converter_Data *cdata);
EAPI Enesim_Converter_Data * eon_buffer_data_get(Eon_Buffer *b);

EAPI void eon_buffer_data_update(Eon_Buffer *b);

EAPI unsigned int eon_buffer_data_width_get(Eon_Buffer *b);
EAPI void eon_buffer_data_width_set(Eon_Buffer *b, unsigned int w);

EAPI unsigned int eon_buffer_data_height_get(Eon_Buffer *b);
EAPI void eon_buffer_data_height_set(Eon_Buffer *b, unsigned int h);

#endif /* EON_BUFFER_H_ */
