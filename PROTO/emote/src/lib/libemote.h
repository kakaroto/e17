#ifndef LIBEMOTE_H
# define LIBEMOTE_H

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

# define EMOTE_PROTOCOL_API_VERSION 1

typedef struct _Emote_Protocol_Api Emote_Protocol_Api;
typedef struct _Emote_Protocol Emote_Protocol;

struct _Emote_Protocol_Api 
{
   int version;
   const char *name, *label;
};

struct _Emote_Protocol 
{
   Emote_Protocol_Api *api;
   void *handle;

   struct 
     {
        void *(*init) (Emote_Protocol *p); // required
        int (*shutdown) (Emote_Protocol *p); // required

        /* TODO: Implement generic functions */
     } funcs;
};

#endif
