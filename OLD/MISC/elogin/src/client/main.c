#include "elogin.h"
#include "events.h"

void
setup(void)
{
   E_Login_Session e;

   e = e_login_session_new();
   e_login_session_init(e);

   setup_events(e);
}

int
main(int argc, char *argv[])
{
   if (!ecore_display_init(argv[1]))
   {
      if (getenv("DISPLAY"))
      {
         printf("Cannot initialize default display:\n");
         printf("DISPLAY=%s\n", getenv("DISPLAY"));
      }
      else
      {
         printf("No DISPLAY variable set!\n");
      }
      printf("Exit.\n");
      exit(-1);
   }

   /* setup handlers for system signals */
   ecore_event_signal_init();
   /* setup the event filter */
   ecore_event_filter_init();
   /* setup the X event internals */
   ecore_event_x_init();

   setup();

   ecore_event_loop();
   return (0);
}
