#include "entice.h"

void e_slide_panel_in(int v, void *data)
{
  /* int i; */
  static double start = 0.0;
  double duration = 0.5;
  double val;
  double px;
  int w;
   
  panel_active = 1;
   
  if (v == 0)
    evas_set_layer(evas, o_showpanel, 180);   
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
   
  evas_get_image_size(evas, o_panel, &w, NULL);
  px =  (w * sin(val * 0.5 * 3.141592654)) - w;
  evas_move(evas, o_panel, px, 0);
  evas_move(evas, o_panel_arrow_u, px, 0);
  evas_move(evas, o_panel_arrow_d, px, win_h - 32);

  icon_x = (int)px;
  e_fix_icons();
   
  if (val < 1.0)
    ecore_add_event_timer("e_slide_panel()", 0.05, e_slide_panel_in, v + 1, NULL);
}

void e_slide_panel_out(int v, void *data)
{
  /* int i; */
  static double start = 0.0;
  double duration = 0.5;
  double val;
  double px;
  int w;
   
  if (v == 0)
    evas_set_layer(evas, o_showpanel, 1000);   
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
   
  evas_get_image_size(evas, o_panel, &w, NULL);
  px =  (w * sin((1.0 - val) * 0.5 * 3.141592654)) - w;
  evas_move(evas, o_panel, px, 0);
  evas_move(evas, o_panel_arrow_u, px, 0);
  evas_move(evas, o_panel_arrow_d, px, win_h - 32);
   
  icon_x = px;
  e_fix_icons();
   
  if (val < 1.0)
    ecore_add_event_timer("e_slide_panel()", 0.05, e_slide_panel_out, v + 1, NULL);
  else
    panel_active = 0;     
}

void show_panel (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  if (!panel_active)
    e_slide_panel_in(0, NULL);
}

void hide_panel (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  if (panel_active)
    e_slide_panel_out(0, NULL);
  if (buttons_active)
    e_slide_buttons_out(0, NULL);
}
