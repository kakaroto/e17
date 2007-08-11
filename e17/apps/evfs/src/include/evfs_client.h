#ifndef __EVFS_CLIENT_H_
#define __EVFS_CLIENT_H_

#include "evfs_command.h"

typedef struct evfs_client evfs_client;
struct evfs_client
{
 
   /*Evfs-> evfs client*/
   Ecore_Ipc_Client *client;

   /*Worker master server - fork side*/
   Ecore_Ipc_Server *master;

   /*Worker -> evfs server client*/
   Ecore_Ipc_Client *worker_client;
   
   Ecore_Event_Handler* event_handler;
   unsigned long id;

   struct evfs_server *server;
   int pid;
   evfs_command *prog_command;
};

#endif
