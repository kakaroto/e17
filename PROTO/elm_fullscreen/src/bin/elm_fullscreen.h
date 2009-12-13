#ifndef ELM_FULLSCREEN_H
# define ELM_FULLSCREEN_H

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include <Elementary.h>

# ifdef ELM_FULLSCREEN_API
#  undef ELM_FULLSCREEN_API
# endif
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define ELM_FULLSCREEN_API __attribute__ ((visibility("default")))
#  else
#   define ELM_FULLSCREEN_API
#  endif
# else
#  define ELM_FULLSCREEN_API
# endif

//# define ELM_FULLSCREEN_TYPEDEFS 1
//# include "includes.h"
//# undef ELM_FULLSCREEN_TYPEDEFS
//# include "includes.h"

# define ELM_FULLSCREEN_NEW(s, n) (s *)calloc(n, sizeof(s))
# define ELM_FULLSCREEN_FREE(p) do { if (p) {free(p); p = NULL;} } while (0)

# ifdef WANT_DEBUG
#  define ELM_FULLSCREEN_DEBUG(x) printf("ELM_FULLSCREEN: %s\n", x)
#  define ELM_FULLSCREEN_DEBUG_VAL(fmt, arg) printf(fmt, arg)
#  define OBJECT_CHECK
# else
#  define ELM_FULLSCREEN_DEBUG(x)
#  define ELM_FULLSCREEN_DEBUG_VAL(fmt, arg)
# endif

#endif
