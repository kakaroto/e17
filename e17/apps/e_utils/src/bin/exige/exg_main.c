#include "Exige.h"
#include "exg_gui.h"
#include "exg_callback.h"
#include "exg_conf.h"

#include <stdlib.h>
#include <stdio.h>


int main(int argc, const char **argv)
{
    Exige *exg =calloc(1,sizeof(Exige));
    
    if (!ecore_init()) {
	fprintf(stderr, "Exige: can't initialize Ecore.\n");
	return 1;
    }

    if(!exg) {
	fprintf(stderr, "Exige: can't create exige struct\n");
	return 1;
    }
  
    if (!ecore_evas_init()) {
	fprintf(stderr, "Exige: can't initialize Ecore_Evas.\n");
	return 1;
    }

    eet_init();
    edje_init();

    exg_eapps_init();    
    exg_gui_init(exg);
    
    ecore_main_loop_begin();
    eet_shutdown();
    edje_shutdown();
    ecore_evas_shutdown();
    ecore_config_save();
    ecore_config_shutdown();
    ecore_shutdown();

    return 0;
}
