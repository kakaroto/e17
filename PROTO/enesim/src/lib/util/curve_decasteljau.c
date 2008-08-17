#include "curve.h"
#include "fixed_16p16.h"

static inline void midpoint_get(enesim_16p16_t c1, enesim_16p16_t c2, enesim_16p16_t *r)
{
	*r = enesim_16p16_float_from(enesim_16p16_float_to(enesim_16p16_add(c1, c2)) / 2);
}

static void curve3_recursive(enesim_16p16_t x1, enesim_16p16_t y1, enesim_16p16_t x2,
		enesim_16p16_t y2, enesim_16p16_t x3, enesim_16p16_t y3, int level,
		Enesim_Curve_Vertex_Add_Callback cb, void *data)
{
	enesim_16p16_t x12, y12, x23, y23, x123, y123;
	enesim_16p16_t rx, ry;

	/* mid points */
	midpoint_get(x1, x2, &x12);
	midpoint_get(y1, y2, &y12);
	midpoint_get(x2, x3, &x23);
	midpoint_get(y2, y3, &y23);
	midpoint_get(x12, x23, &x123);
	midpoint_get(y12, y23, &y123);

	rx = abs(x1 + x3 - x2 - x2);
	ry = abs(y1 + y3 - y2 - y2);
	if ((rx + ry) <= (1 << 16))
	{
		cb(enesim_16p16_float_to(x123), enesim_16p16_float_to(y123), data);
		return;
	}
	
	curve3_recursive(x1, y1, x12, y12, x123, y123, level + 1, cb, data); 
	curve3_recursive(x123, y123, x23, y23, x3, y3, level + 1, cb, data); 
}

void enesim_curve3_decasteljau_generate(float x1, float y1, float x2, float y2,
		float x3, float y3, Enesim_Curve_Vertex_Add_Callback cb, void *data)
{
	cb(x1, y1, data);
	curve3_recursive(enesim_16p16_float_from(x1), enesim_16p16_float_from(y1),
			enesim_16p16_float_from(x2), enesim_16p16_float_from(y2), 
			enesim_16p16_float_from(x3), enesim_16p16_float_from(y3), 0, cb, data);
	cb(x3, y3, data);
}

void enesim_curve4_decasteljau_generate(float x1, float y1, float x2, float y2,
		float x3, float y3, float x4, float y4, Enesim_Curve_Vertex_Add_Callback
		cb, void *data)
{
	
}
