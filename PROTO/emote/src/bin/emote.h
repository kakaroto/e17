#ifndef EMOTE_H
# define EMOTE_H

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# ifdef HAVE_ALLOCA_H
#  include <alloca.h>
# elif defined __GNUC__
#  define alloca __builtin_alloca
# elif defined _AIX
#  define alloca __alloca
# elif defined _MSC_VER
#  include <malloc.h>
#  define alloca _alloca
# else
#  include <stddef.h>
#  ifdef  __cplusplus
extern "C"
#  endif
void *alloca (size_t);
# endif

# ifdef HAVE___ATTRIBUTE__
#  define __UNUSED__ __attribute__((unused))
# else
#  define __UNUSED__
# endif

# ifdef HAVE_GETTEXT
#  include <libintl.h>
#  include <locale.h>
#  define _(str) gettext(str)
#  define gettext_noop(str) (str)
#  define N_(str) gettext_noop(str)
# else
#  define _(str) (str)
#  define N_(str) (str)
# endif

# include <Ecore.h>
# include <Ecore_Con.h>
# include <Elementary.h>


# ifdef EMAPI
#  undef EMAPI
# endif

# ifdef WIN32
#  ifdef BUILDING_DLL
#   define EMAPI __declspec(dllexport)
#  else
#   define EMAPI __declspec(dllimport)
#  endif
# else
#  ifdef __GNUC__
#   if __GNUC__ >= 4
/* BROKEN in gcc 4 on amd64 */
#    if 0
#     pragma GCC visibility push(hidden)
#    endif
#    define EMAPI __attribute__ ((visibility("default")))
#   else
#    define EMAPI
#   endif
#  else
#   define EMAPI
#  endif
# endif

# ifdef EM_INTERN
#  undef EM_INTERN
# endif
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EM_INTERN __attribute__ ((visibility("hidden")))
#  else
#   define EM_INTERN
#  endif
# else
#  define EM_INTERN
# endif

# define EM_TYPEDEFS 1
# include "em_includes.h"
# undef EM_TYPEDEFS
# include "em_includes.h"

# define EM_NEW(s, n) (s *)malloc(n * sizeof(s))
# define EM_FREE(p) do { if (p) {free(p); p = NULL;} } while (0)
# define EM_CLAMP(x, min, max) (x < min ? min : (x > max ? max : x))

#endif
