/** @file etk_colorpicker_vertical.h */
#ifndef _ETK_COLORPICKER_VERTICAL_H_
#define _ETK_COLORPICKER_VERTICAL_H_

#include <Evas.h>
#include "etk_widget.h"
#include "etk_colorpicker.h"

/**
 * @defgroup Etk_Colorpicker_Vertical Etk_Colorpicker_Vertical
 * @{
 */

/** @brief Gets the type of a vertical colorpicker */
#define ETK_COLORPICKER_VERTICAL_TYPE        (etk_colorpicker_vertical_type_get())
/** @brief Casts the object to a vertical colorpicker */
#define ETK_COLORPICKER_VERTICAL(obj)        (ETK_OBJECT_CAST((obj), ETK_COLORPICKER_VERTICAL_TYPE, Etk_Colorpicker_Vertical))
/** @brief Check if the object is an Etk_Cpv */
#define ETK_IS_COLORPICKER_VERTICAL(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_COLORPICKER_VERTICAL_TYPE))

/**
 * @struct Etk_Colorpicker_Vertical
 * @brief Displays a vertical colorpicker
 */
struct _Etk_Colorpicker_Vertical
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Evas_Object *map;
   Evas_Object *cursor;

   int map_width;
   int map_height;

   Etk_Color_Mode color_mode;

   unsigned char cursor_pos;
   Etk_Bool drag;
};

Etk_Type *etk_colorpicker_vertical_type_get();
Etk_Widget *etk_colorpicker_vertical_new(int map_width, int map_height);

void etk_colorpicker_vertical_map_size_set(Etk_Colorpicker_Vertical *cpv, int map_width, int map_height);
void etk_colorpicker_vertical_map_size_get(Etk_Colorpicker_Vertical *cpv, int *map_width, int *map_height);

void etk_colorpicker_vertical_color_mode_set(Etk_Colorpicker_Vertical *cpv, Etk_Color_Mode color_mode);
Etk_Color_Mode etk_colorpicker_vertical_color_mode_get(Etk_Colorpicker_Vertical *cpv);

void etk_colorpicker_vertical_pos_set(Etk_Colorpicker_Vertical *cpv, unsigned char pos);
int etk_colorpicker_vertical_pos_get(Etk_Colorpicker_Vertical *cpv);

Etk_Color etk_colorpicker_vertical_color_get(Etk_Colorpicker_Vertical *cpv, unsigned char pos);

/** @} */

#endif /* _ETK_COLORPICKER_VERTICAL_H_ */
