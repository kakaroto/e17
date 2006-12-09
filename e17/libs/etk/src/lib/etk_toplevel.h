/** @file etk_toplevel.h */
#ifndef _ETK_TOPLEVEL_H_
#define _ETK_TOPLEVEL_H_

#include <Evas.h>
#include "etk_bin.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Toplevel Etk_Toplevel
 * @brief A toplevel widget is a widget that can't have a parent (such as a window or an embed widget)
 * @{
 */

/** Gets the type of a toplevel widget */
#define ETK_TOPLEVEL_TYPE       (etk_toplevel_type_get())
/** Casts the object to an Etk_Toplevel */
#define ETK_TOPLEVEL(obj)       (ETK_OBJECT_CAST((obj), ETK_TOPLEVEL_TYPE, Etk_Toplevel))
/** Checks if the object is an Etk_Toplevel */
#define ETK_IS_TOPLEVEL(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TOPLEVEL_TYPE))

/** @brief The different types of mouse pointer */
typedef enum Etk_Pointer_Type
{
   ETK_POINTER_NONE,
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
 * @brief @widget A widget that can't have a parent
 * @structinfo
 */
struct Etk_Toplevel
{
   /* private: */
   /* Inherit from Etk_Bin */
   Etk_Bin bin;

   Evas *evas;
   void (*evas_position_get)(Etk_Toplevel *toplevel, int *x, int *y);
   void (*screen_position_get)(Etk_Toplevel *toplevel, int *x, int *y);
   void (*size_get)(Etk_Toplevel *toplevel, int *w, int *h); 

   Etk_Widget *focused_widget;

   Evas_List *pointer_stack;
   void (*pointer_set)(Etk_Toplevel *toplevel, Etk_Pointer_Type pointer_type);
   
   Etk_Bool need_update;
};


Etk_Type *etk_toplevel_type_get();

Evas *etk_toplevel_evas_get(Etk_Toplevel *toplevel);

void etk_toplevel_evas_position_get(Etk_Toplevel *toplevel, int *x, int *y);
void etk_toplevel_screen_position_get(Etk_Toplevel *toplevel, int *x, int *y);
void etk_toplevel_size_get(Etk_Toplevel *toplevel, int *w, int *h);

void        etk_toplevel_focused_widget_set(Etk_Toplevel *toplevel, Etk_Widget *widget);
Etk_Widget *etk_toplevel_focused_widget_get(Etk_Toplevel *toplevel);
Etk_Widget *etk_toplevel_focused_widget_next_get(Etk_Toplevel *toplevel);
Etk_Widget *etk_toplevel_focused_widget_prev_get(Etk_Toplevel *toplevel);

void etk_toplevel_pointer_push(Etk_Toplevel *toplevel, Etk_Pointer_Type pointer_type);
void etk_toplevel_pointer_pop(Etk_Toplevel *toplevel, Etk_Pointer_Type pointer_type);

Evas_List *etk_toplevel_widgets_get(void);


/** @} */

#endif
