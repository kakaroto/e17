#ifndef ENGRAVE_COLOR_CLASS_H
#define ENGRAVE_COLOR_CLASS_H

/**
 * @file engrave_color_class.h Engrave_Color_Class block 
 * @brief Contains all of the functions to maniuplate Engrave_Color_Class blocks
 */

/**
 * @defgroup Engrave_Color_Class Engrave_Color_Class: Functions to work with engrave color_class objects
 *
 * @{
 */

/**
 * The Engrave_Color_Class typedef
 */
typedef struct _Engrave_Color_Class Engrave_Color_Class;
typedef struct _Engrave_Color_Class_Color Engrave_Color_Class_Color;

/**
 * @brief Stores the needed color_class information.
 */
struct _Engrave_Color_Class
{
  char *name; /**< The color_class alias */
  void * parent; /**< The color_class parent */
  Engrave_Color_Class_Color * color;
  Engrave_Color_Class_Color * color2;
  Engrave_Color_Class_Color * color3;
};

/**
 * @brief Stores the color_class color information.
 */
struct _Engrave_Color_Class_Color
{
  int r;
  int g;
  int b;
  int a;
};

EAPI Engrave_Color_Class *engrave_color_class_new();
EAPI void engrave_color_class_free(Engrave_Color_Class *ecc);

EAPI Engrave_Color_Class_Color *engrave_color_class_color_new(int r, int g, int b, int a);
EAPI void engrave_color_class_color_free(Engrave_Color_Class_Color *ecc);

EAPI void engrave_color_class_parent_set(Engrave_Color_Class *ecc, void *parent);

EAPI void engrave_color_class_color_set(Engrave_Color_Class *ecc, Engrave_Color_Class_Color * col, int num);
EAPI void engrave_color_class_color_del(Engrave_Color_Class *ecc, Engrave_Color_Class_Color * col, int num);

EAPI const char * engrave_color_class_name_get(Engrave_Color_Class *ecc);
EAPI void engrave_color_class_name_set(Engrave_Color_Class *ecc, const char *name);


/**
 * @}
 */

#endif

