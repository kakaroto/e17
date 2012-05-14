#include <stdio.h>
#include <unistd.h>           /*  misc. UNIX functions      */
#include <fcntl.h>
#include <Ecore.h>
#include <Ecore_Con.h>

#include "helper.h"

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
   char *str;
   EINA_LIST_FREE(info, str)
      free(str);

   EINA_LIST_FREE(trees, str)
      free(str);

   Ecore_Con_Server *svr;
   Ecore_Con_Client *cl;
   const Eina_List *clients, *l;

   clients = ecore_con_server_clients_get(svr);
   sprintf(msg_buf,"Clients connected to this server when exiting: %d\n",
         eina_list_count(clients));
   log_message(LOG_FILE, "a", msg_buf);
   EINA_LIST_FOREACH(clients, l, cl)
     {
        sprintf(msg_buf, "%s\n", ecore_con_client_ip_get(cl));
        log_message(LOG_FILE, "a", msg_buf);
        free(ecore_con_client_data_get(cl));
     }

   sprintf(msg_buf, "Server was up for %0.3f seconds\n",
         ecore_con_server_uptime_get(svr));
   log_message(LOG_FILE, "a", msg_buf);


   ecore_con_shutdown();
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
_add(void *data __UNUSED__, int type __UNUSED__, Ecore_Con_Event_Client_Add *ev)
{
   void *p;
   size_t size = compose_packet(&p, DAEMON, ACK, "hello! - sent from the server");
   Ecore_Con_Server *srv;
   Ecore_Con_Client *cl;
   const Eina_List *clients, *l;

   struct _Client *client = malloc(sizeof(*client));
   client->sdata = 0;

   sprintf(msg_buf, "Client with ip %s, port %d, connected = %d!\n",
         ecore_con_client_ip_get(ev->client),
         ecore_con_client_port_get(ev->client),
         ecore_con_client_connected_get(ev->client));
   log_message(LOG_FILE, "a", msg_buf);

   ecore_con_client_send(ev->client, p, size);
   ecore_con_client_flush(ev->client);
   free(p);

   ecore_con_client_timeout_set(ev->client, 6);

   ecore_con_client_data_set(ev->client, client);

   srv = ecore_con_client_server_get(ev->client);
   sprintf(msg_buf,"Clients connected to this server:\n");
   log_message(LOG_FILE, "a", msg_buf);

   clients = ecore_con_server_clients_get(srv);
   EINA_LIST_FOREACH(clients, l, cl)
     {
        sprintf(msg_buf, "%s\n", ecore_con_client_ip_get(cl));
        log_message(LOG_FILE, "a", msg_buf);
     }

   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
_del(void *data __UNUSED__, int type __UNUSED__, Ecore_Con_Event_Client_Del *ev)
{
   struct _Client *client;

   if (!ev->client)
     return ECORE_CALLBACK_RENEW;

   client = ecore_con_client_data_get(ev->client);

   sprintf(msg_buf, "Lost client with ip %s!\n", ecore_con_client_ip_get(ev->client));
   log_message(LOG_FILE, "a", msg_buf);

   sprintf(msg_buf, "Total data received from this client: %d\n", client->sdata);
   log_message(LOG_FILE, "a", msg_buf);
   sprintf(msg_buf,"Client was connected for %0.3f seconds.\n",
         ecore_con_client_uptime_get(ev->client));
   log_message(LOG_FILE, "a", msg_buf);

   if (client)
     free(client);

   ecore_con_client_del(ev->client);

   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
_data(void *data __UNUSED__, int type __UNUSED__, Ecore_Con_Event_Client_Data *ev)
{
   char fmt[128];
   struct _Client *client = ecore_con_client_data_get(ev->client);
   packet *pkt = ev->data;

   snprintf(fmt, sizeof(fmt),
         "Received %i bytes from client %s port %d:\n"
         ">>>>>\n"
         "%%.%is\n"
         ">>>>>\n",
         ev->size, ecore_con_client_ip_get(ev->client),
         ecore_con_client_port_get(ev->client), ev->size);

   log_message(LOG_FILE, "a", get_packet_str(ev->data));

   switch(pkt->client)
     {
      case APP:  /* Get info from app */
         if (pkt->type == TREE_DATA)
           {
              char txt[1024];
              trees = eina_list_append(trees, strdup(get_packet_str(pkt)));
              sprintf(txt, "Adding Tree (%d)", eina_list_count(trees));
              log_message(LOG_FILE, "a", txt);
              log_message(LOG_FILE, "a", get_packet_str(pkt));
           }
         break;

      case GUI:  /* Sent trees to GUI */
         if (pkt->type == TREE_DATA)
           {
              Eina_List *l;
              char *str;
              EINA_LIST_FOREACH(trees, l, str)
                {  /* Send each tree in a packet */
                   void *p;
                   log_message(LOG_FILE, "a", "Sending Tree");
                   log_message(LOG_FILE, "a", "------------");
                   log_message(LOG_FILE, "a", str);
                   log_message(LOG_FILE, "a", "------------");
                   size_t size = compose_packet(&p, DAEMON, TREE_DATA, str);
                   ecore_con_client_send(ev->client, p, size);
                   ecore_con_client_flush(ev->client);
                   free(p);
                }

           }
         break;
     }

   client->sdata += ev->size;

   return ECORE_CALLBACK_RENEW;
}
/* END   - Ecore communication callbacks */


int main(void)
{
   Ecore_Con_Server *svr;
   Ecore_Con_Client *cl;
   const Eina_List *clients, *l;

   daemonize();
   eina_init();
   ecore_init();
   ecore_con_init();

   if (!(svr = ecore_con_server_add(ECORE_CON_REMOTE_TCP, LOCALHOST, PORT, NULL)))
     exit(1);

   ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_ADD, (Ecore_Event_Handler_Cb)_add, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DEL, (Ecore_Event_Handler_Cb)_del, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DATA, (Ecore_Event_Handler_Cb)_data, NULL);

   ecore_con_server_timeout_set(svr, 10);
   ecore_con_server_client_limit_set(svr, 8, 0);

   ecore_main_loop_begin();
   _daemon_cleanup();

   return 0;
}
