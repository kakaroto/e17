#include "Etox_test.h"

/* globals */
Evas_Object *o_panel;
Evas_Object *o_txt_paneltitle;
Panel_Button *pbutton;

void setup_panel(Evas *_e)
{
	int w;
	Panel_Button *pbutton1, *pbutton2, *pbutton3;

	o_panel = evas_object_image_add(_e);
	evas_object_image_file_set(o_panel, IM "panel.png",
			IM "panel.png");

	evas_object_image_size_get(o_panel, &w, NULL);
	evas_object_move(o_panel, 0, 0);
	evas_object_resize(o_panel, w, win_h);
	evas_object_image_fill_set(o_panel, 0, 0, w, win_h);
	evas_object_layer_set(o_panel, 200);

	evas_object_show(o_panel);

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

}

Panel_Button *panel_button(Evas *_e, char *_label, Evas_List *tests)
{
	Evas_Object *o;
	static int y = 200;

	/*
	 * Create the new panel button's information
	 */
	pbutton = malloc(sizeof(Panel_Button));
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
	evas_object_resize(o, 85, 30);
	evas_object_image_fill_set(o, 0, 0, 85, 30);
	evas_object_move(o, 10, y);
	evas_object_layer_set(o, 250);
	evas_object_show(o);
	pbutton->box = o;

	o = evas_object_text_add(_e);
	evas_object_text_font_set(o, "andover", 24);
	evas_object_text_text_set(o, _label);
	evas_object_color_set(o, 0, 0, 0, 160);
	evas_object_layer_set(o, 251);
	evas_object_move(o, 15, y + 5);
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

	y += 60;

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
