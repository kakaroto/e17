#include "libemote.h"
#include "irc.h"

EMAPI Emote_Protocol_Api emote_protocol_api = 
{
   /* version, name, label */
   EMOTE_PROTOCOL_API_VERSION, "irc", "IRC"
};

EMAPI int 
emote_protocol_init(Emote_Protocol *p) 
{
   /* p->funcs.<something> = _local_function */
   return 1;
}

EMAPI int 
emote_protocol_shutdown(Emote_Protocol *p) 
{
   /* p->funcs.<something> = NULL */
   return 1;
}
