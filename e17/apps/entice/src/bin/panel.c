#include "entice.h"

Ecore_Timer *panel_slide = NULL;

int
e_slide_panel(void * data)
{
   static double start;
   double duration = 0.5; // soon-to-be-configurable time taken to slide
   double val = 0.0;
   double delay = 0.05; // soon-to-be-configurable time between frames
   static enum active_state action;
   static Ecore_Timer *timer = NULL;
   int px, w;


   if (data) { // not called by timer
      if (!timer) { // we are starting afresh
         start = get_time();
      } else { // there is a slide already going on
	 start = 2*get_time() - duration - start;
	 ecore_timer_del(timer);
      }
      action = *(enum active_state *)data;
      panel_active = action;
      timer = ecore_timer_add(delay, e_slide_panel, NULL);
      return 1;
   } else

   val = (get_time() - start) / duration;
   if (val > 1.0) val = 1.0;

   evas_object_image_size_get(o_panel, &w, NULL);
   if (action == active_in || action == active_force_in)
      px = (w * sin(val * 0.5 * 3.141592654)) - w;
   else
      px = (w * sin((1.0 - val) * 0.5 * 3.141592654)) - w;

   evas_object_move(o_panel, px, 0);
   evas_object_move(o_panel_arrow_u, px, 0);
   evas_object_move(o_panel_arrow_d, px, win_h - 32);

   icon_x = px;
   e_fix_icons();

   if (val < 0.99) // keep going
      return 1;
   else { // stick a fork in us, we're done
      timer = NULL;
      return 0;
   }
}

int 
e_slide_panel_in(void* data)
{
   enum active_state command = active_in;
   printf("obsolete call to e_slide_panel_in");
   e_slide_panel(&command);
   return 1;
}

int 
e_slide_panel_out(void* data)
{
   enum active_state command = active_out;
   printf("obsolete call to e_slide_panel_out");
   e_slide_panel(&command);
   return 1;
}

void
show_panel(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   enum active_state command = active_in;
   e_slide_panel(&command);
}

void
hide_panel(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   enum active_state command = active_out;
   e_slide_panel(&command);
}
