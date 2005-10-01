/** @file etk_toplevel_widget.h */
#ifndef _ETK_TOPLEVEL_WIDGET_H_
#define _ETK_TOPLEVEL_WIDGET_H_

#include <Evas.h>
#include "etk_bin.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Toplevel_Widget Etk_Toplevel_Widget
 * @{
 */

/** @brief Gets the type of a toplevel widget */
#define ETK_TOPLEVEL_WIDGET_TYPE       (etk_toplevel_widget_type_get())
/** @brief Casts the object to an Etk_Toplevel_Widget */
#define ETK_TOPLEVEL_WIDGET(obj)       (ETK_OBJECT_CAST((obj), ETK_TOPLEVEL_WIDGET_TYPE, Etk_Toplevel_Widget))
/** @brief Checks if the object is an Etk_Toplevel_Widget */
#define ETK_IS_TOPLEVEL_WIDGET(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TOPLEVEL_WIDGET_TYPE))

/**
 * @struct Etk_Toplevel_Widget
 * @brief A toplevel widget is a widget that can't have a parent widget 
 */
struct _Etk_Toplevel_Widget
{
   /* private: */
   /* Inherit from Etk_Bin */
   Etk_Bin bin;

   Evas *evas;
   int width;
   int height;

   Etk_Widget *focused_widget;
};

Etk_Type *etk_toplevel_widget_type_get();
Evas *etk_toplevel_widget_evas_get(Etk_Toplevel_Widget *toplevel_widget);
void etk_toplevel_widget_size_get(Etk_Toplevel_Widget *toplevel_widget, int *width, int *height);

void etk_toplevel_widget_focused_widget_set(Etk_Toplevel_Widget *toplevel_widget, Etk_Widget *widget);
Etk_Widget *etk_toplevel_widget_focused_widget_get(Etk_Toplevel_Widget *toplevel_widget);
Etk_Widget *etk_toplevel_widget_focused_widget_next_get(Etk_Toplevel_Widget *toplevel_widget);
Etk_Widget *etk_toplevel_widget_focused_widget_prev_get(Etk_Toplevel_Widget *toplevel_widget);


/** @} */

#endif
