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

#if 0
#  define D(x) do {printf("%10s | %7d:  [debug] ", __FILE__, __LINE__); printf x; fflush(stdout);} while (0)
#else
#  define D(x) ((void) 0)
#endif
#define BEGMATCH(a, b)  (!strncasecmp((a), (b), (sizeof(b) - 1)))
#define NONULL(x)       ((x) ? (x) : (""))

#define PREV_HACK()      do {if (idx == 1) idx = hack_cnt - 1; else if (idx == 0) idx = hack_cnt - 2; else idx -= 2;} while (0)
#define CUR_HACK()       ((idx == 0) ? (hack_cnt - 1) : (idx - 1))
#define NEXT_HACK()      ((void) 0)
#define INC_HACK()       do {idx++; if (idx == hack_cnt) idx = 0;} while (0)
#define DEC_HACK()       do {if (idx == 0) idx = hack_cnt - 1; else idx--;} while (0)

Epplet_gadget       close_button, prev_button, next_button, zoom_button,
   cfg_button, lock_button, draw_area;
Epplet_gadget       cfg_tb_delay, *cfg_tb_hacks;
Window              config_win = None;
unsigned long       idx = 0, hack_cnt = 0;
double              delay;
char              **hacks = NULL;
unsigned char       paused = 0;
pid_t               hack_pid = -1;

static void         start_hack(char *hack);
static void         stop_hack(void);
static void         change_hack(void *data);
static void         child_cb(void *data, int pid, int exit_code);
static void         close_cb(void *data);
static void         zoom_cb(void *data);
static void         play_cb(void *data);
static void         in_cb(void *data, Window w);
static void         out_cb(void *data, Window w);
static int          delete_cb(void *data, Window win);
static void         apply_config(void);
static void         ok_cb(void *data);
static void         apply_cb(void *data);
static void         cancel_cb(void *data);
static void         config_cb(void *data);
static void         lock_cb(void *data);

static void
start_hack(char *hack)
{

   char                buff[1024];

   Esnprintf(buff, sizeof(buff), "%s -window-id %ld", hack,
	     Epplet_get_drawingarea_window(draw_area));
   Epplet_register_child_handler(child_cb, NULL);
   hack_pid = Epplet_spawn_command(buff);
}

static void
stop_hack(void)
{
   if (hack_pid != -1)
     {
	kill(hack_pid, SIGTERM);
     }
   hack_pid = -1;
}

static void
change_hack(void *data)
{

   stop_hack();
   start_hack(hacks[idx]);
   INC_HACK();

   Epplet_timer(change_hack, NULL, delay, "CHANGE_HACK");
   return;
   data = NULL;
}

static void
child_cb(void *data, int pid, int exit_code)
{

   if (pid == hack_pid)
     {
	fprintf(stderr, "E-Xss:  Child process exited with return code %d\n",
		exit_code);
	Epplet_remove_timer("CHANGE_HACK");
	Epplet_timer(change_hack, NULL, 0.1, "CHANGE_HACK");
     }
   return;
   data = NULL;
}

static void
close_cb(void *data)
{

   stop_hack();
   Epplet_unremember();
   Esync();
   exit(0);
   data = NULL;
}

static void
zoom_cb(void *data)
{

   Epplet_spawn_command(hacks[CUR_HACK()]);

   return;
   data = NULL;
}

static void
play_cb(void *data)
{

   int                 op = (int)data;

   switch (op)
     {
     case -1:
	/* Previous hack */
	PREV_HACK();
	change_hack(NULL);
	break;
     case 1:
	/* Next hack */
	NEXT_HACK();
	change_hack(NULL);
	break;
     default:
	break;
     }
   return;
   data = NULL;
}

static void
in_cb(void *data, Window w)
{

   if (w == Epplet_get_main_window())
     {
	/*Epplet_gadget_hide(draw_area); */
	Epplet_gadget_show(close_button);
	Epplet_gadget_show(zoom_button);
	Epplet_gadget_show(prev_button);
	Epplet_gadget_show(next_button);
	Epplet_gadget_show(cfg_button);
	Epplet_gadget_show(lock_button);
     }
   return;
   data = NULL;
}

