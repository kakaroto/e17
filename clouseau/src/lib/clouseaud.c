#include <stdio.h>
#include <unistd.h>           /*  misc. UNIX functions      */
#include <fcntl.h>
#include <Ecore.h>
#include <Ecore_Ipc.h>

#include "helper.h"
#include "eet_dump.h"

#define RUNNING_DIR  "/tmp"
#define LOCK_FILE "clouseaud.lock"
#define LOG_FILE  "clouseaud.log"
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define __UNUSED__
#endif

static Eina_List *info = NULL; /* Holds app info to be send to GUI client */

/* For Debug */
char msg_buf[MAX_LINE+1];

struct _Client {
     int sdata;
};

struct _tree_info_st
{
   void *data;
   size_t size;
};
typedef struct _tree_info_st tree_info_st;

Eina_List *trees = NULL;

static void
log_message(char *filename, char *mode, char *message)
{
   FILE *logfile;
   logfile=fopen(filename, mode);
   if(!logfile) return;
   fprintf(logfile,"%s\n",message);
   fclose(logfile);
}

static void
_daemon_cleanup(void)
{  /*  Free strings */
   tree_info_st *tree;
   EINA_LIST_FREE(trees, tree)
     {
        free(tree->data);
        free(tree);
     }

   Ecore_Ipc_Server *svr;
   Ecore_Ipc_Client *cl;
   const Eina_List *clients, *l;

   clients = ecore_ipc_server_clients_get(svr);
   sprintf(msg_buf,"Clients connected to this server when exiting: %d\n",
         eina_list_count(clients));
   log_message(LOG_FILE, "a", msg_buf);
   EINA_LIST_FOREACH(clients, l, cl)
     {
        sprintf(msg_buf, "%s\n", ecore_ipc_client_ip_get(cl));
        log_message(LOG_FILE, "a", msg_buf);
        free(ecore_ipc_client_data_get(cl));
     }

   ecore_ipc_shutdown();
   ecore_shutdown();
   eina_shutdown();
}

void signal_handler(int sig)
{
   switch(sig) {
      case SIGHUP:
         log_message(LOG_FILE, "a", "hangup signal catched");
         break;
      case SIGTERM:
         log_message(LOG_FILE, "a", "terminate signal catched");
         _daemon_cleanup();
         exit(0);
         break;
   }
}

void daemonize(void)
{
   int i,lfp;
   char str[10];
   time_t currentTime;

   if(getppid()==1) return; /* already a daemon */
   i=fork();
   if (i<0) exit(1); /* fork error */
   if (i>0) exit(0); /* parent exits */

   time (&currentTime);

   /* child (daemon) continues */
   setsid(); /* obtain a new process group */
   for (i=getdtablesize();i>=0;--i) close(i); /* close all descriptors */
   i=open("/dev/null",O_RDWR); dup(i); dup(i); /* handle standart I/O */
   umask(027); /* set newly created file permissions */
   chdir(RUNNING_DIR); /* change running directory */
   lfp=open(LOCK_FILE,O_RDWR|O_CREAT,0640);
   if (lfp<0) exit(1); /* can not open */
   if (lockf(lfp,F_TLOCK,0)<0) exit(0); /* can not lock */
   /* first instance continues */
   sprintf(str,"%d\n",getpid());
   write(lfp,str,strlen(str)); /* record pid to lockfile */
   signal(SIGCHLD,SIG_IGN); /* ignore child */
   signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
   signal(SIGTTOU,SIG_IGN);
   signal(SIGTTIN,SIG_IGN);
   signal(SIGHUP,signal_handler); /* catch hangup signal */
   signal(SIGTERM,signal_handler); /* catch kill signal */

   log_message(LOG_FILE, "w", "Daemon Started");
   log_message(LOG_FILE, "a", ctime(&currentTime));
}

