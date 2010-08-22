#include "libemote.h"
#include "irc.h"

EMAPI Emote_Protocol_Api protocol_api =
{
   /* version, name, label */
   EMOTE_PROTOCOL_API_VERSION, "irc", "IRC"
};

EMAPI int
protocol_init(Emote_Protocol *p __UNUSED__)
{
   /* p->funcs.<something> = _local_function */
   return 1;
}

EMAPI int
protocol_shutdown(Emote_Protocol *p __UNUSED__)
{
   /* p->funcs.<something> = NULL */
   return 1;
}
