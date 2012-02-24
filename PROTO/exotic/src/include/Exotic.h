#ifndef EXOTIC_H
# define EXOTIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "exotic_config.h"
#include "exotic_coyote.h"

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

#include "exotic_time.h"
#include "exotic_strings.h"
#include "exotic_stdlib.h"
#include "exotic_stdio.h"
#include "exotic_mman.h"
#include "exotic_inet.h"
#include "exotic_fnmatch.h"
#include "exotic_libgen.h"
#include "exotic_unistd.h"

EAPI void exotic_external_set(const char *name, void *value);

#ifdef __cplusplus
}
#endif

#endif /* EXOTIC_H */
