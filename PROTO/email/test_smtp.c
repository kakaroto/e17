#include "Email.h"

#ifndef __UNUSED__
# define __UNUSED__ __attribute__((unused))
#endif

char *getpass_x(const char *prompt);

static Eina_Bool
con(void *d __UNUSED__, int type __UNUSED__, Email *e)
{
   printf("Connected!\n");
   return ECORE_CALLBACK_RENEW;
}

int
main(int argc, char *argv[])
{
   Email *e;
   char *pass;

   if (argc < 4)
     {
        fprintf(stderr, "Usage: %s username server send_domain\n", argv[0]);
        exit(1);
     }

   email_init();

   pass = getpass_x("Password: ");
   e = email_new(argv[1], pass, NULL);
   ecore_event_handler_add(EMAIL_EVENT_CONNECTED, (Ecore_Event_Handler_Cb)con, NULL);
   email_connect_smtp(e, EINA_FALSE, argv[2], argv[3]);
   ecore_main_loop_begin();

   return 0;
}
