#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define BGCOLOR_WHITE  0
#define BGCOLOR_BLUE   1
#define BGCOLOR_GREEN  2
#define BGCOLOR_YELLOW 3

#define TRANS_0    0
#define TRANS_25   1
#define TRANS_50   2
#define TRANS_75   3
#define TRANS_100  4

typedef struct _Note Note;
typedef struct _Note_Face Note_Face;
typedef struct _Note_Config Note_Config;
typedef struct _Note_Face_Config Note_Face_Config;

struct _Note_Config
{
   Evas_Coord  height;
   Evas_Coord  width;
   int         bgcolor;   
   int         trans;
   Evas_List  *faces;
   
};

struct _Note_Face_Config
{
   Evas_Coord  height;
   Evas_Coord  width;
   int         bgcolor;   
   int         trans;
   char       *text;
   int         fsize;
};

struct _Note
{
   E_Menu     *config_menu;
   Evas_List  *faces;
   
   E_Config_DD *conf_edd;
   Note_Config *conf;
};

struct _Note_Face
{
   Note            *note;
   E_Container     *con;
   Evas            *evas;   
   Evas_Object     *note_object;
   Evas_Object     *note_edje;
   Evas_Object     *event_object;
   Evas_Coord       xx, yy, ww;
   E_Gadman_Client *gmc;
   E_Menu          *menu; 
   E_Menu          *menu_bg;
   E_Menu          *menu_trans;
   E_Menu          *menu_font;   
   E_Config_DD     *conf_edd;
   Note_Face_Config *conf;
};

extern E_Module_Api e_module_api;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);
EAPI int   e_modapi_info     (E_Module *m);
EAPI int   e_modapi_about    (E_Module *m);

#endif /* E_MOD_MAIN_H */
