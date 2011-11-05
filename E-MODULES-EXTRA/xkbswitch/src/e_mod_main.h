/*
 * Main module header.
 * Contains some i18n stuff, module versioning,
 * config and public prototypes from main.
 */

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#ifdef ENABLE_NLS
# include <libintl.h>
# define D_(string) dgettext(PACKAGE, string)
#else
# define bindtextdomain(domain,dir)
# define bind_textdomain_codeset(domain,codeset)
# define D_(string) (string)
#endif

/* Macros used for config file versioning */
/* You can increment the EPOCH value if the old configuration is not
 * compatible anymore, it creates an entire new one.
 * You need to increment GENERATION when you add new values to the
 * configuration file but is not needed to delete the existing conf  */
#define MOD_CONFIG_FILE_EPOCH 0x0001
#define MOD_CONFIG_FILE_GENERATION 0x008d
#define MOD_CONFIG_FILE_VERSION \
   ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)

typedef struct _e_xkb_cfg
{
    /* Not written to disk */
    E_Module *module;
    E_Config_Dialog *cfd;

    /* Written to disk */
    Eina_List *used_layouts;
    int version;
} e_xkb_cfg;

/* used layout - these are saved in a list */
typedef struct _E_XKB_Config_Layout
{
    const char *name;
    const char *model;
    const char *variant;
} E_XKB_Config_Layout;

/* Prototypes */

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

/* Function for calling the module's Configuration Dialog */
E_Config_Dialog *e_xkb_cfg_dialog(E_Container *con, const char *params);

extern e_xkb_cfg *e_xkb_cfg_inst;

#endif
