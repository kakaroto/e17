#include "Etox_test.h"

/* globals */
Evas_Object *o_panel;
Evas_Object *o_showpanel;
Evas_Object *o_hidepanel;
Evas_Object *o_txt_paneltitle;
Panel_Button *pbutton;

int panel_active = 0;

void e_slide_panel_in(int v, void *data)
{
	static double start = 0.0;
	double duration = 0.5;
	double val;
	double px;
	int w;
	int y_offset = 40;
	double ascent, descent;
	Evas_List *l;

	panel_active = 1;

	if (v == 0)
		evas_object_layer_set(o_showpanel, 180);
	if (v == 0)
		start = get_time();
	val = (get_time() - start) / duration;

	evas_object_image_size_get(o_panel, &w, NULL);
	px = (w * sin(val * 0.5 * 3.141592654)) - w;
	evas_object_move(o_panel, px, 0);
	evas_object_move(o_txt_paneltitle, px + 4, 5);
	/* Pack the buttons in the panel in reverse */
	for (l = pbuttons->last; l; l = l->prev) {
		if (!(pbutton = l->data))
			return;
		evas_object_move(pbutton->box, px + 5, win_h - y_offset);
		evas_object_move(pbutton->label, px + 8,
			  win_h - y_offset + 2);
		y_offset += 40;
		ascent = evas_object_text_max_ascent_get(pbutton->label);
		descent = evas_object_text_max_descent_get(pbutton->label);
		evas_object_resize(pbutton->box, 108, ascent - descent + 4);
		evas_object_image_fill_set(pbutton->box, 0, 0, 108,
				    ascent - descent + 4);
	}
	if (val < 1.0)
		ecore_add_event_timer("e_slide_panel()", 0.05,
				      e_slide_panel_in, v + 1, NULL);

	return;
	data = NULL;
}

void e_slide_panel_out(int v, void *data)
{
	static double start = 0.0;
	double duration = 0.5;
	double val;
	double px;
	int w;
	int y_offset = 40;
	double ascent, descent;
	Evas_List *l;

	if (v == 0)
		evas_object_layer_set(o_showpanel, 1000);
	if (v == 0)
		start = get_time();
	val = (get_time() - start) / duration;

	evas_object_image_size_get(o_panel, &w, NULL);
	px = (w * sin((1.0 - val) * 0.5 * 3.141592654)) - w;
	evas_object_move(o_panel, px, 0);
	evas_object_move(o_txt_paneltitle, px + 4, 5);
	/* Pack the buttons in the panel in reverse */
	for (l = pbuttons->last; l; l = l->prev) {
		if (!(pbutton = l->data))
			return;
		evas_object_move(pbutton->box, px + 5, win_h - y_offset);
		evas_object_move(pbutton->label, px + 8, win_h - y_offset + 2);
		y_offset += 40;
		ascent = evas_object_text_max_ascent_get(pbutton->label);
		descent = evas_object_text_max_descent_get(pbutton->label);
		evas_object_resize(pbutton->box, 108, ascent - descent + 4);
		evas_object_image_fill_set(pbutton->box, 0, 0, 108,
				    ascent - descent + 4);
	}
	if (val < 1.0)
		ecore_add_event_timer("e_slide_panel()", 0.05,
				      e_slide_panel_out, v + 1, NULL);
	else
		panel_active = 0;

	return;
	data = NULL;
}

void
show_panel(void *_data, Evas *_e, Evas_Object *_o, void *event_info)
{
	if (!panel_active)
		e_slide_panel_in(0, NULL);

	return;
	_data = NULL;
	_e = NULL;
	_o = NULL;
	event_info = NULL;
}

void
hide_panel(void *_data, Evas *_e, Evas_Object *_o, void *event_info)
{
	if (panel_active)
		e_slide_panel_out(0, NULL);

	return;
	_data = NULL;
	_e = NULL;
	_o = NULL;
	event_info = NULL;
}

