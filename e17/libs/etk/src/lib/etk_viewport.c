/** @file etk_viewport.c */
#include "etk_viewport.h"
#include <stdlib.h>
#include "etk_utils.h"
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
static void _etk_viewport_clip_set(Etk_Widget *widget, Evas_Object *clip);
static void _etk_viewport_clip_unset(Etk_Widget *widget);
static void _etk_viewport_realize_cb(Etk_Object *object, void *data);
static void _etk_viewport_child_added_cb(Etk_Object *object, void *child, void *data);
static void _etk_viewport_child_removed_cb(Etk_Object *object, void *child, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Viewport
 * @return Returns the type on an Etk_Viewport
 */
Etk_Type *etk_viewport_type_get()
{
   static Etk_Type *viewport_type = NULL;

   if (!viewport_type)
      viewport_type = etk_type_new("Etk_Viewport", ETK_BIN_TYPE, sizeof(Etk_Viewport), ETK_CONSTRUCTOR(_etk_viewport_constructor), NULL);

   return viewport_type;
}

/**
 * @brief Creates a new viewport
 * @return Returns the new viewport widget
 */
Etk_Widget *etk_viewport_new()
{
   return etk_widget_new(ETK_VIEWPORT_TYPE, NULL);
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
   viewport->xscroll = 0;
   viewport->yscroll = 0;

   ETK_WIDGET(viewport)->size_request = _etk_viewport_size_request;
   ETK_WIDGET(viewport)->size_allocate = _etk_viewport_size_allocate;
   ETK_WIDGET(viewport)->scroll_size_get = _etk_viewport_scroll_size_get;
   ETK_WIDGET(viewport)->scroll = _etk_viewport_scroll;
   ETK_WIDGET(viewport)->clip_set = _etk_viewport_clip_set;
   ETK_WIDGET(viewport)->clip_unset = _etk_viewport_clip_unset;

   etk_signal_connect("realize", ETK_OBJECT(viewport), ETK_CALLBACK(_etk_viewport_realize_cb), NULL);
   etk_signal_connect_swapped("unrealize", ETK_OBJECT(viewport), ETK_CALLBACK(etk_callback_set_null), &viewport->clip);
   etk_signal_connect("child_added", ETK_OBJECT(viewport), ETK_CALLBACK(_etk_viewport_child_added_cb), NULL);
   etk_signal_connect("child_removed", ETK_OBJECT(viewport), ETK_CALLBACK(_etk_viewport_child_removed_cb), NULL);
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
   
   if ((child = etk_bin_child_get(ETK_BIN(viewport))))
   {
      Etk_Size child_requisition;

      etk_widget_size_request(child, &child_requisition);
      
      if (child_requisition.w <= geometry.w)
         geometry.x += (geometry.w - child_requisition.w) / 2;
      else
         geometry.x -= viewport->xscroll;
      
      if (child_requisition.h <= geometry.h)
         geometry.y += (geometry.h - child_requisition.h) / 2;
      else
         geometry.y -= viewport->yscroll;
      
      geometry.w = child_requisition.w;
      geometry.h = child_requisition.h;
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

/* Clips the viewport against "clip" */
static void _etk_viewport_clip_set(Etk_Widget *widget, Evas_Object *clip)
{
   Etk_Viewport *viewport;
   
   if (!(viewport = ETK_VIEWPORT(widget)) || !viewport->clip || !clip)
      return;
   evas_object_clip_set(viewport->clip, clip);
}

/* Unclips the viewport */
static void _etk_viewport_clip_unset(Etk_Widget *widget)
{
   Etk_Viewport *viewport;
   
   if (!(viewport = ETK_VIEWPORT(widget)) || !viewport->clip)
      return;
   evas_object_clip_unset(viewport->clip);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the viewport is realized */
static void _etk_viewport_realize_cb(Etk_Object *object, void *data)
{
   Etk_Viewport *viewport;
   Etk_Widget *child;
   Evas *evas;

   if (!(viewport = ETK_VIEWPORT(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(viewport))))
      return;

   viewport->clip = evas_object_rectangle_add(evas);
   //TODO: printf("Viewport Clip: %p\n", viewport->clip);
   etk_widget_member_object_add(ETK_WIDGET(viewport), viewport->clip);
   if ((child = etk_bin_child_get(ETK_BIN(viewport))))
   {
      etk_widget_clip_set(child, viewport->clip);
      evas_object_show(viewport->clip);
   }
}

/* Called when a child is added to the viewport */
static void _etk_viewport_child_added_cb(Etk_Object *object, void *child, void *data)
{
   Etk_Viewport *viewport;
   
   if (!(viewport = ETK_VIEWPORT(object)) || !child || !viewport->clip)
      return;
   
   etk_widget_clip_set(ETK_WIDGET(child), viewport->clip);
   evas_object_show(viewport->clip);
}

/* Called when a child is removed from the viewport */
static void _etk_viewport_child_removed_cb(Etk_Object *object, void *child, void *data)
{
   Etk_Viewport *viewport;
   
   if (!(viewport = ETK_VIEWPORT(object)) || !child || !viewport->clip)
      return;
   
   etk_widget_clip_unset(ETK_WIDGET(child));
   evas_object_hide(viewport->clip);
}

/** @} */
