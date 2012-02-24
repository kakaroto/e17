#ifdef HAVE_CONFIG_H
# include "config.h"
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

#include <exotic_inet.h>

#undef ntohl
#undef htonl
#undef NULL

#include <lib.h>

EAPI unsigned int
exotic_htonl(unsigned int hostlong)
{
   return ntohl(hostlong);
}

EAPI unsigned int
exotic_ntohl(unsigned int netlong)
{
   return ntohl(netlong);
}
