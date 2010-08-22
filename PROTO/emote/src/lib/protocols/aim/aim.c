#include "libemote.h"
#include "aim.h"

EMAPI Emote_Protocol_Api protocol_api =
{
   /* version, name, label */
   EMOTE_PROTOCOL_API_VERSION, "aim", "AIM"
};

EMAPI int
protocol_init(void)
{
   return 1;
}

EMAPI int
protocol_shutdown(void)
{
   return 1;
}

EMAPI int
protocol_connect(const char *server, int port, const char *user, const char *pass)
{
   return 1;
}

EMAPI int
protocol_disconnect(const char *server)
{
   return 1;
}
