#include "main.h"

#include <Evas_Engine_FB.h>

int
engine_fb_args(int argc, char **argv)
{
   Evas_Engine_Info_FB *einfo;
   int i;
   int ok = 0;

   for (i = 1; i < argc; i++)
     {
	if ((!strcmp(argv[i], "-e")) && (i < (argc - 1)))
	  {
	     i++;
	     if (!strcmp(argv[i], "fb")) ok = 1;
	  }
     }
   if (!ok) return 0;

   evas_output_method_set(evas, evas_render_method_lookup("fb"));
   einfo = (Evas_Engine_Info_FB *)evas_engine_info_get(evas);
   if (!einfo)
     {
	printf("Evas does not support the FB Engine\n");
	return 0;
     }

   einfo->info.virtual_terminal = 0;
   einfo->info.device_number = 0;
   einfo->info.device_number = 0;
   einfo->info.refresh = 0;
   einfo->info.rotation = 0;
   evas_engine_info_set(evas, (Evas_Engine_Info *) einfo);

   return 1;
}

void
engine_fb_loop(void)
{
   return;
}
