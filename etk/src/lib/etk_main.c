/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_main.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_main.h"

#include <locale.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <Ecore.h>
#include <Ecore_IMF.h>
#include <Ecore_Job.h>
#include <Edje.h>

#include "etk_argument.h"
#include "etk_config.h"
#include "etk_dnd.h"
#include "etk_engine.h"
#include "etk_object.h"
#include "etk_signal.h"
#include "etk_textblock.h"
#include "etk_theme.h"
#include "etk_tooltips.h"
#include "etk_toplevel.h"
#include "etk_type.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Main
 * @{
 */

static int _etk_main_idle_enterer_cb(void *data);
static void _etk_main_size_request_recursive(Etk_Widget *widget);
static void _etk_main_size_allocate_recursive(Etk_Widget *widget, Etk_Bool is_top_level);

static Etk_Bool _etk_main_running = ETK_FALSE;
static int _etk_main_init_count = 0;
static Ecore_Idle_Enterer *_etk_main_idle_enterer = NULL;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Initializes Etk. This function needs to be called before any other call to an etk_* function. @n
 * You can call safely etk_init() several times, it will only have an effect the first time you call it. The other
 * times, it will just increment the init-counter. etk_shutdown() will decrement this counter and will effectively
 * shutdown Etk only when the counter reaches 0. So you need to call etk_shutdown() the same number of times
 * you've called etk_init().
 * @param argc the "argc" parameter passed to main(). It is used to parse the arguments specific to Etk.
 * It can be set to 0.
 * @param argv the "argv" parameter passed to main(). It is used to parse the arguments specific to Etk.
 * It can be set to NULL.
 * @return Returns the number of times Etk has been initialized, or 0 on failure
 * @note It initializes Evas, Ecore and Edje so you don't need to initialize them after an etk_init()
 * @see etk_init_full()
 * @see etk_shutdown()
 */
int etk_init(int argc, char **argv)
{
   return etk_init_full(argc, argv, NULL);
}

/**
 * @brief Does the same thing as etk_init() except you can specify custom arguments that could be then retrieved with
 * etk_argument_* functions. For example, etk_init_full(argc, argv, "--option1 value --toggle1")
 * @param argc the location of the "argc" parameter passed to main(). It is used to parse the arguments specific to Etk.
 * It can be set to NULL.
 * @param argv the location of the "argv" parameter passed to main(). It is used to parse the arguments specific to Etk.
 * It can be set to NULL.
 * @param custom_opts a string corresponding to the custom arguments to add to argv. It can be set to NULL
 * @return Returns the number of times Etk has been initialized, or 0 on failure
 * @note It initializes Evas, Ecore and Edje so you don't need to initialize them after an etk_init()
 * @see etk_init()
 * @see etk_shutdown()
 */
int etk_init_full(int argc, char **argv, const char *custom_opts)
{
   char *engine_name = NULL;

   if (_etk_main_init_count > 0)
   {
      _etk_main_init_count++;
      return _etk_main_init_count;
   }
   else
   {
      /* Parse the arguments */
      etk_argument_init(argc, argv, custom_opts);
      etk_argument_value_get("etk-engine", 0, ETK_TRUE, &engine_name);

      /* Initialize the EFL */
      if (!evas_init())
      {
         ETK_WARNING("Evas initialization failed!");
         return 0;
      }
      if (!ecore_init())
      {
         ETK_WARNING("Ecore initialization failed!");
         return 0;
      }
      if (!ecore_imf_init())
      {
         ETK_WARNING("Ecore_IMF initialization failed!");
      }
      if (!ecore_job_init())
      {
         ETK_WARNING("Ecore_Job initialization failed!");
         return 0;
      }
      if (!edje_init())
      {
         ETK_WARNING("Edje initialization failed!");
         return 0;
      }

      /* TODO: maybe we should do this in etk_main().
       * Problem: if we do this, a program that uses directly ecore_main_loop_begin() and not etk_main() won't work */
      _etk_main_idle_enterer = ecore_idle_enterer_add(_etk_main_idle_enterer_cb, NULL);

      /* Initialize the subsystems of Etk */
      if (!etk_config_init())
      {
         ETK_WARNING("Etk_Config initialization failed!");
         return 0;
      }
      etk_config_load();
      etk_theme_init();
      if (!etk_engine_init())
      {
         ETK_WARNING("Etk_Engine initialization failed!");
         return 0;
      }
      if (!etk_engine_load(engine_name ? engine_name : "ecore_evas_software_x11"))
      {
         ETK_WARNING("Etk can not load the requested engine!");
         return 0;
      }
      etk_event_init();
      etk_tooltips_init();

      /* Initialize Gettext */
      setlocale(LC_ALL, "");
      bindtextdomain(PACKAGE, LOCALEDIR);
      textdomain(PACKAGE);

      free(engine_name);
      _etk_main_init_count++;
      return _etk_main_init_count;
   }
}

/**
 * @brief Shuts down Etk. It decrements the init-counter. If the counter reaches 0, it frees all the resources used by Etk
 * @return Returns the new value of the init-counter. If 0 is returned, it means that the resources has effectively
 * been freed.
 */
int etk_shutdown(void)
{
   if (_etk_main_init_count <= 0)
      return 0;

   _etk_main_init_count--;
   if (_etk_main_init_count == 0)
   {
      ecore_idle_enterer_del(_etk_main_idle_enterer);
      _etk_main_idle_enterer = NULL;

      /* Shutdown the subsystems of Etk */
      etk_object_shutdown();
      etk_type_shutdown();
      etk_signal_shutdown();

      etk_tooltips_shutdown();
      etk_event_shutdown();
      etk_engine_shutdown();
      etk_config_shutdown();
      etk_theme_shutdown();
      etk_argument_shutdown();

      /* Shutdown the EFL*/
      edje_shutdown();
      ecore_job_shutdown();
      ecore_imf_shutdown();
      ecore_shutdown();
      evas_shutdown();
   }

   return _etk_main_init_count;
}

/**
 * @brief Runs the Etk's main loop until etk_main_quit() is called.
 * @note It calls ecore_main_loop_begin() so you should not call ecore_main_loop_begin() or ecore_main_loop_quit()
 * if you are using etk_main() in your program.
 */
void etk_main(void)
{
   if (_etk_main_init_count <= 0 || _etk_main_running)
      return;

   _etk_main_running = ETK_TRUE;
   ecore_main_loop_begin();
}

/**
 * @brief Leaves the main loop of Etk. It will quit the main loop of Ecore (ecore_main_loop_quit())
 * and will make etk_main() return.
 */
void etk_main_quit(void)
{
   if (!_etk_main_running)
      return;

   ecore_main_loop_quit();
   _etk_main_running = ETK_FALSE;
}

/**
 * @brief Runs an iteration of the main loop: it updates the widgets that need to be updated
 * @note You usually do not need to call it manually, you might want to use etk_main() instead
 */
void etk_main_iterate(void)
{
   Eina_List *l;
   Etk_Toplevel *toplevel;

   if (_etk_main_init_count <= 0)
      return;

   etk_object_purge();

   for (l = etk_toplevel_widgets_get(); l; l = l->next)
   {
      toplevel = ETK_TOPLEVEL(l->data);
      if (toplevel->need_update)
      {
         toplevel->need_update = ETK_FALSE;
         _etk_main_size_request_recursive(ETK_WIDGET(toplevel));
         _etk_main_size_allocate_recursive(ETK_WIDGET(toplevel), ETK_TRUE);
      }
   }
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Called each time the process enters the idle state: it runs an iteration */
static int _etk_main_idle_enterer_cb(void *data)
{
   etk_main_iterate();
   return 1;
}

/* Recursively requests the size of all the widgets */
static void _etk_main_size_request_recursive(Etk_Widget *widget)
{
   Eina_List *l;
   Etk_Size unused_size;

   if (!widget)
      return;

   etk_widget_size_request(widget, &unused_size);
   for (l = widget->children; l; l = l->next)
      _etk_main_size_request_recursive(ETK_WIDGET(l->data));
}

/* Recursively allocates the size of all the widgets */
static void _etk_main_size_allocate_recursive(Etk_Widget *widget, Etk_Bool is_top_level)
{
   Eina_List *l;
   Etk_Geometry geometry;

   if (!widget)
      return;

   if (is_top_level)
   {
      etk_toplevel_evas_position_get(ETK_TOPLEVEL(widget), &geometry.x, &geometry.y);
      etk_toplevel_size_get(ETK_TOPLEVEL(widget), &geometry.w, &geometry.h);
   }
   else
   {
      int left, right, top, bottom;

      etk_widget_geometry_get(widget, &geometry.x, &geometry.y, &geometry.w, &geometry.h);
      etk_widget_padding_get(widget, &left, &right, &top, &bottom);
      geometry.x -= left;
      geometry.y -= top;
      geometry.w += left + right;
      geometry.h += top + bottom;
   }
   etk_widget_size_allocate(widget, geometry);

   for (l = widget->children; l; l = l->next)
      _etk_main_size_allocate_recursive(ETK_WIDGET(l->data), ETK_FALSE);
}

/** @} */

/*************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Main
 *
 * Every Etk program should call at least two Etk functions:
 * - etk_init() which initializes Etk. If you don't call it, the program will crash when you call any Etk function
 * - etk_shutdown() which shutdowns Etk and frees the allocated resources. If you don't call it, the memory allocated
 * by Etk won't be freed. @n
 *
 * Most of the time, you will also have to call the function etk_main() which runs the main loop, and etk_main_quit()
 * which quits the main loop (for example, when the menu-item "Quit" is clicked)
 */
