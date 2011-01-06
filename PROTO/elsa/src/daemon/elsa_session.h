#ifndef ELSA_SESSION_H_
#define ELSA_SESSION_H_
#include <pwd.h>

void elsa_session_init(const char *file);
void elsa_session_shutdown();
Eina_Bool elsa_session_authenticate(char *login, char *pwd);
Eina_Bool elsa_session_login(const char *command);
void elsa_session_pid_set(pid_t pid);
pid_t elsa_session_pid_get();
long  elsa_session_seed_get();
char *elsa_session_login_get();

#endif /* ELSA_SESSION_H_ */
