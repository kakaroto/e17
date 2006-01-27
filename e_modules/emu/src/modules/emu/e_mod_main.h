#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H


typedef struct _Config      Config;
typedef struct _Config_Emu  Config_Emu;

#define EMU_WIDTH_AUTO -1
#define EMU_WIDTH_FIXED -2

struct _Config
{
   char         *appdir;
   int           follower;
   double        follow_speed;
   double        autoscroll_speed;
   int           iconsize;
   int           width;
   int		 allow_overlap;
   Evas_List    *emus;
};

struct _Config_Emu
{
   unsigned char enabled;
};

struct _Menu_Item_Data
{
   E_Menu_Item *item;
   char *name;
   char *action;
   char *edje;   /* FIXME: make this similar to the e_modapi_info() shit. */
   Ecore_Exe *exe;
   struct _Menu_Item_Data *next;
};

struct _Menu_Data
{
   E_Menu *menu;
   int level;
   char *name;
   struct _Menu_Data *next;
   struct _Menu_Item_Data *items;
};

typedef struct _Emu_Menu_Data Emu_Menu_Data;
struct _Emu_Menu_Data
{
   struct _Menu_Data *menu;
   E_Menu_Category_Callback *category_cb;
   char *buffer;

   char *name;
   char *category;
   E_Menu_Item *item;
   int valid; /* Probably just being paranoid.  This prevents this structure from being used when it isn't complete. */
};


typedef struct _Emu Emu;
struct _Emu 
{
   E_Gadget_Api api;
   E_Gadget *gad;

   Ecore_Event_Handler *del;

//   Evas_List   *bars;
   E_Menu      *config_menu;

   Config      *conf;
   E_Config_Dialog *config_dialog;   
};

typedef struct _Emu_Face Emu_Face;
struct _Emu_Face 
{
   Emu *emu;
   E_Gadget_Face *face;

   const char *name;
   const char *command;
   Ecore_Exe *exe;
   Ecore_Event_Handler *add;
   Ecore_Event_Handler *read;

   void *data; /**< the raw binary data from the child process that was recieved */
   int   size; /**< the size of this data in bytes */
   Ecore_Exe_Event_Data_Line *lines; /**< a NULL terminated array of line data if line buffered */

   Evas_Hash *menus;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);
EAPI int   e_modapi_info     (E_Module *m);
EAPI int   e_modapi_about    (E_Module *m);

void _emu_cb_config_updated(void *data);

#endif
