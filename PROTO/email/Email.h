#ifndef EMAIL_H
#define EMAIL_H

#include <Eina.h>

typedef struct Email Email;
typedef void (*Email_Stat_Cb)(Email *, unsigned int, size_t);

extern int EMAIL_EVENT_CONNECTED;

int email_init(void);
Email *email_new(const char *username, const char *password, void *data);
void email_cert_add(Email *e, const char *file);
Eina_Bool email_connect_pop3(Email *e, Eina_Bool secure, const char *addr);
Eina_Bool email_stat(Email *e, Email_Stat_Cb cb);

#endif
