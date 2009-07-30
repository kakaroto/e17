#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <Eon.h>

static void _color_animation(Ekeko_Object *o, const char *prop,
		Eon_Color orig, Eon_Color dst, unsigned int secs)
{
	Eon_Animation *a;
	Eon_Clock clock;
	Ekeko_Value v;

	a = (Eon_Animation *)eon_animation_basic_new();
	ekeko_object_child_append((Ekeko_Object *)o, (Ekeko_Object *)a);
	eon_animation_property_set(a, prop);
	eon_animation_begin_set(a, (Ekeko_Object *)o, EKEKO_EVENT_UI_MOUSE_CLICK);
	eon_value_color_from(&v, orig);
	eon_animation_from_set(a, &v);
	eon_value_color_from(&v, dst);
	eon_animation_to_set(a, &v);
	eon_value_clock_seconds_from(&v, &clock, secs);
	eon_animation_duration_set(a, &clock);
	eon_animation_calc_set(a, EON_CALC_LINEAR);
	eon_animation_end_set(a, (Ekeko_Object *)o, EKEKO_EVENT_UI_MOUSE_CLICK);
	eon_animation_repeat_set(a, -1);
}

static void _coord_animation(Ekeko_Object *o, const char *prop,
		int orig, Eon_Coord_Type torig, int dst, Eon_Coord_Type tdst,
		unsigned int secs)
{
	Eon_Animation *a;
	Eon_Coord coord;
	Eon_Clock clock;
	Ekeko_Value v;

	a = (Eon_Animation *)eon_animation_basic_new();
	ekeko_object_child_append((Ekeko_Object *)o, (Ekeko_Object *)a);
	eon_animation_property_set(a, prop);
	eon_coord_set(&coord, orig, torig);
	eon_value_coord_from(&v, &coord);
	eon_animation_from_set(a, &v);
	eon_coord_set(&coord, dst, tdst);
	eon_value_coord_from(&v, &coord);
	eon_animation_to_set(a, &v);
	eon_value_clock_seconds_from(&v, &clock, secs);
	eon_animation_duration_set(a, &clock);
	eon_animation_calc_set(a, EON_CALC_LINEAR);
	eon_animation_end_set(a, (Ekeko_Object *)o, EKEKO_EVENT_UI_MOUSE_CLICK);
	eon_animation_begin_set(a, (Ekeko_Object *)o, EKEKO_EVENT_UI_MOUSE_CLICK);
	eon_animation_repeat_set(a, -1);
}

static Eon_Filter * _filter_add(Eon_Canvas *c)
{
	Eon_Filter *f;
	Eon_Displace *disp;

	f = eon_filter_new(c);
	//disp = eon_displace_new(f);
	return f;
}

static void _filter_apply(Eon_Shape *s, Eon_Filter *f)
{
	printf("Applying filter!!!\n");
	eon_shape_filter_set(s, f);
}

static void _click_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Enesim_Matrix m;
	static Eon_Filter *f1 = NULL;
	static Eon_Filter *f2 = NULL;
	static Eon_Filter *curr = NULL;

#if 0
	Eina_Rectangle r;
	printf("clicked!!\n");

	eina_rectangle_coords_from(&r, 200, 200, 100, 400);
	ekeko_canvas_damage_add((Ekeko_Canvas *)data, &r);
#endif
#if 0
	enesim_matrix_identity(&m);
	enesim_matrix_scale(&m, 1, 1);
	eon_image_matrix_set((Eon_Image *)data, &m);
#endif
	if (!f1)
	{
		Eon_Canvas *c;

		c = eon_shape_canvas_get((Eon_Shape *)data);
		f1 = _filter_add(c);
		f2 = _filter_add(c);
		curr = f1;
	}
	if (curr == f1)
	{

		curr = f2;
	}
	else
	{
		curr = f1;
	}
	_filter_apply((Eon_Shape *)data, curr);
}

