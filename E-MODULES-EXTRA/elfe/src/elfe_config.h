#ifndef E_MOD_CONFIG_H
#define E_MOD_CONFIG_H

#define IL_CONFIG_MIN 0
#define IL_CONFIG_MAJ 0

#define  LONGPRESS_THRESHOLD 16

typedef struct _Elfe_Home_Config Elfe_Home_Config;
typedef struct _Elfe_Desktop_Config Elfe_Desktop_Config;
typedef struct _Elfe_Desktop_Item_Config Elfe_Desktop_Item_Config;
typedef enum _Elfe_Desktop_Item_Type Elfe_Desktop_Item_Type;

enum _Elfe_Desktop_Item_Type
{
    ELFE_DESKTOP_ITEM_APP,
    ELFE_DESKTOP_ITEM_GADGET,
    ELFE_DESKTOP_ITEM_ICON,
};

struct _Elfe_Home_Config
{
    Eina_List *desktops;
    const char *mod_dir;
    const char *theme;
    int cols;
    int rows;
    int icon_size;
    Eina_List *dock_items;
};

struct _Elfe_Desktop_Config
{
    Eina_List *items;
};

struct _Elfe_Desktop_Item_Config
{
    Elfe_Desktop_Item_Type type;
    const char *name;
    Evas_Coord pos_x;
    Evas_Coord pos_y;
    Evas_Coord size_w;
    Evas_Coord size_h;
    int row;
    int col;
};

int elfe_home_config_init(E_Module *m);
int elfe_home_config_shutdown(void);
int elfe_home_config_save(void);
void elfe_home_config_show(E_Container *con, const char *params);
void elfe_home_config_desktop_item_add(int desktop,
                                       Elfe_Desktop_Item_Type type,
                                       int row, int col,
                                       Evas_Coord x, Evas_Coord y,
                                       Evas_Coord w, Evas_Coord h,
                                       const char *name);
void elfe_home_config_desktop_item_del(int desktop,
				       int row, int col);

extern EAPI Elfe_Home_Config *elfe_home_cfg;

#endif
