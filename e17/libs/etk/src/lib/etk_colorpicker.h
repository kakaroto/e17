/** @file etk_colorpicker.h */
#ifndef __ETK_COLORPICKER_H__
#define __ETK_COLORPICKER_H__

#include <etk_widget.h>
#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Colorpicker Etk_Colorpicker
 * @brief TODO: doc
 * @{
 */

/** Gets the type of a colorpicker */
#define ETK_COLORPICKER_TYPE        (etk_colorpicker_type_get())
/** Casts the object to a colorpicker */
#define ETK_COLORPICKER(obj)        (ETK_OBJECT_CAST((obj), ETK_COLORPICKER_TYPE, Etk_Colorpicker))
/** Check if the object is an Etk_Colorpicker */
#define ETK_IS_COLORPICKER(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_COLORPICKER_TYPE))

/** @brief The color modes used by the colorpicker */
typedef enum Etk_Colorpicker_Mode
{
   ETK_COLORPICKER_H,     /**< The "Hue" mode */
   ETK_COLORPICKER_S,     /**< The "Saturation" mode */
   ETK_COLORPICKER_V,     /**< The "Value" mode */
   ETK_COLORPICKER_R,     /**< The "Red" mode */
   ETK_COLORPICKER_G,     /**< The "Green" mode */
   ETK_COLORPICKER_B      /**< The "Blue" mode */
} Etk_Colorpicker_Mode;

/**
 * @brief @widget The structure of a colorpicker
 * @structinfo
 */
struct Etk_Colorpicker
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;
   
   Evas_Object *picker_theme_object;
   
   /* Square picker */
   Evas_Object *sp_object;
   Evas_Object *sp_image;
   Evas_Object *sp_hcursor;
   Evas_Object *sp_vcursor;
   int sp_res;
   float sp_xpos, sp_ypos;
   Etk_Bool sp_dragging;
   Etk_Bool sp_image_needs_update;
   Etk_Bool sp_cursor_needs_update;
   
   /* Vertical picker */
   Evas_Object *vp_object;
   Evas_Object *vp_image;
   Evas_Object *vp_cursor;
   int vp_res;
   float vp_pos;
   Etk_Bool vp_dragging;
   Etk_Bool vp_image_needs_update;
   Etk_Bool vp_cursor_needs_update;
   
   Etk_Widget *table;
   Etk_Widget *radios[6];
   Etk_Widget *sliders[6];
   Etk_Widget *value_labels[6];
   
   Etk_Colorpicker_Mode mode;
   Etk_Color current_color;
};

Etk_Type *etk_colorpicker_type_get();
Etk_Widget *etk_colorpicker_new();

void etk_colorpicker_mode_set(Etk_Colorpicker *cp, Etk_Colorpicker_Mode mode);
Etk_Colorpicker_Mode etk_colorpicker_mode_get(Etk_Colorpicker *cp);

void etk_colorpicker_current_color_set(Etk_Colorpicker *cp, Etk_Color color);
Etk_Color etk_colorpicker_current_color_get(Etk_Colorpicker *cp);

/** @} */

#endif
