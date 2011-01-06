#ifndef ELSA_PAM_H_
#define ELSA_PAM_H_

#include <security/pam_appl.h>

typedef enum ELSA_PAM_ITEM_TYPE_ {
   ELSA_PAM_ITEM_SERVICE = PAM_SERVICE,
   ELSA_PAM_ITEM_USER = PAM_USER,
   ELSA_PAM_ITEM_TTY = PAM_TTY,
   ELSA_PAM_ITEM_RUSER = PAM_RUSER,
   ELSA_PAM_ITEM_RHOST = PAM_RHOST,
   ELSA_PAM_ITEM_CONV = PAM_CONV
} ELSA_PAM_ITEM_TYPE;


int elsa_pam_item_set(ELSA_PAM_ITEM_TYPE type, const void *value);
const void *elsa_pam_item_get(ELSA_PAM_ITEM_TYPE);
int elsa_pam_env_set(const char *env, const char *value);
char **elsa_pam_env_list_get();
int elsa_pam_init(const char *service, const char *display, const char *user);
void elsa_pam_shutdown();
int elsa_pam_open_session();
void elsa_pam_close_session();
int elsa_pam_authenticate();
int elsa_pam_auth_set(const char *login, const char *passwd);
int elsa_pam_end();

#endif /* ELSA_PAM_H_ */
