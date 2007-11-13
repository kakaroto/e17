#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Edata.h"
#include "Enesim.h"
#include "enesim_private.h"
#include "rasterizer.h"
#include "scanline.h"
#include "extender.h"
/*
 * Kiia Kallio Scanline edge-flag algorithm, check it out at:
 * http://mlab.uiah.fi/~kkallio/antialiasing/
 */

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

static const float offsets_8[8] = {
	(5.0f/8.0f),
	(0.0f/8.0f),
	(3.0f/8.0f),
	(6.0f/8.0f),
	(1.0f/8.0f),
	(4.0f/8.0f),
	(7.0f/8.0f)
};

static const float *offsets[ENESIM_RASTERIZER_KIIA_COUNTS] = {
	offsets_8,
	NULL,
	NULL,
};

enum
{
	KIIA_EDGE_DIRECTION_DOWN = -1,
	KIIA_EDGE_DIRECTION_UP = 1,
};

typedef struct _Kiia_Edge Kiia_Edge;
struct _Kiia_Edge
{
	int yfirst;
	int ylast;
	int direction;
	Kiia_Edge *next;
};

typedef struct _Kiia_Vertex
{
	float x;
	float y;
} Kiia_Vertex;

typedef struct _Kiia
{
	Enesim_Rasterizer *r;
	
	Kiia_Edge **table;
	Kiia_Vertex vertex_last;
	int vertex_pos;

	Enesim_Extender extender;
	
	float *subpixel_offsets;
	int subpixel_count;
	int subpixel_shift;
} Kiia;


/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static const unsigned char coverages[256] = {
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

static void _edge_add(Kiia *k, Kiia_Vertex *v)
{
	Kiia_Edge *e, *tmp;

	e = calloc(1, sizeof(Kiia_Edge));
	/* TODO clip to the rasterizer boundaries */
	/* insert the edge to the edge table */
	/* accumulate the maxy and miny */
	if (v->y > k->vertex_last.y)
	{
		e->yfirst = v->y;
		e->ylast = k->vertex_last.y;
		e->direction = KIIA_EDGE_DIRECTION_UP;
	}
	else
	{
		e->yfirst = k->vertex_last.y;
		e->ylast = v->y;
		e->direction = KIIA_EDGE_DIRECTION_DOWN;
	}
	tmp = k->table[e->yfirst];
	e->next = tmp;
	k->table[e->yfirst] = e;
	
	enesim_extender_add(&k->extender, e->yfirst, e->ylast);
}

static void _vertex_add(Kiia *k, float x, float y)
{
	Kiia_Vertex v;
	/* scale the vertices on y by subpixel count */
	/* translate the sampling point to pixel center */
	v.x = x + 0.5;
	v.y = (y * k->subpixel_count) - 0.5;
	/* in case this is the second vertex, add an edge */
	if (k->vertex_pos)
	{
		_edge_add(k, &v);
	}
	k->vertex_pos = (k->vertex_pos + 1) % 2;
	k->vertex_last = v;
}

static void _generate(Kiia *k, Enesim_Scanline *sl)
{
	/* TODO */
}

static void _delete(Kiia *k)
{
	int i;
	
	/* free all edges on the edge table */
	for (i = 0; i < k->r->boundaries.w; i++)
	{
			Kiia_Edge *e, *next;
			
			e = k->table[i];
			while (e)
			{
				next = e->next;
				free(e);
				e = next;
	  		}
	}
	free(k);
}

static Enesim_Rasterizer_Func kiia_func = {
	.vertex_add = ENESIM_RASTERIZER_VERTEX_ADD(_vertex_add),
	.generate   = ENESIM_RASTERIZER_GENERATE(_generate),
	.delete     = ENESIM_RASTERIZER_DELETE(_delete)
};

/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Rasterizer * enesim_rasterizer_kiia_new(Enesim_Rasterizer_Kiia_Count
		count, Enesim_Rectangle boundaries)
{
	Enesim_Rasterizer *r;
	Kiia *k;

	k = calloc(1, sizeof(Kiia));
	k->subpixel_offsets = offsets[count];
	k->subpixel_shift = count + 3; // 0 => 3, 1 => 4, 2 => 5
	k->subpixel_count = 1 << k->subpixel_shift;
	r = enesim_rasterizer_new(k, &kiia_func, boundaries);
	k->r = r;
	k->table = calloc(boundaries.w, sizeof(Kiia_Edge *));
	
	return r;
}
