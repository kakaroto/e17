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
#ifndef ENESIM_CORE_H_
#define ENESIM_CORE_H_

typedef uint32_t Enesim_Color;
#define ENESIM_COLOR_FULL 0xffffffff

typedef uint8_t Enesim_Alpha;

typedef enum _Enesim_Angle
{
	ENESIM_ANGLE_0,
	ENESIM_ANGLE_90,
	ENESIM_ANGLE_180,
	ENESIM_ANGLE_270,
	ENESIM_ANGLES,
} Enesim_Angle;

/*
 *
 */
typedef enum
{
	ENESIM_BLEND, /* D = S + D(1 - Sa) */
	ENESIM_FILL, /* D = S */
	ENESIM_ROPS
} Enesim_Rop;

typedef enum
{
	ENESIM_GOOD,
	ENESIM_FAST,
	ENESIM_QUALITIES
} Enesim_Quality;

typedef enum _Enesim_Format
{
	ENESIM_FORMAT_NONE,
	ENESIM_FORMAT_ARGB8888,  /* argb32 */
	ENESIM_FORMAT_ARGB8888_SPARSE,  /* argb32 with alpha-sparse pixels */
	ENESIM_FORMAT_XRGB8888,  /* argb32 with alpha = 255 */
	ENESIM_FORMAT_A8, /* assume all a8 is sparse? */
	ENESIM_FORMATS
} Enesim_Format;

typedef enum _Enesim_Direction
{
	ENESIM_DIRECTION_NONE,
	ENESIM_DIRECTION_X,
	ENESIM_DIRECTION_Y,
	ENESIM_DIRECTION_XY,
	ENESIM_DIRECTIONS
} Enesim_Direction;

EAPI int enesim_init(void);
EAPI void enesim_shutdown(void);

EAPI const char * enesim_format_name_get(Enesim_Format f);

#endif /*ENESIM_MAIN_H_*/
