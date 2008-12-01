#include "main.h"

static int _ipc_cb_timeout(void *data);
static void _ipc_cb_theme_exit_done(void *data);

static Ecore_Timer *timeout = NULL;
static int timeout_seconds = 0;
static int fd = -1;

int quitting;

static void
fifo_cmd(char *cmd)
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
        if (!quiet)
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
   int fd;
   char buf[4096], buf2[4096], *p, *q;
   int size;
   
   if (ecore_main_fd_handler_active_get(fd_handler, ECORE_FD_READ))
     {
        fd = ecore_main_fd_handler_fd_get(fd_handler);
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
                       if (strlen(buf2) > 0) fifo_cmd(buf2);
                       q = buf2;
                    }
                  else
                    {
                       *q = *p;
                       q++;
                    }
               }
             *q = 0;
             if (strlen(buf2) > 0) fifo_cmd(buf2);
          }
     }
   return 1;
}

void
ipc_init(void)
{
   char *fifo;

   fifo = getenv("EXQUISITE_IPC");
   if (!fifo) fifo = "/tmp/exquisite-fifo";
   mkfifo(fifo, S_IRUSR | S_IWUSR |  S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
   fd = open(fifo, O_RDONLY);
   fcntl(fd, F_SETFL, O_NONBLOCK);
   if (fd < 0)
     {
        printf("EXQUISITE ERROR: cannot open fifo: %s\n", fifo);
        exit(-1);
     }
   ecore_main_fd_handler_add(fd, ECORE_FD_READ, fifo_input, NULL, NULL, NULL);
}

void
ipc_shutdown(void)
{
   char *fifo;

   fifo = getenv("EXQUISITE_IPC");
   if (!fifo) fifo = "/tmp/exquisite-fifo";
   unlink(fifo);
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
