/** @file etk_scrolled_view.c */
#include "etk_scrolled_view.h"
#include <stdlib.h>
#include <string.h>
#include "etk_viewport.h"
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Scrolled_View
* @{
 */

enum _Etk_Scrolled_View_Property_Id
{
   ETK_SCROLLED_VIEW_POLICY_PROPERTY
};

static void _etk_scrolled_view_constructor(Etk_Scrolled_View *scrolled_view);
static void _etk_scrolled_view_destructor(Etk_Scrolled_View *scrolled_view);
static void _etk_scrolled_view_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_scrolled_view_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_scrolled_view_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_scrolled_view_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_scrolled_view_hscrollbar_value_changed_cb(Etk_Object *object, double value, void *data);
static void _etk_scrolled_view_vscrollbar_value_changed_cb(Etk_Object *object, double value, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Scrolled_View
 * @return Returns the type on an Etk_Scrolled_View
 */
Etk_Type *etk_scrolled_view_type_get()
{
   static Etk_Type *scrolled_view_type = NULL;

   if (!scrolled_view_type)
   {
      scrolled_view_type = etk_type_new("Etk_Scrolled_View", ETK_BIN_TYPE, sizeof(Etk_Scrolled_View), ETK_CONSTRUCTOR(_etk_scrolled_view_constructor), ETK_DESTRUCTOR(_etk_scrolled_view_destructor), NULL);

      etk_type_property_add(scrolled_view_type, "policy", ETK_SCROLLED_VIEW_POLICY_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(0));
      
      scrolled_view_type->property_set = _etk_scrolled_view_property_set;
      scrolled_view_type->property_get = _etk_scrolled_view_property_get;
   }

   return scrolled_view_type;
}

/**
 * @brief Creates a new scrolled_view
 * @param label the label of the new scrolled_view
 * @return Returns the new scrolled_view widget
 */
Etk_Widget *etk_scrolled_view_new()
{
   return etk_widget_new(ETK_SCROLLED_VIEW_TYPE, NULL);
}

/**
 * @brief Gets the hscrollbar of the scrolled view. You can directly change its value, its bound values, ...
 * @param scrolled_view a scrolled view
 * @return Returns the hscrollbar of the scrolled view
 */
Etk_Widget *etk_scrolled_view_hscrollbar_get(Etk_Scrolled_View *scrolled_view)
{
   if (!scrolled_view)
      return NULL;
   return scrolled_view->hscrollbar;
}

/**
 * @brief Gets the vscrollbar of the scrolled view. You can directly change its value, its bound values, ...
 * @param scrolled_view a scrolled view
 * @return Returns the vscrollbar of the scrolled view
 */
Etk_Widget *etk_scrolled_view_vscrollbar_get(Etk_Scrolled_View *scrolled_view)
{
   if (!scrolled_view)
      return NULL;
   return scrolled_view->vscrollbar;
}

/**
 * @brief A convenience function that creates a viewport, adds the child to it and attach the viewport to the scrolled view. @n
 * It's useful for widgets that has no scrolling ability
 * @param scrolled_view a scrolled view
 * @param the child to add to the viewport
 */
void etk_scrolled_view_add_with_viewport(Etk_Scrolled_View *scrolled_view, Etk_Widget *child)
{
   Etk_Widget *viewport;

   if (!scrolled_view || !child)
      return;

   if (ETK_BIN(scrolled_view)->child && ETK_IS_VIEWPORT(ETK_BIN(scrolled_view)->child))
      viewport = ETK_BIN(scrolled_view)->child;
   else
   {
      viewport = etk_viewport_new();
      etk_container_add(ETK_CONTAINER(scrolled_view), viewport);
   }

   etk_container_add(ETK_CONTAINER(viewport), child);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_scrolled_view_constructor(Etk_Scrolled_View *scrolled_view)
{
   if (!scrolled_view)
      return;

   scrolled_view->hscrollbar = etk_hscrollbar_new(0.0, 0.0, 0.0, 6.0, 40.0, 0.0);
   etk_widget_parent_set(scrolled_view->hscrollbar, ETK_CONTAINER(scrolled_view));
   etk_widget_show(scrolled_view->hscrollbar);
   scrolled_view->vscrollbar = etk_vscrollbar_new(0.0, 0.0, 0.0, 6.0, 40.0, 0.0);
   etk_widget_parent_set(scrolled_view->vscrollbar, ETK_CONTAINER(scrolled_view));
   etk_widget_show(scrolled_view->vscrollbar);

   ETK_WIDGET(scrolled_view)->size_request = _etk_scrolled_view_size_request;
   ETK_WIDGET(scrolled_view)->size_allocate = _etk_scrolled_view_size_allocate;

   etk_signal_connect("value_changed", ETK_OBJECT(scrolled_view->hscrollbar), ETK_CALLBACK(_etk_scrolled_view_hscrollbar_value_changed_cb), scrolled_view);
   etk_signal_connect("value_changed", ETK_OBJECT(scrolled_view->vscrollbar), ETK_CALLBACK(_etk_scrolled_view_vscrollbar_value_changed_cb), scrolled_view);
}

/* Destroys the scrolled_view */
static void _etk_scrolled_view_destructor(Etk_Scrolled_View *scrolled_view)
{
   if (!scrolled_view)
      return;

   etk_widget_parent_set(scrolled_view->hscrollbar, NULL);
   etk_object_destroy(ETK_OBJECT(scrolled_view->hscrollbar));
   etk_widget_parent_set(scrolled_view->vscrollbar, NULL);
   etk_object_destroy(ETK_OBJECT(scrolled_view->vscrollbar));
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_scrolled_view_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Scrolled_View *scrolled_view;

   if (!(scrolled_view = ETK_SCROLLED_VIEW(object)) || !value)
      return;

   switch (property_id)
   {
      /* TODO */
      case ETK_SCROLLED_VIEW_POLICY_PROPERTY:
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_scrolled_view_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Scrolled_View *scrolled_view;

   if (!(scrolled_view = ETK_SCROLLED_VIEW(object)) || !value)
      return;

   switch (property_id)
   {
      /* TODO */
      case ETK_SCROLLED_VIEW_POLICY_PROPERTY:
         break;
      default:
         break;
   }
}

/* Calculates the ideal size for the scrolled view */
static void _etk_scrolled_view_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Scrolled_View *scrolled_view;
   Etk_Size hscrollbar_requisition, vscrollbar_requisition, child_requisition;

   if (!(scrolled_view = ETK_SCROLLED_VIEW(widget)) || !size_requisition)
      return;

   if (ETK_BIN(scrolled_view)->child)
   {
      etk_widget_size_request_full(scrolled_view->hscrollbar, &hscrollbar_requisition, FALSE);
      etk_widget_size_request_full(scrolled_view->vscrollbar, &vscrollbar_requisition, FALSE);
      etk_widget_size_request(ETK_BIN(scrolled_view)->child, &child_requisition);

      size_requisition->w = ETK_MAX(child_requisition.w, hscrollbar_requisition.w + vscrollbar_requisition.w);
      size_requisition->h = ETK_MAX(child_requisition.h, hscrollbar_requisition.h + vscrollbar_requisition.h);
   }
   else
   {
      size_requisition->w = 0;
      size_requisition->h = 0;
   }
}

/* Resizes the scrolled view to the size allocation */
static void _etk_scrolled_view_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Scrolled_View *scrolled_view;
   Etk_Size hscrollbar_requisition, vscrollbar_requisition, scroll_size;
   Etk_Geometry child_geometry;
   Etk_Widget *child;
   Etk_Bool show_vscrollbar = FALSE, show_hscrollbar = FALSE;
   int visible_width, visible_height;

   if (!(scrolled_view = ETK_SCROLLED_VIEW(widget)))
      return;

   if (!(child = ETK_BIN(scrolled_view)->child))
   {
      etk_widget_hide(scrolled_view->hscrollbar);
      etk_widget_hide(scrolled_view->vscrollbar);
      return;
   }

   etk_widget_size_request_full(scrolled_view->hscrollbar, &hscrollbar_requisition, FALSE);
   etk_widget_size_request_full(scrolled_view->vscrollbar, &vscrollbar_requisition, FALSE);
   child->scroll_size_get(child, &scroll_size);

   if (scroll_size.w > geometry.w)
      show_hscrollbar = TRUE;
   if (scroll_size.h > (geometry.h - (show_hscrollbar ? hscrollbar_requisition.h : 0)))
   {
      show_vscrollbar = TRUE;
      if (scroll_size.w > (geometry.w - vscrollbar_requisition.w))
         show_hscrollbar = TRUE;
   }

   visible_width = geometry.w;
   visible_height = geometry.h;

   if (show_hscrollbar)
   {
      visible_height -= hscrollbar_requisition.h;
      etk_widget_show(scrolled_view->hscrollbar);
      
      child_geometry.x = geometry.x;
      child_geometry.y = geometry.y + geometry.h - hscrollbar_requisition.h;
      child_geometry.w = geometry.w - (show_vscrollbar ? vscrollbar_requisition.w : 0);
      child_geometry.h = hscrollbar_requisition.h;
      etk_widget_size_allocate(scrolled_view->hscrollbar, child_geometry);
   }
   else
      etk_widget_hide(scrolled_view->hscrollbar);

   if (show_vscrollbar)
   {
      visible_width -= vscrollbar_requisition.w;
      etk_widget_show(scrolled_view->vscrollbar);

      child_geometry.x = geometry.x + geometry.w - vscrollbar_requisition.w;
      child_geometry.y = geometry.y;
      child_geometry.w = vscrollbar_requisition.w;
      child_geometry.h = geometry.h - (show_hscrollbar ? hscrollbar_requisition.h : 0);
      etk_widget_size_allocate(scrolled_view->vscrollbar, child_geometry);
   }
   else
      etk_widget_hide(scrolled_view->vscrollbar);

   etk_range_range_set(ETK_RANGE(scrolled_view->hscrollbar), 0, scroll_size.w);
   etk_range_page_size_set(ETK_RANGE(scrolled_view->hscrollbar), visible_width);
   etk_range_range_set(ETK_RANGE(scrolled_view->vscrollbar), 0, scroll_size.h);
   etk_range_page_size_set(ETK_RANGE(scrolled_view->vscrollbar), visible_height);

   child_geometry.x = geometry.x;
   child_geometry.y = geometry.y;
   child_geometry.w = visible_width;
   child_geometry.h = visible_height;
   etk_widget_size_allocate(child, child_geometry);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the value of the hscrollbar has changed */
static void _etk_scrolled_view_hscrollbar_value_changed_cb(Etk_Object *object, double value, void *data)
{
   Etk_Scrolled_View *scrolled_view;
   Etk_Widget *child;

   if (!(scrolled_view = ETK_SCROLLED_VIEW(data)) || !(child = ETK_BIN(scrolled_view)->child) || !child->scroll)
      return;

   child->scroll(child, value, ETK_RANGE(scrolled_view->vscrollbar)->value);
}

/* Called when the value of the vscrollbar has changed */
static void _etk_scrolled_view_vscrollbar_value_changed_cb(Etk_Object *object, double value, void *data)
{
   Etk_Scrolled_View *scrolled_view;
   Etk_Widget *child;

   if (!(scrolled_view = ETK_SCROLLED_VIEW(data)) || !(child = ETK_BIN(scrolled_view)->child) || !child->scroll)
      return;

   child->scroll(child, ETK_RANGE(scrolled_view->hscrollbar)->value, value);
}

/** @} */
