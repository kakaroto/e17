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
        item_tree_item_free(tree->data);
        free(tree);
     }

   return NULL;
}

static Eina_List *
tree_data_add(void *a, void *d)
{
   sprintf(msg_buf, "trees list size <%d>", eina_list_count(trees));
   log_message(LOG_FILE, "a", msg_buf);
   tree_info_st *st = (tree_info_st *)
      eina_list_search_unsorted(trees, _appcmp, a);

   if (st)
     {
        sprintf(msg_buf, "Updating tree-data from APP <%p>", a);
        item_tree_item_free(st->data);
        st->data = d;
     }
   else
     {
        tree_info_st *st = malloc(sizeof(*st));
        if (st)
          {
             sprintf(msg_buf, "Got tree-data from APP <%p>", a);

             st->app = a;
             st->data = d;

             trees = eina_list_append(trees, st);
          }
     }

   log_message(LOG_FILE, "a", msg_buf);
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

   data_descriptors_shutdown();
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

   ecore_ipc_client_data_size_max_set(ev->client, -1);
   sprintf(msg_buf, "<%s> msg from <%p>", __func__, ev->client);
   log_message(LOG_FILE, "a", msg_buf);


   ack_st t = { msg };
   p = packet_compose(DAEMON_ACK, &t, sizeof(t), &size);
   if (p)
     {
        ecore_ipc_client_send(ev->client, 0,0,0,0,EINA_FALSE, p, size);
        ecore_ipc_client_flush(ev->client);
        free(p);
     }

   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
_del(void *data __UNUSED__, int type __UNUSED__, Ecore_Ipc_Event_Client_Del *ev)
{
   struct _Client *client;

   if (!ev->client)
     return ECORE_CALLBACK_RENEW;

   sprintf(msg_buf, "<%s> msg from <%p>", __func__, ev->client);
   log_message(LOG_FILE, "a", msg_buf);
   ecore_ipc_client_del(ev->client);

   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
_data(void *data __UNUSED__, int type __UNUSED__, Ecore_Ipc_Event_Client_Data *ev)
{
   void *p;
   char *msg="hello! - sent from the server";
   int size = 0;

   sprintf(msg_buf, "<%s> msg from <%p>", __func__, ev->client);
   log_message(LOG_FILE, "a", msg_buf);

   Variant_st *v = packet_info_get(ev->data, ev->size);
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

                   ack_st dt = { msg2 };
                   void *p = packet_compose(DAEMON_ACK, &dt, sizeof(dt), &size);
                   if (p)
                     {
                        ecore_ipc_client_send(ev->client, 0,0,0,0,
                              EINA_FALSE, p, size);
                        ecore_ipc_client_flush(ev->client);
                        free(p);
                     }

                   log_message(LOG_FILE, "a", "Asking tree from APP");
                }
              break;

           case GUI_ACK:
                {
                   int size = 0;
                   Eina_List *l;
                   tree_info_st *st;
                   Tree_Item *t;
                   EINA_LIST_FOREACH(trees, l, st)
                     {  /* Sending all trees */
                        t = st->data;
                        sprintf(msg_buf,"t = <%p>", t);
                        log_message(LOG_FILE, "a", msg_buf);
                        p = packet_compose(DAEMON_TREE_DATA, t,
                              sizeof(*t), &size);
                        if (p)
                          {
                             ecore_ipc_client_send(ev->client, 0,0,0,0,
                                   EINA_FALSE, p, size);
                             ecore_ipc_client_flush(ev->client);
                             free(p);
                          }

                        sprintf(msg_buf, "DAEMON <%p> sending tree, size <%d>\n",
                              st->app, size);

                        log_message(LOG_FILE, "a", msg_buf);
                     }
                }
              break;

           case APP_TREE_DATA:
                {
                   sprintf(msg_buf, "<%s> got APP TREE DATA from <%p>", __func__, ev->client);
                   log_message(LOG_FILE, "a", msg_buf);
                   trees = tree_data_add(ev->client, v->data);
                }
              break;
          }

        free(v);  /* NOT variant_free(v), then comes from eet..decode */
     }
   else
     log_message(LOG_FILE, "a", "Failed to decode data.");

   log_message(LOG_FILE, "a", "_data() finished");
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