void setup_panel(Evas *_e)
{
	int w;
	Panel_Button *pbutton1, *pbutton2, *pbutton3;

	o_panel = evas_object_image_add(_e);
	evas_object_image_file_set(o_panel, IM "panel.png",
			IM "panel.png");
	o_showpanel = evas_object_rectangle_add(_e);
	o_hidepanel = evas_object_rectangle_add(_e);
	evas_object_color_set(o_showpanel, 0, 0, 0, 0);
	evas_object_color_set(o_hidepanel, 0, 0, 0, 0);
	evas_object_image_size_get(o_panel, &w, NULL);
	if (!panel_active)
		evas_object_move(o_panel, -w, 0);
	evas_object_resize(o_panel, w, win_h);
	evas_object_image_fill_set(o_panel, 0, 0, w, win_h);
	evas_object_layer_set(o_panel, 200);
	evas_object_resize(o_showpanel, 64, win_h);
	if (panel_active)
		evas_object_layer_set(o_showpanel, 180);
	else
		evas_object_layer_set(o_showpanel, 1000);
	evas_object_move(o_hidepanel, 128, 0);
	evas_object_resize(o_hidepanel, win_w - 128, win_h);
	evas_object_layer_set(o_hidepanel, 1000);
	evas_object_move(o_showpanel, 0, 0);
	evas_object_show(o_panel);
	evas_object_show(o_showpanel);
	evas_object_show(o_hidepanel);

	/* Panel title */
	o_txt_paneltitle = evas_object_text_add(_e);
	evas_object_text_font_set(o_txt_paneltitle, "sinon", 17);
	evas_object_text_text_set(o_txt_paneltitle, "Etox Test");
	evas_object_color_set(o_txt_paneltitle, 255, 255, 255, 255);
	evas_object_layer_set(o_txt_paneltitle, 250);
	evas_object_show(o_txt_paneltitle);

	/* Panel buttons */
	pbutton1 = panel_button(evas, "Basic", basic_tests());
	pbuttons = evas_list_append(pbuttons, pbutton1);

	pbutton2 = panel_button(evas, "Style", style_tests());
	pbuttons = evas_list_append(pbuttons, pbutton2);

	pbutton3 = panel_button(evas, "Callbacks", callback_tests());
	pbuttons = evas_list_append(pbuttons, pbutton3);

	e_slide_panel_out(0, NULL);

	/* Callbacks */
	evas_object_event_callback_add(o_showpanel, EVAS_CALLBACK_MOUSE_IN, show_panel,
			  NULL);
	evas_object_event_callback_add(o_hidepanel, EVAS_CALLBACK_MOUSE_IN, hide_panel,
			  NULL);

}

Panel_Button *panel_button(Evas *_e, char *_label, Evas_List *tests)
{
	Evas_Object *o;

	/*
	 * Create the new panel button's information
	 */
	pbutton = malloc(sizeof(Panel_Button));
	pbutton->evas = _e;
	pbutton->box = NULL;
	pbutton->label = NULL;

	/*
	 * Now initialize it's fields, create the appearance, and add it to
	 * the evas.
	 */
	o = evas_object_image_add(evas);
	evas_object_image_file_set(o, IM "panel_button1.png",
			IM "panel_button1.png");
	evas_object_image_border_set(o, 3, 3, 3, 3);
	evas_object_layer_set(o, 250);
	evas_object_show(o);
	pbutton->box = o;

	o = evas_object_text_add(_e);
	evas_object_text_font_set(o, "andover", 24);
	evas_object_text_text_set(o, _label);
	evas_object_color_set(o, 0, 0, 0, 160);
	evas_object_layer_set(o, 250);
	evas_object_show(o);
	pbutton->label = o;

	/*
	 * Attach the buttons callbacks
	 */
	evas_object_event_callback_add(pbutton->label, EVAS_CALLBACK_MOUSE_IN,
			  button_mouse_in, NULL);
	evas_object_event_callback_add(pbutton->label, EVAS_CALLBACK_MOUSE_OUT,
			  button_mouse_out, NULL);

	evas_object_event_callback_add(pbutton->label, EVAS_CALLBACK_MOUSE_DOWN,
			  button_mouse_down, tests);

	return pbutton;
}

void panel_button_free(Panel_Button * pbutton)
{
	if (pbutton->box)
		evas_object_del(pbutton->box);
	if (pbutton->label)
		evas_object_del(pbutton->label);
	free(pbutton);
}
