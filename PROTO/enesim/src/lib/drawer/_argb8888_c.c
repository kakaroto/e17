static void argb8888_pt_color_blend_c(Enesim_Surface_Data *d, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)
{
	uint32_t data0;
	argb8888_from_argb(color, &data0);
	argb8888_blend(d->argb8888.plane0, data0);
}
static void argb8888_pt_pixel_blend_argb8888_c(Enesim_Surface_Data *d, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)
{
	uint32_t data0;
	unsigned int argb;
	argb8888_to_argb(&argb, *(s->argb8888.plane0));
	argb8888_from_argb(argb, &data0);
	argb8888_blend(d->argb8888.plane0, data0);
}
static void argb8888_pt_pixel_blend_argb8888_unpre_c(Enesim_Surface_Data *d, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)
{
	uint32_t data0;
	unsigned int argb;
	argb8888_unpre_to_argb(&argb, *(s->argb8888_unpre.plane0));
	argb8888_from_argb(argb, &data0);
	argb8888_blend(d->argb8888.plane0, data0);
}
static void argb8888_pt_pixel_blend_rgb565_b1a3_c(Enesim_Surface_Data *d, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)
{
	uint32_t data0;
	unsigned int argb;
	rgb565_b1a3_to_argb(&argb, *(s->rgb565_b1a3.plane0), *(s->rgb565_b1a3.plane1), s->rgb565_b1a3.pixel_plane1);
	argb8888_from_argb(argb, &data0);
	argb8888_blend(d->argb8888.plane0, data0);
}
static void argb8888_sp_color_blend_c(Enesim_Surface_Data *d, unsigned int len, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data dtmp, end;

	argb8888_data_copy(d, &dtmp);
	argb8888_data_offset(d, &end, len);
	while (dtmp.argb8888.plane0 < end.argb8888.plane0)
	{
		argb8888_pt_color_blend_c(&dtmp, NULL, color, NULL);
		argb8888_data_increment(&dtmp, 1);
	}
}
static void argb8888_sp_pixel_blend_argb8888_c(Enesim_Surface_Data *d, unsigned int len, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data stmp, dtmp, end;

	argb8888_data_copy(d, &dtmp);
	argb8888_data_copy(s, &stmp);
	argb8888_data_offset(d, &end, len);
	while (dtmp.argb8888.plane0 < end.argb8888.plane0)
	{
		argb8888_pt_pixel_blend_argb8888_c(&dtmp, &stmp, 0, NULL);
		argb8888_data_increment(&stmp, 1);
		argb8888_data_increment(&dtmp, 1);
	}
}
static void argb8888_sp_pixel_blend_argb8888_unpre_c(Enesim_Surface_Data *d, unsigned int len, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data stmp, dtmp, end;

	argb8888_data_copy(d, &dtmp);
	argb8888_unpre_data_copy(s, &stmp);
	argb8888_data_offset(d, &end, len);
	while (dtmp.argb8888.plane0 < end.argb8888.plane0)
	{
		argb8888_pt_pixel_blend_argb8888_unpre_c(&dtmp, &stmp, 0, NULL);
		argb8888_unpre_data_increment(&stmp, 1);
		argb8888_data_increment(&dtmp, 1);
	}
}
static void argb8888_sp_pixel_blend_rgb565_b1a3_c(Enesim_Surface_Data *d, unsigned int len, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data stmp, dtmp, end;

	argb8888_data_copy(d, &dtmp);
	rgb565_b1a3_data_copy(s, &stmp);
	argb8888_data_offset(d, &end, len);
	while (dtmp.argb8888.plane0 < end.argb8888.plane0)
	{
		argb8888_pt_pixel_blend_rgb565_b1a3_c(&dtmp, &stmp, 0, NULL);
		rgb565_b1a3_data_increment(&stmp, 1);
		argb8888_data_increment(&dtmp, 1);
	}
}
static void argb8888_pt_color_fill_c(Enesim_Surface_Data *d, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)
{
	uint32_t data0;
	argb8888_from_argb(color, &data0);
	argb8888_fill(d->argb8888.plane0, data0);
}
static void argb8888_pt_pixel_fill_argb8888_c(Enesim_Surface_Data *d, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)
{
	uint32_t data0;
	unsigned int argb;
	argb8888_to_argb(&argb, *(s->argb8888.plane0));
	argb8888_from_argb(argb, &data0);
	argb8888_fill(d->argb8888.plane0, data0);
}
static void argb8888_pt_pixel_fill_argb8888_unpre_c(Enesim_Surface_Data *d, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)
{
	uint32_t data0;
	unsigned int argb;
	argb8888_unpre_to_argb(&argb, *(s->argb8888_unpre.plane0));
	argb8888_from_argb(argb, &data0);
	argb8888_fill(d->argb8888.plane0, data0);
}
static void argb8888_pt_pixel_fill_rgb565_b1a3_c(Enesim_Surface_Data *d, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)
{
	uint32_t data0;
	unsigned int argb;
	rgb565_b1a3_to_argb(&argb, *(s->rgb565_b1a3.plane0), *(s->rgb565_b1a3.plane1), s->rgb565_b1a3.pixel_plane1);
	argb8888_from_argb(argb, &data0);
	argb8888_fill(d->argb8888.plane0, data0);
}
static void argb8888_sp_color_fill_c(Enesim_Surface_Data *d, unsigned int len, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data dtmp, end;

	argb8888_data_copy(d, &dtmp);
	argb8888_data_offset(d, &end, len);
	while (dtmp.argb8888.plane0 < end.argb8888.plane0)
	{
		argb8888_pt_color_fill_c(&dtmp, NULL, color, NULL);
		argb8888_data_increment(&dtmp, 1);
	}
}
static void argb8888_sp_pixel_fill_argb8888_c(Enesim_Surface_Data *d, unsigned int len, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data stmp, dtmp, end;

	argb8888_data_copy(d, &dtmp);
	argb8888_data_copy(s, &stmp);
	argb8888_data_offset(d, &end, len);
	while (dtmp.argb8888.plane0 < end.argb8888.plane0)
	{
		argb8888_pt_pixel_fill_argb8888_c(&dtmp, &stmp, 0, NULL);
		argb8888_data_increment(&stmp, 1);
		argb8888_data_increment(&dtmp, 1);
	}
}
static void argb8888_sp_pixel_fill_argb8888_unpre_c(Enesim_Surface_Data *d, unsigned int len, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data stmp, dtmp, end;

	argb8888_data_copy(d, &dtmp);
	argb8888_unpre_data_copy(s, &stmp);
	argb8888_data_offset(d, &end, len);
	while (dtmp.argb8888.plane0 < end.argb8888.plane0)
	{
		argb8888_pt_pixel_fill_argb8888_unpre_c(&dtmp, &stmp, 0, NULL);
		argb8888_unpre_data_increment(&stmp, 1);
		argb8888_data_increment(&dtmp, 1);
	}
}
static void argb8888_sp_pixel_fill_rgb565_b1a3_c(Enesim_Surface_Data *d, unsigned int len, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data stmp, dtmp, end;

	argb8888_data_copy(d, &dtmp);
	rgb565_b1a3_data_copy(s, &stmp);
	argb8888_data_offset(d, &end, len);
	while (dtmp.argb8888.plane0 < end.argb8888.plane0)
	{
		argb8888_pt_pixel_fill_rgb565_b1a3_c(&dtmp, &stmp, 0, NULL);
		rgb565_b1a3_data_increment(&stmp, 1);
		argb8888_data_increment(&dtmp, 1);
	}
}

