#ifndef __EVFS_SERVER_H_
#define __EVFS_SERVER_H_

typedef struct evfs_connection evfs_connection;
struct evfs_connection
{
   Ecore_Ipc_Server *server;
   unsigned long id;
   void (*callback_func) (EvfsEvent * data, void *obj);
   EvfsEvent *prog_event;
   void *obj;
};

typedef struct evfs_server evfs_server;
struct evfs_server
{
   Ecore_Hash *client_hash;
   Ecore_Hash *worker_hash;
   Ecore_Hash *plugin_uri_hash;
   Ecore_Hash *plugin_meta_hash;
   Ecore_Hash *plugin_vfolder_hash;
   
   Ecore_Ipc_Server *ipc_server;
   Ecore_Ipc_Server *worker_server;

   Ecore_Timer* tmr;
   unsigned long clientCounter;

   Ecore_List *incoming_command_list;

   int num_clients;
};

#endif
