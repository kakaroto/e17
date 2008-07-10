/** @file etk_viewport.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_viewport.h"

#include <stdlib.h>

#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Viewport
 * @{
 */

static void _etk_viewport_constructor(Etk_Viewport *viewport);
static void _etk_viewport_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_viewport_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_viewport_scroll_size_get(Etk_Widget *widget, Etk_Size scrollview_size, Etk_Size scrollbar_size, Etk_Size *scroll_size);
static void _etk_viewport_scroll(Etk_Widget *widget, int x, int y);
static Etk_Bool _etk_viewport_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_viewport_child_added_cb(Etk_Object *object, void *child, void *data);
static Etk_Bool _etk_viewport_child_removed_cb(Etk_Object *object, void *child, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Viewport
 * @return Returns the type on an Etk_Viewport
 */
Etk_Type *etk_viewport_type_get(void)
{
   static Etk_Type *viewport_type = NULL;

   if (!viewport_type)
      viewport_type = etk_type_new("Etk_Viewport", ETK_BIN_TYPE,
         sizeof(Etk_Viewport),
         ETK_CONSTRUCTOR(_etk_viewport_constructor), NULL, NULL);

   return viewport_type;
}

/**
 * @brief Creates a new viewport
 * @return Returns the new viewport widget
 */
Etk_Widget *etk_viewport_new(void)
{
   return etk_widget_new(ETK_VIEWPORT_TYPE, NULL);
}

/**
 * @brief Sets the hold_events flag for the viewport
 * This sets the hold_events flag. When set all events on children of the
 * viewport have the ETK_MODIFIER_ON_HOLD in the modifier flags if it is
 * used by a scrolled view
 */
void etk_viewport_hold_events_set(Etk_Viewport *viewport, Etk_Bool hold_events)
{
   if (!viewport) return;
   viewport->hold_events = hold_events;
}

/**
 * @brief Gets the hold_events flag for the viewport
 * @return Returns the hold_events flag
 */
Etk_Bool etk_viewport_hold_events_get(Etk_Viewport *viewport)
{
   if (!viewport) return ETK_FALSE;
   return viewport->hold_events;
}


/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_viewport_constructor(Etk_Viewport *viewport)
{
   if (!viewport)
      return;

   viewport->clip = NULL;
   viewport->event = NULL;
   viewport->xscroll = 0;
   viewport->yscroll = 0;

   ETK_WIDGET(viewport)->size_request = _etk_viewport_size_request;
   ETK_WIDGET(viewport)->size_allocate = _etk_viewport_size_allocate;
   ETK_WIDGET(viewport)->scroll_size_get = _etk_viewport_scroll_size_get;
   ETK_WIDGET(viewport)->scroll = _etk_viewport_scroll;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(viewport), ETK_CALLBACK(_etk_viewport_realized_cb), NULL);
   etk_signal_connect_swapped_by_code(ETK_WIDGET_UNREALIZED_SIGNAL, ETK_OBJECT(viewport), ETK_CALLBACK(etk_callback_set_null), &viewport->clip);
   etk_signal_connect_swapped_by_code(ETK_WIDGET_UNREALIZED_SIGNAL, ETK_OBJECT(viewport), ETK_CALLBACK(etk_callback_set_null), &viewport->event);
   etk_signal_connect_by_code(ETK_CONTAINER_CHILD_ADDED_SIGNAL, ETK_OBJECT(viewport), ETK_CALLBACK(_etk_viewport_child_added_cb), NULL);
   etk_signal_connect_by_code(ETK_CONTAINER_CHILD_REMOVED_SIGNAL, ETK_OBJECT(viewport), ETK_CALLBACK(_etk_viewport_child_removed_cb), NULL);
}

/* Calculates the ideal size of the viewport */
static void _etk_viewport_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   if (!widget || !size_requisition)
      return;
   size_requisition->w = 20;
   size_requisition->h = 20;
}

/* Resizes the viewport to the allocated size */
static void _etk_viewport_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Viewport *viewport;
   Etk_Widget *child;
   Etk_Container *container;

   if (!(viewport = ETK_VIEWPORT(widget)))
      return;

   container = ETK_CONTAINER(widget);

   evas_object_move(viewport->clip, geometry.x, geometry.y);
   evas_object_resize(viewport->clip, geometry.w, geometry.h);
   evas_object_move(viewport->event, geometry.x, geometry.y);
   evas_object_resize(viewport->event, geometry.w, geometry.h);

   if ((child = etk_bin_child_get(ETK_BIN(viewport))))
   {
      Etk_Size child_requisition;

      etk_widget_size_request(child, &child_requisition);
      if (child_requisition.w > geometry.w)
      {
	 geometry.w = child_requisition.w;
         geometry.x -= viewport->xscroll;
      }
      if (child_requisition.h > geometry.h)
      {
	 geometry.h = child_requisition.h;
         geometry.y -= viewport->yscroll;
      }
      etk_widget_size_allocate(child, geometry);
   }
}

