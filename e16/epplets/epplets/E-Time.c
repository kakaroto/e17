/*
 * Copyright (C) 1999, Michael Jennings
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

#include "epplet.h"
Epplet_gadget b_close, label1, label2, label3, label4;

static void timer_cb(void *data);
static void close_cb(void *data);
static void in_cb(void *data, Window w);
static void out_cb(void *data, Window w);

static void
timer_cb(void *data)
{
   char                tm[64];
   struct tm           tim;
   struct tm          *tim2;
   time_t              t2;

   t2 = time(NULL);
   tim2 = localtime(&t2);
   if (tim2)
     {
	memcpy(&tim, tim2, sizeof(struct tm));
        strftime(tm, 63, "%A", &tim);
	Epplet_change_label(label1, tm);
        strftime(tm, 63, "%e  %b", &tim);
	Epplet_change_label(label2, tm);
        strftime(tm, 63, "%H:%M:%S", &tim);
	Epplet_change_label(label3, tm);
        strftime(tm, 63, "%Z %Y", &tim);
	Epplet_change_label(label4, tm);
     }      
   Epplet_timer(timer_cb, NULL, 0.5, "TIMER");   
   data = NULL;
}

static void
close_cb(void *data)
{
   Epplet_unremember();
   Esync();
   Epplet_cleanup();
   data = NULL;
   exit(0);
}

static void
in_cb(void *data, Window w)
{
   Epplet_gadget_show(b_close);
   return;
   data = NULL;
   w = (Window) 0;
}

static void
out_cb(void *data, Window w)
{
   Epplet_gadget_hide(b_close);
   return;
   data = NULL;
   w = (Window) 0;
}

int
main(int argc, char **argv)
{
   atexit (Epplet_cleanup);
   
   Epplet_Init("E-Time", "0.1", "Enlightenment Digital Clock Epplet",
	       3, 3, argc, argv, 0);
   b_close = Epplet_create_button(NULL, NULL, 
				  2, 2, 0, 0, "CLOSE", 0, NULL, 
				  close_cb, NULL);
   Epplet_gadget_show(label1 = Epplet_create_label(-4, 4, "", 1));
   Epplet_gadget_show(label2 = Epplet_create_label(-4, 15, "", 1));
   Epplet_gadget_show(label3 = Epplet_create_label(-4, 26, "", 1));
   Epplet_gadget_show(label4 = Epplet_create_label(-4, 36, "", 1));
   Epplet_register_focus_in_handler(in_cb, NULL);
   Epplet_register_focus_out_handler(out_cb, NULL);
   
   Epplet_show();
   timer_cb(NULL);
   Epplet_Loop();
   return 0;
}
