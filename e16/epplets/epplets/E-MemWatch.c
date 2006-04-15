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
#include "config.h"
#include "epplet.h"

#ifdef HAVE_LIBGTOP
#include <glibtop.h>
#include <glibtop/mem.h>
#include <glibtop/swap.h>
#include "proc.h"
#endif

#if 0
#  define D(x) do {printf("%10s | %7d:  [debug] ", __FILE__, __LINE__); printf x; fflush(stdout);} while (0)
#else
#  define D(x) ((void) 0)
#endif
#define BEGMATCH(a, b)  (!strncasecmp((a), (b), (sizeof(b) - 1)))
#define NONULL(x)       ((x) ? (x) : (""))

Epplet_gadget       close_button, mem_bar, swap_bar, mem_label, swap_label,
   title, cfg_button;
int                 mem_val = 0, swap_val = 0;
int                 show_title = 1;
int                 kernel_2_6 = 0;

static void         timer_cb(void *data);
static void         close_cb(void *data);
static void         in_cb(void *data, Window w);
static void         out_cb(void *data, Window w);

static void
timer_cb(void *data)
{

   char                buff[1024];
   unsigned long       total, used, buffers, cached, free;

#ifdef HAVE_LIBGTOP
   int                 check = 0;
   glibtop_mem         mem;
   glibtop_swap        swap;

   glibtop_get_mem(&mem);

   total = (unsigned long)mem.total;
   used = (unsigned long)mem.used;
   buffers = (unsigned long)mem.buffer;
   cached = (unsigned long)mem.cached;
#else
   FILE               *fp;

   if ((fp = fopen("/proc/meminfo", "r")) == NULL)
     {
	D(("Failed to open /proc/meminfo -- %s\n", strerror(errno)));
	return;
     }

   if (kernel_2_6)
     {
	fgets(buff, sizeof(buff), fp);
	sscanf(buff, "%*s %lu", &total);
	fgets(buff, sizeof(buff), fp);
	sscanf(buff, "%*s %lu", &free);
	fgets(buff, sizeof(buff), fp);
	sscanf(buff, "%*s %lu", &buffers);
	fgets(buff, sizeof(buff), fp);
	sscanf(buff, "%*s %lu", &cached);

	total *= 1024;
	free *= 1024;
	buffers *= 1024;
	cached *= 1024;
	used = total - free;

     }
   else
     {

	fgets(buff, sizeof(buff), fp);	/* Ignore the first line */
	fgets(buff, sizeof(buff), fp);
	sscanf(buff, "%*s %lu %lu %*u %*u %lu %lu", &total, &used, &buffers,
	       &cached);
     }

#endif /* HAVE_LIBGTOP */

   used -= (buffers + cached);
   mem_val = (int)((((float)used) / total) * 100.0);
   D(("%d = 100 * %lu / %lu\n", (100 * used) / total, used, total));
   D(("Memory:  %d%% (%lu/%lu)\n", mem_val, used, total));
   Epplet_gadget_data_changed(mem_bar);
   if (used < 1024)
     {
	Esnprintf(buff, sizeof(buff), "M: %lub", used);
     }
   else if (used < 1024 * 1024)
     {
	Esnprintf(buff, sizeof(buff), "M: %luK", used / 1024);
     }
   else if (used < 1024 * 1024 * 1024)
     {
	Esnprintf(buff, sizeof(buff), "M: %luM", used / (1024 * 1024));
     }
   else
     {
	Esnprintf(buff, sizeof(buff), "M: %luG", used / (1024 * 1024 * 1024));
     }
   Epplet_change_label(mem_label, buff);

#ifdef HAVE_LIBGTOP
   glibtop_get_swap(&swap);
   check = 0;

   do
     {
	total = (unsigned long)swap.total;
	used = (unsigned long)swap.used;
     }
   while (swap.total == 0 && swap.used == 0 && check++ < 15);
#else

   if (kernel_2_6)
     {
	fgets(buff, sizeof(buff), fp);
	fgets(buff, sizeof(buff), fp);
	fgets(buff, sizeof(buff), fp);
	fgets(buff, sizeof(buff), fp);
	fgets(buff, sizeof(buff), fp);
	fgets(buff, sizeof(buff), fp);
	fgets(buff, sizeof(buff), fp);
	fgets(buff, sizeof(buff), fp);
	sscanf(buff, "%*s %lu", &total);
	fgets(buff, sizeof(buff), fp);
	sscanf(buff, "%*s %lu", &free);;

	total *= 1024;
	free *= 1024;
	used = total - free;

     }
   else
     {

	fgets(buff, sizeof(buff), fp);
	sscanf(buff, "%*s %lu %lu", &total, &used);
     }

   fclose(fp);
#endif /* HAVE_LIBGTOP */
   swap_val = (int)((((float)used) / total) * 100.0);

   /*printf ("Swap: %lu %lu %d%%\n", total, used, swap_val); */

   D(("Swap:  %d%% (%lu/%lu)\n", swap_val, used, total));
   Epplet_gadget_data_changed(swap_bar);
   if (used < 1024)
     {
	Esnprintf(buff, sizeof(buff), "S: %lub", used);
     }
   else if (used < 1024 * 1024)
     {
	Esnprintf(buff, sizeof(buff), "S: %luK", used / 1024);
     }
   else if (used < 1024 * 1024 * 1024)
     {
	Esnprintf(buff, sizeof(buff), "S: %luM", used / (1024 * 1024));
     }
   else
     {
	Esnprintf(buff, sizeof(buff), "S: %luG", used / (1024 * 1024 * 1024));
     }
   Epplet_change_label(swap_label, buff);

   Esync();
   Epplet_timer(timer_cb, NULL, 3.0, "TIMER");
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
	Epplet_gadget_move(mem_label, 3, 13);
	Epplet_gadget_move(swap_label, 3, 30);
	Epplet_gadget_move(mem_bar, 3, 22);
	Epplet_gadget_move(swap_bar, 3, 39);
	Epplet_gadget_show(title);
     }
   else
     {
	Epplet_gadget_move(mem_label, 4, 4);
	Epplet_gadget_move(swap_label, 4, 24);
	Epplet_gadget_move(mem_bar, 4, 14);
	Epplet_gadget_move(swap_bar, 4, 36);
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

int
main(int argc, char **argv)
{

   int                 prio;

   /* check for Kernel 2.6 */
   FILE               *fp;
   char                version[8], buff[1024];

   if ((fp = fopen("/proc/version", "r")) == NULL)
     {
	D(("Failed to open /proc/version -- %s\nWill assume not 2.6 kernel",
	   strerror(errno)));
	return 1;
     }

   fgets(buff, sizeof(buff), fp);
   sscanf(buff, "%*s %*s %s", version);

   if (version[2] == '6')
     {
	kernel_2_6 = 1;
     }

   fclose(fp);
   /* end check for Kernel 2.6 */

   atexit(Epplet_cleanup);
   prio = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), prio + 10);
   Epplet_Init("E-MemWatch", "0.1", "Enlightenment RAM/Swap Monitor Epplet", 3,
	       3, argc, argv, 0);
   Epplet_load_config();
   show_title = atoi(Epplet_query_config_def("title", "1"));

   title = Epplet_create_label(3, 3, "Mem Used", 1);
   if (show_title)
     {
	mem_label = Epplet_create_label(3, 13, "M:", 1);
	swap_label = Epplet_create_label(3, 30, "S:", 1);
	mem_bar = Epplet_create_hbar(3, 22, 42, 7, 0, &mem_val);
	swap_bar = Epplet_create_hbar(3, 39, 42, 7, 0, &swap_val);
	Epplet_gadget_show(title);
     }
   else
     {
	mem_label = Epplet_create_label(4, 4, "M:", 1);
	swap_label = Epplet_create_label(4, 24, "S:", 1);
	mem_bar = Epplet_create_hbar(4, 14, 40, 8, 0, &mem_val);
	swap_bar = Epplet_create_hbar(4, 36, 40, 8, 0, &swap_val);
     }
   close_button =
      Epplet_create_button(NULL, NULL, 2, 2, 0, 0, "CLOSE", 0, NULL, close_cb,
			   NULL);
   cfg_button =
      Epplet_create_button(NULL, NULL, 33, 2, 0, 0, "CONFIGURE", 0, NULL,
			   title_cb, NULL);
   Epplet_gadget_show(mem_label);
   Epplet_gadget_show(mem_bar);
   Epplet_gadget_show(swap_label);
   Epplet_gadget_show(swap_bar);
   Epplet_show();

   Epplet_register_focus_in_handler(in_cb, NULL);
   Epplet_register_focus_out_handler(out_cb, NULL);
   timer_cb(NULL);		/* Set everything up */
   Epplet_Loop();

   return 0;
}
