#ifndef IRC_H
# define IRC_H

#include "Emote.h"

EMAPI extern Emote_Protocol_Api protocol_api;

EMAPI int protocol__init(Emote_Protocol *p);
EMAPI int protocol__shutdown(void);
EMAPI int protocol__connect(const char *server, int port, const char *user, const char *pass);
EMAPI int protocol__disconnect(const char *server);
EMAPI int protocol_irc_pass(const char *server, const char *pass);
EMAPI int protocol_irc_nick(const char *server, const char *nick);
EMAPI int protocol_irc_user(const char *server, const char *nick);
EMAPI int protocol_irc_join(const char *server, const char *chan);
EMAPI int protocol_irc_message(const char *server, const char *chan, const char *message);
EMAPI int protocol_irc_identify(const char *server, const char *pass);
EMAPI int protocol_irc_ghost(const char *server, const char *nick, const char *pass);
EMAPI int protocol_irc_part(const char *server, const char *channel, const char *reason);
EMAPI int protocol_irc_back(const char *server);
EMAPI int protocol_irc_away(const char *server, const char *reason);
EMAPI int protocol_irc_away_status(const char *server, const char *channel);
EMAPI int protocol_irc_kick(const char *server, const char *channel, const char *nick, const char *reason);
EMAPI int protocol_irc_invite(const char *server, const char *channel, const char *nick);
EMAPI int protocol_irc_mode(const char *server, const char *channel, const char *mode);
EMAPI int protocol_irc_user_list(const char *server, const char *channel);
EMAPI int protocol_irc_user_host(const char *server, const char *nick);
EMAPI int protocol_irc_user_whois(const char *server, const char *nick);
EMAPI int protocol_irc_action(const char *server, const char *channel, const char *action);
EMAPI int protocol_irc_notice(const char *server, const char *channel, const char *notice);
EMAPI int protocol_irc_topic(const char *server, const char *channel, const char *topic);
EMAPI int protocol_irc_channels_list(const char *server, const char *arg);
EMAPI int protocol_irc_names(const char *server, const char *channel);
EMAPI int protocol_irc_ping(const char *server, const char *to, const char *timestring);
EMAPI int protocol_irc_pong(const char *server, const char *msg);

#endif
