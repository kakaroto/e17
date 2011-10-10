#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define MOD_CONFIG_FILE_EPOCH 0x0004
#define MOD_CONFIG_FILE_GENERATION 0x0001
#define MOD_CONFIG_FILE_VERSION					\
  ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)


#ifdef ENABLE_NLS
# include <libintl.h>
# define D_(string) dgettext(PACKAGE, string)
#else
# define bindtextdomain(domain,dir)
# define bind_textdomain_codeset(domain,codeset)
# define D_(string) (string)
#endif

#define DBG(...)
/* #define DBG(...) printf(__VA_ARGS__) */
#define GO_KEY		0
#define GO_MOUSE	1
#define GO_EDGE		2
#define GO_ACTION	3


typedef struct _Config_Item Config_Item;
typedef struct _Config Config;

struct _Config
{
  E_Module	*module;
  Eina_List	*conf_items;
  int		 version;
  E_Config_Dialog *cfd;
  const char    *theme_path;

  /* scale current desk */
  int           layout_mode;
  unsigned char tight;
  unsigned char grow;
  unsigned char show_iconic;
  double	scale_duration;
  double	spacing;

  /* scale all desk */
  int           desks_layout_mode;
  unsigned char desks_tight;
  unsigned char desks_grow;
  unsigned char desks_show_iconic;
  double	desks_duration;
  double	desks_spacing;
  unsigned char fade_windows;

  unsigned char fade_popups;
  unsigned char fade_desktop;

  /* pager */
  double	pager_duration;
  unsigned char pager_fade_windows;
  unsigned char pager_fade_popups;
  unsigned char pager_fade_desktop;
  unsigned char pager_keep_shelves;
};

struct _Config_Item
{
  const char *id;
  int switch2;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);


E_Config_Dialog *e_int_config_scale_module(E_Container *con, const char *params);

Eina_Bool e_mod_hold_modifier_check(Ecore_Event_Key *ev);
Eina_Bool e_mod_border_ignore(E_Border *bd);

Eina_Bool scale_run(E_Manager *man, const char *params, int init_method);
Eina_Bool pager_run(E_Manager *man, const char *params, int init_method);

extern Config *scale_conf;

#endif