#define argb8888_sp_color_blend argb8888_sp_color_blend_c
#define argb8888_pt_color_blend argb8888_pt_color_blend_c
#define argb8888_sp_pixel_blend_argb8888 argb8888_sp_pixel_blend_argb8888_c
#define argb8888_pt_pixel_blend_argb8888 argb8888_pt_pixel_blend_argb8888_c
#define argb8888_sp_pixel_blend_argb8888_unpre argb8888_sp_pixel_blend_argb8888_unpre_c
#define argb8888_pt_pixel_blend_argb8888_unpre argb8888_pt_pixel_blend_argb8888_unpre_c
#define argb8888_sp_pixel_blend_rgb565_b1a3 argb8888_sp_pixel_blend_rgb565_b1a3_c
#define argb8888_pt_pixel_blend_rgb565_b1a3 argb8888_pt_pixel_blend_rgb565_b1a3_c

#define argb8888_sp_color_fill argb8888_sp_color_fill_c
#define argb8888_pt_color_fill argb8888_pt_color_fill_c
#define argb8888_sp_pixel_fill_argb8888 argb8888_sp_pixel_fill_argb8888_c
#define argb8888_pt_pixel_fill_argb8888 argb8888_pt_pixel_fill_argb8888_c
#define argb8888_sp_pixel_fill_argb8888_unpre argb8888_sp_pixel_fill_argb8888_unpre_c
#define argb8888_pt_pixel_fill_argb8888_unpre argb8888_pt_pixel_fill_argb8888_unpre_c
#define argb8888_sp_pixel_fill_rgb565_b1a3 argb8888_sp_pixel_fill_rgb565_b1a3_c
#define argb8888_pt_pixel_fill_rgb565_b1a3 argb8888_pt_pixel_fill_rgb565_b1a3_c
