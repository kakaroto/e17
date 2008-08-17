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

extern void         get_load_average(double *, double *, double *);

#if 0
#  define D(x) do {printf("%10s | %7d:  [debug] ", __FILE__, __LINE__); printf x; fflush(stdout);} while (0)
#else
#  define D(x) ((void) 0)
#endif
#define BEGMATCH(a, b)  (!strncasecmp((a), (b), (sizeof(b) - 1)))
#define NONULL(x)       ((x) ? (x) : (""))

Epplet_gadget       close_button, cfg_button, label1, label2, label3, vbar1,
   vbar2, vbar3;
Window              config_win;
int                 one, five, fifteen;

static void         timer_cb(void *data);
static void         close_cb(void *data);
static void         in_cb(void *data, Window w);
static void         out_cb(void *data, Window w);
static int          delete_cb(void *data, Window win);

static void
timer_cb(void *data)
{
   char                buff[64];
   double              d1, d2, d3;

   get_load_average(&d1, &d2, &d3);
   one = ((d1 > 100.0) ? (100) : ((int)d1));
   five = ((d2 > 100.0) ? (100) : ((int)d2));
   fifteen = ((d3 > 100.0) ? (100) : ((int)d3));

   Epplet_gadget_data_changed(vbar1);
   Epplet_gadget_data_changed(vbar2);
   Epplet_gadget_data_changed(vbar3);

   Esnprintf(buff, sizeof(buff), "%3.2f", d1);
   Epplet_change_label(label1, buff);
   Esnprintf(buff, sizeof(buff), "%3.2f", d2);
   Epplet_change_label(label2, buff);
   Esnprintf(buff, sizeof(buff), "%3.2f", d3);
   Epplet_change_label(label3, buff);

   Epplet_timer(timer_cb, NULL, 5.0, "TIMER");
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

int
main(int argc, char **argv)
{
   atexit(Epplet_cleanup);

   Epplet_Init("E-LoadMeter", "0.1", "Enlightenment CPU Load Epplet", 3, 3,
	       argc, argv, 0);

   close_button = Epplet_create_std_button("CLOSE", 2, 2, close_cb, NULL);
   Epplet_gadget_show(Epplet_create_label(4, 4, "CPU Load", 1));
   Epplet_gadget_show(label1 = Epplet_create_label(3, 15, "", 1));
   Epplet_gadget_show(label2 = Epplet_create_label(3, 26, "", 1));
   Epplet_gadget_show(label3 = Epplet_create_label(3, 36, "", 1));

   Epplet_gadget_show(vbar1 = Epplet_create_vbar(31, 15, 5, 30, 1, &one));
   Epplet_gadget_show(vbar2 = Epplet_create_vbar(36, 15, 5, 30, 1, &five));
   Epplet_gadget_show(vbar3 = Epplet_create_vbar(41, 15, 5, 30, 1, &fifteen));

   Epplet_register_focus_in_handler(in_cb, NULL);
   Epplet_register_focus_out_handler(out_cb, NULL);
   Epplet_register_delete_event_handler(delete_cb, NULL);

   Epplet_show();
   timer_cb(NULL);
   Epplet_Loop();
   return 0;
}
