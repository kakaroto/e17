#include <stdlib.h>

#include "Edata.h"
#include "Enesim.h"
#include "enesim_private.h"
#include "scanline.h"

/**
 * Anti Aliased scanline
 *
 */

typedef struct _AAlias_Sl
{
	int y;
	int w;
	int a;
	int x0;
	int x1;
	int x2;
} AAlias_Sl;

typedef struct _AAlias
{
	Edata_Array 	*a;
	AAlias_Sl	*sls;
	int 		num_sls;
} AAlias;

static void _a_alloc(AAlias *n, int num)
{
	n->sls = realloc(n->sls, num * sizeof(AAlias_Sl));
}

static void _a_free(AAlias *n)
{
	free(n->sls);
}

static void _sl_free(AAlias *n)
{
	edata_array_free(n->a);
	free(n);
}

static void _sl_add(AAlias *n, int x0, int x1, int y, int coverage)
{
	edata_array_element_new(n->a);
	n->num_sls++;
}

Enesim_Scanline_Func naa = {
	.free	= _sl_free,
	.add 	= _sl_add
};

/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Scanline * enesim_scanline_aalias_new(void)
{
	Enesim_Scanline *sl;
	AAlias *n;

	n = calloc(1, sizeof(AAlias));
	n->a = edata_array_new(n, EDATA_ARRAY_ALLOC(_a_alloc),
		EDATA_ARRAY_FREE(_a_free));

	sl = enesim_scanline_new();
	sl->funcs = &naa;
	sl->anti_alias = 0;
	sl->data = n;
	return sl;
}
