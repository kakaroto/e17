#include "enscribe.h"

/* callback for quit call */
void ens_cb_menu_quit(void *data, Evas_Object * o, const char *emission,
		      const char *source) {
   ecore_main_loop_quit();
}
