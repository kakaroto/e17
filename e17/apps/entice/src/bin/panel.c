#include "entice.h"

void
e_slide_panel_in(int v, void *data)
{
   /* int i; */
   static double       start = 0.0;
   double              duration = 0.5;
   double              val;
   double              px;
   int                 w;
   int		       *force;

   force = (int *)data;

   if(panel_active == active_force_out && !(force && *force)) 
      return;

   if(force && *force)
      panel_active = active_force_in;
   else
      panel_active = active_in;

   if (v == 0)
      evas_object_layer_set(o_showpanel, 180);
   if (v == 0)
      start = get_time();
   val = (get_time() - start) / duration;

   evas_object_image_size_get(o_panel, &w, NULL);
   px = (w * sin(val * 0.5 * 3.141592654)) - w;
   evas_object_move(o_panel, px, 0);
   evas_object_move(o_panel_arrow_u, px, 0);
   evas_object_move(o_panel_arrow_d, px, win_h - 32);

   icon_x = (int)px;
   e_fix_icons();

   if (val < 1.0)
      ecore_add_event_timer("e_slide_panel()", 0.05, e_slide_panel_in, v + 1,
			    force);
}

void
e_slide_panel_out(int v, void *data)
{
   /* int i; */
   static double       start = 0.0;
   double              duration = 0.5;
   double              val;
   double              px;
   int                 w;
   int                 *force;

   force = (int *)data;

   if(panel_active == active_force_in && !(force && *force)) 
      return;

   if (v == 0)
      evas_object_layer_set(o_showpanel, 1000);
   if (v == 0)
      start = get_time();
   val = (get_time() - start) / duration;

   evas_object_image_size_get(o_panel, &w, NULL);
   px = (w * sin((1.0 - val) * 0.5 * 3.141592654)) - w;
   evas_object_move(o_panel, px, 0);
   evas_object_move(o_panel_arrow_u, px, 0);
   evas_object_move(o_panel_arrow_d, px, win_h - 32);

   icon_x = px;
   e_fix_icons();

   if (val < 1.0)
      ecore_add_event_timer("e_slide_panel()", 0.05, e_slide_panel_out, v + 1,
			    force);
   else
      if(force && *force) 
         panel_active = active_force_out;
      else
         panel_active = active_out;
}

void
show_panel(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   if (!panel_active)
      e_slide_panel_in(0, NULL);
}

void
hide_panel(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   if (panel_active)
      e_slide_panel_out(0, NULL);
   if (buttons_active)
      e_slide_buttons_out(0, NULL);
}
