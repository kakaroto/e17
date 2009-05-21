#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static inline Eina_Bool _queue_create(Enesim_Cpu_Queue *q, Enesim_Operator *op, Enesim_Operator_Id id)
{
	q->cb = op->cb;
	q->id = op->id;
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool enesim_operator_drawer_point(Enesim_Operator *op, uint32_t *d,
		uint32_t s, uint32_t color, uint32_t m)
{
	Enesim_Cpu_Queue q;

	if (!_queue_create(&q, op, ENESIM_OPERATOR_DRAWER_POINT))
		return EINA_FALSE;

	q.drawer.d = d;
	q.drawer.type.point.color = color;
	q.drawer.type.point.s = s;
	q.drawer.type.point.m = m;
	op->cpu->run(op->cpu, &q);

	return EINA_TRUE;
}

EAPI Eina_Bool enesim_operator_drawer_span(Enesim_Operator *op, uint32_t *d,
		uint32_t len, uint32_t *s, uint32_t color, uint32_t *m)
{
	Enesim_Cpu_Queue q;

	if (!_queue_create(&q, op, ENESIM_OPERATOR_DRAWER_SPAN))
		return EINA_FALSE;

	q.drawer.d = d;
	q.drawer.type.span.len = len;
	q.drawer.type.span.color = color;
	q.drawer.type.span.s = s;
	q.drawer.type.span.m = m;
	op->cpu->run(op->cpu, &q);

	return EINA_TRUE;
}

EAPI Eina_Bool enesim_operator_converter_1d(Enesim_Operator *op, uint32_t *native,
		uint32_t len, void *conv)
{
	Enesim_Cpu_Queue q;

	if (!_queue_create(&q, op, ENESIM_OPERATOR_CONVERTER1D))
		return EINA_FALSE;

	q.converter.src = native;
	q.converter.dst = conv;
	q.converter.type.dim1.len = len;
	op->cpu->run(op->cpu, &q);

	return EINA_TRUE;
}

EAPI Eina_Bool enesim_operator_scaler_1d(Enesim_Operator *op, uint32_t *s,
		uint32_t sw, uint32_t x, uint32_t len, uint32_t dw,
		uint32_t *d)
{
	Enesim_Cpu_Queue q;

	if (!_queue_create(&q, op, ENESIM_OPERATOR_SCALER1D))
		return EINA_FALSE;

	q.scaler.src = s;
	q.scaler.dst = d;
	q.scaler.type.dim1.len = len;
	q.scaler.type.dim1.x = x;
	q.scaler.type.dim1.sw = sw;
	q.scaler.type.dim1.dw = dw;
	op->cpu->run(op->cpu, &q);

	return EINA_TRUE;
}


EAPI Eina_Bool enesim_operator_transformer_1d(Enesim_Operator *op, uint32_t *src,
		uint32_t spitch,  uint32_t sw, uint32_t sh,
		float ox, float oy,
		float xx, float xy, float xz,
		float yx, float yy, float yz,
		float zx, float zy, float zz,
		uint32_t dx, uint32_t dy, uint32_t dlen,
		uint32_t *dst)
{
	Enesim_Cpu_Queue q;

	if (!_queue_create(&q, op, ENESIM_OPERATOR_TRANSFORMER1D))
		return EINA_FALSE;

	q.transformer.src = src;
	q.transformer.spitch = spitch;
	q.transformer.sw = sw;
	q.transformer.sh = sh;
	q.transformer.ox = ox;
	q.transformer.oy = oy;
	q.transformer.xx = xx;
	q.transformer.xy = xy;
	q.transformer.xz = xz;
	q.transformer.yx = yx;
	q.transformer.yy = yy;
	q.transformer.yz = yz;
	q.transformer.zx = zx;
	q.transformer.zy = zy;
	q.transformer.zz = zz;
	q.transformer.type.dim1.x = dx;
	q.transformer.type.dim1.y = dy;
	q.transformer.type.dim1.len = dlen;
	q.transformer.dst = dst;
	op->cpu->run(op->cpu, &q);

	return EINA_TRUE;
}

EAPI Eina_Bool enesim_operator_raddist_1d(Enesim_Operator *op,
		uint32_t *src, uint32_t spitch, uint32_t sw, uint32_t sh,
		float x0, float y0, float r0, float scale,
		uint32_t dx, uint32_t dy, uint32_t dlen,
		uint32_t *dst)
{
	Enesim_Cpu_Queue q;

	if (!_queue_create(&q, op, ENESIM_OPERATOR_RADDIST1D))
		return EINA_FALSE;

	q.raddist.src = src;
	q.raddist.spitch = spitch;
	q.raddist.sw = sw;
	q.raddist.sh = sh;
	q.raddist.x0 = x0;
	q.raddist.y0 = y0;
	q.raddist.r0 = r0;
	q.raddist.scale = scale;
	q.raddist.dst = dst;
	q.raddist.type.dim1.dx = dx;
	q.raddist.type.dim1.dy = dy;
	q.raddist.type.dim1.dlen = dlen;
	op->cpu->run(op->cpu, &q);

	return EINA_TRUE;
}

EAPI Eina_Bool enesim_operator_dispmap_1d(Enesim_Operator *op,
		uint32_t *src, uint32_t spitch, uint32_t sw, uint32_t sh,
		float scale, uint32_t *map,
		uint32_t dx, uint32_t dy, uint32_t dlen,
		uint32_t *dst)
{
	Enesim_Cpu_Queue q;

	if (!_queue_create(&q, op, ENESIM_OPERATOR_DISPMAP1D))
		return EINA_FALSE;

	q.dispmap.src = src;
	q.dispmap.spitch = spitch;
	q.dispmap.sw = sw;
	q.dispmap.sh = sh;
	q.dispmap.scale = scale;
	q.dispmap.dst = dst;
	q.dispmap.type.dim1.dx = dx;
	q.dispmap.type.dim1.dy = dy;
	q.dispmap.type.dim1.dlen = dlen;
	q.dispmap.type.dim1.map = map;
	op->cpu->run(op->cpu, &q);

	return EINA_TRUE;
}

