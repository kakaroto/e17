#ifndef __EVFS_CLIENT_H_
#define __EVFS_CLIENT_H_

#include "evfs_command.h"

typedef struct evfs_client evfs_client;
struct evfs_client
{
   Ecore_Ipc_Client *client;
   unsigned long id;

   struct evfs_server *server;
   evfs_command *prog_command;
};

#endif
