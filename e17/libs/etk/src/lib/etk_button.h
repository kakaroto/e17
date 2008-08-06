/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_button.h */
#ifndef _ETK_BUTTON_H_
#define _ETK_BUTTON_H_

#include "etk_bin.h"
#include "etk_stock.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Button Etk_Button
 * @brief The Etk_Button widget is a widget that emits a signal when it is pressed, released or clicked
 * @{
 */

/** Gets the type of a button */
#define ETK_BUTTON_TYPE       (etk_button_type_get())
/** Casts the object to an Etk_Button */
#define ETK_BUTTON(obj)       (ETK_OBJECT_CAST((obj), ETK_BUTTON_TYPE, Etk_Button))
/** Checks if the object is an Etk_Button */
#define ETK_IS_BUTTON(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_BUTTON_TYPE))


/** @brief The button's style (icon, text, both vertically, both horizontally) */
typedef enum
{
   ETK_BUTTON_ICON,             /**< Only the icon is visible */
   ETK_BUTTON_TEXT,             /**< Only the label is visible */
   ETK_BUTTON_BOTH_HORIZ,       /**< Both the icon and the label are visible, the icon is at the left of the label */
   ETK_BUTTON_BOTH_VERT         /**< Both the icon and the label are visible, the icon is above the label */
} Etk_Button_Style;

extern int ETK_BUTTON_PRESSED_SIGNAL;
extern int ETK_BUTTON_RELEASED_SIGNAL;
extern int ETK_BUTTON_CLICKED_SIGNAL;

/**
 * @brief @widget A widget that can be pressed, released or clicked
 * @structinfo
 */
struct Etk_Button
{
   /* private: */
   /* Inherit from Etk_Bin */
   Etk_Bin bin;

   Etk_Widget *alignment;
   Etk_Widget *box;
   Etk_Image *image;
   Etk_Widget *label;

   Etk_Bool (*pressed_handler)(Etk_Button *button);
   Etk_Bool (*released_handler)(Etk_Button *button);
   Etk_Bool (*clicked_handler)(Etk_Button *button);

   Etk_Button_Style style;
   Etk_Stock_Size stock_size;
   float xalign;
   float yalign;

   Etk_Bool is_pressed:1;
   Etk_Bool ignore_image_remove:1;
};


Etk_Type        *etk_button_type_get(void);
Etk_Widget      *etk_button_new(void);
Etk_Widget      *etk_button_new_with_label(const char *label);
Etk_Widget      *etk_button_new_from_stock(Etk_Stock_Id stock_id);

void             etk_button_press(Etk_Button *button);
void             etk_button_release(Etk_Button *button);
void             etk_button_click(Etk_Button *button);

void             etk_button_label_set(Etk_Button *button, const char *label);
const char      *etk_button_label_get(Etk_Button *button);
void             etk_button_image_set(Etk_Button *button, Etk_Image *image);
Etk_Image       *etk_button_image_get(Etk_Button *button);
void             etk_button_set_from_stock(Etk_Button *button, Etk_Stock_Id stock_id);

void             etk_button_style_set(Etk_Button *button, Etk_Button_Style style);
Etk_Button_Style etk_button_style_get(Etk_Button *button);
void             etk_button_stock_size_set(Etk_Button *button, Etk_Stock_Size size);
Etk_Stock_Size   etk_button_stock_size_get(Etk_Button *button);

void             etk_button_alignment_set(Etk_Button *button, float xalign, float yalign);
void             etk_button_alignment_get(Etk_Button *button, float *xalign, float *yalign);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
