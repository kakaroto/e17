#include <stdio.h>
#include "em_private.h"

int
aim_init()
{
  printf("Initializing AIM\n");

  return 1;
}

int
aim_connect(char* server, uint16_t port, char* username, char* password)
{
  printf("Connecting AIM server %s:%s@%s:%d\n", username, password, server, port);

  return 1;
}

void
aim_shutdown()
{
  printf("Shutting Down AIM\n");
}

const em_functions em_plugin =
{
  "AIM",
  "Allows communication via AOL Instant Messenger protocol",
  aim_init,
  aim_connect,
  aim_shutdown
};
