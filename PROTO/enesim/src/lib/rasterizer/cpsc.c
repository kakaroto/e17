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
#include "enesim_common.h"
#include "Enesim.h"
#include "enesim_private.h"

/* Code based on the algorithm "Concave Polygon Scan Conversion" by 
 * Paul Heckbert from "Graphics Gems". 
 * 
 * This doesnt support anti-aliasing.
 */

typedef struct _Cpsc_Vertex
{
	float 	x;
	float 	y;
	int 	i;
} Cpsc_Vertex;

typedef struct _Cpsc_Edge
{
	/* x coordinate of edge intersection with current scanline */
	float 	x;
	/* change in x respect to y */
	float 	dx;
	/* edge number */
	int 	i;
} Cpsc_Edge;


typedef struct _Cpsc
{
	Eina_Array	*a;
	Enesim_Rasterizer *r;
	Cpsc_Vertex 	*vertices;
	int 		num_vertices;
} Cpsc;


/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static void _a_alloc(Cpsc *c, int num)
{
	c->vertices = realloc(c->vertices, num * sizeof(Cpsc));
}

static void _a_free(Cpsc *c)
{
	free(c->vertices);
}

/* append edge i to list of edges */
static inline void
_edge_add(Cpsc_Edge *edges, int *nedges, Cpsc_Vertex *vertices, int nvertices, int i, int y)
{
	Cpsc_Vertex *p, *q;
	int n = *nedges;
	int j;
	float dx;

	j = i < nvertices - 1 ? i + 1 : 0;
	if (vertices[i].y < vertices[j].y)
	{
		p = &vertices[i];
		q = &vertices[j];
	}
	else
	{
		p = &vertices[j];
		q = &vertices[i];
	}
	/* intersection of edge with scanline y */
	dx = (q->x - p->x) / (q->y - p->y);
	edges[n].dx = dx;
	edges[n].x = dx * (y + 0.5 - p->y) + p->x;
	edges[n].i = i;
	(*nedges)++;
}

/* remove edge i from list of edges */
static inline void
_edge_del(Cpsc_Edge *edges, int *nedges, int nvertices, int i)
{
	int n = *nedges;
	int j;

	for (j = 0; (j < n) && (edges[j].i != i); j++);
	/* edge on ative list */
	if (j < n)
	{
		(*nedges)--;
		memmove(&(edges[j]), &(edges[j + 1]), (*nedges - j) * sizeof(Cpsc_Edge));
	}
}

static int _compare_vertex(const void *a, const void *b)
{
	Cpsc_Vertex *p, *q;

	p = (Cpsc_Vertex *)a;
	q = (Cpsc_Vertex *)b;
	if (p->y <= q->y) return -1;
	return 1;
}

static int _compare_edge(const void *a, const void *b)
{
	Cpsc_Edge *p, *q;

	p = (Cpsc_Edge *)a;
	q = (Cpsc_Edge *)b;
	if (p->x <= q->x) return -1;
	return 1;
}

static void _vertex_add(Cpsc *r, float x, float y)
{
	int n = r->num_vertices;

	eina_lalloc_element_add(r->a);
	r->vertices[n].x = x;
	r->vertices[n].y = y;
	r->vertices[n].i = n;
	r->num_vertices++;
}

static void _generate(Cpsc *r)
{
	Enesim_Scanline_Alias alias;
	Cpsc_Vertex 	*vertices;
	Cpsc_Edge 	*aet;
	int 		*sindex;
	int 		y0, y1, y, i, j, k;
	int 		nedges;
	int 		n = r->num_vertices;
	
	if (n < 3) return;

	aet = malloc(sizeof(Cpsc_Edge) * n);
	nedges = 0;

	/* FIXME do we have to work on a copy ? */
	vertices = malloc(sizeof(Cpsc_Vertex) * n);
	memcpy(vertices, r->vertices, sizeof(Cpsc_Vertex) * n);

	/* create the sorted index array */
	sindex = malloc(sizeof(int) * n);
	for (i = 0; i < n; i++)
	{
		sindex[i] = i;
	}
	/* sort index by vertex.y */
	qsort(vertices, n, sizeof(Cpsc_Vertex), _compare_vertex);
	for (i = 0; i < n; i++)
	{
		sindex[i] = vertices[i].i;
	}
	free(vertices);
	vertices = r->vertices;
	/* get the min and max y from the polygon and the display size */
	y0 = ceil(vertices[sindex[0]].y - 0.5);
	y1 = floor(vertices[sindex[n - 1]].y - 0.5);

	k = 0;
	/* for every scanline */
	for (y = y0; y <= y1; y++)
	{
		/* check vertices between previous and current scanline */
		for (; (k < n) && (vertices[sindex[k]].y <= ((float)y + 0.5)); k++)
		{
			i = sindex[k];

			/* vertex previous to i */
			j = i > 0 ? (i -1) : (n - 1);
			if (vertices[j].y <= ((float)y - 0.5))
			{
				_edge_del(aet, &nedges, n, j);
			}
			else if (vertices[j].y > ((float)y + 0.5))
			{
				_edge_add(aet, &nedges, vertices, n, j, y);
			}
			/* vertex next to i */
			j = (i < (n - 1)) ? i + 1 : 0;
			if (vertices[j].y <= ((float)y - 0.5))
			{
				_edge_del(aet, &nedges, n, i);
			}
			else if (vertices[j].y > ((float)y + 0.5))
			{
				_edge_add(aet, &nedges, vertices, n, i, y);
			}
		}
		/* sort active edge table by x */
		qsort(aet, nedges, sizeof(Cpsc_Edge), _compare_edge);
		/* store horizontal segments */
		for (j = 0; j < nedges; j += 2)
		{
			int xl, xr;

			/* span from j to j+1 inside, from j+1 to j+2 outside */
			//printf("aet j = %f j + 1 = %f\n", aet[j].x , aet[j+1].x); 
			xl = ceil(aet[j].x - 0.5);
			xr = floor(aet[j + 1].x - 0.5);
			if (xl <= xr)
			{
				/* append a new scanline from xl to xr at y */
				alias.y = y;
				alias.x = xl;
				alias.w = xr - xl + 1;
				r->r->scanline_callback(&alias, ENESIM_SCANLINE_ALIAS, r->r->user_data);
			}
			aet[j].x += aet[j].dx;
			aet[j + 1].x += aet[j + 1].dx;
		}
	}
	free(aet);
	free(sindex);
}

static void _delete(Cpsc *c)
{
	free(c);
}

static Enesim_Rasterizer_Func cpsc_func = {
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
EAPI Enesim_Rasterizer * enesim_rasterizer_cpsc_new(Eina_Rectangle boundaries)
{
	Enesim_Rasterizer *r;
	Cpsc *c;

	c = calloc(1, sizeof(Cpsc));
	c->a = eina_lalloc_new(c, EINA_LALLOC_ALLOC(_a_alloc),
		EINA_LALLOC_FREE(_a_free), 0);
	r = enesim_rasterizer_new(c, &cpsc_func, boundaries, ENESIM_SCANLINE_ALIAS);
	c->r = r;
	return r;
}
