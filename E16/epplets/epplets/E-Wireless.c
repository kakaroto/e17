/*
 *  E-Wireless.c - 802.11 Signal quality montioring epplet
 *  copyleft (C) Mark C. Langston 1999
 *  Damn near all the code originally from: cocidius@telebot.net
 *   and his E-CpuX Epplet.
 */

#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <epplet.h>

#define HEIGHT 30
#define WIDTH  40

Window              win;
RGB_buf             buf;
Epplet_gadget       da, btn_close, btn_config, btn_about;
Epplet_gadget       lbl_cpu, lbl_usage, popup;
int                 num_cpus = 0;
int                 cur_cpu = 0;
int                 cpu_hist_size = 0;
int                *cpu_hist = NULL;
int                 hist_pos = 0;
int                 color = 0;
unsigned long       prev_total = 0;
unsigned long       prev_out = 0;
unsigned long       prev_in = 0;

unsigned char       colors[7][3][3] = {
   {{0x00, 0x23, 0x00}, {0x00, 0x50, 0x00}, {0x00, 0xff, 0x00}},	//green
   {{0x00, 0x00, 0x23}, {0x00, 0x00, 0x50}, {0x00, 0x00, 0xff}},	//blue
   {{0x23, 0x00, 0x00}, {0x50, 0x00, 0x00}, {0xff, 0x00, 0x00}},	//red
   {{0x23, 0x23, 0x00}, {0x50, 0x50, 0x00}, {0xff, 0xff, 0x00}},	//yellow
   {{0x00, 0x23, 0x23}, {0x00, 0x50, 0x50}, {0x00, 0xff, 0xff}},	//cyan
   {{0x23, 0x00, 0x23}, {0x50, 0x00, 0x50}, {0xff, 0x00, 0xff}},	//magenta
   {{0x23, 0x23, 0x23}, {0x50, 0x50, 0x50}, {0xff, 0xff, 0xff}},	//gray
};

static void         cb_timer(void *data);
static void         cb_in(void *data, Window w);
static void         cb_out(void *data, Window w);
static void         cb_close(void *data);
static void         cb_config(void *data);
static void         cb_color(void *data);
static void         cb_about(void *data);
int                 count_cpus(void);

void
draw_line(unsigned char *rgb, int x1, int y1, int x2, int y2,
	  unsigned char *clr)
{
   unsigned char      *rptr;
   int                 i, steep = 0, sx, sy, dx, dy, e;

   dx = abs(x2 - x1);
   sx = ((x2 - x1) > 0) ? 1 : -1;
   dy = abs(y2 - y1);
   sy = ((y2 - y1) > 0) ? 1 : -1;

   if (dy > dx)
     {
	steep = 1;
	x1 ^= y1;
	y1 ^= x1;
	x1 ^= y1;
	dx ^= dy;
	dy ^= dx;
	dx ^= dy;
	sx ^= sy;
	sy ^= sx;
	sx ^= sy;
     }

   e = 2 * dy - dx;

   for (i = 0; i < dx; i++)
     {
	if (steep)
	  {
	     rptr = rgb + (((x1 * WIDTH) + y1) * 4);
	  }
	else
	  {
	     rptr = rgb + (((y1 * WIDTH) + x1) * 4);
	  }
	rptr[0] = clr[0];
	rptr[1] = clr[1];
	rptr[2] = clr[2];
	while (e >= 0)
	  {
	     y1 += sy;
	     e -= 2 * dx;
	  }
	x1 += sx;
	e += 2 * dy;
     }

   rptr = rgb + (((y2 * WIDTH) + x2) * 4);
   rptr[0] = clr[0];
   rptr[1] = clr[1];
   rptr[2] = clr[2];
}

void
draw_graph(void)
{
   unsigned char      *rgb, *rptr;
   int                 i, j, x, y, y1;

   rgb = Epplet_get_rgb_pointer(buf);

   for (y = 0; y < HEIGHT; y++)
     {
	rptr = rgb + (y * WIDTH * 4);
	for (x = 0; x < WIDTH; x++)
	  {
	     rptr[0] = colors[color][0][0];
	     rptr[1] = colors[color][0][1];
	     rptr[2] = colors[color][0][2];
	     rptr += 4;
	  }
     }

   draw_line(rgb, 0, 6, WIDTH - 1, 6, &colors[color][1][0]);
   draw_line(rgb, 0, 14, WIDTH - 1, 14, &colors[color][1][0]);
   draw_line(rgb, 0, 22, WIDTH - 1, 22, &colors[color][1][0]);

   draw_line(rgb, 9, 0, 9, HEIGHT - 1, &colors[color][1][0]);
   draw_line(rgb, 19, 0, 19, HEIGHT - 1, &colors[color][1][0]);
   draw_line(rgb, 29, 0, 29, HEIGHT - 1, &colors[color][1][0]);

   j = ((hist_pos + 1) % WIDTH);
   y = HEIGHT - ((HEIGHT * cpu_hist[j]) / 100);
   for (i = 0; i < WIDTH - 1; i++)
     {
	j++;
	j %= WIDTH;
	y1 = HEIGHT - ((HEIGHT * cpu_hist[j]) / 100);
	if (y1 >= HEIGHT)
	   y1 = HEIGHT - 1;
	if (y1 < 0)
	   y1 = 0;
	draw_line(rgb, i, y, i + 1, y1, &colors[color][2][0]);
	y = y1;
     }
}

