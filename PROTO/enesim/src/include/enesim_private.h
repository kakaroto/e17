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
#ifndef _ENESIM_PRIVATE_H
#define _ENESIM_PRIVATE_H
 
/*
 * TODO replace all DATA32, DATA16, DATA8 with their standard definitions (inttypes.h)
 * TODO change fixed types from ints to int32_t (inttypes.h) 
 */
/** 
 * @defgroup Enesim_Internal_Group Internal Implementation
 * @{
 */

#include <string.h>
#include <limits.h>
#include <stdint.h>
/* TODO remove all assert statements */
#include <assert.h>

#include "config.h"

#define DEBUG
/* Magic values for each system */
typedef enum
{
	ENESIM_MAGIC  = 0x10000000,
	ENESIM_RENDERER_MAGIC = 0x11000000,
	ENESIM_RENDERER_FILLCOLOR_MAGIC = 0x11100000,
	ENESIM_RENDERER_FILLSURFACE_MAGIC = 0x11200000,
	ENESIM_RASTERIZER_MAGIC = 0x12000000,
	ENESIM_RASTERIZER_KIIA_MAGIC = 0x12100000,
	ENESIM_RASTERIZER_CPSC_MAGIC = 0x12200000,
	ENESIM_SURFACE_MAGIC = 0x13000000,
	ENESIM_TRANSFORMATION_MAGIC = 0x14000000
} Enesim_Magic;

/* Debugging routines
 * ASSERT will abort the program execution
 * ERROR will set the error and return false
 */
#ifndef DEBUG
#define ENESIM_MAGIC_CHECK(p, m)
#define ENESIM_MAGIC_SET(p, m)
#define ENESIM_ASSERT(cond, err)
#define ENESIM_ERROR(err) \
	/* enesim_error_set(err);*/  \
	return EINA_FALSE; \
#define ENESIM_ERROR_COND(err, cond) \
	if (!(cond)) \
	{ \
		ENESIM_ERROR(err);
	}
#else

#define ENESIM_MAGIC_SET(p, m) \
	p->magic = m;
#define ENESIM_MAGIC_CHECK(p, m) \
	if (p->magic != m) \
	{ \
		/*fprintf(stderr, "[Enesim] Magic Failed. %s at %s:%d - %s():\n", enesim_error_to_str(ENESIM_ERROR_HANDLE_INVALID), __FILE__, __LINE__, __FUNCTION__); */ \
		abort(); \
	}

#define ENESIM_ERROR(err) \
	/* fprintf(stderr, "[Enesim] %s at %s:%d - %s():\n", enesim_error_to_str(err), __FILE__, __LINE__, __FUNCTION__); */ \
	/* enesim_error_set(err); */ \
	return EINA_FALSE; \

#define ENESIM_ERROR_COND(err, cond) \
	if (!(cond)) \
	{ \
		ENESIM_ERROR(err) \
	}
#define ENESIM_ASSERT(cond, err) \
	if (!(cond)) \
	{ \
		/* fprintf(stderr, "[Enesim] %s at %s:%d - %s():\n", enesim_error_to_str(err), __FILE__, __LINE__, __FUNCTION__); */ \
		abort(); \
	}
#endif

#ifdef MIN
#undef MIN
#endif
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* FIXME move the above to other place */

/**
 * To be documented
 * FIXME: To be fixed
 */
static inline int enesim_hline_cut(int x, int *w, int *rx, int *rw, int cx)
{

	if ((x <= cx) && (x + *w > cx))	
	{
		int x2;

		x2 = x + *w;
		*w = cx - x;
		*rx = cx;
		*rw = x2 - cx;
		return 1;
	}
	return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#ifndef _ISOC99_SOURCE // truncf support
#define _ISOC99_SOURCE
#endif

#include <math.h>
/* SIMD intrinsics */
#ifdef __MMX__
#include <mmintrin.h>
typedef __m64 mmx_t;
#endif

#ifdef  __SSE__
#include <xmmintrin.h>
#endif

#ifdef __SSE2__
#include <emmintrin.h>
typedef __m128i sse2_t;
#endif



#include "extender_int.h"
// #include "extender_float.h"
#include "private/curve.h"
#include "private/drawer.h"
#include "private/matrix.h"
#include "private/rasterizer.h"
#include "private/renderer.h"
#include "private/surface.h"
#include "private/transformer.h"
/* now the surface format backends */
#include "private/surface_argb8888_core.h"
#include "private/surface_argb8888_rop.h"
/* common rgb565 code in case some other uses it */
#include "private/surface_rgb565_rop.h"

#ifdef BUILD_SURFACE_ARGB8888_UNPRE
#include "private/surface_argb8888_unpre_core.h"
#include "private/surface_argb8888_unpre_rop.h"
#endif


#ifdef BUILD_SURFACE_RGB565_XA5
#include "private/surface_rgb565_xa5_core.h"
#include "private/surface_rgb565_xa5_rop.h"
#endif


#ifdef BUILD_SURFACE_RGB565_B1A3
#include "private/surface_rgb565_b1a3_core.h"
#include "private/surface_rgb565_b1a3_rop.h"
#endif

/** @} */
#endif
