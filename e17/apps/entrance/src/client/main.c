#include "entrance.h"
#include "events.h"

void
setup(void)
{
   Entrance_Session e;

   e = entrance_session_new();
   entrance_session_init(e);

   setup_events(e);
}

int
main(int argc, char *argv[])
{
   char *disp;
   
   openlog("entrance", LOG_NOWAIT, LOG_DAEMON);

   if (!ecore_display_init(argv[1]))
   {
      disp = getenv("DISPLAY");
	  
      if (disp)
         syslog(LOG_CRIT, "Cannot initialize default display \"%s\". Exiting.", disp);
      else
         syslog(LOG_CRIT, "No DISPLAY variable set! Exiting.");
      
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
   closelog();

   return (0);
}
