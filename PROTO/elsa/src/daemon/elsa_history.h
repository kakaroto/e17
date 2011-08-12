#ifndef ELSA_HISTORY_
#define ELSA_HISTORY_

void elsa_history_init();
void elsa_history_shutdown();
void elsa_history_push(const char *login, const char *session);
Eina_List *elsa_history_get();

typedef struct _Elsa_Login Elsa_Login;
typedef struct _Elsa_History Elsa_History;

struct _Elsa_Login
{
   const char *login;
   const char *session;
};

struct _Elsa_History
{
   Eina_List *history;
};


#endif /* ELSA_HISTORY_ */
