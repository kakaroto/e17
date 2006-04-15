#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include "epplet.h"
#include "net.h"

double              bands[] =
   { 1000000000, 100000000, 10000000, 2000000, 1540000, 1000000, 512000, 256000,
   144000, 128000, 64000, 56000, 33600, 28800, 14400, 9600, 4800, 2400, 300, 75
};

double              upstream_max = 1540000;
double              downstream_max = 1540000;
int                 up_val = 0;
int                 down_val = 0;
double              up_last = 0;
double              down_last = 0;
Epplet_gadget       load_up;
Epplet_gadget       load_down;

static void         cb_timer(void *data);
static void         cb_close(void *data);
static void         cb_set_upstream(void *data);
static void         cb_set_downstream(void *data);
static void         save_conf(void);
static void         load_conf(void);

static void
cb_timer(void *data)
{
   double              val = -1.0, val2 = -1.0, dval, dval2;
   unsigned char       invalid;

   invalid = net_get_bytes_inout("eth0", &val, &val2);
   if (invalid)
     {
	char                err[255];

	Esnprintf(err, sizeof(err),
		  "Unable to get network device statistics for eth0:  %s",
		  net_strerror(invalid));
	Epplet_dialog_ok(err);
	Epplet_unremember();
	Esync();
	exit(-1);
     }
   if (val != -1.0)
     {
	dval2 = val2 - down_last;
	dval = val - up_last;
	down_last = val2;
	up_last = val;
	down_val = (int)((dval2 * 800 * 3) / downstream_max);
	up_val = (int)((dval * 800 * 3) / upstream_max);
	if (down_val > 100)
	   down_val = 100;
	if (up_val > 100)
	   up_val = 100;
	Epplet_gadget_data_changed(load_up);
	Epplet_gadget_data_changed(load_down);
	Esync();
     }
   Epplet_timer(cb_timer, NULL, 0.333, "TIMER");
   return;
   data = NULL;
}

static void
save_conf(void)
{
   char                s[1024];

   Esnprintf(s, sizeof(s), "%f", upstream_max);
   Epplet_modify_config("upstream_max", s);
   Esnprintf(s, sizeof(s), "%f", downstream_max);
   Epplet_modify_config("downstream_max", s);
   Epplet_save_config();
}

static void
load_conf(void)
{
   char               *s;

   s = Epplet_query_config_def("upstream_max", "1540000");
   upstream_max = (double)atof(s);

   s = Epplet_query_config_def("downstream_max", "1540000");
   downstream_max = (double)atof(s);
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
   Esync();
   data = NULL;
   exit(0);
}

