#include "irc.h"
#include "irc_parse.h"
#include "emote_private.h"

#include <Ecore_Con.h>
#include <unistd.h>

EMAPI Emote_Protocol_Api protocol_api =
{
   /* version, name, label */
   EMOTE_PROTOCOL_API_VERSION, "irc", "IRC"
};

typedef struct _IRC_Server_Params IRC_Server_Params;

static Eina_Bool _irc_cb_server_add(void *data, int type __UNUSED__, void *event __UNUSED__);
static Eina_Bool _irc_cb_server_del(void *data, int type __UNUSED__, void *event __UNUSED__);
static Eina_Bool _irc_cb_server_data(void *data, int type __UNUSED__, void *event);
static Eina_Bool _irc_event_handler(void *data __UNUSED__, int type, void *event);

static Emote_Protocol *m;
static Eina_Hash *_irc_servers = NULL;

static Emote_Event_Type _em_events[] =
{
   EMOTE_EVENT_SERVER_CONNECT,
   EMOTE_EVENT_SERVER_DISCONNECT,
   EMOTE_EVENT_CHAT_JOIN,
   EMOTE_EVENT_CHAT_MESSAGE_SEND,
   EMOTE_EVENT_SERVER_MESSAGE_SEND
};

struct _IRC_Server_Params
{
   const char *name;
   int port;
   const char *nick;
   const char *user;
   const char *pass;
};

EMAPI int
protocol_init(Emote_Protocol *p)
{
   unsigned int i;
   m = p;

   ecore_con_init();

   _irc_servers = eina_hash_string_superfast_new(NULL);

   for (i = 0; i < (sizeof(_em_events)/sizeof(Emote_Event_Type)); ++i)
      emote_event_handler_add(_em_events[i], _irc_event_handler, NULL);

   return 1;
}

EMAPI int
protocol_shutdown(void)
{
   ecore_con_shutdown();
   if (_irc_servers)
     eina_hash_free(_irc_servers);
   return 1;
}

int
protocol_irc_connect(const char *server, int port, const char *nick, const char *user, const char *pass)
{
   Ecore_Con_Server *serv = NULL;
   IRC_Server_Params *params;

   if (!server) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server)))
     {
        if (port <= 0) port = 6667;
        if (!user)
          user = getlogin();
        if (!nick)
          nick = user;
        if (!pass)
          pass = user;

        // Create params
        params = EMOTE_NEW(IRC_Server_Params, 1);
        params->name = ((server != NULL) ? eina_stringshare_add(server) : NULL);
        params->nick = ((nick != NULL) ? eina_stringshare_add(nick) : NULL);
        params->user = ((user != NULL) ? eina_stringshare_add(user) : NULL);
        params->pass = ((pass != NULL) ? eina_stringshare_add(pass) : NULL);
        params->port = port;

        serv = ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM,
                                        server, port, params);
        ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD,
                                _irc_cb_server_add, NULL);
        ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL,
                                _irc_cb_server_del, NULL);
        ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA,
                                _irc_cb_server_data, NULL);
        eina_hash_add(_irc_servers, server, serv);

        protocol_irc_pass(server, pass);
        protocol_irc_user(server, user);
        protocol_irc_nick(server, nick);
     }
   return 1;
}

int
protocol_irc_disconnect(const char *server)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if (!server) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "QUIT\r\n");
   ecore_con_server_send(serv, buf, len);
   ecore_con_server_flush(serv);
   ecore_con_server_del(serv);
   return 1;
}

int
protocol_irc_pass(const char *server, const char *pass)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!pass)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "PASS %s\r\n", pass);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_nick(const char *server, const char *nick)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!nick)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "NICK %s\r\n", nick);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_user(const char *server, const char *nick)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512], host[64];
   int len = 0;

   if ((!server) || (!nick)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   gethostname(host, 63);
   len = snprintf(buf, sizeof(buf), "USER %s %s %s :%s\r\n",
		  nick, host, server, "Entourage Tester");
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_join(const char *server, const char *chan)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!chan)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "JOIN %s\r\n", chan);
   ecore_con_server_send(serv, buf, len);

   return 1;
}

