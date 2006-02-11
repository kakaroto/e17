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
static void _etk_canvas_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_canvas_realize_cb(Etk_Object *object, void *data);
static void _etk_canvas_unrealize_cb(Etk_Object *object, void *data);

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
      canvas_type = etk_type_new("Etk_Canvas", ETK_WIDGET_TYPE, sizeof(Etk_Canvas), ETK_CONSTRUCTOR(_etk_canvas_constructor), NULL);

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
 * @return Returns ETK_TRUE on success. ETK_FALSE on failure, probably because the canvas and the object do not belong to the same evas
 * @note The object will be automatically deleted when the canvas will be destroyed
 */
Etk_Bool etk_canvas_object_add(Etk_Canvas *canvas, Evas_Object *object)
{
   Etk_Bool result;
   Evas_Coord x, y;
   int cx, cy;

   if (!canvas || !object || (evas_object_evas_get(object) != etk_widget_toplevel_evas_get(ETK_WIDGET(canvas))))
      return ETK_FALSE;

   etk_widget_geometry_get(ETK_WIDGET(canvas), &cx, &cy, NULL, NULL);
   evas_object_geometry_get(object, &x, &y, NULL, NULL);
   result = etk_widget_member_object_add(ETK_WIDGET(canvas), object);
   evas_object_move(object, x + cx, y + cy);

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
   ETK_WIDGET(canvas)->size_allocate = _etk_canvas_size_allocate;
   etk_signal_connect("realize", ETK_OBJECT(canvas), ETK_CALLBACK(_etk_canvas_realize_cb), NULL);
   etk_signal_connect("unrealize", ETK_OBJECT(canvas), ETK_CALLBACK(_etk_canvas_unrealize_cb), NULL);
}

/* Moves and resizes the clip of the canvas */
static void _etk_canvas_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Canvas *canvas;
   
   if (!(canvas = ETK_CANVAS(widget)))
      return;
   
   evas_object_move(canvas->clip, geometry.x, geometry.y);
   evas_object_resize(canvas->clip, geometry.w, geometry.h);
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

   canvas->clip = evas_object_rectangle_add(evas);
   etk_widget_clip_set(ETK_WIDGET(canvas), canvas->clip);
}

/* Called when the canvas is unrealized */
static void _etk_canvas_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Canvas *canvas;
   
   if (!(canvas = ETK_CANVAS(object)))
      return;
   
   etk_widget_clip_unset(ETK_WIDGET(canvas));
   evas_object_del(canvas->clip);
}

/** @} */
