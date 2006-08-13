/** @file etk_toolbar.h */
#ifndef _ETK_TOOLBAR_H_
#define _ETK_TOOLBAR_H_

#include "etk_widget.h"
#include "etk_box.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Toolbar Etk_Toolbar
 * @brief The Etk_Toolbar widget is that can contain widgets, mainly buttons, entries, etc. 
 * packed in one direction (horizontal of vertical) 
 * @{
 */

/** Gets the type of a toolbar */
#define ETK_TOOLBAR_TYPE       (etk_toolbar_type_get())
/** Casts the object to an Etk_Toolbar */
#define ETK_TOOLBAR(obj)       (ETK_OBJECT_CAST((obj), ETK_TOOLBAR_TYPE, Etk_Toolbar))
/** Checks if the object is an Etk_Toolbar */
#define ETK_IS_TOOLBAR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TOOLBAR_TYPE))

/**
 * @enum Etk_Toolbar_Orientation
 * @brief The orientation a toolbar has.
 */
typedef enum _Etk_Toolbar_Orientation
{
   ETK_TOOLBAR_ORIENTATION_HORIZONTAL,
   ETK_TOOLBAR_ORIENTATION_VERTICAL,
} Etk_Toolbar_Orientation;

/**
 * @brief @widget The structure of a toolbar
 * @structinfo
 */
struct _Etk_Toolbar
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Etk_Widget *box;
   int orientation;
};

Etk_Type *etk_toolbar_type_get();

Etk_Widget *etk_toolbar_new();

void etk_toolbar_append(Etk_Toolbar *toolbar, Etk_Widget *widget);
void etk_toolbar_prepend(Etk_Toolbar *toolbar, Etk_Widget *widget);

/** @} */

#endif
