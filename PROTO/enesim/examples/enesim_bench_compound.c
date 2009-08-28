#include "enesim_bench_common.h"

void compound_bench(void)
{
	Enesim_Renderer *r, *rl, *rect, *c;
	Enesim_Surface *dst = NULL;
	Enesim_Matrix m;

	printf("*****************\n");
	printf("*    Compound   *\n");
	printf("*****************\n");
	surfaces_create(NULL, 0, NULL, 0, &dst, opt_fmt);

	r = enesim_renderer_compound_new();

	rl = enesim_renderer_stripes_new();
	enesim_renderer_stripes_color_set(rl, 0xffff0000, 0xffff00ff);
	enesim_renderer_stripes_thickness_set(rl, 1, 1);
	m.xx = 1; m.xy = 0; m.xz = 0;
	m.yx = 0; m.yy = 1.0 / opt_height; m.yz = 0;
	m.zx = 0; m.zy = 0; m.zz = 1;
	enesim_renderer_transform_set(rl, &m);

	rect = enesim_renderer_rectangle_new();
	enesim_renderer_rectangle_size_set(rect, opt_width, opt_height);
	enesim_renderer_rectangle_corner_radius_set(rect, 20);
	enesim_renderer_rectangle_corners_set(rect, 1, 1, 1, 1);
	enesim_renderer_shape_fill_renderer_set(rect, rl);
	enesim_renderer_shape_draw_mode_set(rect, ENESIM_SHAPE_DRAW_MODE_FILL);

	c = enesim_renderer_circle_new();
	enesim_renderer_circle_center_set(c, opt_width / 2.0, -(2 * opt_width) + (opt_height / 2.0));
	enesim_renderer_circle_radius_set(c, 2 * opt_width);
	enesim_renderer_shape_fill_color_set(c, 0x80808080);
	enesim_renderer_shape_draw_mode_set(c, ENESIM_SHAPE_DRAW_MODE_FILL);

	enesim_renderer_compound_layer_add(r, rect, ENESIM_FILL);
	enesim_renderer_compound_layer_add(r, c, ENESIM_BLEND);

	renderer_run(r, dst, "Compound", "compound");
}

