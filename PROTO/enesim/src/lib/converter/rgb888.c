#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _2d_rgb888_none_argb8888(Enesim_Converter_Data *data, uint32_t dw, uint32_t dh,
		uint32_t dpitch, uint32_t *src, uint32_t sw, uint32_t sh,
		uint32_t spitch)
{
	uint8_t *dst = data->pixels.rgb888.plane0;
	while (dh--)
	{
		uint16_t *ddst = dst;
		uint32_t *ssrc = src;
		uint32_t ddw = dw;
		while (ddw--)
		{
			*dst++ = (*ssrc >> 16) & 0xff;
			*dst++ = (*ssrc >> 8) & 0xff;
			*dst++ = *ssrc & 0xff;
			ssrc++;
		}
		dst += dpitch;
		src += spitch;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_converter_rgb888_init(void)
{
	/* TODO check if the cpu is the host */
	enesim_converter_surface_register(
			ENESIM_CONVERTER_2D(_2d_rgb888_none_argb8888),
			ENESIM_CONVERTER_RGB888,
			ENESIM_ANGLE_0,
			ENESIM_FORMAT_ARGB8888);
}

