/** @file etk_canvas.c */
#include "etk_canvas.h"
#include <stdlib.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Canvas
* @{
 */

static void _etk_canvas_constructor(Etk_Canvas *canvas);
static void _etk_canvas_realize_cb(Etk_Object *object, void *data);

static void _etk_canvas_intercept_move_cb(void *data, Evas_Object *object, Evas_Coord x, Evas_Coord y);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Canvas
 * @return Returns the type on an Etk_Canvas
 */
Etk_Type *etk_canvas_type_get()
{
   static Etk_Type *canvas_type = NULL;

   if (!canvas_type)
   {
      canvas_type = etk_type_new("Etk_Canvas", ETK_WIDGET_TYPE, sizeof(Etk_Canvas), ETK_CONSTRUCTOR(_etk_canvas_constructor), NULL, NULL);
   }

   return canvas_type;
}

/**
 * @brief Creates a new canvas
 * @return Returns the new canvas widget
 */
Etk_Widget *etk_canvas_new()
{
   return etk_widget_new(ETK_CANVAS_TYPE, NULL);
}

/**
 * @brief Adds an evas object to the canvas
 * @param canvas a canvas
 * @param object the object to add
 * @return Returns TRUE on success. FALSE on failure, probably because the canvas and the object do not belong to the same evas
 * @note The object will be automatically deleted when the canvas will be destroyed
 */
Etk_Bool etk_canvas_object_add(Etk_Canvas *canvas, Evas_Object *object)
{
   Etk_Bool result;
   Evas_Coord x, y;

   if (!canvas || !object || (evas_object_evas_get(object) != etk_widget_toplevel_evas_get(ETK_WIDGET(canvas))))
      return FALSE;

   evas_object_geometry_get(object, &x, &y, NULL, NULL);
   evas_object_intercept_move_callback_add(object, _etk_canvas_intercept_move_cb, canvas);
   result = etk_widget_member_object_add(ETK_WIDGET(canvas), object);
   evas_object_move(object, x, y);

   return result;
}


/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the canvas */
static void _etk_canvas_constructor(Etk_Canvas *canvas)
{
   if (!canvas)
      return;

   canvas->clip = NULL;

   etk_signal_connect_after("realize", ETK_OBJECT(canvas), ETK_CALLBACK(_etk_canvas_realize_cb), NULL);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the canvas is realized */
static void _etk_canvas_realize_cb(Etk_Object *object, void *data)
{
   Evas *evas;
   Etk_Canvas *canvas;

   if (!(canvas = ETK_CANVAS(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(canvas))))
      return;

   //canvas->clip = evas_object_rectangle_add(evas);
   /* TODO */
}

/* Called when an object requests to be moved  */
static void _etk_canvas_intercept_move_cb(void *data, Evas_Object *object, Evas_Coord x, Evas_Coord y)
{
   Etk_Widget *canvas_widget;

   if (!(canvas_widget = ETK_WIDGET(data)))
      evas_object_move(object, x, y);
   else
      evas_object_move(object, x + canvas_widget->geometry.x + canvas_widget->left_padding, y + canvas_widget->geometry.y + canvas_widget->top_padding);
}

/** @} */
