#include "Enesim.h"
#include "enesim_private.h"
#include "surface.h"
#include "argb8888.h"

void argb8888_c_color(Enesim_Surface_Data *data, int off, DATA32 c, int len)
{
	DATA32 *d, *e, a;

	d = data->argb8888.data + off;
	e = d + len;
	a = 256 - (c >> 24);

	while (d < e)
	{
		*d = c + mul_256(a, *d);
		d++;
	}
}

void argb8888_c_color_mask(Enesim_Surface_Data *data, int off, DATA32 c, int len, DATA8 *mask)
{
	DATA32 *d, *e;

	d = data->argb8888.data + off;
	e = d + len;
	len = 256 - (c >> 24);
	while (d < e)
	{
		DATA32 a = *mask;
		switch(a)
		{
			case 0:
			break;

			case 255:
			*d = c + mul_256(len, *d);
			break;

			default:
			{
				DATA32 mc = mul_sym(a, c);
				a = 256 - (mc >> 24);
				*d = mc + mul_256(a, *d);
			}
			break;
		}
		d++;
		mask++;
	}

}


#if 0
void argb8888_c_color_gradient(Enesim_Surface_Data *data, int off, DATA32 c,
	DATA8 a0, DATA8 a1, int w)
{

}

void argb8888_c_color_alpha(Enesim_Surface_Data *data, int off, DATA32 c, DATA8 alpha,
int w)
{

}

void argb8888_c_color_mask(Enesim_Surface_Data *data, int off, Enesim_Surface_Data *sdata, int soff, DATA32 c, DATA8 *mask, int w)
{

}
#endif