static void
cb_timer(void *data)
{
   FILE               *fpStat;
   int                 percent;
   double              sigstr;
   char                s[256], ss[64];
   unsigned long       l0;

   fpStat = fopen("/proc/net/wireless", "r");
   if (fpStat)
     {

	fgets(s, sizeof(s), fpStat);
	fgets(s, sizeof(s), fpStat);
	fgets(s, sizeof(s), fpStat);

	sscanf(s, "%*s %*s %s %*s %*s %*s %*s %*s", ss);

	sigstr = atof(ss);

	l0 = sigstr;
	percent = (100 * l0 / 92);
	if (percent > 100)
	   percent = 100;

	cpu_hist[hist_pos] = percent;
	sprintf(ss, "%d%%", percent);
	Epplet_change_label(lbl_usage, ss);

	fclose(fpStat);
	draw_graph();
	hist_pos++;
	hist_pos %= WIDTH;
	Epplet_paste_buf(buf, win, 0, 0);
	Epplet_timer(cb_timer, NULL, 1, "TIMER");
     }
   data = NULL;
}

static void
cb_in(void *data, Window w)
{
   Epplet_gadget_show(btn_close);
   Epplet_gadget_show(btn_config);
   Epplet_gadget_show(btn_about);
}

static void
cb_out(void *data, Window w)
{
   Epplet_gadget_hide(btn_close);
   Epplet_gadget_hide(btn_config);
   Epplet_gadget_hide(btn_about);
}

static void
cb_close(void *data)
{
   Epplet_unremember();
   Esync();
   data = NULL;
   free(cpu_hist);
   exit(0);
}

static void
cb_about(void *data)
{
   data = NULL;
   Epplet_show_about("E-Wireless");
}

static void
cb_config(void *data)
{
   data = NULL;
   Epplet_gadget_show(popup);
}

static void
cb_color(void *data)
{
   char                s[2] = { 0, 0 };

   color = (int)data;
   s[0] = '0' + color;
   Epplet_modify_config("color", s);
   Epplet_save_config();
   Epplet_gadget_hide(popup);
}

int
main(int argc, char *argv[])
{
   char                s[64], *ptr;
   int                 priority;
   Epplet_gadget       p;

   priority = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), priority + 10);
   atexit(Epplet_cleanup);

   cpu_hist_size = WIDTH;
   cpu_hist = (int *)malloc(sizeof(int) * cpu_hist_size);
   memset(cpu_hist, 0x00, sizeof(int) * cpu_hist_size);
   hist_pos = 0;

   Epplet_Init("E-Wireless", "0.1", "E 802.11 signal monitoring epplet",
	       3, 3, argc, argv, 0);

   Epplet_load_config();
   Epplet_timer(cb_timer, NULL, 1, "TIMER");

   Epplet_gadget_show(da =
		      Epplet_create_drawingarea(2, 2, WIDTH + 4, HEIGHT + 4));
   win = Epplet_get_drawingarea_window(da);
   buf = Epplet_make_rgb_buf(WIDTH, HEIGHT);

   btn_close = Epplet_create_button(NULL, NULL, 0, 0, 0, 0, "CLOSE", win, NULL,
				    cb_close, NULL);
   btn_config =
      Epplet_create_button(NULL, NULL, 28, 0, 0, 0, "CONFIGURE", win, NULL,
			   cb_config, NULL);
   btn_about =
      Epplet_create_button(NULL, NULL, 14, 0, 0, 0, "HELP", win, NULL, cb_about,
			   NULL);

   sprintf(s, "Signal:");
   Epplet_gadget_show(lbl_cpu = Epplet_create_label(2, 36, s, 1));
   Epplet_gadget_show(lbl_usage = Epplet_create_label(-3, 36, "0%", 1));

   p = Epplet_create_popup();
   Epplet_add_popup_entry(p, "Green", NULL, cb_color, (void *)0L);
   Epplet_add_popup_entry(p, "Blue", NULL, cb_color, (void *)1L);
   Epplet_add_popup_entry(p, "Red", NULL, cb_color, (void *)2L);
   Epplet_add_popup_entry(p, "Yellow", NULL, cb_color, (void *)3L);
   Epplet_add_popup_entry(p, "Cyan", NULL, cb_color, (void *)4L);
   Epplet_add_popup_entry(p, "Magenta", NULL, cb_color, (void *)5L);
   Epplet_add_popup_entry(p, "Gray", NULL, cb_color, (void *)6L);
   popup = Epplet_create_popupbutton("Colors", NULL, 6, 24, 36, 12, NULL, p);

   Epplet_register_focus_in_handler(cb_in, NULL);
   Epplet_register_focus_out_handler(cb_out, NULL);

   ptr = Epplet_query_config_def("color", "0");
   color = *ptr - '0';

   Epplet_show();
   Epplet_Loop();

   return (0);
}
