/**********************************************************************
 * E-Pinger                                               December 2000
 * Horms                                             horms@vergenet.net
 * 
 * E-Pinger
 * Enlightenment epplet to monitor hosts using Ping
 * Copyright (C) 2000  Horms
 *  
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *   
 * The above copyright notice and this permission notice shall be
 * included in all copies of the Software, its documentation and
 * marketing & publicity materials, and acknowledgment shall be given
 * in the documentation, materials and software packages that this
 * Software was used.
 *    
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 **********************************************************************/

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

#include "epplet.h"

#define BUF_LEN 64
#define LOG_LEN 7
#define CMD_LEN 256
#define LABEL_CHAR 24

static Epplet_gadget b_close, b_configure, b_help, p_log, pb_log, pb_log_small;
static Epplet_gadget tb_host, tb_pause;
static Window       win, config_win;
static int          log_entries, nchild;
static char         buf[BUF_LEN];
static char        *result, *new_result;
static char        *host;
static int          pinger_pause = 30;
static char         cmd[CMD_LEN];
static int          pinger_when;
static int          new_ping;

#define CMD_BASE "{ HOST=\"%s\"; if ping -c 10 $HOST | fgrep -q time= ; then echo online: $HOST; else echo offline: $HOST; fi ; }"
#define DEFAULT_PAUSE_STR "10"

/*
 * #define CMD_BASE "{ HOST=\"%s\"; echo Stix >& 2 ; if ping -c 10 $HOST | fgrep -q time= ; then echo $HOST online; else echo $HOST offline; fi ; } 2>/dev/null"
 * #define DEFAULT_PAUSE_STR "60"
*/

static void         cb_timer(void *data);
static void         cb_close(void *data);
static void         cb_in(void *data, Window w);
static void         cb_out(void *data, Window w);
static void         cb_help(void *data);
static int          bg_system(char *command);
static int          nb_read(int fd, char *buffer, int n);
static void         start_ping(void);
void                ping_reaper(void);

static void
start_ping(void)
{
   int                 now;

   now = time(NULL);

   if (pinger_when == 0 || pinger_when <= now)
     {
	pinger_when = now + pinger_pause;
	bg_system(cmd);
     }
}

/**********************************************************************
 * pinger_reaper
 * A signal handler that waits for SIGCHLD and runs wait3 to free
 * the resources of any exited children. This stops zombie processes
 * from hanging around.
 * pre: SIGCHLD is recieved by the process
 * post: Resoerces of any exited children are freed
 *       Signal Handler for SIGCHLD reset
 **********************************************************************/

void
pinger_reaper(void)
{
   int                 status;

   signal(SIGCHLD, (void (*)(int))pinger_reaper);
   while (wait3(&status, WNOHANG, 0) > 0)
     {
	nchild--;
     }
}

/* Run a command as a child process, that is in the "background".
 * Modified from man system(2) */

static int
bg_system(char *command)
{
   int                 pid;

   extern char       **environ;
   extern int          errno;

   /* Set reaper so we don't get zombies */
   signal(SIGCHLD, (void (*)(int))pinger_reaper);

   if (command == NULL || *command == '\0')
     {
	return 1;
     }
   pid = fork();
   if (pid == -1)
     {
	return -1;
     }
   if (pid == 0)
     {
	char               *argv[4];

	argv[0] = "sh";
	argv[1] = "-c";
	argv[2] = command;
	argv[3] = NULL;
	execve("/bin/sh", argv, environ);
	exit(127);
     }

   nchild++;
   return (0);
}

/* Non-blocking read */
static int
nb_read(int fd, char *buffer, int n)
{
   int                 bytes_read;
   int                 status;
   size_t              offset;
   fd_set              except_template;
   fd_set              read_template;
   struct timeval      timeout;

   offset = 0;

   while (offset < n)
     {
	FD_ZERO(&read_template);
	FD_SET(fd, &read_template);
	FD_ZERO(&except_template);
	FD_SET(fd, &except_template);
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	status =
	   select(FD_SETSIZE, &read_template, NULL, &except_template, &timeout);
	if (status < 0)
	  {
	     if (errno != EINTR)
	       {
		  perror("nb_read: select");
		  return (-1);
	       }
	     continue;		/* Ignore EINTR */
	  }
	else if (FD_ISSET(fd, &except_template))
	  {
	     fprintf(stderr, "read_token: error on file descriptor");
	     return (-1);
	  }
	else if (status == 0)
	  {			/* No data to read */
	     return (offset);
	  }

	/*If we get this far fd must be ready for reading */
	if ((bytes_read = read(fd, buffer + offset, n - offset)) < 0)
	  {
	     perror("read_token: error reading input");
	     return (-1);
	  }
	if (bytes_read == 0)
	  {
	     return (offset);
	  }

	offset += bytes_read;
     }

   return (offset);
}

