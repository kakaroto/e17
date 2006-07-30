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
 * @enum Etk_Pointer_Type
 * @brief The different types of mouse pointer
 */
typedef enum Etk_Pointer_Type
{
   ETK_POINTER_DEFAULT,
   ETK_POINTER_MOVE,
   ETK_POINTER_H_DOUBLE_ARROW,
   ETK_POINTER_V_DOUBLE_ARROW,
   ETK_POINTER_RESIZE,
   ETK_POINTER_RESIZE_TL,
   ETK_POINTER_RESIZE_T,
   ETK_POINTER_RESIZE_TR,
   ETK_POINTER_RESIZE_R,
   ETK_POINTER_RESIZE_BR,
   ETK_POINTER_RESIZE_B,
   ETK_POINTER_RESIZE_BL,
   ETK_POINTER_RESIZE_L,
   ETK_POINTER_TEXT_EDIT,
   ETK_POINTER_DND_DROP
} Etk_Pointer_Type;

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
   void (*evas_position_get)(Etk_Toplevel_Widget *toplevel_widget, int *x, int *y);
   void (*screen_position_get)(Etk_Toplevel_Widget *toplevel_widget, int *x, int *y);
   void (*size_get)(Etk_Toplevel_Widget *toplevel_widget, int *w, int *h); 

   Etk_Widget *focused_widget;

   Evas_List *pointer_stack;
   void (*pointer_set)(Etk_Toplevel_Widget *toplevel_widget, Etk_Pointer_Type pointer_type);
};

Etk_Type *etk_toplevel_widget_type_get();
Evas *etk_toplevel_widget_evas_get(Etk_Toplevel_Widget *toplevel_widget);

void etk_toplevel_widget_evas_position_get(Etk_Toplevel_Widget *toplevel_widget, int *x, int *y);
void etk_toplevel_widget_screen_position_get(Etk_Toplevel_Widget *toplevel_widget, int *x, int *y);
void etk_toplevel_widget_size_get(Etk_Toplevel_Widget *toplevel_widget, int *w, int *h);

void etk_toplevel_widget_focused_widget_set(Etk_Toplevel_Widget *toplevel_widget, Etk_Widget *widget);
Etk_Widget *etk_toplevel_widget_focused_widget_get(Etk_Toplevel_Widget *toplevel_widget);
Etk_Widget *etk_toplevel_widget_focused_widget_next_get(Etk_Toplevel_Widget *toplevel_widget);
Etk_Widget *etk_toplevel_widget_focused_widget_prev_get(Etk_Toplevel_Widget *toplevel_widget);

void etk_toplevel_widget_pointer_push(Etk_Toplevel_Widget *toplevel_widget, Etk_Pointer_Type pointer_type);
void etk_toplevel_widget_pointer_pop(Etk_Toplevel_Widget *toplevel_widget, Etk_Pointer_Type pointer_type);


/** @} */

#endif
