#include "enscribe.h"

int main(int argc, char **argv)
{

   Ecore_Evas  *ee;   
   Evas        *evas;
   Ens_Window  *ens_window;   
   
   ee = ecore_evas_software_x11_new(0, 0, 0, 0, 640, 480);
   ecore_evas_show(ee);
   evas = ecore_evas_get(ee);      
   
   edje_init();
   ecore_init();
   ecore_evas_init();
   
   ens_window = ens_window_init(ee);
   ens_window_tab_add(ens_window);
      
   ecore_main_loop_begin();
   ecore_evas_shutdown();
   ecore_shutdown();
         
   return 0;
}
