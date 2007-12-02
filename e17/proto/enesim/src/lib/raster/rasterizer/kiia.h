#ifndef _KIIA_H
#define _KIIA_H

/*
 * Kiia Kallio Scanline edge-flag algorithm, check it out at:
 * http://mlab.uiah.fi/~kkallio/antialiasing/
 *
 * TODO:
 * - Support NonZero Rule
 */

/* TODO Move the fixed point thing to a common place */
#define FIXED_POINT int
#define FIXED_POINT_SHIFT 16
#define INT_TO_FIXED(a) ((a) << FIXED_POINT_SHIFT)
#define FLOAT_TO_FIXED(a) (int)((a)*((float)(1 << FIXED_POINT_SHIFT)))

#define FIXED_TO_INT(a) ((a) >> FIXED_POINT_SHIFT) 
#define FIXED_TO_FLOAT(a) ((float)(a)/((float)(1 << FIXED_POINT_SHIFT)))
#define RATIONAL_TO_FIXED(a) FLOAT_TO_FIXED(a)
#define FIXED_TO_RATIONAL(a) FIXED_TO_FLOAT(a)

#define SLOPE_FIX_SHIFT 8
#define SLOPE_FIX_STEP (1 << SLOPE_FIX_SHIFT)
#define SLOPE_FIX_MASK (SLOPE_FIX_STEP - 1)
#define SLOPE_FIX_SCANLINES (1 << (SLOPE_FIX_SHIFT - KIIA_SUBPIXEL_SHIFT))
#define SLOPE_FIX_SCANLINE_MASK (SLOPE_FIX_SCANLINES - 1)

enum
{
	KIIA_EDGE_DIRECTION_UP = 1,
	KIIA_EDGE_DIRECTION_DOWN = -1,
};

typedef struct _Kiia_Edge Kiia_Edge;
struct _Kiia_Edge
{
	int yfirst;
	int ylast;
	FIXED_POINT x;
	FIXED_POINT slope;
	FIXED_POINT slope_fix;
	int direction;
	Kiia_Edge *next; /* the next in the line */
	Kiia_Edge *gnext; /* the next while generating the AET */
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
	
	KIIA_SUBPIXEL_DATA *mask;
	DATA8 *coverages;
	Enesim_Scanline_Mask sl; /* FIXME is it needed ??? */
} Kiia;


static const FIXED_POINT _offsets[KIIA_SUBPIXEL_COUNT] = KIIA_SUBPIXEL_OFFSETS;

