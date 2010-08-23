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

#include <Eina.h>

extern int EMOTE_EVENT_MSG_RECEIVED;
extern int EMOTE_EVENT_MSG_SEND;

typedef struct _Emote_Protocol_Api Emote_Protocol_Api;
typedef struct _Emote_Protocol Emote_Protocol;
typedef struct _Emote_Event_Data Emote_Event_Data;

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

struct _Emote_Event_Data
{
   Emote_Protocol *protocol;
   void *data;
};

EMAPI int emote_init(void);
EMAPI int emote_shutdown(void);

EMAPI void emote_event_send(int type, Emote_Protocol *p, void *data);

EMAPI Emote_Protocol *emote_protocol_load(const char *name);
EMAPI void emote_protocol_unload(Emote_Protocol *p);
EMAPI Eina_List *emote_protocol_list(void);

#endif
