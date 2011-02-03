#ifndef ELSA_EVENT_
#define ELSA_EVENT_

typedef enum Elsa_Event_Type_
{
   ELSA_EVENT_UNKNOWN,
   ELSA_EVENT_AUTH,
   ELSA_EVENT_STATUS,
   ELSA_EVENT_XSESSIONS
} Elsa_Event_Type;

typedef struct Elsa_Xsession_
{
    const char *name;
    const char *command;
    const char *icon;
} Elsa_Xsession;

typedef struct Elsa_Xsessions_Event_
{
   Eina_List *xsessions;
} Elsa_Xsessions_Event;

typedef struct Elsa_Auth_Event
{
   const char *login;
   const char *password;
   const char *session;
} Elsa_Auth_Event;

typedef struct Elsa_Status_Event_
{
   Eina_Bool granted;
   const char *login;
} Elsa_Status_Event;

typedef struct Elsa_Event_
{
   Elsa_Event_Type type;
   union
     {
        Elsa_Xsessions_Event xsessions;
        Elsa_Auth_Event auth;
        Elsa_Status_Event status;
     } event;
} Elsa_Event;

void *elsa_event_encode(Elsa_Event *ev, int *size);
Elsa_Event *elsa_event_decode(void *data, int size);
#endif /* ELSA_EVENT_ */
