
#include <Ewl.h>

void
ewl_object_set_current_geometry(Ewl_Object * o, int x, int y, int w, int h)
{
	DCHECK_PARAM_PTR("o", o);

	o->current.x = x;
	o->current.y = y;
	o->current.w = w;
	o->current.h = h;
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

	o->current.w = w;
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
ewl_object_requested_geometry(Ewl_Object * o, int *x, int *y, int *w, int *h)
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
	o->minimum.h = h;
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

	o->maximum.w = w;
	o->maximum.h = h;
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

void
ewl_object_set_custom_size(Ewl_Object * o, int w, int h)
{
	DCHECK_PARAM_PTR("o", o);

	o->custom.w = w;
	o->custom.h = h;
	o->maximum.w = w;
	o->maximum.h = h;
	o->minimum.w = w;
	o->minimum.h = h;
}

void
ewl_object_get_custom_size(Ewl_Object * o, int *w, int *h)
{
	DCHECK_PARAM_PTR("o", o);

	if (w)
		*w = o->custom.w;
	if (h)
		*h = o->custom.h;
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
