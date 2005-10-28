/** @file etk_canvas.c */
#include "etk_canvas.h"
#include <stdlib.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Canvas
* @{
 */

/* TODO: finish and fix the memleaks */

typedef struct _Etk_Canvas_Object
{
   Etk_Canvas *canvas;
   Evas_Object *object;
   Etk_Bool visible;
} Etk_Canvas_Object;

static void _etk_canvas_constructor(Etk_Canvas *canvas);
static void _etk_canvas_destructor(Etk_Canvas *canvas);
static void _etk_canvas_move_resize(Etk_Widget *widget, int x, int y, int w, int h);
static void _etk_canvas_realized_cb(Etk_Object *object, void *data);
static void _etk_canvas_unrealized_cb(Etk_Object *object, void *data);
static void _etk_canvas_show_cb(Etk_Object *object, void *data);
static void _etk_canvas_hide_cb(Etk_Object *object, void *data);
static void _etk_canvas_object_del(void *data);

static void _etk_canvas_intercept_show_cb(void *data, Evas_Object *object);
static void _etk_canvas_intercept_hide_cb(void *data, Evas_Object *object);
static void _etk_canvas_intercept_move_cb(void *data, Evas_Object *object, Evas_Coord x, Evas_Coord y);
static void _etk_canvas_intercept_raise_cb(void *data, Evas_Object *object);
static void _etk_canvas_intercept_lower_cb(void *data, Evas_Object *object);

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
      canvas_type = etk_type_new("Etk_Canvas", ETK_WIDGET_TYPE, sizeof(Etk_Canvas), ETK_CONSTRUCTOR(_etk_canvas_constructor), ETK_DESTRUCTOR(_etk_canvas_destructor), NULL);
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
 * @param The object will be automatically deleted when the canvas will be destroyed
 */
void etk_canvas_object_add(Etk_Canvas *canvas, Evas_Object *object)
{
   Etk_Canvas_Object *new_object;
   Evas_Coord x, y;

   if (!canvas || !object || (evas_object_evas_get(object) != etk_widget_toplevel_evas_get(ETK_WIDGET(canvas))))
      return;

   new_object = malloc(sizeof(Etk_Canvas_Object));
   new_object->canvas = canvas;
   new_object->object = object;
   new_object->visible = FALSE;

   ecore_list_append(canvas->objects, object);
   if (ecore_list_nodes(canvas->objects) == 1)
      evas_object_color_set(canvas->clip, 255, 255, 255, 255);

   evas_object_clip_set(object, canvas->clip);

   evas_object_intercept_show_callback_add(object, _etk_canvas_intercept_show_cb, new_object);
   evas_object_intercept_hide_callback_add(object, _etk_canvas_intercept_hide_cb, new_object);
   evas_object_intercept_move_callback_add(object, _etk_canvas_intercept_move_cb, canvas);
   evas_object_intercept_raise_callback_add(object, _etk_canvas_intercept_raise_cb, canvas);
   evas_object_intercept_lower_callback_add(object, _etk_canvas_intercept_lower_cb, canvas);

   if (evas_object_visible_get(object))
      evas_object_show(object);
   else
      evas_object_hide(object);

   evas_object_geometry_get(object, &x, &y, NULL, NULL);
   evas_object_move(object, x, y);
   /* TODO: restack? */
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
   canvas->objects = ecore_list_new();
   ecore_list_set_free_cb(canvas->objects, _etk_canvas_object_del);
   ETK_WIDGET(canvas)->move_resize = _etk_canvas_move_resize;

   etk_signal_connect_after("realized", ETK_OBJECT(canvas), ETK_CALLBACK(_etk_canvas_realized_cb), NULL);
   etk_signal_connect("unrealized", ETK_OBJECT(canvas), ETK_CALLBACK(_etk_canvas_unrealized_cb), NULL);
   etk_signal_connect("show", ETK_OBJECT(canvas), ETK_CALLBACK(_etk_canvas_show_cb), NULL);
   etk_signal_connect("hide", ETK_OBJECT(canvas), ETK_CALLBACK(_etk_canvas_hide_cb), NULL);
}

/* Destroy the canvas */
static void _etk_canvas_destructor(Etk_Canvas *canvas)
{
   if (!canvas)
      return;

   ecore_list_destroy(canvas->objects);
   canvas->objects = NULL;
}

/* TODO */
static void _etk_canvas_move_resize(Etk_Widget *widget, int x, int y, int w, int h)
{
   Etk_Canvas *canvas;

   if (!(canvas = ETK_CANVAS(widget)))
      return

   evas_object_move(canvas->clip, x, y);
   evas_object_resize(canvas->clip, w, h);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the canvas is realized */
static void _etk_canvas_realized_cb(Etk_Object *object, void *data)
{
   Evas *evas;
   Etk_Canvas *canvas;

   if (!(canvas = ETK_CANVAS(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(canvas))))
      return;

   canvas->clip = evas_object_rectangle_add(evas);
   evas_object_color_set(canvas->clip, 255, 255, 255, 0);
   etk_widget_member_object_add(ETK_WIDGET(canvas), canvas->clip);
}

/* Called when the canvas is unrealized */
static void _etk_canvas_unrealized_cb(Etk_Object *object, void *data)
{
   Etk_Canvas *canvas;

   if (!(canvas = ETK_CANVAS(object)) || !canvas->objects)
      return;

   ecore_list_clear(canvas->objects);
}

/* TODO: not really show */
/* Called when the canvas is shown */
static void _etk_canvas_show_cb(Etk_Object *object, void *data)
{
   Etk_Canvas *canvas;
   Etk_Canvas_Object *canvas_object;

   if (!(canvas = ETK_CANVAS(object)))
      return;

   ecore_list_goto_first(canvas->objects);
   while ((canvas_object = ecore_list_next(canvas->objects)))
   {
      if (canvas_object->visible)
         evas_object_show(canvas_object->object);
   }
}

/* TODO: not really hide */
/* Called when the canvas is hidden */
static void _etk_canvas_hide_cb(Etk_Object *object, void *data)
{
   Etk_Canvas *canvas;
   Etk_Canvas_Object *canvas_object;

   if (!(canvas = ETK_CANVAS(object)))
      return;

   ecore_list_goto_first(canvas->objects);
   while ((canvas_object = ecore_list_next(canvas->objects)))
      evas_object_hide(canvas_object->object);
}

/* Called when the object is removed from the list of objects of the canvas */
static void _etk_canvas_object_del(void *data)
{
   Etk_Canvas_Object *object;
   
   if (!(object = data))
      return;

   evas_object_del(object->object);
   free(object);
}

/* Called when an object requests to be shown */
static void _etk_canvas_intercept_show_cb(void *data, Evas_Object *object)
{
   Etk_Canvas *canvas;
   Etk_Canvas_Object *canvas_object;

   if (!object)
      return;

   if (!(canvas_object = data) || !(canvas = canvas_object->canvas))
   {
      evas_object_show(object);
      return;
   }

   evas_object_show(object);
   canvas_object->visible = TRUE;
}

/* Called when an object requests to be hidden */
static void _etk_canvas_intercept_hide_cb(void *data, Evas_Object *object)
{
   Etk_Canvas_Object *canvas_object;

   if (!object)
      return;

   evas_object_hide(object);

   if (!(canvas_object = data))
      canvas_object->visible = FALSE;
}

/* TODO */
static void _etk_canvas_intercept_move_cb(void *data, Evas_Object *object, Evas_Coord x, Evas_Coord y)
{
   Etk_Canvas *canvas;
   Etk_Widget *canvas_widget;

   if (!(canvas = ETK_CANVAS(data)))
   {
      evas_object_move(object, x, y);
      return;
   }

   canvas_widget = ETK_WIDGET(canvas);
   evas_object_move(object, x + canvas_widget->geometry.x + canvas_widget->left_padding, y + canvas_widget->geometry.y + canvas_widget->top_padding);
}

/* TODO */
static void _etk_canvas_intercept_raise_cb(void *data, Evas_Object *object)
{
   evas_object_raise(object);
}

/* TODO */
static void _etk_canvas_intercept_lower_cb(void *data, Evas_Object *object)
{
   evas_object_lower(object);
}

/** @} */
