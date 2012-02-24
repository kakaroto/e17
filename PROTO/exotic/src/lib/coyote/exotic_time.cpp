#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Time.h>
#include <Exotic.h>

EAPI int
exotic_clock_gettime(clockid_t clk_id __UNUSED__, struct timespec *tp)
{
   unsigned long long result;

   result = Time::GetKernelTime(); /* µs * 1000 => ns, µs / (1000 * 1000) => s */
   tp->tv_sec = result / (1000 * 1000);
   tp->tv_nsec = (result - tp->tv_sec * 1000 * 1000) * 1000;

   return 0;
}

EAPI int
exotic_gettimeofday(struct timeval *tv, struct timezone *tz __UNUSED__)
{
   unsigned long long result;

   result = Time::GetTime();
   tv->tv_sec = result / (1000 * 1000);
   tv->tv_usec = result - tv->tv_sec * 1000 * 1000;
   return 0;
}

EAPI int
exotic_usleep(useconds_t usec)
{
   /* Note: funny but Sleep take a usec and not a sec in Point-Core world */
   Time::Sleep(usec);
   return 0;
}


