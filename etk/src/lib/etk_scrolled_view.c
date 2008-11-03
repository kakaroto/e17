/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_scrolled_view.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_scrolled_view.h"

#include <stdlib.h>
#include <string.h>

#include "etk_event.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"
#include "etk_viewport.h"

/**
 * @addtogroup Etk_Scrolled_View
 * @{
 */

#define ETK_SCROLLED_VIEW_DRAG_DAMPING_MAGIC 100
#define ETK_SCROLLED_VIEW_DRAG_SAMPLE_INTERVAL_MAGIC 0.5f

enum Etk_Scrolled_View_Property_Id
{
   ETK_SCROLLED_VIEW_HPOLICY_PROPERTY,
   ETK_SCROLLED_VIEW_VPOLICY_PROPERTY,
   ETK_SCROLLED_VIEW_DRAGABLE_PROPERTY,
   ETK_SCROLLED_VIEW_DRAG_BOUNCY_PROPERTY,
   ETK_SCROLLED_VIEW_DRAG_DAMPING_PROPERTY,
   ETK_SCROLLED_VIEW_DRAG_SAMPLE_INTERVAL_PROPERTY
};

static void _etk_scrolled_view_constructor(Etk_Scrolled_View *scrolled_view);
static void _etk_scrolled_view_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_scrolled_view_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_scrolled_view_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_scrolled_view_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static Etk_Bool _etk_scrolled_view_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static Etk_Bool _etk_scrolled_view_mouse_wheel(Etk_Object *object, Etk_Event_Mouse_Wheel *event, void *data);
static Etk_Bool _etk_scrolled_view_hscrollbar_value_changed_cb(Etk_Object *object, double value, void *data);
static Etk_Bool _etk_scrolled_view_vscrollbar_value_changed_cb(Etk_Object *object, double value, void *data);
static Etk_Bool _etk_scrolled_view_child_added_cb(Etk_Object *object, void *child, void *data);
static Etk_Bool _etk_scrolled_view_child_removed_cb(Etk_Object *object, void *child, void *data);
static Etk_Bool _etk_scrolled_view_child_scroll_size_changed_cb(Etk_Object *object, void *data);
static int _etk_scrolled_view_motive_bounce(void *data); 


static Etk_Bool _etk_scrolled_view_mouse_down(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data); 
static Etk_Bool _etk_scrolled_view_mouse_up(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data);
static Etk_Bool _etk_scrolled_view_mouse_click(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data);
static Etk_Bool _etk_scrolled_view_mouse_move(Etk_Object *object, Etk_Event_Mouse_Move *event, void *data); 

static Etk_Bool _etk_scrolled_view_bar_mouse_down(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data); 
/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Scrolled_View
 * @return Returns the type of an Etk_Scrolled_View
 */
Etk_Type *etk_scrolled_view_type_get(void)
{
   static Etk_Type *scrolled_view_type = NULL;

   if (!scrolled_view_type)
   {
      scrolled_view_type = etk_type_new("Etk_Scrolled_View", ETK_BIN_TYPE,
         sizeof(Etk_Scrolled_View),
         ETK_CONSTRUCTOR(_etk_scrolled_view_constructor), NULL, NULL);

      etk_type_property_add(scrolled_view_type, "hpolicy", ETK_SCROLLED_VIEW_HPOLICY_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_POLICY_AUTO));

      etk_type_property_add(scrolled_view_type, "vpolicy", ETK_SCROLLED_VIEW_VPOLICY_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_POLICY_AUTO));

      etk_type_property_add(scrolled_view_type, "dragable", ETK_SCROLLED_VIEW_DRAGABLE_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));

      etk_type_property_add(scrolled_view_type, "drag-bouncy", ETK_SCROLLED_VIEW_DRAG_BOUNCY_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));

      etk_type_property_add(scrolled_view_type, "drag-damping", ETK_SCROLLED_VIEW_DRAG_DAMPING_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_SCROLLED_VIEW_DRAG_DAMPING_MAGIC));

      etk_type_property_add(scrolled_view_type, "drag-sample-interval", ETK_SCROLLED_VIEW_DRAG_SAMPLE_INTERVAL_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_SCROLLED_VIEW_DRAG_SAMPLE_INTERVAL_MAGIC));

      scrolled_view_type->property_set = _etk_scrolled_view_property_set;
      scrolled_view_type->property_get = _etk_scrolled_view_property_get;
   }

   return scrolled_view_type;
}

/**
 * @brief Creates a new scrolled view
 * @return Returns the new scrolled view widget
 */
Etk_Widget *etk_scrolled_view_new(void)
{
   return etk_widget_new(ETK_SCROLLED_VIEW_TYPE, "theme-group", "scrolled_view", NULL);
}

/**
 * @brief Gets the hoizontal scrollbar of the scrolled view. You can then change its value, bound values, ...
 * @param scrolled_view a scrolled view
 * @return Returns the hoizontal scrollbar of the scrolled view
 */
Etk_Range *etk_scrolled_view_hscrollbar_get(Etk_Scrolled_View *scrolled_view)
{
   if (!scrolled_view)
      return NULL;
   return ETK_RANGE(scrolled_view->hscrollbar);
}

/**
 * @brief Gets the vertical scrollbar of the scrolled view. You can then change its value, bound values, ...
 * @param scrolled_view a scrolled view
 * @return Returns the vertical scrollbar of the scrolled view
 */
Etk_Range *etk_scrolled_view_vscrollbar_get(Etk_Scrolled_View *scrolled_view)
{
   if (!scrolled_view)
      return NULL;
   return ETK_RANGE(scrolled_view->vscrollbar);
}

/**
 * @brief A convenient function that creates a viewport, attachs the child to it and adds the viewport to the
 * scrolled view. It's useful for widgets that have no scrolling ability
 * @param scrolled_view a scrolled view
 * @param child the child to add to the viewport
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
      etk_widget_internal_set(viewport, ETK_TRUE);
      etk_widget_show(viewport);
   }

   etk_container_add(ETK_CONTAINER(viewport), child);
   scrolled_view->viewport = viewport;
}

/**
 * @brief Sets the visibility policy of the hscrollbar and the vscrollbar of the scrolled view
 * @param scrolled_view a scrolled view
 * @param hpolicy the visibility policy to use for the hscrollbar
 * @param vpolicy the visibility policy to use for the vscrollbar
 * @see Etk_Scrolled_View_Policy
 */
