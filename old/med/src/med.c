
#include "evaswin.h"
#include "extras.h"

void app_exit(void);
void med_set_parent( Window win );

int
main ()
{
  atexit( app_exit );

  evaswin_new(450,450,"med - E17 Menu Editor");

  med_setup_entries( evaswin_get_e() );

  /* display menu */
  pop_menu( 10, 10 );

  /* event handler - never returns (uses exit()) */
  ecore_event_loop();

  return 0;
}


void
app_exit(void)
{
  evaswin_free();
}


/*eof*/
