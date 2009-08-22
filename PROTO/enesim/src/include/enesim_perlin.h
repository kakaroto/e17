#ifndef ENESIM_PERLIN_H_
#define ENESIM_PERLIN_H_

EAPI Eina_F16p16 enesim_perlin_get(Eina_F16p16 xx, Eina_F16p16 yy,
	unsigned int octaves, Eina_F16p16 *xfreq, Eina_F16p16 *yfreq,
	Eina_F16p16 *ampl);
EAPI void enesim_perlin_coeff_set(unsigned int octaves, float persistence,
	float xfreq, float yfreq, float amplitude, Eina_F16p16 *xfreqcoeff,
	Eina_F16p16 *yfreqcoeff, Eina_F16p16 *amplcoeff);

#endif /*ENESIM_PERLIN_H_*/