void etk_scrolled_view_policy_set(Etk_Scrolled_View *scrolled_view, Etk_Scrolled_View_Policy hpolicy, Etk_Scrolled_View_Policy vpolicy)
{
   if (!scrolled_view)
      return;

   if (scrolled_view->hpolicy != hpolicy)
   {
      scrolled_view->hpolicy = hpolicy;
      etk_widget_redraw_queue(ETK_WIDGET(scrolled_view));
      etk_object_notify(ETK_OBJECT(scrolled_view), "hpolicy");
   }
   if (scrolled_view->vpolicy != vpolicy)
   {
      scrolled_view->vpolicy = vpolicy;
      etk_widget_redraw_queue(ETK_WIDGET(scrolled_view));
      etk_object_notify(ETK_OBJECT(scrolled_view), "vpolicy");
   }
}

/**
 * @brief Gets the visibility policy of the hscrollbar and the vscrollbar of the scrolled view
 * @param scrolled_view a scrolled view
 * @param hpolicy the location where to store the visibility policy of the hscrollbar
 * @param vpolicy the location where to store the visibility policy of the vscrollbar
 */
void etk_scrolled_view_policy_get(Etk_Scrolled_View *scrolled_view, Etk_Scrolled_View_Policy *hpolicy, Etk_Scrolled_View_Policy *vpolicy)
{
   if (!scrolled_view)
      return;

   if (hpolicy)
      *hpolicy = scrolled_view ? scrolled_view->hpolicy : ETK_POLICY_AUTO;
   if (vpolicy)
      *vpolicy = scrolled_view ? scrolled_view->vpolicy : ETK_POLICY_AUTO;
}

/**
 * @brief Set the scrolled view dragable or not
 * @param scrolled_view a scrolled view
 * @param dragable The scrolled view is dragable or not?
 */ 
void etk_scrolled_view_dragable_set(Etk_Scrolled_View *scrolled_view, Etk_Bool dragable) 
{
   if (!scrolled_view || scrolled_view->drag.dragable == dragable)
      return;

   scrolled_view->drag.dragable = dragable;
   etk_object_notify(ETK_OBJECT(scrolled_view), "dragable");
}

/**
 * @brief Get the scrolled view dragable flag
 * @param scrolled_view a scrolled view
 * @return Returns ETK_TURE if the scrolled view is dragable
 */
Etk_Bool etk_scrolled_view_dragable_get(Etk_Scrolled_View *scrolled_view) 
{
   if (!scrolled_view)
      return ETK_FALSE;

   return scrolled_view->drag.dragable;
}

/**
 * @brief Set the scrolled view boucy or not.
 * @param scrolled_view  a scrolled view
 * @param bouncy The scrolled view is bouncy or not (Default TRUE)
 */ 
void etk_scrolled_view_drag_bouncy_set(Etk_Scrolled_View *scrolled_view, Etk_Bool bouncy) 
{
   if (!scrolled_view || scrolled_view->drag.bouncy == bouncy)
      return;

   scrolled_view->drag.bouncy = bouncy;
   etk_object_notify(ETK_OBJECT(scrolled_view), "drag-bouncy");
}

/**
 * @brief  Get the scrolled view bouncy flag
 * @param scrolled_view  a scrolled view
 * @return Returns ETK_TURE if the scrolled view is bouncy
 */ 
Etk_Bool etk_scrolled_view_drag_bouncy_get(Etk_Scrolled_View *scrolled_view) 
{
   if (!scrolled_view)
      return ETK_FALSE;
   return scrolled_view->drag.bouncy;
}

/**
 * @brief Set the scrolled view sample interval to calculate the scrolling speed.
 * @param scrolled_view a scrolled view
 * @param interval The interval of sampling latest scrolling speed (minimial 0.2 second, default 0.5 second)
 * @return Returns the actual sampling interval set. If scrolled_view is NULL returns 0.0f. 
 */
double etk_scrolled_view_drag_sample_interval_set(Etk_Scrolled_View *scrolled_view,double interval) 
{
   if (!scrolled_view || scrolled_view->drag.sample_magic == interval)
      return 0.0f;

   interval = interval >= 0.2f ? interval : 0.2f;
   scrolled_view->drag.sample_magic = interval;
   etk_object_notify(ETK_OBJECT(scrolled_view), "drag-sample-interval");
   return scrolled_view->drag.sample_magic;
}

/**
 * @brief Get the scrolled view sample interval to calculate the scrolling speed.
 * @param scrolled_view a scrolled view
 * @return Returns the sampling interval. If scrolled_view is NULL return 0.0f. 
 */
double etk_scrolled_view_drag_sample_interval_get(Etk_Scrolled_View *scrolled_view)
{
   if (!scrolled_view)
      return 0.0f;
   return scrolled_view->drag.sample_magic;
}

/**
 * @brief Set the damping magic number of a dragable scrolled view
 * @param scrolled_view a scrolled view
 * @param damping The damping factor of the dragable scrolled view (default 100)
 * @return Returns the actual damping factor set
 */ 
unsigned int etk_scrolled_view_drag_damping_set(Etk_Scrolled_View *scrolled_view,unsigned int damping)
{
   if (!scrolled_view || scrolled_view->drag.damping_magic == damping)
      return 0;

   scrolled_view->drag.damping_magic = damping;
   etk_object_notify(ETK_OBJECT(scrolled_view), "drag-damping");
   return scrolled_view->drag.damping_magic;
}

/**
 * @brief Get the damping magic number of a dragable scrolled view
 * @param scrolled_view a scrolled view
 * @return Returns the actual damping factor
 */ 
unsigned int etk_scrolled_view_drag_damping_get(Etk_Scrolled_View *scrolled_view) 
{
   if (!scrolled_view)
      return 0;
   return scrolled_view->drag.damping_magic;
}

