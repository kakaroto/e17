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
#include <glibtop/cpu.h>
#include "proc.h"
#endif

int                 cpus = 0;
double             *prev_val = NULL;
double             *prev_val_nice = NULL;
int                *load_val = NULL;
Window              win;
RGB_buf             buf;
Epplet_gadget       da, b_close, b_config, b_help, b_nice, pop;
unsigned int       *flame = NULL;
int                *vspread, *hspread, *residual;
unsigned char       rm[255], gm[255], bm[255];
int                 include_nice = 0;

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

static void         save_conf(int d1, int d2, int d3, int d4, int d5, int d6,
			      int d7, int d8, int d9);
static void         load_conf(void);
static void         cb_in(void *data, Window w);
static void         cb_out(void *data, Window w);
static void         cb_timer(void *data);
static void         cb_close(void *data);
static void         cb_config(void *data);
static void         cb_help(void *data);
static int          count_cpus(void);
static void         draw_flame(void);
static void         flame_col(int r1, int g1, int b1, int r2, int g2, int b2,
			      int r3, int g3, int b3);

#define VARIANCE 40
#define VARTREND 16

#define HSPREAD  10
#define VSPREAD  160
#define RESIDUAL 75

#define MAX      255

static void
flame_col(int r1, int g1, int b1, int r2, int g2, int b2, int r3, int g3,
	  int b3)
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

static void
draw_flame(void)
{
   unsigned char      *rgb, *rptr;
   unsigned int       *ptr;
   int                 x, y, val1, val2, val3, i, j;

   if (!flame)
     {
	vspread = malloc(40 * sizeof(int));
	hspread = malloc(40 * sizeof(int));
	residual = malloc(40 * sizeof(int));
	flame = calloc(40 * 40, sizeof(unsigned int));
     }

   ptr = flame + (39 * 40);
   for (x = 0; x < 40; x++)
     {
	vspread[x] = VSPREAD + (load_val[(x * cpus) / 40] / 50);
	hspread[x] = HSPREAD + (load_val[(x * cpus) / 40] / 50);
	residual[x] = RESIDUAL + (load_val[(x * cpus) / 40] / 50);
	ptr[x] = (rand() % ((load_val[(x * cpus) / 40]) + 155));
	if (ptr[x] > MAX)
	   ptr[x] = 0;
	else if (ptr[x] < 0)
	   ptr[x] = 0;
     }

   for (i = 0; i < cpus; i++)
     {
	for (x = (40 / (cpus * 2)) - 1 + (i * (40 / cpus));
	     x <= (40 / (cpus * 2)) + 1 + (i * (40 / cpus)); x++)
	  {
	     j = (load_val[i] * 40) / 100;
	     ptr = flame + ((40 - j) * (40)) + (x);
	     for (y = 0; y < j; y++)
	       {
		  ptr[0] += ((y * 64 * load_val[i]) / (j * 100));
		  ptr += 40;
	       }
	  }
     }
   for (y = 39; y >= 2; y--)
     {
	ptr = flame + (y * 40);
	for (x = 1; x < 39; x++)
	  {
	     val1 = (ptr[x] * vspread[x]) >> 8;
	     val2 = (ptr[x] * hspread[x]) >> 8;
	     val3 = (ptr[x] * residual[x]) >> 8;

	     ptr[x - 1] += val2;
	     if (ptr[x - 1] > MAX)
		ptr[x - 1] = MAX;
	     ptr[x - 40] += val1;
	     if (ptr[x - 40] > MAX)
		ptr[x - 40] = MAX;
	     ptr[x + 1] += val2;
	     if (ptr[x + 1] > MAX)
		ptr[x + 1] = MAX;
	     ptr[x] = val3;
	  }
     }
   for (x = 0; x < 40; x++)
      flame[x] = 0;
   for (x = 0; x < 40; x++)
      flame[40 + x] /= 2;
   for (y = 0; y < 40; y++)
      flame[y * 40] = 0;
   for (y = 0; y < 40; y++)
      flame[(y * 40) + 39] = 0;
   for (y = 0; y < 40; y++)
      flame[(y * 40) + 38] /= 2;

   rgb = Epplet_get_rgb_pointer(buf);
   for (y = 0; y < 40; y++)
     {
	ptr = flame + (y * 40);
	rptr = rgb + (y * 40 * 4);
	for (x = 0; x < 40; x++)
	  {
	     val1 = ptr[x] & 0xff;
	     rptr[0] = rm[val1];
	     rptr[1] = gm[val1];
	     rptr[2] = bm[val1];
	     rptr += 4;
	  }
     }
}

