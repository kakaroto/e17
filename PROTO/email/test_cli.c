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
mail_retr(Email *e, Eina_Binbuf *buf)
{
   printf("Received message (%zu bytes): \n%*s\n",
     eina_binbuf_length_get(buf), (int)eina_binbuf_length_get(buf),
     (char*)eina_binbuf_string_get(buf));
   email_quit(e, (Ecore_Cb)mail_quit);
}

static void
mail_list(Email *e, Eina_List *list)
{
   Email_List_Item *it;
   Eina_List *l;

   EINA_LIST_FOREACH(list, l, it)
     {
        printf("#%u, %zu octets\n", it->id, it->size);
        email_retrieve(e, it->id, mail_retr);
     }
}

static void
mail_stat(Email *e, unsigned int num __UNUSED__, size_t size __UNUSED__)
{
   email_list(e, mail_list);
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
