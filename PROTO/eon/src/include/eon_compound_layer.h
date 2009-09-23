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
#ifndef EON_COMPOUND_LAYER_H_
#define EON_COMPOUND_LAYER_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_COMPOUND_LAYER_PAINT_CHANGED "paintChanged"
#define EON_COMPOUND_LAYER_ROP_CHANGED "ropChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Compound_Layer_Private Eon_Compound_Layer_Private;
struct _Eon_Compound_Layer
{
	Ekeko_Object parent;
	Eon_Compound_Layer_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type * eon_compound_layer_type_get(void);
EAPI Eon_Compound_Layer * eon_compound_layer_new(void);

#endif /* EON_COMPOUND_LAYER_H_ */
