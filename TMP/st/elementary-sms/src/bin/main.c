#include "common.h"

#ifndef ELM_LIB_QUICKLAUNCH
EAPI int
elm_main(int argc, char **argv)
{
   int i;
#if HAVE_EOFONO
   E_DBus_Connection *c;
#endif

   for (i = 1; i < argc; i++)
     {
	// parse args
     }

#ifdef HAVE_EFSO
   efso_init();
   efso_gsm_sms_init();
#elif HAVE_EOFONO
   e_dbus_init();
   c = e_dbus_bus_get(DBUS_BUS_SYSTEM);
   if (!c)
     {
	fprintf(stderr, "ERR: can't connect to system session\n");
	return -1;
     }
   e_ofono_system_init(c);
#endif   
   data_init();
   create_main_win();
   elm_run();
   data_shutdown();
#ifdef HAVE_EFSO
   efso_gsm_sms_shutdown();
   efso_shutdown();
#elif HAVE_EOFONO
   e_ofono_system_shutdown();
   e_dbus_connection_close(c);
   e_dbus_shutdown();
#endif
   elm_shutdown();
   return 0;
}
#endif
ELM_MAIN()
