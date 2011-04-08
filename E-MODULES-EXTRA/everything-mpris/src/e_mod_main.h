#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define _(str) dgettext(PACKAGE, str)
#define N_(str) (str)

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);

#endif
