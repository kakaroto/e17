#include "entice.h"

void
bt_tip_start(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   int                 i;

   for (i = 0; i < 5; i++)
     {
	double              tw, th, x, y;

	evas_object_show(o_txt_tip[i]);
	evas_object_layer_set(o_txt_tip[i], 2000);
	evas_object_text_text_set(o_txt_tip[i], data);
	evas_object_geometry_get(o_txt_tip[i], NULL, NULL, &tw, &th);
	x = ((double)win_w - tw) / 2;
	y = ((double)win_h - th) / 2;
	if (i == 0)
	   evas_object_move(o_txt_tip[i], x - 1, y + 0);
	else if (i == 1)
	   evas_object_move(o_txt_tip[i], x + 0, y - 1);
	else if (i == 2)
	   evas_object_move(o_txt_tip[i], x + 1, y + 0);
	else if (i == 3)
	   evas_object_move(o_txt_tip[i], x + 0, y + 1);
	else if (i == 4)
	   evas_object_move(o_txt_tip[i], x + 0, y + 0);
	if (i == 4)
	   evas_object_color_set(o_txt_tip[i], 255, 255, 255, 255);
	else
	   evas_object_color_set(o_txt_tip[i], 0, 0, 0, 255);
     }
}

void
bt_tip_stop(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   int                 i;

   for (i = 0; i < 5; i++)
      evas_object_hide(o_txt_tip[i]);
}

void
bt_close_down(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_close_2.png", NULL);
}

void
bt_close_up(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_close_1.png", NULL);
   exit(0);
}

void
bt_delete_down(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_delete_2.png", NULL);
}

void
bt_delete_up(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_delete_1.png", NULL);
   e_delete_current_image();
}

void
bt_expand_down(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_expand_2.png", NULL);
}

void
bt_expand_up(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_expand_1.png", NULL);
   if (o_image)
     {
	double              sh, sv;
	int                 w, h;

	evas_object_image_size_get(o_image, &w, &h);
	sh = (double)w / (double)win_w;
	sv = (double)h / (double)win_h;
	scale = sv;
	if (sh > sv)
	   scale = sh;
	e_handle_resize();
     }
}

void
bt_full_down(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_full_2.png", NULL);
}

void
bt_full_up(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_full_1.png", NULL);
   e_toggle_fullscreen();
}

void
bt_next_down(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_next_2.png", NULL);
}

void
bt_next_up(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_next_1.png", NULL);
   e_load_next_image();
}

void
bt_prev_down(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_prev_2.png", NULL);
}

void
bt_prev_up(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_prev_1.png", NULL);
   e_load_prev_image();
}

void
bt_zoom_in_down(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_zoom_in_2.png", NULL);
}

void
bt_zoom_in_up(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_zoom_in_1.png", NULL);
   scale /= 1.414;
   if (scale > 0.03125)
     scale = 0.03125;
   e_handle_resize();
}

void
bt_zoom_normal_down(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_zoom_normal_2.png", NULL);
}

void
bt_zoom_normal_up(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_zoom_normal_1.png", NULL);
   scale = 1.0;
   e_handle_resize();
}

void
bt_zoom_out_down(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_zoom_out_2.png", NULL);
}

void
bt_zoom_out_up(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   evas_object_image_file_set(obj, IM "bt_zoom_out_1.png", NULL);
   scale *= 1.414;
   e_handle_resize();
}

int
e_slide_buttons(void * data)
{
   static double start;
   double duration = 1.0; // soon-to-be-configurable time taken to slide
   double val = 0.0;
   double delay = 0.05; // soon-to-be-configurable time between frames
   static enum active_state action;
   static Ecore_Timer *timer = NULL;
   int px;


   if (data) { // not called by timer
      action = *(enum active_state *)data;
      if (!timer) { // we are starting afresh
	 if ((buttons_active == active_force_in && action != active_force_out) ||
	     (buttons_active == active_force_out && action != active_force_in) ||
	     (buttons_active == active_in && action == active_in) ||
	     (buttons_active == active_out && action == active_out))
	   return 0;
         start = get_time();
	 buttons_active = active_running;
      } else { // there is a slide already going on
	 start = 2*get_time() - duration - start;
	 ecore_timer_del(timer);
      }
      timer = ecore_timer_add(delay, e_slide_buttons, NULL);
      return 1;
   } else
      val = (get_time() - start) / duration;
   if (val > 1.0) val = 1.0;
   if (action == active_in || action == active_force_in)
      px = win_w - (288 * sin(val * 0.5 * 3.141592654));
   else
      px = win_w - (288 * sin((1.0 - val) * 0.5 * 3.141592654));

   evas_object_move(o_bt_prev, px + 0, 0);
   evas_object_move(o_bt_next, px + 32, 0);
   evas_object_move(o_bt_zoom_normal, px + 64, 0);
   evas_object_move(o_bt_zoom_in, px + 96, 0);
   evas_object_move(o_bt_zoom_out, px + 128, 0);
   evas_object_move(o_bt_expand, px + 160, 0);
   evas_object_move(o_bt_full, px + 192, 0);
   evas_object_move(o_bt_delete, px + 224, 0);
   evas_object_move(o_bt_close, px + 256, 0);

   if (val < 0.99) // keep going
      return 1;
   else { // stick a fork in us, we're done
      buttons_active = action;
      timer = NULL;
      return 0;
   }
}

void
show_buttons(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   enum active_state command = active_in;
   e_slide_buttons(&command);
}

void hide_buttons(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   enum active_state command = active_out;
   e_slide_buttons(&command);
}
