#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <epplet.h>
#include "cloak.h"

double bands[] = 
{1000000000, 100000000, 10000000, 2000000, 1540000, 1000000, 512000, 256000,
   144000, 128000, 64000, 56000, 33600, 28800, 14400, 9600, 4800, 2400, 300, 75
};

RGB_buf buf;

double upstream_max = 1540000;
double downstream_max = 1540000;
int up_val = 0;
int down_val = 0;
double up_last = 0;
double down_last = 0;
Epplet_gadget load_up;
Epplet_gadget load_down;
Epplet_gadget net_graph;

static void cb_timer(void *data);
static void cb_close(void *data);
static void cb_set_upstream(void *data);
static void cb_set_downstream(void *data);
static void save_conf(void);
static void load_conf(void);

static void
cb_timer(void *data)
{
   static FILE *f;
   unsigned char *rgb, *rptr;

   f = fopen("/proc/net/dev", "r");
   if (f)
     {
	char s[256], ss[32];
	double val, val2, dval, dval2;
	char ok = 1;
	
	fgets(s, 255, f);
	fgets(s, 255, f);

	while(ok)
	  {
	     if (!fgets(s, 255, f))
		ok = 0;
	     else
	       {
		  char *sp, s1[64],  s2[64];
		  
		  sp = strchr(s, ':');
		  if (sp)
		     *sp = ' ';
		  val = 0;
		  val2 = 0;
		  sscanf(s, "%s %s %*s %*s %*s %*s %*s %*s %*s %s %*s %*s %*s %*s %*s %*s %*s", 
			 ss, s1, s2);
		  val = atof(s1);
		  val2 = atof(s2);
		  if (!strcmp(ss, "eth0"))
		    {
		       dval2 = val2 - down_last;
		       dval = val - up_last;
		       down_last = val2;
		       up_last = val;
		       down_val = (int)((dval2 * 800 * 3) / downstream_max);
		       up_val = (int)((dval * 800 * 3) / upstream_max);
		       if (down_val > 40)
			  down_val = 40;
		       if (up_val > 40)
			  up_val = 40;
//		       Epplet_gadget_data_changed(load_up);
//		       Epplet_gadget_data_changed(load_down);
		       ok = 0;
		    }
	       }
	  }
	fclose(f);

	scroll_buf();
	aa_line(39, 39, 39, 39-up_val, 255, 255, 0, 0);
	aa_line(39, 0, 39, down_val, 255, 128, 128, 128);
	Epplet_paste_buf(buf, Epplet_get_drawingarea_window(net_graph), 0, 0);

	Esync();
	Epplet_timer(cb_timer, NULL, 0.09, "TIMER");
     }
   data = NULL;
}

static void
save_conf(void)
{
   char s[1024];

   sprintf(s, "%f", upstream_max);
   Epplet_modify_config("upstream_max", s);
   sprintf(s, "%f", downstream_max);
   Epplet_modify_config("downstream_max", s);
   Epplet_save_config();
}

static void
load_conf(void)
{
   char *s;

   s = Epplet_query_config_def("upstream_max", "1540000");
   upstream_max = (double) atof(s);

   s = Epplet_query_config_def("downstream_max", "1540000");
   downstream_max = (double) atof(s);
}

static void
cb_set_upstream(void *data)
{
   upstream_max = *((double *)data);
   save_conf();
}

static void
cb_set_downstream(void *data)
{
   downstream_max = *((double *)data);
   save_conf();
}

static void
cb_close(void *data)
{
   Epplet_unremember();
   Epplet_cleanup();
   Esync();
   data = NULL;
   exit(0);
}

static void
got_focus(void *data, Window win)
{
  Epplet_gadget *g;

  g = (Epplet_gadget *)data;

  Epplet_gadget_show(*g);
  return;
}

static void
lost_focus(void *data, Window win)
{
  Epplet_gadget *g;

  g = (Epplet_gadget *)data;

  Epplet_gadget_hide(*g);
}

int
main(int argc, char **argv)
{
  Epplet_gadget p1, p2, close_button;   
   int prio;

   prio = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), prio + 10);
   
   atexit(Epplet_cleanup);
   
   Epplet_Init("E-NetGraph", "0.1", "Another Enlightenment Network Load Epplet",
	       3, 3, argc, argv, 0);
   Epplet_load_config();
   load_conf();

   buf = Epplet_make_rgb_buf(40, 40);
   Epplet_timer(cb_timer, NULL, 0.333, "TIMER");

   Epplet_gadget_show(net_graph = Epplet_create_drawingarea(2, 2, 44, 44));
   
   close_button = Epplet_create_button(NULL, NULL, 2, 2, 0, 0, 
				       "CLOSE", 0, NULL, cb_close, NULL);

   Epplet_register_focus_in_handler(got_focus, &close_button);
   Epplet_register_focus_out_handler(lost_focus, &close_button);
   
  

   //Epplet_gadget_show(load_up = Epplet_create_vbar(2, 19, 12, 46, 1, &up_val));
   //Epplet_gadget_show(load_down = Epplet_create_vbar(16, 19, 12, 46, 0, &down_val));
   //Epplet_gadget_show(Epplet_create_popupbutton("In", NULL, 1, 64, 16, 12, NULL, p1));
   //Epplet_gadget_show(Epplet_create_popupbutton("Out", NULL, 17, 64, 16, 12, NULL, p2));
   Epplet_show();
   Epplet_Loop();
   return 0;
}
