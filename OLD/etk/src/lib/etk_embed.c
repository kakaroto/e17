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

/** @file etk_embed.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_embed.h"

#include <stdlib.h>

#include "etk_main.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Embed
 * @{
 */

static void _etk_embed_constructor(Etk_Embed *embed);
static void _etk_embed_evas_position_get(Etk_Toplevel *toplevel, int *x, int *y);
static void _etk_embed_screen_position_get(Etk_Toplevel *toplevel, int *x, int *y);
static void _etk_embed_size_get(Etk_Toplevel *toplevel, int *w, int *h);
static void _etk_embed_pointer_set(Etk_Toplevel *toplevel, Etk_Pointer_Type pointer_type);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Embed
 * @return Returns the type of an Etk_Embed
 */
Etk_Type *etk_embed_type_get(void)
{
   static Etk_Type *embed_type = NULL;

   if (!embed_type)
   {
      embed_type = etk_type_new("Etk_Embed", ETK_TOPLEVEL_TYPE,
         sizeof(Etk_Embed),
         ETK_CONSTRUCTOR(_etk_embed_constructor), NULL, NULL);
   }

   return embed_type;
}

/**
 * @brief Creates a new embed widget
 * @param evas the Evas where the embed object should belong
 * @return Returns the new embed widget, or NULL on failure (most probably because the Evas is invalid)
 */
Etk_Widget *etk_embed_new(Evas *evas)
{
   Etk_Widget *embed;

   if (!evas)
      return NULL;

   embed = etk_widget_new(ETK_EMBED_TYPE, NULL);
   ETK_TOPLEVEL(embed)->evas = evas;
   ETK_TOPLEVEL(embed)->evas_position_get = _etk_embed_evas_position_get;
   ETK_TOPLEVEL(embed)->screen_position_get = _etk_embed_screen_position_get;
   ETK_TOPLEVEL(embed)->size_get = _etk_embed_size_get;
   ETK_TOPLEVEL(embed)->pointer_set = _etk_embed_pointer_set;

   /* TODO: FIXME: We need that to force the widget to realize... */
   etk_object_properties_set(ETK_OBJECT(embed), "theme-group", "", NULL);

   /* If the widget has failed to realize, we destroy it, and we return NULL */
   if (!(ETK_WIDGET(embed)->smart_object))
   {
      ETK_WARNING("The embed widget could not be created, the Evas seems invalid");
      etk_object_destroy(ETK_OBJECT(embed));
      return NULL;
   }

   /* Force the children of the embed to be realized */
   etk_object_notify(ETK_OBJECT(embed), "evas");

   return embed;
}

/**
 * @brief Gets the smart-object of the embed widget. This object can be manipulated like the other Evas objects,
 * with evas_object_move(), evas_object_resize(), evas_object_clip_set(), ...
 * @param embed an embed widget
 * @return Returns the smart-object of the embed widget
 */
Evas_Object *etk_embed_object_get(Etk_Embed *embed)
{
   if (!embed)
      return NULL;
   return ETK_WIDGET(embed)->smart_object;
}

/**
 * @brief Sets the function to call to get the position of the Evas where the embed widget belongs, relative to the
 * screen. This is used to place correctly the menus and the combobox windows in the embed widget, so if the embed
 * widget does not contain a menu bar or a combobox, you don't need to call this function
 * @param embed an embed widget
 * @param position_get the function to call to get the position of top-left corner of the Evas. The
 * returned position should be relative to the top-left corner of the screen
 * @param position_data the data to pass as the first param when @a position_get is called. It can be set to NULL
 */
void etk_embed_position_method_set(Etk_Embed *embed, void (*position_get)(void *position_data, int *x, int *y), void *position_data)
{
   if (!embed)
      return;

   embed->position_get = position_get;
   embed->position_data = position_data;
}

/**
 * @brief Sets the function to call to set the current mouse pointer used by the Embed.
 * This is used to change the pointer when the mouse is over an entry for example, so if the embed
 * widget does not contain a widget that makes the pointer change (such as an entry or a tree), you don't
 * need to call this function
 * @param embed an embed widget
 * @param pointer_set the function to call to set the current mouse pointer
 * @param pointer_data the data to pass as the first param when @a pointer_set is called. It can be set to NULL
 */