int
protocol_irc_command(const char *server, const char *message)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!message)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;

   printf("Parsing Command %s\n", message);

   len = snprintf(buf, sizeof(buf), "%s\r\n", message);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_message(const char *server, const char *chan, const char *message)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!chan) || (!message)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "PRIVMSG %s :%s\r\n", chan, message);

   printf("Sending %s\n", buf);

   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_identify(const char *server, const char *pass)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!pass)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "IDENTIFY %s\r\n", pass);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_ghost(const char *server, const char *nick, const char *pass)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!nick) || (!pass)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "GHOST %s %s\r\n", nick, pass);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_part(const char *server, const char *channel, const char *reason)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   if (reason[0])
     len = snprintf(buf, sizeof(buf), "PART %s :%s\r\n", channel, reason);
   else
     len = snprintf(buf, sizeof(buf), "PART %s\r\n", channel);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_back(const char *server)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if (!server) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "AWAY\r\n");
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_away(const char *server, const char *reason)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if (!server) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   if (reason)
     {
        if (!reason[0]) reason = " ";
     }
   else
     reason = " ";
   len = snprintf(buf, sizeof(buf), "AWAY :%s\r\n", reason);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_away_status(const char *server, const char *channel)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "WHO %s\r\n", channel);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_kick(const char *server, const char *channel, const char *nick, const char *reason)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel) || (!nick)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   if (reason[0])
     len = snprintf(buf, sizeof(buf), "KICK %s %s :%s\r\n",
                    channel, nick, reason);
   else
     len = snprintf(buf, sizeof(buf), "KICK %s %s\r\n", channel, nick);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_invite(const char *server, const char *channel, const char *nick)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel) || (!nick)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "INVITE %s %s\r\n", nick, channel);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_mode(const char *server, const char *channel, const char *mode)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel) || (!mode)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "MODE %s %s\r\n", channel, mode);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_user_list(const char *server, const char *channel)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "WHO %s\r\n", channel);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_user_host(const char *server, const char *nick)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!nick)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "USERHOST %s\r\n", nick);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_user_whois(const char *server, const char *nick)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!nick)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "WHOIS %s\r\n", nick);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_action(const char *server, const char *channel, const char *action)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel) || (!action)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf),
                  "PRIVMSG %s :\001ACTION %s\001\r\n", channel, action);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_notice(const char *server, const char *channel, const char *notice)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel) || (!notice)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "NOTICE %s :%s\r\n", channel, notice);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_topic(const char *server, const char *channel, const char *topic)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   if (!topic)
     len = snprintf(buf, sizeof(buf), "TOPIC %s :\r\n", channel);
   else if (topic[0])
     len = snprintf(buf, sizeof(buf), "TOPIC %s :%s\r\n", channel, topic);
   else
     len = snprintf(buf, sizeof(buf), "TOPIC %s\r\n", channel);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_channels_list(const char *server, const char *arg)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if (!server) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   if (arg[0])
     len = snprintf(buf, sizeof(buf), "LIST %s\r\n", arg);
   else
     len = snprintf(buf, sizeof(buf), "LIST\r\n");
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_names(const char *server, const char *channel)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "NAMES %s\r\n", channel);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_ping(const char *server, const char *to, const char *timestring)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!timestring)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   if ((to) && (to[0]))
     len = snprintf(buf, sizeof(buf),
                    "PRIVMSG %s :\001PING %s\001\r\n", to, timestring);
   else
     len = snprintf(buf, sizeof(buf), "PING %s\r\n", timestring);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

int
protocol_irc_pong(const char *server, const char *msg)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!msg)) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "PONG %s\n", msg);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

static Eina_Bool
_irc_cb_server_add(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Con_Event_Server_Data *ev;
   Emote_Event *d;
   IRC_Server_Params *params;

   ev = event;
   if (!ev->server) return EINA_FALSE;


   params = ecore_con_server_data_get(ev->server);
   d = emote_event_new
       (
          m,
          EMOTE_EVENT_SERVER_CONNECTED,
          params->name,
          params->port,
          params->nick,
          params->user,
          params->pass
       );
   emote_event_send(d);

   return EINA_FALSE;
}

static Eina_Bool
_irc_cb_server_del(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Con_Event_Server_Data *ev;
   Emote_Event *d;
   IRC_Server_Params *params;

   ev = event;
   if (!ev->server) return EINA_FALSE;

   d = emote_event_new(m, EMOTE_EVENT_SERVER_DISCONNECTED, ecore_con_server_name_get(ev->server));
   emote_event_send(d);

   params = ecore_con_server_data_get(ev->server);
   eina_hash_del_by_key(_irc_servers, params->name);
   if (params->name) eina_stringshare_del(params->name);
   if (params->nick) eina_stringshare_del(params->name);
   if (params->user) eina_stringshare_del(params->name);
   if (params->pass) eina_stringshare_del(params->name);
   EMOTE_FREE(params);

   return EINA_FALSE;
}

static Eina_Bool
_irc_cb_server_data(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Con_Event_Server_Data *ev;
   char *msg;

   ev = event;
   if(!ev->server) return EINA_FALSE;

   msg = calloc((ev->size + 1), sizeof(char));
   strncpy(msg, ev->data, ev->size);
   irc_parse_input(msg, ecore_con_server_name_get(ev->server), m);
   free(msg);

   return EINA_FALSE;
}

static Eina_Bool
_irc_event_handler(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   if (EMOTE_EVENT_T(event)->protocol != m)
      return EINA_FALSE;

   printf("%s Received Event %u\n", m->api->label, EMOTE_EVENT_T(event)->type);

   switch(EMOTE_EVENT_T(event)->type)
   {
      case EMOTE_EVENT_SERVER_CONNECT:
        {
           Emote_Event_Server_Connect *d;

           d = event;
           protocol_irc_connect(
                                  EMOTE_EVENT_SERVER_T(d)->server,
                                  d->port,
                                  d->nick,
                                  d->username,
                                  d->password
                               );
           break;
        }
      case EMOTE_EVENT_SERVER_DISCONNECT:
        {
           Emote_Event_Server *d;

           d = event;
           protocol_irc_disconnect(d->server);
           break;
        }
      case EMOTE_EVENT_CHAT_JOIN:
        {
           Emote_Event_Chat *d;

           d = event;
           protocol_irc_join(EMOTE_EVENT_SERVER_T(d)->server, d->channel);
           break;
        }
      case EMOTE_EVENT_CHAT_MESSAGE_SEND:
        {
           Emote_Event_Chat_Message *d;

           d = event;
           protocol_irc_message(EMOTE_EVENT_SERVER_T(d)->server,
                                     EMOTE_EVENT_CHAT_T(d)->channel,
                                     ((d->message[0] == '/') ? &(d->message[1]) : d->message));
           break;
        }
      case EMOTE_EVENT_SERVER_MESSAGE_SEND:
        {
           Emote_Event_Server_Message *d;

           d = event;
           protocol_irc_command(EMOTE_EVENT_SERVER_T(d)->server, &(d->message[1]));
           break;
        }
      default:
         printf("Unhandled Event!\n");
         return EINA_FALSE;
   }

   return EINA_TRUE;
}
