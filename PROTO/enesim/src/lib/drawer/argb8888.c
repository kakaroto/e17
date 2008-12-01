#include "Enesim.h"
#include "enesim_private.h"
#include "_argb8888_c.c"
#include "_argb8888_mmx.c"
#include "_argb8888_sse2.c"

Enesim_Drawer argb8888_drawer = {
	.sp_color[ENESIM_BLEND] = argb8888_sp_color_blend,
	.pt_color[ENESIM_BLEND] = argb8888_pt_color_blend,
	.sp_pixel[ENESIM_BLEND][ENESIM_SURFACE_ARGB8888] = argb8888_sp_pixel_blend_argb8888,
	.pt_pixel[ENESIM_BLEND][ENESIM_SURFACE_ARGB8888] = argb8888_pt_pixel_blend_argb8888,
	.sp_pixel[ENESIM_BLEND][ENESIM_SURFACE_ARGB8888_UNPRE] = argb8888_sp_pixel_blend_argb8888_unpre,
	.pt_pixel[ENESIM_BLEND][ENESIM_SURFACE_ARGB8888_UNPRE] = argb8888_pt_pixel_blend_argb8888_unpre,
	.sp_pixel[ENESIM_BLEND][ENESIM_SURFACE_RGB565_XA5] = NULL,
	.pt_pixel[ENESIM_BLEND][ENESIM_SURFACE_RGB565_XA5] = NULL,
	.sp_pixel[ENESIM_BLEND][ENESIM_SURFACE_RGB565_B1A3] = argb8888_sp_pixel_blend_rgb565_b1a3,
	.pt_pixel[ENESIM_BLEND][ENESIM_SURFACE_RGB565_B1A3] = argb8888_pt_pixel_blend_rgb565_b1a3,
	.sp_color[ENESIM_FILL] = argb8888_sp_color_fill,
	.pt_color[ENESIM_FILL] = argb8888_pt_color_fill,
	.sp_pixel[ENESIM_FILL][ENESIM_SURFACE_ARGB8888] = argb8888_sp_pixel_fill_argb8888,
	.pt_pixel[ENESIM_FILL][ENESIM_SURFACE_ARGB8888] = argb8888_pt_pixel_fill_argb8888,
	.sp_pixel[ENESIM_FILL][ENESIM_SURFACE_ARGB8888_UNPRE] = argb8888_sp_pixel_fill_argb8888_unpre,
	.pt_pixel[ENESIM_FILL][ENESIM_SURFACE_ARGB8888_UNPRE] = argb8888_pt_pixel_fill_argb8888_unpre,
	.sp_pixel[ENESIM_FILL][ENESIM_SURFACE_RGB565_XA5] = NULL,
	.pt_pixel[ENESIM_FILL][ENESIM_SURFACE_RGB565_XA5] = NULL,
	.sp_pixel[ENESIM_FILL][ENESIM_SURFACE_RGB565_B1A3] = argb8888_sp_pixel_fill_rgb565_b1a3,
	.pt_pixel[ENESIM_FILL][ENESIM_SURFACE_RGB565_B1A3] = argb8888_pt_pixel_fill_rgb565_b1a3,
};
