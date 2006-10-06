/** @file etk_embed.c */
#include "etk_embed.h"
#include <stdlib.h>
#include "etk_main.h"
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "config.h"

/**
 * @addtogroup Etk_Embed
 * @{
 */

static void _etk_embed_constructor(Etk_Embed *embed);
static void _etk_embed_evas_position_get(Etk_Toplevel *toplevel, int *x, int *y);
static void _etk_embed_screen_position_get(Etk_Toplevel *toplevel, int *x, int *y);
static void _etk_embed_size_get(Etk_Toplevel *toplevel, int *w, int *h);

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
Etk_Type *etk_embed_type_get()
{
   static Etk_Type *embed_type = NULL;

   if (!embed_type)
   {
      embed_type = etk_type_new("Etk_Embed", ETK_TOPLEVEL_TYPE, sizeof(Etk_Embed),
         ETK_CONSTRUCTOR(_etk_embed_constructor), NULL);
   }

   return embed_type;
}

/**
 * @brief Creates a new embed widget
 * @param evas the evas where the embed object should belong
 * @param window_position_get the function to call to know the top-left corner of the window containing the evas. @n
 * It's used to place correctly the menus and the combobox windows. If the embed widget does not contain a menu bar
 * or a combobox, this can be set to NULL.
 * @param window_data the data to pass as the first param when @a window_position_get is called. It can be set to NULL.
 * @return Returns the new embed widget, or NULL on failure (if the evas is invalid)
 */
Etk_Widget *etk_embed_new(Evas *evas, void (*window_position_get)(void *window_data, int *x, int *y), void *window_data)
{
   Etk_Widget *embed;
   
   if (!evas)
      return NULL;
   
   embed = etk_widget_new(ETK_EMBED_TYPE, NULL);
   ETK_EMBED(embed)->window_position_get = window_position_get;
   ETK_EMBED(embed)->window_data = window_data;
   ETK_TOPLEVEL(embed)->evas = evas;
   ETK_TOPLEVEL(embed)->evas_position_get = _etk_embed_evas_position_get;
   ETK_TOPLEVEL(embed)->screen_position_get = _etk_embed_screen_position_get;
   ETK_TOPLEVEL(embed)->size_get = _etk_embed_size_get;
   
   /* TODO: remove font path */
   evas_font_path_append(evas, PACKAGE_DATA_DIR "/fonts/");
   /* TODO: FIXME: We need that to force the widget to realize... */
   etk_object_properties_set(ETK_OBJECT(embed), "theme_group", "", NULL);
   
   /* If the widget has failed to realize, we destroy it, and we return NULL */
   if (!(ETK_WIDGET(embed)->smart_object))
   {
      ETK_WARNING("The embed widget could not be created, the evas seems invalid");
      etk_object_destroy(ETK_OBJECT(embed));
      return NULL;
   }
   
   /* TODO: FIXME: This is dirty too... used to first the children of the embed to be realized */
   etk_object_notify(ETK_OBJECT(embed), "evas");
   
   return embed;
}

/**
 * @brief Gets the smart-object of the embed widget. This object can be manipulated like the other evas objects,
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
   
   embed->window_position_get = NULL;
   embed->window_data = NULL;
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Gets the evas position of the embed widget */
static void _etk_embed_evas_position_get(Etk_Toplevel *toplevel, int *x, int *y)
{
   etk_widget_geometry_get(ETK_WIDGET(toplevel), x, y, NULL, NULL);
}

/* Gets the screen position of the embed widget */
static void _etk_embed_screen_position_get(Etk_Toplevel *toplevel, int *x, int *y)
{
   Etk_Embed *embed;
   int win_x, win_y;
   
   if (!(embed = ETK_EMBED(toplevel)))
      return;
   
   etk_widget_geometry_get(ETK_WIDGET(embed), x, y, NULL, NULL);
   if (embed->window_position_get)
   {
      embed->window_position_get(embed->window_data, &win_x, &win_y);
      x += win_x;
      y += win_y;
   }
}

/* Gets the size of the embed widget */
static void _etk_embed_size_get(Etk_Toplevel *toplevel, int *w, int *h)
{
   etk_widget_geometry_get(ETK_WIDGET(toplevel), NULL, NULL, w, h);
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
 * The embed widget is created with etk_embed_new() which requires a valid Evas and, optionally, a function to call
 * to get the position of the window containing the Evas. This function is used to know where the popup-windows should
 * be popped up. Thus, this is not required if the embed widget doesn't make any window pop up (i.e. if it contains no
 * menu-bar, no combobox, ...). @n
 * Here is an example that adds a colorpicker in an existing Ecore_Evas:
 * @code
 * //This function gets the position of the window containing the embed object.
 * //This is needed only if the embed widget uses popup-windows. This is so unused in this example
 * static void _window_position_get(void *window_data, int *x, int *y)
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
 *    evas = ecore_evas_get(ee);
 *    embed = etk_embed_new(evas, _window_position_get, ee);
 *    colorpicker = etk_colorpicker_new();
 *    etk_container_add(ETK_CONTAINER(embed), colorpicker);
 *    etk_widget_show_all(embed);
 *
 *    embed_object = etk_embed_object_get(ETK_EMBED(embed));
 *    evas_object_move(embed_object, 10, 10);
 *    evas_object_resize(embed_object, 250, 150);
 *    //Note: evas_object_show(embed) is not required here since
 *    //"etk_widget_show_all(embed);" shows automatically the embed object
 * }
 * @endcode @n
 * 
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Toplevel
 *       - Etk_Embed
 */
