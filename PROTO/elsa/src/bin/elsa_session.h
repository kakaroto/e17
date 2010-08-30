#ifndef ELSA_SESSION_H_
#define ELSA_SESSION_H_

void elsa_session_init(const char *file);
void elsa_session_shutdown();
Eina_Bool elsa_session_login(void *data);
void elsa_session_run(struct passwd *pwd, char *cmd);
void elsa_session_pid_set(pid_t pid);
pid_t elsa_session_pid_get();

#endif /* ELSA_SESSION_H_ */