static void _setup_scene(Eon_Canvas *c)
{
	Eon_Image *i;
	Eon_Rect *r;

#if 1
	/* create an image */
	i = eon_image_new(c);
	//eon_image_x_set(i, 30);
	//eon_image_y_set(i, 30);
	//eon_image_w_set(i, 400);
	//eon_image_h_set(i, 412);
	//eon_image_file_set(i, "/home/jl/code/efl-research/trunk/eon/data/tiger.png");
	eon_image_file_set(i, "/home/jl/checker.png");
	eon_image_x_set(i, 150);
	eon_image_y_set(i, 150);
	eon_image_w_set(i, 256);
	eon_image_h_set(i, 256);
	eon_image_show(i);
#endif
#if 1
	/* create a rectangle */
	r = eon_rect_new(c);
	eon_rect_x_rel_set(r, 25);
	eon_rect_y_rel_set(r, 25);
	eon_rect_w_rel_set(r, 50);
	eon_rect_h_rel_set(r, 50);
	eon_rect_color_set(r, 0xaaaa0000);
	eon_rect_rop_set(r, ENESIM_BLEND);
	eon_rect_show(r);
#endif
	_color_animation((Ekeko_Object *)r, "color", 0xff00ff00, 0xaaffaaff, 30);
	//_coord_animation((Ekeko_Object *)r, "w", 10, EON_COORD_RELATIVE, 100, EON_COORD_RELATIVE, 30);
	//ekeko_event_listener_add((Ekeko_Object *)i, EVENT_UI_MOUSE_DOWN, _click_cb, EINA_FALSE, c);
	//ekeko_event_listener_add((Ekeko_Object *)r, EVENT_UI_MOUSE_DOWN, _click_cb, EINA_FALSE, r);

#if 0
	r = eon_rect_new(sc);
	eon_rect_x_rel_set(r, 0);
	eon_rect_y_rel_set(r, 0);
	eon_rect_w_rel_set(r, 100);
	eon_rect_h_rel_set(r, 100);
	eon_rect_color_set(r, 0xaaaa00aa);
	eon_rect_rop_set(r, ENESIM_FILL);
	eon_rect_show(r);
#endif
}

int main(int argc, char **argv)
{
	Eon_Document *d;
	Eon_Canvas *c, *sc;
	Eon_Rect *r;
	Enesim_Matrix mx1, mx2;

	eon_init();

	d = eon_document_new("sdl", 480, 640);
	c = eon_document_canvas_get(d);
	eon_canvas_w_rel_set(c, 100);
	eon_canvas_h_rel_set(c, 100);

	/* create the background */
	r = eon_rect_new(c);
	eon_rect_x_rel_set(r, 0);
	eon_rect_y_rel_set(r, 0);
	eon_rect_w_rel_set(r, 100);
	eon_rect_h_rel_set(r, 100);
	eon_rect_color_set(r, 0xffffffff);
	eon_rect_rop_set(r, ENESIM_FILL);
	eon_rect_show(r);

	_setup_scene(c);
#if 0
	/* add a subcanvas */
	sc = eon_canvas_new(c);
	eon_canvas_x_rel_set(sc, 0);
	eon_canvas_y_rel_set(sc, 0);
	eon_canvas_w_rel_set(sc, 50);
	eon_canvas_h_rel_set(sc, 50);

	enesim_matrix_scale(&mx1, 0.8, 0.8);
	/*
	enesim_matrix_rotate(&mx1, 1.3);
	enesim_matrix_translate(&mx2, 60, 20);
	enesim_matrix_compose(&mx2, &mx1, &mx1);
	enesim_matrix_translate(&mx2, -60, -20);
	enesim_matrix_compose(&mx1, &mx2, &mx1);
	//enesim_matrix_translate(&mx1, 30, 30);

	*/
	eon_canvas_matrix_set(sc, &mx1);
	_setup_scene(sc);
	eon_canvas_show(sc);
#endif
	ekeko_object_dump((Ekeko_Object *)d, ekeko_object_dump_printf);
	eon_loop();
	eon_shutdown();

	return 0;
}