/**
 * @brief Set the vertical extra margin to the scrolled view
 * @param  scrolled_view a scrolled view
 * @param margin the extra margin in pixels
 */ 
void etk_scrolled_view_extra_vmargin_set(Etk_Scrolled_View *scrolled_view, int margin)
{
   if (!scrolled_view)
      return;
   margin = margin > 0 ? margin : 0;
   scrolled_view->extra_vmargin = margin;
}

/**
 * @brief Get the vertical extra margin to the scrolled view
 * @param  scrolled_view a scrolled view
 * @return Returns the vertical extra margin in pixels
 */ 
int etk_scrolled_view_extra_vmargin_get(Etk_Scrolled_View *scrolled_view)
{
   if (!scrolled_view)
      return -1;
   return scrolled_view->extra_vmargin;
}

/**
 * @brief Set the horizontal extra margin to the scrolled view
 * @param  scrolled_view a scrolled view
 * @param margin the extra margin in pixels
 */ 
void etk_scrolled_view_extra_hmargin_set(Etk_Scrolled_View *scrolled_view, int margin) 
{
   if (!scrolled_view)
      return;
   margin = margin > 0 ? margin : 0;
   scrolled_view->extra_hmargin = margin;
}

/**
 * @brief Get the horizontal extra margin to the scrolled view
 * @param  scrolled_view a scrolled view
 * @return Returns the horizontal extra margin in pixels
 */ 

int etk_scrolled_view_extra_hmargin_get(Etk_Scrolled_View *scrolled_view) 
{
   if (!scrolled_view)
      return -1;
   return scrolled_view->extra_hmargin;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the scrolled view */
static void _etk_scrolled_view_constructor(Etk_Scrolled_View *scrolled_view)
{
   if (!scrolled_view)
      return;

   scrolled_view->viewport = NULL;
   
   scrolled_view->hpolicy = ETK_POLICY_AUTO;
   scrolled_view->vpolicy = ETK_POLICY_AUTO;

   scrolled_view->drag.mouse_down  = ETK_FALSE;
   scrolled_view->drag.bar_pressed = ETK_FALSE;
   scrolled_view->drag.dragable    = ETK_FALSE;
   scrolled_view->drag.bouncy      = ETK_TRUE;
   scrolled_view->drag.damping_magic = ETK_SCROLLED_VIEW_DRAG_DAMPING_MAGIC;
   scrolled_view->extra_hmargin    = 0;
   scrolled_view->extra_vmargin    = 0;

   // FIXME This can be put in etk_config (Make whole system be configured)
   etk_scrolled_view_drag_sample_interval_set(scrolled_view, ETK_SCROLLED_VIEW_DRAG_SAMPLE_INTERVAL_MAGIC);
   etk_scrolled_view_drag_damping_set(scrolled_view, ETK_SCROLLED_VIEW_DRAG_DAMPING_MAGIC);

   scrolled_view->hscrollbar = etk_hscrollbar_new(0.0, 0.0, 0.0, 12.0, 50.0, 0.0);
   etk_widget_theme_parent_set(scrolled_view->hscrollbar, ETK_WIDGET(scrolled_view));
   etk_widget_parent_set(scrolled_view->hscrollbar, ETK_WIDGET(scrolled_view));
   etk_widget_internal_set(scrolled_view->hscrollbar, ETK_TRUE);
   etk_widget_show(scrolled_view->hscrollbar);

   scrolled_view->vscrollbar = etk_vscrollbar_new(0.0, 0.0, 0.0, 12.0, 50.0, 0.0);
   etk_widget_theme_parent_set(scrolled_view->vscrollbar, ETK_WIDGET(scrolled_view));
   etk_widget_parent_set(scrolled_view->vscrollbar, ETK_WIDGET(scrolled_view));
   etk_widget_internal_set(scrolled_view->vscrollbar, ETK_TRUE);
   etk_widget_show(scrolled_view->vscrollbar);

   ETK_WIDGET(scrolled_view)->size_request = _etk_scrolled_view_size_request;
   ETK_WIDGET(scrolled_view)->size_allocate = _etk_scrolled_view_size_allocate;

   etk_signal_connect_by_code(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(scrolled_view), ETK_CALLBACK(_etk_scrolled_view_key_down_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_DOWN_SIGNAL, ETK_OBJECT(scrolled_view), ETK_CALLBACK(_etk_scrolled_view_mouse_down), &scrolled_view->drag);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_UP_SIGNAL, ETK_OBJECT(scrolled_view), ETK_CALLBACK(_etk_scrolled_view_mouse_up), &scrolled_view->drag);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_CLICK_SIGNAL, ETK_OBJECT(scrolled_view), ETK_CALLBACK(_etk_scrolled_view_mouse_click), &scrolled_view->drag);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_MOVE_SIGNAL, ETK_OBJECT(scrolled_view), ETK_CALLBACK(_etk_scrolled_view_mouse_move), &scrolled_view->drag);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_WHEEL_SIGNAL, ETK_OBJECT(scrolled_view), ETK_CALLBACK(_etk_scrolled_view_mouse_wheel), NULL);
   etk_signal_connect_by_code(ETK_CONTAINER_CHILD_ADDED_SIGNAL, ETK_OBJECT(scrolled_view), ETK_CALLBACK(_etk_scrolled_view_child_added_cb), NULL);
   etk_signal_connect_by_code(ETK_CONTAINER_CHILD_REMOVED_SIGNAL, ETK_OBJECT(scrolled_view), ETK_CALLBACK(_etk_scrolled_view_child_removed_cb), NULL);
   etk_signal_connect_by_code(ETK_RANGE_VALUE_CHANGED_SIGNAL, ETK_OBJECT(scrolled_view->hscrollbar), ETK_CALLBACK(_etk_scrolled_view_hscrollbar_value_changed_cb), scrolled_view);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_DOWN_SIGNAL, ETK_OBJECT(scrolled_view->hscrollbar), ETK_CALLBACK(_etk_scrolled_view_bar_mouse_down), scrolled_view);
   etk_signal_connect_by_code(ETK_RANGE_VALUE_CHANGED_SIGNAL, ETK_OBJECT(scrolled_view->vscrollbar), ETK_CALLBACK(_etk_scrolled_view_vscrollbar_value_changed_cb), scrolled_view);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_DOWN_SIGNAL, ETK_OBJECT(scrolled_view->vscrollbar), ETK_CALLBACK(_etk_scrolled_view_bar_mouse_down), scrolled_view);

}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_scrolled_view_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Scrolled_View *scrolled_view;

   if (!(scrolled_view = ETK_SCROLLED_VIEW(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_SCROLLED_VIEW_HPOLICY_PROPERTY:
         etk_scrolled_view_policy_set(scrolled_view, etk_property_value_int_get(value), scrolled_view->vpolicy);
         break;
      case ETK_SCROLLED_VIEW_VPOLICY_PROPERTY:
         etk_scrolled_view_policy_set(scrolled_view, scrolled_view->hpolicy, etk_property_value_int_get(value));
         break;
      case ETK_SCROLLED_VIEW_DRAGABLE_PROPERTY:
         etk_scrolled_view_dragable_set(scrolled_view, etk_property_value_bool_get(value));
         break;
       case ETK_SCROLLED_VIEW_DRAG_BOUNCY_PROPERTY:
         etk_scrolled_view_drag_bouncy_set(scrolled_view, etk_property_value_bool_get(value));
         break;
       case ETK_SCROLLED_VIEW_DRAG_DAMPING_PROPERTY:
         etk_scrolled_view_drag_damping_set(scrolled_view, etk_property_value_int_get(value));
         break;
       case ETK_SCROLLED_VIEW_DRAG_SAMPLE_INTERVAL_PROPERTY:
         etk_scrolled_view_drag_sample_interval_set(scrolled_view, etk_property_value_int_get(value));
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
      case ETK_SCROLLED_VIEW_HPOLICY_PROPERTY:
         etk_property_value_int_set(value, scrolled_view->hpolicy);
         break;
      case ETK_SCROLLED_VIEW_VPOLICY_PROPERTY:
         etk_property_value_int_set(value, scrolled_view->vpolicy);
         break;
      case ETK_SCROLLED_VIEW_DRAGABLE_PROPERTY:
         etk_property_value_bool_set(value, scrolled_view->drag.dragable);
         break;
       case ETK_SCROLLED_VIEW_DRAG_BOUNCY_PROPERTY:
         etk_property_value_bool_set(value, scrolled_view->drag.bouncy);
         break;
       case ETK_SCROLLED_VIEW_DRAG_DAMPING_PROPERTY:
         etk_property_value_int_set(value, scrolled_view->drag.damping_magic);
         break;
       case ETK_SCROLLED_VIEW_DRAG_SAMPLE_INTERVAL_PROPERTY:
         etk_property_value_int_set(value, scrolled_view->drag.sample_magic);
         break;
    default:
         break;
   }
}

/* Calculates the ideal size for the scrolled view */
static void _etk_scrolled_view_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Scrolled_View *scrolled_view;
   Etk_Size hscrollbar_size, vscrollbar_size, child_size;

   if (!(scrolled_view = ETK_SCROLLED_VIEW(widget)) || !size)
      return;

   if (ETK_BIN(scrolled_view)->child)
   {
      etk_widget_size_request_full(scrolled_view->hscrollbar, &hscrollbar_size, ETK_FALSE);
      etk_widget_size_request_full(scrolled_view->vscrollbar, &vscrollbar_size, ETK_FALSE);
      etk_widget_size_request(ETK_BIN(scrolled_view)->child, &child_size);

      size->w = ETK_MAX(child_size.w, hscrollbar_size.w + vscrollbar_size.w);
      size->h = ETK_MAX(child_size.h, hscrollbar_size.h + vscrollbar_size.h);
   }
   else
   {
      size->w = 0;
      size->h = 0;
   }
}

