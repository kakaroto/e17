
#include <Ewl.h>

void
ewl_object_init(Ewl_Object * o, int w, int h, Ewl_Fill_Policy fill,
		Ewl_Alignment align)
{
	DCHECK_PARAM_PTR("o", o);

	MIN_W(o) = 1;
	MIN_H(o) = 1;
	MAX_W(o) = 1 << 30;
	MAX_H(o) = 1 << 30;

	CURRENT_W(o) = REQUEST_W(o) = w;
	CURRENT_H(o) = REQUEST_H(o) = h;

	o->fill = fill;
	o->align = align;
}

void
ewl_object_set_current_geometry(Ewl_Object * o, int x, int y, int w, int h)
{
	DCHECK_PARAM_PTR("o", o);

	CURRENT_X(o) = x;
	CURRENT_Y(o) = y;
	CURRENT_W(o) = w;
	CURRENT_H(o) = h;

	if (CURRENT_W(o) < MIN_W(o))
		CURRENT_W(o) = MIN_W(o);
	else if (w > MAX_W(o))
		CURRENT_W(o) = MAX_W(o);

	if (CURRENT_H(o) < MIN_H(o))
		CURRENT_H(o) = MIN_H(o);
	else if (w > MAX_H(o))
		CURRENT_H(o) = MAX_H(o);
}

void
ewl_object_get_current_geometry(Ewl_Object * o,
				int *x, int *y, int *w, int *h)
{
	DCHECK_PARAM_PTR("o", o);

	if (x)
		*x = o->current.x;
	if (y)
		*y = o->current.y;
	if (w)
		*w = o->current.w;
	if (h)
		*h = o->current.h;
}

void
ewl_object_set_current_size(Ewl_Object * o, int w, int h)
{
	DCHECK_PARAM_PTR("o", o);

	if (w >= MIN_W(o) && w <= MAX_W(o))
		o->current.w = w;

	if (h >= MIN_H(o) && h <= MAX_H(o))
		o->current.h = h;
}

void
ewl_object_get_current_size(Ewl_Object * o, int *w, int *h)
{
	DCHECK_PARAM_PTR("o", o);

	if (w)
		*w = o->current.w;
	if (h)
		*h = o->current.h;
}

void
ewl_object_request_geometry(Ewl_Object * o, int x, int y, int w, int h)
{
	DCHECK_PARAM_PTR("o", o);

	o->request.x = x;
	o->request.y = y;
	o->request.w = w;
	o->request.h = h;
}

inline void
ewl_object_request_x(Ewl_Object * o, int x)
{
	DCHECK_PARAM_PTR("o", o);

	o->request.x = x;
}

inline void
ewl_object_request_y(Ewl_Object * o, int y)
{
	DCHECK_PARAM_PTR("o", o);

	o->request.y = y;
}

inline void
ewl_object_request_w(Ewl_Object * o, int w)
{
	DCHECK_PARAM_PTR("o", o);

	o->request.w = w;
}

inline void
ewl_object_request_h(Ewl_Object * o, int h)
{
	DCHECK_PARAM_PTR("o", o);

	o->request.h = h;
}

void
ewl_object_requested_geometry(Ewl_Object * o, int *x, int *y, int *w,
			      int *h)
{
	DCHECK_PARAM_PTR("o", o);

	if (x)
		*x = o->request.x;
	if (y)
		*y = o->request.y;
	if (w)
		*w = o->request.w;
	if (h)
		*h = o->request.h;
}

void
ewl_object_set_minimum_size(Ewl_Object * o, int w, int h)
{
	DCHECK_PARAM_PTR("o", o);

	o->minimum.w = w;
	if (CURRENT_W(o) < w)
		CURRENT_W(o) = w;

	o->minimum.h = h;
	if (CURRENT_H(o) < h)
		CURRENT_H(o) = h;
}

void
ewl_object_get_minimum_size(Ewl_Object * o, int *w, int *h)
{
	DCHECK_PARAM_PTR("o", o);

	if (w)
		*w = o->minimum.w;
	if (h)
		*h = o->minimum.h;
}

void
ewl_object_set_maximum_size(Ewl_Object * o, int w, int h)
{
	DCHECK_PARAM_PTR("o", o);

	if (w >= MIN_W(o))
		o->maximum.w = w;

	if (CURRENT_W(o) > w)
		CURRENT_W(o) = w;

	if (h >= MIN_H(o))
		o->maximum.h = h;

	if (CURRENT_H(o) > h)
		CURRENT_H(o) = h;
}

void
ewl_object_get_maximum_size(Ewl_Object * o, int *w, int *h)
{
	DCHECK_PARAM_PTR("o", o);

	if (w)
		*w = o->maximum.w;
	if (h)
		*h = o->maximum.h;
}

inline void
ewl_object_set_realized(Ewl_Object * o, int r)
{
	DCHECK_PARAM_PTR("o", o);

	o->realized = r;
}

inline int
ewl_object_is_realized(Ewl_Object * o)
{
	DCHECK_PARAM_PTR_RET("o", o, -1);

	return o->realized;
}

inline void
ewl_object_set_visible(Ewl_Object * o, int v)
{
	DCHECK_PARAM_PTR("o", o);

	o->visible = v;
}

int
ewl_object_is_visible(Ewl_Object * o)
{
	DCHECK_PARAM_PTR_RET("o", o, -1);

	return o->visible;
}

inline void
ewl_object_set_layer(Ewl_Object * o, int l)
{
	DCHECK_PARAM_PTR("o", o);

	o->layer = l;
}

inline int
ewl_object_get_layer(Ewl_Object * o)
{
	DCHECK_PARAM_PTR_RET("o", o, 0);

	return o->layer;
}

inline void
ewl_object_set_alignment(Ewl_Object * o, Ewl_Alignment align)
{
	DCHECK_PARAM_PTR("o", o);

	o->align = align;
}

inline void
ewl_object_set_fill_policy(Ewl_Object * o, Ewl_Fill_Policy fill)
{
	DCHECK_PARAM_PTR("o", o);

	o->fill = fill;
}

inline Ewl_Alignment
ewl_object_get_alignment(Ewl_Object * o)
{
	DCHECK_PARAM_PTR("o", o);

	return o->align;

}

inline Ewl_Fill_Policy
ewl_object_get_fill_policy(Ewl_Object * o)
{
	DCHECK_PARAM_PTR("o", o);

	return o->fill;
}
