/* E-Netgraph v0.2 - Oct. 27, 1999
 * Written by Daniel Erat, a.k.a. hunchback
 * erat@cats.ucsc.edu, danerat@mindspring.com
 * http://www2.ucsc.edu/~erat/
 * You can also occasionally find me on efnet #e as hunchback. 
 * Modified by Rahsheen Porter (StriderZ) */

#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include "config.h"
#include "epplet.h"
#include "net.h"

/* global variables */
RGB_buf             buf;
Window              win, config_win = None;
unsigned char      *rgb_pointer_start = NULL;

Epplet_gadget       close_button, cfg_button, dev_popup, dev_button, cfg_tb_in,
   cfg_tb_out, cfg_tb_bg, cfg_tb_dev;

long                total_bytes_in = 0;
long                total_bytes_out = 0;
int                 old_in_y = 0, old_out_y = 39;
struct timeval      last_time;

char               *device_string = NULL, *cfg_device_string = NULL;
long                max_bytes_in_per_sec = 8192;
long                max_bytes_out_per_sec = 8192;
int                 in_color[3];
int                 out_color[3];
int                 bg_color[3];
int                 log_scale = 0, cfg_log_scale = 0;
unsigned int        in_color_hex = 0x0040c0;
unsigned int        out_color_hex = 0xc000c0;
unsigned int        bg_color_hex = 0x000000;

static int          delete_cb(void *data, Window win);
static void         apply_config(void);
static void         ok_cb(void *data);
static void         apply_cb(void *data);
static void         cancel_cb(void *data);
static void         config_cb(void *data);

/* functions */
static void
timer_draw(void *data)
{
   long                new_bytes_in = 0;
   long                new_bytes_out = 0;
   double              new_total_bytes_in = -1.0, new_total_bytes_out = -1.0;
   unsigned char      *rgb_pointer_dynamic = NULL, invalid;
   int                 i, j;
   int                 new_in_y, new_out_y;
   float               intensity;
   long                elapsed_microseconds;
   struct timeval      current_time;
   char                err[255];

   for (;
	(invalid =
	 net_get_bytes_inout(device_string, &new_total_bytes_in,
			     &new_total_bytes_out)) != 0;)
     {
	Esnprintf(err, sizeof(err),
		  "Unable to get network device statistics for %s:  %s",
		  device_string, net_strerror(invalid));
	Epplet_dialog_ok(err);
	Esync();
	config_cb(NULL);
	return;
     }
   if (new_total_bytes_in != -1.0)
     {
	if (total_bytes_in == 0)
	   total_bytes_in = (long)new_total_bytes_in;
	if (total_bytes_out == 0)
	   total_bytes_out = (long)new_total_bytes_out;
	new_bytes_in = (long)new_total_bytes_in - total_bytes_in;
	new_bytes_out = (long)new_total_bytes_out - total_bytes_out;
	total_bytes_in = new_total_bytes_in;
	total_bytes_out = new_total_bytes_out;
     }

   /* find out how long it's been since our last calculation */
   gettimeofday(&current_time, NULL);
   elapsed_microseconds =
      (current_time.tv_sec * 1000000L + current_time.tv_usec) -
      (last_time.tv_sec * 1000000L + last_time.tv_usec);
   last_time = current_time;

   /* calculate the percentage of the max, averaging with the last pct */
   if (log_scale)
     {
	new_in_y = (int)(0.5 * old_in_y + 0.5 * 39 *
			 (1 + log10(0.1 + (float)new_bytes_in /
				    ((float)max_bytes_in_per_sec *
				     ((float)elapsed_microseconds /
				      1000000)))));
	new_out_y =
	   39 - (int)(0.5 * (39 - old_out_y) +
		      0.5 * 39 * (1 +
				  log10(0.1 +
					(float)new_bytes_out /
					((float)max_bytes_out_per_sec *
					 ((float)elapsed_microseconds /
					  1000000)))));
     }
   else
     {
	new_in_y = (int)(0.5 * old_in_y + 0.5 * 39 *
			 (float)new_bytes_in /
			 ((float)max_bytes_in_per_sec *
			  ((float)elapsed_microseconds / 1000000)));
	new_out_y = 39 - (int)(0.5 * (39 - old_out_y) + 0.5 * 39 *
			       (float)new_bytes_out /
			       ((float)max_bytes_out_per_sec *
				((float)elapsed_microseconds / 1000000)));
     }

   /* whee.. shift the buffer one pixel to the left */
   for (i = 0; i <= 39; i++)
     {
	for (rgb_pointer_dynamic = rgb_pointer_start + (i * 4 * 40);
	     rgb_pointer_dynamic <= rgb_pointer_start + (i * 4 * 40) + 4 * 38;
	     rgb_pointer_dynamic += 4)
	  {
	     for (j = 0; j <= 2; j++)
		rgb_pointer_dynamic[j] = rgb_pointer_dynamic[j + 4];
	  }
     }

   /* clear the line on the right, and draw the new data */
   for (i = 1; i <= 38; i++)
     {
	rgb_pointer_dynamic = rgb_pointer_start + (i * 4 * 40) + 4 * 38;

	for (j = 0; j <= 2; j++)
	   rgb_pointer_dynamic[j] = bg_color[j];

	intensity = 0;

	if (i >= old_out_y)
	  {
	     if (old_out_y >= new_out_y || i >= new_out_y)
		intensity = ((float)(39 - i) / (float)(39 - old_out_y));
	     else
		intensity = 1 - ((float)(new_out_y - i) / (float)
				 (new_out_y - old_out_y));
	  }
	else if (i >= new_out_y)
	   intensity = 1 - ((float)(old_out_y - i) / (float)
			    (old_out_y - new_out_y));

	if (intensity != 0)
	   for (j = 0; j <= 2; j++)
	      rgb_pointer_dynamic[j] = intensity * out_color[j] +
		 (1 - intensity) * rgb_pointer_dynamic[j];

	intensity = 0;

	if (i <= old_in_y)
	  {
	     if (old_in_y <= new_in_y || i <= new_in_y)
		intensity = ((float)i / (float)old_in_y);
	     else
		intensity = 1 - ((float)(i - new_in_y) / (float)
				 (old_in_y - new_in_y));
	  }
	else if (i <= new_in_y)
	   intensity = 1 - ((float)(i - old_in_y) / (float)
			    (new_in_y - old_in_y));

	if (intensity != 0)
	   for (j = 0; j <= 2; j++)
	      rgb_pointer_dynamic[j] = intensity * in_color[j] +
		 (1 - intensity) * rgb_pointer_dynamic[j];
     }

   old_in_y = new_in_y;
   old_out_y = new_out_y;

   Epplet_paste_buf(buf, win, 0, 0);
   Esync();
   Epplet_timer(timer_draw, NULL, 0.1, "timer_draw");
   return;
   data = NULL;
}

