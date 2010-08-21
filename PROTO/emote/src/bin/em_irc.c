#include "emote.h"

/* local function prototypes */
static Eina_Bool _em_irc_cb_server_add(void *data __UNUSED__, int type __UNUSED__, void *event);
static Eina_Bool _em_irc_cb_server_del(void *data, int type __UNUSED__, void *event);
static Eina_Bool _em_irc_cb_server_data(void *data __UNUSED__, int type __UNUSED__, void *event);

/* local variables */
static Eina_Hash *_em_servers = NULL;

EM_INTERN int 
em_irc_init(void) 
{
   _em_servers = eina_hash_string_superfast_new(NULL);
   return 1;
}

EM_INTERN int 
em_irc_shutdown(void) 
{
   if (_em_servers) 
     eina_hash_free(_em_servers);
   return 1;
}

EM_INTERN int 
em_irc_connect(const char *server, int port) 
{
   Ecore_Con_Server *serv = NULL;

   if (!server) return 0;

   /* see if we are already connected to this server */
   if (!(serv = eina_hash_find(_em_servers, server))) 
     {
        if (port <= 0)
          serv = ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM, 
                                          server, 6667, NULL);
        else
          serv = ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM, 
                                          server, port, NULL);
        if (!serv) return 0;
        printf("Hello\n");
        ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD, 
                                _em_irc_cb_server_add, server);
        ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL, 
                                _em_irc_cb_server_del, server);
        ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA, 
                                _em_irc_cb_server_data, server);
        eina_hash_add(_em_servers, server, serv);
     }
   return 1;
}

EM_INTERN int 
em_irc_disconnect(const char *server) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if (!server) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "QUIT\r\n");
   ecore_con_server_send(serv, buf, len);
   ecore_con_server_flush(serv);
   ecore_con_server_del(serv);
   return 1;
}

EM_INTERN int
em_irc_pass(const char *server, const char *pass)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!pass)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "PASS %s\r\n", pass);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int
em_irc_nick(const char *server, const char *nick)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!nick)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "NICK %s\r\n", nick);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int
em_irc_user(const char *server, const char *nick)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512], host[64];
   int len = 0;

   if ((!server) || (!nick)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   gethostname(host, 63);
   len = snprintf(buf, sizeof(buf), "USER %s %s %s :%s\r\n",
		  nick, host, server, "Emote Tester");
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int
em_irc_join(const char *server, const char *chan)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!chan)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "JOIN %s\r\n", chan);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int
em_irc_message(const char *server, const char *chan, const char *message)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!chan) || (!message)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "PRIVMSG %s :%s\r\n", chan, message);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_identify(const char *server, const char *pass) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!pass)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "IDENTIFY %s\r\n", pass);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_ghost(const char *server, const char *nick, const char *pass) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!nick) || (!pass)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "GHOST %s %s\r\n", nick, pass);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_part(const char *server, const char *channel, const char *reason) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   if (reason[0])
     len = snprintf(buf, sizeof(buf), "PART %s :%s\r\n", channel, reason);
   else
     len = snprintf(buf, sizeof(buf), "PART %s\r\n", channel);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_back(const char *server) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if (!server) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "AWAY\r\n");
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_away(const char *server, const char *reason) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if (!server) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
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

EM_INTERN int 
em_irc_away_status(const char *server, const char *channel) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "WHO %s\r\n", channel);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_kick(const char *server, const char *channel, const char *nick, const char *reason) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel) || (!nick)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   if (reason[0]) 
     len = snprintf(buf, sizeof(buf), "KICK %s %s :%s\r\n", 
                    channel, nick, reason);
   else
     len = snprintf(buf, sizeof(buf), "KICK %s %s\r\n", channel, nick);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_invite(const char *server, const char *channel, const char *nick) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel) || (!nick)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "INVITE %s %s\r\n", nick, channel);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_mode(const char *server, const char *channel, const char *mode) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel) || (!mode)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "MODE %s %s\r\n", channel, mode);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_user_list(const char *server, const char *channel) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "WHO %s\r\n", channel);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_user_host(const char *server, const char *nick) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!nick)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "USERHOST %s\r\n", nick);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_user_whois(const char *server, const char *nick) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!nick)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "WHOIS %s\r\n", nick);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_action(const char *server, const char *channel, const char *action) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel) || (!action)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), 
                  "PRIVMSG %s :\001ACTION %s\001\r\n", channel, action);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_notice(const char *server, const char *channel, const char *notice) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel) || (!notice)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "NOTICE %s :%s\r\n", channel, notice);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_topic(const char *server, const char *channel, const char *topic) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
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

EM_INTERN int 
em_irc_channels_list(const char *server, const char *arg) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if (!server) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   if (arg[0]) 
     len = snprintf(buf, sizeof(buf), "LIST %s\r\n", arg);
   else
     len = snprintf(buf, sizeof(buf), "LIST\r\n");
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_names(const char *server, const char *channel) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!channel)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "NAMES %s\r\n", channel);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_ping(const char *server, const char *to, const char *timestring) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!timestring)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   if ((to) && (to[0]))
     len = snprintf(buf, sizeof(buf), 
                    "PRIVMSG %s :\001PING %s\001\r\n", to, timestring);
   else
     len = snprintf(buf, sizeof(buf), "PING %s\r\n", timestring);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

EM_INTERN int 
em_irc_pong(const char *server, const char *msg) 
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if ((!server) || (!msg)) return 0;
   if (!(serv = eina_hash_find(_em_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "PONG %s\n", msg);
   ecore_con_server_send(serv, buf, len);
   return 1;
}

/* local functions */
static Eina_Bool 
_em_irc_cb_server_add(void *data __UNUSED__, int type __UNUSED__, void *event) 
{
   Ecore_Con_Event_Server_Add *ev;

   ev = event;
   printf("Emote: Server Added\n");
   return EINA_FALSE;
}

static Eina_Bool 
_em_irc_cb_server_del(void *data, int type __UNUSED__, void *event) 
{
   Ecore_Con_Event_Server_Del *ev;
   const char *server;

   ev = event;
   if (!(server = data)) return EINA_FALSE;
   printf("Emote: Server Deleted\n");
   eina_hash_del_by_key(_em_servers, server);
   return EINA_FALSE;
}

static Eina_Bool 
_em_irc_cb_server_data(void *data __UNUSED__, int type __UNUSED__, void *event) 
{
   Ecore_Con_Event_Server_Data *ev;
   char *msg;

   ev = event;
   msg = calloc((ev->size + 1), sizeof(char));
   strncpy(msg, ev->data, ev->size);
   printf("%s\n", msg);
   em_irc_parse_input(msg);
   free(msg);
   return EINA_FALSE;
}
