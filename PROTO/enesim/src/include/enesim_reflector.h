#ifndef ENESIM_REFLECTOR_H_
#define ENESIM_REFLECTOR_H_

typedef void (*Enesim_Reflector)(void *src, uint32_t x, uint32_t y, uint32_t len, void *dst);

EAPI void enesim_reflector_register(Enesim_Cpu *cpu, Enesim_Reflector rfl,
		Enesim_Format sfmt, Enesim_Direction dir,
		Enesim_Format dfmt);
EAPI Eina_Bool enesim_reflector_op_get(Enesim_Operator *op, Enesim_Cpu *cpu,
		Enesim_Format sfmt, Enesim_Direction dir, Enesim_Format dfmt);

#endif /* ENESIM_REFLECTOR_H_ */
