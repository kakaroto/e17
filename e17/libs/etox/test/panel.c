#include "Etox_test.h"
  
/* globals */
Evas_Object o_panel;
Evas_Object o_showpanel;
Evas_Object o_hidepanel;
Evas_Object o_txt_paneltitle;
Panel_Button *pbutton;

int panel_active = 0;

void
e_slide_panel_in (int v, void *data)
{
  static double start = 0.0;
  double duration = 0.5;
  double val;
  double px;
  int w;
  int y_offset = 40;
  double ascent, descent;
  Evas_List l;

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
  /* Pack the buttons in the panel in reverse */
  for (l = pbuttons->last; l; l = l->prev)
  {
    if (!(pbutton = l->data))
      return;
    evas_move (evas, pbutton->box, px + 5, win_h - y_offset);
    evas_move (evas, pbutton->label, px + 8, win_h - y_offset + 2);
    y_offset += 40;
    evas_text_get_max_ascent_descent (evas, pbutton->label, &ascent,
              &descent);
    evas_resize (evas, pbutton->box, 108, ascent - descent + 4);
    evas_set_image_fill (evas, pbutton->box, 0, 0, 108, ascent - descent + 4);
  }
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
  int y_offset = 40;
  double ascent, descent;
  Evas_List l;

  if (v == 0)
    evas_set_layer (evas, o_showpanel, 1000);
  if (v == 0)
    start = get_time ();
  val = (get_time () - start) / duration;

  evas_get_image_size (evas, o_panel, &w, NULL);
  px = (w * sin ((1.0 - val) * 0.5 * 3.141592654)) - w;
  evas_move (evas, o_panel, px, 0);
  evas_move (evas, o_txt_paneltitle, px + 4, 5);
  /* Pack the buttons in the panel in reverse */
  for (l = pbuttons->last; l; l = l->prev)
  {
    if (!(pbutton = l->data))
      return;
    evas_move (evas, pbutton->box, px + 5, win_h - y_offset);
    evas_move (evas, pbutton->label, px + 8, win_h - y_offset + 2);
    y_offset += 40;
    evas_text_get_max_ascent_descent (evas, pbutton->label, &ascent,
              &descent);
    evas_resize (evas, pbutton->box, 108, ascent - descent + 4);
    evas_set_image_fill (evas, pbutton->box, 0, 0, 108, ascent - descent + 4);
  }
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
setup_panel (Evas _e)
{
  int w;
  Panel_Button *pbutton1, *pbutton2;
  Evas_List l;

  o_panel = evas_add_image_from_file (_e, IM "panel.png");
  o_showpanel = evas_add_rectangle (_e);
  o_hidepanel = evas_add_rectangle (_e);
  evas_set_color (_e, o_showpanel, 0, 0, 0, 0);
  evas_set_color (_e, o_hidepanel, 0, 0, 0, 0);
  evas_get_image_size (_e, o_panel, &w, NULL);
  if (!panel_active)
    evas_move (_e, o_panel, -w, 0);
  evas_resize (_e, o_panel, w, win_h);
  evas_set_layer (_e, o_panel, 200);
  evas_resize (_e, o_showpanel, 64, win_h);
  if (panel_active)
    evas_set_layer (_e, o_showpanel, 180);
  else
    evas_set_layer (_e, o_showpanel, 1000);
  evas_move (_e, o_hidepanel, 128, 0);
  evas_resize (_e, o_hidepanel, win_w - 128, win_h);
  evas_set_layer (_e, o_hidepanel, 1000);
  evas_move (_e, o_showpanel, 0, 0);
  evas_show (_e, o_panel);
  evas_show (_e, o_showpanel);
  evas_show (_e, o_hidepanel);

  /* Panel title */
  o_txt_paneltitle = evas_add_text (_e, "sinon", 17, "Etox Test");
  evas_set_color (_e, o_txt_paneltitle, 255, 255, 255, 255);
  evas_set_layer (_e, o_txt_paneltitle, 250);
  evas_show (_e, o_txt_paneltitle);

  /* Panel buttons */
  pbutton1 = panel_button (evas, "Basic");
  pbuttons = evas_list_append(pbuttons, pbutton1);
  pbutton2 = panel_button (evas, "Style");
  pbuttons = evas_list_append(pbuttons, pbutton2);

  e_slide_panel_out (0, NULL);

  /* Callbacks */
  evas_callback_add (evas, o_showpanel, CALLBACK_MOUSE_IN, show_panel, NULL);
  evas_callback_add (evas, o_hidepanel, CALLBACK_MOUSE_IN, hide_panel, NULL);
  for (l = pbuttons; l; l = l->next)
  {
    pbutton = l->data;
  evas_callback_add (evas, pbutton->label, CALLBACK_MOUSE_IN, button_mouse_in,
		     NULL);
  evas_callback_add (evas, pbutton->label, CALLBACK_MOUSE_OUT, button_mouse_out,
		     NULL);
  }
  evas_callback_add (evas, pbutton1->label, CALLBACK_MOUSE_DOWN, test_basic,
		     NULL);
  evas_callback_add (evas, pbutton2->label, CALLBACK_MOUSE_DOWN, test_style,
		     NULL);
}

Panel_Button
*panel_button (Evas _e, char *_label)
{
  Evas_Object o;

  pbutton = panel_button_new (_e);
  o = evas_add_image_from_file (evas, IM "panel_button1.png");
  evas_set_image_border (_e, o, 3, 3, 3, 3);
  evas_set_layer (_e, o, 250);
  evas_show (_e, o);
  pbutton->box = o;
  o = evas_add_text (_e, "andover", 24, _label);
  evas_set_color (_e, o, 0, 0, 0, 160);
  evas_set_layer (_e, o, 250);
  evas_show (_e, o);
  pbutton->label = o;
  return pbutton;
}

Panel_Button
*panel_button_new (Evas _e)
{
  pbutton = malloc (sizeof (Panel_Button));
  pbutton->evas = _e;
  pbutton->box = NULL;
  pbutton->label = NULL;
  return pbutton;
}

void
panel_button_free (Evas _e, Panel_Button *pbutton)
{
  if (pbutton->box)
    evas_del_object (_e, pbutton->box);
  if (pbutton->label)
    evas_del_object (_e, pbutton->label);
  free (pbutton);
}
