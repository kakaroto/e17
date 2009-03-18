#include "conf_options.h"
#include "main.h"

void 
config_option_fullscreen(void *data)
{
   Ecore_Evas *ee = (Ecore_Evas *)data;
   int *i = malloc(sizeof(int));

   if (ecore_evas_fullscreen_get(ee))
   {
      i[0] = 0;
      ecore_evas_fullscreen_set(ee, 0);
      ecore_evas_cursor_set(ee, NULL, 0, 0, 0);
      eet_write(eet_config, "/config/fullscreen", i, sizeof(int), 0);
   }
   else 
   {
      i[0] = 1;
      ecore_evas_fullscreen_set(ee, 1);
      ecore_evas_cursor_set(ee, "", 999, 0, 0);
      eet_write(eet_config, "/config/fullscreen", i, sizeof(int), 0);
   }
}

void 
config_option_themes(void *data)
{
}

void
config_option_modes_switch(void* data)
{
   int mode = (int)data;
   eet_write(eet_config, "/config/mode", &mode, sizeof(int), 0);
   main_reset();
}

void 
config_option_modes(void *data)
{
   menu_push("menu", "Modes", NULL, NULL);
   if (ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_SOFTWARE_X11))
      menu_item_add("icon/x11", "Software",
	    	    NULL, NULL, config_option_modes_switch,
		    (void*)0, NULL, NULL, NULL);
   if (ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_OPENGL_X11))
      menu_item_add("icon/mesa", "Mesa OpenGL",
	    	    NULL, NULL, config_option_modes_switch,
		    (void*)1, NULL, NULL, NULL);
   if (ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_SOFTWARE_FB))
      menu_item_add("icon/fb", "Software Framebuffer",
	    	    NULL, NULL, config_option_modes_switch,
		    (void*)2, NULL, NULL, NULL);
   if (ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_XRENDER_X11))
      menu_item_add("icon/xr", "XRender Extension",
	    	    NULL, NULL, config_option_modes_switch,
		    (void*)3, NULL, NULL, NULL);
   if (ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_DIRECTFB))
      menu_item_add("icon/dfb", "DirectFB",
	    	    NULL, NULL, config_option_modes_switch,
		    (void*)4, NULL, NULL, NULL);
   if (ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_SOFTWARE_SDL))
      menu_item_add("icon/sdl", "SDL Engine",
	    	    NULL, NULL, config_option_modes_switch,
		    (void*)5, NULL, NULL, NULL);
   /* WINDOWS ENGINES - maybe later
   if (ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_SOFTWARE_DDRAW)) 
      menu_item_add("icon/x11", "Software",
	    	    NULL, NULL, config_option_modes_switch,
		    6, NULL, NULL, NULL);
   if (ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_DIRECT3D))
      menu_item_add("icon/x11", "Direct3D",
	    	    NULL, NULL, config_option_modes_switch,
		    7, NULL, NULL, NULL);
   if (ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_SOFTWARE_16_WINCE))
      menu_item_add("icon/x11", "Software",
	    	    NULL, NULL, config_option_modes_switch,
		    7, NULL, NULL, NULL);
   */

   menu_go();
   menu_item_select("Software");
}

void 
config_option_volumes(void *data)
{
}

