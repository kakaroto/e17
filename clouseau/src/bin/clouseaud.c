#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <fcntl.h>
#include <Ecore_Ipc.h>

#include "clouseau_private.h"

#define RUNNING_DIR  "/tmp"
#define LOCK_FILE ".clouseaud.lock"
#define LOG_FILE  "clouseaud.log"

static Eina_List *gui = NULL; /* List of app_info_st for gui clients */
static Eina_List *app = NULL; /* List of app_info_st for app clients */
Ecore_Ipc_Server *ipc_svr = NULL;

/* For Debug */
char msg_buf[MAX_LINE+1];

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

static void
_daemon_cleanup(void)
{  /*  Free strings */
   app_info_st *p;

   sprintf(msg_buf,"Clients connected to this server when exiting: %d\n",
         eina_list_count(ecore_ipc_server_clients_get(ipc_svr)));
   log_message(LOG_FILE, "a", msg_buf);

   EINA_LIST_FREE(gui, p)
     {
        if(p->file)
          free(p->file);

        free(p->name);
        free(p);
     }

   EINA_LIST_FREE(app, p)
     {
        if(p->file)
          free(p->file);

        free(p->name);
        free(p);
     }

   gui = app = NULL;
   ipc_svr = NULL;

   clouseau_data_shutdown();
   ecore_ipc_shutdown();
   ecore_shutdown();
   eina_shutdown();
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

   log_message(LOG_FILE, "w", "Daemon Started");
   log_message(LOG_FILE, "a", ctime(&currentTime));
}

/* START - Ecore communication callbacks */
static int
_client_ptr_cmp(const void *d1, const void *d2)
{
   return (((app_info_st *) d1)->ptr - ((unsigned long long) (uintptr_t) d2));
}

