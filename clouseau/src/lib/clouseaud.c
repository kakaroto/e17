/*
 * UNIX Daemon Server Programming Sample Program
 * Levent Karakas <levent at mektup dot at> May 2001
 *
 * compile: gcc clouseaud.c -o clouseaud `pkg-config elementary --cflags --libs`
 * To run:     ./clouseaud
 * To test daemon:   ps -ef|grep clouseaud (or ps -aux on BSD systems)
 * To test log:   tail -f /tmp/clouseaud.log
 * To test signal:   kill -HUP `cat /tmp/clouseaud.lock`
 * To terminate:  kill `cat /tmp/clouseaud.lock`
 * */
#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */
#include <fcntl.h>

#include "helper.h"           /*  our own helper functions  */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Elementary.h>

#define RUNNING_DIR  "/tmp"
#define LOCK_FILE "clouseaud.lock"
#define LOG_FILE  "clouseaud.log"

static Eina_List *info = NULL; /* Holds app info to be send to GUI client */

static void
_daemon_cleanup(void)
{  /*  Free strings */
   char *name;
   EINA_LIST_FREE(info, name)
      free(name);
}

void log_message(char *filename, char *mode, char *message)
{
   FILE *logfile;
   logfile=fopen(filename, mode);
   if(!logfile) return;
   fprintf(logfile,"%s\n",message);
   fclose(logfile);
}

void signal_handler(int sig)
{
   switch(sig) {
      case SIGHUP:
         log_message(LOG_FILE, "a", "hangup signal catched");
         break;
      case SIGTERM:
         _daemon_cleanup();
         log_message(LOG_FILE, "a", "terminate signal catched");
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

#ifndef ELM_LIB_QUICKLAUNCH
EAPI int
elm_main(int argc, char **argv)
{
   int       list_s;                /*  listening socket          */
   int       conn_s;                /*  connection socket         */
   short int port;                  /*  port number               */
   struct    sockaddr_in servaddr;  /*  socket address structure  */
   char      buffer[MAX_LINE];      /*  character buffer          */
   char     *endptr;                /*  for strtol()              */

   daemonize();
   eina_init();
   eet_init();

   /* Setting up communication */
   port = ECHO_PORT;
   /*  Create the listening socket  */
   if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
     {
        log_message(LOG_FILE, "a", "ECHOSERV: Error creating listening socket.\n");
        exit(EXIT_FAILURE);
     }

   /*  Set all bytes in socket address structure to
       zero, and fill in the relevant data members   */
   memset(&servaddr, 0, sizeof(servaddr));
   servaddr.sin_family      = AF_INET;
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
   servaddr.sin_port        = htons(port);


   if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 )
     { /* Bind socket addresss to the listening socket, and call listen()  */
        log_message(LOG_FILE, "a",  "ECHOSERV: Error calling bind()\n");
        exit(EXIT_FAILURE);
     }

   if ( listen(list_s, LISTENQ) < 0 )
     {
        log_message(LOG_FILE, "a",  "ECHOSERV: Error calling listen()\n");
        exit(EXIT_FAILURE);
     }

   /*  Enter an infinite loop to respond
       to client requests and echo input  */

   while ( 1 ) {
        if ( (conn_s = accept(list_s, NULL, NULL) ) < 0 )
          { /*  Wait for a connection, then accept() it  */
             log_message(LOG_FILE, "a",  "ECHOSERV: Error calling accept()\n");
             exit(EXIT_FAILURE);
          }


        /*  Retrieve an input line from the connected socket
            then simply write it back to the same socket.     */
        while(Readline(conn_s, buffer, MAX_LINE-1))
          {
             if (!strncmp(buffer, END_OF_MESSAGE, strlen(END_OF_MESSAGE)))
               break;

             if (!strncmp(buffer, "server", strlen("server")))
               {
                  info = eina_list_append(info, strdup(buffer));
                  log_message(LOG_FILE, "a", buffer);
                  Writeline(conn_s, "Got info\n", strlen("Got info\n"));
               }
             else if (!strncmp(buffer, "client", strlen("client")))
               {
                  Eina_List *l;
                  char *line;
                  EINA_LIST_FOREACH(info, l, line)
                    {
                       Writeline(conn_s, line, strlen(line));
                    }

                  break;
               }
          }

        Writeline(conn_s, "OK\n", strlen("OK\n"));
        log_message(LOG_FILE, "a", "DONE!!!!");
        if ( close(conn_s) < 0 )
          { /*  Close the connected socket  */
             log_message(LOG_FILE, "a",  "ECHOSERV: Error calling close()\n");
             exit(EXIT_FAILURE);
          }
   }  /* End of while (1) */
}
#endif
ELM_MAIN()