/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static const unsigned char _coverages[256] = {
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

static void _table_insert(Kiia *k, Kiia_Edge *e)
{
	Kiia_Edge *tmp;
	int line_first;
	int line_last;

	line_first = e->yfirst >> KIIA_SUBPIXEL_SHIFT;
	line_last = e->ylast >> KIIA_SUBPIXEL_SHIFT;
	/* insert the edge to the edge table */
	tmp = k->table[line_first];
	e->next = tmp;
	e->gnext = tmp;
	k->table[line_first] = e;
	
	/* accumulate the maxy and miny */
	enesim_extender_add(&k->extender, line_first, line_last);
	printf("edge added yfirst = %d, ylast = %d, x = %d, slope = %d slope_fix = %d\n", e->yfirst, e->ylast, e->x, e->slope, e->slope_fix);
}

static void _edge_add(Kiia *k, Kiia_Vertex *v)
{
	Kiia_Edge *e, *tmp;
	float ystart;
	float yend;
	float xstart;
	float xend;
	int direction;
	float slope;
	int first_line;
	int last_line;

	/* TODO clip to the rasterizer boundaries */
	if (k->vertex_last.y <= v->y)
	{
		xstart = k->vertex_last.x;
		ystart = k->vertex_last.y;
		xend = v->x;
		yend = v->y;
		direction = KIIA_EDGE_DIRECTION_UP;
	}
	else
	{
		xstart = v->x;
		ystart = v->y;
		xend = k->vertex_last.x;
		yend = k->vertex_last.y;
		direction = KIIA_EDGE_DIRECTION_DOWN;
	}
	
	/* check for a horizontal line */
	if (ystart == yend)
		return;
	first_line = (int)truncf(ystart) + 1;
	last_line = truncf(yend);

	/* create the new edge */
	e = calloc(1, sizeof(Kiia_Edge));
	slope = (xend - xstart) / (yend - ystart);
	e->slope = RATIONAL_TO_FIXED(slope);
	if (last_line - first_line >= SLOPE_FIX_STEP)
	{
		e->slope_fix = RATIONAL_TO_FIXED(slope * SLOPE_FIX_STEP) - (e->slope << SLOPE_FIX_SHIFT);
	}
	else
	{
		e->slope_fix = 0;
	}
	e->x = RATIONAL_TO_FIXED(xstart + (((float)first_line - ystart) * slope));
	e->direction = direction;
	e->yfirst = first_line;
	e->ylast = last_line;
	//printf("%f %f %f %f\n", xstart, xend, ystart, yend);
	_table_insert(k, e);
}

static void _vertex_add(Kiia *k, float x, float y)
{
	Kiia_Vertex v;
	
	/* scale the vertices on y by subpixel count */
	/* translate the sampling point to pixel center */
	v.x = x + (0.5/8.0);
	v.y = (y * KIIA_SUBPIXEL_COUNT) - 0.5;
	/* in case this is the second vertex, add an edge */
	if (k->vertex_pos)
	{
		_edge_add(k, &v);
	}
	else
	{
		k->vertex_pos = 1;
	}
	k->vertex_last = v;
}

static inline FIXED_POINT _dda(Kiia *k, FIXED_POINT xs, int ys, int ye, FIXED_POINT slope, KIIA_SUBPIXEL_DATA mask)
{
	FIXED_POINT x;
	int y;

	x = xs;

	for (y = ys; y <= ye; y++)
	{
		int xp =  FIXED_TO_INT((x + _offsets[y]));
		
		k->mask[xp] ^= mask;
		mask <<= 1;
		x += slope;	
	}

	return x;
}

static void _rasterize_edge(Kiia *k, Kiia_Edge **aet, Enesim_Extender *length,
		int curr_line)
{
	Kiia_Edge *curr_edge = *aet, *prev_edge = NULL;

	while (curr_edge)
	{
		int last_line = curr_edge->ylast >> KIIA_SUBPIXEL_SHIFT;
		if (last_line == curr_line)
		{
			int ye;
			int xe;
			FIXED_POINT x;

			ye = curr_edge->ylast & (KIIA_SUBPIXEL_COUNT - 1);
			x = _dda(k, curr_edge->x, 0, ye, curr_edge->slope, 1);
			xe = FIXED_TO_INT(x - curr_edge->slope);
		
			enesim_extender_add_sort(length, FIXED_TO_INT(curr_edge->x), xe);
			/* remove the edge from the AET */
			curr_edge = curr_edge->gnext;
			if (prev_edge)
				prev_edge->gnext = curr_edge;
			else
				*aet = curr_edge;
		}
		else
		{
			int xe;
			FIXED_POINT x;

			x = _dda(k, curr_edge->x, 0, KIIA_SUBPIXEL_COUNT - 1, curr_edge->slope, 1);
			xe = FIXED_TO_INT(x - curr_edge->slope);
			enesim_extender_add_sort(length, FIXED_TO_INT(curr_edge->x), xe);
			/* update the edge */
			if ((curr_line & SLOPE_FIX_SCANLINE_MASK) == 0)
				curr_edge->x = x + curr_edge->slope_fix;
			else
				curr_edge->x = x;
			/* next edges in AET */
			prev_edge = curr_edge;
			curr_edge = curr_edge->gnext;
		}
	}
	/* fetch edges from the edge table */
	curr_edge = k->table[curr_line];
	if (curr_edge)
	{
		k->table[curr_line] = NULL;
		do
		{
			int last_line = curr_edge->ylast >> KIIA_SUBPIXEL_SHIFT;

			if (last_line == curr_line)
			{
				int ye;
				int ys;
				int xe;
				FIXED_POINT x;
				ye = curr_edge->ylast & (KIIA_SUBPIXEL_COUNT - 1);
				ys = curr_edge->yfirst & (KIIA_SUBPIXEL_COUNT - 1);
				x = _dda(k, curr_edge->x, ys, ye, curr_edge->slope, 1 << ys);
				xe = FIXED_TO_INT(x - curr_edge->slope);
				enesim_extender_add_sort(length, FIXED_TO_INT(curr_edge->x), xe);
				/* ignore this edge */
			}
			else
			{
				int ys;
				int xe;
				FIXED_POINT x;
				ys = curr_edge->yfirst & (KIIA_SUBPIXEL_COUNT - 1);
				x = _dda(k, curr_edge->x, ys, KIIA_SUBPIXEL_COUNT - 1, curr_edge->slope, 1 << ys);
				xe = FIXED_TO_INT(x - curr_edge->slope);
				enesim_extender_add_sort(length, FIXED_TO_INT(curr_edge->x), xe);
				curr_edge->x = x;
				/* add the edge to the AET */
				if (prev_edge)
					prev_edge->gnext = curr_edge;
				else
					*aet = curr_edge;
				prev_edge = curr_edge;
			}
			curr_edge = curr_edge->gnext;

		} while (curr_edge);
	}
	if (prev_edge)
	{
		prev_edge->gnext = NULL;
	}
}

static void _generate(Kiia *k, int sl)
{
	int y;
	int ymin = k->extender.min;
	int ymax = k->extender.max;
	Enesim_Extender length; /* to keep track of min and max x */
	Kiia_Edge *edge = NULL;
	Enesim_Scanline_Alias alias;

	//printf("ymin %d ymax %d\n", ymin, ymax);
	for (y = 0; y <= ymax; y++)
	{
		int xmin;
		int xmax;
		KIIA_SUBPIXEL_DATA *mp;
		KIIA_SUBPIXEL_DATA *end;
		KIIA_SUBPIXEL_DATA mask;

		DATA8 *cov;

		enesim_extender_reset(&length);
		_rasterize_edge(k, &edge, &length, y);
		xmin = length.min;
		xmax = length.max + 1;
		
		if (xmin >= xmax)
		{
			continue;
		}
		mp = &k->mask[xmin];
		end = &k->mask[xmax + 1];
		*end = KIIA_FULL_COVERAGE;
		cov = k->coverages + xmin;
		k->sl.y = y;
		k->sl.x = xmin;
		k->sl.w = xmax - xmin + 1;
		k->sl.coverages = cov;
		mask = *mp;
		*mp++ = 0;
		while (mp <= end)
		{
			if (mask == 0)
			{
				do
				{
					mask = *mp++;
					*cov = 0;
					cov++;
				} while (mask == 0);
				mp[-1] = 0;
			}
			else if (mask == KIIA_FULL_COVERAGE)
			{
				/* FIXME here instead of sending a mask scanline
				 * send an alias */
				KIIA_SUBPIXEL_DATA tmp;
				do
				{
					tmp = *mp++;
					*cov = 255;
					cov++;
				} while (tmp == 0);
				mp[-1] = 0;
				mask ^= tmp;
			}
			else
			{
				/* FIXME here send a mask scanline directly */
				do
				{
					// TODO fix this!
					/* coverages from 0 to subpixel_count */
					*cov = KIIA_SUBPIXEL_COVERAGE(mask) << KIIA_COVERAGE_SHIFT;
					cov++;
					mask ^= *mp;
					*mp++ = 0;
				} while (!((mask == 0) || (mask == KIIA_FULL_COVERAGE)));
			}
		}
		k->r->scanline_callback(&k->sl, k->r->user_data);
	}
}

static void _delete(Kiia *k)
{
	int i;

	/* free all edges on the edge table */
	for (i = 0; i < k->r->boundaries.h; i++)
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
	free(k->table);
	free(k->mask);
	free(k->coverages);
	free(k);
}

static inline Enesim_Rasterizer * _new(Enesim_Rasterizer_Func *func, Enesim_Rectangle boundaries)
{
	Kiia *k;
	Enesim_Rasterizer *r;
		
	k = calloc(1, sizeof(Kiia)); 
	r = enesim_rasterizer_new(k, func, boundaries, ENESIM_SCANLINE_MASK);
	k->r = r;
	k->table = calloc(boundaries.h, sizeof(Kiia_Edge *));
	/* TODO explain the 3 */
	k->mask = calloc(boundaries.w + 3, sizeof(KIIA_SUBPIXEL_DATA));
	k->coverages = calloc(boundaries.w + 3, sizeof(DATA8));
	
	return r;
}

#endif /*_KIIA_H*/