/* Resizes the scrolled view to the allocated size */
static void _etk_scrolled_view_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Scrolled_View *scrolled_view;
   Etk_Size hscrollbar_size, vscrollbar_size;
   Etk_Size scrollview_size;
   Etk_Size scrollbar_size;
   Etk_Size scroll_size;
   Etk_Geometry child_geometry;
   Etk_Widget *child;
   Etk_Bool show_vscrollbar = ETK_FALSE, show_hscrollbar = ETK_FALSE;

   if (!(scrolled_view = ETK_SCROLLED_VIEW(widget)))
      return;

   if (!(child = ETK_BIN(scrolled_view)->child) || !child->scroll_size_get || !child->scroll)
   {
      etk_widget_hide(scrolled_view->hscrollbar);
      etk_widget_hide(scrolled_view->vscrollbar);
      if (child)
         etk_widget_size_allocate(child, geometry);
      return;
   }

   if (scrolled_view->hpolicy == ETK_POLICY_AUTO || scrolled_view->hpolicy == ETK_POLICY_SHOW)
      etk_widget_size_request_full(scrolled_view->hscrollbar, &hscrollbar_size, ETK_FALSE);
   else
   {
      hscrollbar_size.w = 0;
      hscrollbar_size.h = 0;
   }
   if (scrolled_view->vpolicy == ETK_POLICY_AUTO || scrolled_view->vpolicy == ETK_POLICY_SHOW)
      etk_widget_size_request_full(scrolled_view->vscrollbar, &vscrollbar_size, ETK_FALSE);
   else
   {
      vscrollbar_size.w = 0;
      vscrollbar_size.h = 0;
   }

   scrollview_size.w = geometry.w - child->inset.left - child->inset.right;
   scrollview_size.h = geometry.h - child->inset.top - child->inset.bottom;
   if (child->scroll_margins_get)
   {
      Etk_Size margins_size;

      child->scroll_margins_get(child, &margins_size);
      scrollview_size.w -= margins_size.w;
      scrollview_size.h -= margins_size.h;
   }

   child->scroll_size_get(child, scrollview_size, scrollbar_size, &scroll_size);

   if ((scrolled_view->hpolicy == ETK_POLICY_AUTO 
            && (scroll_size.w + 2 * scrolled_view->extra_hmargin) > scrollview_size.w)
         || scrolled_view->hpolicy == ETK_POLICY_SHOW)
   {
      show_hscrollbar = ETK_TRUE;
   }
   if ((scrolled_view->vpolicy == ETK_POLICY_AUTO
         && (scroll_size.h + 2 * scrolled_view->extra_vmargin) > (scrollview_size.h - (show_hscrollbar ? hscrollbar_size.h : 0) ))
      || scrolled_view->vpolicy == ETK_POLICY_SHOW)
   {
      show_vscrollbar = ETK_TRUE;
      if (scrolled_view->hpolicy == ETK_POLICY_AUTO && 
            (scroll_size.w + 2 * scrolled_view->extra_hmargin) > (scrollview_size.w - vscrollbar_size.w))
         show_hscrollbar = ETK_TRUE;
   }

   /* Moves and resizes the hscrollbar */
   if (show_hscrollbar)
   {
      scrollview_size.h -= hscrollbar_size.h;
      etk_widget_show(scrolled_view->hscrollbar);

      child_geometry.x = geometry.x;
      child_geometry.y = geometry.y + geometry.h - hscrollbar_size.h;
      child_geometry.w = geometry.w - (show_vscrollbar ? vscrollbar_size.w : 0);
      child_geometry.h = hscrollbar_size.h;
      etk_widget_size_allocate(scrolled_view->hscrollbar, child_geometry);
   }
   else
      etk_widget_hide(scrolled_view->hscrollbar);

   /* Moves and resizes the vscrollbar */
   if (show_vscrollbar)
   {
      scrollview_size.w -= vscrollbar_size.w;
      etk_widget_show(scrolled_view->vscrollbar);

      child_geometry.x = geometry.x + geometry.w - vscrollbar_size.w;
      child_geometry.y = geometry.y;
      child_geometry.w = vscrollbar_size.w;
      child_geometry.h = geometry.h - (show_hscrollbar ? hscrollbar_size.h : 0);
      etk_widget_size_allocate(scrolled_view->vscrollbar, child_geometry);
   }
   else
      etk_widget_hide(scrolled_view->vscrollbar);

   etk_range_range_set(ETK_RANGE(scrolled_view->hscrollbar), 
         0 - scrolled_view->extra_hmargin, 
         scroll_size.w + scrolled_view->extra_hmargin);
   if (scroll_size.w < scrollview_size.w)
      etk_range_page_size_set(ETK_RANGE(scrolled_view->hscrollbar), scroll_size.w );
   else 
      etk_range_page_size_set(ETK_RANGE(scrolled_view->hscrollbar), scrollview_size.w );
   etk_range_range_set(ETK_RANGE(scrolled_view->vscrollbar), 
         0 - scrolled_view->extra_vmargin, 
         scroll_size.h + scrolled_view->extra_vmargin);
   if (scroll_size.h < scrollview_size.h)
      etk_range_page_size_set(ETK_RANGE(scrolled_view->vscrollbar), scroll_size.h);
   else
      etk_range_page_size_set(ETK_RANGE(scrolled_view->vscrollbar), scrollview_size.h);

   /* Moves and resizes the child */
   child_geometry.x = geometry.x;
   child_geometry.y = geometry.y;
   child_geometry.w = geometry.w - (show_vscrollbar ? vscrollbar_size.w : 0);
   child_geometry.h = geometry.h - (show_hscrollbar ? hscrollbar_size.h : 0);
   etk_widget_size_allocate(child, child_geometry);
}

