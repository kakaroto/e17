#include "Empower.h"

int 
main(int argc, char** argv)
{
   --argc; ++argv;
	
   em = calloc(1, sizeof(Empower));
	
   if(argc) /* commands */
     {
	snprintf(em->buf, 1024, "sudo -S %s ", *argv);
	
	--argc; ++argv;
	while(argc)
	  {
	     strncat(em->buf, " ", 1024);
	     strncat(em->buf, *argv, 1024);
	     --argc; ++argv;
	  }
     }
   else
     {
	printf("Usage: ./empower <program name>\n");
	return 1;
     }
     
   display_window();
   
   etk_main();
   etk_main_quit();
   etk_shutdown();
      
   return 0;
}
