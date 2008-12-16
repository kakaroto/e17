/*
 * TODO
 * + Use mmx if possible, that means that we need to refactor several
 * parts of the header and the _argb8888_mmx source file
 * 
 * + all the above generic functions should convert the src, dst and mask
 * to argb8888 (internal format) and handle the real operation in that
 * format. This will incredible slow, but works as a generic way
 * to handle this, any surface format implementor should just
 * create the converters from/to argb8888. In case he wants an accelerated
 * drawer he must implement a specifc drawer
 */
/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
/*============================================================================*
 *                                 Blend                                      *
 *============================================================================*/
static void generic_pt_color_blend(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	Enesim_Surface_Pixel c_argb8888, d_argb8888, tmp;

	enesim_surface_pixel_convert(color, &c_argb8888, ENESIM_SURFACE_ARGB8888);
	enesim_surface_data_pixel_get(d, &tmp);
	enesim_surface_pixel_convert(&tmp, &d_argb8888, ENESIM_SURFACE_ARGB8888);
	argb8888_blend(&d_argb8888.pixel.argb8888.plane0, c_argb8888.pixel.argb8888.plane0 >> 24, c_argb8888.pixel.argb8888.plane0);
	enesim_surface_pixel_convert(&d_argb8888, &tmp, d->format);
	enesim_surface_data_pixel_set(d, &tmp);
}
static void generic_pt_pixel_blend(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{

}
static void generic_pt_pixel_color_blend(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{

}
static void generic_pt_mask_color_blend(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{

}

static void generic_pt_pixel_mask_blend(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{

}

static void generic_sp_color_blend(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{

}

static void generic_sp_mask_color_blend(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	
}

static void generic_sp_pixel_color_blend(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	
}

static void generic_sp_pixel_mask_blend(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	
}

static void generic_sp_pixel_blend(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{

}
/*============================================================================*
 *                                  Fill                                      * 
 *============================================================================*/
static void generic_pt_color_fill(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	Enesim_Surface_Pixel c_argb8888, d_argb8888, tmp;

	enesim_surface_pixel_convert(color, &c_argb8888, ENESIM_SURFACE_ARGB8888);
	enesim_surface_data_pixel_get(d, &tmp);
	enesim_surface_pixel_convert(&tmp, &d_argb8888, ENESIM_SURFACE_ARGB8888);
	argb8888_fill(&d_argb8888.pixel.argb8888.plane0, c_argb8888.pixel.argb8888.plane0);
	enesim_surface_pixel_convert(&d_argb8888, &tmp, d->format);
	enesim_surface_data_pixel_set(d, &tmp);
}
static void generic_pt_pixel_fill(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	Enesim_Surface_Pixel s_argb8888, d_argb8888, tmp;

	enesim_surface_pixel_convert(s, &s_argb8888, ENESIM_SURFACE_ARGB8888);
	enesim_surface_data_pixel_get(d, &tmp);
	enesim_surface_pixel_convert(&tmp, &d_argb8888, ENESIM_SURFACE_ARGB8888);
	argb8888_fill(&d_argb8888.pixel.argb8888.plane0, s_argb8888.pixel.argb8888.plane0);
	enesim_surface_pixel_convert(&d_argb8888, &tmp, d->format);
	enesim_surface_data_pixel_set(d, &tmp);
}
static void generic_pt_pixel_color_fill(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	//argb8888_fill(d->data.argb8888.plane0, argb8888_mul4_sym(color->pixel.argb8888.plane0, s->pixel.argb8888.plane0));
}
static void generic_pt_mask_color_fill(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{

}

static void generic_pt_pixel_mask_fill(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{

}

static void generic_sp_color_fill(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{

}
static void generic_sp_pixel_fill(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
}

static void generic_sp_mask_color_fill(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	
}

static void generic_sp_pixel_color_fill(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	
}

static void generic_sp_pixel_mask_fill(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	
}

Enesim_Drawer_Generic generic_drawer = {
	.pt_color[ENESIM_BLEND] = generic_pt_color_blend,
	.pt_mask_color[ENESIM_BLEND] = generic_pt_mask_color_blend,
	.pt_pixel[ENESIM_BLEND] = generic_pt_pixel_blend,
	.pt_pixel_color[ENESIM_BLEND] = generic_pt_pixel_color_blend,
	.pt_pixel_mask[ENESIM_BLEND] = generic_pt_pixel_mask_blend,
	
	.sp_color[ENESIM_BLEND] = generic_sp_color_blend,
	.sp_mask_color[ENESIM_BLEND] = generic_sp_mask_color_blend,
	.sp_pixel[ENESIM_BLEND] = generic_sp_pixel_blend,
	.sp_pixel_color[ENESIM_BLEND] = generic_sp_pixel_color_blend,
	.sp_pixel_mask[ENESIM_BLEND] = generic_sp_pixel_mask_blend,
	
	.pt_color[ENESIM_FILL] = generic_pt_color_fill,
	.pt_mask_color[ENESIM_FILL] = generic_pt_mask_color_fill,
	.pt_pixel[ENESIM_FILL] = generic_pt_pixel_fill,
	.pt_pixel_color[ENESIM_FILL] = generic_pt_pixel_color_fill,
	.pt_pixel_mask[ENESIM_FILL] = generic_pt_pixel_mask_fill,
		
	.sp_color[ENESIM_FILL] = generic_sp_color_fill,
	.sp_mask_color[ENESIM_FILL] = generic_sp_mask_color_fill,
	.sp_pixel[ENESIM_FILL] = generic_sp_pixel_fill,
	.sp_pixel_color[ENESIM_FILL] = generic_sp_pixel_color_fill,
	.sp_pixel_mask[ENESIM_FILL] = generic_sp_pixel_mask_fill,
};
