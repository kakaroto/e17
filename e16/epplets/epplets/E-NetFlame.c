/****************************************************************/
/* E-NetFlame v0.3                                              */
/*   author: Shawn M. Veader                                    */
/*   email: veader@cc.gatech.edu                                */
/****************************************************************/
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include "epplet.h"
#include "net.h"

double             *prev_val = NULL;
int                *load_val = NULL;
Window              win;
RGB_buf             buf;
Epplet_gadget       da, b_close, b_config, b_help, pop, pop2, text;
unsigned int       *flame = NULL;
int                *vspread, *hspread, *residual;
unsigned char       rm[255], gm[255], bm[255];
char               *netdev;
double              stream_max = 2000;
double              bandwidths[] = {
   1000000000, 100000000, 10000000, 2000000,
   1540000, 1000000, 512000, 256000,
   144000, 128000, 64000, 56000,
   33600, 28800, 14400, 9600,
   4800, 2400, 300, 75
};
static int          colors[] = {
   30, 90, 90,
   50, 255, 255,
   255, 255, 255,

   255, 0, 0,
   255, 255, 0,
   255, 255, 255,

   0, 255, 0,
   255, 255, 0,
   255, 255, 255,

   0, 0, 255,
   255, 0, 255,
   255, 255, 255,

   0, 0, 200,
   40, 80, 255,
   100, 200, 255,

   80, 90, 140,
   140, 150, 180,
   255, 230, 200,

   20, 40, 180,
   255, 160, 0,
   255, 255, 100
};

static void         save_conf(int d1, int d2, int d3, int d4, int d5,
			      int d6, int d7, int d8, int d9);
static void         load_conf(void);
static void         draw_flame(void);
static void         flame_col(int r1, int g1, int b1,
			      int r2, int g2, int b2, int r3, int g3, int b3);
static void         epplet_in(void *data, Window w);
static void         epplet_out(void *data, Window w);
static void         epplet_timer(void *data);
static void         epplet_close(void *data);
static void         epplet_config(void *data);
static void         epplet_help(void *data);
static void         epplet_color(void *data);
static void         epplet_bandwidth(void *data);

#define VARIANCE 40
#define VARTREND 16

#define HSPREAD  10
#define VSPREAD  160
#define RESIDUAL 75

#define DIVISIONS 2
#define WIDTH  40
#define HEIGHT 30

#define MAX      255

#define DOWN 0
#define UP   1

/* set the flame color */
static void
flame_col(int r1, int g1, int b1,
	  int r2, int g2, int b2, int r3, int g3, int b3)
{
   int                 i;

   for (i = 0; i < 25; i++)
     {
	rm[i] = (r1 * i) / 25;
	gm[i] = (g1 * i) / 25;
	bm[i] = (b1 * i) / 25;
     }
   for (i = 0; i < 25; i++)
     {
	rm[25 + i] = ((r2 * i) + (r1 * (25 - i))) / 25;
	gm[25 + i] = ((g2 * i) + (g1 * (25 - i))) / 25;
	bm[25 + i] = ((b2 * i) + (b1 * (25 - i))) / 25;
     }
   for (i = 0; i < 25; i++)
     {
	rm[50 + i] = ((r3 * i) + (r2 * (25 - i))) / 25;
	gm[50 + i] = ((g3 * i) + (g2 * (25 - i))) / 25;
	bm[50 + i] = ((b3 * i) + (b2 * (25 - i))) / 25;
     }

   for (i = 75; i < 255; i++)
     {
	rm[i] = rm[74];
	gm[i] = gm[74];
	bm[i] = bm[74];
     }
}

