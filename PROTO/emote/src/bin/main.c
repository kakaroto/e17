#include "em_global.h"

#ifndef ELM_LIB_QUICKLAUNCH

# define EM_MAX_LEVEL 3

/* local function prototypes */
static void _em_main_shutdown_push(int (*func)(void));
static void _em_main_shutdown(int errcode);
static void _em_main_interrupt(int x __UNUSED__, siginfo_t *info __UNUSED__, void *data __UNUSED__);
static Eina_Bool _em_main_chat_events_handler(void *data, int type, void *event);

/* local variables */
static int (*_em_main_shutdown_func[EM_MAX_LEVEL])(void);
static int _em_main_level = 0;

Eina_Hash *em_protocols;

/* public functions */
EAPI int
elm_main(int argc __UNUSED__, char **argv __UNUSED__)
{
   Emote_Event *d;
   struct sigaction action;
   Emote_Protocol *p;

# ifdef ENABLE_NLS
   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
   textdomain(PACKAGE);
# endif

   /* trap keyboard interrupt from user (Ctrl + C) */
   action.sa_sigaction = _em_main_interrupt;
   action.sa_flags = SA_NODEFER | SA_RESETHAND | SA_SIGINFO;
   sigemptyset(&action.sa_mask);
   sigaction(SIGINT, &action, NULL);

   if (!em_object_init()) _em_main_shutdown(EXIT_FAILURE);
   _em_main_shutdown_push(em_object_shutdown);

   /* init our config subsystem */
   if (!em_config_init()) _em_main_shutdown(EXIT_FAILURE);
   _em_main_shutdown_push(em_config_shutdown);

   /* init protocol subsystem */
   if (!emote_init()) _em_main_shutdown(EXIT_FAILURE);
   _em_main_shutdown_push(emote_shutdown);

   /* init our gui subsystem */
   if (!em_gui_init()) _em_main_shutdown(EXIT_FAILURE);
   _em_main_shutdown_push(em_gui_shutdown);

   emote_event_handler_add(EMOTE_EVENT_SERVER_CONNECTED,
                           _em_main_chat_events_handler, NULL);
   emote_event_handler_add(EMOTE_EVENT_SERVER_DISCONNECTED,
                           _em_main_chat_events_handler, NULL);
   emote_event_handler_add(EMOTE_EVENT_CHAT_CHANNEL_JOINED,
                           _em_main_chat_events_handler, NULL);
   emote_event_handler_add(EMOTE_EVENT_SERVER_MESSAGE_RECEIVED,
                           _em_main_chat_events_handler, NULL);
   emote_event_handler_add(EMOTE_EVENT_CHAT_CHANNEL_MESSAGE_RECEIVED,
                           _em_main_chat_events_handler, NULL);

   em_protocols = eina_hash_string_superfast_new(NULL);

   p = emote_protocol_load("irc");
   eina_hash_add(em_protocols, "irc", p);

   d = emote_event_new
       (
          eina_hash_find(em_protocols, "irc"),
          EMOTE_EVENT_SERVER_CONNECT,
          "irc.freenode.net",
          6667,
          "emote",
          "emote"
       );
   emote_event_send(d);

   /* start main loop */
   elm_run();

   /* shutdown elm */
   elm_shutdown();

   /* shutdown */
   _em_main_shutdown(EXIT_SUCCESS);

   return EXIT_SUCCESS;
}

/* local functions */
static void
_em_main_shutdown_push(int (*func)(void))
{
   _em_main_level++;
   if (_em_main_level > EM_MAX_LEVEL)
     {
        _em_main_level--;
        return;
     }
   _em_main_shutdown_func[_em_main_level - 1] = func;
}

static void
_em_main_shutdown(int errcode)
{
   int i = 0;

   eina_hash_free(em_protocols);
   /* loop the shutdown functions and call each one on the stack */
   for (i = (_em_main_level - 1); i >= 0; i--)
     (*_em_main_shutdown_func[i])();

   /* exit if we err'd */
   if (errcode < 0) exit(errcode);
}

static void
_em_main_interrupt(int x __UNUSED__, siginfo_t *info __UNUSED__, void *data __UNUSED__)
{
   printf("\nEmote: Caught Interrupt Signal, Exiting\n");

   /* if we are finished with init, then we need to call elm_exit
    * as the app is in a 'running' state else we have not completed our init
    * function(s) so call our own shutdown */
   if (_em_main_level == EM_MAX_LEVEL)
     elm_exit();
   else
     {
        _em_main_shutdown(EXIT_SUCCESS);
        exit(EXIT_SUCCESS);
     }
}

static Eina_Bool
_em_main_chat_events_handler(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   switch(EMOTE_EVENT_T(event)->type)
   {
      case EMOTE_EVENT_SERVER_CONNECTED:
        {
           Emote_Event_Server *d;
           Emote_Event *c;

           d = event;
           printf("Server %s from protocol %s is now connected.\n",
                  d->server, EMOTE_EVENT_T(d)->protocol->api->label);
           em_gui_server_add(d->server, EMOTE_EVENT_T(d)->protocol);

           c = emote_event_new
               (
                  EMOTE_EVENT_T(d)->protocol,
                  EMOTE_EVENT_CHAT_CHANNEL_JOIN,
                  d->server,
                  "#emote"
               );
            emote_event_send(c);
            break;
        }
      case EMOTE_EVENT_SERVER_DISCONNECTED:
        {
           Emote_Event_Server *d;

           d = event;
           printf("Server %s from protocol %s is now disconnected.\n",
                  d->server, EMOTE_EVENT_T(d)->protocol->api->label);
           break;
        }
      case EMOTE_EVENT_CHAT_CHANNEL_JOINED:
        {
           Emote_Event_Chat_Channel *d;

           d = event;
           em_gui_channel_add(EMOTE_EVENT_SERVER_T(d)->server, d->channel, EMOTE_EVENT_T(d)->protocol);
           break;
        }
      case EMOTE_EVENT_SERVER_MESSAGE_RECEIVED:
        {
           Emote_Event_Server_Message *d;

           d = event;

           em_gui_message_add(EMOTE_EVENT_SERVER_T(d)->server, NULL, d->message);
           break;
        }
      case EMOTE_EVENT_CHAT_CHANNEL_MESSAGE_RECEIVED:
        {
           Emote_Event_Chat_Channel_Message *d;

           d = event;
           em_gui_message_add(EMOTE_EVENT_SERVER_T(d)->server, EMOTE_EVENT_CHAT_CHANNEL_T(d)->channel, d->message);
           break;
        }
      default:
         printf("Unhandled Event!\n");
         return EINA_FALSE;
   }

   return EINA_TRUE;
}

#endif
ELM_MAIN();
