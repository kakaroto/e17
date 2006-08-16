/** @file etk_canvas.c */
#include "etk_canvas.h"
#include <stdlib.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Canvas
 * @{
 */

static void _etk_canvas_constructor(Etk_Canvas *canvas);
static void _etk_canvas_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_canvas_clip_set(Etk_Widget *widget, Evas_Object *clip);
static void _etk_canvas_clip_unset(Etk_Widget *widget);
static void _etk_canvas_realize_cb(Etk_Object *object, void *data);
static void _etk_canvas_unrealize_cb(Etk_Object *object, void *data);
static void _etk_canvas_object_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

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
 * and will be resized to 32x32. The object will also be clipped against the canvas. @n
 * You can then use any function of evas to control the object
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

   if (!canvas || !object)
      return ETK_FALSE;
   if (evas_object_evas_get(object) != etk_widget_toplevel_evas_get(ETK_WIDGET(canvas)))
   {
      ETK_WARNING("Unable to add the object to the canvas: the canvas and the object do not belong to the same Evas");
      return ETK_FALSE;
   }

   etk_widget_geometry_get(ETK_WIDGET(canvas), &cx, &cy, NULL, NULL);
   if ((result = etk_widget_member_object_add(ETK_WIDGET(canvas), object)))
   {
      evas_object_move(object, cx, cy);
      evas_object_resize(object, 32, 32);
      evas_object_clip_set(object, canvas->clip);
      evas_object_show(canvas->clip);
      
      evas_object_event_callback_add(object, EVAS_CALLBACK_FREE, _etk_canvas_object_deleted_cb, canvas);
      canvas->objects = evas_list_append(canvas->objects, object);
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
   Evas_List *l;
   
   if (!canvas || !object)
      return;
   
   if ((l = evas_list_find_list(canvas->objects, object)))
   {
      etk_widget_member_object_del(ETK_WIDGET(canvas), object);
      evas_object_clip_unset(object);
      evas_object_hide(object);
      
      evas_object_event_callback_del(object, EVAS_CALLBACK_FREE, _etk_canvas_object_deleted_cb);
      canvas->objects = evas_list_remove_list(canvas->objects, l);
      if (!canvas->objects)
         evas_object_hide(canvas->clip);
   }
}

/**
 * @brief Moves an Evas Object to position ( @a x, @a y ), relatively to the canvas top-left corner.
 * @param canvas a canvas
 * @param object the object to move. The object does not necessarily belong to the canvas
 * @param x the x component of the position where to move the object, relative to the canvas top-left corner
 * @param y the y component of the position where to move the object, relative to the canvas top-left corner
 * @note You can still use evas_object_move(), but the position passed to evas_object_move() is relative to the evas,
 * not to the canvas.
 */
void etk_canvas_object_move(Etk_Canvas *canvas, Evas_Object *object, int x, int y)
{
   int cx, cy;
   
   if (!canvas || !object)
      return;
   
   etk_widget_geometry_get(ETK_WIDGET(canvas), &cx, &cy, NULL, NULL);
   evas_object_move(object, cx + x, cy + y);
}

/**
 * @brief Gets the geometry of an Evas Object. The returned position will be relative to the canvas top-left corner
 * @param canvas a canvas
 * @param object the object to get the position of. The object does not necessarily belong to the canvas
 * @param x the location where to store the x component of the position of the object,
 * relative to the canvas top-left corner
 * @param y the location where to store the y component of the position of the object,
 * relative to the canvas top-left corner
 * @param w the location where to store the width of the object
 * @param h the location where to store the height of the object
 * @note You can still use evas_object_geometry_get(), but the position returned by evas_object_geometry_get() is
 * relative to the evas, not to the canvas.
 */
void etk_canvas_object_geometry_get(Etk_Canvas *canvas, Evas_Object *object, int *x, int *y, int *w, int *h)
{
   int cx, cy;
   
   if (!canvas || !object)
      return;
   
   etk_widget_geometry_get(ETK_WIDGET(canvas), &cx, &cy, NULL, NULL);
   evas_object_geometry_get(object, x, y, w, h);
   if (x)   *x -= cx;
   if (y)   *y -= cy;
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
   canvas->objects = NULL;
   
   ETK_WIDGET(canvas)->size_allocate = _etk_canvas_size_allocate;
   ETK_WIDGET(canvas)->clip_set = _etk_canvas_clip_set;
   ETK_WIDGET(canvas)->clip_unset = _etk_canvas_clip_unset;
   
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

/* Clips the canvas against "clip" */
static void _etk_canvas_clip_set(Etk_Widget *widget, Evas_Object *clip)
{
   Etk_Canvas *canvas;
   
   if (!(canvas = ETK_CANVAS(widget)) || !canvas->clip || !clip)
      return;
   evas_object_clip_set(canvas->clip, clip);
}

/* Unclips the canvas */
static void _etk_canvas_clip_unset(Etk_Widget *widget)
{
   Etk_Canvas *canvas;
   
   if (!(canvas = ETK_CANVAS(widget)) || !canvas->clip)
      return;
   evas_object_clip_unset(canvas->clip);
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
   Evas_Object *obj;
   Evas_List *l;

   if (!(canvas = ETK_CANVAS(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(canvas))))
      return;
   
   canvas->clip = evas_object_rectangle_add(evas);
   etk_widget_member_object_add(ETK_WIDGET(canvas), canvas->clip);
   
   for (l = canvas->objects; l; l = l->next)
   {
      obj = l->data;
      evas_object_clip_set(obj, canvas->clip);
      evas_object_show(canvas->clip);
   }
}

/* Called when the canvas is unrealized */
static void _etk_canvas_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Canvas *canvas;
   
   if (!(canvas = ETK_CANVAS(object)))
      return;
   
   canvas->clip = NULL;
   canvas->objects = NULL;
}

/* Called when an object of the canvas is deleted */
static void _etk_canvas_object_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Canvas *canvas;
   
   if (!(canvas = ETK_CANVAS(data)) || !obj)
      return;
   
   canvas->objects = evas_list_remove(canvas->objects, obj);
   if (!canvas->objects)
      evas_object_hide(canvas->clip);
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
 * Once an object is added to the canvas, you can use any evas_object_* functions to control it. @n
 * You just have to keep in mind that calling evas_object_move() on an object belonging to the canvas
 * will move the object relatively to the top-left corner of the window, and not to the top corner of the canvas itself. @n
 * So if you want to move your object to the position (200, 300) inside the canvas, you first have to get the position
 * (cx, cy) of the canvas and then to move the object relatively to it: @n
 * @code
 * etk_widget_geometry_get(canvas, &cx, &cy, NULL, NULL);
 * evas_object_move(object, cx + 200, cy + 300);
 * @endcode @n
 * The function etk_canvas_object_move() does that for you. @n @n
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
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Canvas
 */
