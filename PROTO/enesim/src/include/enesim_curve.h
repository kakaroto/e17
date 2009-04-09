#ifndef _ENESIM_CURVE_H
#define _ENESIM_CURVE_H

typedef void (*Enesim_Curve_Vertex_Add_Callback)(float x, float y, void *data);

void enesim_curve3_decasteljau_generate(float x1, float y1, float x2, float y2,
		float x3, float y3, Enesim_Curve_Vertex_Add_Callback cb, void *data);

void enesim_curve4_decasteljau_generate(float x1, float y1, float x2, float y2,
		float x3, float y3, float x4, float y4, Enesim_Curve_Vertex_Add_Callback
		cb, void *data);
#endif
