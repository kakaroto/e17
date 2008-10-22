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

/** @file etk_container.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_container.h"

#include <stdlib.h>

#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Container
 * @{
 */

int ETK_CONTAINER_CHILD_ADDED_SIGNAL;
int ETK_CONTAINER_CHILD_REMOVED_SIGNAL;

enum Etk_Container_Property_Id
{
   ETK_CONTAINER_BORDER_WIDTH_PROPERTY
};

static void _etk_container_constructor(Etk_Container *container);
static void _etk_container_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_container_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static Etk_Bool _etk_container_child_added_cb(Etk_Object *object, Etk_Widget *child, void *data);
static Etk_Bool _etk_container_child_removed_cb(Etk_Object *object, Etk_Widget *child, void *data);
static void _etk_container_child_parent_changed_cb(Etk_Object *object, const char *property_name, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Container
 * @return Returns the type of an Etk_Container
 */
Etk_Type *etk_container_type_get(void)
{
   static Etk_Type *container_type = NULL;

   if (!container_type)
   {
      const Etk_Signal_Description signals[] = {
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_CONTAINER_CHILD_ADDED_SIGNAL,
            "child-added", etk_marshaller_OBJECT),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_CONTAINER_CHILD_REMOVED_SIGNAL,
            "child-removed", etk_marshaller_OBJECT),
         ETK_SIGNAL_DESCRIPTION_SENTINEL
      };

      container_type = etk_type_new("Etk_Container", ETK_WIDGET_TYPE,
         sizeof(Etk_Container), ETK_CONSTRUCTOR(_etk_container_constructor),
         NULL, signals);

      etk_type_property_add(container_type, "border-width", ETK_CONTAINER_BORDER_WIDTH_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(0));

      container_type->property_set = _etk_container_property_set;
      container_type->property_get = _etk_container_property_get;
   }

   return container_type;
}

/**
 * @brief Adds a child to the container
 * @param container a container
 * @param widget the widget to add
 */
void etk_container_add(Etk_Container *container, Etk_Widget *widget)
{
   if (!container || !widget || !container->child_add)
      return;
   container->child_add(container, widget);
}

/**
 * @brief Removes a child from its container. It is equivalent to etk_widget_parent_set(widget, NULL)
 * @param widget the widget to remove
 */
void etk_container_remove(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_parent_set(widget, NULL);
}

/**
 * @brief Unpacks all the children of the container
 * @param container a container
 */
void etk_container_remove_all(Etk_Container *container)
{
   Eina_List *children, *l;

   if (!container)
      return;

   children = etk_container_children_get(container);
   for (l = children; l; l = l->next)
      etk_container_remove(ETK_WIDGET(l->data));
   eina_list_free(children);
}

/**
 * @brief Sets the border width of a container. The border width is the amount of space left around the inside of
 * the container. To add free space around the outside of a container, you can use etk_widget_padding_set()
 * @param container a container
 * @param border_width the border width to set
 * @see etk_widget_padding_set()
 */
void etk_container_border_width_set(Etk_Container *container, int border_width)
{
   if (!container)
      return;

   container->border_width = border_width;
   etk_widget_size_recalc_queue(ETK_WIDGET(container));
   etk_object_notify(ETK_OBJECT(container), "border-width");
}

/**
 * @brief Gets the border width of the container
 * @param container a container
 * @return Returns the border width of the container
 */
int etk_container_border_width_get(Etk_Container *container)
{
   if (!container)
      return 0;
   return container->border_width;
}

/**
 * @brief Gets the list of the children of the container. It simply calls the "childrend_get()" method of the container
 * @param container a container
 * @return Returns the list of the container's children
 * @note The returned list will have to be freed with eina_list_free() when you no longer need it
 */
Eina_List *etk_container_children_get(Etk_Container *container)
{
   if (!container || !container->children_get)
      return NULL;
   return container->children_get(container);
}

/**
 * @brief Gets whether the widget is a child of the container
 * @param container a container
 * @param widget the widget you want to check if it is a child of the container
 * @return Returns ETK_TRUE if the widget is a child of the container, ETK_FALSE otherwise
 */
Etk_Bool etk_container_is_child(Etk_Container *container, Etk_Widget *widget)
{
   Eina_List *children;
   Etk_Bool is_child;

   if (!container || !widget)
      return ETK_FALSE;

   children = etk_container_children_get(container);
   is_child = (eina_list_data_find(children, widget) != NULL);
   eina_list_free(children);

   return is_child;
}

/**
 * @brief Calls @a for_each_cb(child) for each child of the container
 * @param container the container
 * @param for_each_cb the function to call
 */
void etk_container_for_each(Etk_Container *container, void (*for_each_cb)(Etk_Widget *child))
{
   Eina_List *children, *l;

   if (!container || !for_each_cb)
      return;

   children = etk_container_children_get(container);
   for (l = children; l; l = l->next)
      for_each_cb(ETK_WIDGET(l->data));
   eina_list_free(children);
}

/**
 * @brief Calls @a for_each_cb(child, data) for each child of the container
 * @param container the container
 * @param for_each_cb the function to call
 * @param data the data to pass as the second argument of @a for_each_cb()
 */
void etk_container_for_each_data(Etk_Container *container, void (*for_each_cb)(Etk_Widget *child, void *data), void *data)
{
   Eina_List *children, *l;

   if (!container || !for_each_cb)
      return;

   children = etk_container_children_get(container);
   for (l = children; l; l = l->next)
      for_each_cb(ETK_WIDGET(l->data), data);
   eina_list_free(children);
}

