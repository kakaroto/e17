#include "emote_private.h"
#include "Emote.h"

#include <libgen.h>

/* local function prototypes */
static void _emote_locate_paths(void);
static void _emote_event_free(void *data, void *event);

int EMOTE_EVENT_CHAT_SERVER_CONNECTED;
int EMOTE_EVENT_CHAT_SERVER_DISCONNECTED;
int EMOTE_EVENT_CHAT_SERVER_MESSAGE_SEND;
int EMOTE_EVENT_CHAT_SERVER_MESSAGE_RECEIVED;
int EMOTE_EVENT_CHAT_CHANNEL_ADD;
int EMOTE_EVENT_CHAT_CHANNEL_MESSAGE_SEND;
int EMOTE_EVENT_CHAT_CHANNEL_MESSAGE_RECEIVED;

/* public functions */
EMAPI int
emote_init(void)
{
   static int ref_count = 0;
   int ret;

   ret = 1;
   if (!ref_count)
     {
        EMOTE_EVENT_CHAT_SERVER_CONNECTED = ecore_event_type_new();
        EMOTE_EVENT_CHAT_SERVER_DISCONNECTED = ecore_event_type_new();
        EMOTE_EVENT_CHAT_SERVER_MESSAGE_SEND = ecore_event_type_new();
        EMOTE_EVENT_CHAT_SERVER_MESSAGE_RECEIVED = ecore_event_type_new();
        EMOTE_EVENT_CHAT_CHANNEL_ADD = ecore_event_type_new();
        EMOTE_EVENT_CHAT_CHANNEL_MESSAGE_RECEIVED = ecore_event_type_new();
        EMOTE_EVENT_CHAT_CHANNEL_MESSAGE_SEND = ecore_event_type_new();

        _emote_locate_paths();

        ret &= emote_protocol_init();

        ref_count++;
     }

   return ret;
}

EMAPI int
emote_shutdown(void)
{
   emote_protocol_shutdown();
   return 1;
}

EMAPI void
emote_event_send(int type, void *event_data)
{
   ecore_event_add(type, event_data, _emote_event_free, NULL);
}

/* local functions */
static void
_emote_event_free(void *data __UNUSED__, void *event)
{
   EMOTE_FREE(event);
}

static void
_emote_locate_paths(void)
{
#if HAVE_DLADDR
   Dl_info emote_dl;
#endif

   // Avoid filling paths multiple times
   if (emote_paths.libdir[0]) return;

   if (getenv("em_LIBDIR"))
     strncpy(emote_paths.libdir, getenv("em_LIBDIR"),
             sizeof(emote_paths.libdir));

   // Attempt to use dladdr
#if HAVE_DLADDR
   if ((!emote_paths.libdir[0]) && (dladdr((void*)emote_init, &emote_dl)))
     strncpy(emote_paths.libdir, dirname((char*)emote_dl.dli_fname),
             sizeof(emote_paths.libdir));
#endif

   // Fallbacks
#ifdef PACKAGE_LIB_DIR
   if (!emote_paths.libdir[0])
     strncpy(emote_paths.libdir, PACKAGE_LIB_DIR, sizeof(emote_paths.libdir));
#endif

   if (!emote_paths.libdir[0])
     printf("Cannot determine library path!");

   // Fill in protocoldir which is just based on libdir
   sprintf(emote_paths.protocoldir, "%s/emote/protocols", emote_paths.libdir);
}
