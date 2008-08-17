#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Edata.h"
#include "Enesim.h"
#include "enesim_private.h"
#include "rasterizer.h"
#include "extender_int.h"

#define KIIA_SUBPIXEL_SHIFT 3
#define KIIA_SUBPIXEL_DATA unsigned char
#define KIIA_SUBPIXEL_COUNT 8
#define	KIIA_FULL_COVERAGE 0xff
#define KIIA_SUBPIXEL_COVERAGE(a) (_coverages[(a)])
#define KIIA_COVERAGE_SHIFT 5

/* 
 * 8x8 n-queens pattern
 * [][][][][]##[][] 5
 * ##[][][][][][][] 0
 * [][][]##[][][][] 3
 * [][][][][][]##[] 6
 * []##[][][][][][] 1
 * [][][][]##[][][] 4
 * [][][][][][][]## 7
 * [][]##[][][][][] 2
 */
#define KIIA_SUBPIXEL_OFFSETS { \
	(5.0f/8.0f),\
	(0.0f/8.0f),\
	(3.0f/8.0f),\
	(6.0f/8.0f),\
	(1.0f/8.0f),\
	(4.0f/8.0f),\
	(7.0f/8.0f) \
}

#include "kiia.h"

/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static Enesim_Rasterizer_Func kiia8_func = {
	.vertex_add = ENESIM_RASTERIZER_VERTEX_ADD(_vertex_add),
	.generate   = ENESIM_RASTERIZER_GENERATE(_generate),
	.delete     = ENESIM_RASTERIZER_DELETE(_delete)
};

/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
Enesim_Rasterizer * enesim_rasterizer_kiia8_new(Enesim_Rectangle boundaries)
{
	Enesim_Rasterizer *r;
	r = _new(&kiia8_func, boundaries);
	
	return r;
}
