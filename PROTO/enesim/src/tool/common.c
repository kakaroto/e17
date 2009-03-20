#include "common.h"

/*
 * In order to add a new surface format, you should follow the next steps:
 * 1. Create a new format description, look for the examples below
 * 2. Add your format into the formats struct, with the conditionals macros
 * 3. 
 */

const char * surface_format_name[ENESIM_SURFACE_FORMATS] = {
	[ENESIM_FORMAT_ARGB8888]       = "ENESIM_SURFACE_ARGB8888",
	[ENESIM_SURFACE_ARGB8888_UNPRE] = "ENESIM_SURFACE_ARGB8888_UNPRE",
	[ENESIM_SURFACE_RGB565_XA5]     = "ENESIM_SURFACE_RGB565_XA5",
	[ENESIM_SURFACE_RGB565_B1A3]    = "ENESIM_SURFACE_RGB565_B1A3",
};

/*============================================================================*
 *                                  argb8888                                  * 
 *============================================================================*/
Format argb8888_pre = {
	.sformat = ENESIM_FORMAT_ARGB8888,
	.name = "argb8888",
	.planes[0] = {
		.colors = {
			{
				.offset = 0,
				.length = 8,
				.name = COLOR_BLUE,
				.type = TYPE_UINT8,
			},
			{
				.offset = 8,
				.length = 8,
				.name = COLOR_GREEN,
				.type = TYPE_UINT8,
			},
			{
				.offset = 16,
				.length = 8,
				.name = COLOR_RED,
				.type = TYPE_UINT8,
			},
			{
				.offset = 24,
				.length = 8,
				.type = TYPE_UINT8,
				.name = COLOR_ALPHA,
			},
		},
		.num_colors = 4,
		.type = TYPE_UINT32,
	},
	.num_planes = 1,
	.premul = 1,
};
/*============================================================================*
 *                              argb8888_unpre                                * 
 *============================================================================*/
#ifdef BUILD_SURFACE_ARGB8888_UNPRE
Format argb8888 = {
	.sformat = ENESIM_SURFACE_ARGB8888_UNPRE,
	.name = "argb8888_unpre",
	.planes[0] = {
		.colors = {
			{
				.offset = 0,
				.length = 8,
				.name = COLOR_BLUE,
				.type = TYPE_UINT8,
			},
			{
				.offset = 8,
				.length = 8,
				.name = COLOR_GREEN,
				.type = TYPE_UINT8,
			},
			{
				.offset = 16,
				.length = 8,
				.name = COLOR_RED,
				.type = TYPE_UINT8,
			},
			{
				.offset = 24,
				.length = 8,
				.type = TYPE_UINT8,
				.name = COLOR_ALPHA,
			},
		},
		.num_colors = 4,
		.type = TYPE_UINT32,
	},
	.num_planes = 1,
	.premul = 0,
};
#endif
/*============================================================================*
 *                                rgb565_xa5                                  * 
 *============================================================================*/
#ifdef BUILD_SURFACE_RGB565_XA5
Format rgb565_xa5 = {
	.sformat = ENESIM_SURFACE_RGB565_XA5,
	.name = "rgb565_xa5",
	.planes[0] = {
		.colors = {
			{
				.offset = 0,
				.length = 5,
				.name = COLOR_BLUE,
				.type = TYPE_UINT8,
			},
			{
				.offset = 5,
				.length = 6,
				.name = COLOR_GREEN,
				.type = TYPE_UINT8,
			},
			{
				.offset = 11,
				.length = 5,
				.name = COLOR_RED,
				.type = TYPE_UINT8,
			},
		},
		.num_colors = 3,
		.type = TYPE_UINT16,
		.length = 16,
	},
	.planes[1] = {
		.colors = {
			{
				.offset = 0,
				.length = 5,
				.name = COLOR_ALPHA,
				.type = TYPE_UINT8,
			},
		},
		.num_colors = 1,
		.type = TYPE_UINT8,
		.length = 8,
	},
	.num_planes = 2,
	.premul = 1,
};
#endif
/*============================================================================*
 *                               rgb565_b1a3                                  * 
 *============================================================================*/
#ifdef BUILD_SURFACE_RGB565_B1A3
Format rgb565_b1a3 = {
	.sformat = ENESIM_SURFACE_RGB565_B1A3,
	.name = "rgb565_b1a3",
	.planes[0] = {
		.colors = {
			{
				.offset = 0,
				.length = 5,
				.name = COLOR_BLUE,
				.type = TYPE_UINT8,
			},
			{
				.offset = 5,
				.length = 6,
				.name = COLOR_GREEN,
				.type = TYPE_UINT8,
			},
			{
				.offset = 11,
				.length = 5,
				.name = COLOR_RED,
				.type = TYPE_UINT8,
			},
			},
			.num_colors = 3,
			.type = TYPE_UINT16,
		},
	.planes[1] = {
		.colors = {
			{
				.offset = 0,
				.length = 3,
				.name = COLOR_ALPHA,
				.type = TYPE_UINT8,
			},
		},
		.num_colors = 1,
		.type = TYPE_UINT8,
		.length = 4,
		.contiguous = 1,
	},
	.num_planes = 2,
	.premul = 1,
};
#endif

Format *formats[] = {
	&argb8888_pre,
#ifdef BUILD_SURFACE_ARGB8888_UNPRE
	&argb8888,
#endif
#ifdef BUILD_SURFACE_RGB565_XA5
	&rgb565_xa5,
#endif
#ifdef BUILD_SURFACE_RGB565_B1A3
	&rgb565_b1a3,
#endif
	NULL,
};

