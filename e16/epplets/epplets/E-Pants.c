/**********************************************************************
 * E-Pants                                                 January 2001
 * Horms                                             horms@vergenet.net
 * 
 * E-Pants
 * Enlightenment epplet to monitor pants
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
#define LABEL_CHAR 24

#define PANTS_ON_STR "Pants On"
#define PANTS_OFF_STR "Pants Off"

static Epplet_gadget b_close, b_configure, b_help, p_log, pb_log, pb_log_small;
static Window       win;
static int          log_entries;
static int          pants_on;
static char         buf[BUF_LEN];

static void         cb_timer(void *data);
static void         cb_in(void *data, Window w);
static void         cb_out(void *data, Window w);
static void         cb_help(void *data);

static void         toggle_pants(void *data);
static void         set_pants(int pants);
static void         set_pants_on(void *data);
static void         set_pants_off(void *data);
static void         log_pants(void *data);
static void         add_log(char *button_string, char *entry_string);

static void
toggle_pants(void *data)
{
   set_pants(pants_on ? 0 : 1);
}

static void
set_pants(int pants)
{
   if (pants)
      set_pants_on(NULL);
   else
      set_pants_off(NULL);
}

static void
set_pants_on(void *data)
{
   pants_on = 1;
   Epplet_modify_config("pants_on", "1");
   log_pants(NULL);
}

static void
set_pants_off(void *data)
{
   pants_on = 0;
   Epplet_modify_config("pants_on", "0");
   log_pants(NULL);
}

static void
log_pants(void *data)
{
   time_t              now;
   int                 len;
   char               *s;

   s = (pants_on) ? PANTS_ON_STR : PANTS_OFF_STR;

   len = strlen(s) + 1;

   now = time(NULL);
   strncpy(buf, s, BUF_LEN - 1);
   strftime(buf + len + 1, BUF_LEN - len - 1, "%H:%M:%S %a %e %b %G",
	    localtime(&now));
   *(buf + len - 1) = ':';
   *(buf + len) = ' ';
   add_log(s, buf);
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
   Epplet_show_about("E-Pants");
   return;
   data = NULL;
}

static void
cb_configure(void *data)
{
   toggle_pants(NULL);

   return;
   data = NULL;
}

static void
cb_timer(void *data)
{
   Epplet_timer(cb_timer, NULL, 0.5, "TIMER");

   return;
   data = NULL;
}

int
main(int argc, char **argv)
{
   atexit(Epplet_cleanup);

   Epplet_Init("E-Pants", "0.1", "Enlightenment Pants Epplet",
	       4, 1, argc, argv, 0);
   Epplet_timer(cb_timer, NULL, 0.05, "TIMER");

   log_entries = 0;

   b_close = Epplet_create_button(NULL, NULL,
				  2, 2, 0, 0, "CLOSE", 0, NULL, cb_close, NULL);
   b_configure = Epplet_create_button(NULL, NULL,
				      36, 2, 0, 0, "CONFIGURE", win, NULL,
				      cb_configure, NULL);
   b_help = Epplet_create_button(NULL, NULL,
				 50, 2, 0, 0, "HELP", win, NULL, cb_help, NULL);

   p_log = Epplet_create_popup();

   pb_log = Epplet_create_popupbutton("Flim", NULL, 0, 0, 64, 16, NULL, p_log);
   pb_log_small = Epplet_create_popupbutton("Flim", NULL, 15, 0, 20, 16,
					    NULL, p_log);
   Epplet_gadget_show(pb_log);

   Epplet_register_focus_in_handler(cb_in, NULL);
   Epplet_register_focus_out_handler(cb_out, NULL);
   win = Epplet_get_main_window();

   Epplet_load_config();
   set_pants(atoi(Epplet_query_config_def("pants_on", "1")));

   Epplet_show();
   Epplet_Loop();
   return 0;
}
