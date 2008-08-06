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

/** @file etk_colorpicker.h */
#ifndef __ETK_COLORPICKER_H__
#define __ETK_COLORPICKER_H__

#include <Evas.h>

#include "etk_widget.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TODO/FIXME list:
 * - Html code somewhere
 * - Replace sliders' label by spinners? or make slider's label editable?
 * - Fix the labels of S and V
 * - Add a "Previous" color-rect below the "Current" color-rect
 */

/**
 * @defgroup Etk_Colorpicker Etk_Colorpicker
 * @brief The colorpicker is a widget that allows the user to select a color
 * @{
 */

/** Gets the type of a colorpicker */
#define ETK_COLORPICKER_TYPE        (etk_colorpicker_type_get())
/** Casts the object to a colorpicker */
#define ETK_COLORPICKER(obj)        (ETK_OBJECT_CAST((obj), ETK_COLORPICKER_TYPE, Etk_Colorpicker))
/** Check if the object is an Etk_Colorpicker */
#define ETK_IS_COLORPICKER(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_COLORPICKER_TYPE))


/** @brief The different color modes that can be used by the colorpicker */
typedef enum
{
   ETK_COLORPICKER_H,     /**< The "Hue" mode */
   ETK_COLORPICKER_S,     /**< The "Saturation" mode */
   ETK_COLORPICKER_V,     /**< The "Value" mode */
   ETK_COLORPICKER_R,     /**< The "Red" mode */
   ETK_COLORPICKER_G,     /**< The "Green" mode */
   ETK_COLORPICKER_B      /**< The "Blue" mode */
} Etk_Colorpicker_Mode;

extern int ETK_CP_COLOR_CHANGED_SIGNAL;

/**
 * @brief @widget A widget that allows the user to select a color
 * @structinfo
 */
struct Etk_Colorpicker
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Etk_Widget *main_table;
   Etk_Widget *picker_widget;

   /* Square picker */
   Evas_Object *sp_object;
   Evas_Object *sp_image;
   Evas_Object *sp_hcursor;
   Evas_Object *sp_vcursor;
   int sp_res;
   float sp_xpos, sp_ypos;
   Etk_Colorpicker_Mode sp_xcomponent, sp_ycomponent;
   Etk_Bool sp_dragging:1;
   Etk_Bool sp_image_needs_update:1;
   Etk_Bool sp_cursor_needs_update:1;

   /* Vertical picker */
   Evas_Object *vp_object;
   Evas_Object *vp_image;
   Evas_Object *vp_cursor;
   int vp_res;
   float vp_pos;
   Etk_Bool vp_dragging:1;
   Etk_Bool vp_image_needs_update:1;
   Etk_Bool vp_cursor_needs_update:1;

   /* Component widgets */
   Etk_Widget *component_table;
   Etk_Widget *radios[6];

   /* Sliders */
   Etk_Widget *sliders[6];
   Evas_Object *sliders_image[6];
   int sliders_res;
   Etk_Bool sliders_need_update:1;

   /* Alpha slider */
   Etk_Widget *alpha_slider;
   Etk_Widget *alpha_label;

   /* Current color objects */
   Etk_Widget *color_table;
   Etk_Widget *current_color_label;
   Etk_Widget *current_color_widget;
   Evas_Object *current_color_rect;

   Etk_Bool ignore_value_changed:1;
   Etk_Bool use_alpha:1;
   Etk_Colorpicker_Mode mode;
   Etk_Color current_color;
};


Etk_Type            *etk_colorpicker_type_get(void);
Etk_Widget          *etk_colorpicker_new(void);

void                 etk_colorpicker_mode_set(Etk_Colorpicker *cp, Etk_Colorpicker_Mode mode);
Etk_Colorpicker_Mode etk_colorpicker_mode_get(Etk_Colorpicker *cp);
void                 etk_colorpicker_current_color_set(Etk_Colorpicker *cp, Etk_Color color);
Etk_Color            etk_colorpicker_current_color_get(Etk_Colorpicker *cp);
void                 etk_colorpicker_use_alpha_set(Etk_Colorpicker *cp, Etk_Bool use_alpha);
Etk_Bool             etk_colorpicker_use_alpha_get(Etk_Colorpicker *cp);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
