#include <Etk.h>

#include "Evolve.h"
#include "config.h"

char *_custom_data1 = "Evolve rocks!";
char *_custom_data2 = "Evolve owns!";

void _custom_signal_emission_cb(char *emission, void *data)
{
   printf("This is a custom emission (%s) with data '%s'!\n",
	  emission, (char*)data);
}

void _custom_signal_emission_cb2(char *emission, void *data)
{
   printf("This is another callback for custom emission (%s) with data '%s'!\n",
	  emission, (char*)data);	  
}

int _window_deleted_event_cb(Etk_Object *object)
{
   Etk_Widget *window;
   
   window = ETK_WIDGET(object);
   etk_main_quit();
   return 1;
}

void _hello_button_clicked_cb(Etk_Object *object, void *data)
{
   Etk_Widget *button;
   
   button = ETK_WIDGET(object);
   printf("Clicked button (data=%p)!\n", data);
}

int main(int argc, char **argv)
{
   Evolve *evolve;
   
   evolve_init();
   etk_init(argc, argv);
   
   evolve = evolve_eet_load(PACKAGE_DATA_DIR"/interfaces/etk_test/etk_test.eet");
   if (!evolve)
     {     
	fprintf(stderr, "Can't load eet file!\n");	
	exit(-1);
     }
   
   evolve_signal_connect(evolve, "custom_signal_emission", _custom_signal_emission_cb, _custom_data1);
   evolve_signal_connect(evolve, "custom_signal_emission", _custom_signal_emission_cb2, _custom_data2);
   
   evolve_render(evolve);
   
   etk_main();
   etk_shutdown();
   evolve_shutdown();
   
   return 0;
}
