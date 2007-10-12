#ifndef _ARGB8888_H
#define _ARGB8888_H

static inline DATA32 mul_256(DATA32 c, DATA32 a)
{
	return  ( (((((c) >> 8) & 0x00ff00ff) * (a)) & 0xff00ff00) +
	(((((c) & 0x00ff00ff) * (a)) >> 8) & 0x00ff00ff) );
}

void argb8888_c_color(Enesim_Surface_Data *data, int off, DATA32 c, int len);

#endif
