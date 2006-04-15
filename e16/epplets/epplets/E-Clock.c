#include "epplet.h"
Epplet_gadget       b_close, b_help;
Window              win;
double              minutes = 20, hours = 12, seconds = 0;

static void         cb_timer(void *data);
static void         cb_close(void *data);
static void         cb_in(void *data, Window w);
static void         cb_out(void *data, Window w);
static void         cb_expose(void *data, Window win, int x, int y, int w,
			      int h);
static void         draw_arms(void);
static void         cb_help(void *data);

static void
cb_timer(void *data)
{
   struct tm           tim;
   struct tm          *tim2;
   time_t              t2;
   int                 h, m, s;

   t2 = time(NULL);
   tim2 = localtime(&t2);
   if (tim2)
     {
	memcpy(&tim, tim2, sizeof(struct tm));
	h = tim.tm_hour;
	m = tim.tm_min;
	s = tim.tm_sec;
	hours = (double)h + ((double)m / 60) + ((double)s / 3600);
	minutes = (double)m + ((double)s / 60);
	seconds = (double)s;
     }
   draw_arms();
   Epplet_timer(cb_timer, NULL, 0.5, "TIMER");
   data = NULL;
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
	Epplet_gadget_show(b_close);
	Epplet_gadget_show(b_help);
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
     }
   return;
   data = NULL;
}

static void
cb_expose(void *data, Window win, int x, int y, int w, int h)
{
   draw_arms();
   return;
   data = NULL;
   win = (Window) 0;
   x = y = w = h = 0;
}

static void
cb_help(void *data)
{
   Epplet_show_about("E-Clock");
   return;
   data = NULL;
}

static void
draw_arms(void)
{
   int                 x, y;

   Epplet_clear_window(win);
   x = 24 + (int)(sin((seconds * 3.14159 * 2) / 60) * 19);
   y = 24 - (int)(cos((seconds * 3.14159 * 2) / 60) * 19);
   Epplet_draw_line(win, 24, 24, x, y, 70, 70, 70);
   x = 24 + (int)(sin((minutes * 3.14159 * 2) / 60) * 18);
   y = 24 - (int)(cos((minutes * 3.14159 * 2) / 60) * 18);
   Epplet_draw_line(win, 24, 24, x, y, 70, 70, 70);
   x = 24 + (int)(sin((hours * 3.14159 * 2) / 12) * 12);
   y = 24 - (int)(cos((hours * 3.14159 * 2) / 12) * 12);
   Epplet_draw_line(win, 24, 24, x, y, 70, 70, 70);
   x = 23 + (int)(sin((seconds * 3.14159 * 2) / 60) * 19);
   y = 23 - (int)(cos((seconds * 3.14159 * 2) / 60) * 19);
   Epplet_draw_line(win, 23, 23, x, y, 200, 50, 50);
   x = 23 + (int)(sin((minutes * 3.14159 * 2) / 60) * 18);
   y = 23 - (int)(cos((minutes * 3.14159 * 2) / 60) * 18);
   Epplet_draw_line(win, 23, 23, x, y, 0, 0, 0);
   x = 23 + (int)(sin((hours * 3.14159 * 2) / 12) * 12);
   y = 23 - (int)(cos((hours * 3.14159 * 2) / 12) * 12);
   Epplet_draw_line(win, 23, 23, x, y, 0, 0, 0);
   Esync();
}

int
main(int argc, char **argv)
{
   atexit(Epplet_cleanup);
   Epplet_Init("E-Clock", "0.1", "Enlightenment Clock Epplet",
	       3, 3, argc, argv, 0);
   Epplet_timer(cb_timer, NULL, 0.5, "TIMER");
   b_close = Epplet_create_button(NULL, NULL,
				  2, 2, 0, 0, "CLOSE", 0, NULL, cb_close, NULL);
   b_help = Epplet_create_button(NULL, NULL,
				 34, 2, 0, 0, "HELP", win, NULL, cb_help, NULL);
   Epplet_gadget_show(Epplet_create_image(2, 2, 44, 44, "E-Clock-Image.png"));
   Epplet_register_focus_in_handler(cb_in, NULL);
   Epplet_register_focus_out_handler(cb_out, NULL);
   Epplet_register_expose_handler(cb_expose, NULL);
   win = Epplet_get_main_window();
   Epplet_show();
   Epplet_Loop();
   return 0;
}
