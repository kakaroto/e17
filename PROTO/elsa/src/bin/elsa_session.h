#ifdef ELSA_SESSION_H_
#define ELSA_SESSION_H_

void elsa_session_init(struct passwd *pwd);
void elsa_session_run(struct passwd *pwd);
void elsa_session_shutdown(struct passwd *pwd);
long elsa_session_seed_get();
void elsa_session_auth(const char *file);
void elsa_session_pid_set(pid_t pid);
pid_t elsa_session_pid_get();

#endif