/* draw the flame to display */
static void
draw_flame(void)
{
   unsigned char      *rgb, *rptr;
   unsigned int       *ptr;
   int                 x, y, val1, val2, val3, i, j;

   /* initialize the flame if it isn't done already */
   if (!flame)
     {
	vspread = malloc(WIDTH * sizeof(int));
	hspread = malloc(WIDTH * sizeof(int));
	residual = malloc(WIDTH * sizeof(int));
	flame = calloc(WIDTH * HEIGHT, sizeof(unsigned int));
     }
   /* move to the bottom left of the drawing area */
   ptr = flame + ((HEIGHT - 1) * WIDTH);
   /* scan along the bottom row of pixels to start flames */
   for (x = 0; x < WIDTH; x++)
     {
	/* adjust spreads and residual values to reflect the
	 * load values... */
	vspread[x] = VSPREAD + (load_val[(x * DIVISIONS) / WIDTH] / 50);
	hspread[x] = HSPREAD + (load_val[(x * DIVISIONS) / WIDTH] / 50);
	residual[x] = RESIDUAL + (load_val[(x * DIVISIONS) / WIDTH] / 50);
	/* assign a random value to the pixel according to the
	 * load ... gives randomness to flames */
	ptr[x] = (rand() % ((load_val[(x * DIVISIONS) / WIDTH]) + 155));
	/* bounds checking */
	if (ptr[x] > MAX)
	  {
	     ptr[x] = 0;
	  }
	else if (ptr[x] < 0)
	  {
	     ptr[x] = 0;
	  }
     }

   /* this divides up the chart into multiple sections
    * depending on the cpus value */
   for (i = 0; i < DIVISIONS; i++)
     {
	for (x = (WIDTH / (DIVISIONS * 2)) - 1 + (i * (WIDTH / DIVISIONS));
	     x <= (WIDTH / (DIVISIONS * 2)) + 1 + (i * (WIDTH / DIVISIONS));
	     x++)
	  {
	     j = (load_val[i] * HEIGHT) / 100;
	     ptr = flame + ((HEIGHT - j) * (WIDTH)) + (x);
	     /* marches down a column increasing the intensity
	      * as you travel down */
	     for (y = 0; y < j; y++)
	       {
		  ptr[0] += ((y * 64 * load_val[i]) / (j * 100));
		  ptr += WIDTH;
	       }
	  }
     }
   /* ----- *
    * --A-- *
    * -BCD- *
    * ----- */
   for (y = HEIGHT - 1; y >= 2; y--)
     {
	ptr = flame + (y * 40);
	for (x = 1; x < WIDTH - 1; x++)
	  {
	     /* val1 = (C * vspread) / 256 */
	     val1 = (ptr[x] * vspread[x]) >> 8;
	     /* val2 = (C * hspread) / 256 */
	     val2 = (ptr[x] * hspread[x]) >> 8;
	     /* val3 = (C * residual) / 256 */
	     val3 = (ptr[x] * residual[x]) >> 8;

	     /* add val2 to B */
	     ptr[x - 1] += val2;
	     if (ptr[x - 1] > MAX)
		ptr[x - 1] = MAX;
	     /* add val1 to A */
	     ptr[x - WIDTH] += val1;
	     if (ptr[x - WIDTH] > MAX)
		ptr[x - WIDTH] = MAX;
	     /* add val2 to D */
	     ptr[x + 1] += val2;
	     if (ptr[x + 1] > MAX)
		ptr[x + 1] = MAX;
	     /* add val3 to C */
	     ptr[x] = val3;
	  }
     }

   /* blank the top row of pixels */
   for (x = 0; x < WIDTH; x++)
      flame[x] = 0;
   /* half the intensity of the second row */
   for (x = 0; x < WIDTH; x++)
      flame[WIDTH + x] /= 2;
   /* blank out left most column */
   for (y = 0; y < HEIGHT; y++)
      flame[y * WIDTH] = 0;
   /* half the intensity on second left most column */
   for (y = 0; y < HEIGHT; y++)
      flame[(y * WIDTH) + 1] /= 2;
   /* blank out right most column */
   for (y = 0; y < HEIGHT; y++)
      flame[(y * WIDTH) + (WIDTH - 1)] = 0;
   /* half the intensity of the second right most column */
   for (y = 0; y < HEIGHT; y++)
      flame[(y * WIDTH) + (WIDTH - 2)] /= 2;

   rgb = Epplet_get_rgb_pointer(buf);
   for (y = 0; y < HEIGHT; y++)
     {
	ptr = flame + (y * WIDTH);
	rptr = rgb + (y * WIDTH * 4);
	for (x = 0; x < WIDTH; x++)
	  {
	     val1 = ptr[x] & 0xff;
	     rptr[0] = rm[val1];
	     rptr[1] = gm[val1];
	     rptr[2] = bm[val1];
	     rptr += 4;
	  }
     }
}

/* handles the timer, check /proc/net/dev to establish
 * up and down load */
