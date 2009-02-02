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
#define MOD_CONFIG_FILE_GENERATION 0x008f
#define MOD_CONFIG_FILE_VERSION \
   ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)

#define D_(str) dgettext(PACKAGE, str)

typedef struct _Config Config;
struct _Config 
{
   E_Module *module;
   E_Config_Dialog *cfd;

   Eina_List *conf_items;

   /* config file version */
   int version;

   const char *fm;
   unsigned char auto_mount;
   unsigned char auto_open;

   unsigned char show_home;
   unsigned char show_desk;
   unsigned char show_trash;
   unsigned char show_root;
   unsigned char show_temp;
   unsigned char show_bookm;
};

typedef struct _Config_Item Config_Item;
struct _Config_Item 
{
   const char *id;

   int switch2;
};

typedef struct _Instance Instance;
struct _Instance 
{
   E_Gadcon_Client *gcc;
   Evas_Object *o_box;
   E_Menu *menu;
   Config_Item *conf_item;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

EAPI E_Config_Dialog *e_int_config_places_module(E_Container *con);

extern Config *places_conf;
extern Eina_List *instances;

#endif
