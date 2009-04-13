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
static void _1d_from_argb8888_argb8888_none(uint32_t *native, uint32_t len, void *conv)
{
	uint32_t *ddst = conv;

	while (len--)
	{
		uint8_t pa;

		pa = (*native >> 24);
		if ((pa > 0) && (pa < 255))
		{
			*ddst = (pa << 24)|
				(((argb8888_red_get(*native) * 255) / pa) << 16) |
				(((argb8888_green_get(*native) * 255) / pa) << 8) |
				((argb8888_blue_get(*native) * 255) / pa);
		}
		else
		{
			*ddst = *native;
		}
		ddst++;
		native++;
	}
}
static void _1d_to_argb8888_argb8888_none(uint32_t *native, uint32_t len, void *conv)
{
	uint32_t *ssrc = conv;

	while (len--)
	{
		uint16_t a = (*ssrc >> 24) + 1;

		if (a != 256)
		{
			*native = (*ssrc & 0xff000000) + (((((*ssrc) >> 8) & 0xff) * a) & 0xff00) +
			(((((*ssrc) & 0x00ff00ff) * a) >> 8) & 0x00ff00ff);
		}
		else
			*native = *ssrc;
		native++;
		ssrc++;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_converter_argb8888_init(Enesim_Cpu *cpu)
{
	/* TODO check if the cpu is the host */
	enesim_converter_2d_register(ENESIM_CONVERTER_2D(_2d_argb8888_argb8888_none), cpu, ENESIM_FORMAT_ARGB8888, ENESIM_ROTATOR_NONE, ENESIM_CONVERTER_ARGB8888);
	enesim_converter_1d_from_register(ENESIM_CONVERTER_1D(_1d_from_argb8888_argb8888_none), cpu, ENESIM_FORMAT_ARGB8888, ENESIM_CONVERTER_ARGB8888);
	enesim_converter_1d_to_register(ENESIM_CONVERTER_1D(_1d_to_argb8888_argb8888_none), cpu, ENESIM_CONVERTER_ARGB8888, ENESIM_FORMAT_ARGB8888);
}