/* Check if reaching the boundary */
static inline double _etk_scrolled_view_bounce_check(Etk_Range * range, double delta,double v) 
{
   double pos = (range->value + delta);
   if (pos <= range->lower) 
   {
      pos = range->lower + (range->lower - pos);
      v *= -1;
   }
   etk_range_value_set(range, pos);
   if (pos > range->value) 
      v *= -1;
   return v;
}

/* Animator for inertial scrolling */
static int _etk_scrolled_view_motive_bounce(void *data) 
{
   Etk_Scrolled_View *scrolled_view = ETK_SCROLLED_VIEW(data);
   struct Etk_Scrolled_View_Mouse_Drag *drag;
   Etk_Range *vscrollbar_range;
   Etk_Range *hscrollbar_range;
   double delta_time;
   double delta_V;
   double delta_x;
   double delta_y;
   
   if (!scrolled_view) 
      return 0;
   drag = &scrolled_view->drag;
   if (drag->mouse_down) 
      return 0;

   // Using trapezoid method to calculate the distance. 
   delta_time = ecore_time_get() - drag->timestamp;
   delta_V = delta_time * (drag->damping_magic + abs(drag->Vx)+ abs(drag->Vy)) / 2;
   delta_time = delta_time < 0.01 ? 0.01 : delta_time;
   
   if (drag->Vx < delta_V && drag->Vx > -delta_V) 
   {
      delta_x = 0;
      drag->Vx = 0;
   } 
   else if (drag->Vx > 0)
   {
      delta_x = ((drag->Vx * 2) - delta_V) * delta_time / 2;
      drag->Vx = drag->Vx - delta_V;
   } 
   else 
   {
      delta_x = ((drag->Vx * 2) + delta_V) * delta_time / 2;
      drag->Vx = drag->Vx +  delta_V;
   }

   if (drag->Vy < delta_V && drag->Vy > -delta_V) 
   {
      drag->Vy = 0;
      delta_y = 0;
   } 
   else if(drag->Vy > 0)
   {
      delta_y = ((drag->Vy * 2) - delta_V) * delta_time / 2;
      drag->Vy = drag->Vy - delta_V;
   } else 
   {
      delta_y = ((drag->Vy * 2) + delta_V) * delta_time / 2;
      drag->Vy = drag->Vy + delta_V;
   }

   if (drag->Vx == 0 && drag->Vy == 0) 
      return 0;

   vscrollbar_range = ETK_RANGE(scrolled_view->vscrollbar);
   hscrollbar_range = ETK_RANGE(scrolled_view->hscrollbar);
   if (drag->bouncy)  
   {
      drag->Vx = _etk_scrolled_view_bounce_check(hscrollbar_range, delta_x, drag->Vx);
      drag->Vy = _etk_scrolled_view_bounce_check(vscrollbar_range, delta_y, drag->Vy);
   }
   else 
   {
      drag->Vx = drag->Vx == _etk_scrolled_view_bounce_check(hscrollbar_range, delta_x, drag->Vx) ? drag->Vx : 0.0f;
      drag->Vy = drag->Vy == _etk_scrolled_view_bounce_check(vscrollbar_range, delta_y, drag->Vy) ? drag->Vy : 0.0f;
   }
   drag->timestamp = ecore_time_get();
   return 1;
}


