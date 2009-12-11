#ifndef ELM_SOFTKEY_H
# define ELM_SOFTKEY_H

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include <Elementary.h>

# ifdef ELM_SOFTKEY_API
#  undef ELM_SOFTKEY_API
# endif
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define ELM_SOFTKEY_API __attribute__ ((visibility("default")))
#  else
#   define ELM_SOFTKEY_API
#  endif
# else
#  define ELM_SOFTKEY_API
# endif

# define ELM_SOFTKEY_NEW(s, n) (s *)calloc(n, sizeof(s))
# define ELM_SOFTKEY_FREE(p) do { if (p) {free(p); p = NULL;} } while (0)

# ifdef WANT_DEBUG
#  define ELM_SOFTKEY_DEBUG(x) printf("ELM_SOFTKEY: %s\n", x)
#  define ELM_SOFTKEY_DEBUG_VAL(fmt, arg) printf(fmt, arg)
#  define OBJECT_CHECK
# else
#  define ELM_SOFTKEY_DEBUG(x)
#  define ELM_SOFTKEY_DEBUG_VAL(fmt, arg)
# endif

#endif
