#ifndef EM_TYPEDEFS

#else
# ifndef EM_IRC_H
#  define EM_IRC_H

EM_INTERN int em_irc_init(void);
EM_INTERN int em_irc_shutdown(void);
EM_INTERN int em_irc_connect(const char *server, int port);
EM_INTERN int em_irc_disconnect(const char *server);
EM_INTERN int em_irc_pass(const char *server, const char *pass);
EM_INTERN int em_irc_nick(const char *server, const char *nick);
EM_INTERN int em_irc_user(const char *server, const char *nick);
EM_INTERN int em_irc_join(const char *server, const char *chan);
EM_INTERN int em_irc_message(const char *server, const char *chan, const char *message);
EM_INTERN int em_irc_identify(const char *server, const char *pass);
EM_INTERN int em_irc_ghost(const char *server, const char *nick, const char *pass);
EM_INTERN int em_irc_part(const char *server, const char *channel, const char *reason);
EM_INTERN int em_irc_back(const char *server);
EM_INTERN int em_irc_away(const char *server, const char *reason);
EM_INTERN int em_irc_away_status(const char *server, const char *channel);
EM_INTERN int em_irc_kick(const char *server, const char *channel, const char *nick, const char *reason);
EM_INTERN int em_irc_invite(const char *server, const char *channel, const char *nick);
EM_INTERN int em_irc_mode(const char *server, const char *channel, const char *mode);
EM_INTERN int em_irc_user_list(const char *server, const char *channel);
EM_INTERN int em_irc_user_host(const char *server, const char *nick);
EM_INTERN int em_irc_user_whois(const char *server, const char *nick);
EM_INTERN int em_irc_action(const char *server, const char *channel, const char *action);
EM_INTERN int em_irc_notice(const char *server, const char *channel, const char *notice);
EM_INTERN int em_irc_topic(const char *server, const char *channel, const char *topic);
EM_INTERN int em_irc_channels_list(const char *server, const char *arg);
EM_INTERN int em_irc_names(const char *server, const char *channel);
EM_INTERN int em_irc_ping(const char *server, const char *to, const char *timestring);
EM_INTERN int em_irc_pong(const char *server, const char *msg);

# endif
#endif