static void
cb_close(void *data)
{
   Epplet_unremember();
   Esync();
   data = NULL;
   exit(0);
}

static void
cb_in(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_hide(pb_log);
	Epplet_gadget_show(pb_log_small);
	Epplet_gadget_show(b_close);
	Epplet_gadget_show(b_help);
	Epplet_gadget_show(b_configure);
     }
   return;
   data = NULL;
}

static void
cb_out(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_hide(b_close);
	Epplet_gadget_hide(b_help);
	Epplet_gadget_hide(b_configure);
	Epplet_gadget_hide(pb_log_small);
	Epplet_gadget_show(pb_log);
     }
   return;
   data = NULL;
}

static void
add_log(char *button_string, char *entry_string)
{
   char               *tmp;

   if (button_string != NULL)
     {
	tmp = strdup(button_string);
	if (strlen(tmp) > LABEL_CHAR)
	  {
	     *(tmp + LABEL_CHAR) = '\0';
	     *(tmp + LABEL_CHAR - 1) = '.';
	     *(tmp + LABEL_CHAR - 2) = '.';
	     *(tmp + LABEL_CHAR - 3) = '.';
	  }
	Epplet_change_popbutton_label(pb_log, tmp);
	Epplet_change_popbutton_label(pb_log_small, tmp);
	free(tmp);
     }
   if (entry_string != NULL)
     {
	Epplet_add_popup_entry(p_log, entry_string, NULL, NULL, NULL);
	if (log_entries >= LOG_LEN)
	  {
	     Epplet_remove_popup_entry(p_log, 0);
	  }
	else
	  {
	     log_entries++;
	  }
     }
   Epplet_redraw();
}

static void
cb_help(void *data)
{
   Epplet_show_about("E-Pinger");
   return;
   data = NULL;
}

static int
set_host(char *new_host)
{
   char               *c_p, *last_p;

   if (new_host == NULL || *new_host == '\0')
     {
	return (0);
     }

   if (host != NULL)
     {
	free(host);
     }

   for (last_p = new_host + strlen(new_host), c_p = new_host; c_p < last_p;
	c_p++)
     {
	if (!(isalnum(*c_p) || *c_p == '.' || *c_p == '-'))
	  {
	     return (-1);
	  }
     }

   if ((host = strdup(new_host)) == NULL)
     {
	perror("set_host: strdup");
	return (-1);
     }

   snprintf(cmd, CMD_LEN, CMD_BASE, host);
   new_ping = 1;
   add_log("...", NULL);

   return (0);
}

static int
set_pause(char *new_pause)
{
   char               *c_p;
   char               *last_p;

   if (new_pause == NULL || *new_pause == '\0')
     {
	return (0);
     }

   for (last_p = new_pause + strlen(new_pause), c_p = new_pause; c_p < last_p;
	c_p++)
     {
	if (!(isdigit(*c_p)))
	  {
	     return (-1);
	  }
     }

   pinger_pause = atoi(new_pause);
   pinger_when = 0;

   return (0);
}

static void
cb_ok(void *data)
{
   char                tmp_buf[BUF_LEN];

   host = NULL;
   if (set_host(Epplet_textbox_contents(tb_host)) < 0)
     {
	Epplet_change_textbox(tb_host, "*ERROR*");
	Epplet_redraw();
	usleep(100000);
	Epplet_change_textbox(tb_host, host);
	Epplet_redraw();
	return;
     }

   if (set_pause(Epplet_textbox_contents(tb_pause)) < 0)
     {
	Epplet_change_textbox(tb_pause, "*ERROR*");
	Epplet_redraw();
	usleep(100000);
	snprintf(tmp_buf, BUF_LEN, "%d", pinger_pause);
	Epplet_change_textbox(tb_pause, tmp_buf);
	Epplet_redraw();
	return;
     }

   Epplet_window_destroy(config_win);
   config_win = None;

   Epplet_modify_config("pause", tmp_buf);
   Epplet_modify_config("host", host);
   Epplet_save_config();

   /* Reset timer to check host */
   Epplet_remove_timer("EXEC_TIMER");

   return;
   data = NULL;
}

static void
cb_cancel(void *data)
{
   Epplet_window_destroy(config_win);
   config_win = None;
   return;
   data = NULL;
}

