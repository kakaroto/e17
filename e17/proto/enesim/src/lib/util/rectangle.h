#ifndef _RECTANGLE_H_
#define _RECTANGLE_H_

typedef struct _Enesim_Rectangle
{
	int 	x;
	int 	y;
	int 	w;
	int 	h;
} Enesim_Rectangle;

static inline int
enesim_spans_intersect(int c1, int l1, int c2, int l2)
{
	return (!(((c2 + l2) <= c1) || (c2 >= (c1 + l1))));
}

static inline int
enesim_rectangles_intersect(Enesim_Rectangle *r1, Enesim_Rectangle *r2)
{
	return (enesim_spans_intersect(r1->x, r1->w, r2->x, r2->w) && enesim_spans_intersect(r1->y, r1->h, r2->y, r2->h));
}

static inline int
enesim_rectangle_hspan_inside(Enesim_Rectangle *r, int x, int l)
{
	
}

static inline int
enesim_rectangle_vspan_inside(Enesim_Rectangle *r, int y, int l)
{
	
}

static inline int
enesim_rectangle_xcoord_inside(Enesim_Rectangle *r, int x)
{
	return ((x >= r->x) && (x < (r->x + r->w)));
}

static inline int
enesim_rectangle_ycoord_inside(Enesim_Rectangle *r, int y)
{
	return ((y >= r->y) && (y < (r->y + r->h)));
}

static inline void
enesim_rectangle_rectangle_intersection_get(Enesim_Rectangle *d, Enesim_Rectangle *s)
{
	if (enesim_rectangles_intersect(d, s))
	{
		if (d->x < s->x)
		{
			d->x = s->x;
			d->w += d->x - s->x;
			if (d->w < 0)
				d->w = 0;
		}
		if ((d->x + d->w) > (s->x + s->w))
		{
			d->w = s->x +s->w - d->x;
			if (d->y < s->y)
			{
				d->h += d->y - s->y;
				d->y = s->y;
				if (d->h < 0)
					d->h = 0;
			
			}
			if ((d->y + d->h) > (s->y + s->h))
				d->h = s->y + s->h - d->y;
		}
	}
	else
	{
		d->w = 0;
		d->h = 0;
	}
}

static inline int
enesim_rectangle_is_empty(Enesim_Rectangle *r)
{
	return ((r->w < 1) || (r->h < 1));
}


static inline void
enesim_rectangle_coords_from(Enesim_Rectangle *r, int x, int y, int w, int h)
{
	r->x = x;
	r->y = y;
	r->w = w;
	r->h = h;
}

#endif /*_RECTANGLE_H_*/