/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the user presses a key */
static Etk_Bool _etk_scrolled_view_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Scrolled_View *scrolled_view;
   Etk_Range *hscrollbar_range;
   Etk_Range *vscrollbar_range;
   Etk_Bool propagate = ETK_FALSE;

   if (!(scrolled_view = ETK_SCROLLED_VIEW(object)))
      return ETK_TRUE;
   hscrollbar_range = ETK_RANGE(scrolled_view->hscrollbar);
   vscrollbar_range = ETK_RANGE(scrolled_view->vscrollbar);

   if (strcmp(event->keyname, "Right") == 0)
      etk_range_value_set(hscrollbar_range, hscrollbar_range->value + hscrollbar_range->step_increment);
   else if (strcmp(event->keyname, "Down") == 0)
      etk_range_value_set(vscrollbar_range, vscrollbar_range->value + vscrollbar_range->step_increment);
   else if (strcmp(event->keyname, "Left") == 0)
      etk_range_value_set(hscrollbar_range, hscrollbar_range->value - hscrollbar_range->step_increment);
   else if (strcmp(event->keyname, "Up") == 0)
      etk_range_value_set(vscrollbar_range, vscrollbar_range->value - vscrollbar_range->step_increment);
   else if (strcmp(event->keyname, "Home") == 0)
      etk_range_value_set(vscrollbar_range, vscrollbar_range->lower + scrolled_view->extra_vmargin);
   else if (strcmp(event->keyname, "End") == 0)
      etk_range_value_set(vscrollbar_range, vscrollbar_range->upper);
   else if (strcmp(event->keyname, "Next") == 0)
      etk_range_value_set(vscrollbar_range, vscrollbar_range->value + vscrollbar_range->page_increment);
   else if (strcmp(event->keyname, "Prior") == 0)
      etk_range_value_set(vscrollbar_range, vscrollbar_range->value - vscrollbar_range->page_increment);
   else
      propagate = ETK_TRUE;

   return propagate;
}

/* Called when mouse button has been pressed down */
static Etk_Bool _etk_scrolled_view_mouse_down(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data) 
{
   Etk_Scrolled_View *scrolled_view;
   Etk_Range *vscrollbar_range;
   Etk_Range *hscrollbar_range;
   struct Etk_Scrolled_View_Mouse_Drag *drag = (struct Etk_Scrolled_View_Mouse_Drag *) data;

   if (!(scrolled_view = ETK_SCROLLED_VIEW(object)))
      return ETK_FALSE;

   if (!drag->dragable) 
      return ETK_FALSE;

   if (!drag->mouse_down && event->button) 
   {
      int i;
      
      vscrollbar_range = ETK_RANGE(scrolled_view->vscrollbar);
      hscrollbar_range = ETK_RANGE(scrolled_view->hscrollbar);
      drag->mouse_down = ETK_TRUE;
      for (i = 0; i < 5; i++) drag->prev_position_timestamp[i] = 0.0;
      drag->timestamp = ecore_time_get(); 
      drag->old_timestamp = 0.0;
      drag->Vx = 0;
      drag->Vy = 0;
      drag->position.x = -1;
      drag->position.y = -1;
      drag->down_position = drag->position = event->widget;
      drag->bar_position.x = hscrollbar_range->value;
      drag->bar_position.y = vscrollbar_range->value;
      etk_viewport_hold_events_set(ETK_VIEWPORT(scrolled_view->viewport), ETK_FALSE);
   }
   return ETK_FALSE;
}

/* Called when mouse is dragging */
static Etk_Bool _etk_scrolled_view_mouse_move(Etk_Object *object, Etk_Event_Mouse_Move *event, void *data) 
{
   Etk_Scrolled_View *scrolled_view;
   Etk_Range *vscrollbar_range;
   Etk_Range *hscrollbar_range;
   double delta_time, t;
   struct Etk_Scrolled_View_Mouse_Drag *drag = (struct Etk_Scrolled_View_Mouse_Drag *) data;
   int i;
   
   if (!(scrolled_view = ETK_SCROLLED_VIEW(object)))
      return ETK_FALSE;

   if (!drag->dragable)
      return ETK_FALSE;

   if (!drag->mouse_down) 
      return ETK_FALSE;

   if (!event->buttons) 
      return ETK_FALSE;

   vscrollbar_range = ETK_RANGE(scrolled_view->vscrollbar);
   hscrollbar_range = ETK_RANGE(scrolled_view->hscrollbar);
   if (drag->scroll_flag == 0) 
   {
         drag->scroll_flag = (event->cur.widget.y - drag->position.y)/vscrollbar_range->step_increment || (event->cur.widget.x - drag->position.x) / hscrollbar_range->step_increment;
   }

   if (drag->scroll_flag) 
   {
      if ((drag->position.x == event->cur.widget.x) &&
	  (drag->position.y == event->cur.widget.y))
	return ETK_TRUE;
      /* if we have dragged beyond 15 pixels from the down point, in any
       * direction, set on hold flag */
      if ((((event->cur.widget.x - drag->down_position.x) *
	    (event->cur.widget.x - drag->down_position.x)) +
	   ((event->cur.widget.y - drag->down_position.y) *
	    (event->cur.widget.y - drag->down_position.y))) >
	  (15 * 15))
      {
	 etk_viewport_hold_events_set(ETK_VIEWPORT(scrolled_view->viewport), ETK_TRUE);
      }
      if (drag->bar_pressed == ETK_FALSE) 
      {
         double _value = vscrollbar_range->value - (event->cur.widget.y - drag->position.y);
         if (_value > vscrollbar_range->lower && _value < vscrollbar_range->upper) {
            etk_range_value_set(vscrollbar_range, _value);
         }
         _value = hscrollbar_range->value - (event->cur.widget.x - drag->position.x);
         if (_value > hscrollbar_range->lower && _value < hscrollbar_range->upper) {
            etk_range_value_set(hscrollbar_range, _value);
         }
      }
      t = ecore_time_get();
      /* record the last 5 mouse moves and timestamps */
      for (i = 5; i >= 1; i--)
	{
	   drag->prev_position[i] = drag->prev_position[i - 1];
	   drag->prev_position_timestamp[i] = drag->prev_position_timestamp[i - 1];
	}
      drag->prev_position_timestamp[0] = drag->position_timestamp;
      drag->prev_position[0] = drag->position;
      
      drag->position_timestamp = t;
      drag->position = event->cur.widget;
      delta_time = ecore_time_get() - drag->timestamp;
      // in case delta_time is zero
      delta_time = delta_time == 0.0f ? drag->sample_magic : delta_time;

      if ((delta_time > drag->sample_magic) || (drag->old_timestamp == 0.0)) 
      {
         drag->old_timestamp = drag->timestamp;
         drag->timestamp = ecore_time_get();
         drag->Vx = (drag->Vx + (hscrollbar_range->value - drag->bar_position.x) / delta_time) / 2;
         drag->Vy = (drag->Vy + (vscrollbar_range->value - drag->bar_position.y) / delta_time) / 2;
         drag->bar_position.x = hscrollbar_range->value;
         drag->bar_position.y = vscrollbar_range->value;
      }
      return ETK_TRUE;
   }
   return ETK_FALSE;
}

