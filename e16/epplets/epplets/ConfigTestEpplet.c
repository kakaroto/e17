#include "epplet.h"

static void cb_close(void *data);

static void
cb_close(void *data)
{
   Epplet_unremember();
   Esync();
   Epplet_cleanup();
   data = NULL;
   exit(0);
}


int
main(int argc, char **argv)
{
   char          s[1024];
   char         *result = NULL;
   int           use_count = 1; 

   Epplet_Init("E-Config", "0.1", "Enlightenment Config Test Epplet",
	       5, 2, argc, argv, 0);
   Epplet_load_config();
   result = Epplet_query_config_def("USE_COUNT", "0");
   use_count = atoi(result);
   use_count++;
   snprintf(s, 1024, "%i", use_count);
   Epplet_modify_config("USE_COUNT", s); 

   snprintf(s, 1024, "Instance %i", Epplet_get_instance()); 
   Epplet_gadget_show(Epplet_create_label(10, 2, s, 0));

   snprintf(s, 1024, "Used %i times.", use_count);
   Epplet_gadget_show(Epplet_create_label(10, 14, s, 0));

   Epplet_gadget_show(Epplet_create_button(NULL, NULL, 
					   65, 2, 0, 0, "CLOSE", 0, NULL, 
					   cb_close, NULL));
   Epplet_show();
   Epplet_Loop();
   return 0;
}
