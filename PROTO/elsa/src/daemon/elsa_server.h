#ifndef ELSA_CONNECT_
#define ELSA_CONNECT_

#define ELSA_EVENT_SESSIONS 1
#define ELSA_EVENT_GRANTED 2

typedef struct Elsa_Xsession_
{
    const char *name;
    const char *command;
    const char *icon;
} Elsa_Xsession;

typedef struct Elsa_Xsession_Send_
{
   Eina_List *xsessions;
} Elsa_Xsession_Send;

typedef struct Elsa_Server_Event_
{
   int type;
   /*
   union
     {
        Elsa_Xsession_Send *xsessions;
        Elsa_Auth *auth;
        Elsa_Status *status;
     }
     */
} Elsa_Server_Event;


void elsa_server_init();
void elsa_server_shutdown();
#endif /* ELSA_CONNECT_ */
