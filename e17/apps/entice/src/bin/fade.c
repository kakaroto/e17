#include "entice.h"

Ecore_Timer * fade_timer;

int
e_fade_info_in(void *data)
{
   int                 i, j;
   static int	       v=0;
   static double       start = 0.0;
   double              duration = 0.5;
   double              val;

   if (v == 0) {
      start = get_time();
      fade_timer = ecore_timer_add(0.05, e_fade_info_in, data);
   }
   val = (get_time() - start) / duration;
   if (val > 1.0)
      val = 1.0;

   for (j = 0; j < 2; j++)
     {
	for (i = 0; i < 5; i++)
	  {
	     evas_object_show(o_txt_info[(j * 5) + i]);
	     evas_object_layer_set(o_txt_info[(j * 5) + i], 2000);
	     evas_object_text_text_set(o_txt_info[(j * 5) + i], txt_info[j]);
	     if (i == 0)
		evas_object_move(o_txt_info[(j * 5) + i], 0, (j * 20) + 1);
	     else if (i == 1)
		evas_object_move(o_txt_info[(j * 5) + i], 1, (j * 20));
	     else if (i == 2)
		evas_object_move(o_txt_info[(j * 5) + i], 2, (j * 20) + 1);
	     else if (i == 3)
		evas_object_move(o_txt_info[(j * 5) + i], 1, (j * 20) + 2);
	     else if (i == 4)
		evas_object_move(o_txt_info[(j * 5) + i], 1, (j * 20) + 1);
	     if (i == 4)
		evas_object_color_set(o_txt_info[(j * 5) + i], 255, 255, 255,
				      (val * 255));
	     else
		evas_object_color_set(o_txt_info[(j * 5) + i], 0, 0, 0,
				      (val * 255));
	  }
     }

   if (val < 1.0) {
      v++;
      return 1;
   } else {
      v = 0;
      return 0;
   }
}

int
e_fade_info_out(void *data)
{
   int                 i, j;
   static int	       v = 0;
   static double       start = 0.0;
   double              duration = 8.0;
   double              val;

   if (v == 0) {
      start = get_time();
      fade_timer = ecore_timer_add(0.05, e_fade_info_out, data);
   }
   val = (get_time() - start) / duration;
   if (val > 1.0)
      val = 1.0;
   val = 1.0 - val;

   for (j = 0; j < 2; j++)
     {
	for (i = 0; i < 5; i++)
	  {
	     if (val > 0.0)
		evas_object_show(o_txt_info[(j * 5) + i]);
	     else
		evas_object_hide(o_txt_info[(j * 5) + i]);
	     evas_object_layer_set(o_txt_info[(j * 5) + i], 2000);
	     evas_object_text_text_set(o_txt_info[(j * 5) + i], txt_info[j]);
	     if (i == 0)
		evas_object_move(o_txt_info[(j * 5) + i], 0, (j * 20) + 1);
	     else if (i == 1)
		evas_object_move(o_txt_info[(j * 5) + i], 1, (j * 20));
	     else if (i == 2)
		evas_object_move(o_txt_info[(j * 5) + i], 2, (j * 20) + 1);
	     else if (i == 3)
		evas_object_move(o_txt_info[(j * 5) + i], 1, (j * 20) + 2);
	     else if (i == 4)
		evas_object_move(o_txt_info[(j * 5) + i], 1, (j * 20) + 1);
	     if (i == 4)
		evas_object_color_set(o_txt_info[(j * 5) + i], 255, 255, 255,
				      (val * 255));
	     else
		evas_object_color_set(o_txt_info[(j * 5) + i], 0, 0, 0,
				      (val * 255));
	  }
     }

   if (val > 0.0) {
      v++;
      return 1;
   } else {
      v = 0;
      return 0;
   }
}

