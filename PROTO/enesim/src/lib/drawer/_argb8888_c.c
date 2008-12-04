/*============================================================================*
 *                                 Blend                                      * 
 *============================================================================*/
static void argb8888_pt_color_blend_c(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	argb8888_blend(d->data.argb8888.plane0, color->pixel.argb8888.plane0);
}
static void argb8888_pt_pixel_blend_argb8888_c(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	argb8888_blend(d->data.argb8888.plane0, s->pixel.argb8888.plane0);
}

static void argb8888_pt_pixel_blend_generic_c(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	uint32_t argb;

	argb = enesim_surface_pixel_argb_to(s);
	argb8888_blend(d->data.argb8888.plane0, argb);
}

static void argb8888_sp_color_blend_c(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data dtmp, end;

	argb8888_data_copy(d, &dtmp);
	argb8888_data_offset(d, &end, len);
	while (dtmp.data.argb8888.plane0 < end.data.argb8888.plane0)
	{
		argb8888_blend(dtmp.data.argb8888.plane0, color->pixel.argb8888.plane0);
		argb8888_data_increment(&dtmp, 1);
	}
}
static void argb8888_sp_pixel_blend_argb8888_c(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data stmp, dtmp, end;

	argb8888_data_copy(d, &dtmp);
	argb8888_data_copy(s, &stmp);
	argb8888_data_offset(d, &end, len);
	while (dtmp.data.argb8888.plane0 < end.data.argb8888.plane0)
	{	
		argb8888_blend(dtmp.data.argb8888.plane0, *stmp.data.argb8888.plane0);
		argb8888_data_increment(&stmp, 1);
		argb8888_data_increment(&dtmp, 1);
	}
}
static void argb8888_sp_pixel_blend_generic_c(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data stmp, dtmp, end;

	argb8888_data_copy(d, &dtmp);
	stmp = *s;
	argb8888_data_offset(d, &end, len);
	while (dtmp.data.argb8888.plane0 < end.data.argb8888.plane0)
	{
		Enesim_Surface_Pixel p;

		enesim_surface_data_pixel_get(&stmp, &p);
		argb8888_pt_pixel_blend_generic_c(&dtmp, &p, 0, NULL);
		enesim_surface_data_increment(&stmp, 1);
		argb8888_data_increment(&dtmp, 1);
	}
}
/*============================================================================*
 *                                  Fill                                      * 
 *============================================================================*/
static void argb8888_pt_color_fill_c(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	argb8888_fill(d->data.argb8888.plane0, color->pixel.argb8888.plane0);
}
static void argb8888_pt_mask_color_fill_c(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s,
		Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	
}
static void argb8888_pt_pixel_fill_argb8888_c(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s,
		Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	argb8888_fill(d->data.argb8888.plane0, s->pixel.argb8888.plane0);
}
static void argb8888_pt_pixel_fill_generic_c(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s,
		Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	uint32_t argb;

	argb = enesim_surface_pixel_argb_to(s);
	argb8888_fill(d->data.argb8888.plane0, argb);
}

static void argb8888_sp_color_fill_c(Enesim_Surface_Data *d, unsigned int len,
		Enesim_Surface_Data *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Data *m)
{
	Enesim_Surface_Data dtmp, end;

	argb8888_data_copy(d, &dtmp);
	argb8888_data_offset(d, &end, len);
	while (dtmp.data.argb8888.plane0 < end.data.argb8888.plane0)
	{
		argb8888_fill(dtmp.data.argb8888.plane0, color->pixel.argb8888.plane0);
		argb8888_data_increment(&dtmp, 1);
	}
}
static void argb8888_sp_pixel_fill_argb8888_c(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data stmp, dtmp, end;

	argb8888_data_copy(d, &dtmp);
	argb8888_data_copy(s, &stmp);
	argb8888_data_offset(d, &end, len);
	while (dtmp.data.argb8888.plane0 < end.data.argb8888.plane0)
	{
		argb8888_fill(dtmp.data.argb8888.plane0, *stmp.data.argb8888.plane0);
		argb8888_data_increment(&stmp, 1);
		argb8888_data_increment(&dtmp, 1);
	}
}
static void argb8888_sp_pixel_fill_generic_c(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data stmp, dtmp, end;

	argb8888_data_copy(d, &dtmp);
	stmp = *s;
	argb8888_data_offset(d, &end, len);
	while (dtmp.data.argb8888.plane0 < end.data.argb8888.plane0)
	{
		Enesim_Surface_Pixel p;

		enesim_surface_data_pixel_get(&stmp, &p);
		argb8888_pt_pixel_fill_generic_c(&dtmp, &p, 0, NULL);
		enesim_surface_data_increment(&stmp, 1);
		argb8888_data_increment(&dtmp, 1);
	}
}

#define argb8888_sp_color_blend argb8888_sp_color_blend_c
#define argb8888_pt_color_blend argb8888_pt_color_blend_c
#define argb8888_sp_pixel_blend_argb8888 argb8888_sp_pixel_blend_argb8888_c
#define argb8888_pt_pixel_blend_argb8888 argb8888_pt_pixel_blend_argb8888_c
#define argb8888_sp_pixel_blend_argb8888_unpre argb8888_sp_pixel_blend_generic_c
#define argb8888_pt_pixel_blend_argb8888_unpre argb8888_pt_pixel_blend_generic_c
#define argb8888_sp_pixel_blend_rgb565_b1a3 argb8888_sp_pixel_blend_generic_c
#define argb8888_pt_pixel_blend_rgb565_b1a3 argb8888_pt_pixel_blend_generic_c

#define argb8888_sp_color_fill argb8888_sp_color_fill_c
#define argb8888_pt_color_fill argb8888_pt_color_fill_c
#define argb8888_sp_pixel_fill_argb8888 argb8888_sp_pixel_fill_argb8888_c
#define argb8888_pt_pixel_fill_argb8888 argb8888_pt_pixel_fill_argb8888_c
#define argb8888_sp_pixel_fill_argb8888_unpre argb8888_sp_pixel_fill_generic_c
#define argb8888_pt_pixel_fill_argb8888_unpre argb8888_pt_pixel_fill_generic_c
#define argb8888_sp_pixel_fill_rgb565_b1a3 argb8888_sp_pixel_fill_generic_c
#define argb8888_pt_pixel_fill_rgb565_b1a3 argb8888_pt_pixel_fill_generic_c
