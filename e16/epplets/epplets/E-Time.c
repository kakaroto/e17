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

static const char   cvs_ident[] =
   "$Id$";

#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include "epplet.h"

#if 0
#  define D(x) do {printf("%10s | %7d:  [debug] ", __FILE__, __LINE__); printf x; fflush(stdout);} while (0)
#else
#  define D(x) ((void) 0)
#endif
#define BEGMATCH(a, b)  (!strncasecmp((a), (b), (sizeof(b) - 1)))
#define NONULL(x)       ((x) ? (x) : (""))

Epplet_gadget       close_button, cfg_button, label1, label2, label3, label4;
Epplet_gadget       cfg_tb_line1, cfg_tb_line2, cfg_tb_line3, cfg_tb_line4,
   cfg_tb_tz;
Window              config_win;
char               *line1, *line2, *line3, *line4, *timezone_str;
int                 just = 1, cfg_just = 1;

static void         timer_cb(void *data);
static void         close_cb(void *data);
static void         in_cb(void *data, Window w);
static void         out_cb(void *data, Window w);
static int          delete_cb(void *data, Window win);
static void         apply_config(void);
static void         ok_cb(void *data);
static void         apply_cb(void *data);
static void         cancel_cb(void *data);
static void         config_cb(void *data);

static void
timer_cb(void *data)
{
   char                tm[64];
   struct tm           tim;
   struct tm          *tim2;
   time_t              t2;

   if (timezone_str && *timezone_str)
     {
	Esnprintf(tm, sizeof(tm), "TZ=%s", timezone_str);
	putenv(tm);
     }
   t2 = time(NULL);
   tim2 = localtime(&t2);
   if (tim2)
     {
	memcpy(&tim, tim2, sizeof(struct tm));
	strftime(tm, 63, line1, &tim);
	Epplet_change_label(label1, tm);
	strftime(tm, 63, line2, &tim);
	Epplet_change_label(label2, tm);
	strftime(tm, 63, line3, &tim);
	Epplet_change_label(label3, tm);
	strftime(tm, 63, line4, &tim);
	Epplet_change_label(label4, tm);
     }
   Epplet_timer(timer_cb, NULL, 0.5, "TIMER");
   return;
   data = NULL;
}

static void
close_cb(void *data)
{
   Epplet_unremember();
   Esync();
   data = NULL;
   exit(0);
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

static int
delete_cb(void *data, Window win)
{
   config_win = None;
   return 1;
   win = (Window) 0;
   data = NULL;
}

static void
apply_config(void)
{
   char                buff[1024];
   int                 tmp;

   tmp = (cfg_just ? -1 : 1);
   if (just != tmp)
     {
	just = tmp;
	Epplet_gadget_move(label1, 4 * just, 4);
	Epplet_gadget_move(label2, 4 * just, 15);
	Epplet_gadget_move(label3, 4 * just, 26);
	Epplet_gadget_move(label4, 4 * just, 36);
	Esnprintf(buff, sizeof(buff), "%d", just);
	Epplet_modify_config("just", buff);
     }

   strcpy(buff, NONULL(Epplet_textbox_contents(cfg_tb_line1)));
   Epplet_modify_config("line1", buff);
   line1 = Epplet_query_config("line1");

   strcpy(buff, NONULL(Epplet_textbox_contents(cfg_tb_line2)));
   Epplet_modify_config("line2", buff);
   line2 = Epplet_query_config("line2");

   strcpy(buff, NONULL(Epplet_textbox_contents(cfg_tb_line3)));
   Epplet_modify_config("line3", buff);
   line3 = Epplet_query_config("line3");

   strcpy(buff, NONULL(Epplet_textbox_contents(cfg_tb_line4)));
   Epplet_modify_config("line4", buff);
   line4 = Epplet_query_config("line4");

   strcpy(buff, NONULL(Epplet_textbox_contents(cfg_tb_tz)));
   Epplet_modify_config("timezone", buff);
   timezone_str = Epplet_query_config("timezone");

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
      Epplet_create_window_config(200, 280, "E-Time Configuration", ok_cb, NULL,
				  apply_cb, NULL, cancel_cb, NULL);

   Epplet_gadget_show(Epplet_create_label(4, 4, "First Line:", 2));
   Epplet_gadget_show(cfg_tb_line1 =
		      Epplet_create_textbox(NULL, line1, 4, 18, 192, 20, 2,
					    NULL, NULL));

   Epplet_gadget_show(Epplet_create_label(4, 50, "Second Line:", 2));
   Epplet_gadget_show(cfg_tb_line2 =
		      Epplet_create_textbox(NULL, line2, 4, 64, 192, 20, 2,
					    NULL, NULL));

   Epplet_gadget_show(Epplet_create_label(4, 96, "Third Line:", 2));
   Epplet_gadget_show(cfg_tb_line3 =
		      Epplet_create_textbox(NULL, line3, 4, 110, 192, 20, 2,
					    NULL, NULL));

   Epplet_gadget_show(Epplet_create_label(4, 142, "Fourth Line:", 2));
   Epplet_gadget_show(cfg_tb_line4 =
		      Epplet_create_textbox(NULL, line4, 4, 156, 192, 20, 2,
					    NULL, NULL));

   Epplet_gadget_show(Epplet_create_label(4, 186, "Time Zone ($TZ):", 2));
   Epplet_gadget_show(cfg_tb_tz =
		      Epplet_create_textbox(NULL, timezone_str, 4, 200, 192, 20,
					    2, NULL, NULL));

   cfg_just = ((just == -1) ? 1 : 0);
   Epplet_gadget_show(Epplet_create_togglebutton
		      (NULL, NULL, 4, 232, 12, 12, &cfg_just, NULL, NULL));
   Epplet_gadget_show(Epplet_create_label(20, 232, "Right-justify text?", 2));

   Epplet_window_show(config_win);
   Epplet_window_pop_context();

   return;
   data = NULL;
}

static void
parse_config(void)
{
   line1 = Epplet_query_config_def("line1", "%A");
   line2 = Epplet_query_config_def("line2", "%e  %b");
   line3 = Epplet_query_config_def("line3", "%H:%M:%S");
   line4 = Epplet_query_config_def("line4", "%Z %Y");
   timezone_str = Epplet_query_config_def("timezone", "");
   just = atoi(Epplet_query_config_def("just", "-1"));
}

int
main(int argc, char **argv)
{
   atexit(Epplet_cleanup);

   Epplet_Init("E-Time", "0.1", "Enlightenment Digital Clock Epplet", 3, 3,
	       argc, argv, 0);
   Epplet_load_config();
   parse_config();

   close_button = Epplet_create_std_button("CLOSE", 2, 2, close_cb, NULL);
   cfg_button = Epplet_create_std_button("CONFIGURE", 33, 2, config_cb, NULL);
   Epplet_gadget_show(label1 = Epplet_create_label(4 * just, 4, "", 1));
   Epplet_gadget_show(label2 = Epplet_create_label(4 * just, 15, "", 1));
   Epplet_gadget_show(label3 = Epplet_create_label(4 * just, 26, "", 1));
   Epplet_gadget_show(label4 = Epplet_create_label(4 * just, 36, "", 1));
   Epplet_register_focus_in_handler(in_cb, NULL);
   Epplet_register_focus_out_handler(out_cb, NULL);
   Epplet_register_delete_event_handler(delete_cb, NULL);

   Epplet_show();
   timer_cb(NULL);
   Epplet_Loop();
   return 0;
}
