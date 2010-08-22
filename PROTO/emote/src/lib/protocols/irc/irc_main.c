#include <stdio.h>
#include "em_private.h"

int
irc_init()
{
  printf("Initializing IRC\n");

  return 1;
}

int
irc_connect(char* server, uint16_t port, char* username, char* password)
{
  printf("Connecting IRC server %s:%s@%s:%d\n", username, password, server, port);

  return 1;
}

void
irc_shutdown()
{
  printf("Shutting Down IRC\n");
}

const em_functions em_plugin =
{
  "IRC",
  "Allows communication via Internet Relay Chat protocol",
  irc_init,
  irc_connect,
  irc_shutdown
};