static void
epplet_timer(void *data)
{
   double              val = -1.0, val2 = -1.0, dval, dval2;
   unsigned char       invalid;

   invalid = net_get_bytes_inout(netdev, &val, &val2);
   if (invalid)
     {
	char                err[255];

	Esnprintf(err, sizeof(err),
		  "Unable to get network device statistics for %s:  %s", netdev,
		  net_strerror(invalid));
	Epplet_dialog_ok(err);
	Epplet_unremember();
	Esync();
	exit(-1);
     }
   if (val != -1.0)
     {
	dval = val - prev_val[DOWN];
	dval2 = val2 - prev_val[UP];
	prev_val[DOWN] = val;
	prev_val[UP] = val2;
	load_val[DOWN] = (int)((dval * 800 * 3) / stream_max);
	load_val[UP] = (int)((dval2 * 800 * 3) / stream_max);
	if (load_val[DOWN] > 100)
	   load_val[DOWN] = 100;
	if (load_val[UP] > 100)
	   load_val[UP] = 100;
     }
   draw_flame();
   Epplet_paste_buf(buf, win, 0, 0);
   Epplet_timer(epplet_timer, NULL, 0.1, "TIMER");
   return;
   data = NULL;
}

/* called when you close the epplet */
static void
epplet_close(void *data)
{
   Epplet_unremember();
   Esync();
   data = NULL;
   exit(0);
}

/* called when you select a color from the epplet color menu */
static void
epplet_color(void *data)
{
   int                *d;

   d = (int *)data;
   flame_col(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8]);
   save_conf(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8]);
   Epplet_gadget_hide(pop);
   Epplet_gadget_hide(pop2);
}

/* called when you select a bandwidth from the epplet bandwidth menu */
static void
epplet_bandwidth(void *data)
{
   double             *d;
   char                s[64];

   d = (double *)data;
   stream_max = d[0];
   Esnprintf(s, sizeof(s), "%f", stream_max);
   Epplet_modify_config("bandwidth", s);
   Epplet_save_config();
   Epplet_gadget_hide(pop);
   Epplet_gadget_hide(pop2);
}

/* called when the config button is pressed */
static void
epplet_config(void *data)
{
   data = NULL;
   Epplet_gadget_show(pop);
   Epplet_gadget_show(pop2);
}

/* called when the help button is pressed */
static void
epplet_help(void *data)
{
   data = NULL;
   Epplet_show_about("E-NetFlame");
}

/* called when the epplet gets the focus */
static void
epplet_in(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_show(b_close);
	Epplet_gadget_show(b_config);
	Epplet_gadget_show(b_help);
     }
   return;
   data = NULL;
}

/* called when the epplet loses the focus */
static void
epplet_out(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_hide(b_close);
	Epplet_gadget_hide(b_config);
	Epplet_gadget_hide(b_help);
     }
   return;
   data = NULL;
}

/* save off the color of the flame in a config file */
static void
save_conf(int d1, int d2, int d3, int d4, int d5,
	  int d6, int d7, int d8, int d9)
{
   char                s[1024];

   Esnprintf(s, sizeof(s), "%d %d %d", d1, d2, d3);
   Epplet_modify_config("color1", s);
   Esnprintf(s, sizeof(s), "%d %d %d", d4, d5, d6);
   Epplet_modify_config("color2", s);
   Esnprintf(s, sizeof(s), "%d %d %d", d7, d8, d9);
   Epplet_modify_config("color3", s);
   Esnprintf(s, sizeof(s), "%f", stream_max);
   Epplet_modify_config("bandwidth", s);
   Epplet_save_config();
}

/* load the color of the flame from a config file */
static void
load_conf(void)
{
   int                 d1, d2, d3, d4, d5, d6, d7, d8, d9;
   char               *str;

   str = Epplet_query_config_def("color1", "30 90 90");
   sscanf(str, "%d %d %d", &d1, &d2, &d3);
   str = Epplet_query_config_def("color2", "50 255 255");
   sscanf(str, "%d %d %d", &d4, &d5, &d6);
   str = Epplet_query_config_def("color3", "255 255 255");
   sscanf(str, "%d %d %d", &d7, &d8, &d9);
   str = Epplet_query_config_def("bandwidth", "2400");
   sscanf(str, "%lf", &stream_max);
#ifdef __sun__
   netdev = Epplet_query_config_def("device", "hme0");
#else
   netdev = Epplet_query_config_def("device", "eth0");
#endif
   flame_col(d1, d2, d3, d4, d5, d6, d7, d8, d9);
}

