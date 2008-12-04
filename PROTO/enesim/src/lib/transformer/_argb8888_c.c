static inline argb8888_convolution2x2(Enesim_Surface_Data *data, Eina_F16p16 x, Eina_F16p16 y,
		unsigned w, unsigned int h, Enesim_Surface_Pixel *ret)
{
	Enesim_Surface_Data tmp;
	unsigned int p3 = 0, p2 = 0, p1 = 0, p0 = 0;
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
		//printf("%p\n", data->data.argb8888.plane0);
		p0 = *data->data.argb8888.plane0;
	}
	if ((sy > -1) && ((sx + 1) < w))
	{
		tmp = *data;
		
		argb8888_data_increment(&tmp, 1);
		p1 = *tmp.data.argb8888.plane0;
	}
	if ((sy + 1) < h)
	{
		if (sx > -1)
		{
			tmp = *data;
			argb8888_data_increment(&tmp, w);
			p2 = *tmp.data.argb8888.plane0;
		}
		if ((sx + 1) < w)
		{
			tmp = *data;
			
			argb8888_data_increment(&tmp, w + 1);
			p3 = *tmp.data.argb8888.plane0;
		}
	}
	if (p0 != p1)
		p0 = interp_256(ax, p1, p0);
	if (p2 != p3)
		p2 = interp_256(ax, p3, p2);
	if (p0 != p2)
		p0 = interp_256(ay, p2, p0);
	
	ret->pixel.argb8888.plane0 = p0;
	ret->format = ENESIM_SURFACE_ARGB8888;
}

static void argb8888_argb8888_good_affine_no_no(Enesim_Transformation *t, Enesim_Surface *ss,
		Eina_Rectangle *srect, Enesim_Surface *ds, Eina_Rectangle *drect)
{
	Enesim_Surface_Data sdata, ddata;
	Eina_F16p16 sx, sy;
	Enesim_Drawer_Point ptfnc;
	Eina_F16p16 a, b, c, d, e, f, g, h, i;
	Enesim_Surface_Pixel color;

	int hlen;

	/* force an alpha color, return if we dont have a point function */
	enesim_surface_pixel_argb_from(&color, ds->sdata.format, 0x55555555);
	ptfnc = enesim_drawer_point_color_get(t->rop, ds->sdata.format, &color);
	if (!ptfnc)
		return;
	
	enesim_matrix_fixed_values_get(t->matrix, &a, &b, &c, &d, &e, &f, &g, &h, &i);
	c += eina_f16p16_float_from(t->ox);
	f += eina_f16p16_float_from(t->oy);
	sx = eina_f16p16_mul(a, drect->x) + eina_f16p16_mul(b, drect->y) + c;
	sy = eina_f16p16_mul(d, drect->x) + eina_f16p16_mul(e, drect->y) + f;
	
	enesim_surface_data_get(ds, &ddata);
	enesim_surface_data_get(ss, &sdata);
	argb8888_data_increment(&ddata, (drect->y * ds->w) + drect->x);

	hlen = drect->h;
	while (hlen--)
	{
		Enesim_Surface_Data ddata2;
		int wlen;
		Eina_F16p16 sxx, syy;
		
		ddata2 = ddata;
		sxx = sx;
		syy = sy;

		wlen = drect->w;
		while (wlen--)
		{
			int six, siy;
			Enesim_Surface_Data sdata2;
			
			sdata2 = sdata;
			six = eina_f16p16_int_to(sxx);
			siy = eina_f16p16_int_to(syy);
			
			/* check that we are inside the source rectangle */
			if ((eina_rectangle_xcoord_inside(srect, six)) && (eina_rectangle_ycoord_inside(srect, siy)))
			{
				Enesim_Surface_Pixel argb;
				
				/* use 2x2 convolution kernel to interpolate */
				argb8888_data_increment(&sdata2, (siy * ss->w) + six);
				argb8888_convolution2x2(&sdata2, sxx, syy, ss->w, ss->h, &argb);
				ptfnc(&ddata2, NULL, &argb, NULL);
			}
			argb8888_data_increment(&ddata2, 1);
			sxx += a;
			syy += d;
		}
		argb8888_data_increment(&ddata, ds->w);
		sx += b;
		sy += e;
	}
}
