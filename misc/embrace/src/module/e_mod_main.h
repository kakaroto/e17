#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config        Config;
typedef struct _Config_Face   Config_Face;
typedef struct _Embrace       Embrace;
typedef struct _Embrace_Face  Embrace_Face;
typedef struct _Embrace_Desk  Embrace_Desk;
typedef struct _Embrace_Win   Embrace_Win;

struct _Config {
	int dummy;
};

struct _Embrace {
	Evas_List   *faces;
	E_Menu      *config_menu;
	Evas_List   *menus;

	Config      *conf;
};

struct _Embrace_Face {
	E_Gadman_Client *gmc;
	E_Menu          *menu;
	Evas            *evas;

	E_Zone          *zone;

	Evas_Object     *embrace_object;
	Evas_Object     *box_object;
};

EAPI void *e_modapi_init     (E_Module *module);
EAPI int   e_modapi_shutdown (E_Module *module);
EAPI int   e_modapi_save     (E_Module *module);
EAPI int   e_modapi_info     (E_Module *module);
EAPI int   e_modapi_about    (E_Module *module);

#endif
