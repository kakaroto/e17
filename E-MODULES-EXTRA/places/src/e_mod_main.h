/*  Copyright (C) 2008 Davide Andreoli (see AUTHORS)
 *
 *  This file is part of places.
 *  places is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  places is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with places.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

/* Macros used for config file versioning */
#define MOD_CONFIG_FILE_EPOCH 0x0001
#define MOD_CONFIG_FILE_GENERATION 0x008e
#define MOD_CONFIG_FILE_VERSION \
   ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)

#define D_(str) dgettext(PACKAGE, str)

typedef struct _Config Config;
struct _Config 
{
   E_Module *module;
   E_Config_Dialog *cfd;

   /* The list; their location on screen ? */
   Eina_List *conf_items;

   /* config file version */
   int version;

   /* actual config properties; Define your own. (per-module) */
   const char *fm;
   unsigned char show_header;
   unsigned char auto_mount;
   unsigned char auto_open;
};

/* This struct used to hold config for individual items from above list */
typedef struct _Config_Item Config_Item;
struct _Config_Item 
{
   /* unique id */
   const char *id;

   /* actual config properties; Define your own per-item (pos, clr) */
   int switch2;
};

typedef struct _Instance Instance;
struct _Instance 
{
   /* pointer to this gadget's container */
   E_Gadcon_Client *gcc;

   /* evas_object used to display */
   Evas_Object *o_box;

   /* popup anyone ? */
   E_Menu *menu;

   /* Config_Item structure. Every gadget should have one :) */
   Config_Item *conf_item;
};

/* Setup the E Module Version, Needed to check if module can run. */
EAPI extern E_Module_Api e_modapi;

/* E API Module Interface Declarations
 * 
 * Need to initialize, shutdown, save the module */
EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

/* Function for calling the modules config dialog */
EAPI E_Config_Dialog *e_int_config_places_module(E_Container *con, const char *params __UNUSED__);

extern Config *places_conf;
extern Eina_List *instances;

#endif
