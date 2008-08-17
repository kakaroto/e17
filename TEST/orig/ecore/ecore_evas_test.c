#include "ecore_evas_test.h"
   
int
main(int argc, const char **argv)
{
   if (app_start(argc, argv) < 1) return -1;
   
   bg_start();
   
   calibrate_start();

   ecore_main_loop_begin();
   
   app_finish();
   
   return 0;
}
