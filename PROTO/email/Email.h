#ifndef EMAIL_H
#define EMAIL_H

#include <Eina.h>
#include <Ecore.h>

typedef struct Email Email;
typedef void (*Email_Stat_Cb)(Email *, unsigned int, size_t);
typedef void (*Email_List_Cb)(Email *, Eina_List */* Email_List_Item */);
typedef void (*Email_Retr_Cb)(Email *, Eina_Binbuf *);

typedef struct
{
   unsigned int id;
   size_t size;
} Email_List_Item;

extern int EMAIL_EVENT_CONNECTED;

int email_init(void);
Email *email_new(const char *username, const char *password, void *data);
void email_free(Email *e);
void email_data_set(Email *e, void *data);
void *email_data_get(Email *e);
void email_cert_add(Email *e, const char *file);
const Eina_List *email_queue_get(Email *e);
Eina_Bool email_op_cancel(Email *e, unsigned int op_number);
Eina_Bool email_connect_pop3(Email *e, Eina_Bool secure, const char *addr);
Eina_Bool email_quit(Email *e, Ecore_Cb cb);
Eina_Bool email_stat(Email *e, Email_Stat_Cb cb);
Eina_Bool email_list(Email *e, Email_List_Cb cb);
Eina_Bool email_rset(Email *e, Ecore_Cb cb);
Eina_Bool email_delete(Email *e, unsigned int id, Ecore_Cb cb);
Eina_Bool email_retrieve(Email *e, unsigned int id, Email_Retr_Cb cb);

#endif
