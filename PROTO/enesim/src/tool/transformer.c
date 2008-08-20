#if 0
static inline void
ARGB8888_CONVOLUTION_2X2_PROTO
{
	uint32_t p3 = 0, p2 = 0, p1 = 0, p0 = 0;
	int ax, ay;
	int sx, sy;
	
	/* 8 bits error to alpha */
	ax = 1 + enesim_16p16_fracc_get(x) >> 8;
	ay = 1 + enesim_16p16_fracc_get(y) >> 8;
	/* integer values for the coordinates */
	sx = enesim_16p16_int_to(x);
	sy = enesim_16p16_int_to(y);
	
	if ((sx > -1) && (sy > -1))
		p0 = *p;
	if ((sy > -1) & ((sx + 1) < sw))
		p1 = *(p + 1);
	if ((sy + 1) < sh)
	{
		if (sx> -1)
			p2 = *(p + sw);
		if ((sx + 1) < sw)
			p3 = *(p + sw + 1);
	}

	if (p0 != p1)
		p0 = interp_256(ax, p1, p0);
	if (p2 != p3)
		p2 = interp_256(ax, p3, p2);
	if (p0 != p2)
		p0 = interp_256(ay, p2, p0);
	if (p0)
	{
		RENDER_OP_CALL
	}
}
void 
ARGB8888_NOSCALE_AFFINE_ITERATOR_PROTO
{
	uint32_t *dp;
	uint32_t *sp;
	Eina_F16p16 sx, sy;
	int h;
	
	sx = enesim_16p16_mul(t[MATRIX_XX], drect->x) + enesim_16p16_mul(t[MATRIX_XY],drect->y) + t[MATRIX_XZ];
	sy = enesim_16p16_mul(t[MATRIX_YX], drect->x) + enesim_16p16_mul(t[MATRIX_YY],drect->y) + t[MATRIX_YZ];
	
	dp = ds->data.argb8888.plane0 + (drect->y * ds->w) + drect->x;
	h = drect->h;
	while (h--)
	{
		uint32_t *d = dp;
		uint32_t *e = d + drect->w;
		Eina_F16p16 sxx, syy;
		
		sxx = sx;
		syy = sy;

		while (d < e)
		{

			int six, siy;
			
			six = enesim_16p16_int_to(sxx);
			siy = enesim_16p16_int_to(syy);
			
			/* check that we are inside the source rectangle */
			if ((enesim_rectangle_xcoord_inside(srect, six)) && (enesim_rectangle_ycoord_inside(srect, siy)))
			{
				/* use 2x2 convolution kernel to interpolate */
				sp = ss->data.argb8888.plane0 + (siy * ss->w) + six;
				ARGB8888_CONVOLUTION_CALL
			}
			d++;
			sxx += t[MATRIX_XX];
			syy += t[MATRIX_YX];
		}
		dp += ds->w;
		sx += t[MATRIX_XY];
		sy += t[MATRIX_YY];
	}
}

void 
ARGB8888_NOSCALE_IDENTITY_ITERATOR_PROTO
{
	uint32_t *dp, *sp;
	int h;
	
	dp = ds->data.argb8888.plane0 + (drect->y * ds->w) + drect->x;
	sp = ss->data.argb8888.plane0 + (srect->y * ss->w) + srect->x; 
	h = drect->h;
	while (h--)
	{
		uint32_t *d = dp;
		uint32_t *s = sp;
		uint32_t *e = d + drect->w;

		while (d < e)
		{
			RENDER_OP_CALL
			d++;
			s++;
		}
		dp += ds->w;
		sp += ss->w;
	}
}
#endif

static void transformer_prototype_start(Format *f)
{
	fprintf(fout, "(Enesim_Transformation *t, Enesim_Surface *ss, Enesim_Rectangle *srect, Enesim_Surface *ds, Enesim_Rectangle *drect)\n")
	fprintf(fout, "{\n");
}

static void transformer_prototype_end(Format *f)
{
	fprintf(fout, "}\n");
}

static void affine(Format *f)
{
	fprintf(fout, "static void transformer_affine_"); 
	transformer_prototype_start();
	frptinf(fout, "unsigned int h;");
	fprintf(fout, "Enesim_Data sdata, ddata;\n\n");
	fprintf(fout, "enesim_surface_data_copy(ss, &sdata);\n");
	fprintf(fout, "enesim_surface_data_copy(ds, &ddata);\n");
	fprintf(fout, "enesim_surface_data_increment(&sdata, ss->format, (srect->y * ss->w) + srect->x;\n");
	fprintf(fout, "enesim_surface_data_increment(&ddata, ds->format, (drect->y * ds->w) + drect->x;\n");
	fprintf(fout, "h = drect->h;\n");
	fprintf(fout, "while (h--)\n");
}

static void projective(Format *f)
{
	
}

static void identity(Format *f)
{
	
}

void transformer_functions(Format *df)
{
	
}
