#ifndef ELSA_CONNECT_
#define ELSA_CONNECT_
void elsa_connect_init();
void elsa_connect_auth_send(const char *login, const char *password, const char *session);
void elsa_connect_action_send(int id);
void elsa_connect_shutdown();
#endif /* ELSA_CONNECT_ */
