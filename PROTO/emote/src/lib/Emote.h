#ifndef EMOTE_H
# define EMOTE_H

# ifdef EMAPI
#  undef EMAPI
# endif

# ifdef WIN32
#  ifdef BUILDING_DLL
#   define EMAPI __declspec(dllexport)
#  else
#   define EMAPI __declspec(dllimport)
#  endif
# else
#  ifdef __GNUC__
#   if __GNUC__ >= 4
/* BROKEN in gcc 4 on amd64 */
#    if 0
#     pragma GCC visibility push(hidden)
#    endif
#    define EMAPI __attribute__ ((visibility("default")))
#   else
#    define EMAPI
#   endif
#  else
#   define EMAPI
#  endif
# endif

# include <Eina.h>

typedef struct _Emote_Protocol_Api Emote_Protocol_Api;
typedef struct _Emote_Protocol Emote_Protocol;

typedef int (*emote_protocol_init_t)(Emote_Protocol *p);
typedef int (*emote_protocol_shutdown_t)(void);

typedef enum _Emote_Event_Type Emote_Event_Type;

typedef void (*Emote_Object_Cleanup_Func) (void *obj);
typedef struct _Emote_Object Emote_Object;

typedef struct _Emote_Event Emote_Event;
typedef struct _Emote_Event_Server Emote_Event_Server;
typedef struct _Emote_Event_Server_Connect Emote_Event_Server_Connect;
typedef struct _Emote_Event_Server_Message Emote_Event_Server_Message;
typedef struct _Emote_Event_Chat Emote_Event_Chat;
typedef struct _Emote_Event_Chat_Message Emote_Event_Chat_Message;

enum _Emote_Event_Type
{
   EMOTE_EVENT_SERVER_CONNECT=0,
   EMOTE_EVENT_SERVER_DISCONNECT=1,
   EMOTE_EVENT_SERVER_CONNECTED=2,
   EMOTE_EVENT_SERVER_DISCONNECTED=3,
   EMOTE_EVENT_SERVER_MESSAGE_SEND=4,
   EMOTE_EVENT_SERVER_MESSAGE_RECEIVED=5,
   EMOTE_EVENT_CHAT_JOIN=6,
   EMOTE_EVENT_CHAT_JOINED=7,
   EMOTE_EVENT_CHAT_PART=8,
   EMOTE_EVENT_CHAT_PARTED=9,
   EMOTE_EVENT_CHAT_MESSAGE_SEND=10,
   EMOTE_EVENT_CHAT_MESSAGE_RECEIVED=11,
   EMOTE_EVENT_SERVER_NICK_CHANGED=12,
   EMOTE_EVENT_CHAT_TOPIC=13,
   EMOTE_EVENT_CHAT_USERS=14,
   EMOTE_EVENT_COUNT
};

struct _Emote_Object
{
   int magic, type, references;
   Emote_Object_Cleanup_Func del_func, cleanup_func;
   void (*free_att_func) (void *obj);
   void (*del_att_func) (void *obj);
   void *data;
   Eina_Bool deleted : 1;
};

struct _Emote_Protocol_Api
{
   int version;
   const char *name, *label;
};

struct _Emote_Protocol
{
   Emote_Object em_object_inherit;

   Emote_Protocol_Api *api;
   void *handle;

   struct
     {
        emote_protocol_init_t init; // required
        emote_protocol_shutdown_t shutdown; // required
     } funcs;
};

#define EMOTE_EVENT_T(x) ((Emote_Event*)x)
#define EMOTE_EVENT_SERVER_T(x) ((Emote_Event_Server*)x)
#define EMOTE_EVENT_SERVER_CONNECT_T(x) ((Emote_Event_Server_Connect*)x)
#define EMOTE_EVENT_SERVER_MESSAGE_T(x) ((Emote_Event_Server_Message*)x)
#define EMOTE_EVENT_CHAT_T(x) ((Emote_Event_Chat*)x)
#define EMOTE_EVENT_CHAT_MESSAGE_T(x) ((Emote_Event_Chat_Message*)x)

struct _Emote_Event
{
   Emote_Object Emote_Object_inherit;

   Emote_Protocol *protocol;
   Emote_Event_Type type;
};

struct _Emote_Event_Server
{
   Emote_Event e;
   const char *server;
};

struct _Emote_Event_Server_Connect
{
   Emote_Event_Server s;
   int port;
   const char *nick;
   const char *username;
   const char *password;
};

struct _Emote_Event_Server_Message
{
   Emote_Event_Server s;
   const char *user;
   const char *message;
};

struct _Emote_Event_Chat
{
   Emote_Event_Server s;
   const char *channel;
   const char *user;
};

struct _Emote_Event_Chat_Message
{
   Emote_Event_Chat c;
   const char *message;
};

// Main
EMAPI Eina_Bool emote_init(void);
EMAPI Eina_Bool emote_shutdown(void);

// Protocol
EMAPI Emote_Protocol *emote_protocol_load(const char *name);
EMAPI void            emote_protocol_unload(Emote_Protocol *p);
EMAPI Eina_List      *emote_protocol_list(void);

// Events
EMAPI Emote_Event    *emote_event_new(Emote_Protocol *p, int type, ...);
EMAPI void            emote_event_send(Emote_Event *e);
EMAPI void            emote_event_handler_add(int type, Eina_Bool (*cb)(void*,int,void*), void *data);

#endif
