#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define MOD_CONFIG_FILE_EPOCH 0x0002
#define MOD_CONFIG_FILE_GENERATION 0x008d
#define MOD_CONFIG_FILE_VERSION					\
  ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)

#ifdef ENABLE_NLS
# include <libintl.h>
# define _(string) dgettext(PACKAGE, string)
#else
# define bindtextdomain(domain,dir)
# define bind_textdomain_codeset(domain,codeset)
# define _(string) (string)
#endif
#define N_(str) (str)

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);

#endif
