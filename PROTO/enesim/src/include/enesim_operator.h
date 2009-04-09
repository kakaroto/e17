#ifndef ENESIM_OPERATOR_H_
#define ENESIM_OPERATOR_H_

typedef struct _Enesim_Operator
{
	Enesim_Cpu *cpu;
	Enesim_Operator_Id id;
	void *cb;
} Enesim_Operator;

EAPI Eina_Bool enesim_operator_drawer_point(Enesim_Operator *op, uint32_t *d,
		uint32_t s, uint32_t color, uint32_t m);
EAPI Eina_Bool enesim_operator_drawer_span(Enesim_Operator *op, uint32_t *d,
		uint32_t len, uint32_t *s, uint32_t color, uint32_t *m);
EAPI Eina_Bool enesim_operator_converter_1d(Enesim_Operator *op, uint32_t *s,
		uint32_t len, void *dst);

#endif /* ENESIM_OPERATOR_H_ */