int
e_fade_scroller(void * data)
{
   static double start = 0.0;
   double duration = 0.5; // soon-to-be-configurable time taken to slide
   double val = 0.0;
   double delay = 0.05; // soon-to-be-configurable time between frames
   static enum active_state action;
   static Ecore_Timer *timer = NULL;

   if (!o_mini_image)
      return 0;
   if (data) { // not called by timer
      if (!timer) { // we are starting afresh
         start = get_time();
      } else { // there is a fade already going on
	 start = 2*get_time() - duration - start;
	 ecore_timer_del(timer);
      }
      timer = ecore_timer_add(delay, e_fade_scroller, NULL);
      action = *(enum active_state *)data;
      buttons_active = action;
      return 1;
   } else
      val = (get_time() - start) / duration;
   if (val > 1.0) val = 1.0;
   if (action == active_in) {
      evas_object_color_set(o_mini_image, 255, 255, 255, (val * 255));
      evas_object_color_set(o_mini_select, 255, 255, 255, (val * 255));
   } else {
      evas_object_color_set(o_mini_image, 255, 255, 255, ((1.0 - val) * 255));
      evas_object_color_set(o_mini_select, 255, 255, 255, ((1.0 - val) * 255));
   }

   if (val < 0.99) // keep going
      return 1;
   else { // stick a fork in us, we're done
      timer = NULL;
      return 0;
   }
}
/*
int
e_fade_scroller_in(void *data)
{
   * int i; *
   static int	       v = 0;
   static double       start = 0.0;
   double              duration = 0.5;
   double              val;

   if (v == 0) {
      start = get_time();
      fade_timer = ecore_timer_add(0.05, e_fade_scroller_in, data);
   }
   val = (get_time() - start) / duration;
   if (val > 1.0)
      val = 1.0;

   if (o_mini_image)
         evas_object_color_set(o_mini_image, 255, 255, 255, (val * 255));
   evas_object_color_set(o_mini_select, 255, 255, 255, (val * 255));

   if (val < 1.0) {
      v++;
      return 1;
   } else {
      v = 0;
      return 0;
   }
   if (data)
      e_fade_scroller_out(NULL);
}

int
e_fade_scroller_out(void *data)
{
   * int i; *
   static int	       v = 0;
   static double       start = 0.0;
   double              duration = 2.0;
   double              val;

   if (v == 0) {
      start = get_time();
      fade_timer = ecore_timer_add(0.05, e_fade_scroller_out, data);
   }
   val = (get_time() - start) / duration;
   if (val > 1.0)
      val = 1.0;
   val = 1.0 - val;
   if (o_mini_image)
         evas_object_color_set(o_mini_image, 255, 255, 255, (val * 255));
   evas_object_color_set(o_mini_select, 255, 255, 255, (val * 255));

   if (val > 0.0) {
      v++;
      return 1;
   } else {
      v = 0;
      return 0;
   }
}
*/

int
e_fade_scroller_in(void *data) {
   enum active_state command = active_in;
   e_fade_scroller(&command);
   return 1;
}

int
e_fade_scroller_out(void *data) {
   enum active_state command = active_out;
   e_fade_scroller(&command);
   return 1;
}

int
e_fade_trash_in(void *data)
{
   static int	       v = 0;
   static double       start = 0.0;
   double              duration = 0.5;
   double              val;

   if (v == 0) {
      start = get_time();
      fade_timer = ecore_timer_add(0.05, e_fade_trash_in, data);
   }
   val = (get_time() - start) / duration;
   if (val > 1.0)
      val = 1.0;

   evas_object_color_set(o_trash, 255, 255, 255, (val * 175));
   evas_object_color_set(o_trash_can, 255, 255, 255, (val * 255));

   if (val < 1.0) {
      v++;
      return 1;
   } else {
      v = 0;
      return 0;
   }
}

int
e_fade_trash_out(void *data)
{
   static int	       v = 0;
   static double       start = 0.0;
   double              duration = 0.5;
   double              val;

   if (v == 0) {
      start = get_time();
      fade_timer = ecore_timer_add(0.05, e_fade_trash_out, data);
   }
   val = (get_time() - start) / duration;
   if (val > 1.0)
      val = 1.0;
   val = 1.0 - val;

   evas_object_color_set(o_trash, 255, 255, 255, (val * 175));
   evas_object_color_set(o_trash_can, 255, 255, 255, (val * 255));

   if (val > 0.0) {
      v++;
      return 1;
   } else {
      v = 0;
      return 0;
   }
}

int
e_fade_logo_in(void *data)
{
   /* int i; */
   static int	       v = 0;
   static double       start = 0.0;
   double              duration = 1.0;
   double              val;

   if (v == 0) {
      start = get_time();
      fade_timer = ecore_timer_add(0.05, e_fade_logo_in, data);
   }
   val = (get_time() - start) / duration;

   evas_object_color_set(o_logo, 255, 255, 255, (val * 255));

   if (val < 1.0) {
      v++;
      return 1;
   } else {
      v = 0;
      return 0;
   }
}

int
e_fade_l_arrow_in(void *data)
{
   /* int i; */
   static int	       v = 0;
   static double       start = 0.0;
   double              duration = 0.5;
   double              val;

   if (arrow_l == 2)
      return 0;
   if (v == 0) {
      start = get_time();
      fade_timer = ecore_timer_add(0.05, e_fade_l_arrow_in, data);
   }
   val = (get_time() - start) / duration;

   evas_object_color_set(o_arrow_l, 255, 255, 255, (int)(val * 255));

   if (val < 1.0) {
      arrow_l = 1;
      v++;
      evas_object_show(o_arrow_l);
      return 1;
   } else {
      arrow_l = 2;
      v = 0;
      evas_object_show(o_arrow_l);
      return 0;
   }
}

