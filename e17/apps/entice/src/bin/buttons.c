#include "entice.h"

void bt_tip_start(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  int i;
      
  for (i = 0; i < 5; i++)
    {
      double tw, th, x, y;
	
      evas_show(evas, o_txt_tip[i]);
      evas_set_layer(evas, o_txt_tip[i], 2000);
      evas_set_text(evas, o_txt_tip[i], _data);
      tw = evas_get_text_width(evas, o_txt_tip[i]);
      th = evas_get_text_height(evas, o_txt_tip[i]);
      x = ((double)win_w - tw) / 2;
      y = ((double)win_h - th) / 2;
      if      (i == 0)
	evas_move(evas, o_txt_tip[i], x - 1, y + 0);
      else if (i == 1)
	evas_move(evas, o_txt_tip[i], x + 0, y - 1);
      else if (i == 2)
	evas_move(evas, o_txt_tip[i], x + 1, y + 0);
      else if (i == 3)
	evas_move(evas, o_txt_tip[i], x + 0, y + 1);
      else if (i == 4)
	evas_move(evas, o_txt_tip[i], x + 0, y + 0);
      if (i == 4)
	evas_set_color(evas, o_txt_tip[i], 255, 255, 255, 255);
      else
	evas_set_color(evas, o_txt_tip[i], 0, 0, 0, 255);
    }
}

void bt_tip_stop (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  int i;
      
  for (i = 0; i < 5; i++) evas_hide(evas, o_txt_tip[i]);
}

void bt_close_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_set_image_file(_e, _o, IM"bt_close_2.png");
}

void bt_close_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_set_image_file(_e, _o, IM"bt_close_1.png");
  exit(0);
}

void bt_expand_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_set_image_file(_e, _o, IM"bt_expand_2.png");
}

void bt_expand_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_set_image_file(_e, _o, IM"bt_expand_1.png");
  if (o_image)
    {
      double sh, sv;
      int w, h;
	
      evas_get_image_size(evas, o_image, &w, &h);
      sh = (double)w / (double)win_w;
      sv = (double)h / (double)win_h;
      scale = sv;
      if (sh > sv) scale = sh;
      e_handle_resize();
    }
}

void bt_full_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_set_image_file(_e, _o, IM"bt_full_2.png");
}

void bt_full_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_set_image_file(_e, _o, IM"bt_full_1.png");
  e_toggle_fullscreen();
}

void bt_next_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_set_image_file(_e, _o, IM"bt_next_2.png");
}

void bt_next_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_set_image_file(_e, _o, IM"bt_next_1.png");
  if (!current_image)
    current_image = images;
  else
    {
      if (current_image->next)
	current_image = current_image->next;
    }
  e_display_current_image();
}

void bt_prev_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_set_image_file(_e, _o, IM"bt_prev_2.png");
}

void bt_prev_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_set_image_file(_e, _o, IM"bt_prev_1.png");
  if (!current_image)
    current_image = images;
  else
    current_image = current_image->prev;
  e_display_current_image();
}

void bt_zoom_in_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_set_image_file(_e, _o, IM"bt_zoom_in_2.png");
}

void bt_zoom_in_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_set_image_file(_e, _o, IM"bt_zoom_in_1.png");
  scale -= 0.5;
  if (scale < 0.5) scale = 0.5;
  e_handle_resize();
}

void bt_zoom_normal_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_set_image_file(_e, _o, IM"bt_zoom_normal_2.png");
}

void bt_zoom_normal_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_set_image_file(_e, _o, IM"bt_zoom_normal_1.png");
  scale = 1.0;
  e_handle_resize();
}

void bt_zoom_out_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_set_image_file(_e, _o, IM"bt_zoom_out_2.png");
}

void bt_zoom_out_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_set_image_file(_e, _o, IM"bt_zoom_out_1.png");
  scale += 0.5;
  e_handle_resize();
}

void e_slide_buttons_in(int v, void *data)
{
  /* int i; */
  static double start = 0.0;
  double duration = 0.5;
  double val;
  double px;
   
  buttons_active = 1;
   
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
   
  px =  win_w - 256 - ((256 * sin(val * 0.5 * 3.141592654)) - 256);

  evas_move(evas, o_bt_prev, px + 0, 0);
  evas_move(evas, o_bt_next, px + 32, 0);
  evas_move(evas, o_bt_zoom_normal, px + 64, 0);
  evas_move(evas, o_bt_zoom_in, px + 96, 0);
  evas_move(evas, o_bt_zoom_out, px + 128, 0);
  evas_move(evas, o_bt_expand, px + 160, 0);
  evas_move(evas, o_bt_full, px + 192, 0);
  evas_move(evas, o_bt_close, px + 224, 0);
   
  if (val < 1.0)
    ecore_add_event_timer("e_slide_buttons()", 0.05, e_slide_buttons_in, v + 1, NULL);
}

void e_slide_buttons_out(int v, void *data)
{
  /* int i; */
  static double start = 0.0;
  double duration = 0.5;
  double val;
  double px;
   
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
   
  px = win_w - 256 - ((256 * sin((1.0 - val) * 0.5 * 3.141592654)) - 256);

  evas_move(evas, o_bt_prev, px + 0, 0);
  evas_move(evas, o_bt_next, px + 32, 0);
  evas_move(evas, o_bt_zoom_normal, px + 64, 0);
  evas_move(evas, o_bt_zoom_in, px + 96, 0);
  evas_move(evas, o_bt_zoom_out, px + 128, 0);
  evas_move(evas, o_bt_expand, px + 160, 0);
  evas_move(evas, o_bt_full, px + 192, 0);
  evas_move(evas, o_bt_close, px + 224, 0);
   
  if (val < 1.0)
    ecore_add_event_timer("e_slide_buttons()", 0.05, e_slide_buttons_out, v + 1, NULL);
  else
    buttons_active = 0;
}

void show_buttons (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  if (!buttons_active)
    e_slide_buttons_in(0, NULL);
}
