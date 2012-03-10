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

typedef struct _E_XKB_Config
{
    /* Not written to disk */
    E_Module *module;
    E_Config_Dialog *cfd;

    /* Written to disk */
    int version;
    Eina_List *used_layouts;
} E_XKB_Config;

/* This represents the node data in used_layouts */
typedef struct _E_XKB_Config_Layout
{
    const char *name;
    const char *model;
    const char *variant;
} E_XKB_Config_Layout;

/* automatically typedef'd by E */
struct _E_Config_Dialog_Data 
{
    Evas *evas, *dlg_evas;
    Evas_Object *layout_list, *used_list, *model_list, *variant_list;
    Evas_Object *btn_add, *btn_del, *btn_up, *btn_down;
    Ecore_Timer *fill_delay;
    Ecore_Timer *dlg_fill_delay;

    Eina_List *cfg_layouts;

    E_Dialog *dlg_add_new;
};

/* Prototypes */

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init    (E_Module *m);
EAPI int   e_modapi_shutdown(E_Module *m);
EAPI int   e_modapi_save    (E_Module *m);

void e_xkb_update_icon  (void);
void e_xkb_update_layout(void);

void e_xkb_layout_next(void);
void e_xkb_layout_prev(void);

E_Config_Dialog *e_xkb_cfg_dialog(E_Container *con, const char *params);

extern E_XKB_Config *e_xkb_cfg;

#endif
