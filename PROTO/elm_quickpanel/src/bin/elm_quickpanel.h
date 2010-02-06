#ifndef ELM_QUICKPANEL_H
# define ELM_QUICKPANEL_H

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include <Ecore_X.h>
# include <Elementary.h>

# ifdef ELM_QUICKPANEL_API
#  undef ELM_QUICKPANEL_API
# endif
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define ELM_QUICKPANEL_API __attribute__ ((visibility("default")))
#  else
#   define ELM_QUICKPANEL_API
#  endif
# else
#  define ELM_QUICKPANEL_API
# endif

//# define ELM_QUICKPANEL_TYPEDEFS 1
//# include "includes.h"
//# undef ELM_QUICKPANEL_TYPEDEFS
//# include "includes.h"

# define ELM_QUICKPANEL_NEW(s, n) (s *)calloc(n, sizeof(s))
# define ELM_QUICKPANEL_FREE(p) do { if (p) {free(p); p = NULL;} } while (0)

# ifdef WANT_DEBUG
#  define ELM_QUICKPANEL_DEBUG(x) printf("ELM_QUICKPANEL: %s\n", x)
#  define ELM_QUICKPANEL_DEBUG_VAL(fmt, arg) printf(fmt, arg)
#  define OBJECT_CHECK
# else
#  define ELM_QUICKPANEL_DEBUG(x)
#  define ELM_QUICKPANEL_DEBUG_VAL(fmt, arg)
# endif

#endif
