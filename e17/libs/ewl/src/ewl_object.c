
#include <Ewl.h>

void
ewl_object_set_current_geometry(Ewl_Object * o, int x, int y, int w, int h)
{
	CHECK_PARAM_POINTER("o", o);

	o->current.x = x;
	o->current.y = y;
	o->current.w = w;
	o->current.h = h;
}

void
ewl_object_get_current_geometry(Ewl_Object * o,
				int *x, int *y, int *w, int *h)
{
	CHECK_PARAM_POINTER("o", o);

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
	CHECK_PARAM_POINTER("o", o);

	o->current.w = w;
	o->current.h = h;
}

void
ewl_object_get_current_size(Ewl_Object * o, int *w, int *h)
{
	CHECK_PARAM_POINTER("o", o);

	if (w)
		*w = o->current.w;
	if (h)
		*h = o->current.h;
}

void
ewl_object_request_geometry(Ewl_Object * o, int x, int y, int w, int h)
{
	CHECK_PARAM_POINTER("o", o);

	o->request.x = x;
	o->request.y = y;
	o->request.w = w;
	o->request.h = h;
}

inline void
ewl_object_request_x(Ewl_Object * o, int x)
{
	CHECK_PARAM_POINTER("o", o);

	o->request.x = x;
}

inline void
ewl_object_request_y(Ewl_Object * o, int y)
{
	CHECK_PARAM_POINTER("o", o);

	o->request.y = y;
}

inline void
ewl_object_request_w(Ewl_Object * o, int w)
{
	CHECK_PARAM_POINTER("o", o);

	o->request.w = w;
}

inline void
ewl_object_request_h(Ewl_Object * o, int h)
{
	CHECK_PARAM_POINTER("o", o);

	o->request.h = h;
}

void
ewl_object_requested_geometry(Ewl_Object * o, int *x, int *y, int *w, int *h)
{
	CHECK_PARAM_POINTER("o", o);

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
	CHECK_PARAM_POINTER("o", o);

	o->minimum.w = w;
	o->minimum.h = h;
}

void
ewl_object_get_minimum_size(Ewl_Object * o, int *w, int *h)
{
	CHECK_PARAM_POINTER("o", o);

	if (w)
		*w = o->minimum.w;
	if (h)
		*h = o->minimum.h;
}

void
ewl_object_set_maximum_size(Ewl_Object * o, int w, int h)
{
	CHECK_PARAM_POINTER("o", o);

	o->maximum.w = w;
	o->maximum.h = h;
}

void
ewl_object_get_maximum_size(Ewl_Object * o, int *w, int *h)
{
	CHECK_PARAM_POINTER("o", o);

	if (w)
		*w = o->maximum.w;
	if (h)
		*h = o->maximum.h;
}

void
ewl_object_set_custom_size(Ewl_Object * o, int w, int h)
{
	CHECK_PARAM_POINTER("o", o);

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
	CHECK_PARAM_POINTER("o", o);

	if (w)
		*w = o->custom.w;
	if (h)
		*h = o->custom.h;
}

void
ewl_object_set_realized(Ewl_Object * o, int r)
{
	CHECK_PARAM_POINTER("o", o);

	o->realized = r;
}

void
ewl_object_get_realized(Ewl_Object * o, int *r)
{
	CHECK_PARAM_POINTER("o", o);

	if (r)
		*r = o->realized;
}

void
ewl_object_set_visible(Ewl_Object * o, int v)
{
	CHECK_PARAM_POINTER("o", o);

	o->visible = v;
}

void
ewl_object_get_visible(Ewl_Object * o, int *v)
{
	CHECK_PARAM_POINTER("o", o);

	if (v)
		*v = o->visible;
}

void
ewl_object_set_layer(Ewl_Object * o, int l)
{
	CHECK_PARAM_POINTER("o", o);

	o->layer = l;
}

int
ewl_object_get_layer(Ewl_Object * o)
{
	CHECK_PARAM_POINTER_RETURN("o", o, 0);

	return o->layer;
}
