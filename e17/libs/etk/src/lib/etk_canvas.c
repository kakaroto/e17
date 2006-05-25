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
 * @return Returns the type of an Etk_Canvas
 */
Etk_Type *etk_canvas_type_get()
{
   static Etk_Type *canvas_type = NULL;

   if (!canvas_type)
   {
      canvas_type = etk_type_new("Etk_Canvas", ETK_WIDGET_TYPE, sizeof(Etk_Canvas),
         ETK_CONSTRUCTOR(_etk_canvas_constructor), NULL);
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
 * @brief Adds an evas object to the canvas. The object will be moved to the top left corner of the canvas,
 * and will be resized to 32x32. You can then use any function of evas to control the object
 * @param canvas a canvas
 * @param object the object to add
 * @return Returns ETK_TRUE on success, or ETK_FALSE on failure
 * (probably because the canvas and the object do not belong to the same evas)
 * @note The object will be automatically deleted when the canvas is destroyed
 * @warning The object position remains relative to the window, and not to the canvas itself
 * (See the detailed description Etk_Canvas, at the top of this page, for more information)
 */
Etk_Bool etk_canvas_object_add(Etk_Canvas *canvas, Evas_Object *object)
{
   Etk_Bool result;
   int cx, cy;

   if (!canvas || !object || (evas_object_evas_get(object) != etk_widget_toplevel_evas_get(ETK_WIDGET(canvas))))
      return ETK_FALSE;

   etk_widget_geometry_get(ETK_WIDGET(canvas), &cx, &cy, NULL, NULL);
   if ((result = etk_widget_member_object_add(ETK_WIDGET(canvas), object)))
   {
      evas_object_move(object, cx, cy);
      evas_object_resize(object, 32, 32);
   }

   return result;
}

/**
 * @brief Removes an evas object from the canvas. The evas object will also be automatically hidden
 * @param canvas a canvas
 * @param object the evas object to remove
 */
void etk_canvas_object_remove(Etk_Canvas *canvas, Evas_Object *object)
{
   if (!canvas || !object)
      return;
   
   etk_widget_member_object_del(ETK_WIDGET(canvas), object);
   evas_object_hide(object);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the canvas */
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

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Canvas
 *
 * @image html widgets/canvas.png
 * To add an object to a canvas, the object and the canvas should belong to the same evas. It means the canvas has to be
 * realized when you create the objects. You can for example create the objects in a callback connected to the "realize"
 * signal of the canvas widget.
 * @code
 * etk_signal_connect("realize", ETK_OBJECT(canvas), ETK_CALLBACK(canvas_realize_cb), NULL),
 *
 * void canvas_realize_cb(Etk_Widget *canvas, void *data)
 * {
 *    Evas *evas;
 *    Evas_Object *obj;
 *
 *    evas = etk_widget_toplevel_evas_get(canvas);
 *    obj = evas_object_rectangle_add(evas);
 *    etk_canvas_object_add(canvas, obj);
 * }
 * @endcode @n
 *
 * 
 * Once an object is added to the canvas, you can use any evas_object_* functions to control it. @n
 * You just have to keep in mind that calling evas_object_move() on an object belonging to the canvas
 * will move the object relatively to the top corner of the window, and not to the top corner of the canvas itself. @n
 * So if you want to move your object to the position (200,300) inside the canvas, you first have to get the position
 * (cx, cy) of the canvas and then to move the object relatively to it: @n
 * @code
 * etk_widget_geometry_get(canvas, &cx, &cy, NULL, NULL);
 * evas_object_move(object, cx + 200, cy + 300);
 * @endcode @n
 *
 *
 * When the canvas is moved, the objects belonging to it are automatically moved with it,
 * but you might want to add a notification callback to the @a "geometry" property of the canvas widget, which will be
 * called each time the geometry of the canvas is changed. That way, you can resize the objects when the size of the
 * canvas is modified: @n
 * @code
 * etk_object_notification_callback_add(ETK_OBJECT(canvas), "geometry", canvas_geometry_changed_cb, NULL);
 *
 * void canvas_geometry_changed_cb(Etk_Object *canvas, const char *property_name, void *data)
 * {
 *    int cx, cy, cw, ch;
 *
 *     etk_widget_geometry_get(ETK_WIDGET(canvas), &cx, &cy, &cw, &ch);
 *    //Move and resize the evas objects here
 * }
 * @endcode @n
 * 
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Canvas
 */
