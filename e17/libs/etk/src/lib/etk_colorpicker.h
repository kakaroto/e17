/** @file etk_colorpicker.h */
#ifndef __ETK_COLORPICKER_H__
#define __ETK_COLORPICKER_H__

#include "etk_box.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Colorpicker Etk_Colorpicker
 * @{
 */

/** @brief Gets the type of a colorpicker */
#define ETK_COLORPICKER_TYPE        (etk_colorpicker_type_get())
/** @brief Casts the object to a colorpicker */
#define ETK_COLORPICKER(obj)        (ETK_OBJECT_CAST((obj), ETK_COLORPICKER_TYPE, Etk_Colorpicker))
/** @brief Check if the object is an Etk_Colorpicker */
#define ETK_IS_COLORPICKER(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_COLORPICKER_TYPE))

/**
 * @enum Etk_Color_Mode
 * @brief The color modes used by the colorpicker
 */
enum _Etk_Color_Mode
{
   ETK_COLOR_MODE_H, 
   ETK_COLOR_MODE_S,
   ETK_COLOR_MODE_V,
   ETK_COLOR_MODE_R,
   ETK_COLOR_MODE_G,
   ETK_COLOR_MODE_B
};

struct _Etk_Colorpicker
{
  Etk_HBox hbox;
  
  Etk_Widget *cps;
  Etk_Widget *cpv;
  Etk_Widget *radio[6];
  
  Etk_Color_Mode color_mode;
  
  Etk_Color color;
};

Etk_Type *etk_colorpicker_type_get();
Etk_Widget *etk_colorpicker_new();

void etk_colorpicker_color_mode_set(Etk_Colorpicker *cp, Etk_Color_Mode color_mode);
Etk_Color_Mode etk_colorpicker_color_mode_get(Etk_Colorpicker *cp);

Etk_Color etk_colorpicker_color_get(Etk_Colorpicker *cp);

void etk_colorpicker_rgb_to_hsv(Etk_Color color, double *h, double *s, double *v);
void etk_colorpicker_hsv_to_rgb(double h, double s, double v, Etk_Color *color);

/** @} */


#endif /*  __ETK_COLORPICKER_H__ */
