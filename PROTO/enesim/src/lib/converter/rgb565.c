#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _2d_rgb565_argb8888_none(uint32_t *src, uint32_t sw, uint32_t sh,
		uint32_t spitch, uint16_t *dst, uint32_t dw, uint32_t dh,
		uint32_t dpitch)
{
	while (dh--)
	{
		uint16_t *ddst = dst;
		uint32_t *ssrc = src;
		uint32_t ddw = dw;
		while (ddw--)
		{
			*dst = ((*src & 0xf80000) >> 8) | ((*src & 0xfc00) >> 5)
					| ((*src & 0xf8) >> 3);
			ssrc++;
			ddst++;
		}
		dst += dpitch;
		src += spitch;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_converter_rgb565_init(Enesim_Cpu *cpu)
{
	unsigned int numcpu;
	Enesim_Cpu **cpus;

	cpus = enesim_cpu_get(&numcpu);
	/* TODO check if the cpu is the host */
	enesim_converter_2d_register(ENESIM_CONVERTER_2D(_2d_rgb565_argb8888_none), cpu, ENESIM_FORMAT_ARGB8888, ENESIM_ROTATOR_NONE, ENESIM_CONVERTER_RGB565);
}

