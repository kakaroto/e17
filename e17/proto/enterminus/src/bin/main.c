#include "term.h"

int main(int argc, char **argv) {

   Ecore_Evas  *ee;     /* ecore_evas */
   Term        *term;   /* terminal   */
   
   //term_window_init(ee, evas);
   ee = ecore_evas_software_x11_new(0, 0, 0, 0, 640, 480);
   ecore_evas_show(ee);
   
   term = term_new(ee);
   
   term_term_bg_set(term, DATADIR"white.png");

   execute_command(term, argc, argv);

   term->cmd_fd.ecore =  ecore_main_fd_handler_add(term->cmd_fd.sys,
						   ECORE_FD_READ,
						   term_tcanvas_data, term,
						   NULL, NULL);
   ecore_main_loop_begin();
   ecore_evas_shutdown();
   ecore_shutdown();

   return 0;
}