/* Called when mouse button has been released */
static Etk_Bool _etk_scrolled_view_mouse_up(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data) 
{
   Etk_Scrolled_View *scrolled_view;
   struct Etk_Scrolled_View_Mouse_Drag *drag = (struct Etk_Scrolled_View_Mouse_Drag *) data;

   if (!(scrolled_view = ETK_SCROLLED_VIEW(object)))
      return ETK_FALSE;

   etk_viewport_hold_events_set(ETK_VIEWPORT(scrolled_view->viewport), ETK_FALSE);
   
   if (!drag->dragable) 
      return ETK_FALSE;

   if (!drag->mouse_down) 
      return ETK_FALSE;

   if (drag->bar_pressed == ETK_TRUE) 
   {
      drag->bar_pressed = ETK_FALSE;
      return ETK_FALSE;
   }

   drag->mouse_down = ETK_FALSE;

   if (drag->scroll_flag) 
   {
      int max_speed = ETK_SCROLLED_VIEW_DRAG_DAMPING_MAGIC*5;
      int i, idelt;
      double tlast, tdelt;
      Etk_Position lastpos;
      
      drag->old_timestamp = drag->timestamp;
      drag->timestamp = ecore_time_get();
      
      /* now check to see the final mouse move was not > 4 times the time
       * between other moves (like a stop at the end - an aberration),
       * and check to see over the past 5 moves or 0.2 seconds - whichever
       * is "less", what the total movement is - if its more than 20
       * pixels - then have momentum */
      tlast = drag->position_timestamp - 0.2;
      lastpos = drag->position;
      
      tdelt = drag->position_timestamp;
      idelt = 0;
      for (i = 0; i < 5; i++)
	{
	   if (drag->prev_position_timestamp[i] >= tlast)
	     {
		lastpos = drag->prev_position[i];
		tdelt = drag->prev_position_timestamp[i];
		idelt++;
	     }
	}
      if (idelt < 1)
	return ETK_FALSE;
      if ((tdelt / idelt) * 4 < (drag->position_timestamp - drag->prev_position_timestamp[0]))
	return ETK_FALSE;
      if ((((drag->position.x - lastpos.x) * (drag->position.x - lastpos.x)) +
	   ((drag->position.y - lastpos.y) * (drag->position.y - lastpos.y)))
	  <= (20 * 20))
	return ETK_FALSE;
      drag->Vx = drag->Vx > 0 ?  
         drag->Vx > max_speed ?  max_speed : drag->Vx : 
         drag->Vx < -max_speed ? -max_speed : drag->Vx;
      drag->Vy = drag->Vy > 0 ?  
         drag->Vy > max_speed ?  max_speed : drag->Vy : 
         drag->Vy < -max_speed ? -max_speed : drag->Vy;

      ecore_animator_add(_etk_scrolled_view_motive_bounce, scrolled_view);
      return ETK_TRUE;
   }
   return ETK_FALSE;
}

/* Called when mouse button has been clicked */
static Etk_Bool _etk_scrolled_view_mouse_click (Etk_Object *object, Etk_Event_Mouse_Up *event, void *data) 
{
   Etk_Scrolled_View *scrolled_view;
   struct Etk_Scrolled_View_Mouse_Drag *drag = (struct Etk_Scrolled_View_Mouse_Drag *) data;
   if (!(scrolled_view = ETK_SCROLLED_VIEW(object)))
      return ETK_TRUE;

   if (drag->scroll_flag) 
   {
      drag->scroll_flag = 0;
      return ETK_TRUE;
   }
   return ETK_FALSE;
}

/* Called when the user wants to scroll the scrolled view with the mouse wheel */
static Etk_Bool _etk_scrolled_view_mouse_wheel(Etk_Object *object, Etk_Event_Mouse_Wheel *event, void *data)
{
   Etk_Scrolled_View *scrolled_view;
   Etk_Range *vscrollbar_range;

   if (!(scrolled_view = ETK_SCROLLED_VIEW(object)))
      return ETK_TRUE;

   vscrollbar_range = ETK_RANGE(scrolled_view->vscrollbar);
   etk_range_value_set(vscrollbar_range, vscrollbar_range->value + event->z * vscrollbar_range->step_increment);
   return ETK_FALSE;
}