static Eina_List *
_add_client(Eina_List *clients, connect_st *t, void *client)
{
   if(!eina_list_search_unsorted(clients, _client_ptr_cmp, client))
     {
        app_info_st *st = calloc(1, sizeof(app_info_st));
        st->name = strdup(t->name);
        st->pid = t->pid;
        st->ptr = (unsigned long long) (uintptr_t) client;

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
   ecore_ipc_client_data_size_max_set(ev->client, -1);
   sprintf(msg_buf, "<%s> msg from <%p>", __func__, ev->client);
   log_message(LOG_FILE, "a", msg_buf);

   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
_del(void *data EINA_UNUSED, int type EINA_UNUSED, Ecore_Ipc_Event_Client_Del *ev)
{
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
        app_closed_st t = { (unsigned long long) (uintptr_t) ev->client };
        Eina_List *l;
        int size;
        void *p = clouseau_data_packet_compose(CLOUSEAU_APP_CLOSED,
              &t, sizeof(t), &size, NULL, 0);

        if (p)
          {
             EINA_LIST_FOREACH(gui, l, i)
               {
                  ecore_ipc_client_send((void *) (uintptr_t) i->ptr,
                        0,0,0,0,EINA_FALSE, p, size);
                  ecore_ipc_client_flush((void *) (uintptr_t) i->ptr);
               }

             free(p);
          }

        app = _remove_client(app, ev->client);
     }

   ecore_ipc_client_del(ev->client);

   if (!eina_list_count(ecore_ipc_server_clients_get(ipc_svr)))
     {  /* Trigger cleanup and exit when all clients disconneced */
        ecore_main_loop_quit();
     }

   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
_data(void *data EINA_UNUSED, int type EINA_UNUSED, Ecore_Ipc_Event_Client_Data *ev)
{
   void *p;
   int size = 0;

   sprintf(msg_buf, "<%s> msg from <%p>", __func__, ev->client);
   log_message(LOG_FILE, "a", msg_buf);

   Variant_st *v =  clouseau_data_packet_info_get(ev->data, ev->size);
   /* This is where daemon impl communication protocol.
    * In order to simplify, all messages also contains recipient ptr
    * as saved by daemon.
    * Thus we only need to peek this info then FWD, reply to this recipient */
   if (!v)
     {
        log_message(LOG_FILE, "a", "Failed to decode data.");
        return ECORE_CALLBACK_RENEW;
     }

   switch(clouseau_data_packet_mapping_type_get(v->type))
     {
      case CLOUSEAU_APP_CLIENT_CONNECT:
        {  /* Register APP then notify GUI about it */
           app_info_st *st;
           Eina_List *l;
           connect_st *t = v->data;
           app_info_st m = { t->pid, (char *) t->name, NULL,
                             (unsigned long long) (uintptr_t) ev->client, NULL, 0 };

           app = _add_client(app, t, ev->client);
           p = clouseau_data_packet_compose(CLOUSEAU_APP_ADD,
                 &m, sizeof(m), &size, NULL, 0);

           if (p)
             {
                EINA_LIST_FOREACH(gui, l, st)
                  {  /* Notify all GUI clients to add APP */
                     ecore_ipc_client_send(
                                           (void *) (uintptr_t) st->ptr,
                                           0,0,0,0,EINA_FALSE, p, size);
                     ecore_ipc_client_flush(
                                            (void *) (uintptr_t) st->ptr);
                  }

                free(p);
             }
        }
        break;

      case CLOUSEAU_GUI_CLIENT_CONNECT:
        {  /* Register GUI, then notify about all APP */
           app_info_st *st;
           Eina_List *l;
           connect_st *t = v->data;
           gui = _add_client(gui, t, ev->client);
           EINA_LIST_FOREACH(app, l, st)
             {  /* Add all registered apps to newly open GUI */
                p = clouseau_data_packet_compose(CLOUSEAU_APP_ADD,
                      st, sizeof(*st), &size, NULL, 0);

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

      case CLOUSEAU_DATA_REQ:
        {  /* msg coming from GUI, FWD this to app specified in req */
           data_req_st *req = v->data;
           if (req->app)
             {  /* Requesting specific app data */
                if(eina_list_search_unsorted(app,
                                             _client_ptr_cmp,
                                             (void *) (uintptr_t) req->app))
                  {  /* Do the req only of APP connected to daemon */
                     data_req_st t = {
                       (unsigned long long) (uintptr_t) ev->client,
                       (unsigned long long) (uintptr_t) req->app };

                     p = clouseau_data_packet_compose(CLOUSEAU_DATA_REQ,
                                        &t, sizeof(t), &size, NULL, 0);

                     if (p)
                       {
                          ecore_ipc_client_send(
                                                (void *) (uintptr_t) req->app,
                                                0,0,0,0, EINA_FALSE, p, size);
                          ecore_ipc_client_flush(
                                                 (void *) (uintptr_t) req->app);
                          free(p);
                       }
                  }
             }
           else
             {  /* requesting ALL apps data */
                Eina_List *l;
                app_info_st *st;
                data_req_st t = {
                  (unsigned long long) (uintptr_t) ev->client,
                  (unsigned long long) (uintptr_t) NULL };

                EINA_LIST_FOREACH(app, l, st)
                  {
                     t.app = (unsigned long long) (uintptr_t) st->ptr;
                     p = clouseau_data_packet_compose(CLOUSEAU_DATA_REQ,
                                        &t, sizeof(t), &size, NULL, 0);

                     if (p)
                       {
                          ecore_ipc_client_send(
                                                (void *) (uintptr_t) st->ptr,
                                                0,0,0,0, EINA_FALSE, p, size);
                          ecore_ipc_client_flush(
                                                 (void *) (uintptr_t) st->ptr);
                          free(p);
                       }
                  }
             }
        }
        break;

      case CLOUSEAU_TREE_DATA:
        {  /* Tree Data comes from APP, GUI client specified in msg */
           tree_data_st *td = v->data;
           if (td->gui)
             {  /* Sending tree data to specific GUI client */
                if(eina_list_search_unsorted(gui,
                                             _client_ptr_cmp,
                                             (void *) (uintptr_t) td->gui))
                  {  /* Do the req only of GUI connected to daemon */
                     ecore_ipc_client_send(
                                           (void *) (uintptr_t) td->gui, 0,0,0,0,
                                           EINA_FALSE, ev->data, ev->size);
                     ecore_ipc_client_flush(
                                            (void *) (uintptr_t) td->gui);
                  }
             }
           else
             {  /* Sending tree data to all GUI clients */
                Eina_List *l;
                app_info_st *info;
                EINA_LIST_FOREACH(gui, l, info)
                  {
                     ecore_ipc_client_send(
                                           (void *) (uintptr_t) info->ptr, 0,0,0,0,
                                           EINA_FALSE, ev->data, ev->size);
                     ecore_ipc_client_flush(
                                            (void *) (uintptr_t) info->ptr);
                  }
             }

           clouseau_data_tree_free(td->tree);
        }
        break;

      case CLOUSEAU_HIGHLIGHT:
        {  /* FWD this message to app */
           highlight_st *ht = v->data;
           if(eina_list_search_unsorted(app,
                                        _client_ptr_cmp, (void *) (uintptr_t) ht->app))
             {  /* Do the req only of APP connected to daemon */
                ecore_ipc_client_send((void *)
                                      (uintptr_t) ht->app, 0,0,0,0,
                                      EINA_FALSE, ev->data, ev->size);
                ecore_ipc_client_flush((void *) (uintptr_t) ht->app);
             }
        }
        break;

      case CLOUSEAU_BMP_REQ:
        {
           bmp_req_st *req = v->data;
           if(eina_list_search_unsorted(app,
                                        _client_ptr_cmp, (void *) (uintptr_t) req->app))
             {  /* Do the req only of APP connected to daemon */
                bmp_req_st t = {
                  (unsigned long long) (uintptr_t) ev->client,
                  req->app, req->object, req->ctr };

                p = clouseau_data_packet_compose(CLOUSEAU_BMP_REQ,
                                   &t, sizeof(t), &size, NULL, 0);

                if (p)
                  {  /* FWD req to app with client data */
                     ecore_ipc_client_send(
                                           (void *) (uintptr_t) req->app,
                                           0,0,0,0, EINA_FALSE, p, size);
                     ecore_ipc_client_flush(
                                            (void *) (uintptr_t) req->app);
                     free(p);
                  }
             }
        }
        break;

      case CLOUSEAU_BMP_DATA:
        {  /* Bmp Data comes from APP, GUI client specified in msg */
           bmp_info_st *st = v->data;
           if (st->gui)
             {  /* Sending BMP data to specific GUI client */
                if(eina_list_search_unsorted(gui,
                                             _client_ptr_cmp,
                                             (void *) (uintptr_t) st->gui))
                  {  /* Do the req only of GUI connected to daemon */
                     ecore_ipc_client_send(
                                           (void *) (uintptr_t) st->gui, 0,0,0,0,
                                           EINA_FALSE, ev->data, ev->size);
                     ecore_ipc_client_flush(
                                            (void *) (uintptr_t) st->gui);
                  }
             }
           else
             {  /* Sending BMP data to all GUI clients */
                Eina_List *l;
                app_info_st *info;
                EINA_LIST_FOREACH(gui, l, info)
                  {
                             ecore_ipc_client_send(
                                                   (void *) (uintptr_t) info->ptr, 0,0,0,0,
                                                   EINA_FALSE, ev->data, ev->size);
                             ecore_ipc_client_flush(
                                                    (void *) (uintptr_t) info->ptr);
                  }
             }

           if (st->bmp)
             free(st->bmp);
        }
        break;

      default:
         break;
     }

   clouseau_data_variant_free(v);

   log_message(LOG_FILE, "a", "_data() finished");
   return ECORE_CALLBACK_RENEW;
}
/* END   - Ecore communication callbacks */

int main(void)
{
   daemonize();
   eina_init();
   ecore_init();
   ecore_ipc_init();
   clouseau_data_init();

   if (!(ipc_svr = ecore_ipc_server_add(ECORE_IPC_REMOTE_SYSTEM,
               LISTEN_IP, PORT, NULL)))
     exit(1);

   ecore_ipc_server_data_size_max_set(ipc_svr, -1);

   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD, (Ecore_Event_Handler_Cb)_add, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL, (Ecore_Event_Handler_Cb)_del, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, (Ecore_Event_Handler_Cb)_data, NULL);

   ecore_main_loop_begin();
   _daemon_cleanup();

   return 0;
}
