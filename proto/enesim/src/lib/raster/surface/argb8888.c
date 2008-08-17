#include "Enesim.h"
#include "enesim_private.h"
#include "surface.h"
#include "argb8888.h"

static void argb8888_premul(Enesim_Surface_Data *sdata, int len)
{
	DATA32  *d, *e;
	d = sdata->argb8888.data;
	e = d + len;

	while (d < e) {
		DATA32 a = 1 + (*d >> 24);

		*d = (*d & 0xff000000) + (((((*d) >> 8) & 0xff) * a) & 0xff00)
				+ (((((*d) & 0x00ff00ff) * a) >> 8) & 0x00ff00ff);
		d++;
	}
}

static void argb8888_unpremul(Enesim_Surface_Data *sdata, Enesim_Surface_Data *ddata, int len)
{
	DATA32 *d, *s, *e;

	s = sdata->argb8888.data;
	d = ddata->argb8888.data;
	
	e = s + len;
	while (s < e) {
		DATA32 a = (*s >> 24);

		if ((a > 0) && (a < 255))
			*d = ARGB_JOIN(a, (R_VAL(s) * 255) / a, (G_VAL(s) * 255) / a,
					(B_VAL(s) * 255) / a);
		s++;
		d++;
	}
}

Surface_Backend argb8888_backend = {
		.rops = {
				[ENESIM_RENDERER_BLEND] = {
						argb8888_c_color,
						argb8888_c_color_mask,
					},
		},
		.premul = argb8888_premul,
		.unpremul = argb8888_unpremul,
};

