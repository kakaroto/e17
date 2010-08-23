#ifndef IRC_H
# define IRC_H

#include "Emote.h"

EMAPI extern Emote_Protocol_Api protocol_api;

EMAPI int protocol_init(Emote_Protocol *p);
EMAPI int protocol_shutdown(void);
EMAPI int protocol_connect(const char *server, int port, const char *user, const char *pass);
EMAPI int protocol_disconnect(const char *server);

#endif