void etk_embed_pointer_method_set(Etk_Embed *embed, void (*pointer_set)(void *pointer_data, Etk_Pointer_Type pointer_type), void *pointer_data)
{
   if (!embed)
      return;

   embed->pointer_set = pointer_set;
   embed->pointer_data = pointer_data;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the embed widget */
static void _etk_embed_constructor(Etk_Embed *embed)
{
   if (!embed)
      return;

   embed->position_get = NULL;
   embed->position_data = NULL;
   embed->pointer_set = NULL;
   embed->pointer_data = NULL;
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Gets the position of the embed widget, relative to the Evas where it belongs */
static void _etk_embed_evas_position_get(Etk_Toplevel *toplevel, int *x, int *y)
{
   etk_widget_geometry_get(ETK_WIDGET(toplevel), x, y, NULL, NULL);
}

/* Gets the position of the embed widget, relative to the screen */
static void _etk_embed_screen_position_get(Etk_Toplevel *toplevel, int *x, int *y)
{
   Etk_Embed *embed;
   int win_x, win_y;

   if (!(embed = ETK_EMBED(toplevel)))
      return;

   etk_widget_geometry_get(ETK_WIDGET(embed), x, y, NULL, NULL);
   if (embed->position_get)
   {
      embed->position_get(embed->position_data, &win_x, &win_y);
      if (x)
         *x += win_x;
      if (y)
         *y += win_y;
   }
}

/* Gets the size of the embed widget */
static void _etk_embed_size_get(Etk_Toplevel *toplevel, int *w, int *h)
{
   etk_widget_geometry_get(ETK_WIDGET(toplevel), NULL, NULL, w, h);
}

/* Sets the current mouse-pointer */
static void _etk_embed_pointer_set(Etk_Toplevel *toplevel, Etk_Pointer_Type pointer_type)
{
   Etk_Embed *embed;

   if (!(embed = ETK_EMBED(toplevel)))
      return;

   if (embed->pointer_set)
      embed->pointer_set(embed->pointer_data, pointer_type);
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Embed
 *
 * @image html widgets/embed.png
 *
 * The embed widget allows you to add Etk widgets in your Evas programs, without having to create an Etk_Window. @n
 * The embed widget is created with etk_embed_new() which only requires the Evas where to create the embed object. You
 * can then get the corresponding Evas object with etk_embed_object_get(). This object can be manipulated as any other
 * Evas objects, with evas_object_move(), evas_object_resize(), evas_object_show(), ... @n
 *
 * If the embed widget contains widgets that makes popup-windows pop up (such as a combobox or a menubar), you should
 * also call etk_embed_position_method_set() to set the function to call to get the position of the Evas where the embed
 * object belongs. Without this function, the menus and the combobox-windows will pop up at an incorrect position. @n
 * Also, if the embed widget contains widgets that may change the mouse pointer (such as an entry or a tree), you have
 * to call etk_embed_pointer_method_set() to set the function to call to change the current mouse pointer. Otherwise,
 * the mouse pointer will not be changed. @n
 *
 * Here is an example that adds a colorpicker in an existing Ecore_Evas:
 * @code
 * //This function gets the position of the window containing the embed object.
 * //This is needed only if the embed widget uses popup-windows so this is unused in this example since
 * //the embed widget only contains a colorpicker
 * static void window_position_get(void *window_data, int *x, int *y)
 * {
 *    ecore_evas_geometry_get(window_data, x, y, NULL, NULL);
 * }
 *
 * //This function creates a colorpicker in the given Ecore_Evas
 * void create_colorpicker(Ecore_Evas *ee)
 * {
 *    Evas *evas;
 *    Etk_Widget *embed, *colorpicker;
 *    Evas_Object *embed_object;
 *
 *    //Create the embed widget
 *    evas = ecore_evas_get(ee);
 *    embed = etk_embed_new(evas);
 *    etk_embed_position_method_set(ETK_EMBED(embed, window_position_get, ee);
 *
 *    //Pack a colorpicker into the embed widget
 *    colorpicker = etk_colorpicker_new();
 *    etk_container_add(ETK_CONTAINER(embed), colorpicker);
 *    etk_widget_show_all(embed);
 *
 *    //Get the Evas object corresponding to the embed widget.
 *    //Then, we place it at (10,10) and resize it to 250x150
 *    embed_object = etk_embed_object_get(ETK_EMBED(embed));
 *    evas_object_move(embed_object, 10, 10);
 *    evas_object_resize(embed_object, 250, 150);
 *    //Note: evas_object_show(embed) is not required here since
 *    //etk_widget_show_all(embed) shows automatically the embed object
 * }
 * @endcode @n
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Toplevel
 *           - Etk_Embed
 */
