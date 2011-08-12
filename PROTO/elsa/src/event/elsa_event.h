#ifndef ELSA_EVENT_
#define ELSA_EVENT_

typedef enum Elsa_Event_Type_
{
   ELSA_EVENT_UNKNOWN,
   ELSA_EVENT_AUTH,
   ELSA_EVENT_STATUS,
   ELSA_EVENT_XSESSIONS,
   ELSA_EVENT_USERS,
   ELSA_EVENT_ACTIONS,
   ELSA_EVENT_ACTION
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

typedef struct Elsa_Action_Event_
{
   int action;
} Elsa_Action_Event;

typedef struct Elsa_User_
{
   const char *login;
   const char *image;
   const char *lsess;
} Elsa_User;


typedef struct Elsa_Users_Event_
{
   Eina_List *users;
} Elsa_Users_Event;

typedef struct Elsa_Action_
{
   int id;
   const char *label;
} Elsa_Action;

typedef struct Elsa_Actions_Event_
{
   Eina_List *actions;
} Elsa_Actions_Event;

typedef struct Elsa_Event_
{
   Elsa_Event_Type type;
   union
     {
        Elsa_Xsessions_Event xsessions;
        Elsa_Auth_Event auth;
        Elsa_Status_Event status;
        Elsa_Users_Event users;
        Elsa_Actions_Event actions;
        Elsa_Action_Event action;
     } event;
} Elsa_Event;

void *elsa_event_encode(Elsa_Event *ev, int *size);
Elsa_Event *elsa_event_decode(void *data, int size);
#endif /* ELSA_EVENT_ */
