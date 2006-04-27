/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2006 Kim Woelders
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
#include "E.h"
#include "eobj.h"
#include "xwin.h"
#include <sys/time.h>
#include <time.h>

/* This is a general quicksort algorithm, using median-of-three strategy.
 * 
 * Parameters:
 * ===========
 * a:            array of items to be sorted (list of void pointers).
 * l:            left edge of sub-array to be sorted. Toplevel call has 0 here.
 * r:            right edge of sub-array to be sorted. Toplevel call has |a| - 1 here.
 * CompareFunc:  Pointer to a function that accepts two general items d1 and d2
 * and returns values as follows:
 * 
 * < 0  --> d1 "smaller" than d2
 * > 0  --> d1 "larger"  than d2
 * 0    --> d1 "==" d2.
 * 
 * See sample application in ipc.c's IPC_Help.
 */
void
Quicksort(void **a, int l, int r, int (*CompareFunc) (void *d1, void *d2))
{

   int                 i, j, m;
   void               *v, *t;

   if (r > l)
     {

	m = (r + l) / 2 + 1;
	if (CompareFunc(a[l], a[r]) > 0)
	  {
	     t = a[l];
	     a[l] = a[r];
	     a[r] = t;
	  }
	if (CompareFunc(a[l], a[m]) > 0)
	  {
	     t = a[l];
	     a[l] = a[m];
	     a[m] = t;
	  }
	if (CompareFunc(a[r], a[m]) > 0)
	  {
	     t = a[r];
	     a[r] = a[m];
	     a[m] = t;
	  }

	v = a[r];
	i = l - 1;
	j = r;

	for (;;)
	  {
	     while (CompareFunc(a[++i], v) < 0)
		;
	     while (CompareFunc(a[--j], v) > 0)
		;
	     if (i >= j)
		break;
	     t = a[i];
	     a[i] = a[j];
	     a[j] = t;
	  }
	t = a[i];
	a[i] = a[r];
	a[r] = t;
	Quicksort(a, l, i - 1, CompareFunc);
	Quicksort(a, i + 1, r, CompareFunc);
     }
}

/*
 * Stuff to make loops for animated effects.
 */
struct timeval      etl_tv_start;
static int          etl_k1, etl_k2;
static double       etl_k, etl_fac;

/*
 * Return elapsed time in seconds since t0
 */
static double
ETimeElapsed(struct timeval *t0)
{
   struct timeval      tv;
   int                 sec, usec;

   gettimeofday(&tv, NULL);
   sec = tv.tv_sec - t0->tv_sec;
   usec = tv.tv_usec - t0->tv_usec;
   return (double)sec + (((double)usec) / 1000000);
}

#include <math.h>

static float
ETimeCurve(int k1, int k2, float k, int mode)
{
   float               x, l;

   if (k >= k2 || mode == 0)
      return k;

   l = k2 - k1;
   x = k - k1;

   switch (mode)
     {
     case 1:			/* Sinuoidal - half cycle */
	x = x / l - 0.5;	/* x: -0.5 -> 0.5 */
	x = 0.5 * (1. + sin(x * M_PI));
	break;
     case 2:			/* Sinuoidal - quarter cycle */
	x = x / l;		/* x: 0 -> 1 */
	x = sin(x * M_PI / 2);
	break;
     }

   return k1 + x * l;
}

void
ETimedLoopInit(int k1, int k2, int speed)
{
   etl_k1 = k1;
   etl_k2 = k2;
   if (speed < 500)
      speed = 500;
   /* When speed is 1000 the loop will take one sec. */
   etl_fac = (k2 - k1) * (double)speed / 1000.;

   gettimeofday(&etl_tv_start, NULL);
   ESync();
}

int
ETimedLoopNext(void)
{
   double              tm, y;

   /* Is this portable? */
   usleep(5000);

   /* Find elapsed time since loop start */
   tm = ETimeElapsed(&etl_tv_start);
   etl_k = etl_k1 + tm * etl_fac;
#if 0
   Eprintf("ETimedLoopNext k=%4f tm=%.3f\n", etl_k, tm);
#endif
   y = ETimeCurve(etl_k1, etl_k2, (float)etl_k, 2);

   EobjsRepaint();

   return y;
}

/*
 * Debug/error message printing.
 */

#if 1				/* Set to 0 for differential time */

void
Eprintf(const char *fmt, ...)
{
   static time_t       t0 = 0;
   va_list             args;
   struct timeval      tv;

   if (t0 == 0)
      t0 = time(NULL);

   gettimeofday(&tv, NULL);
   fprintf(stdout, "[%d] %4ld.%06ld: ", getpid(), tv.tv_sec - t0, tv.tv_usec);
   va_start(args, fmt);
   vfprintf(stdout, fmt, args);
   va_end(args);
}

#else

void
Eprintf(const char *fmt, ...)
{
   static struct timeval t0;
   va_list             args;
   struct timeval      tv;
   long                ts, tus;

   gettimeofday(&tv, NULL);
   ts = tv.tv_sec - t0.tv_sec;
   tus = tv.tv_usec - t0.tv_usec;
   if (tus < 0)
     {
	tus += 1000000;
	ts -= 1;
     }
   fprintf(stdout, "[%d] %#lx %4ld.%06ld: ", getpid(), NextRequest(disp), ts,
	   tus);
   va_start(args, fmt);
   vfprintf(stdout, fmt, args);
   va_end(args);
   gettimeofday(&t0, NULL);
}

#endif
