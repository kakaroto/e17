#ifndef _ARGB8888_H
#define _ARGB8888_H

#define ARGB_JOIN(a,r,g,b) (((a) << 24) + ((r) << 16) + ((g) << 8) + (b))
#define A_VAL(p) ((DATA8 *)(p))[3]
#define R_VAL(p) ((DATA8 *)(p))[2]
#define G_VAL(p) ((DATA8 *)(p))[1]
#define B_VAL(p) ((DATA8 *)(p))[0]

static inline DATA32 mul_256(DATA32 a, DATA32 c)
{
	return  ( (((((c) >> 8) & 0x00ff00ff) * (a)) & 0xff00ff00) +
	(((((c) & 0x00ff00ff) * (a)) >> 8) & 0x00ff00ff) );
}

static inline DATA32 mul_sym(DATA32 a, DATA32 x)
{
 	return ( (((((x) >> 8) & 0x00ff00ff) * (a) + 0xff00ff) & 0xff00ff00) +
	(((((x) & 0x00ff00ff) * (a) + 0xff00ff) >> 8) & 0x00ff00ff) );
}

void argb8888_c_color(Enesim_Surface_Data *data, int off, DATA32 c, int len);
void argb8888_c_color_mask(Enesim_Surface_Data *data, int off, DATA32 c, int len, DATA8 *mask);

#endif
