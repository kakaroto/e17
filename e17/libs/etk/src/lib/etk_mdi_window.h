/** @file etk_mdi_window.h */
#ifndef _ETK_MDI_WINDOW_H_
#define _ETK_MDI_WINDOW_H_

#include <Evas.h>
#include <Ecore.h>

#include "etk_toplevel.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Mdi_Window Etk_Mdi_Window
 * @brief A toplevel-widget in a mdi area
 * @{
 */

/** Gets the type of a window */
#define ETK_MDI_WINDOW_TYPE    (etk_mdi_window_type_get())
/** Casts the object to an Etk_Mdi_Window */
#define ETK_MDI_WINDOW(obj)    (ETK_OBJECT_CAST((obj), ETK_MDI_WINDOW_TYPE, Etk_Mdi_Window))
/** Check if the object is an Etk_Mdi_Window */
#define ETK_IS_MDI_WINDOW(obj) (ETK_OBJECT_CHECK_TYPE((obj), ETK_MDI_WINDOW_TYPE))

typedef struct _Etk_Mdi_Window_Data Etk_Mdi_Window_Data;

/**
 * @brief @widget A toplevel-widget in a mdi area
 * @structinfo
 */
struct _Etk_Mdi_Window
{
   /* Inherit from Etk_Toplevel */
   Etk_Toplevel toplevel;

   Etk_Bool (*delete_event)(Etk_Mdi_Window *window);

   /* private: */
   char *wm_theme_file;
   Etk_Bool wait_size_request;
   Etk_Mdi_Window_Data *data;
};

Etk_Type   *etk_mdi_window_type_get(void);
Etk_Widget *etk_mdi_window_new(Evas *evas);

void        etk_mdi_window_delete_request(Etk_Mdi_Window *window);

void        etk_mdi_window_title_set(Etk_Mdi_Window *window, const char *title);
const char *etk_mdi_window_title_get(Etk_Mdi_Window *window);

void        etk_mdi_window_move(Etk_Mdi_Window *window, int x, int y);
void        etk_mdi_window_resize(Etk_Mdi_Window *window, int w, int h);
void        etk_mdi_window_geometry_get(Etk_Mdi_Window *window, int *x, int *y, int *w, int *h);
void        etk_mdi_window_modal_for_window(Etk_Mdi_Window *window_to_modal, Etk_Mdi_Window *window);

void        etk_mdi_window_raise(Etk_Mdi_Window *window);
void        etk_mdi_window_lower(Etk_Mdi_Window *window);

void        etk_mdi_window_maximized_set(Etk_Mdi_Window *window, Etk_Bool maximized);
Etk_Bool    etk_mdi_window_maximized_get(Etk_Mdi_Window *window);

void        etk_mdi_window_decorated_set(Etk_Mdi_Window *window, Etk_Bool decorated);
Etk_Bool    etk_mdi_window_decorated_get(Etk_Mdi_Window *window);

Etk_Bool    etk_mdi_window_hide_on_delete(Etk_Object *window, void *data);

/** @} */

#endif
