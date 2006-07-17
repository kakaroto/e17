#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H_INCLUDED
#define E_MOD_MAIN_H_INCLUDED

#include <e.h>

#define DEBUG
#ifdef DEBUG
# ifndef DBG
#  define DBG fprintf
# endif
#else
# ifndef DBG
#  define DBG
# endif
#endif

extern const char *module_root;
extern char *module_theme;

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);
EAPI int e_modapi_about(E_Module *m);
EAPI int e_modapi_config(E_Module *m);

#endif //E_MOD_MAIN_H_INCLUDED
