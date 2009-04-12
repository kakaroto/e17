#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* NEW CODE */
/* FIXME use the area.x and area.y for the offsets and rows */
/*
 * o = origin coordinate
 * e = end coordinate
 * sl = source length
 * dl = destination length
 * gets the source coordinate based on the destination coordinate
 */
static inline Eina_Bool _generate_offsets(uint32_t o, uint32_t e, uint32_t sl, uint32_t dl, uint32_t *offsets)
{
	uint32_t c;

	if (!sl || !dl)
		return EINA_FALSE;
	for (c = 0; c < e - o; c++)
	{
		offsets[c] = (c * sl) / dl;
	}
	return EINA_TRUE;
}
/* FIXME the destination surface data should be at offset x,y already ? */
static void _1d_argb8888_argb8888(uint32_t *s, uint32_t sw, uint32_t x,
		uint32_t len, uint32_t dw, uint32_t *d)
{
	uint32_t *xoffsets;
	/* generate the x offsets */
	xoffsets = malloc(sizeof(uint32_t) * len);
	if (!xoffsets)
		return;
	//printf("%p %p %d %d\n", s, d, x, len);
	if (!_generate_offsets(x, x + len, sw, dw, xoffsets))
	{
		free(xoffsets);
		return;
	}
	while (len--)
	{
		int off = xoffsets[x++];

		*d = *(s + off);
		d++;
	}
	free(xoffsets);
	//printf("ok\n");
}
/* FIXME the destination surface data should be at offset x,y already ? */
static void _2d_argb8888_argb8888(uint32_t *s, uint32_t sw, uint32_t sh,
		uint32_t x, uint32_t y, uint32_t len, uint32_t dw, uint32_t dh,
		uint32_t *d)
{
	uint32_t *xoffsets;
	uint32_t *ss;
	uint32_t sy;

	/* generate the x offsets */
	xoffsets = alloca(sizeof(uint32_t) * len);
	sy = (y * sh) / dh;
	if (!_generate_offsets(x, x + len, sw, dw, xoffsets))
		return;
	/* TODO change w by the pitch */
	ss = s + (sy * sw);
	while (len--)
	{
		*d = ss[xoffsets[x++]];
		d++;
	}
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_scaler_argb8888_init(Enesim_Cpu *cpu)
{
	/* TODO check if the cpu is the host */
	enesim_scaler_1d_register(cpu, _1d_argb8888_argb8888, ENESIM_FORMAT_ARGB8888, ENESIM_FORMAT_ARGB8888);
}
