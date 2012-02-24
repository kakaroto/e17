#ifndef EXOTIC_TIME_H_
# define EXOTIC_TIME_H_

#include <time.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_EINA_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_EINA_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#ifdef EXOTIC_PROVIDE_CLOCK_GETTIME

#ifdef clock_gettime
# undef clock_gettime
#endif
#define clock_gettime exotic_clock_gettime

EAPI int exotic_clock_gettime(clockid_t clk_id, struct timespec *tp);

#endif

#ifdef EXOTIC_PROVIDE_GETTIMEOFDAY

#include <sys/time.h>

#ifdef gettimeofday
# undef gettimeofday
#endif
#define gettimeofday exotic_gettimeofday

EAPI int exotic_gettimeofday(struct timeval *tv, struct timezone *tz);

#endif

#ifdef EXOTIC_PROVIDE_USLEEP

#include <unistd.h>

#ifdef usleep
# undef usleep
#endif
#define usleep exotic_usleep

EAPI int exotic_usleep(useconds_t usec);

#endif

#endif /* EXOTIC_TIME_H */
