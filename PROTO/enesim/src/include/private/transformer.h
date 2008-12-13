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
#ifndef TRANSFORMER_H_
#define TRANSFORMER_H_

typedef enum
{
	ENESIM_TRANSFORMATION_IDENTITY,
	ENESIM_TRANSFORMATION_AFFINE,
	ENESIM_TRANSFORMATION_PROJECTIVE,
	ENESIM_TRANSFORMATIONS 
} Enesim_Transformation_Type;

typedef enum
{
	ENESIM_SCALE_NO,
	ENESIM_SCALE_UP,
	ENESIM_SCALE_DOWN
} Enesim_Scale;

/* TODO override this from a drawer context
 * TODO rename to transformer context
 */
struct _Enesim_Transformation
{
#ifdef DEBUG
	Enesim_Magic magic;
#endif
	Enesim_Matrix *matrix;
	float ox;
	float oy;
	struct {
		int l, t, r, b;
		Eina_Bool used;	
	} border;
	Enesim_Rop rop;
	Enesim_Surface *mask;
	Enesim_Surface_Pixel *color;
	Enesim_Quality quality;
};


typedef void (*Enesim_Transformer_Func)(Enesim_Transformation *t,
		Enesim_Surface *ss, Eina_Rectangle *srect, Enesim_Surface *ds,
		Eina_Rectangle *drect);

/* 
 * TODO Later we can add [border][xscale][yscale]
 */
typedef struct _Enesim_Transformer
{
	Enesim_Transformer_Func mask[ENESIM_SURFACE_FORMATS][ENESIM_SURFACE_FORMATS][ENESIM_TRANSFORMATIONS][ENESIM_QUALITIES];
	Enesim_Transformer_Func normal[ENESIM_SURFACE_FORMATS][ENESIM_TRANSFORMATIONS][ENESIM_QUALITIES];
} Enesim_Transformer;

#endif /*TRANSFORMER_H_*/
