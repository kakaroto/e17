#include "Etox_test.h"

void
e_slide_panel_in (int v, void *data)
{
  static double start = 0.0;
  double duration = 0.5;
  double val;
  double px;
  int w;
  double ascent, descent;

  panel_active = 1;

  if (v == 0)
    evas_set_layer (evas, o_showpanel, 180);
  if (v == 0)
    start = get_time ();
  val = (get_time () - start) / duration;

  evas_get_image_size (evas, o_panel, &w, NULL);
  px = (w * sin (val * 0.5 * 3.141592654)) - w;
  evas_move (evas, o_panel, px, 0);
  evas_move (evas, o_txt_paneltitle, px + 4, 5);
  evas_move (evas, o_panel_box1, px + 5, 40);
  evas_move (evas, o_txt_panel_box1, px + 8, 42);
  evas_text_get_max_ascent_descent (evas, o_txt_panel_box1, &ascent,
				    &descent);
  evas_resize (evas, o_panel_box1, 108, ascent - descent + 4);
  evas_set_image_fill (evas, o_panel_box1, 0, 0, 108, ascent - descent + 4);
  if (val < 1.0)
    ecore_add_event_timer ("e_slide_panel()", 0.05, e_slide_panel_in, v + 1,
			   NULL);
}

void
e_slide_panel_out (int v, void *data)
{
  static double start = 0.0;
  double duration = 0.5;
  double val;
  double px;
  int w;
  double ascent, descent;

  if (v == 0)
    evas_set_layer (evas, o_showpanel, 1000);
  if (v == 0)
    start = get_time ();
  val = (get_time () - start) / duration;

  evas_get_image_size (evas, o_panel, &w, NULL);
  px = (w * sin ((1.0 - val) * 0.5 * 3.141592654)) - w;
  evas_move (evas, o_panel, px, 0);
  evas_move (evas, o_txt_paneltitle, px + 4, 5);
  evas_move (evas, o_panel_box1, px + 5, 40);
  evas_move (evas, o_txt_panel_box1, px + 8, 42);
  evas_text_get_max_ascent_descent (evas, o_txt_panel_box1, &ascent,
				    &descent);
  evas_resize (evas, o_panel_box1, 108, ascent - descent + 4);
  evas_set_image_fill (evas, o_panel_box1, 0, 0, 108, ascent - descent + 4);
  if (val < 1.0)
    ecore_add_event_timer ("e_slide_panel()", 0.05, e_slide_panel_out, v + 1,
			   NULL);
  else
    panel_active = 0;
}

void
show_panel (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  if (!panel_active)
    e_slide_panel_in (0, NULL);
}

void
hide_panel (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  if (panel_active)
    e_slide_panel_out (0, NULL);
}

void
panel_button (Evas _e, char *_label)
{
  Evas_Object o;

  o = evas_add_image_from_file (evas, IM "panel_button1.png");
  evas_set_image_border (_e, o, 3, 3, 3, 3);
  evas_set_layer (_e, o, 250);
  evas_show (_e, o);
  o_panel_box1 = o;
  o_txt_panel_box1 = evas_add_text (_e, "andover", 24, _label);
  evas_set_color (_e, o_txt_panel_box1, 0, 0, 0, 160);
  evas_set_layer (_e, o_txt_panel_box1, 250);
  evas_show (_e, o_txt_panel_box1);
}


