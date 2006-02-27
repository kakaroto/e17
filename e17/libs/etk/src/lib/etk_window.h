/** @file etk_window.h */
#ifndef _ETK_WINDOW_H_
#define _ETK_WINDOW_H_

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include "etk_toplevel_widget.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Window Etk_Window
 * @{
 */

/** @brief Gets the type of a window */
#define ETK_WINDOW_TYPE       (etk_window_type_get())
/** @brief Casts the object to an Etk_Window */
#define ETK_WINDOW(obj)       (ETK_OBJECT_CAST((obj), ETK_WINDOW_TYPE, Etk_Window))
/** @brief Check if the object is an Etk_Window */
#define ETK_IS_WINDOW(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_WINDOW_TYPE))

/**
 * @enum Etk_Window_Position
 * @brief The position of a window
 */
enum _Etk_Window_Position
{
   ETK_WINDOW_POSITION_CENTER,
   ETK_WINDOW_POSITION_MOUSE     
};

struct _Etk_Window
{
   /* private: */
   /* Inherit from Etk_Toplevel_Widget */
   Etk_Toplevel_Widget toplevel_widget;

   Ecore_Evas *ecore_evas;
   Ecore_X_Window x_window;
   
   Etk_Window_Position position;
   Etk_Window *parent;
   
   Etk_Bool (*delete_event)(Etk_Window *window);
};

Etk_Type *etk_window_type_get();
Etk_Widget *etk_window_new();

void etk_window_title_set(Etk_Window *window, const char *title);
void etk_window_wmclass_set(Etk_Window *window, const char *window_name, const char *window_class);

void etk_window_move(Etk_Window *window, int x, int y);
void etk_window_resize(Etk_Window *window, int w, int h);
void etk_window_geometry_get(Etk_Window *window, int *x, int *y, int *w, int *h);

void etk_window_iconify(Etk_Window *window);
void etk_window_deiconify(Etk_Window *window);
void etk_window_maximize(Etk_Window *window);
void etk_window_unmaximize(Etk_Window *window);
void etk_window_fullscreen(Etk_Window *window);
void etk_window_unfullscreen(Etk_Window *window);
void etk_window_stick(Etk_Window *window);
void etk_window_unstick(Etk_Window *window);
void etk_window_focus(Etk_Window *window);
void etk_window_unfocus(Etk_Window *window);

void     etk_window_decorated_set(Etk_Window *window, Etk_Bool decorated);
Etk_Bool etk_window_decorated_get(Etk_Window *window);
void     etk_window_shaped_set(Etk_Window *window, Etk_Bool shaped);
Etk_Bool etk_window_shaped_get(Etk_Window *window);

void     etk_window_skip_taskbar_hint_set(Etk_Window *window, Etk_Bool skip_taskbar_hint);
Etk_Bool etk_window_skip_taskbar_hint_get(Etk_Window *window);
void     etk_window_skip_pager_hint_set(Etk_Window *window, Etk_Bool skip_pager_hint);
Etk_Bool etk_window_skip_pager_hint_get(Etk_Window *window);

void     etk_window_dnd_aware_set(Etk_Window *window, Etk_Bool on);

Etk_Bool etk_window_hide_on_delete(Etk_Object *window, void *data);

void     etk_window_position_set(Etk_Window *window, Etk_Window_Position position);
void     etk_window_parent_set(Etk_Window *window, Etk_Window *parent);
  
/** @} */

#endif
