#include "entice.h"

void e_fade_info_in(int v, void *data)
{
  int i, j;
  static double start = 0.0;
  double duration = 0.5;
  double val;
   
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
  if (val > 1.0) val = 1.0;
   
  for (j = 0; j < 2; j++)
    {
      for (i = 0; i < 5; i++)
	{
	  evas_show(evas, o_txt_info[(j * 5) + i]);
	  evas_set_layer(evas, o_txt_info[(j * 5) + i], 2000);
	  evas_set_text(evas, o_txt_info[(j * 5) + i], txt_info[j]);
	  if      (i == 0)
	    evas_move(evas, o_txt_info[(j * 5) + i], 0, (j * 20) + 1);
	  else if (i == 1)
	    evas_move(evas, o_txt_info[(j * 5) + i], 1, (j * 20));
	  else if (i == 2)
	    evas_move(evas, o_txt_info[(j * 5) + i], 2, (j * 20) + 1);
	  else if (i == 3)
	    evas_move(evas, o_txt_info[(j * 5) + i], 1, (j * 20) + 2);
	  else if (i == 4)
	    evas_move(evas, o_txt_info[(j * 5) + i], 1, (j * 20) + 1);
	  if (i == 4)
	    evas_set_color(evas, o_txt_info[(j * 5) + i], 255, 255, 255, (val * 255));
	  else
	    evas_set_color(evas, o_txt_info[(j * 5) + i], 0, 0, 0, (val * 255));
	}
    }
   
  if (val < 1.0)
    ecore_add_event_timer("e_fade_info_in()", 0.05, e_fade_info_in, v + 1, data);
  else e_fade_info_out(0, NULL);
}

void e_fade_info_out(int v, void *data)
{
  int i, j;
  static double start = 0.0;
  double duration = 8.0;
  double val;
   
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
  if (val > 1.0) val = 1.0;
  val = 1.0 - val;

  for (j = 0; j < 2; j++)
    {
      for (i = 0; i < 5; i++)
	{
	  if (val > 0.0)
	    evas_show(evas, o_txt_info[(j * 5) + i]);
	  else
	    evas_hide(evas, o_txt_info[(j * 5) + i]);
	  evas_set_layer(evas, o_txt_info[(j * 5) + i], 2000);
	  evas_set_text(evas, o_txt_info[(j * 5) + i], txt_info[j]);
	  if      (i == 0)
	    evas_move(evas, o_txt_info[(j * 5) + i], 0, (j * 20) + 1);
	  else if (i == 1)
	    evas_move(evas, o_txt_info[(j * 5) + i], 1, (j * 20));
	  else if (i == 2)
	    evas_move(evas, o_txt_info[(j * 5) + i], 2, (j * 20) + 1);
	  else if (i == 3)
	    evas_move(evas, o_txt_info[(j * 5) + i], 1, (j * 20) + 2);
	  else if (i == 4)
	    evas_move(evas, o_txt_info[(j * 5) + i], 1, (j * 20) + 1);
	  if (i == 4)
	    evas_set_color(evas, o_txt_info[(j * 5) + i], 255, 255, 255, (val * 255));
	  else
	    evas_set_color(evas, o_txt_info[(j * 5) + i], 0, 0, 0, (val * 255));
	}
    }
   
  if (val < 1.0)
    ecore_add_event_timer("e_fade_info_in()", 0.05, e_fade_info_out, v + 1, NULL);
}

void e_fade_scroller_in(int v, void *data)
{
  /* int i; */
  static double start = 0.0;
  double duration = 0.5;
  double val;
   
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
  if (val > 1.0) val = 1.0;
   
  evas_set_color(evas, o_mini_image, 255, 255, 255, (val * 255));
  evas_set_color(evas, o_mini_select, 255, 255, 255, (val * 255));
   
  if (val < 1.0)
    ecore_add_event_timer("e_fade_logo_in()", 0.05, e_fade_scroller_in, v + 1, data);
  if (data)
    e_fade_scroller_out(0, NULL);
}

void e_fade_scroller_out(int v, void *data)
{
  /* int i; */
  static double start = 0.0;
  double duration = 2.0;
  double val;
   
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
  if (val > 1.0) val = 1.0;
  val = 1.0 - val;
  evas_set_color(evas, o_mini_image, 255, 255, 255, (val * 255));
  evas_set_color(evas, o_mini_select, 255, 255, 255, (val * 255));
   
  if (val < 1.0)
    ecore_add_event_timer("e_fade_logo_in()", 0.05, e_fade_scroller_out, v + 1, NULL);
}

void e_fade_logo_in(int v, void *data)
{
  /* int i; */
  static double start = 0.0;
  double duration = 1.0;
  double val;
   
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
   
  evas_set_color(evas, o_logo, 255, 255, 255, (val * 255));
   
  if (val < 1.0)
    ecore_add_event_timer("e_fade_logo_in()", 0.05, e_fade_logo_in, v + 1, NULL);
}

