#include <Eina.h>
#include <Eet.h>
#include "elsa_event.h"

#define ELSA_EVENT_AUTH_NAME "ElsaEventAuth"
#define ELSA_EVENT_XSESSIONS_NAME "ElsaEventSession"
#define ELSA_EVENT_STATUS_NAME "ElsaEventStatus"
#define ELSA_EVENT_USERS_NAME "ElsaEventUsers"
#define ELSA_EVENT_ACTIONS_NAME "ElsaEventActions"
#define ELSA_EVENT_ACTION_NAME "ElsaEventAction"

static Eina_Bool _elsa_event_type_set(const char *type, void *data, Eina_Bool unknow);
static const char *_elsa_event_type_get(const void *data, Eina_Bool *unknow);

static Eet_Data_Descriptor *_elsa_event_auth_dd();
static Eet_Data_Descriptor *_elsa_event_status_dd();
static Eet_Data_Descriptor *_elsa_event_xsessions_dd();

static Eina_Bool
_elsa_event_type_set(const char *type, void *data, Eina_Bool unknow)
{
   Elsa_Event_Type *ev = data;
   if (unknow)
     return EINA_FALSE;
   if (!strcmp(type, ELSA_EVENT_AUTH_NAME))
     *ev = ELSA_EVENT_AUTH;
   else if (!strcmp(type, ELSA_EVENT_STATUS_NAME))
     *ev = ELSA_EVENT_STATUS;
   else if (!strcmp(type, ELSA_EVENT_XSESSIONS_NAME))
     *ev = ELSA_EVENT_XSESSIONS;
   else if (!strcmp(type, ELSA_EVENT_USERS_NAME))
     *ev = ELSA_EVENT_USERS;
   else if (!strcmp(type, ELSA_EVENT_ACTIONS_NAME))
     *ev = ELSA_EVENT_ACTIONS;
   else if (!strcmp(type, ELSA_EVENT_ACTION_NAME))
     *ev = ELSA_EVENT_ACTION;
   else
     {
        printf("error on type set\n");
        *ev = ELSA_EVENT_UNKNOWN;
        return EINA_FALSE;
     }
   return EINA_TRUE;
}

static const char *
_elsa_event_type_get(const void *data, Eina_Bool *unknow)
{
   const Elsa_Event_Type *ev = data;
   if (*ev == ELSA_EVENT_AUTH)
     return ELSA_EVENT_AUTH_NAME;
   else if (*ev == ELSA_EVENT_STATUS)
     return ELSA_EVENT_STATUS_NAME;
   else if (*ev == ELSA_EVENT_XSESSIONS)
     return ELSA_EVENT_XSESSIONS_NAME;
   else if (*ev == ELSA_EVENT_USERS)
     return ELSA_EVENT_USERS_NAME;
   else if (*ev == ELSA_EVENT_ACTIONS)
     return ELSA_EVENT_ACTIONS_NAME;
   else if (*ev == ELSA_EVENT_ACTION)
     return ELSA_EVENT_ACTION_NAME;
   if (*unknow)
     {
        printf("error on type get\n");
        *unknow = EINA_TRUE;
     }
   return NULL;
}

static Eet_Data_Descriptor *
_elsa_event_xsessions_dd()
{
   Eet_Data_Descriptor_Class eddc, eddcl;
   Eet_Data_Descriptor *edd, *eddl;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Elsa_Xsession);
   edd = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Xsession, "name",
                                 name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Xsession, "icon",
                                 icon, EET_T_STRING);

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddcl, Elsa_Xsessions_Event);
   eddl = eet_data_descriptor_stream_new(&eddcl);
   EET_DATA_DESCRIPTOR_ADD_LIST(eddl, Elsa_Xsessions_Event, "xsessions", xsessions, edd);
   return eddl;
}

static Eet_Data_Descriptor *
_elsa_event_auth_dd()
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Elsa_Auth_Event);
   edd = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Auth_Event, "login",
                                 login, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Auth_Event, "password",
                                 password, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Auth_Event, "session",
                                 session, EET_T_STRING);
   return edd;

}

static Eet_Data_Descriptor *
_elsa_event_status_dd()
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Elsa_Status_Event);
   edd = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Status_Event, "login",
                                 login, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Status_Event, "granted",
                                 granted, EET_T_UCHAR);

   return edd;

}

static Eet_Data_Descriptor *
_elsa_event_users_dd()
{
   Eet_Data_Descriptor *edd, *eddl;
   Eet_Data_Descriptor_Class eddc, eddcl;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Elsa_User);
   edd = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_User, "login",
                                 login, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_User, "image",
                                 image, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_User, "lsess",
                                 lsess, EET_T_STRING);
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddcl, Elsa_Users_Event);
   eddl = eet_data_descriptor_stream_new(&eddcl);
   EET_DATA_DESCRIPTOR_ADD_LIST(eddl, Elsa_Users_Event, "users", users, edd);
   return eddl;
}

static Eet_Data_Descriptor *
_elsa_event_actions_dd()
{
   Eet_Data_Descriptor *edd, *eddl;
   Eet_Data_Descriptor_Class eddc, eddcl;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Elsa_Action);
   edd = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Action, "label",
                                 label, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Action, "id",
                                 id, EET_T_INT);
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddcl, Elsa_Actions_Event);
   eddl = eet_data_descriptor_stream_new(&eddcl);
   EET_DATA_DESCRIPTOR_ADD_LIST(eddl, Elsa_Actions_Event, "actions", actions, edd);
   return eddl;
}

static Eet_Data_Descriptor *
_elsa_event_action_dd()
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Elsa_Status_Event);
   edd = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Action_Event, "action",
                                 action, EET_T_INT);
   return edd;
}

static Eet_Data_Descriptor *
_elsa_event_new()
{
   Eet_Data_Descriptor_Class eddc;
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor *unified;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Elsa_Event);
   edd = eet_data_descriptor_stream_new(&eddc);

   eddc.version = EET_DATA_DESCRIPTOR_CLASS_VERSION;
   eddc.func.type_get = _elsa_event_type_get;
   eddc.func.type_set = _elsa_event_type_set;
   unified = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(unified, ELSA_EVENT_XSESSIONS_NAME, _elsa_event_xsessions_dd());
   EET_DATA_DESCRIPTOR_ADD_MAPPING(unified, ELSA_EVENT_AUTH_NAME, _elsa_event_auth_dd());
   EET_DATA_DESCRIPTOR_ADD_MAPPING(unified, ELSA_EVENT_STATUS_NAME, _elsa_event_status_dd());
   EET_DATA_DESCRIPTOR_ADD_MAPPING(unified, ELSA_EVENT_USERS_NAME, _elsa_event_users_dd());
   EET_DATA_DESCRIPTOR_ADD_MAPPING(unified, ELSA_EVENT_ACTIONS_NAME, _elsa_event_actions_dd());
   EET_DATA_DESCRIPTOR_ADD_MAPPING(unified, ELSA_EVENT_ACTION_NAME, _elsa_event_action_dd());

   EET_DATA_DESCRIPTOR_ADD_UNION(edd, Elsa_Event, "event", event, type, unified);
   return edd;
}

void *
elsa_event_encode(Elsa_Event *eev, int *size)
{
   Eet_Data_Descriptor *edd;

   edd = _elsa_event_new();

   return eet_data_descriptor_encode(edd, eev, size);
}

Elsa_Event *
elsa_event_decode(void *data, int size)
{
   Eet_Data_Descriptor *edd;

   edd = _elsa_event_new();

   return eet_data_descriptor_decode(edd, data, size);
}