static void
cb_timer(void *data)
{
#ifdef HAVE_LIBGTOP

   glibtop_cpu         cpu;
   double              val, val2;
   int                 i;

   glibtop_get_cpu(&cpu);

   for (i = 0; i < cpus; i++)
     {
	val = (double)(cpu.xcpu_user[i] + cpu.xcpu_nice[i] + cpu.xcpu_sys[i]);
	if (prev_val[i] == 0)
	   prev_val[i] = val;
	val2 = (val - prev_val[i]);
	prev_val[i] = val;
	val2 *= 10;
	if (val2 > 100)
	   val2 = 100;
	load_val[i] = val2;
	/*      printf ("CPU%d: %ld : %ld : %d : %d\n",i, val, prev_val[i], val2, load_val[i]); */
     }

#else

   static FILE        *f;
   int                 i;

   f = fopen("/proc/stat", "r");
   if (f)
     {
	char                s[256];

	if (cpus > 1)
	   fgets(s, 255, f);
	for (i = 0; i < cpus; i++)
	  {
	     char                sUserCPU[64];
	     char                sNiceCPU[64];
	     double              val, val2, val_nice, val2_nice;

	     fgets(s, 255, f);
	     sscanf(s, "%*s %s %s %*s %*s", sUserCPU, sNiceCPU);

	     val = atof(sUserCPU);
	     val_nice = atof(sNiceCPU);

	     val2 = val - prev_val[i];
	     prev_val[i] = val;
	     val2 *= 10;
	     if (val2 > 100)
		val2 = 100;

	     val2_nice = val_nice - prev_val_nice[i];
	     prev_val_nice[i] = val_nice;
	     val2_nice *= 10;
	     if (val2_nice > 100)
		val2_nice = 100;

	     if (include_nice)
		load_val[i] = val2 + val2_nice;
	     else
		load_val[i] = val2;

	     if (load_val[i] > 100)
		load_val[i] = 100;

	  }
	fclose(f);
     }

#endif

   draw_flame();
   Epplet_paste_buf(buf, win, 0, 0);
   Epplet_timer(cb_timer, NULL, 0.1, "TIMER");
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
cb_color(void *data)
{
   int                *d;

   d = (int *)data;
   flame_col(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8]);
   save_conf(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8]);
   Epplet_gadget_hide(pop);
}

static void
cb_config(void *data)
{
   data = NULL;
   Epplet_gadget_show(pop);
}

static void
cb_help(void *data)
{
   data = NULL;
   Epplet_show_about("E-Cpu");
}

static void
cb_in(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_show(b_close);
	Epplet_gadget_show(b_config);
	Epplet_gadget_show(b_help);
	Epplet_gadget_show(b_nice);
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
	Epplet_gadget_hide(b_config);
	Epplet_gadget_hide(b_help);
	Epplet_gadget_hide(b_nice);
     }
   return;
   data = NULL;
}

static void
toggle_nice(void *data)
{
   char                s[10];

   sprintf(s, "%d", include_nice);
   Epplet_modify_config("nice", s);
   Epplet_save_config();
   return;
   data = NULL;
}