int
e_fade_l_arrow_out(void *data)
{
   /* int i; */
   static int	       v = 0;
   static double       start = 0.0;
   double              duration = 0.5;
   double              val;

   if (arrow_l == 0)
      return 0;
   if (v == 0) {
      start = get_time();
      fade_timer = ecore_timer_add(0.05, e_fade_l_arrow_out, data);
   }
   val = (get_time() - start) / duration;

   evas_object_color_set(o_arrow_l, 255, 255, 255, ((1.0 - val) * 255));

   if (val < 1.0)
     {
	arrow_l = 1;
	v++;
	return 1;
     }
   else
     {
	evas_object_hide(o_arrow_l);
	arrow_l = 0;
	return 0;
     }
}

int
e_fade_r_arrow_in(void *data)
{
   /* int i; */
   static int	       v = 0;
   static double       start = 0.0;
   double              duration = 0.5;
   double              val;

   if (arrow_r == 2)
      return 0;
   if (v == 0) {
      start = get_time();
      fade_timer = ecore_timer_add(0.05, e_fade_r_arrow_in, data);
   }
   val = (get_time() - start) / duration;

   evas_object_color_set(o_arrow_r, 255, 255, 255, (int)(val * 255));

   if (val < 1.0) {
      v++;
      arrow_r = 1;
      evas_object_show(o_arrow_r);
      return 1;
   } else {
      arrow_r = 2;
      v = 0;
      evas_object_show(o_arrow_r);
      return 0;
   }
}

int
e_fade_r_arrow_out(void *data)
{
   /* int i; */
   static int	       v = 0;
   static double       start = 0.0;
   double              duration = 0.5;
   double              val;

   if (arrow_r == 0)
      return 0;
   if (v == 0) {
      start = get_time();
      fade_timer = ecore_timer_add(0.05, e_fade_r_arrow_out, data);
   }
   val = (get_time() - start) / duration;

   evas_object_color_set(o_arrow_r, 255, 255, 255, ((1.0 - val) * 255));

   if (val < 1.0) {
      v++;
      arrow_r = 1;
      return 1;
   } else {
      evas_object_hide(o_arrow_r);
      arrow_r = 0;
      v = 0;
      return 0;
   }
}

int
e_fade_t_arrow_in(void *data)
{
   /* int i; */
   static int	       v = 0;
   static double       start = 0.0;
   double              duration = 0.5;
   double              val;

   if (arrow_t == 2)
      return 0;
   if (v == 0) {
      start = get_time();
      fade_timer = ecore_timer_add(0.05, e_fade_t_arrow_in, data);
   }
   val = (get_time() - start) / duration;

   evas_object_color_set(o_arrow_t, 255, 255, 255, (int)(val * 255));

   if (val < 1.0) {
      v++;
      arrow_t = 1;
      evas_object_show(o_arrow_t);
      return 1;
   } else {
      v = 0;
      arrow_t = 2;
      evas_object_show(o_arrow_t);
      return 0;
   }
}

int
e_fade_t_arrow_out(void *data)
{
   /* int i; */
   static int	       v = 0;
   static double       start = 0.0;
   double              duration = 0.5;
   double              val;

   if (arrow_t == 0)
      return 0;
   if (v == 0) {
      start = get_time();
      fade_timer = ecore_timer_add(0.05, e_fade_t_arrow_out, data);
   }
   val = (get_time() - start) / duration;

   evas_object_color_set(o_arrow_t, 255, 255, 255, ((1.0 - val) * 255));

   if (val < 1.0) {
      v++;
      arrow_t = 1;
      return 1;
   } else {
      evas_object_hide(o_arrow_t);
      arrow_t = 0;
      v = 0;
      return 0;
   }
}

int
e_fade_b_arrow_in(void *data)
{
   /* int i; */
   static int	       v = 0;
   static double       start = 0.0;
   double              duration = 0.5;
   double              val;

   if (arrow_b == 2)
      return 0;
   if (v == 0) {
      start = get_time();
      fade_timer = ecore_timer_add(0.05, e_fade_b_arrow_in, data);
   }
   val = (get_time() - start) / duration;

   evas_object_color_set(o_arrow_b, 255, 255, 255, (int)(val * 255));

   if (val < 1.0) {
      arrow_b = 1;
      v++;
      evas_object_show(o_arrow_b);
      return 1;
   } else {
      arrow_b = 2;
      v = 0;
      evas_object_show(o_arrow_b);
      return 0;
   }
}

int
e_fade_b_arrow_out(void *data)
{
   /* int i; */
   static int	       v = 0;
   static double       start = 0.0;
   double              duration = 0.5;
   double              val;

   if (arrow_b == 0)
      return 0;
   if (v == 0) {
      start = get_time();
      fade_timer = ecore_timer_add(0.05, e_fade_b_arrow_out, data);
   }
   val = (get_time() - start) / duration;

   evas_object_color_set(o_arrow_b, 255, 255, 255, ((1.0 - val) * 255));

   if (val < 1.0)
     {
	arrow_b = 1;
	v++;
	return 1;
     }
   else
     {
	evas_object_hide(o_arrow_b);
	arrow_b = 0;
	return 0;
     }
}
