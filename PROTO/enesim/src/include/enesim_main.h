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

/**
 * TODO every function that expects a color must be cnahed to this
 * type
 * The color is premultiplied!!!!!!
 */
//typedef uint32_t Enesim_Color;

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

/** Basic data type handlers */

typedef struct _Enesim_Format Enesim_Format;
/**
 * Representation of a span in a specific surface format
 */
typedef struct _Enesim_Surface_Data
{
	uint32_t *plane0;
	uint32_t *plane1;
	uint32_t *plane2;
	uint32_t *plane3;
	Enesim_Format *format;
} Enesim_Surface_Data;

/**
 * Representation of a pixel in a specific surface format
 */
typedef struct _Enesim_Surface_Pixel
{
	uint32_t plane0;
	uint32_t plane1;
	uint32_t plane2;
	uint32_t plane3;
	Enesim_Format *format;
} Enesim_Surface_Pixel;

//EAPI void enesim_color_get(Enesim_Color *color, uint8_t a, uint8_t r, uint8_t g, uint8_t b);

EAPI int enesim_init(void);
EAPI void enesim_shutdown(void);


/* TODO
 * remove this error handling and use eina's approach
 * extern Eina_Error ENESIM_ERROR_FORMAT;
 */
typedef enum
{
	ENESIM_ERROR_HANDLE_INVALID 	= 1,
	ENESIM_ERROR_SCANLINE_NOT_SUPPORTED,
	ENESIM_ERROR_GEOMETRY_INVALID,
	ENESIM_ERROR_FROMAT_NOT_SUPPORTED,
	ENESIM_ERROR_SRCRECT_INVALID,
	ENESIM_ERROR_DSTRECT_INVALID,
	ENESIM_ERROR_TRANSFORMATION_NOT_SUPPORTED,
} Enesim_Error;

/**
 *
 */
extern unsigned int enesim_err;

EAPI char * enesim_error_to_str(unsigned int err);

#endif /*ENESIM_CORE_H_*/
