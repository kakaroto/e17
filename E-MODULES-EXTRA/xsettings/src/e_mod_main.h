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

#define MOD_CONFIG_FILE_EPOCH 0x0000
#define MOD_CONFIG_FILE_GENERATION 0x0001
#define MOD_CONFIG_FILE_VERSION \
   ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)

typedef struct _Config Config;
typedef struct _Setting Setting;

#define SETTING_TYPE_INT	0
#define SETTING_TYPE_STRING	1
#define SETTING_TYPE_COLOR	2

struct _Config
{
  int version;
  int match_e17_theme;
  int match_e17_icon_theme;
  const char *gtk_theme;
  const char *icon_theme;
  Eina_List *settings;

  E_Module *module;
  E_Config_Dialog *cfd;
};

struct _Setting
{
  unsigned short type;

  const char *name;

  struct { const char *value; } s;
  struct { int value; } i;
  struct { unsigned short red, green, blue, alpha; } c;

  unsigned long length;
  unsigned long last_change;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

E_Config_Dialog *e_int_config_xsettings_module(E_Container *con, const char *params);
void xsettings_config_update(void);

extern Config *xsettings_conf;

#endif
