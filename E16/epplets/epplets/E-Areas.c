/*
 * Copyright (C) 1999-2000 John J. Slee <john@chirp.com.au>
 *
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "epplet.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
Epplet_gadget       b_close;

static void         close_cb(void *data);
static void         in_cb(void *data, Window w);
static void         out_cb(void *data, Window w);
Epplet_gadget       epp_create_std_button(int x, int y, char *std,
					  void (*func) (void *data),
					  void *data);

#if 0
/* Unused */
static void
epp_dialog_ok_f(char *fmt, ...)
{
   va_list             ap;
   char                buf[1024];

   va_start(ap, fmt);
   Evsnprintf(buf, 1023, fmt, ap);
   va_end(ap);
   Epplet_dialog_ok(buf);
}
#endif

static void
close_cb(void *data)
{
   Epplet_unremember();
   Esync();
   data = NULL;
   exit(0);
}

static void
arrow_cb(void *data)
{
   Epplet_send_ipc(data);
}

static void
in_cb(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_show(b_close);
     }
   return;
   data = NULL;
}

static void
out_cb(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_hide(b_close);
     }
   return;
   data = NULL;
}

Epplet_gadget
epp_create_std_button(int x, int y, char *std,
		      void (*func) (void *data), void *data)
{
   assert(func != NULL);
   return Epplet_create_button(NULL, NULL, x, y, 0, 0, std, 0, NULL, func,
			       data);
}

#if 0				/* < e16 < 0.16.8 */
#define AREA_UP     "goto_area prev vert"
#define AREA_DOWN   "goto_area next vert"
#define AREA_LEFT   "goto_area prev horiz"
#define AREA_RIGHT  "goto_area next horiz"
#else
#define AREA_UP     "area move 0 -1"
#define AREA_DOWN   "area move 0 1"
#define AREA_LEFT   "area move -1 0"
#define AREA_RIGHT  "area move 1 0"
#endif

int
main(int argc, char **argv)
{
   Epplet_gadget       b_up, b_down, b_left, b_right;

   atexit(Epplet_cleanup);
   Epplet_Init("E-Areas", "0.1",
	       "A desktop areas navigator; John Slee <john@chirp.com.au>",
	       3, 3, argc, argv, 0);
   b_close = epp_create_std_button(18, 18, "CLOSE", close_cb, NULL);

   b_up = epp_create_std_button(18, 6, "ARROW_UP", arrow_cb, AREA_UP);
   b_down = epp_create_std_button(18, 30, "ARROW_DOWN", arrow_cb, AREA_DOWN);
   b_left = epp_create_std_button(6, 18, "ARROW_LEFT", arrow_cb, AREA_LEFT);
   b_right = epp_create_std_button(30, 18, "ARROW_RIGHT", arrow_cb, AREA_RIGHT);
   Epplet_gadget_show(b_up);
   Epplet_gadget_show(b_down);
   Epplet_gadget_show(b_left);
   Epplet_gadget_show(b_right);

   Epplet_register_focus_in_handler(in_cb, NULL);
   Epplet_register_focus_out_handler(out_cb, NULL);

   Epplet_show();
   Epplet_Loop();
   return 0;
}
