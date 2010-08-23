#ifndef EMOTE_PRIVATE_H
# define EMOTE_PRIVATE_H

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

# if HAVE_DLFCN_H
#  include <dlfcn.h>
# endif

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <limits.h>

# include <Ecore.h>
# include <Ecore_File.h>

# include "emote_types.h"

# define EM_TYPEDEFS
# include "emote_object.h"
# undef EM_TYPEDEFS
# include "emote_object.h"

# define EMOTE_PROTOCOL_API_VERSION 1

# define EMOTE_NEW(s, n) (s *)malloc(n * sizeof(s))
# define EMOTE_FREE(p) do { if (p) {free(p); p = NULL;} } while (0)
# define EMOTE_CLAMP(x, min, max) (x < min ? min : (x > max ? max : x))

typedef struct _Emote_Paths Emote_Paths;

struct _Emote_Paths
{
   char libdir[PATH_MAX];
   char protocoldir[PATH_MAX];
};

Emote_Paths emote_paths;

EM_INTERN int emote_protocol_init(void);
EM_INTERN int emote_protocol_shutdown(void);

#endif