static void
cb_close(void *data)
{
   Epplet_unremember();
   Esync();
   exit(0);
   return;
   data = NULL;
}

static void
cb_in(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_show(cfg_button);
	Epplet_gadget_show(close_button);
     }
   return;
   data = NULL;
}

static void
cb_out(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_hide(cfg_button);
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

static void
apply_config(void)
{
   char               *s;

   device_string = cfg_device_string;
   Epplet_modify_config("device", device_string);

   s = Epplet_textbox_contents(cfg_tb_in);
   if (strlen(s))
     {
	Epplet_modify_config("in_color", s);
	in_color_hex = strtoul(s, (char **)NULL, 0);
	in_color[0] = in_color_hex >> 16;
	in_color[1] = (in_color_hex >> 8) & 0xff;
	in_color[2] = in_color_hex & 0xff;
     }
   s = Epplet_textbox_contents(cfg_tb_out);
   if (strlen(s))
     {
	Epplet_modify_config("out_color", s);
	out_color_hex = strtoul(s, (char **)NULL, 0);
	out_color[0] = out_color_hex >> 16;
	out_color[1] = (out_color_hex >> 8) & 0xff;
	out_color[2] = out_color_hex & 0xff;
     }
   s = Epplet_textbox_contents(cfg_tb_bg);
   if (strlen(s))
     {
	Epplet_modify_config("bg_color", s);
	bg_color_hex = strtoul(s, (char **)NULL, 0);
	bg_color[0] = bg_color_hex >> 16;
	bg_color[1] = (bg_color_hex >> 8) & 0xff;
	bg_color[2] = bg_color_hex & 0xff;
     }

   log_scale = cfg_log_scale;
   Epplet_modify_config("log_scale", (log_scale ? "1" : "0"));

   timer_draw(NULL);
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
dev_popup_cb(void *data)
{
   cfg_device_string = (char *)data;
   Epplet_change_textbox(cfg_tb_dev, cfg_device_string);
}

static void
config_cb(void *data)
{
   char                buff[128];
   static char       **devices = NULL;
   Epplet_gadget       dev_popup;
   unsigned long       i;

   if (config_win)
     {
	return;
     }

   config_win =
      Epplet_create_window_config(200, 230, "E-NetGraph Configuration", ok_cb,
				  NULL, apply_cb, NULL, cancel_cb, NULL);

   dev_popup = Epplet_create_popup();
   if (!devices)
     {
	devices = net_get_devices(NULL);
     }
   if (devices)
     {
	for (i = 0; devices[i]; i++)
	  {
	     Epplet_add_popup_entry(dev_popup, devices[i], NULL, dev_popup_cb,
				    (void *)devices[i]);
	  }
     }
   cfg_device_string = device_string;

   Epplet_gadget_show(Epplet_create_label(4, 4, "Interface to monitor...", 2));
   Epplet_gadget_show(cfg_tb_dev =
		      Epplet_create_textbox(NULL, device_string, 4, 18, 178, 20,
					    2, NULL, NULL));
   Epplet_gadget_show(Epplet_create_popupbutton
		      (NULL, NULL, 185, 22, 12, 12, "ARROW_UP", dev_popup));

   sprintf(buff, "0x%02x%02x%02x", in_color[0], in_color[1], in_color[2]);
   Epplet_gadget_show(Epplet_create_label
		      (4, 50, "Color for incoming packets:", 2));
   Epplet_gadget_show(cfg_tb_in =
		      Epplet_create_textbox(NULL, buff, 4, 64, 192, 20, 2, NULL,
					    NULL));

   sprintf(buff, "0x%02x%02x%02x", out_color[0], out_color[1], out_color[2]);
   Epplet_gadget_show(Epplet_create_label
		      (4, 96, "Color for outgoing packets:", 2));
   Epplet_gadget_show(cfg_tb_out =
		      Epplet_create_textbox(NULL, buff, 4, 110, 192, 20, 2,
					    NULL, NULL));

   sprintf(buff, "0x%02x%02x%02x", bg_color[0], bg_color[1], bg_color[2]);
   Epplet_gadget_show(Epplet_create_label
		      (4, 142, "Color for window background:", 2));
   Epplet_gadget_show(cfg_tb_bg =
		      Epplet_create_textbox(NULL, buff, 4, 156, 192, 20, 2,
					    NULL, NULL));

   cfg_log_scale = log_scale;
   Epplet_gadget_show(Epplet_create_togglebutton
		      (NULL, NULL, 4, 188, 12, 12, &cfg_log_scale, NULL, NULL));
   Epplet_gadget_show(Epplet_create_label
		      (20, 188, "Use logarithmic scale?", 2));

   Epplet_window_show(config_win);
   Epplet_window_pop_context();

   return;
   data = NULL;
}

static void
load_config(void)
{
   char               *s;

   device_string = Epplet_query_config_def("device", "ppp0");
   max_bytes_in_per_sec = atoi(Epplet_query_config_def("max_in", "150000"));
   max_bytes_out_per_sec = atoi(Epplet_query_config_def("max_out", "150000"));
   s = Epplet_query_config_def("in_color", "0x0040c0");
   in_color_hex = strtol(s, (char **)NULL, 0);
   s = Epplet_query_config_def("out_color", "0xc000c0");
   out_color_hex = strtol(s, (char **)NULL, 0);
   s = Epplet_query_config_def("bg_color", "0");
   bg_color_hex = strtol(s, (char **)NULL, 0);
   log_scale = atoi(Epplet_query_config_def("log_scale", "0"));
}

int
main(int argc, char **argv)
{
   Epplet_gadget       drawingarea;
   int                 i = 1;
   int                 prio;

   prio = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), prio + 10);
   Epplet_Init("E-NetGraph", "0.2",
	       "Enlightenment Network Activity epplet", 3, 3, argc, argv, 0);

   Epplet_load_config();
   load_config();
   atexit(Epplet_cleanup);

   while (i < argc)
     {
	if (argv[i][0] == '-')
	  {
	     switch (argv[i][1])
	       {
	       case 'd':
		  if (i + 1 < argc)
		     device_string = strdup(argv[i + 1]);
		  else
		    {
		       fprintf(stderr, "missing device "
			       "string -- aborting\n");
		       Epplet_unremember();
		       Esync();
		       exit(1);
		    }
		  i += 2;
		  break;
	       case 'i':
		  if (i + 1 < argc)
		     sscanf(argv[i + 1], "%x", &in_color_hex);
		  else
		    {
		       fprintf(stderr, "missing incoming color "
			       "string ignored -- aborting\n");
		       exit(1);
		    }
		  i += 2;
		  break;
	       case 'o':
		  if (i + 1 < argc)
		     sscanf(argv[i + 1], "%x", &out_color_hex);
		  else
		    {
		       fprintf(stderr, "missing outgoing color "
			       "string ignored -- aborting\n");
		       exit(1);
		    }
		  i += 2;
		  break;
	       case 'b':
		  if (i + 1 < argc)
		     sscanf(argv[i + 1], "%x", &bg_color_hex);
		  else
		    {
		       fprintf(stderr, "missing background color "
			       "string ignored -- aborting\n");
		       exit(1);
		    }
		  i += 2;
		  break;
	       case 'r':
		  if (i + 1 < argc)
		     max_bytes_in_per_sec = atoi(argv[i + 1]);
		  else
		    {
		       fprintf(stderr, "missing maximum incoming bytes "
			       "per second value -- aborting\n");
		       exit(1);
		    }
		  i += 2;
		  break;
	       case 't':
		  if (i + 1 < argc)
		     max_bytes_out_per_sec = atoi(argv[i + 1]);
		  else
		    {
		       fprintf(stderr, "missing maximum outgoing bytes "
			       "per second value -- aborting\n");
		       exit(1);
		    }
		  i += 2;
		  break;
	       case 'l':
		  log_scale = 1;
		  i++;
		  break;
	       case 'h':
		  printf("usage: E-Netgraph.epplet [OPTION] [OPTION] ..\n"
			 "  -d <string>      defines device to monitor "
			 "[default = ppp0]\n"
			 "  -i <hex number>  defines color to use for "
			 "incoming bytes [default = #0040c0]\n"
			 "  -o <hex number>  defines color to use for "
			 "outgoing bytes [default = #c000c0]\n"
			 "  -b <hex number>  defines color to use for "
			 "background [default = #000000]\n"
			 "  -r <number>      defines max. "
			 "incoming bytes per second [default = 8192]\n"
			 "  -t <number>      defines max. "
			 "outgoing bytes per second [default = 8192]\n"
			 "  -l               enables logarithmic mode "
			 "(for high-bandwidth connections)\n"
			 "  -h               displays this screen\n");
		  exit(0);
	       default:
		  fprintf(stderr, "invalid option %c -- use -h for help\n",
			  argv[i][1]);
		  exit(1);
	       }
	  }
	else
	   i++;
     }

   in_color[0] = in_color_hex >> 16;
   in_color[1] = (in_color_hex >> 8) & 0xff;
   in_color[2] = in_color_hex & 0xff;
   out_color[0] = out_color_hex >> 16;
   out_color[1] = (out_color_hex >> 8) & 0xff;
   out_color[2] = out_color_hex & 0xff;
   bg_color[0] = bg_color_hex >> 16;
   bg_color[1] = (bg_color_hex >> 8) & 0xff;
   bg_color[2] = bg_color_hex & 0xff;

   Epplet_gadget_show(drawingarea = Epplet_create_drawingarea(2, 2, 44, 44));

   buf = Epplet_make_rgb_buf(40, 40);
   win = Epplet_get_drawingarea_window(drawingarea);
   rgb_pointer_start = Epplet_get_rgb_pointer(buf);

   close_button = Epplet_create_std_button("CLOSE", 2, 2, cb_close, NULL);
   cfg_button = Epplet_create_std_button("CONFIGURE", 33, 2, config_cb, NULL);
   Epplet_register_focus_in_handler(cb_in, NULL);
   Epplet_register_focus_out_handler(cb_out, NULL);
   Epplet_register_delete_event_handler(delete_cb, NULL);

   gettimeofday(&last_time, NULL);

   Epplet_timer(timer_draw, NULL, 0.0, "timer_draw");

   Epplet_show();
   Epplet_Loop();

   return (0);
}
