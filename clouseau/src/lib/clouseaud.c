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
   void *app;    /* app ptr to identify where the data came from */
   void *data;   /* Tree data */
};
typedef struct _tree_info_st tree_info_st;

static void
log_message(char *filename, char *mode, char *message)
{
   FILE *logfile;
   logfile=fopen(filename, mode);
   if(!logfile) return;
   fprintf(logfile,"%s\n",message);
   fclose(logfile);
}

static int
_appcmp(const void *d1, const void *d2)
{
   return ((((tree_info_st *) d1)->app) - d2);
}

Eina_List *trees = NULL;

static Eina_List *
tree_data_cleanup()
{
   tree_info_st *tree;
   log_message(LOG_FILE, "a","cleanup - clearing trees");
   EINA_LIST_FREE(trees, tree)
     {
        _item_tree_item_free(tree->data);
        free(tree);
     }

   return NULL;
}

static Eina_List *
tree_data_add(void *a, void *d)
{
   char m[64];

   sprintf(m, "trees list size <%d>", eina_list_count(trees));
   log_message(LOG_FILE, "a", m);
   tree_info_st *st = (tree_info_st *)
      eina_list_search_unsorted(trees, _appcmp, a);

   if (st)
     {
        sprintf(m, "Updating tree-data from APP <%p>", a);
        _item_tree_item_free(st->data);
        st->data = d;
     }
   else
     {
        tree_info_st *st = malloc(sizeof(*st));
        if (st)
          {
             sprintf(m, "Got tree-data from APP <%p>", a);

             st->app = a;
             st->data = d;

             trees = eina_list_append(trees, st);
          }
     }

   log_message(LOG_FILE, "a", m);
   return trees;
}

static void
_daemon_cleanup(void)
{  /*  Free strings */
   Ecore_Ipc_Server *svr;
   Ecore_Ipc_Client *cl;
   const Eina_List *clients, *l;

   trees = tree_data_cleanup();
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
   char m[128];
   int size = 0;

   ecore_ipc_client_data_size_max_set(ev->client, -1);
   sprintf(m, "<%s> msg from <%p>", __func__, ev->client);
   log_message(LOG_FILE, "a", m);


   data_desc *td = _data_descriptors_init();
   ack_st t = { msg };
   Variant_st *v = variant_alloc(DAEMON_ACK, sizeof(t), &t);
   p = eet_data_descriptor_encode(td->_variant_descriptor , v, &size);
   ecore_ipc_client_send(ev->client, 0,0,0,0,EINA_FALSE, p, size);
   ecore_ipc_client_flush(ev->client);
   free(p);
   variant_free(v);

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
   char m[128];
   sprintf(m, "<%s> msg from <%p>", __func__, ev->client);
   log_message(LOG_FILE, "a", m);
   ecore_ipc_client_del(ev->client);

   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
_data(void *data __UNUSED__, int type __UNUSED__, Ecore_Ipc_Event_Client_Data *ev)
{
   void *p;
   char *msg="hello! - sent from the server";
   int size = 0;

   char m[128];
   sprintf(m, "<%s> msg from <%p>", __func__, ev->client);
   log_message(LOG_FILE, "a", m);

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
                   log_message(LOG_FILE, "a", "--- 1 ---");
                   char m[128];
                   int size = 0;
                   Eina_List *l;
                   tree_info_st *st;
                   Tree_Item *t;
                   data_desc *td = _data_descriptors_init();
                   log_message(LOG_FILE, "a", "--- 2 ---");
                   EINA_LIST_FOREACH(trees, l, st)
                     {  /* Sending all trees */
                        t = st->data;
                        sprintf(m,"t = <%p>", t);
                        log_message(LOG_FILE, "a", m);
                        v = variant_alloc(DAEMON_TREE_DATA, sizeof(*t), t);
                        void *p = eet_data_descriptor_encode(
                              td->_variant_descriptor , v, &size);

                   log_message(LOG_FILE, "a", "--- 3 ---");
                        sprintf(m, "DAEMON <%p> sending tree, size <%d>\n",
                              st->app, size);

                        log_message(LOG_FILE, "a", m);
                        ecore_ipc_client_send(ev->client, 0,0,0,0,
                              EINA_FALSE, p, size);

                        ecore_ipc_client_flush(ev->client);
                        free(p);
                        variant_free(v);
                     }
                }
              break;

           case APP_TREE_DATA:
                {
                   char m[1024];
                   sprintf(m, "<%s> got APP TREE DATA from <%p>", __func__, ev->client);
                   log_message(LOG_FILE, "a", m);
                   trees = tree_data_add(ev->client, v->data);
                }
              break;
          }
     }
   else
     log_message(LOG_FILE, "a", "Failed to decode data.");

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

   ecore_ipc_server_data_size_max_set(svr, -1);

   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD, (Ecore_Event_Handler_Cb)_add, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL, (Ecore_Event_Handler_Cb)_del, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, (Ecore_Event_Handler_Cb)_data, NULL);

   ecore_main_loop_begin();
   _daemon_cleanup();

   return 0;
}
