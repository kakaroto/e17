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
typedef enum _Enesim_Scanline_Type
{
	ENESIM_SCANLINE_ALIAS  	    = (1 << 0),
	ENESIM_SCANLINE_ANTIALIAS   = (1 << 1),
	ENESIM_SCANLINE_MASK        = (1 << 2),
	ENESIM_SCANLINES
} Enesim_Scanline_Type;

typedef struct _Enesim_Scanline
{
	Enesim_Scanline_Type type;
	union {
		Enesim_Scanline_Alias alias;
		Enesim_Scanline_Mask mask;
	} data;

} Enesim_Scanline /**< Scanline Handler */;

/**
 *
 */
typedef void (*Enesim_Scanline_Callback)(Enesim_Scanline *sl, void *data);

/**
 * @todo
 * - make each rasterizer to export what kind of scanlines it supports
 * - add a function/struct element to set up window boundaries i.e
 * destination surface/clip
 * - im not sure about using a rectangle for the boundaries, maybe only
 * width and height?
 * - support passing edges directly instead of vertices (useful for
 *   subpaths)
 * - Add a way to get the rasterizer implementation, i.e
 * void * enesim_rasterizer_implementation_get(Enesim_Rasterizer *r);
 * - Do we need to call the generate() directly? why not make that function
 * return a function pointer?
 * - Add magic checks
 * - Remove the data pointer from the rasterizer
 */

typedef struct _Enesim_Rasterizer Enesim_Rasterizer; /**< Rasterizer Handler */

/**
 *
 */
typedef enum _Enesim_Rasterizer_Fill_Rule
{
	ENESIM_RASTERIZER_FILL_RULE_EVENODD,
	ENESIM_RASTERIZER_FILL_RULE_NONEZERO,
	ENESIM_RASTERIZER_FILL_RULES
} Enesim_Rasterizer_Fill_Rule;

typedef enum _Enesim_Rasterizer_Fill_Policy
{
	ENESIM_RASTERIZER_FILL_POLICY_BORDER,
	ENESIM_RASTERIZER_FILL_POLICY_FILL,
} Enesim_Rasterizer_Fill_Policy;

EAPI void enesim_rasterizer_vertex_add(Enesim_Rasterizer *r, float x, float y);
EAPI Eina_Bool enesim_rasterizer_generate(Enesim_Rasterizer *r, Eina_Rectangle *rect, Enesim_Scanline_Callback cb, void *data);
EAPI void enesim_rasterizer_delete(Enesim_Rasterizer *r);
EAPI void enesim_rasterizer_reset(Enesim_Rasterizer *r);
/**
 * @defgroup Enesim_Rasterizer_Cpsc_Group Cpsc Rasterizer
 * @{
 */
EAPI Enesim_Rasterizer * enesim_rasterizer_cpsc_new(void);
/**
 * @}
 * @defgroup Enesim_Rasterizer_Kiia_Group Kiia Rasterizer
 * @{
 */
typedef struct _Enesim_Rasterizer_Kiia Enesim_Rasterizer_Kiia;
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

EAPI Enesim_Rasterizer * enesim_rasterizer_kiia_new(Enesim_Rasterizer_Kiia_Count count);
EAPI void enesim_rasterizer_kiia_count_set(Enesim_Rasterizer *k, Enesim_Rasterizer_Kiia_Count count);
EAPI void enesim_rasterizer_kiia_rule_set(Enesim_Rasterizer *r, Enesim_Rasterizer_Fill_Rule rule);
/**
 * @}
 * @defgroup Enesim_Rasterizer_Circle_Group Circle Rasterizer
 * @{
 */
EAPI Enesim_Rasterizer * enesim_rasterizer_circle_new(void);
EAPI void enesim_rasterizer_circle_fill_policy_set(Enesim_Rasterizer *c, Enesim_Rasterizer_Fill_Policy po);
EAPI void enesim_rasterizer_circle_radius_set(Enesim_Rasterizer *c, float radius);
/**
 * @}
 * @}
 */ //End of Enesim_Rasterizer_Group

#endif /*ENESIM_RASTERIZER_H_*/
