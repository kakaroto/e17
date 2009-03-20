/* TODO
 * + move the changes here to generic
 */
#define normal_argb8888_argb8888_affine_no_no(body)				\
	uint32_t *sdata, *ddata;						\
	Eina_F16p16 sx, sy;							\
	Enesim_Drawer_Point ptfnc;						\
	Eina_F16p16 a, b, c, d, e, f, g, h, i;					\
	Enesim_Surface_Pixel color;						\
										\
	int hlen;								\
	Eina_F16p16 x, y;							\
										\
	/* force an alpha color, return if we dont have a point function */	\
	ptfnc = enesim_transformation_drawer_point_get(t, ds, ss);		\
	if (!ptfnc)								\
		return;								\
										\
	ddata = ds->sdata.data.argb8888.plane0;					\
	sdata = ss->sdata.data.argb8888.plane0;					\
	ddata += (drect->y * ds->w) + drect->x;					\
	x = eina_f16p16_int_from(drect->x - t->ox);				\
	y = eina_f16p16_int_from(drect->y - t->oy);				\
										\
	enesim_matrix_fixed_values_get(&t->matrix, &a, &b, &c, &d, &e,		\
			&f, &g, &h, &i);					\
	c += eina_f16p16_float_from(t->ox);					\
	f += eina_f16p16_float_from(t->oy);					\
	sx = eina_f16p16_mul(a, x) + eina_f16p16_mul(b, y) + c;			\
	sy = eina_f16p16_mul(d, x) + eina_f16p16_mul(e, y) + f;			\
										\
	hlen = drect->h;							\
	while (hlen--)								\
	{									\
		uint32_t *ddata2;						\
		int wlen;							\
		Eina_F16p16 sxx, syy;						\
										\
		ddata2 = ddata;							\
		sxx = sx;							\
		syy = sy;							\
										\
		wlen = drect->w;						\
		while (wlen--)							\
		{								\
			int six, siy;						\
			uint32_t *sdata2;					\
										\
			sdata2 = sdata;						\
			six = eina_f16p16_int_to(sxx);				\
			siy = eina_f16p16_int_to(syy);				\
										\
			/* check that we are inside the source rectangle */	\
			if ((eina_rectangle_xcoord_inside(srect, six)) && 	\
				(eina_rectangle_ycoord_inside(srect, siy)))	\
			{							\
				Enesim_Surface_Pixel argb;			\
				Enesim_Surface_Data tmp;			\
										\
				sdata2 += (siy * ss->w) + six;			\
										\
				body						\
										\
				tmp.data.argb8888.plane0 = ddata2;		\
				ptfnc(&tmp, &argb, t->color, NULL);		\
			}							\
			ddata2++;						\
			sxx += a;						\
			syy += d;						\
		}								\
		ddata += ds->w;							\
		sx += b;							\
		sy += e;							\
	}									\

static inline argb8888_convolution2x2(uint32_t *data, Eina_F16p16 x, Eina_F16p16 y,
		unsigned w, unsigned int h, Enesim_Surface_Pixel *ret)
{
	uint32_t p3 = 0, p2 = 0, p1 = 0, p0 = 0;
	int ax, ay;
	int sx, sy;

	/* 8 bits error to alpha */
	ax = 1 + (eina_f16p16_fracc_get(x) >> 8);
	ay = 1 + (eina_f16p16_fracc_get(y) >> 8);
	/* integer values for the coordinates */
	sx = eina_f16p16_int_to(x);
	sy = eina_f16p16_int_to(y);

	if ((sx > -1) && (sy > -1))
	{
		p0 = *data;
	}
	if ((sy > -1) && ((sx + 1) < w))
	{
		p1 = *(data + 1);
	}
	if ((sy + 1) < h)
	{
		if (sx > -1)
		{
			p2 = *(data + w);
		}
		if ((sx + 1) < w)
		{
			p3 = *(data + w + 1);
		}
	}
	if (p0 != p1)
		p0 = interp_256(ax, p1, p0);
	if (p2 != p3)
		p2 = interp_256(ax, p3, p2);
	if (p0 != p2)
		p0 = interp_256(ay, p2, p0);

	ret->pixel.argb8888.plane0 = p0;
	ret->format = ENESIM_FORMAT_ARGB8888;
}

static void normal_argb8888_argb8888_affine_good_no_no(Enesim_Transformation *t, Enesim_Surface *ss,
		Eina_Rectangle *srect, Enesim_Surface *ds, Eina_Rectangle *drect)
{
	normal_argb8888_argb8888_affine_no_no(
		/* use 2x2 convolution kernel to interpolate */
		argb8888_convolution2x2(sdata2, sxx, syy, ss->w, ss->h, &argb);
	)
}

static void normal_argb8888_argb8888_affine_fast_no_no(Enesim_Transformation *t, Enesim_Surface *ss,
		Eina_Rectangle *srect, Enesim_Surface *ds, Eina_Rectangle *drect)
{
	normal_argb8888_argb8888_affine_no_no(
		argb.pixel.argb8888.plane0 = *sdata2;
	)
}
