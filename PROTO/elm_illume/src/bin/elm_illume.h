#ifndef ELM_ILLUME_H
# define ELM_ILLUME_H

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include <Elementary.h>

# ifdef ELM_ILLUME_API
#  undef ELM_ILLUME_API
# endif
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define ELM_ILLUME_API __attribute__ ((visibility("default")))
#  else
#   define ELM_ILLUME_API
#  endif
# else
#  define ELM_ILLUME_API
# endif

//# define ELM_ILLUME_TYPEDEFS 1
//# include "includes.h"
//# undef ELM_ILLUME_TYPEDEFS
//# include "includes.h"

# define ELM_ILLUME_NEW(s, n) (s *)calloc(n, sizeof(s))
# define ELM_ILLUME_FREE(p) do { if (p) {free(p); p = NULL;} } while (0)

# ifdef WANT_DEBUG
#  define ELM_ILLUME_DEBUG(x) printf("ELM_ILLUME: %s\n", x)
#  define ELM_ILLUME_DEBUG_VAL(fmt, arg) printf(fmt, arg)
#  define OBJECT_CHECK
# else
#  define ELM_ILLUME_DEBUG(x)
#  define ELM_ILLUME_DEBUG_VAL(fmt, arg)
# endif

#endif
