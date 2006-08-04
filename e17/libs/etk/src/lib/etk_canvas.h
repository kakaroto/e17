/** @file etk_canvas.h */
#ifndef _ETK_CANVAS_H_
#define _ETK_CANVAS_H_

#include <Evas.h>
#include "etk_widget.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Canvas Etk_Canvas
 * @brief The Etk_Canvas widget can contain any sort of Evas_Object
 * @{
 */

/** Gets the type of a canvas */
#define ETK_CANVAS_TYPE       (etk_canvas_type_get())
/** Casts the object to an Etk_Canvas */
#define ETK_CANVAS(obj)       (ETK_OBJECT_CAST((obj), ETK_CANVAS_TYPE, Etk_Canvas))
/** Checks if the object is an Etk_Canvas */
#define ETK_IS_CANVAS(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_CANVAS_TYPE))

/**
 * @brief @widget The structure of a canvas
 * @structinfo
 */
struct Etk_Canvas
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Evas_Object *clip;
   Evas_List *objects;
};

Etk_Type *etk_canvas_type_get();
Etk_Widget *etk_canvas_new();

Etk_Bool etk_canvas_object_add(Etk_Canvas *canvas, Evas_Object *object);
void etk_canvas_object_remove(Etk_Canvas *canvas, Evas_Object *object);

/** @} */

#endif
