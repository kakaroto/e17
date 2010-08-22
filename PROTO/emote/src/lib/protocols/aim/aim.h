#ifndef AIM_H
# define AIM_H

#include <Ecore.h>
#include <Ecore_Con.h>

EMAPI extern Emote_Protocol_Api emote_protocol_api;

EMAPI int protocol_init(void);
EMAPI int protocol_shutdown(void);
EMAPI int protocol_connect(const char* server, int port, const char* user,
                           const char* pass);
EMAPI int protocol_disconnect(void);

#endif
