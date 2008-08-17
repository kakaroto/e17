#include "e.h"
#include "border_props.h"
#include "easy_menu.h"
#include "e_mod_config.h"
#include "config.h"

#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config Config;
typedef struct _Config_Emu Config_Emu;

#define EMU_WIDTH_AUTO -1
#define EMU_WIDTH_FIXED -2

struct _Config
{
   char *appdir;
   Evas_List *emus;
};

struct _Config_Emu
{
   unsigned char enabled;
};

typedef struct _Emu_Face Emu_Face;
struct _Emu_Face
{
   E_Gadcon_Client *gcc;
   Evas_Object     *o_button;

   const char *name;
   const char *command;
   Ecore_Exe *exe;
   Ecore_Event_Handler *del;
   Ecore_Event_Handler *add;
   Ecore_Event_Handler *read;

   void *data;
               /**< the raw binary data from the child process that was recieved */
   int size;
               /**< the size of this data in bytes */
   Ecore_Exe_Event_Data_Line *lines; /**< a NULL terminated array of line data if line buffered */

   Evas_Hash *menus;

   E_Menu *config_menu;

   Config *conf;
   E_Config_Dialog *config_dialog;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

void _emu_cb_config_updated(void *data);

#endif
