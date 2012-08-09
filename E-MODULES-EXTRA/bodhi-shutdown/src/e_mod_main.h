#ifndef E_MOD_MAIN_H
# define E_MOD_MAIN_H

# include "config.h"

# if HAVE_LOCALE_H
#  include <locale.h>
# endif

# ifdef ENABLE_NLS
#  include <libintl.h>
#  define _(str) dgettext(PACKAGE, str)
# else
#  define _(str) (str)
# endif

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

#endif
