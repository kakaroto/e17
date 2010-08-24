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

extern int EMOTE_EVENT_CHAT_SERVER_CONNECTED;
extern int EMOTE_EVENT_CHAT_SERVER_DISCONNECTED;
extern int EMOTE_EVENT_CHAT_SERVER_MESSAGE_SEND;
extern int EMOTE_EVENT_CHAT_SERVER_MESSAGE_RECEIVED;
extern int EMOTE_EVENT_CHAT_CHANNEL_ADD;
extern int EMOTE_EVENT_CHAT_CHANNEL_MESSAGE_SEND;
extern int EMOTE_EVENT_CHAT_CHANNEL_MESSAGE_RECEIVED;

typedef struct _Emote_Protocol_Api Emote_Protocol_Api;
typedef struct _Emote_Protocol Emote_Protocol;

typedef struct _Emote_Event Emote_Event;
typedef struct _Emote_Event_Chat_Server Emote_Event_Chat_Server;
typedef struct _Emote_Event_Chat_Server_Message Emote_Event_Chat_Server_Message;
typedef struct _Emote_Event_Chat_Channel_Add Emote_Event_Chat_Channel_Add;
typedef struct _Emote_Event_Chat_Channel_Message Emote_Event_Chat_Channel_Message;

typedef int (*emote_protocol_init_t)(Emote_Protocol *p);
typedef int (*emote_protocol_shutdown_t)(void);
typedef int (*emote_protocol_connect_t)(const char *, int, const char *, const char *);
typedef int (*emote_protocol_disconnect_t)(const char *);

struct _Emote_Protocol_Api
{
   int version;
   const char *name, *label;
};

struct _Emote_Protocol
{
   Emote_Protocol_Api *api;
   void *handle;

   struct
     {
        emote_protocol_init_t init; // required
        emote_protocol_shutdown_t shutdown; // required
        emote_protocol_connect_t connect;
        emote_protocol_disconnect_t disconnect;

        /* TODO: Implement generic functions */
     } funcs;
};

struct _Emote_Event
{
   Emote_Protocol *protocol;
   void *data;
};

struct _Emote_Event_Chat_Server
{
   Emote_Protocol *protocol;
   const char *server;
};

struct _Emote_Event_Chat_Server_Message
{
   Emote_Protocol *protocol;
   const char *server;
   const char *message;
};

struct _Emote_Event_Chat_Channel_Add
{
   Emote_Protocol *protocol;
   const char *server;
   const char *channel;
};

struct _Emote_Event_Chat_Channel_Message
{
   Emote_Protocol *protocol;
   const char *server;
   const char *channel;
   const char *user;
   const char *message;
};

EMAPI int emote_init(void);
EMAPI int emote_shutdown(void);

EMAPI void emote_event_send(int type, void *event_data);

EMAPI Emote_Protocol *emote_protocol_load(const char *name);
EMAPI void emote_protocol_unload(Emote_Protocol *p);
EMAPI Eina_List *emote_protocol_list(void);
EMAPI void emote_protocol_connect(const char *name, const char *server, int port, const char *username, const char *password);
EMAPI void emote_protocol_disconnect(const char *name, const char *server);

#endif
