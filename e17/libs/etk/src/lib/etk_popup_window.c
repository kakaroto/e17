/** @file etk_popup_window.c */
#include "etk_popup_window.h"
#include <stdlib.h>
#include <Ecore.h>

#include "config.h"

#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_engine.h"

/**
 * @addtogroup Etk_Popup_Window
 * @{
 */

enum Etk_Popup_Window_Signal_Id
{
   ETK_POPUP_WINDOW_POPPED_DOWN_SIGNAL,
   ETK_POPUP_WINDOW_POPPED_UP_SIGNAL,
   ETK_POPUP_WINDOW_NUM_SIGNALS
};

static void _etk_popup_window_constructor(Etk_Popup_Window *popup_window);

static Etk_Popup_Window *_etk_popup_window_focused_window = NULL;

static Etk_Signal *_etk_popup_window_signals[ETK_POPUP_WINDOW_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Popup_Window
 * @return Returns the type of an Etk_Popup_Window
 */
Etk_Type *etk_popup_window_type_get()
{
   static Etk_Type *popup_window_type = NULL;

   if (!popup_window_type)
   {
      popup_window_type = etk_type_new("Etk_Popup_Window", ETK_WINDOW_TYPE, sizeof(Etk_Popup_Window),
         ETK_CONSTRUCTOR(_etk_popup_window_constructor), NULL);

      _etk_popup_window_signals[ETK_POPUP_WINDOW_POPPED_UP_SIGNAL] = etk_signal_new("popped_up",
         popup_window_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_popup_window_signals[ETK_POPUP_WINDOW_POPPED_DOWN_SIGNAL] = etk_signal_new("popped_down",
         popup_window_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
   }

   return popup_window_type;
}

/**
 * @brief Pops up the popup window at the position (x, y)
 * @param popup_window a popup window
 * @param x the x component of the position where to pop up the popup window
 * @param y the y component of the position where to pop up the popup window
 */
void etk_popup_window_popup_at_xy(Etk_Popup_Window *popup_window, int x, int y)
{
   if (!popup_window)
      return;
   
   etk_engine_popup_window_popup_at_xy(popup_window, x, y);
   etk_signal_emit(_etk_popup_window_signals[ETK_POPUP_WINDOW_POPPED_UP_SIGNAL], ETK_OBJECT(popup_window), NULL);   
}

/**
 * @brief Pops up the popup window at the mouse pointer position
 * @param popup_window a popup window
 */
void etk_popup_window_popup(Etk_Popup_Window *popup_window)
{
   etk_engine_popup_window_popup(popup_window);
}

/**
 * @brief Pops down the popup window
 * @param popup_window a popup window
 */
void etk_popup_window_popdown(Etk_Popup_Window *popup_window)
{
   Evas_List *l;
   Evas_List **l2;
   Etk_Popup_Window *last_focused;
   
   if (!popup_window || !(l = evas_list_find_list(*etk_engine_popup_window_popped_get(), popup_window)))
     return;
   
   l2 = etk_engine_popup_window_popped_get();
   *l2 = evas_list_remove_list(*l2, l);
   last_focused = ETK_POPUP_WINDOW(evas_list_data(evas_list_last(*l2)));
   etk_popup_window_focused_window_set(last_focused);   
   
   etk_widget_hide(ETK_WIDGET(popup_window));      
   etk_engine_popup_window_popdown(popup_window);
   etk_signal_emit(_etk_popup_window_signals[ETK_POPUP_WINDOW_POPPED_DOWN_SIGNAL], ETK_OBJECT(popup_window), NULL);   
}

/**
 * @brief Pops down all the popped windows
 */
void etk_popup_window_popdown_all()
{
   Evas_List *l;
      
   while ((l = *etk_engine_popup_window_popped_get()))
      etk_popup_window_popdown(ETK_POPUP_WINDOW(l->data));
}

/**
 * @brief Gets whether the popup window is popped up
 * @param popup_window a popup window
 * @return Returns ETK_TRUE if the popup window is popped up, ETK_FALSE otherwise
 */
Etk_Bool etk_popup_window_is_popped_up(Etk_Popup_Window *popup_window)
{
   if (!popup_window || !evas_list_find(*etk_engine_popup_window_popped_get(), popup_window))
      return ETK_FALSE;
   return ETK_TRUE;
}

/**
 * @brief Sets the focused popup window. The focused window is the one which will receive the keyboard events. @n
 * When a new popup window is popped up, the popup window is automatically focused
 * @param popup_window the popup window to focus
 * @note The popup window should be popped up to be focused
 */
void etk_popup_window_focused_window_set(Etk_Popup_Window *popup_window)
{
   if (popup_window)
   {
      if (etk_popup_window_is_popped_up(popup_window))
         _etk_popup_window_focused_window = popup_window;
   }
   else if (!*etk_engine_popup_window_popped_get())
      _etk_popup_window_focused_window = NULL;
}

/**
 * @brief Gets the focused popup window
 * @return Returns the focused popup window
 */
Etk_Popup_Window *etk_popup_window_focused_window_get()
{
   return _etk_popup_window_focused_window;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the popup window */
static void _etk_popup_window_constructor(Etk_Popup_Window *popup_window)
{
   if (!popup_window)
      return;

   etk_window_decorated_set(ETK_WINDOW(popup_window), ETK_FALSE);
   etk_window_skip_taskbar_hint_set(ETK_WINDOW(popup_window), ETK_TRUE);
   etk_window_skip_pager_hint_set(ETK_WINDOW(popup_window), ETK_TRUE);
   etk_engine_popup_window_constructor(popup_window);     
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Popup_Window
 *
 * When a popup window is popped up, it grabs the keyboard and the mouse input so the user won't be able to manipulate
 * the other windows. To pop down the window, the user has to click outside of it. @n
 * If the popup window intersects one of the edges of the screen, the popup window will slide smoothly when the mouse
 * pointer reaches this edge. @n
 * You usually do not need to directly create a popup window in your programs, use Etk_Menu or Etk_Combobox instead.
 * However, Etk_Popup_Window can be useful if you are creating a new widget.
 * 
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Toplevel_Widget
 *           - Etk_Window
 *             - Etk_Popup_Window
 *
 * \par Signals:
 * @signal_name "popped_up": Emitted when the popup window is popped up
 * @signal_cb void callback(Etk_Popup_Window *popup_window, void *data)
 * @signal_arg popup_window: the popup window which has been popped up
 * @signal_data
 * \par
 * @signal_name "popped_down": Emitted when the popup window is popped down
 * @signal_cb void callback(Etk_Popup_Window *popup_window, void *data)
 * @signal_arg popup_window: the popup window which has been popped down
 * @signal_data
 */
