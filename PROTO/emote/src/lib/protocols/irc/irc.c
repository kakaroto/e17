#include "libemote.h"
#include "irc.h"

EMAPI Emote_Protocol_Api protocol_api =
{
   /* version, name, label */
   EMOTE_PROTOCOL_API_VERSION, "irc", "IRC"
};

EMAPI int
protocol_init(void)
{
   return 1;
}

EMAPI int
protocol_shutdown(void)
{
   printf("IRC Shutdown\n");
   /* p->funcs.<something> = NULL */
   return 1;
}

EMAPI int
protocol_connect(const char *server, int port, const char *user, const char *pass)
{
   return 1;
}

EMAPI int
protocol_disconnect(void)
{
   return 1;
}
