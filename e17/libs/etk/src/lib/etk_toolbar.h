/** @file etk_toolbar.h */
#ifndef _ETK_TOOLBAR_H_
#define _ETK_TOOLBAR_H_

#include "etk_widget.h"
#include "etk_box.h"
#include "etk_types.h"
#include "etk_utils.h"
#include "etk_stock.h"

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
   ETK_TOOLBAR_HORIZ,
   ETK_TOOLBAR_VERT,
} Etk_Toolbar_Orientation;

/**
 * @enum Etk_Toolbar_Style
 * @brief The style a toolbar has (icons, text, both, both horizontally)
 */
typedef enum _Etk_Toolbar_Style
{
   ETK_TOOLBAR_ICONS,            /**< Only show icons */
   ETK_TOOLBAR_TEXT,             /**< Only show text */
   ETK_TOOLBAR_BOTH_VERT,        /**< Show icons and text, vertically */
   ETK_TOOLBAR_BOTH_HORIZ,       /**< Show icons and text, horizontally */
   ETK_TOOLBAR_DEFAULT           /**< Use Etk's default policy */
} Etk_Toolbar_Style;

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
   Etk_Toolbar_Orientation orientation;
   Etk_Toolbar_Style style;

   Etk_Stock_Size stock_size;
};

Etk_Type *etk_toolbar_type_get();

Etk_Widget *etk_toolbar_new();

void etk_toolbar_append(Etk_Toolbar *toolbar, Etk_Widget *widget);
void etk_toolbar_prepend(Etk_Toolbar *toolbar, Etk_Widget *widget);

void etk_toolbar_orientation_set(Etk_Toolbar *toolbar, Etk_Toolbar_Orientation orientation);
Etk_Toolbar_Orientation etk_toolbar_orientation_get(Etk_Toolbar *toolbar);

void etk_toolbar_style_set(Etk_Toolbar *toolbar, Etk_Toolbar_Style style);
Etk_Toolbar_Style etk_toolbar_style_get(Etk_Toolbar *toolbar);

void etk_toolbar_stock_size_set(Etk_Toolbar *toolbar, Etk_Stock_Size size);
Etk_Stock_Size etk_toolbar_stock_size_get(Etk_Toolbar *toolbar);

/** @} */

#endif
