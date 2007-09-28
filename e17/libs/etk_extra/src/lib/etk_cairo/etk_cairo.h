/** @file etk_cairo.h */
#ifndef _ETK_CAIRO_H_
#define _ETK_CAIRO_H_

#include <Evas.h>
#include <Etk.h>
#include <cairo/cairo.h>

/**
 * @defgroup Etk_Cairo Etk_Cairo
 * @brief An Etk_Cairo is a widget that allows you to use Cairo to draw to it.
 * @{
 */

extern int ETK_CAIRO_REDRAW_REQUIRED_SIGNAL;

typedef struct Etk_Cairo Etk_Cairo;

/** Gets the type of an etk_cairo */
#define ETK_CAIRO_TYPE        (etk_cairo_type_get())
/** Casts the object to an Etk_Cairo */
#define ETK_CAIRO(obj)        (ETK_OBJECT_CAST((obj), ETK_CAIRO_TYPE, Etk_Cairo))
/** Check if the object is an Etk_Cairo */
#define ETK_IS_CAIRO(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_CAIRO_TYPE))

/**
 * @brief @widget A widget that allowsy you to use Cairo to draw to it.
 * @structinfo
 */
struct Etk_Cairo
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Etk_Widget *image;

   Etk_Bool (*redraw_required_handler)(Etk_Cairo *cairo);

   struct
   {
     cairo_t *cr;
     cairo_surface_t *surface;
   } cairo;

   Etk_Size min_size;
};

Etk_Type    *etk_cairo_type_get(void);
Etk_Widget  *etk_cairo_new(int min_width, int min_height);
cairo_t *etk_cairo_get(Etk_Cairo *cairo);

/** @} */

#endif