/* Scroll the viewport */
static void _etk_viewport_scroll(Etk_Widget *widget, int x, int y)
{
   Etk_Viewport *viewport;

   if (!(viewport = ETK_VIEWPORT(widget)))
      return;

   viewport->xscroll = x;
   viewport->yscroll = y;
   etk_widget_redraw_queue(widget);
}

/* Gets the scrolling size needed by the viewport  */
static void _etk_viewport_scroll_size_get(Etk_Widget *widget, Etk_Size scrollview_size, Etk_Size scrollbar_size, Etk_Size *scroll_size)
{
   Etk_Viewport *viewport;
   Etk_Widget *child;

   if (!(viewport = ETK_VIEWPORT(widget)) || !scroll_size)
      return;

   if ((child = ETK_BIN(viewport)->child))
      etk_widget_size_request(child, scroll_size);
   else
   {
      scroll_size->w = 0;
      scroll_size->h = 0;
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

static void _etk_viewport_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Viewport *viewport;
   Evas_Event_Mouse_Down *event;
   
   if (!(viewport = ETK_VIEWPORT(data)) || !(event = event_info))
      return;
   if (viewport->hold_events)
     event->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
}

static void _etk_viewport_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Viewport *viewport;
   Evas_Event_Mouse_Up *event;
   
   if (!(viewport = ETK_VIEWPORT(data)) || !(event = event_info))
      return;
   if (viewport->hold_events)
     event->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
}

static void _etk_viewport_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Viewport *viewport;
   Evas_Event_Mouse_Move *event;
   
   if (!(viewport = ETK_VIEWPORT(data)) || !(event = event_info))
      return;
   if (viewport->hold_events)
     event->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
}

/* Called when the viewport is realized */
static Etk_Bool _etk_viewport_realized_cb(Etk_Object *object, void *data)
{
   Etk_Viewport *viewport;
   Etk_Widget *child;
   Evas *evas;

   if (!(viewport = ETK_VIEWPORT(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(viewport))))
      return ETK_TRUE;

   viewport->clip = evas_object_rectangle_add(evas);
   etk_widget_member_object_add(ETK_WIDGET(viewport), viewport->clip);
   if ((child = etk_bin_child_get(ETK_BIN(viewport))))
   {
      etk_widget_clip_set(child, viewport->clip);
      evas_object_show(viewport->clip);
   }
   viewport->event = evas_object_rectangle_add(evas);
   evas_object_event_callback_add(viewport->event, EVAS_CALLBACK_MOUSE_DOWN, _etk_viewport_mouse_down_cb, viewport);
   evas_object_event_callback_add(viewport->event, EVAS_CALLBACK_MOUSE_UP, _etk_viewport_mouse_up_cb, viewport);
   evas_object_event_callback_add(viewport->event, EVAS_CALLBACK_MOUSE_MOVE, _etk_viewport_mouse_move_cb, viewport);
   etk_widget_member_object_add(ETK_WIDGET(viewport), viewport->event);
   evas_object_color_set(viewport->event, 0, 0, 0, 0);
   evas_object_repeat_events_set(viewport->event, 1);
   evas_object_raise(viewport->event);
   evas_object_show(viewport->event);

   return ETK_TRUE;
}

/* Called when a child is added to the viewport */
static Etk_Bool _etk_viewport_child_added_cb(Etk_Object *object, void *child, void *data)
{
   Etk_Viewport *viewport;

   if (!(viewport = ETK_VIEWPORT(object)) || !child || !viewport->clip)
      return ETK_TRUE;

   etk_widget_clip_set(ETK_WIDGET(child), viewport->clip);
   evas_object_show(viewport->clip);
   evas_object_raise(viewport->event);
   return ETK_TRUE;
}

/* Called when a child is removed from the viewport */
static Etk_Bool _etk_viewport_child_removed_cb(Etk_Object *object, void *child, void *data)
{
   Etk_Viewport *viewport;

   if (!(viewport = ETK_VIEWPORT(object)) || !child || !viewport->clip)
      return ETK_TRUE;

   etk_widget_clip_unset(ETK_WIDGET(child));
   evas_object_hide(viewport->clip);
   return ETK_TRUE;
}

/** @} */
