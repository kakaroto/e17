#ifndef ENESIM_SCALER_H_
#define ENESIM_SCALER_H_

typedef void (*Enesim_Scaler_1D)(uint32_t *s, uint32_t sw, uint32_t x,
		uint32_t len, uint32_t dw, uint32_t *d);
EAPI void enesim_scaler_1d_register(Enesim_Cpu *cpu, Enesim_Scaler_1D scl,
		Enesim_Format sfmt, Enesim_Format dfmt);
#endif /* ENESIM_SCALER_H_ */