static int
count_cpus(void)
{
#ifdef HAVE_LIBGTOP
   int                 i, c = 0;
   int                 bits;
   glibtop_cpu         cpu;

   glibtop_get_cpu(&cpu);
   bits = (int)cpu.xcpu_flags;
   for (i = 0; i < GLIBTOP_NCPU; i++)
     {
	c += bits & 1;
	/*      printf ("%d: %o - %d\n",i,bits,c ); */
	bits >>= 1;
     }
   /* printf ("CPUs: %d\n", c); */

   return c;
#else
   FILE               *f;
   char                s[256];

   f = fopen("/proc/stat", "r");
   if (f)
     {
	int                 count = 0;
	char                ok = 1;

	while (ok)
	  {
	     if (!fgets(s, 255, f))
		ok = 0;
	     else
	       {
		  if (strncmp(s, "cpu", 3))
		     ok = 0;
		  else
		     count++;
	       }
	  }
	if (count > 1)
	   count--;
	fclose(f);
	return count;
     }
   exit(1);
#endif
}

static void
save_conf(int d1, int d2, int d3, int d4, int d5, int d6, int d7, int d8,
	  int d9)
{
   char                s[1024];

   Esnprintf(s, sizeof(s), "%d %d %d", d1, d2, d3);
   Epplet_modify_config("color1", s);
   Esnprintf(s, sizeof(s), "%d %d %d", d4, d5, d6);
   Epplet_modify_config("color2", s);
   Esnprintf(s, sizeof(s), "%d %d %d", d7, d8, d9);
   Epplet_modify_config("color3", s);
   sprintf(s, "%d", include_nice);
   Epplet_modify_config("nice", s);
   Epplet_save_config();
}

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
   flame_col(d1, d2, d3, d4, d5, d6, d7, d8, d9);
   str = Epplet_query_config_def("nice", "0");
   sscanf(str, "%d", &include_nice);
}

int
main(int argc, char **argv)
{
   Epplet_gadget       p;
   int                 prio;

   prio = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), prio + 10);
   atexit(Epplet_cleanup);

   cpus = count_cpus();
   load_val = calloc(cpus, sizeof(int));
   prev_val = calloc(cpus, sizeof(double));
   prev_val_nice = calloc(cpus, sizeof(double));

   Epplet_Init("E-Cpu", "0.1", "Enlightenment CPU Epplet", 3, 3, argc, argv, 0);
   Epplet_load_config();
   Epplet_timer(cb_timer, NULL, 0.1, "TIMER");
   Epplet_gadget_show(da = Epplet_create_drawingarea(2, 2, 44, 44));
   win = Epplet_get_drawingarea_window(da);
   buf = Epplet_make_rgb_buf(40, 40);
   b_close = Epplet_create_button(NULL, NULL,
				  0, 0, 0, 0, "CLOSE", win, NULL,
				  cb_close, NULL);
   b_config = Epplet_create_button(NULL, NULL,
				   28, 0, 0, 0, "CONFIGURE", win, NULL,
				   cb_config, NULL);
   b_help = Epplet_create_button(NULL, NULL,
				 14, 0, 0, 0, "HELP", win, NULL, cb_help, NULL);
   b_nice = Epplet_create_togglebutton("N", NULL,
				       32, 32, 13, 13, &include_nice,
				       toggle_nice, NULL);
   p = Epplet_create_popup();
   Epplet_add_popup_entry(p, "Turquoise", NULL, cb_color,
			  (void *)(&(colors[0 * 9])));
   Epplet_add_popup_entry(p, "Fire", NULL, cb_color,
			  (void *)(&(colors[1 * 9])));
   Epplet_add_popup_entry(p, "Copper", NULL, cb_color,
			  (void *)(&(colors[2 * 9])));
   Epplet_add_popup_entry(p, "Violet", NULL, cb_color,
			  (void *)(&(colors[3 * 9])));
   Epplet_add_popup_entry(p, "Night", NULL, cb_color,
			  (void *)(&(colors[4 * 9])));
   Epplet_add_popup_entry(p, "Sunrise", NULL, cb_color,
			  (void *)(&(colors[5 * 9])));
   Epplet_add_popup_entry(p, "Sunset", NULL, cb_color,
			  (void *)(&(colors[6 * 9])));
   pop = Epplet_create_popupbutton("Colors", NULL, 6, 24, 36, 12, NULL, p);
   Epplet_register_focus_in_handler(cb_in, NULL);
   Epplet_register_focus_out_handler(cb_out, NULL);
   load_conf();
   Epplet_show();
   Epplet_Loop();
   return 0;
}