/**
 * @brief A utility function that resizes the given space according to the specified fill-policy.
 * It is mainly used by container implementations
 * @param child a child
 * @param child_space the allocated space for the child. It will be modified according to the fill options
 * @param hfill if @a hfill == ETK_TRUE, the child will fill the space horizontally
 * @param vfill if @a vfill == ETK_TRUE, the child will fill the space vertically
 * @param xalign the horizontal alignment of the child widget in the child space (has no effect if @a hfill is ETK_TRUE)
 * @param yalign the vertical alignment of the child widget in the child space (has no effect if @a vfill is ETK_TRUE)
 */
void etk_container_child_space_fill(Etk_Widget *child, Etk_Geometry *child_space, Etk_Bool hfill, Etk_Bool vfill, float xalign, float yalign)
{
   Etk_Size min_size;

   if (!child || !child_space)
      return;

   xalign = ETK_CLAMP(xalign, 0.0, 1.0);
   yalign = ETK_CLAMP(yalign, 0.0, 1.0);

   etk_widget_size_request(child, &min_size);
   if (!hfill && child_space->w > min_size.w)
   {
      child_space->x += (child_space->w - min_size.w) * xalign;
      child_space->w = min_size.w;
   }
   if (!vfill && child_space->h > min_size.h)
   {
      child_space->y += (child_space->h - min_size.h) * yalign;
      child_space->h = min_size.h;
   }
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the container */
static void _etk_container_constructor(Etk_Container *container)
{
   if (!container)
      return;

   container->child_add = NULL;
   container->child_remove = NULL;
   container->children_get = NULL;
   container->border_width = 0;

   etk_signal_connect_by_code(ETK_CONTAINER_CHILD_ADDED_SIGNAL, ETK_OBJECT(container), ETK_CALLBACK(_etk_container_child_added_cb), NULL);
   etk_signal_connect_by_code(ETK_CONTAINER_CHILD_REMOVED_SIGNAL, ETK_OBJECT(container), ETK_CALLBACK(_etk_container_child_removed_cb), NULL);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_container_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Container *container;

   if (!(container = ETK_CONTAINER(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_CONTAINER_BORDER_WIDTH_PROPERTY:
         etk_container_border_width_set(container, etk_property_value_int_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_container_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Container *container;

   if (!(container = ETK_CONTAINER(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_CONTAINER_BORDER_WIDTH_PROPERTY:
         etk_property_value_int_set(value, etk_container_border_width_get(container));
         break;
      default:
         break;
   }
}

/**************************
 *
 * Handlers and callbacks
 *
 **************************/

/* Called when a child is added to the container */
static Etk_Bool _etk_container_child_added_cb(Etk_Object *object, Etk_Widget *child, void *data)
{
   Etk_Container *container;

   if (!(container = ETK_CONTAINER(object)) || !child)
      return ETK_TRUE;

   etk_object_notification_callback_add(ETK_OBJECT(child), "parent",
         _etk_container_child_parent_changed_cb, container);
   return ETK_TRUE;
}

/* Called when a child is removed from the container */
static Etk_Bool _etk_container_child_removed_cb(Etk_Object *object, Etk_Widget *child, void *data)
{
   Etk_Container *container;

   if (!(container = ETK_CONTAINER(object)) || !child)
      return ETK_TRUE;

   etk_object_notification_callback_remove(ETK_OBJECT(child), "parent", _etk_container_child_parent_changed_cb);
   return ETK_TRUE;
}

/* Called when a child of the container is reparented */
static void _etk_container_child_parent_changed_cb(Etk_Object *object, const char *property_name, void *data)
{
   Etk_Container *container;
   Etk_Widget *child;

   if (!(child = ETK_WIDGET(object)) || !(container = ETK_CONTAINER(data)))
      return;

   etk_object_notification_callback_remove(ETK_OBJECT(child), "parent", _etk_container_child_parent_changed_cb);
   if (container->child_remove)
      container->child_remove(container, child);
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Container
 *
 * Etk_Container is an abstract class which offers methods to add or remove children to the inheriting container: @n
 * - etk_container_add() calls the @a child_add() method of the inheriting container. For example, etk_container_add()
 * on a bin will call etk_bin_child_set(), and etk_container_add() on a box will call etk_box_append() with default
 * packing settings.
 * But most of the time, you will rather have to call directly the appropriate function of the container's API in
 * order to have more control on the packing settings. For instance, you'll have to call directly etk_box_append() with
 * the @a ETK_BOX_END parameter to pack a child at the end of a box since etk_container_add() would only pack the child
 * at the start of the box (default behavior). @n
 * - etk_container_remove() unparents the given widget, which will result in calling the @a child_remove() method
 * of the container containing the removed widget. @n
 *
 * You can also get the list of the container's children with etk_container_children_get() and checks if a widget is
 * a child of the container with etk_container_is_child(). @n
 * Note that when a container is destroyed, all its children are automatically destroyed too. If you want to avoid that,
 * before destroying the container, you can call etk_container_remove_all().
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *
 * \par Signals:
 * @signal_name "child-added": Emitted when a child has been added to the container
 * @signal_cb Etk_Bool callback(Etk_Container *container, Etk_Widget *child, void *data)
 * @signal_arg container: the container connected to the callback
 * @signal_arg child: the child which has been added
 * @signal_data
 * \par
 * @signal_name "child-removed": Emitted when a child has been removed from the container
 * @signal_cb Etk_Bool callback(Etk_Container *container, Etk_Widget *child, void *data)
 * @signal_arg container: the container connected to the callback
 * @signal_arg child: the child which has been removed
 * @signal_data
 *
 * \par Properties:
 * @prop_name "border-width": The amount of space left around the inside of the container
 * @prop_type Integer
 * @prop_rw
 * @prop_val 0
 */
