/** @file etk_mdi_window.h */
#ifndef _ETK_MDI_WINDOW_H_
#define _ETK_MDI_WINDOW_H_

#include "etk_bin.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Mdi_Window Etk_Mdi_Window
 * @brief A widget in a mdi area
 * @{
 */

/** Gets the type of a mdi_window */
#define ETK_MDI_WINDOW_TYPE    (etk_mdi_window_type_get())
/** Casts the object to an Etk_Mdi_Window */
#define ETK_MDI_WINDOW(obj)    (ETK_OBJECT_CAST((obj), ETK_MDI_WINDOW_TYPE, Etk_Mdi_Window))
/** Check if the object is an Etk_Mdi_Window */
#define ETK_IS_MDI_WINDOW(obj) (ETK_OBJECT_CHECK_TYPE((obj), ETK_MDI_WINDOW_TYPE))

typedef struct Etk_Mdi_Window_Data Etk_Mdi_Window_Data;

/*
typedef enum
{
   ETK_MDI_WINDOW_WM_DECORATION_NONE = 1 << 0,
   ETK_MDI_WINDOW_WM_DECORATION_MINIMIZE = 1 << 1,
   ETK_MDI_WINDOW_WM_DECORATION_MAXIMIZE = 1 << 2,
   ETK_MDI_WINDOW_WM_DECORATION_CLOSE = 1 << 3,
   ETK_MDI_WINDOW_WM_DECORATION_ALL =
      ETK_MDI_WINDOW_WM_DECORATION_MINIMIZE | ETK_MDI_WINDOW_WM_DECORATION_MAXIMIZE | ETK_MDI_WINDOW_WM_DECORATION_CLOSE
} Etk_Mdi_Window_WM_Decoration;
*/

/**
 * @brief @widget A widget in a mdi area
 * @structinfo
 */
struct Etk_Mdi_Window
{
   /* private: */
   /* Inherit from Etk_Bin */
   Etk_Bin bin;

   Etk_Bool (*delete_event)(Etk_Mdi_Window *mdi_window);

   char *title;
   Etk_Bool maximized;
   Etk_Bool draggable;
   Etk_Bool resizable;
   Etk_Bool decorated;
   /* Etk_Mdi_Window_WM_Decoration decorations; */
   Etk_Bool dragging;
   int drag_offset_x;
   int drag_offset_y;
   Etk_Bool resizing;
   int resize_offset_x;
   int resize_offset_y;
};

Etk_Type   *etk_mdi_window_type_get(void);
Etk_Widget *etk_mdi_window_new(void);

void        etk_mdi_window_delete_request(Etk_Mdi_Window *mdi_window);

void        etk_mdi_window_title_set(Etk_Mdi_Window *mdi_window, const char *title);
const char *etk_mdi_window_title_get(Etk_Mdi_Window *mdi_window);

void        etk_mdi_window_move(Etk_Mdi_Window *mdi_window, int x, int y);

/*
void        etk_mdi_window_modal_for_window(Etk_Mdi_Window *window_to_modal, Etk_Mdi_Window *mdi_window);
*/

void        etk_mdi_window_maximized_set(Etk_Mdi_Window *mdi_window, Etk_Bool maximized);
Etk_Bool    etk_mdi_window_maximized_get(Etk_Mdi_Window *mdi_window);

void        etk_mdi_window_draggable_set(Etk_Mdi_Window *mdi_window, Etk_Bool draggable);
Etk_Bool    etk_mdi_window_draggable_get(Etk_Mdi_Window *mdi_window);

void        etk_mdi_window_resizable_set(Etk_Mdi_Window *mdi_window, Etk_Bool resizable);
Etk_Bool    etk_mdi_window_resizable_get(Etk_Mdi_Window *mdi_window);

void        etk_mdi_window_decorated_set(Etk_Mdi_Window *mdi_window, Etk_Bool decorated);
Etk_Bool    etk_mdi_window_decorated_get(Etk_Mdi_Window *mdi_window);

/*
void        etk_mdi_window_decorations_set(Etk_Mdi_Window *mdi_window, Etk_Mdi_Window_WM_Decoration decorations);
Etk_Mdi_Window_WM_Decoration etk_mdi_window_decorations_get(Etk_Mdi_Window *mdi_window);
*/

Etk_Bool    etk_mdi_window_hide_on_delete(Etk_Object *mdi_window, void *data);

/** @} */

#endif
