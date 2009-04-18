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
EAPI Eina_Bool enesim_operator_transformer_1d(Enesim_Operator *op, uint32_t *src,
		uint32_t spitch,  uint32_t sw, uint32_t sh,
		float ox, float oy,
		float xx, float xy, float xz,
		float yx, float yy, float yz,
		float zx, float zy, float zz,
		uint32_t dx, uint32_t dy, uint32_t dlen,
		uint32_t *dst);
EAPI Eina_Bool enesim_operator_raddist_1d(Enesim_Operator *op,
		uint32_t *src, uint32_t spitch, uint32_t sw, uint32_t sh,
		float x0, float y0, float r0, float scale,
		uint32_t dx, uint32_t dy, uint32_t dlen,
		uint32_t *dst);
#endif /* ENESIM_OPERATOR_H_ */