/* Called when the value of the hscrollbar has changed */
static Etk_Bool _etk_scrolled_view_hscrollbar_value_changed_cb(Etk_Object *object, double value, void *data)
{
   Etk_Scrolled_View *scrolled_view;
   Etk_Widget *child;

   if (!(scrolled_view = ETK_SCROLLED_VIEW(data)) || !(child = ETK_BIN(scrolled_view)->child) || !child->scroll)
      return ETK_TRUE;
   child->scroll(child, value, ETK_RANGE(scrolled_view->vscrollbar)->value);

   return ETK_TRUE;
}

/* Called when dragging on the scrollbar */
static Etk_Bool _etk_scrolled_view_bar_mouse_down(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data) 
{
   Etk_Scrolled_View *scrolled_view;
   Etk_Widget *child;
   if (!(scrolled_view = ETK_SCROLLED_VIEW(data)) || !(child = ETK_BIN(scrolled_view)->child) || !child->scroll)
      return ETK_FALSE;
   scrolled_view->drag.bar_pressed = ETK_TRUE;
   return ETK_FALSE;
}


/* Called when the value of the vscrollbar has changed */
static Etk_Bool _etk_scrolled_view_vscrollbar_value_changed_cb(Etk_Object *object, double value, void *data)
{
   Etk_Scrolled_View *scrolled_view;
   Etk_Widget *child;

   if (!(scrolled_view = ETK_SCROLLED_VIEW(data)) || !(child = ETK_BIN(scrolled_view)->child) || !child->scroll)
      return ETK_TRUE;
   child->scroll(child, ETK_RANGE(scrolled_view->hscrollbar)->value, value);

   return ETK_TRUE;
}

/* Called when a new child is added */
static Etk_Bool _etk_scrolled_view_child_added_cb(Etk_Object *object, void *child, void *data)
{
   if (!object || !child)
      return ETK_TRUE;
   etk_signal_connect_by_code(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(child),
      ETK_CALLBACK(_etk_scrolled_view_child_scroll_size_changed_cb), object);
   return ETK_TRUE;
}

/* Called when a child is removed */
static Etk_Bool _etk_scrolled_view_child_removed_cb(Etk_Object *object, void *child, void *data)
{
   if (!object || !child)
      return ETK_TRUE;
   etk_signal_disconnect_by_code(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(child),
      ETK_CALLBACK(_etk_scrolled_view_child_scroll_size_changed_cb), object);
   return ETK_TRUE;
}

/* Called when the scroll size of the scrolled view's child has changed */
static Etk_Bool _etk_scrolled_view_child_scroll_size_changed_cb(Etk_Object *object, void *data)
{
   Etk_Widget *child;
   Etk_Scrolled_View *scrolled_view;
   Etk_Size hscrollbar_requisition, vscrollbar_requisition;
   Etk_Size scrollview_size;
   Etk_Size scrollbar_size;
   Etk_Size scroll_size;

   if (!(child = ETK_WIDGET(object)) || !child->scroll_size_get || !(scrolled_view = ETK_SCROLLED_VIEW(data)))
      return ETK_TRUE;

   if (scrolled_view->hpolicy == ETK_POLICY_AUTO || scrolled_view->hpolicy == ETK_POLICY_SHOW)
      etk_widget_size_request_full(scrolled_view->hscrollbar, &hscrollbar_requisition, ETK_FALSE);
   else
   {
      hscrollbar_requisition.w = 0;
      hscrollbar_requisition.h = 0;
   }
   if (scrolled_view->vpolicy == ETK_POLICY_AUTO || scrolled_view->vpolicy == ETK_POLICY_SHOW)
      etk_widget_size_request_full(scrolled_view->vscrollbar, &vscrollbar_requisition, ETK_FALSE);
   else
   {
      vscrollbar_requisition.w = 0;
      vscrollbar_requisition.h = 0;
   }

   etk_widget_inner_geometry_get(ETK_WIDGET(scrolled_view), NULL, NULL, &scrollview_size.w, &scrollview_size.h);
   scrollbar_size.w = vscrollbar_requisition.w;
   scrollbar_size.h = hscrollbar_requisition.h;
   child->scroll_size_get(child, scrollview_size, scrollbar_size, &scroll_size);

   etk_range_range_set(ETK_RANGE(scrolled_view->hscrollbar), 
         0 - scrolled_view->extra_hmargin, 
         scroll_size.w + scrolled_view->extra_hmargin);
   etk_range_range_set(ETK_RANGE(scrolled_view->vscrollbar), 
         0 - scrolled_view->extra_vmargin, 
         scroll_size.h + scrolled_view->extra_vmargin);
   etk_widget_redraw_queue(ETK_WIDGET(scrolled_view));

   return ETK_TRUE;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Scrolled_View
 *
 * @image html widgets/scrolled_view.png
 * A scrolled view is made up of a hscrollbar which controls the horizontal scrolling of the child, and of a
 * vscrollbar which controls the vertical scrolling of the child. @n
 * These two scrollbars can have different visibility policy:
 * - <b>ETK_POLICY_SHOW</b>: the scrollbar is always shown
 * - <b>ETK_POLICY_HIDE</b>: the scrollbar is always hidden
 * - <b>ETK_POLICY_AUTO</b>: the scrollbar is shown and hidden automatically whether or not the child can fit entirely in the
 * scrolled view
 *
 * The visibility policy can be set with etk_scrolled_view_policy_set(). @n
 *
 * Most of the widgets doesn't have a scrolling ability, which means that you have to create an Etk_Viewport that
 * implements this ability, attach the child to the viewport, and add the viewport to the scrolled view.
 * etk_scrolled_view_add_with_viewport() is a convenient function that does that for you. @n @n
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Scrolled_View
 *
 * \par Properties:
 * @prop_name "hpolicy": The visibility policy of the horizontal scrollbar
 * @prop_type Integer (Etk_Scrolled_View_Policy)
 * @prop_rw
 * @prop_val ETK_POLICY_AUTO
 * \par
 * @prop_name "vpolicy": The visibility policy of the vertical scrollbar
 * @prop_type Integer (Etk_Scrolled_View_Policy)
 * @prop_rw
 * @prop_val ETK_POLICY_AUTO
 */
