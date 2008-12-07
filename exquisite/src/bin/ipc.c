#include "main.h"

static int _ipc_cb_timeout(void *data);
static void _ipc_cb_theme_exit_done(void *data);

static Ecore_Timer *timeout = NULL;
static int timeout_seconds = 0;
static int fd = -1;
static Ecore_Con_Server *sock;

int quitting;

static void
parse_cmd(char *cmd)
{
   char *p, *q;
   char buf[4096];

   p = buf;
   for (q = cmd; *q && (*q != ' ') && (p < (buf + sizeof(buf) - 1)); q++)
     {
        *p = *q;
        p++;
     }
   *p = 0;
   if (*q == ' ') q++;
   
   if (timeout) 
     {
        ecore_timer_del(timeout);
        timeout = NULL;
        timeout = ecore_timer_add(timeout_seconds, _ipc_cb_timeout, NULL);
     }
   
   if (!strcmp(buf, "QUIT"))
     {
        ecore_main_loop_quit();
     }
   else if (!strcmp(buf, "PROGRESS"))
     {
	theme_progress_set(atoi(q) / 100.0);
     }
   else if (!strcmp(buf, "MSG"))
     {
        theme_message_set(q);
     }
   else if (!strcmp(buf, "TITLE"))
     {
        theme_title_set(q);
     }
   else if (!strcmp(buf, "END"))
     {
        theme_exit(_ipc_cb_theme_exit_done, NULL);
     }
   else if (!strcmp(buf, "TICK"))
     {
	theme_tick();
     }
   else if (!strcmp(buf, "PULSATE"))
     {
	theme_pulsate();
     }
   else if (!strcmp(buf, "TEXT"))
     {
        if (!(flags & EXQUISITE_FLAG_QUIET))
          theme_text_add(q);
     }
   else if (!strcmp(buf, "TEXT-URGENT"))
     {
        theme_text_add(q);
     }
   else if (!strcmp(buf, "STATUS"))
     {
        theme_status_set(q, -1);
     }
   else if (!strcmp(buf, "SUCCESS"))
     {
        theme_status_set(q, 1);
     }
   else if (!strcmp(buf, "FAILURE"))
     {
        theme_status_set(q, 0);
     }
   else if (!strcmp(buf, "CLEAR"))
     {
        theme_text_clear();
     }
   else if (!strcmp(buf, "TIMEOUT"))
     {
        timeout_seconds = atoi(q);
        if (timeout) 
          {
             ecore_timer_del(timeout);
             timeout = NULL;
          }
        timeout = ecore_timer_add(timeout_seconds, _ipc_cb_timeout, NULL);
     }
}

static int
fifo_input(void *data, Ecore_Fd_Handler *fd_handler)
{
   char buf[4096], buf2[4096], *p, *q;
   int size;
   int fd;

   fd = ecore_main_fd_handler_fd_get(fd_handler);
   if (ecore_main_fd_handler_active_get(fd_handler, ECORE_FD_READ))
     {
        for (;;)
          {
             size = read(fd, buf, sizeof(buf) - 1);
             if (size <= 0) break;
             buf[size] = 0;
             q = buf2;
             for (p = buf; *p; p++)
               {
                  if (*p == '\n')
                    {
                       *q = 0;
                       if (strlen(buf2) > 0) parse_cmd(buf2);
                       q = buf2;
                    }
                  else
                    {
                       *q = *p;
                       q++;
                    }
               }
             *q = 0;
             if (strlen(buf2) > 0) parse_cmd(buf2);
          }
     }
   return ECORE_CALLBACK_RENEW;
}

static int
socket_input(void *data, int type, void *ev)
{
   Ecore_Con_Event_Client_Data *cl_data;
   char buf2[4096], *p, *q;
   int size;

   cl_data = ev;

   size = cl_data->size;
   for (;;)
     {
        if (size <= 0) break;

        q = buf2;
        for (p = cl_data->data; *p; p++)
          {
             if (*p == '\n')
               {
                  *q = 0;
                  if (strlen(buf2) > 0) parse_cmd(buf2);
                  q = buf2;
                  size -= strlen(buf2)+1;
               }
             else
               {
                  *q = *p;
                  q++;
               }
          }
        *q = 0;
        if (strlen(buf2) > 0) parse_cmd(buf2);
        size -= strlen(buf2);
     }
   return ECORE_CALLBACK_RENEW;
}

static int
socket_del(void *data, int type, void *ev)
{
  Ecore_Con_Event_Client_Del *cl_ev;

  cl_ev = ev;

  ecore_con_client_del(cl_ev->client);
}

void
ipc_init(void)
{
   char *file = NULL;
   Ecore_Fd_Handler *fdh;
   
   file = getenv("EXQUISITE_IPC");
   if (!file || !file[0]) file = "/tmp/exquisite";

   if (!method) method = getenv("EXQUISITE_IPC_METHOD");

   if (!method || !method[0] || !strcmp(method, "fifo"))
     {
        unlink(file);
        mkfifo(file, S_IRUSR | S_IWUSR |  S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
        fd = open(file, O_RDWR);
        fcntl(fd, F_SETFL, O_NONBLOCK);
        if (fd < 0)
          {
             printf("EXQUISITE ERROR: Cannot create fifo: %s\n", file);
             exit(-1);
          }
        fdh = ecore_main_fd_handler_add(fd, ECORE_FD_READ, 
                                        fifo_input, NULL, 
                                        NULL, NULL);
     }
   else if (strstr(method, "socket"))
     {
        ecore_con_init();

        if (!strcmp(method, "socket"))
          {
             sock = ecore_con_server_add(ECORE_CON_LOCAL_SYSTEM, 
                                         file,
                                         0, NULL);
          }
        else
          {
             sock = ecore_con_server_add(ECORE_CON_LOCAL_ABSTRACT, 
                                         file,
                                         0, NULL);
          }

        if (!sock)
          {
            printf("EXQUISITE ERROR: Cannot create socket %s.\n", file);
            exit(-1);
          }

        ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DATA,
                                socket_input, NULL);
        ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DEL,
                                socket_del, NULL);
     }
   else
     {
        printf("Invalid ipc method: %s\n", method);
        exit(0);
     }
}

void
ipc_shutdown(void)
{
   char *ipc;

   if (!method || !strcmp(method, "fifo"))
     {
        ipc = getenv("EXQUISITE_IPC");
        if (!ipc) ipc = "/tmp/exquisite";
        unlink(ipc);
     }
   else if (strstr(method, "socket"))
     {
        if (sock) ecore_con_server_del(sock);
        ecore_con_shutdown();
     }
}

static int
_ipc_cb_timeout(void *data)
{
   theme_exit(_ipc_cb_theme_exit_done, data);
   return 0;
}

static void
_ipc_cb_theme_exit_done(void *data)
{
   ecore_main_loop_quit();
}
