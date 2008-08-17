#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Edata.h"
#include "Enesim.h"
#include "enesim_private.h"
#include "rasterizer.h"
#include "extender_int.h"

#define KIIA_SUBPIXEL_SHIFT 4
#define KIIA_SUBPIXEL_DATA unsigned short int
#define KIIA_SUBPIXEL_COUNT 16
#define	KIIA_FULL_COVERAGE 0xffff
#define KIIA_SUBPIXEL_COVERAGE(a) (_coverages[(a) & 0xff] + _coverages[((a) >> 8) & 0xff])
#define KIIA_COVERAGE_SHIFT 4

/* 
 * 16x16 n-queens pattern:
 * []##[][][][][][][][][][][][][][] 1
 * [][][][][][][][]##[][][][][][][] 8
 * [][][][]##[][][][][][][][][][][] 4
 * [][][][][][][][][][][][][][][]## 15
 * [][][][][][][][][][][]##[][][][] 11
 * [][]##[][][][][][][][][][][][][] 2
 * [][][][][][]##[][][][][][][][][] 6
 * [][][][][][][][][][][][][][]##[] 14
 * [][][][][][][][][][]##[][][][][] 10
 * [][][]##[][][][][][][][][][][][] 3
 * [][][][][][][]##[][][][][][][][] 7 
 * [][][][][][][][][][][][]##[][][] 12
 * ##[][][][][][][][][][][][][][][] 0
 * [][][][][][][][][]##[][][][][][] 9
 * [][][][][]##[][][][][][][][][][] 5
 * [][][][][][][][][][][][][]##[][] 13
*/


#define KIIA_SUBPIXEL_OFFSETS { \
	(1.0f/16.0f),\
	(8.0f/16.0f),\
	(4.0f/16.0f),\
	(15.0f/16.0f),\
	(11.0f/16.0f),\
	(2.0f/16.0f),\
	(6.0f/16.0f),\
	(14.0f/16.0f),\
	(10.0f/16.0f),\
	(3.0f/16.0f),\
	(7.0f/16.0f),\
	(12.0f/16.0f),\
	(0.0f/16.0f),\
	(9.0f/16.0f),\
	(5.0f/16.0f),\
	(13.0f/16.0f)\
}

#include "kiia.h"

/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static Enesim_Rasterizer_Func kiia16_func = {
	.vertex_add = ENESIM_RASTERIZER_VERTEX_ADD(_vertex_add),
	.generate   = ENESIM_RASTERIZER_GENERATE(_generate),
	.delete     = ENESIM_RASTERIZER_DELETE(_delete)
};

/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
Enesim_Rasterizer * enesim_rasterizer_kiia16_new(Enesim_Rectangle boundaries)
{
	Enesim_Rasterizer *r;
	r = _new(&kiia16_func, boundaries);
	
	return r;
}
