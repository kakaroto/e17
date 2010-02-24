#include "main.h"

#include <Evas_Engine_FB.h>

Eina_Bool
engine_fb_args(const char *engine, int width, int height)
{
   Evas_Engine_Info_FB *einfo;
   int i;

   evas_output_method_set(evas, evas_render_method_lookup("fb"));
   einfo = (Evas_Engine_Info_FB *)evas_engine_info_get(evas);
   if (!einfo)
     {
	printf("Evas does not support the FB Engine\n");
	return EINA_FALSE;
     }

   einfo->info.virtual_terminal = 0;
   einfo->info.device_number = 0;
   einfo->info.device_number = 0;
   einfo->info.refresh = 0;
   einfo->info.rotation = 0;
   if (!evas_engine_info_set(evas, (Evas_Engine_Info *) einfo))
     {
	printf("Evas can not setup the informations of the FB Engine\n");
	return EINA_FALSE;
     }

   return EINA_TRUE;
}

void
engine_fb_loop(void)
{
   return;
}

void
engine_fb_shutdown(void)
{
   return;
}