void e_fade_l_arrow_in(int v, void *data)
{
  /* int i; */
  static double start = 0.0;
  double duration = 0.5;
  double val;
   
  if (arrow_l == 2) return;
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
   
  evas_set_color(evas, o_arrow_l, 255, 255, 255, (int)(val * 255));
   
  if (val < 1.0)
    {
      ecore_add_event_timer("e_fade_l_arrow_in()", 0.05, e_fade_l_arrow_in, v + 1, NULL);
      arrow_l = 1;
    }
  else
    arrow_l = 2;
  evas_show(evas, o_arrow_l);
}

void e_fade_l_arrow_out(int v, void *data)
{
  /* int i; */
  static double start = 0.0;
  double duration = 0.5;
  double val;
   
  if (arrow_l == 0) return;
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
   
  evas_set_color(evas, o_arrow_l, 255, 255, 255, ((1.0 - val) * 255));
   
  if (val < 1.0)
    {
      ecore_add_event_timer("e_fade_l_arrow_out()", 0.05, e_fade_l_arrow_out, v + 1, NULL);
      arrow_l = 1;
    }
  else
    {
      evas_hide(evas, o_arrow_l);
      arrow_l = 0;
    }
}

void e_fade_r_arrow_in(int v, void *data)
{
  /* int i; */
  static double start = 0.0;
  double duration = 0.5;
  double val;
   
  if (arrow_r == 2) return;
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
   
  evas_set_color(evas, o_arrow_r, 255, 255, 255, (int)(val * 255));
   
  if (val < 1.0)
    {
      ecore_add_event_timer("e_fade_r_arrow_in()", 0.05, e_fade_r_arrow_in, v + 1, NULL);
      arrow_r = 1;
    }
  else
    arrow_r = 2;
  evas_show(evas, o_arrow_r);
}

void e_fade_r_arrow_out(int v, void *data)
{
  /* int i; */
  static double start = 0.0;
  double duration = 0.5;
  double val;
   
  if (arrow_r == 0) return;
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
   
  evas_set_color(evas, o_arrow_r, 255, 255, 255, ((1.0 - val) * 255));
   
  if (val < 1.0)
    {
      ecore_add_event_timer("e_fade_r_arrow_out()", 0.05, e_fade_r_arrow_out, v + 1, NULL);
      arrow_r = 1;
    }
  else
    {
      evas_hide(evas, o_arrow_r);
      arrow_r = 0;
    }
}

void e_fade_t_arrow_in(int v, void *data)
{
  /* int i; */
  static double start = 0.0;
  double duration = 0.5;
  double val;
   
  if (arrow_t == 2) return;
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
   
  evas_set_color(evas, o_arrow_t, 255, 255, 255, (int)(val * 255));
   
  if (val < 1.0)
    {
      ecore_add_event_timer("e_fade_t_arrow_in()", 0.05, e_fade_t_arrow_in, v + 1, NULL);
      arrow_t = 1;
    }
  else
    arrow_t = 2;
  evas_show(evas, o_arrow_t);
}

void e_fade_t_arrow_out(int v, void *data)
{
  /* int i; */
  static double start = 0.0;
  double duration = 0.5;
  double val;
   
  if (arrow_t == 0) return;
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
   
  evas_set_color(evas, o_arrow_t, 255, 255, 255, ((1.0 - val) * 255));
   
  if (val < 1.0)
    {
      ecore_add_event_timer("e_fade_t_arrow_out()", 0.05, e_fade_t_arrow_out, v + 1, NULL);
      arrow_t = 1;
    }
  else
    {
      evas_hide(evas, o_arrow_t);
      arrow_t = 0;
    }
}

void e_fade_b_arrow_in(int v, void *data)
{
  /* int i; */
  static double start = 0.0;
  double duration = 0.5;
  double val;
   
  if (arrow_b == 2) return;
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
   
  evas_set_color(evas, o_arrow_b, 255, 255, 255, (int)(val * 255));
   
  if (val < 1.0)
    {
      ecore_add_event_timer("e_fade_b_arrow_in()", 0.05, e_fade_b_arrow_in, v + 1, NULL);
      arrow_b = 1;
    }
  else
    arrow_b = 2;
  evas_show(evas, o_arrow_b);
}

void e_fade_b_arrow_out(int v, void *data)
{
  /* int i; */
  static double start = 0.0;
  double duration = 0.5;
  double val;
   
  if (arrow_b == 0) return;
  if (v == 0) start = get_time();
  val = (get_time() - start) / duration;
   
  evas_set_color(evas, o_arrow_b, 255, 255, 255, ((1.0 - val) * 255));
   
  if (val < 1.0)
    {
      ecore_add_event_timer("e_fade_b_arrow_out()", 0.05, e_fade_b_arrow_out, v + 1, NULL);
      arrow_b = 1;
    }
  else
    {
      evas_hide(evas, o_arrow_b);
      arrow_b = 0;
    }
}
