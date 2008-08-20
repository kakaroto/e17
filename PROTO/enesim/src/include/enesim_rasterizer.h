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
#ifndef ENESIM_RASTERIZER_H_
#define ENESIM_RASTERIZER_H_

/**
 * @defgroup Enesim_Rasterizer_Group Rasterizer
 * @{
 */

/* TODO
 * move all types of Scanlines into a common union
 * refactor the callback
 */

/**
 * To be documented
 * FIXME: To be fixed
 */
typedef struct _Enesim_Scanline_Alias
{
	int y;
	int x;
	int w;
} Enesim_Scanline_Alias;

/**
 * To be documented
 * FIXME: To be fixed
 */
typedef struct _Enesim_Scanline_Mask
{
	int y;
	int x;
	int w;
	uint8_t *coverages;
} Enesim_Scanline_Mask;

/**
 * 
 */
enum {
	ENESIM_SCANLINE_ALIAS  	    = (1 << 0),
	ENESIM_SCANLINE_ANTIALIAS   = (1 << 1),
	ENESIM_SCANLINE_MASK        = (1 << 2),
	ENESIM_SCANLINES
};

/**
 * 
 */
typedef void (*Enesim_Scanline_Callback)(void *sl, int type, void *data);

typedef struct _Enesim_Scanline Enesim_Scanline; /**< Scanline Handler */
EAPI Enesim_Scanline 	*enesim_scanline_alias_new(void);
EAPI void 		enesim_scanline_delete(Enesim_Scanline *sl);

/**
 * 
 */
typedef enum _Enesim_Rasterizer_Fill_Rule
{
	ENESIM_RASTERIZER_FILL_RULE_EVENODD,
	ENESIM_RASTERIZER_FILL_RULE_NONEZERO,
	ENESIM_RASTERIZER_FILL_RULES
} Enesim_Rasterizer_Fill_Rule;

typedef struct _Enesim_Rasterizer Enesim_Rasterizer; /**< Rasterizer Handler */

EAPI void enesim_rasterizer_vertex_add(Enesim_Rasterizer *r, float x, float y);
EAPI Eina_Bool enesim_rasterizer_generate(Enesim_Rasterizer *r, Enesim_Scanline_Callback cb, void *data);
EAPI void enesim_rasterizer_delete(Enesim_Rasterizer *r);
EAPI void enesim_rasterizer_fill_rule_set(Enesim_Rasterizer *r, Enesim_Rasterizer_Fill_Rule rule);

EAPI Enesim_Rasterizer * enesim_rasterizer_cpsc_new(Eina_Rectangle boundaries);

/**
 * 
 */
typedef enum
{
	ENESIM_RASTERIZER_KIIA_COUNT_8,
	ENESIM_RASTERIZER_KIIA_COUNT_16,
	ENESIM_RASTERIZER_KIIA_COUNT_32,
	ENESIM_RASTERIZER_KIIA_COUNTS
} Enesim_Rasterizer_Kiia_Count;

EAPI Enesim_Rasterizer * enesim_rasterizer_kiia_new(Enesim_Rasterizer_Kiia_Count count,
		Eina_Rectangle boundaries);

/** @} */ //End of Enesim_Rasterizer_Group



#endif /*ENESIM_RASTERIZER_H_*/
