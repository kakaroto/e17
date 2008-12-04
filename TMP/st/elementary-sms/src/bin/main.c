#include "common.h"

int
main(int argc, char **argv)
{
   int i;
   
   elm_init(argc, argv);
   
   for (i = 1; i < argc; i++)
     {
	// parse args
     }
   
   data_init();
   
   create_main_win();
   
   elm_run();
   
   data_shutdown();
   elm_shutdown();
   
   return 0; 
}