static void
out_cb(void *data, Window w)
{

   if (w == Epplet_get_main_window())
     {
	/*Epplet_gadget_show(draw_area); */
	Epplet_gadget_hide(close_button);
	Epplet_gadget_hide(zoom_button);
	Epplet_gadget_hide(prev_button);
	Epplet_gadget_hide(next_button);
	Epplet_gadget_hide(cfg_button);
	Epplet_gadget_hide(lock_button);
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
   char                buff[1024], *s;
   unsigned long       i, j;

   strcpy(buff, Epplet_textbox_contents(cfg_tb_delay));
   if ((delay = atof(buff)) != 0.0)
     {
	Epplet_modify_config("delay", buff);
     }
   else
     {
	delay = atof(Epplet_query_config_def("delay", "60.0"));
     }

   for (i = 0, j = 0; i < hack_cnt; i++)
     {
	s = Epplet_textbox_contents(cfg_tb_hacks[i]);
	if (strlen(s))
	  {
	     Esnprintf(buff, sizeof(buff), "hack_%lu", j);
	     Epplet_modify_config(buff, s);
	     hacks[j++] = Epplet_query_config(buff);
	  }
     }
   hack_cnt = j;
   if (idx >= hack_cnt)
     {
	idx = 0;
     }
   hacks = (char **)realloc(hacks, sizeof(char *) * (hack_cnt));
   DEC_HACK();
   change_hack(NULL);
}

static void
ok_cb(void *data)
{
   apply_config();
   Epplet_save_config();
   Epplet_window_destroy(config_win);
   config_win = None;
   free(cfg_tb_hacks);
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
   free(cfg_tb_hacks);
   return;
   data = NULL;
}

static void
config_cb(void *data)
{
   char                buff[128];
   unsigned long       h, y, i;

   if (config_win)
     {
	return;
     }

   hacks = (char **)realloc(hacks, sizeof(char *) * (++hack_cnt));

   cfg_tb_hacks = (Epplet_gadget *) malloc(sizeof(Epplet_gadget) * hack_cnt);
   memset(cfg_tb_hacks, 0, sizeof(Epplet_gadget) * hack_cnt);

   h = 70 + (hack_cnt * 41);
   config_win =
      Epplet_create_window_config(300, h, "E-Xss Configuration", ok_cb, NULL,
				  apply_cb, NULL, cancel_cb, NULL);

   sprintf(buff, "%3.2f", delay);
   Epplet_gadget_show(Epplet_create_label
		      (4, 4, "Delay between images (seconds):", 2));
   Epplet_gadget_show(cfg_tb_delay =
		      Epplet_create_textbox(NULL, buff, 4, 18, 292, 20, 2, NULL,
					    NULL));

   for (i = 0, y = 50; i < hack_cnt; i++)
     {
	Esnprintf(buff, sizeof(buff), "Hack #%d:", i);
	Epplet_gadget_show(Epplet_create_label(4, y, buff, 2));
	y += 14;
	Epplet_gadget_show(cfg_tb_hacks[i] =
			   Epplet_create_textbox(NULL,
						 ((i ==
						   hack_cnt -
						   1) ? NULL : hacks[i]), 4, y,
						 292, 20, 2, NULL, NULL));
	y += 27;
     }

   Epplet_window_show(config_win);
   Epplet_window_pop_context();

   return;
   data = NULL;
}

static void
lock_cb(void *data)
{
   Epplet_spawn_command("xscreensaver-command -lock");

   return;
   data = NULL;
}

static void
parse_config(void)
{

   delay = atof(Epplet_query_config_def("delay", "60.0"));
   hacks = (char **)malloc(sizeof(char *) * (hack_cnt + 1));
   for (; 1; hack_cnt++)
     {
	char               *tmp;
	char                buff[40];

	Esnprintf(buff, sizeof(buff), "hack_%lu", hack_cnt);
	tmp = Epplet_query_config(buff);
	if (!tmp)
	  {
	     break;
	  }
	hacks = (char **)realloc(hacks, sizeof(char *) * (hack_cnt + 1));
	hacks[hack_cnt] = tmp;
     }
   if (hack_cnt == 0)
     {
	hack_cnt = 3;
	hacks = (char **)realloc(hacks, sizeof(char *) * (hack_cnt));
	hacks[0] = "kaleidescope";
	hacks[1] = "rorschach";
	hacks[2] = "qix -solid -delay 0 -segments 100";
	Epplet_add_config("hack_0", "kaleidescope");
	Epplet_add_config("hack_1", "strange");
	Epplet_add_config("hack_2", "qix -solid -delay 50 -segments 100");
	Epplet_add_config("hack_3", "demon");
	Epplet_add_config("hack_4", "bubbles");
	Epplet_add_config("hack_5", "xflame");
	Epplet_add_config("hack_6", "compass");
	Epplet_add_config("hack_7", "drift");
	Epplet_add_config("hack_8", "flow");
	Epplet_add_config("hack_9", "galaxy");
	Epplet_add_config("hack_10", "hypercube");
	Epplet_add_config("hack_11", "ifs");
	Epplet_add_config("hack_12", "kumppa -delay 20000");
	Epplet_add_config("hack_13", "laser");
	Epplet_add_config("hack_14", "swirl");
	Epplet_add_config("hack_15", "t3d");
	Epplet_add_config("hack_16", "triangle");
	Epplet_add_config("hack_17", "worm");
     }
}

int
main(int argc, char **argv)
{

   int                 prio;

   prio = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), prio + 10);
   atexit(Epplet_cleanup);
   Epplet_Init("E-Xss", "0.2", "Enlightenment Xscreensaver Epplet", 3, 3, argc,
	       argv, 0);
   Epplet_load_config();
   parse_config();

   draw_area = Epplet_create_drawingarea(3, 3, 43, 43);
   Epplet_gadget_show(draw_area);
   close_button =
      Epplet_create_button(NULL, NULL, 3, 3, 0, 0, "CLOSE", 0, NULL, close_cb,
			   NULL);
   cfg_button = Epplet_create_std_button("CONFIGURE", 33, 3, config_cb, NULL);

   lock_button =
      Epplet_create_button("Lock", NULL, 2, 17, 44, 13, 0, 0, NULL, lock_cb,
			   NULL);

   prev_button =
      Epplet_create_button(NULL, NULL, 3, 33, 0, 0, "PREVIOUS", 0, NULL,
			   play_cb, (void *)(-1));
   zoom_button =
      Epplet_create_button(NULL, NULL, 18, 33, 0, 0, "EJECT", 0, NULL, zoom_cb,
			   NULL);
   next_button =
      Epplet_create_button(NULL, NULL, 33, 33, 0, 0, "NEXT", 0, NULL, play_cb,
			   (void *)(1));

   Epplet_show();

   Epplet_register_focus_in_handler(in_cb, NULL);
   Epplet_register_focus_out_handler(out_cb, NULL);
   Epplet_register_delete_event_handler(delete_cb, NULL);
   change_hack(NULL);
   Epplet_Loop();

   return 0;
}
