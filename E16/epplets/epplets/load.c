/*
 * Copyright (C) 1999-2000, Michael Jennings
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <sys/time.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

#if 0
#  define D(x) do {printf("%10s | %7d:  [debug] ", __FILE__, __LINE__); printf x; fflush(stdout);} while (0)
#else
#  define D(x) ((void) 0)
#endif
#define BEGMATCH(a, b)  (!strncasecmp((a), (b), (sizeof(b) - 1)))
#define NONULL(x)       ((x) ? (x) : (""))

#define SET_PTR(p, v)             if (p) {D(("Setting %s (0x%08x) to %3.2g\n", #p, p, (double) (v))); *(p) = (double) (v);}
#define SET_AND_RETURN(o, f, t)   do {SET_PTR(one, o); SET_PTR(five, f); SET_PTR(fifteen, t); return;} while (0)

#ifdef __sun__
# ifndef FSCALE
#  define FSCALE 100.0
# endif
void
get_load_average(double *one, double *five, double *fifteen)
{
   kstat_ctl_t        *kc;
   kstat_t            *ks;
   kstat_named_t      *d1, *d5, *d15;

   if ((kc = kstat_open()) == NULL)
     {
	SET_AND_RETURN(0, 0, 0);
     }
   if ((ks = kstat_lookup(kc, "unix", 0, "system_misc")) == NULL)
     {
	SET_AND_RETURN(0, 0, 0);
     }
   if ((kstat_read(kc, ks, NULL)) < 0)
     {
	SET_AND_RETURN(0, 0, 0);
     }

   d1 = kstat_data_lookup(ks, "avenrun_1min");
   d5 = kstat_data_lookup(ks, "avenrun_5min");
   d15 = kstat_data_lookup(ks, "avenrun_15min");
   kstat_close(kc);

   SET_AND_RETURN(d1->value.ul / FSCALE, d5->value.ul / FSCALE,
		  d15->value.ul / FSCALE);
}

#elif defined(linux)
void
get_load_average(double *one, double *five, double *fifteen)
{
   FILE               *fp;
   char                buff[64];
   double              a, b, c;

   if ((fp = fopen("/proc/loadavg", "rt")) == NULL)
     {
	SET_AND_RETURN(0, 0, 0);
     }
   fgets(buff, sizeof(buff), fp);
   fclose(fp);
   sscanf(buff, "%lf %lf %lf", &a, &b, &c);
   SET_AND_RETURN(a, b, c);
}

#else
/* A generic fallback that should work just about anywhere. */
void
get_load_average(double *one, double *five, double *fifteen)
{
   FILE               *pp;
   char                buff[128], *p;
   double              a, b, c;

   if ((pp = popen("uptime", "r")) == NULL)
     {
	SET_AND_RETURN(0, 0, 0);
     }
   fgets(buff, sizeof(buff), pp);
   pclose(pp);
   p = strrchr(buff, ':');
   sscanf(p, "%lf, %lf, %lf", &a, &b, &c);
   SET_AND_RETURN(a, b, c);
}

#endif
