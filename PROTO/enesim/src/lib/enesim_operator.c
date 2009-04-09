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

EAPI Eina_Bool enesim_operator_converter_1d(Enesim_Operator *op, uint32_t *s,
		uint32_t len, void *dst)
{
	Enesim_Cpu_Queue q;

	if (!_queue_create(&q, op, ENESIM_OPERATOR_CONVERTER1D))
		return EINA_FALSE;

	q.converter.src = s;
	q.converter.dst = dst;
	q.converter.type.dim1.len = len;
	op->cpu->run(op->cpu, &q);

	return EINA_TRUE;
}
