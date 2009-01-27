#include "common.h"

EAPI int
elm_main(int argc, char **argv)
{
   int i;
   
   for (i = 1; i < argc; i++)
     {
	// parse args
     }
   
#ifdef HAVE_EFSO
   efso_init();
   efso_gsm_sms_init();
#endif   
   data_init();
   create_main_win();
   elm_run();
   data_shutdown();
   elm_shutdown();
#ifdef HAVE_EFSO
   efso_gsm_sms_shutdown();
   efso_shutdown();
#endif
   return 0;
}
ELM_MAIN()
