#include "config.h"
#include "epplet.h"

#ifdef HAVE_LIBGTOP
#include <glibtop.h>
#include <glibtop/loadavg.h>
#include <glibtop/cpu.h>
#include "proc.h"
#endif

int                 cpus = 0;
double             *prev_val = NULL;
int                *load_val = NULL;
Epplet_gadget      *load = NULL;

static void         cb_timer(void *data);
static void         cb_close(void *data);
int                 count_cpus(void);

static void
cb_timer(void *data)
{
#ifdef HAVE_LIBGTOP

/* libgtop only handles total load, not per-CPU load */

   glibtop_loadavg     loadavg;
   double              val, val2;
   int                 i;

   glibtop_get_loadavg(&loadavg);
   val2 = loadavg.loadavg[0];
   val2 *= 20;

   /* printf ("Load: %f\n", val2); */

   if (val2 > 100)
      val2 = 100;
   load_val[0] = val2;
   Epplet_gadget_data_changed(load[0]);

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
	     char                ss[64];
	     double              val, val2;

	     fgets(s, 255, f);
	     sscanf(s, "%*s %s %*s %*s %*s", ss);
	     val = atof(ss);
	     val2 = val - prev_val[i];
	     prev_val[i] = val;
	     val2 *= 3;
	     if (val2 > 100)
		val2 = 100;
	     load_val[i] = val2;
	     Epplet_gadget_data_changed(load[i]);
	  }
	fclose(f);
     }

#endif

   Esync();
   Epplet_timer(cb_timer, NULL, 0.333, "TIMER");
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

int
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

int
main(int argc, char **argv)
{
   int                 i;

   atexit(Epplet_cleanup);
   cpus = count_cpus();
   load_val = malloc(sizeof(int) * cpus);
   prev_val = malloc(sizeof(double) * cpus);
   load = malloc(sizeof(Epplet_gadget) * cpus);

   Epplet_Init("E-Load", "0.1", "Enlightenment Load Epplet",
	       5, cpus, argc, argv, 0);
   Epplet_timer(cb_timer, NULL, 0.333, "TIMER");
   Epplet_gadget_show(Epplet_create_button(NULL, NULL,
					   2, 2, 0, 0, "CLOSE", 0, NULL,
					   cb_close, NULL));
   for (i = 0; i < cpus; i++)
     {
	load[i] = Epplet_create_hbar(16, 3 + (i * 16), 62, 12,
				     0, &(load_val[i]));
	Epplet_gadget_show(load[i]);
     }
   Epplet_show();
   Epplet_Loop();
   return 0;
}
