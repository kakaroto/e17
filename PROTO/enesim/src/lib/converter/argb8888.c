#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _2d_argb8888_argb8888_none(uint32_t *src, uint32_t sw, uint32_t sh,
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
			uint8_t pa;

			pa = (*ssrc >> 24);
			if ((pa > 0) && (pa < 255))
			{
				*ddst = (pa << 24)|
					(((argb8888_red_get(*ssrc) * 255) / pa) << 16) |
					(((argb8888_green_get(*ssrc) * 255) / pa) << 8) |
					((argb8888_blue_get(*ssrc) * 255) / pa);
			}
			else
			{
				*ddst = *ssrc;
			}

			ssrc++;
			ddst++;
		}
		dst += dpitch;
		src += spitch;
	}
}
static void _1d_argb8888_argb8888_none(uint32_t *src, uint32_t len, void *dst)
{
	uint32_t *ddst = dst;

	while (len--)
	{
		uint8_t pa;

		pa = (*src >> 24);
		if ((pa > 0) && (pa < 255))
		{
			*ddst = (pa << 24)|
				(((argb8888_red_get(*src) * 255) / pa) << 16) |
				(((argb8888_green_get(*src) * 255) / pa) << 8) |
				((argb8888_blue_get(*src) * 255) / pa);
		}
		else
		{
			*ddst = *src;
		}
		ddst++;
		src++;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_converter_argb8888_init(Enesim_Cpu *cpu)
{
	/* TODO check if the cpu is the host */
	enesim_converter_2d_register(ENESIM_CONVERTER_2D(_2d_argb8888_argb8888_none), cpu, ENESIM_FORMAT_ARGB8888, ENESIM_ROTATOR_NONE, ENESIM_CONVERTER_ARGB888);
	enesim_converter_1d_register(ENESIM_CONVERTER_1D(_1d_argb8888_argb8888_none), cpu, ENESIM_FORMAT_ARGB8888, ENESIM_CONVERTER_ARGB888);
}
