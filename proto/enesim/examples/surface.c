#include <stdlib.h>

#include "Enesim.h"

Enesim_Surface * surface_new(int w, int h, Enesim_Surface_Format fmt)
{
	Enesim_Surface *s = NULL;
	switch(fmt)
	{
		case ENESIM_SURFACE_ARGB8888:
		{
			DATA32 *d;

			d = calloc(1, sizeof(DATA32) * w * h);
			s = enesim_surface_new(fmt, w, h, ENESIM_SURFACE_ALPHA, d);
		}
		break;

		default:
		break;
	}
	return s;
}

void surface_free(Enesim_Surface *s)
{
	Enesim_Surface_Format fmt;
	fmt = enesim_surface_format_get(s);
	switch(fmt)
	{
		case ENESIM_SURFACE_ARGB8888:
		{
			DATA32 *d;

			enesim_surface_data_get(s, &d);
			free(d);
		}
		default:
		break;
	}
	free(s);
}
