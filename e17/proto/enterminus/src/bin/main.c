#include "term.h"

int main(int argc, char **argv) {

   Ecore_Evas  *ee;     /* ecore_evas */
   Evas        *evas;   /* evas */
   Evas_Object *term;   /* terminal   */
      
   ee = ecore_evas_software_x11_new(0, 0, 0, 0, 0, 0);
   ecore_evas_show(ee);   
   evas = ecore_evas_get(ee);
      
   term = term_new(evas);
   evas_object_show(term);
   //evas_object_resize(term, 640, 480);
   
   enterm_init(ee, term);   
   
   ecore_main_loop_begin();
   ecore_evas_shutdown();
   ecore_shutdown();

   return 0;
}
