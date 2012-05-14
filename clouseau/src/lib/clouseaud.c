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

static Eina_List *gui = NULL; /* List of app_info_st for gui clients */
static Eina_List *app = NULL; /* List of app_info_st for app clients */

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

static void
_daemon_cleanup(void)
{  /*  Free strings */
   app_info_st *p;
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

   EINA_LIST_FREE(gui, p)
     {
        free(p->name);
        free(p);
     }

   EINA_LIST_FREE(app, p)
     {
        free(p->name);
        free(p);
     }

   gui = app = NULL;

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
static int
_client_ptr_cmp(const void *d1, const void *d2)
{
   return (((app_info_st *) d1)->ptr - d2);
}

static Eina_List *
_add_client(Eina_List *clients, connect_st *t, void *client)
{
   if(!eina_list_search_unsorted(clients, _client_ptr_cmp, client))
     {
        app_info_st *st = malloc(sizeof(app_info_st));
        st->name = strdup(t->name);
        st->pid = t->pid;
        st->ptr = client;

        return eina_list_append(clients, st);
     }

   return clients;
}

static Eina_List *
_remove_client(Eina_List *clients, void *client)
{
   app_info_st *p = eina_list_search_unsorted(clients, _client_ptr_cmp, client);
   if (p)
     {
        free(p->name);
        free(p);
        return eina_list_remove(clients, p);
     }

   return clients;
}

Eina_Bool
_add(void *data EINA_UNUSED, int type EINA_UNUSED, Ecore_Ipc_Event_Client_Add *ev)
{
   void *p;
   int size = 0;

   ecore_ipc_client_data_size_max_set(ev->client, -1);
   sprintf(msg_buf, "<%s> msg from <%p>", __func__, ev->client);
   log_message(LOG_FILE, "a", msg_buf);

   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
_del(void *data EINA_UNUSED, int type EINA_UNUSED, Ecore_Ipc_Event_Client_Del *ev)
{
   struct _Client *client;

   if (!ev->client)
     return ECORE_CALLBACK_RENEW;

   sprintf(msg_buf, "<%s> msg from <%p>", __func__, ev->client);
   log_message(LOG_FILE, "a", msg_buf);

   /* Now we need to find if its an APP or GUI client */
   app_info_st *i = eina_list_search_unsorted(gui, _client_ptr_cmp, ev->client);
   if (i)  /* Only need to remove GUI client from list */
     gui = _remove_client(gui, ev->client);

   i = eina_list_search_unsorted(app, _client_ptr_cmp, ev->client);
   if (i)
     {  /* Notify all GUI clients to remove this APP */
        app_closed_st t = { ev->client };
        Eina_List *l;
        int size;
        void *p = packet_compose(APP_CLOSED, &t, sizeof(t), &size);
        if (p)
          {
             EINA_LIST_FOREACH(gui, l, i)
               {
                  ecore_ipc_client_send(i->ptr,
                        0,0,0,0,EINA_FALSE, p, size);
                  ecore_ipc_client_flush(i->ptr);
               }

             free(p);
          }

        app = _remove_client(app, ev->client);
     }

   ecore_ipc_client_del(ev->client);

   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
_data(void *data EINA_UNUSED, int type EINA_UNUSED, Ecore_Ipc_Event_Client_Data *ev)
{
   void *p;
   int size = 0;

   sprintf(msg_buf, "<%s> msg from <%p>", __func__, ev->client);
   log_message(LOG_FILE, "a", msg_buf);

   Variant_st *v = packet_info_get(ev->data, ev->size);
   if (v)
     {
        switch(packet_mapping_type_get(v->t.type))
          {
           case APP_CLIENT_CONNECT:
                {  /* Register APP then notify GUI about it */
                   app_info_st *st;
                   Eina_List *l;
                   connect_st *t = v->data;
                   app_info_st m = { t->pid, (char *) t->name, ev->client};
                   app = _add_client(app, t, ev->client);
                   p = packet_compose(APP_ADD, &m, sizeof(m), &size);
                   if (p)
                     {
                        EINA_LIST_FOREACH(gui, l, st)
                          {  /* Notify all GUI clients to add APP */
                             ecore_ipc_client_send(st->ptr,
                                   0,0,0,0,EINA_FALSE, p, size);
                             ecore_ipc_client_flush(st->ptr);
                          }

                        free(p);
                     }
                }
              break;

           case GUI_CLIENT_CONNECT:
                {  /* Register GUI, then notify about all APP */
                   app_info_st *st;
                   Eina_List *l;
                   connect_st *t = v->data;
                   gui = _add_client(gui, t, ev->client);
                   EINA_LIST_FOREACH(app, l, st)
                     {  /* Add all registered apps to newly open GUI */
                        p = packet_compose(APP_ADD, st, sizeof(*st), &size);
                        if (p)
                          {
                             ecore_ipc_client_send(ev->client,
                                   0,0,0,0,EINA_FALSE, p, size);
                             ecore_ipc_client_flush(ev->client);
                             free(p);
                          }
                     }

                }
              break;

           case DATA_REQ:
                {  /* msg coming from GUI, FWD this to app */
                   data_req_st *req = v->data;
                   if (req->app)
                     {  /* Requesting specific app data */
                        if(eina_list_search_unsorted(app,
                                 _client_ptr_cmp, req->app))
                          {  /* Do the req only of APP connected to daemon */
                             data_req_st t = { ev->client, req->app };
                             p = packet_compose(DATA_REQ,
                                   &t, sizeof(t), &size);
                             if (p)
                               {
                                  ecore_ipc_client_send(req->app, 0,0,0,0,
                                        EINA_FALSE, p, size);
                                  ecore_ipc_client_flush(req->app);
                                  free(p);
                               }
                          }
                     }
                   else
                     {  /* requesting ALL apps data */
                        Eina_List *l;
                        app_info_st *st;
                        data_req_st t = { ev->client, NULL };
                        EINA_LIST_FOREACH(app, l, st)
                          {
                             t.app = st->ptr;
                             p = packet_compose(DATA_REQ,
                                   &t, sizeof(t), &size);
                             if (p)
                               {
                                  ecore_ipc_client_send(st->ptr, 0,0,0,0,
                                        EINA_FALSE, p, size);
                                  ecore_ipc_client_flush(st->ptr);
                                  free(p);
                               }
                          }
                     }
                }
              break;

           case TREE_DATA:
                {
                   tree_data_st *td = v->data;
                   if (td->gui)
                     {  /* Sending tree data to specific GUI client */
                        if(eina_list_search_unsorted(gui,
                                 _client_ptr_cmp, td->gui))
                          {  /* Do the req only of GUI connected to daemon */
                             ecore_ipc_client_send(td->gui, 0,0,0,0,
                                   EINA_FALSE, ev->data, ev->size);
                             ecore_ipc_client_flush(td->gui);
                          }
                     }
                   else
                     {  /* Sending tree data to all GUI clients */
                        Eina_List *l;
                        app_info_st *p;
                        EINA_LIST_FOREACH(gui, l, p)
                          {
                             ecore_ipc_client_send(p->ptr, 0,0,0,0,
                                   EINA_FALSE, ev->data, ev->size);
                             ecore_ipc_client_flush(p->ptr);
                          }
                     }
                }
              break;

           case HIGHLIGHT:
                {  /* FWD this message to app */
                   highlight_st *ht = v->data;
                   if(eina_list_search_unsorted(app,
                            _client_ptr_cmp, ht->app))
                     {  /* Do the req only of APP connected to daemon */
                        ecore_ipc_client_send(ht->app, 0,0,0,0,
                              EINA_FALSE, ev->data, ev->size);
                        ecore_ipc_client_flush(ht->app);
                     }
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