int
main(int argc, char **argv)
{
   Epplet_gadget       p1, p2;
   int                 prio;

   prio = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), prio + 10);
   atexit(Epplet_cleanup);

   Epplet_Init("E-Net", "0.1", "Enlightenment Network Load Epplet",
	       5, 2, argc, argv, 0);
   Epplet_load_config();
   load_conf();

   Epplet_timer(cb_timer, NULL, 0.333, "TIMER");
   Epplet_gadget_show(Epplet_create_button(NULL, NULL,
					   2, 2, 0, 0, "CLOSE", 0, NULL,
					   cb_close, NULL));
   p1 = Epplet_create_popup();
   Epplet_add_popup_entry(p1, "1 Gbit", NULL, cb_set_upstream,
			  (void *)(&(bands[0])));
   Epplet_add_popup_entry(p1, "100 Mbit", NULL, cb_set_upstream,
			  (void *)(&(bands[1])));
   Epplet_add_popup_entry(p1, "10 Mbit", NULL, cb_set_upstream,
			  (void *)(&(bands[2])));
   Epplet_add_popup_entry(p1, "2 Mbit", NULL, cb_set_upstream,
			  (void *)(&(bands[3])));
   Epplet_add_popup_entry(p1, "1.54 Mbit T1", NULL, cb_set_upstream,
			  (void *)(&(bands[4])));
   Epplet_add_popup_entry(p1, "1 Mbit", NULL, cb_set_upstream,
			  (void *)(&(bands[5])));
   Epplet_add_popup_entry(p1, "512 Kbit", NULL, cb_set_upstream,
			  (void *)(&(bands[6])));
   Epplet_add_popup_entry(p1, "256 Kbit", NULL, cb_set_upstream,
			  (void *)(&(bands[7])));
   Epplet_add_popup_entry(p1, "144 Kbit", NULL, cb_set_upstream,
			  (void *)(&(bands[8])));
   Epplet_add_popup_entry(p1, "128 Kbit ISDN", NULL, cb_set_upstream,
			  (void *)(&(bands[9])));
   Epplet_add_popup_entry(p1, "64 Kbit ISDN", NULL, cb_set_upstream,
			  (void *)(&(bands[10])));
   Epplet_add_popup_entry(p1, "56 Kbit Modem", NULL, cb_set_upstream,
			  (void *)(&(bands[11])));
   Epplet_add_popup_entry(p1, "33.6 Kbit Modem", NULL, cb_set_upstream,
			  (void *)(&(bands[12])));
   Epplet_add_popup_entry(p1, "28.8 Kbit Modem", NULL, cb_set_upstream,
			  (void *)(&(bands[13])));
   Epplet_add_popup_entry(p1, "14.4 Kbit Modem", NULL, cb_set_upstream,
			  (void *)(&(bands[14])));
   Epplet_add_popup_entry(p1, "9600 baud Modem", NULL, cb_set_upstream,
			  (void *)(&(bands[15])));
   Epplet_add_popup_entry(p1, "4800 baud Modem", NULL, cb_set_upstream,
			  (void *)(&(bands[16])));
   Epplet_add_popup_entry(p1, "2400 baud Modem", NULL, cb_set_upstream,
			  (void *)(&(bands[17])));
   Epplet_add_popup_entry(p1, "300 baud Modem", NULL, cb_set_upstream,
			  (void *)(&(bands[18])));
   Epplet_add_popup_entry(p1, "75 baud Modem", NULL, cb_set_upstream,
			  (void *)(&(bands[19])));

   p2 = Epplet_create_popup();
   Epplet_add_popup_entry(p2, "1 Gbit", NULL, cb_set_downstream,
			  (void *)(&(bands[0])));
   Epplet_add_popup_entry(p2, "100 Mbit", NULL, cb_set_downstream,
			  (void *)(&(bands[1])));
   Epplet_add_popup_entry(p2, "10 Mbit", NULL, cb_set_downstream,
			  (void *)(&(bands[2])));
   Epplet_add_popup_entry(p2, "2 Mbit", NULL, cb_set_downstream,
			  (void *)(&(bands[3])));
   Epplet_add_popup_entry(p2, "1.54 Mbit T1", NULL, cb_set_downstream,
			  (void *)(&(bands[4])));
   Epplet_add_popup_entry(p2, "1 Mbit", NULL, cb_set_downstream,
			  (void *)(&(bands[5])));
   Epplet_add_popup_entry(p2, "512 Kbit", NULL, cb_set_downstream,
			  (void *)(&(bands[6])));
   Epplet_add_popup_entry(p2, "256 Kbit", NULL, cb_set_downstream,
			  (void *)(&(bands[7])));
   Epplet_add_popup_entry(p2, "144 Kbit", NULL, cb_set_downstream,
			  (void *)(&(bands[8])));
   Epplet_add_popup_entry(p2, "128 Kbit ISDN", NULL, cb_set_downstream,
			  (void *)(&(bands[9])));
   Epplet_add_popup_entry(p2, "64 Kbit ISDN", NULL, cb_set_downstream,
			  (void *)(&(bands[10])));
   Epplet_add_popup_entry(p2, "56 Kbit Modem", NULL, cb_set_downstream,
			  (void *)(&(bands[11])));
   Epplet_add_popup_entry(p2, "33.6 Kbit Modem", NULL, cb_set_downstream,
			  (void *)(&(bands[12])));
   Epplet_add_popup_entry(p2, "28.8 Kbit Modem", NULL, cb_set_downstream,
			  (void *)(&(bands[13])));
   Epplet_add_popup_entry(p2, "14.4 Kbit Modem", NULL, cb_set_downstream,
			  (void *)(&(bands[14])));
   Epplet_add_popup_entry(p2, "9600 baud Modem", NULL, cb_set_downstream,
			  (void *)(&(bands[15])));
   Epplet_add_popup_entry(p2, "4800 baud Modem", NULL, cb_set_downstream,
			  (void *)(&(bands[16])));
   Epplet_add_popup_entry(p2, "2400 baud Modem", NULL, cb_set_downstream,
			  (void *)(&(bands[17])));
   Epplet_add_popup_entry(p2, "300 baud Modem", NULL, cb_set_downstream,
			  (void *)(&(bands[18])));
   Epplet_add_popup_entry(p2, "75 baud Modem", NULL, cb_set_downstream,
			  (void *)(&(bands[19])));

   Epplet_gadget_show(load_up = Epplet_create_hbar(16, 3, 46, 12, 0, &up_val));
   Epplet_gadget_show(load_down =
		      Epplet_create_hbar(16, 19, 46, 12, 0, &down_val));
   Epplet_gadget_show(Epplet_create_popupbutton
		      ("In", NULL, 62, 2, 16, 12, NULL, p1));
   Epplet_gadget_show(Epplet_create_popupbutton
		      ("Out", NULL, 62, 18, 16, 12, NULL, p2));
   Epplet_show();
   Epplet_Loop();
   return 0;
}
