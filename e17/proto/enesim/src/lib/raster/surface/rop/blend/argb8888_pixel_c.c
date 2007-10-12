#include "Enesim.h"
#include "enesim_private.h"
#include "surface.h"
#include "argb8888.h"

void argb8888_c_pixel(Enesim_Surface_Data *data, int off, Enesim_Surface_Data *sdata, int soff, int len)
{
	DATA32 *d, *s, *e;

	*e = d + len;
	while (d < e)
	{
		len = 256 - (*s >> 24);
                *d = *s++ + mul_256(*d, len);
                d++;
	}
}