/* where it all begins... */
int
main(int argc, char **argv)
{
   Epplet_gadget       p;
   int                 prio;

   prio = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), prio + 10);
   atexit(Epplet_cleanup);

   load_val = calloc(DIVISIONS, sizeof(int));
   prev_val = calloc(DIVISIONS, sizeof(double));

   Epplet_Init("E-NetFlame", "0.3", "E Net-Flame Epplet", 3, 3, argc, argv, 0);
   Epplet_load_config();
   Epplet_timer(epplet_timer, NULL, 0.1, "TIMER");
   Epplet_gadget_show(da =
		      Epplet_create_drawingarea(2, 2, (WIDTH + 4),
						(HEIGHT + 4)));
   win = Epplet_get_drawingarea_window(da);
   buf = Epplet_make_rgb_buf(WIDTH, HEIGHT);
   Epplet_gadget_show(text = Epplet_create_label(8, 36, "IN   OUT", 1));
   b_close = Epplet_create_button(NULL, NULL,
				  3, 3, 0, 0, "CLOSE", 0, NULL,
				  epplet_close, NULL);
   b_help = Epplet_create_button(NULL, NULL,
				 17, 3, 0, 0, "HELP", 0, NULL,
				 epplet_help, NULL);
   b_config = Epplet_create_button(NULL, NULL,
				   31, 3, 0, 0, "CONFIGURE", 0, NULL,
				   epplet_config, NULL);
   p = Epplet_create_popup();
   Epplet_add_popup_entry(p, "Turquoise", NULL, epplet_color,
			  (void *)(&(colors[0 * 9])));
   Epplet_add_popup_entry(p, "Fire     ", NULL, epplet_color,
			  (void *)(&(colors[1 * 9])));
   Epplet_add_popup_entry(p, "Copper", NULL, epplet_color,
			  (void *)(&(colors[2 * 9])));
   Epplet_add_popup_entry(p, "Violet", NULL, epplet_color,
			  (void *)(&(colors[3 * 9])));
   Epplet_add_popup_entry(p, "Night", NULL, epplet_color,
			  (void *)(&(colors[4 * 9])));
   Epplet_add_popup_entry(p, "Sunrise", NULL, epplet_color,
			  (void *)(&(colors[5 * 9])));
   Epplet_add_popup_entry(p, "Sunset", NULL, epplet_color,
			  (void *)(&(colors[6 * 9])));
   pop = Epplet_create_popupbutton("Colors", NULL, 5, 24, 36, 12, NULL, p);
   p = Epplet_create_popup();
   Epplet_add_popup_entry(p, "1 Gbit", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[0])));
   Epplet_add_popup_entry(p, "100 Mbit", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[1])));
   Epplet_add_popup_entry(p, "10 Mbit", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[2])));
   Epplet_add_popup_entry(p, "2 Mbit", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[3])));
   Epplet_add_popup_entry(p, "1.54 Mbit T1", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[4])));
   Epplet_add_popup_entry(p, "1 Mbit", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[5])));
   Epplet_add_popup_entry(p, "512 Kbit", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[6])));
   Epplet_add_popup_entry(p, "256 Kbit", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[7])));
   Epplet_add_popup_entry(p, "144 Kbit", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[8])));
   Epplet_add_popup_entry(p, "128 Kbit ISDN", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[9])));
   Epplet_add_popup_entry(p, "64 Kbit ISDN", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[10])));
   Epplet_add_popup_entry(p, "56 Kbit Modem", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[11])));
   Epplet_add_popup_entry(p, "33.6 Kbit Modem", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[12])));
   Epplet_add_popup_entry(p, "28.8 Kbit Modem", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[13])));
   Epplet_add_popup_entry(p, "14.4 Kbit Modem", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[14])));
   Epplet_add_popup_entry(p, "9600 baud Modem", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[15])));
   Epplet_add_popup_entry(p, "4800 baud Modem", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[16])));
   Epplet_add_popup_entry(p, "2400 baud Modem", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[17])));
   Epplet_add_popup_entry(p, "300 baud Modem", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[18])));
   Epplet_add_popup_entry(p, "75 baud Modem", NULL, epplet_bandwidth,
			  (void *)(&(bandwidths[19])));
   pop2 = Epplet_create_popupbutton("Bandwidth", NULL, 2, 14, 44, 12, NULL, p);
   Epplet_register_focus_in_handler(epplet_in, NULL);
   Epplet_register_focus_out_handler(epplet_out, NULL);
   load_conf();
   Epplet_show();
   Epplet_Loop();
   return 0;
}
