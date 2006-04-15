/*
 * Copyright (C) 1999-2005, Michael Jennings
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

Epplet_gadget       close_button, in_bar, out_bar, in_label, out_label, title,
   cfg_button;
int                 in_val = 0, out_val = 0;
unsigned long       max_in = 1, max_out = 1;
unsigned char       show_title = 1;

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

   FILE               *fp;
   char                buff[1024];
   unsigned long       a, b, c, d, e;
   unsigned long       in_blks = 0, out_blks = 0;
   static unsigned long last_in = 0, last_out = 0, in_delta = 0, out_delta = 0;

   if ((fp = fopen("/proc/diskstats", "r")) == NULL)
     {
	if ((fp = fopen("/proc/stat", "r")) == NULL)
	  {
	     D(("Failed to open /proc/stat -- %s\n", strerror(errno)));
	     return;
	  }
     }

   for (; fgets(buff, sizeof(buff), fp);)
     {
	if (BEGMATCH(buff, "disk_rblk"))
	  {
	     sscanf(buff, "%*s %lu %lu %lu %lu", &a, &b, &c, &d);
	     in_blks = a + b + c + d;
	     fgets(buff, sizeof(buff), fp);
	     sscanf(buff, "%*s %lu %lu %lu %lu", &a, &b, &c, &d);
	     out_blks = a + b + c + d;
	  }
	else if (BEGMATCH(buff, "disk_io"))
	  {
	     char               *pbuff = buff + 9;

	     for (in_blks = out_blks = 0; *pbuff == '(';
		  pbuff = strchr(pbuff, ' ') + 1)
	       {
		  pbuff = strchr(++pbuff, '(');
		  sscanf(++pbuff, "%lu,%lu,%lu,%lu,%lu", &a, &b, &c, &d, &e);
		  in_blks += c;
		  out_blks += e;
	       }
	  }
	else
	  {
	     char                tmp[64];

	     /* Hopefully no one can exploit /proc/diskstats without root. :-) */
	     sscanf(buff, "%lu %lu %s %*u %*u %lu %*u %*u %*u %lu", &a, &b, tmp,
		    &c, &d);
	     /* Check for hdX or sdX, or md#, or...??? */
	     if ((((*tmp == 'h') || (*tmp == 's')) && (tmp[1] == 'd')
		  && (isalpha(tmp[2])) && (tmp[3] == 0)) || ((*tmp == 'm')
							     && (tmp[1] == 'd')
							     &&
							     (isdigit(tmp[2]))))
	       {
		  in_blks += c;
		  out_blks += d;
	       }
	  }
     }
   in_blks -= last_in;
   out_blks -= last_out;
   fclose(fp);

   if (last_in)
     {
	/* We must have some history data to do anything. */
	if (in_blks > max_in)
	  {
	     max_in = in_blks;
	     Esnprintf(buff, sizeof(buff), "%lu", max_in);
	     Epplet_modify_config("max_in", buff);
	  }
	if (in_blks != in_delta)
	  {
	     in_val = (int)((((float)in_blks) / max_in) * 100.0);
	     Epplet_gadget_data_changed(in_bar);
	     sprintf(buff, "I: ");
	     show_size(in_blks * 512, buff + 3);
	     strcat(buff, "/s");
	     Epplet_change_label(in_label, buff);
	  }
	in_delta = in_blks;
     }
   last_in += in_blks;

   if (last_out)
     {
	/* We must have some history data to do anything. */
	if (out_blks > max_out)
	  {
	     max_out = out_blks;
	     Esnprintf(buff, sizeof(buff), "%lu", max_out);
	     Epplet_modify_config("max_out", buff);
	  }
	if (out_blks != out_delta)
	  {
	     out_val = (int)((((float)out_blks) / max_out) * 100.0);
	     Epplet_gadget_data_changed(out_bar);
	     sprintf(buff, "O: ");
	     show_size(out_blks * 512, buff + 3);
	     strcat(buff, "/s");
	     Epplet_change_label(out_label, buff);
	  }
	out_delta = out_blks;
     }
   last_out += out_blks;

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
title_cb(void *data)
{

   show_title = !show_title;
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
   Epplet_redraw();
   return;
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
}

int
main(int argc, char **argv)
{

   int                 prio;

   prio = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), prio + 10);
   atexit(Epplet_cleanup);
   Epplet_Init("E-Disk", "0.2", "Enlightenment Disk I/O Monitor Epplet", 3, 3,
	       argc, argv, 0);
   Epplet_load_config();
   parse_conf();

   title = Epplet_create_label(3, 3, "Disk I/O", 1);
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
			   title_cb, NULL);
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