static void
cb_configure(void *data)
{
   char                tmp_buf[BUF_LEN];

   if (config_win)
      return;

   config_win =
      Epplet_create_window_config(100, 52, "E-Pinger Configuration", cb_ok,
				  NULL, NULL, NULL, cb_cancel, NULL);

   snprintf(tmp_buf, BUF_LEN, "%d", pinger_pause);
   Epplet_gadget_show(Epplet_create_label(2, 18, "Pause:", 2));
   Epplet_gadget_show(tb_pause =
		      Epplet_create_textbox(NULL, tmp_buf, 40, 16, 40, 14, 2,
					    NULL, NULL));

   Epplet_gadget_show(Epplet_create_label(2, 2, "Host:", 2));
   Epplet_gadget_show(tb_host =
		      Epplet_create_textbox(NULL, host, 40, 2, 140, 14, 2, NULL,
					    NULL));

   Epplet_window_show(config_win);
   Epplet_window_pop_context();

   return;
   data = NULL;
}

static void
cb_timer(void *data)
{
   time_t              now;
   int                 nbyte;
   char               *s;

   if ((nbyte = nb_read(0, new_result, BUF_LEN)) > 0)
     {
	new_result[nbyte - 1] = '\0';
     }
   else
     {
	if (!nchild)
	  {
	     start_ping();
	  }
	Epplet_timer(cb_timer, NULL, 0.5, "TIMER");
	return;
     }

   if (strncmp(new_result, result, BUF_LEN))
     {
	s = result;
	result = new_result;
	new_result = s;

	now = time(NULL);
	strncpy(buf, result, BUF_LEN - 1);
	strftime(buf + nbyte + 1, BUF_LEN - nbyte - 1, "%H:%M %a %e %b %G",
		 localtime(&now));
	*(buf + nbyte - 1) = ':';
	*(buf + nbyte) = ' ';
	add_log(result, buf);
     }
   else if (new_ping)
     {
	add_log(result, NULL);
	new_ping = 0;
     }

   start_ping();
   Epplet_timer(cb_timer, NULL, 0.5, "TIMER");

   return;
   data = NULL;
}

int
main(int argc, char **argv)
{
   int                 filedes[2];

   atexit(Epplet_cleanup);

   Epplet_Init("E-Pinger", "0.1", "Enlightenment Pinger Epplet",
	       6, 1, argc, argv, 0);
   Epplet_timer(cb_timer, NULL, 0.05, "TIMER");

   /* Set reaper so we don't get zombies */
   signal(SIGCHLD, (void (*)(int))pinger_reaper);

   log_entries = 0;

   b_close = Epplet_create_button(NULL, NULL,
				  2, 2, 0, 0, "CLOSE", 0, NULL, cb_close, NULL);
   b_configure = Epplet_create_button(NULL, NULL,
				      68, 2, 0, 0, "CONFIGURE", win, NULL,
				      cb_configure, NULL);
   b_help = Epplet_create_button(NULL, NULL,
				 82, 2, 0, 0, "HELP", win, NULL, cb_help, NULL);

   if ((result = (char *)malloc(BUF_LEN)) == NULL)
     {
	perror("main: malloc result");
	return (-1);
     }
   *result = '\0';
   if ((new_result = (char *)malloc(BUF_LEN)) == NULL)
     {
	perror("main: malloc new_result");
	return (-1);
     }
   *new_result = '\0';

   p_log = Epplet_create_popup();

   pb_log = Epplet_create_popupbutton("Flim", NULL, 0, 0, 96, 16, NULL, p_log);
   pb_log_small = Epplet_create_popupbutton("Flim", NULL, 15, 0, 52, 16,
					    NULL, p_log);
   Epplet_gadget_show(pb_log);

   Epplet_register_focus_in_handler(cb_in, NULL);
   Epplet_register_focus_out_handler(cb_out, NULL);
   win = Epplet_get_main_window();

   Epplet_load_config();
   set_host(Epplet_query_config("host"));
   set_pause(Epplet_query_config_def("pause", DEFAULT_PAUSE_STR));

   /* Nasty. Assume that pipe will use fd 0 and 1, so 
    * we can read the ouput of child commands writing to
    * strdout by reading 0.
    */
   close(0);
   close(1);
   if (pipe(filedes) < 0)
     {
	perror("main: pipe");
	return (-1);
     }
   pinger_when = 0;
   start_ping();

   Epplet_show();
   Epplet_Loop();
   return 0;
}