/* START - Ecore communication callbacks */
Eina_Bool
_add(void *data __UNUSED__, int type __UNUSED__, Ecore_Ipc_Event_Client_Add *ev)
{
   void *p;
   char *msg="hello! - sent from the server";
   int size = 0;

   data_desc *td = _data_descriptors_init();
   ack_st t = { msg };
   Variant_st *v = variant_alloc(DAEMON_ACK, sizeof(t), &t);
   p = eet_data_descriptor_encode(td->_variant_descriptor , v, &size);
   ecore_ipc_client_send(ev->client, 0,0,0,0,EINA_FALSE, p, size);
   ecore_ipc_client_flush(ev->client);
   free(p);
   variant_free(v);



#if 0

   log_message(LOG_FILE, "a", msg_buf);
   void *p;
   char *msg="hello! - sent from the server";
   size_t size = compose_packet(&p, DAEMON, TSUITE_EVENT_ACK, msg, strlen(msg)+1);
   Ecore_Ipc_Server *srv;
   Ecore_Ipc_Client *cl;
   const Eina_List *clients, *l;

   struct _Client *client = malloc(sizeof(*client));
   client->sdata = 0;

   ecore_ipc_client_send(ev->client, 0,0,0,0,EINA_FALSE,p, size);
   ecore_ipc_client_flush(ev->client);
   free(p);

   ecore_ipc_client_data_set(ev->client, client);

   srv = ecore_ipc_client_server_get(ev->client);
   sprintf(msg_buf,"Clients connected to this server:\n");
   log_message(LOG_FILE, "a", msg_buf);

   clients = ecore_ipc_server_clients_get(srv);
   EINA_LIST_FOREACH(clients, l, cl)
     {
        sprintf(msg_buf, "%s\n", ecore_ipc_client_ip_get(cl));
        log_message(LOG_FILE, "a", msg_buf);
     }
#endif
   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
_del(void *data __UNUSED__, int type __UNUSED__, Ecore_Ipc_Event_Client_Del *ev)
{
   struct _Client *client;

   if (!ev->client)
     return ECORE_CALLBACK_RENEW;
/*
   client = ecore_ipc_client_data_get(ev->client);

   sprintf(msg_buf, "Total data received from this client: %d\n", client->sdata);
   log_message(LOG_FILE, "a", msg_buf);
   if (client)
     free(client);
*/
   ecore_ipc_client_del(ev->client);

   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
_data(void *data __UNUSED__, int type __UNUSED__, Ecore_Ipc_Event_Client_Data *ev)
{
   log_message(LOG_FILE, "a", "Server Got data from Client");
   void *p;
   char *msg="hello! - sent from the server";
   int size = 0;

   data_desc *td = _data_descriptors_init();
   Variant_st *v = eet_data_descriptor_decode(td->_variant_descriptor,
         ev->data, ev->size);
   if (v)
     {
        switch(packet_mapping_type_get(v->t.type))
          {
           case APP_ACK:
                {
                   char *msg2="server asking for tree";
                   ack_st *t = v->data;
                   log_message(LOG_FILE, "a", "Got data from APP");
                   log_message(LOG_FILE, "a", t->text);

                   data_desc *td = _data_descriptors_init();
                   ack_st dt = { msg2 };
                   Variant_st *v = variant_alloc(DAEMON_ACK, sizeof(dt), &dt);
                   void *p = eet_data_descriptor_encode(td->_variant_descriptor , v, &size);
                   ecore_ipc_client_send(ev->client, 0,0,0,0,EINA_FALSE, p, size);
                   ecore_ipc_client_flush(ev->client);
                   free(p);
                   variant_free(v);
                   log_message(LOG_FILE, "a", "Asking tree from APP");
                }
              break;

           case GUI_ACK:
                {
                   ack_st *t = v->data;
                   log_message(LOG_FILE, "a", "Got data from GUI");
                   log_message(LOG_FILE, "a", t->text);
                }
              break;

           case APP_TREE_DATA:
              log_message(LOG_FILE, "a", "Got tree-data from APP");
              break;
          }
     }
   else
     log_message(LOG_FILE, "a", "Failed to decode data.");


/*
   test_st *t = test_st_new(GUI, TSUITE_EVENT_ACK, msg);
   p = eet_data_descriptor_encode(td, t, &size);

   ecore_ipc_client_send(ev->client, 0,0,0,0,EINA_FALSE, p, size);
   ecore_ipc_client_flush(ev->client);
   free(p);
   free(t);
*/

#if 0
   char fmt[128];
   struct _Client *client = ecore_ipc_client_data_get(ev->client);
   Variant_st *pkt = get_packet_data(ev->data, ev->size);

   snprintf(fmt, sizeof(fmt), "%s %d pkt->client=<%d> pkt->type=<%d>", __func__, __LINE__, pkt->client, pkt->type);
   log_message(LOG_FILE, "a", fmt);
   snprintf(fmt, sizeof(fmt),
         "Received %i bytes from client\n",ev->size);
   log_message(LOG_FILE, "a", fmt);

   switch(pkt->client)
     {
      case APP:  /* Get info from app */
         log_message(LOG_FILE, "a", "Message from APP.");
         if (pkt->type == TSUITE_EVENT_TREE_ITEM)
           {
              log_message(LOG_FILE, "a", "Tree info message from APP.");
              char txt[1024];
              tree_info_st *tree = malloc(sizeof(*tree));
              tree->data = ev->data;
              tree->size = ev->size;
              trees = eina_list_append(trees, tree);
              sprintf(txt, "Adding Tree (%d)", eina_list_count(trees));
              log_message(LOG_FILE, "a", txt);
           }
         break;

      case GUI:  /* Sent trees to GUI */
         log_message(LOG_FILE, "a", "Message from GUI.");
         if (pkt->type == TSUITE_EVENT_TREE_ITEM)
           {
              log_message(LOG_FILE, "a", "Tree info message from GUI.");
              Eina_List *l;
              tree_info_st *tree;
              EINA_LIST_FOREACH(trees, l, tree)
                {  /* Send each tree in a packet */
                   log_message(LOG_FILE, "a", "Sending Tree");
                   ecore_ipc_client_send(ev->client,0,0,0,0,EINA_FALSE,tree->data, tree->size);
                   ecore_ipc_client_flush(ev->client);
                }

           }
         break;
     }

   client->sdata += ev->size;
#endif
   return ECORE_CALLBACK_RENEW;
}
/* END   - Ecore communication callbacks */


int main(void)
{
   Ecore_Ipc_Server *svr;
   Ecore_Ipc_Client *cl;
   const Eina_List *clients, *l;

   daemonize();
   eina_init();
   ecore_init();
   ecore_ipc_init();

   if (!(svr = ecore_ipc_server_add(ECORE_IPC_REMOTE_SYSTEM, LOCALHOST, PORT, NULL)))
     exit(1);

   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD, (Ecore_Event_Handler_Cb)_add, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL, (Ecore_Event_Handler_Cb)_del, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, (Ecore_Event_Handler_Cb)_data, NULL);

   ecore_main_loop_begin();
   _daemon_cleanup();

   return 0;
}
