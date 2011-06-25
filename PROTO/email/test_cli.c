#include "Email.h"

#ifndef __UNUSED__
# define __UNUSED__ __attribute__((unused))
#endif

char *getpass_x(const char *prompt);

static void
mail_quit(Email *e __UNUSED__)
{
   ecore_main_loop_quit();
}

static void
mail_stat(Email *e, unsigned int num __UNUSED__, size_t size __UNUSED__)
{
   email_quit(e, (Ecore_Cb)mail_quit);
}

static Eina_Bool
con(void *d __UNUSED__, int type __UNUSED__, Email *e)
{
   email_stat(e, mail_stat);
   return ECORE_CALLBACK_RENEW;
}

int
main(int argc, char *argv[])
{
   Email *e;
   char *pass;

   if (argc < 3)
     {
        fprintf(stderr, "Usage: %s username server\n", argv[0]);
        exit(1);
     }

   email_init();

   pass = getpass_x("Password: ");
   e = email_new(argv[1], pass, NULL);
   ecore_event_handler_add(EMAIL_EVENT_CONNECTED, (Ecore_Event_Handler_Cb)con, NULL);
   email_connect_pop3(e, EINA_TRUE, argv[2]);
   ecore_main_loop_begin();

   return 0;
}
