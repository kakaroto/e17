#include "curve.h"

#define curve_is_flat level > 2

static inline void midpoint_get(float c1, float c2, float *r)
{
	*r = (c1 + c2) / 2;                
}

static void curve3_recursive(float x1, float y1, float x2, float y2, float x3,
		float y3, int level, Enesim_Curve_Vertex_Add_Callback cb, void *data)
{
	float x12, y12, x23, y23, x123, y123;

	/* mid points */
	midpoint_get(x1, x2, &x12);
	midpoint_get(y1, y2, &y12);
	midpoint_get(x2, x3, &x23);
	midpoint_get(y2, y3, &y23);
	midpoint_get(x12, x23, &x123);
	midpoint_get(y12, y23, &y123);

	if (curve_is_flat)
	{
		/* line from x1, y1 to x3, y3 */
		cb(x1, y1, data);
		cb(x3, y3, data);
		return;
	}
		
	curve3_recursive(x1, y1, x12, y12, x123, y123, level + 1, cb, data); 
	curve3_recursive(x123, y123, x23, y23, x3, y3, level + 1, cb, data); 
}

void enesim_curve3_decasteljau_generate(float x1, float y1, float x2, float y2,
		float x3, float y3, Enesim_Curve_Vertex_Add_Callback cb, void *data)
{
	cb(x1, y1, data);
	curve3_recursive(x1, y1, x2, y2, x3, y3, 0, cb, data);
	cb(x3, y3, data);
}

void enesim_curve4_decasteljau_generate(float x1, float y1, float x2, float y2,
		float x3, float y3, float x4, float y4, Enesim_Curve_Vertex_Add_Callback
		cb, void *data)
{
	
}

