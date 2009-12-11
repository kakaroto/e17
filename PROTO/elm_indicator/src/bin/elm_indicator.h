#ifndef ELM_INDICATOR_H
# define ELM_INDICATOR_H

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include <Elementary.h>

# ifdef ELM_INDICATOR_API
#  undef ELM_INDICATOR_API
# endif
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define ELM_INDICATOR_API __attribute__ ((visibility("default")))
#  else
#   define ELM_INDICATOR_API
#  endif
# else
#  define ELM_INDICATOR_API
# endif

# define ELM_INDICATOR_NEW(s, n) (s *)calloc(n, sizeof(s))
# define ELM_INDICATOR_FREE(p) do { if (p) {free(p); p = NULL;} } while (0)

# ifdef WANT_DEBUG
#  define ELM_INDICATOR_DEBUG(x) printf("ELM_INDICATOR: %s\n", x)
#  define ELM_INDICATOR_DEBUG_VAL(fmt, arg) printf(fmt, arg)
#  define OBJECT_CHECK
# else
#  define ELM_INDICATOR_DEBUG(x)
#  define ELM_INDICATOR_DEBUG_VAL(fmt, arg)
# endif

#endif
