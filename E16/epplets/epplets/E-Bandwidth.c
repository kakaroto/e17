/*
 * Copyright (C) 1999-2000, Michael Jennings
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include "epplet.h"
#include "net.h"

#if 0
#  define D(x) do {printf("%10s | %7d:  [debug] ", __FILE__, __LINE__); printf x; fflush(stdout);} while (0)
#else
#  define D(x) ((void) 0)
#endif
#define BEGMATCH(a, b)  (!strncasecmp((a), (b), (sizeof(b) - 1)))
#define NONULL(x)       ((x) ? (x) : (""))

Epplet_gadget       close_button, in_bar, out_bar, in_label, out_label, title,
   cfg_button;
Epplet_gadget       cfg_tb_dev;
Window              config_win = None;
int                 in_val = 0, out_val = 0, show_title = 1;
unsigned long       max_in = 1, max_out = 1;
char               *device_string = NULL;
unsigned long       last_in = 0, last_out = 0;

static void         timer_cb(void *data);
static void         close_cb(void *data);
static void         in_cb(void *data, Window w);
static void         out_cb(void *data, Window w);
static void         parse_conf(void);

static void
show_size(unsigned long n, char *buff)
{
   if (n < 1024)
     {
	sprintf(buff, "%lu b", n);
     }
   else if (n < 1024 * 1024)
     {
	sprintf(buff, "%lu k", n / 1024);
     }
   else if (n < 1024 * 1024 * 1024)
     {
	sprintf(buff, "%lu M", n / (1024 * 1024));
     }
   else
     {
	sprintf(buff, "%lu G", n / (1024 * 1024 * 1024));
     }
}

static void
timer_cb(void *data)
{
   char                buff[1024];
   double              val = -1.0, val2 = -1.0;
   unsigned char       invalid;
   unsigned long       in_bytes, out_bytes;
   static unsigned long in_delta = -1, out_delta = -1;

   invalid = net_get_bytes_inout(device_string, &val, &val2);
   if (invalid)
     {
	D(("Failed to open %s -- %s\n", device_string, strerror(errno)));
	Esync();
	Epplet_timer(timer_cb, NULL, 10.0, "TIMER");
	return;
     }
   if ((last_in == 0) && (last_out == 0))
     {
	last_in = (unsigned long)val;
	last_out = (unsigned long)val2;
	Esync();
	Epplet_timer(timer_cb, NULL, 1.0, "TIMER");
	return;
     }

   in_bytes = (unsigned long)val - last_in;
   out_bytes = (unsigned long)val2 - last_out;
   D(("In %lu, out %lu\n", in_bytes, out_bytes));

   if (in_bytes > max_in)
     {
	max_in = in_bytes;
	Esnprintf(buff, sizeof(buff), "%lu", max_in);
	Epplet_modify_config("max_in", buff);
	D(("Set max_in to %lu\n", max_in));
     }
   if (in_bytes != in_delta)
     {
	in_val = (int)((((float)in_bytes) / max_in) * 100.0);
	Epplet_gadget_data_changed(in_bar);
	sprintf(buff, "I: ");
	show_size(in_bytes, buff + 3);
	strcat(buff, "/s");
	Epplet_change_label(in_label, buff);
	in_delta = in_bytes;
     }
   last_in += in_bytes;

   if (out_bytes > max_out)
     {
	max_out = out_bytes;
	Esnprintf(buff, sizeof(buff), "%lu", max_out);
	Epplet_modify_config("max_out", buff);
	D(("Set max_out to %lu\n", max_out));
     }
   if (out_bytes != out_delta)
     {
	out_val = (int)((((float)out_bytes) / max_out) * 100.0);
	Epplet_gadget_data_changed(out_bar);
	sprintf(buff, "O: ");
	show_size(out_bytes, buff + 3);
	strcat(buff, "/s");
	Epplet_change_label(out_label, buff);
	out_delta = out_bytes;
     }
   last_out += out_bytes;

   Esync();
   Epplet_timer(timer_cb, NULL, 1.0, "TIMER");
   return;
   data = NULL;
}

static void
close_cb(void *data)
{

   Epplet_unremember();
   Esync();
   exit(0);
   data = NULL;
}

static void
in_cb(void *data, Window w)
{

   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_show(close_button);
	Epplet_gadget_show(cfg_button);
     }
   return;
   data = NULL;
}

static void
out_cb(void *data, Window w)
{

   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_hide(close_button);
	Epplet_gadget_hide(cfg_button);
     }
   return;
   data = NULL;
}

static void
apply_config(void)
{
   char                buff[1024];

   if (show_title)
     {
	Epplet_gadget_move(in_label, 3, 13);
	Epplet_gadget_move(out_label, 3, 30);
	Epplet_gadget_move(in_bar, 3, 22);
	Epplet_gadget_move(out_bar, 3, 39);
	Epplet_gadget_show(title);
     }
   else
     {
	Epplet_gadget_move(in_label, 4, 4);
	Epplet_gadget_move(out_label, 4, 24);
	Epplet_gadget_move(in_bar, 4, 14);
	Epplet_gadget_move(out_bar, 4, 36);
	Epplet_gadget_hide(title);
     }
   Epplet_modify_config("title", (show_title ? "1" : "0"));

   strcpy(buff, NONULL(Epplet_textbox_contents(cfg_tb_dev)));
   if (strcmp(buff, device_string))
     {
	Epplet_modify_config("dev", buff);
	device_string = Epplet_query_config("dev");
	last_in = last_out = 0;
     }

   timer_cb(NULL);
}

static void
ok_cb(void *data)
{
   apply_config();
   Epplet_save_config();
   Epplet_window_destroy(config_win);
   config_win = None;
   return;
   data = NULL;
}

static void
apply_cb(void *data)
{
   apply_config();
   return;
   data = NULL;
}

static void
cancel_cb(void *data)
{
   Epplet_window_destroy(config_win);
   config_win = None;
   return;
   data = NULL;
}

static void
config_cb(void *data)
{
   if (config_win)
     {
	return;
     }

   config_win =
      Epplet_create_window_config(200, 100, "E-Bandwidth Configuration", ok_cb,
				  NULL, apply_cb, NULL, cancel_cb, NULL);

   Epplet_gadget_show(Epplet_create_label(4, 4, "Device:", 2));
   Epplet_gadget_show(cfg_tb_dev =
		      Epplet_create_textbox(NULL, device_string, 4, 18, 192, 20,
					    2, NULL, NULL));

   Epplet_gadget_show(Epplet_create_togglebutton
		      (NULL, NULL, 4, 50, 12, 12, &show_title, NULL, NULL));
   Epplet_gadget_show(Epplet_create_label(20, 50, "Show title?", 2));

   Epplet_window_show(config_win);
   Epplet_window_pop_context();

   return;
   data = NULL;
}

static void
parse_conf(void)
{

   char               *s;

   s = Epplet_query_config("max_in");
   if (s)
     {
	max_in = strtoul(s, (char **)NULL, 10);
     }
   s = Epplet_query_config("max_out");
   if (s)
     {
	max_out = strtoul(s, (char **)NULL, 10);
     }
   show_title = atoi(Epplet_query_config_def("title", "1"));
   device_string = Epplet_query_config_def("dev", "eth0");
}

int
main(int argc, char **argv)
{

   int                 prio;

   prio = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), prio + 10);
   atexit(Epplet_cleanup);
   Epplet_Init("E-Bandwidth", "0.2", "Enlightenment Network I/O Monitor Epplet",
	       3, 3, argc, argv, 0);
   Epplet_load_config();
   parse_conf();

   title = Epplet_create_label(3, 3, "Net I/O", 1);
   if (show_title)
     {
	/* New arrangement */
	in_label = Epplet_create_label(3, 13, "I: 0 b/s", 1);
	out_label = Epplet_create_label(3, 30, "O: 0 b/s", 1);
	in_bar = Epplet_create_hbar(3, 22, 42, 7, 0, &in_val);
	out_bar = Epplet_create_hbar(3, 39, 42, 7, 0, &out_val);
	Epplet_gadget_show(title);
     }
   else
     {
	/* Old arrangement */
	in_label = Epplet_create_label(4, 4, "I: 0 b/s", 1);
	out_label = Epplet_create_label(4, 24, "O: 0 b/s", 1);
	in_bar = Epplet_create_hbar(4, 14, 40, 8, 0, &in_val);
	out_bar = Epplet_create_hbar(4, 36, 40, 8, 0, &out_val);
     }
   close_button =
      Epplet_create_button(NULL, NULL, 2, 2, 0, 0, "CLOSE", 0, NULL, close_cb,
			   NULL);
   cfg_button =
      Epplet_create_button(NULL, NULL, 33, 2, 0, 0, "CONFIGURE", 0, NULL,
			   config_cb, NULL);
   Epplet_gadget_show(in_label);
   Epplet_gadget_show(in_bar);
   Epplet_gadget_show(out_label);
   Epplet_gadget_show(out_bar);
   Epplet_show();

   Epplet_register_focus_in_handler(in_cb, NULL);
   Epplet_register_focus_out_handler(out_cb, NULL);
   timer_cb(NULL);		/* Set everything up */
   Epplet_Loop();

   return 0;
}
