#include "term.h"

void enterm_init(Ecore_Evas *ee, Evas_Object *term) {
   
   Term *t;

   t = evas_object_smart_data_get(term);
   ecore_evas_data_set(ee, "term", term);
   ecore_evas_callback_resize_set(ee, enterm_cb_resize);
   ecore_x_window_prop_step_size_set(ecore_evas_software_x11_window_get(ee),
				     t->font.width, t->font.height);
   ecore_evas_resize(ee, t->w, t->h);

}

void enterm_cb_resize(Ecore_Evas *ee) {   
   int x, y, w, h;
   Evas_Object *term;
   
   term = ecore_evas_data_get(ee, "term");   
   ecore_evas_geometry_get(ee, &x, &y, &w, &h);
   term_smart_resize(term , w, h);
      
}
