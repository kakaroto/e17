#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define BGCOLOR_WHITE  1
#define BGCOLOR_BLUE   2
#define BGCOLOR_GREEN  3
#define BGCOLOR_YELLOW 4

typedef struct _Note Note;
typedef struct _Note_Face Note_Face;
typedef struct _Config Config;

struct _Config
{
   Evas_Coord  height;
   Evas_Coord  width;
   int         bgcolor;   
};



struct _Note
{
   E_Menu     *config_menu;
   E_Menu     *config_menu_bg;
   E_Menu     *config_menu_trans;   
   Note_Face  *face;
   
   E_Config_DD *conf_edd;
   Config      *conf;
};

struct _Note_Face
{
   Note            *note;
   E_Container     *con;
   Evas            *evas;   
   Evas_Object     *note_object;
   Evas_Object     *note_edje;
   Evas_Coord       xx, yy, ww;
   E_Gadman_Client *gmc;
   E_Menu          *menu;   
};

EAPI void *init     (E_Module *m);
EAPI int   shutdown (E_Module *m);
EAPI int   save     (E_Module *m);
EAPI int   info     (E_Module *m);
EAPI int   about    (E_Module *m);

#endif /* E_MOD_MAIN_H */
