/*
 * GeeXboX Enna Media Center.
 * Copyright (C) 2005-2010 The Enna Project
 *
 * This file is part of Enna.
 *
 * Enna is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Enna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Enna; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <Eina.h>

#include "enna.h"
#include "module.h"
#include "enna_config.h"

static Eina_List *_enna_modules = NULL;   /** List of Enna_Modules* */
static Eina_Array *_plugins_array = NULL; /** Array of Eina_Modules* (or api* in static mode)*/

#ifdef BUILD_BROWSER_LOCALFILES
extern Enna_Module_Api enna_mod_browser_localfiles_api;
#endif
#ifdef BUILD_BROWSER_UPNP
extern Enna_Module_Api enna_mod_browser_upnp_api;
#endif
#ifdef BUILD_MODULE_VOLUME_MTAB
extern Enna_Module_Api enna_mod_volume_mtab_api;
#endif
#ifdef BUILD_MODULE_VOLUME_UDEV
extern Enna_Module_Api enna_mod_volume_udev_api;
#endif


static Enna_Module *
enna_module_open(Enna_Module_Api *api)
{
   Enna_Module *m;

   if (!api || !api->name) return NULL;

   if (api->version != ENNA_MODULE_VERSION )
     {
        return NULL;
     }

   m = ENNA_NEW(Enna_Module, 1);
   m->api = api;
   m->enabled = 0;
   _enna_modules = eina_list_append(_enna_modules, m);
   return m;
}

/**
 * @brief Init the module system
 */
int
enna_module_init(void)
{
#ifdef USE_STATIC_MODULES

   /* Populate the array of available plugins statically */
   _plugins_array = eina_array_new(20);

#ifdef BUILD_BROWSER_LOCALFILES
   eina_array_push(_plugins_array, &enna_mod_browser_localfiles_api);
#endif
#ifdef BUILD_BROWSER_UPNP
   eina_array_push(_plugins_array, &enna_mod_browser_upnp_api);
#endif

#ifdef BUILD_MODULE_VOLUME_MTAB
   eina_array_push(_plugins_array, &enna_mod_volume_mtab_api);
#endif
#ifdef BUILD_MODULE_VOLUME_UDEV
   eina_array_push(_plugins_array, &enna_mod_volume_udev_api);
#endif

#else
   Eina_Module *module;

   /* Populate the array of available plugins dinamically */
   _plugins_array = eina_array_new(20);
   _plugins_array = eina_module_list_get(_plugins_array,
                                         PACKAGE_LIB_DIR"/enna/modules/", 0, NULL, NULL);
#endif /* USE_STATIC_MODULES */

#if ENABLE_CONFIG_PANEL
   _config_panel = enna_config_panel_register(_("Modules"), "icon/module",
                                              _config_panel_show, _config_panel_hide, NULL);
#endif

   return 0;
}

/**
 * @brief Disable/Free all modules registered and free the Eina_Module Array
 */
void
enna_module_shutdown(void)
{
   Enna_Module *m;

#if ENABLE_CONFIG_PANEL
   enna_config_panel_unregister(_config_panel);
#endif

   /* Disable and free all Enna_Modules */
   EINA_LIST_FREE(_enna_modules, m)
     {
        if (m->enabled)
          enna_module_disable(m);
        free(m);
     }
   _enna_modules = NULL;

   if (_plugins_array)
     {
#ifdef USE_STATIC_MODULES
        /* Free the Eina_Array of static pointer */
        eina_array_free(_plugins_array);
        _plugins_array = NULL;
#else
        /* Free the Eina_Array of Eina_Module */
        eina_module_list_unload(_plugins_array);
        eina_module_list_free(_plugins_array);
        eina_array_free(_plugins_array);
        _plugins_array = NULL;
#endif /* USE_STATIC_MODULES */
     }
}

/**
 * @brief Enable the given module
 */
int
enna_module_enable(Enna_Module *m)
{
   if (!m)
     return -1;
   if (m->enabled)
     return 0;
   if (m->api->func.init)
     m->api->func.init(m);
   m->enabled = 1;
   return 0;
}

/**
 * @brief Disable the given module
 */
int
enna_module_disable(Enna_Module *m)
{
   if (!m)
     return -1;
   if (!m->enabled)
     return 0;
   if (m->api->func.shutdown)
     {
        m->api->func.shutdown(m);
        m->enabled = 0;
        return 0;
     }
   return -1;
}

/**
 * @brief Load/Enable all the know modules
 */
void
enna_module_load_all(void)
{
   Enna_Module_Api *api;
   Enna_Module *em;
   Eina_Array_Iterator iterator;
   unsigned int i;

   if (!_plugins_array)
     return;

#ifdef USE_STATIC_MODULES
   EINA_ARRAY_ITER_NEXT(_plugins_array, i, api, iterator)
     {
#else
        Eina_Module *plugin;

        eina_module_list_load(_plugins_array);

        EINA_ARRAY_ITER_NEXT(_plugins_array, i, plugin, iterator)
          {
             api = eina_module_symbol_get(plugin, ENNA_STRINGIFY(MOD_PREFIX) "_api");
#endif /* USE_STATIC_MODULES */

             em = enna_module_open(api);
             enna_module_enable(em);
          }
     }

   /******************************************************************************/
   /*                     Config Panel Stuff                                     */
   /******************************************************************************/

#if ENABLE_CONFIG_PANEL
   static Eina_Bool
     _input_events_cb(void *data, enna_input event)
   {
      //~ printf("INPUT.. to module! %d\n", event);
      return enna_list2_input_feed(o_list, event);
   }

   void
     _info_button_cb(void *data)
   {
      //~ Enna_Module *m = data;

      printf("Info clicked...TODO show module info\n");
   }

   void
     _enable_button_cb(void *data)
   {
      Enna_Module *m = data;

      if (!m) return;
      printf("Button clicked...\n");

      printf("SELECTED MOD: %s\n", m->name);
      if (m->enabled)
        enna_module_disable(m);
      else
        enna_module_enable(m);
   }

   static Evas_Object *
     _config_panel_show(void *data)
   {
      Enna_Module *m;
      Eina_List *l;

      printf("** Modules Panel Show **\n");
      if (o_list) return o_list;

      // create list
      o_list = enna_list2_add(enna->evas);
      evas_object_size_hint_align_set(o_list, -1.0, -1.0);
      evas_object_size_hint_weight_set(o_list, 1.0, 1.0);
      evas_object_show(o_list);

      // populate list
      EINA_LIST_FOREACH(_enna_modules, l, m)
        {
           Elm_Genlist_Item *item;
           item = enna_list2_append(o_list,
                                    m->api->title ? _(m->api->title) : _(m->name),
                                    m->api->short_desc ? _(m->api->short_desc) :
                                    _("No information provided"),
                                    m->api->icon ? m->api->icon : "icon/module",
                                    NULL, NULL);

           enna_list2_item_button_add(item, "icon/podcast", "info",
                                      _info_button_cb , NULL);
           //~ enna_list2_item_toggle_add(item, "icon/photo", "just for test",
           //~ _button_cb , NULL);
           enna_list2_item_check_add(item, "icon/music", "enabled",
                                     m->enabled, _enable_button_cb, m);
        }

      if (!_listener)
        _listener = enna_input_listener_add("configuration/modules",
                                            _input_events_cb, NULL);

      return o_list;
   }

   static void
     _config_panel_hide(void *data)
   {
      printf("** Modules Panel Hide **\n");
      enna_input_listener_del(_listener);
      _listener = NULL;
      ENNA_OBJECT_DEL(o_list);
   }
#endif
