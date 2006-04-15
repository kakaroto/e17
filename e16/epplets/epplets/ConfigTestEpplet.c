#include "epplet.h"

static void         cb_close(void *data);

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
   char                s[1024];
   char               *result = NULL;
   int                 use_count = 1;
   char              **results = NULL;
   int                 i, num_results;

   Epplet_Init("E-Config", "0.1", "Enlightenment Config Test Epplet",
	       5, 2, argc, argv, 0);
   Epplet_load_config();
   result = Epplet_query_config_def("USE_COUNT", "0");
   use_count = atoi(result);
   use_count++;
   Esnprintf(s, sizeof(s), "%i", use_count);
   Epplet_modify_config("USE_COUNT", s);

   printf("Dump before modification:\n");
   results = Epplet_query_multi_config("PARAMS", &num_results);
   for (i = 0; i < num_results; i++)
     {
	printf("%s\n", results[i]);
     }
   if (results)
     {
	free(results);
	results = NULL;
     }

   Epplet_modify_multi_config("PARAMS", argv, argc);

   printf("Dump after modification:\n");
   results = Epplet_query_multi_config("PARAMS", &num_results);
   for (i = 0; i < num_results; i++)
     {
	printf("%s\n", results[i]);
     }
   if (results)
      free(results);

   Esnprintf(s, sizeof(s), "Instance %i", Epplet_get_instance());
   Epplet_gadget_show(Epplet_create_label(10, 2, s, 0));

   Esnprintf(s, sizeof(s), "Used %i times.", use_count);
   Epplet_gadget_show(Epplet_create_label(10, 14, s, 0));

   Epplet_gadget_show(Epplet_create_button(NULL, NULL,
					   65, 2, 0, 0, "CLOSE", 0, NULL,
					   cb_close, NULL));
   Epplet_show();
   Epplet_Loop();
   return 0;
}
