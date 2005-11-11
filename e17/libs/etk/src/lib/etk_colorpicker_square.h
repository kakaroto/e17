/** @file etk_colorpicker_square.h */
#ifndef _ETK_COLORPICKER_SQUARE_H_
#define _ETK_COLORPICKER_SQUARE_H_

#include <Evas.h>
#include "etk_widget.h"
#include "etk_colorpicker.h"

/**
 * @defgroup Etk_Colorpicker_Square Etk_Colorpicker_Square
 * @{
 */

/** @brief Gets the type of a square colorpicker */
#define ETK_COLORPICKER_SQUARE_TYPE        (etk_colorpicker_square_type_get())
/** @brief Casts the object to a square colorpicker */
#define ETK_COLORPICKER_SQUARE(obj)        (ETK_OBJECT_CAST((obj), ETK_COLORPICKER_SQUARE_TYPE, Etk_Colorpicker_Square))
/** @brief Check if the object is an Etk_Colorpicker_Square */
#define ETK_IS_COLORPICKER_SQUARE(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_COLORPICKER_SQUARE_TYPE))

/**
 * @struct Etk_Colorpicker_Square
 * @brief Displays a square colorpicker
 */
struct _Etk_Colorpicker_Square
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Evas_Object *map;
   Evas_Object *hcursor;
   Evas_Object *vcursor;

   int map_width, map_height;

   Etk_Color_Mode color_mode;
   Etk_Color current_color;

   unsigned char cursor_x;
   unsigned char cursor_y;
   Etk_Bool drag;
};

Etk_Type *etk_colorpicker_square_type_get();
Etk_Widget *etk_colorpicker_square_new(int map_width, int map_height);

void etk_colorpicker_square_map_size_set(Etk_Colorpicker_Square *cps, int map_width, int map_height);
void etk_colorpicker_square_map_size_get(Etk_Colorpicker_Square *cps, int *map_width, int *map_height);

void etk_colorpicker_square_color_mode_set(Etk_Colorpicker_Square *cps, Etk_Color_Mode color_mode);
Etk_Color_Mode etk_colorpicker_square_color_mode_get(Etk_Colorpicker_Square *cps);

void etk_colorpicker_square_cursor_xy_set(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y);
void etk_colorpicker_square_cursor_xy_get(Etk_Colorpicker_Square *cps, unsigned char *x, unsigned char *y);

Etk_Color etk_colorpicker_square_color_get(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y);
void etk_colorpicker_square_update(Etk_Colorpicker_Square *cps);

/** @} */

#endif /* _ETK_COLORPICKER_SQUARE_H_ */
