#ifndef ELM_KBD_H
# define ELM_KBD_H

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include <Ecore_X.h>
# include <Elementary.h>

# ifdef ELM_KBD_API
#  undef ELM_KBD_API
# endif
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define ELM_KBD_API __attribute__ ((visibility("default")))
#  else
#   define ELM_KBD_API
#  endif
# else
#  define ELM_KBD_API
# endif

//# define ELM_KBD_TYPEDEFS 1
//# include "includes.h"
//# undef ELM_KBD_TYPEDEFS
//# include "includes.h"

# define ELM_KBD_NEW(s, n) (s *)calloc(n, sizeof(s))
# define ELM_KBD_FREE(p) do { if (p) {free(p); p = NULL;} } while (0)

# ifdef WANT_DEBUG
#  define ELM_KBD_DEBUG(x) printf("ELM_KBD: %s\n", x)
#  define ELM_KBD_DEBUG_VAL(fmt, arg) printf(fmt, arg)
#  define OBJECT_CHECK
# else
#  define ELM_KBD_DEBUG(x)
#  define ELM_KBD_DEBUG_VAL(fmt, arg)
# endif

#endif
