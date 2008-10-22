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

/** @file etk_paned.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_paned.h"
#include <stdlib.h>
#include <Evas.h>
#include "etk_separator.h"
#include "etk_toplevel.h"
#include "etk_event.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Paned
 * @{
 */

enum Etk_Paned_Property_Id
{
   ETK_PANED_POSITION_PROPERTY
};

static void _etk_paned_constructor(Etk_Paned *paned);
static void _etk_hpaned_constructor(Etk_HPaned *hpaned);
static void _etk_vpaned_constructor(Etk_VPaned *vpaned);
static void _etk_hpaned_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_vpaned_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_hpaned_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_vpaned_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_paned_child_add(Etk_Container *container, Etk_Widget *widget);
static void _etk_paned_child_remove(Etk_Container *container, Etk_Widget *widget);
static Eina_List *_etk_paned_children_get(Etk_Container *container);

static Etk_Bool _etk_paned_separator_mouse_in_cb(Etk_Object *object, Etk_Event_Mouse_In *event, void *data);
static Etk_Bool _etk_paned_separator_mouse_out_cb(Etk_Object *object, Etk_Event_Mouse_Out *event, void *data);
static Etk_Bool _etk_paned_separator_mouse_up_cb(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data);
static Etk_Bool _etk_paned_separator_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data);
static Etk_Bool _etk_paned_separator_mouse_move_cb(Etk_Object *object, Etk_Event_Mouse_Move *event, void *data);

static void _etk_hpaned_position_calc(Etk_Paned *paned);
static void _etk_vpaned_position_calc(Etk_Paned *paned);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Paned
 * @return Returns the type of an Etk_Paned
 */
Etk_Type *etk_paned_type_get(void)
{
   static Etk_Type *paned_type = NULL;

   if (!paned_type)
   {
      paned_type = etk_type_new("Etk_Paned", ETK_CONTAINER_TYPE, sizeof(Etk_Paned),
            ETK_CONSTRUCTOR(_etk_paned_constructor), NULL, NULL);

      etk_type_property_add(paned_type, "position", ETK_PANED_POSITION_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(0));
   }

   return paned_type;
}

/**
 * @internal
 * @brief Gets the type of an Etk_HPaned
 * @return Returns the type of an Etk_HPaned
 */
Etk_Type *etk_hpaned_type_get(void)
{
   static Etk_Type *hpaned_type = NULL;

   if (!hpaned_type)
   {
      hpaned_type = etk_type_new("Etk_HPaned", ETK_PANED_TYPE, sizeof(Etk_HPaned),
            ETK_CONSTRUCTOR(_etk_hpaned_constructor), NULL, NULL);
   }

   return hpaned_type;
}

/**
 * @internal
 * @brief Gets the type of an Etk_VPaned
 * @return Returns the type of an Etk_VPaned
 */
Etk_Type *etk_vpaned_type_get(void)
{
   static Etk_Type *vpaned_type = NULL;

   if (!vpaned_type)
   {
      vpaned_type = etk_type_new("Etk_VPaned", ETK_PANED_TYPE, sizeof(Etk_VPaned),
            ETK_CONSTRUCTOR(_etk_vpaned_constructor), NULL, NULL);
   }

   return vpaned_type;
}

/**
 * @brief Creates a new horizontal paned container
 * @return Returns the horizontal paned container
 */
Etk_Widget *etk_hpaned_new(void)
{
   return etk_widget_new(ETK_HPANED_TYPE, "theme-group", "hpaned", NULL);
}

/**
 * @brief Creates a new vertical paned container
 * @return Returns the vertical paned container
 */
Etk_Widget *etk_vpaned_new(void)
{
   return etk_widget_new(ETK_VPANED_TYPE, "theme-group", "vpaned", NULL);
}

/**
 * @brief Sets the first child of the paned (the left one for a hpaned, the top one for a vpaned)
 * @param paned a paned
 * @param child the child to set
 * @param expand if @a expand is ETK_TRUE, when the paned will be resized, the child will try to expand
 * as much as possible
 */
void etk_paned_child1_set(Etk_Paned *paned, Etk_Widget *child, Etk_Bool expand)
{
   if (!paned || paned->child1 == child)
      return;

   if (paned->child1)
      etk_container_remove(paned->child1);

   paned->child1 = child;
   paned->expand1 = expand;
   if (child)
   {
      etk_widget_parent_set(child, ETK_WIDGET(paned));
      etk_widget_raise(paned->separator);
      etk_signal_emit(ETK_CONTAINER_CHILD_ADDED_SIGNAL, ETK_OBJECT(paned),
                      child);
   }
}

/**
 * @brief Sets the second child of the paned (the right one for a hpaned, the bottom one for a vpaned)
 * @param paned a paned
 * @param child the child to set
 * @param expand if @a expand is ETK_TRUE, when the paned will be resized, the first child will try to expand
 * as much as possible
 */
void etk_paned_child2_set(Etk_Paned *paned, Etk_Widget *child, Etk_Bool expand)
{
   if (!paned || paned->child2 == child)
      return;

   if (paned->child2)
      etk_container_remove(paned->child2);

   paned->child2 = child;
   paned->expand2 = expand;
   if (child)
   {
      etk_widget_parent_set(child, ETK_WIDGET(paned));
      etk_widget_raise(paned->separator);
      etk_signal_emit(ETK_CONTAINER_CHILD_ADDED_SIGNAL, ETK_OBJECT(paned),
                      child);
   }
}

/**
 * @brief Gets the first child of the paned (the left one for a hpaned, the top one for a vpaned)
 * @param paned a paned
 * @return Returns the first child of the paned
 */
Etk_Widget *etk_paned_child1_get(Etk_Paned *paned)
{
   if (!paned)
      return NULL;
   return paned->child1;
}

/**
 * @brief Gets the second child of the paned (the right one for a hpaned, the bottom one for a vpaned)
 * @param paned a paned
 * @return Returns the second child of the paned
 */
Etk_Widget *etk_paned_child2_get(Etk_Paned *paned)
{
   if (!paned)
      return NULL;
   return paned->child2;
}

/**
 * @brief Sets whether the first child should expand as much as possible when the paned is resized
 * @param paned a paned
 * @param expand ETK_TRUE to make the first child expand, ETK_FALSE otherwise
 */
void etk_paned_child1_expand_set(Etk_Paned *paned, Etk_Bool expand)
{
   if (!paned)
      return;
   paned->expand1 = expand;
}

/**
 * @brief Sets whether the second child should expand as much as possible when the paned is resized
 * @param paned a paned
 * @param expand ETK_TRUE to make the second child expand, ETK_FALSE otherwise
 */
void etk_paned_child2_expand_set(Etk_Paned *paned, Etk_Bool expand)
{
   if (!paned)
      return;
   paned->expand2 = expand;
}

/**
 * @brief Gets whether the first child expands
 * @param paned a paned
 * @return Returns ETK_TRUE if the first child expands, ETK_FALSE otherwise
 */
Etk_Bool etk_paned_child1_expand_get(Etk_Paned *paned)
{
   if (!paned)
      return ETK_FALSE;
   return paned->expand1;
}

/**
 * @brief Gets whether the second child expands
 * @param paned a paned
 * @return Returns ETK_TRUE if the second child expands, ETK_FALSE otherwise
 */
Etk_Bool etk_paned_child2_expand_get(Etk_Paned *paned)
{
   if (!paned)
      return ETK_FALSE;
   return paned->expand2;
}

/**
 * @brief Sets the position in pixels of the separator of the paned
 * @param paned a paned
 * @param position the new position in pixels of the separator.
 * This is the position from the left border for a hpaned and from the top border for a vpaned
 */
void etk_paned_position_set(Etk_Paned *paned, int position)
{
   int prev_position;

   if (!paned)
      return;

   prev_position = paned->position;
   paned->position = position;
   if (ETK_IS_HPANED(paned))
      _etk_hpaned_position_calc(paned);
   else
      _etk_vpaned_position_calc(paned);

   etk_widget_redraw_queue(ETK_WIDGET(paned));
   if (prev_position != paned->position)
      etk_object_notify(ETK_OBJECT(paned), "position");
}

/**
 * @brief Gets the position in pixels of the separator of the paned
 * @param paned a paned
 * @return Returns the position in pixels of the separator.
 * This is the position from the left border for a hpaned and from the top border for a vpaned
 */
int etk_paned_position_get(Etk_Paned *paned)
{
   if (!paned)
      return 0;
   return paned->position;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the paned */
static void _etk_paned_constructor(Etk_Paned *paned)
{
   if (!paned)
      return;

   paned->drag = ETK_FALSE;
   paned->position = 0;

   paned->child1 = NULL;
   paned->child2 = NULL;
   ETK_CONTAINER(paned)->child_add = _etk_paned_child_add;
   ETK_CONTAINER(paned)->child_remove = _etk_paned_child_remove;
   ETK_CONTAINER(paned)->children_get = _etk_paned_children_get;

   paned->separator = etk_widget_new(ETK_WIDGET_TYPE, "theme-group", "separator", "theme-parent", paned, NULL);
   etk_widget_parent_set(paned->separator, ETK_WIDGET(paned));
   etk_widget_internal_set(paned->separator, ETK_TRUE);

   Etk_Signal_Connect_Desc desc[] = {
      ETK_SC_DESC(ETK_WIDGET_MOUSE_IN_SIGNAL,
                  _etk_paned_separator_mouse_in_cb),
      ETK_SC_DESC(ETK_WIDGET_MOUSE_OUT_SIGNAL,
                  _etk_paned_separator_mouse_out_cb),
      ETK_SC_DESC(ETK_WIDGET_MOUSE_MOVE_SIGNAL,
                  _etk_paned_separator_mouse_move_cb),
      ETK_SC_DESC(ETK_WIDGET_MOUSE_DOWN_SIGNAL,
                  _etk_paned_separator_mouse_down_cb),
      ETK_SC_DESC(ETK_WIDGET_MOUSE_UP_SIGNAL,
                  _etk_paned_separator_mouse_up_cb),
      ETK_SC_DESC_SENTINEL
   };

   etk_signal_connect_multiple(desc, ETK_OBJECT(paned->separator), paned);
}

/* Initializes the hpaned */
static void _etk_hpaned_constructor(Etk_HPaned *hpaned)
{
   Etk_Paned *paned;

   if (!(paned = ETK_PANED(hpaned)))
      return;

   ETK_WIDGET(hpaned)->size_request = _etk_hpaned_size_request;
   ETK_WIDGET(hpaned)->size_allocate = _etk_hpaned_size_allocate;
}

/* Initializes the vpaned */
static void _etk_vpaned_constructor(Etk_VPaned *vpaned)
{
   Etk_Paned *paned;

   if (!(paned = ETK_PANED(vpaned)))
      return;

   ETK_WIDGET(vpaned)->size_request = _etk_vpaned_size_request;
   ETK_WIDGET(vpaned)->size_allocate = _etk_vpaned_size_allocate;
}

/* Calculates the ideal size of the hpaned */
static void _etk_hpaned_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Paned *paned;
   Etk_Size child1_size, child2_size, separator_size;

   if (!(paned = ETK_PANED(widget)) || !size)
      return;

   if (paned->child1)
      etk_widget_size_request(paned->child1, &child1_size);
   else
   {
      child1_size.w = 0;
      child1_size.h = 0;
   }

   if (paned->child2)
      etk_widget_size_request(paned->child2, &child2_size);
   else
   {
      child2_size.w = 0;
      child2_size.h = 0;
   }

   etk_widget_size_request(paned->separator, &separator_size);
   size->w = child1_size.w + child2_size.w + separator_size.w;
   size->h = ETK_MAX(child1_size.h, ETK_MAX(child2_size.h, separator_size.h));

   /* We force the paned to redraw */
   /* TODO: maybe a smarter way to fix the redraw bug */
   etk_widget_redraw_queue(widget);
}

/* Calculates the ideal size of the vpaned */
static void _etk_vpaned_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Paned *paned;
   Etk_Size child1_size, child2_size, separator_size;

   if (!(paned = ETK_PANED(widget)) || !size)
      return;

   if (paned->child1)
      etk_widget_size_request(paned->child1, &child1_size);
   else
   {
      child1_size.w = 0;
      child1_size.h = 0;
   }

   if (paned->child2)
      etk_widget_size_request(paned->child2, &child2_size);
   else
   {
      child2_size.w = 0;
      child2_size.h = 0;
   }

   etk_widget_size_request(paned->separator, &separator_size);
   size->w = ETK_MAX(child1_size.w, ETK_MAX(child2_size.w, separator_size.w));
   size->h = child1_size.h + child2_size.h + separator_size.h;

   /* We force the paned to redraw */
   /* TODO: maybe a smarter way to fix the redraw bug */
   etk_widget_redraw_queue(widget);
}

/* Resizes the hpaned to the allocated size */
static void _etk_hpaned_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Paned *paned;

   if (!(paned = ETK_PANED(widget)))
      return;

   if (!paned->child1 || !paned->child2)
      etk_widget_hide(paned->separator);
   else
      etk_widget_show(paned->separator);

   if (!paned->child2 && paned->child1)
      etk_widget_size_allocate(paned->child1, geometry);
   else if (!paned->child1 && paned->child2)
      etk_widget_size_allocate(paned->child2, geometry);
   else if (paned->child1 && paned->child2)
   {
      int prev_size;
      Etk_Size separator_size;
      Etk_Geometry child_geometry;

      if (paned->child1->geometry.w == -1
	  && paned->child2->geometry.w == -1
	  && paned->separator->geometry.w == -1
	  && paned->position != 0)
	return;

      prev_size = paned->child1->geometry.w + paned->child2->geometry.w + paned->separator->geometry.w;
      if (prev_size <= 0 && paned->position != 0)
	 etk_paned_position_set(paned, paned->position);
      else if (paned->expand1 == paned->expand2)
	 etk_paned_position_set(paned, paned->position + (geometry.w - prev_size) / 2);
      else if (paned->expand1 && !paned->expand2)
	 etk_paned_position_set(paned, geometry.w - (prev_size - paned->position));
      else
	 etk_paned_position_set(paned, paned->position);

      child_geometry.x = geometry.x;
      child_geometry.y = geometry.y;
      child_geometry.w = paned->position;
      child_geometry.h = geometry.h;
      etk_widget_size_allocate(paned->child1, child_geometry);

      etk_widget_size_request(paned->separator, &separator_size);
      child_geometry.x += paned->position;
      child_geometry.w = separator_size.w;
      etk_widget_size_allocate(paned->separator, child_geometry);

      child_geometry.x += separator_size.w;
      child_geometry.w = geometry.w - (paned->position + separator_size.w);
      etk_widget_size_allocate(paned->child2, child_geometry);
   }
}

/* Resizes the vpaned to the allocated size */
static void _etk_vpaned_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Paned *paned;

   if (!(paned = ETK_PANED(widget)))
      return;

   _etk_vpaned_position_calc(paned);

   if (!paned->child1 || !paned->child2)
      etk_widget_hide(paned->separator);
   else
      etk_widget_show(paned->separator);

   if (!paned->child2 && paned->child1)
      etk_widget_size_allocate(paned->child1, geometry);
   else if (!paned->child1 && paned->child2)
      etk_widget_size_allocate(paned->child2, geometry);
   else if (paned->child1 && paned->child2)
   {
      int prev_size;
      Etk_Size separator_size;
      Etk_Geometry child_geometry;

      if (paned->child1->geometry.h == -1
	  && paned->child2->geometry.h == -1
	  && paned->separator->geometry.h == -1
	  && paned->position != 0)
	return;

      prev_size = paned->child1->geometry.h + paned->child2->geometry.h + paned->separator->geometry.h;
      if (prev_size <= 0 && paned->position != 0)
	 etk_paned_position_set(paned, paned->position);
      else if (paned->expand1 == paned->expand2)
	 etk_paned_position_set(paned, paned->position + (geometry.h - prev_size) / 2);
      else if (paned->expand1 && !paned->expand2)
         etk_paned_position_set(paned, geometry.h - (prev_size - paned->position));
      else
	 etk_paned_position_set(paned, paned->position);

      child_geometry.x = geometry.x;
      child_geometry.y = geometry.y;
      child_geometry.w = geometry.w;
      child_geometry.h = paned->position;
      etk_widget_size_allocate(paned->child1, child_geometry);

      etk_widget_size_request(paned->separator, &separator_size);
      child_geometry.y += paned->position;
      child_geometry.h = separator_size.h;
      etk_widget_size_allocate(paned->separator, child_geometry);

      child_geometry.y += separator_size.h;
      child_geometry.h = geometry.h - (paned->position + separator_size.h);
      etk_widget_size_allocate(paned->child2, child_geometry);
   }
}

/* Adds a child to the paned */
static void _etk_paned_child_add(Etk_Container *container, Etk_Widget *widget)
{
   if (!container || !widget)
      return;
   etk_paned_child1_set(ETK_PANED(container), widget, ETK_FALSE);
}

/* Removes the child from the paned */
static void _etk_paned_child_remove(Etk_Container *container, Etk_Widget *widget)
{
   Etk_Paned *paned;

   if (!(paned = ETK_PANED(container)) || !widget)
      return;

   if (widget == paned->child1)
      paned->child1 = NULL;
   else if (widget == paned->child2)
      paned->child2 = NULL;
   else
      return;

   etk_widget_size_recalc_queue(ETK_WIDGET(paned));
   etk_signal_emit(ETK_CONTAINER_CHILD_REMOVED_SIGNAL, ETK_OBJECT(paned),
                   widget);
}

/* Returns the children of the paned */
static Eina_List *_etk_paned_children_get(Etk_Container *container)
{
   Etk_Paned *paned;
   Eina_List *children;

   if (!(paned = ETK_PANED(container)))
      return NULL;

   children = NULL;
   if (paned->child1)
      children = eina_list_append(children, paned->child1);
   if (paned->child2)
      children = eina_list_append(children, paned->child2);

   return children;
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the mouse enters the paned separator */
static Etk_Bool _etk_paned_separator_mouse_in_cb(Etk_Object *object, Etk_Event_Mouse_In *event, void *data)
{
   Etk_Paned *paned;
   Etk_Widget *separator_widget;

   if (!(separator_widget = ETK_WIDGET(object)) || !(paned = ETK_PANED(data)))
      return ETK_TRUE;

   if (ETK_IS_HPANED(paned))
      etk_toplevel_pointer_push(etk_widget_toplevel_parent_get(separator_widget), ETK_POINTER_H_DOUBLE_ARROW);
   else
      etk_toplevel_pointer_push(etk_widget_toplevel_parent_get(separator_widget), ETK_POINTER_V_DOUBLE_ARROW);

   return ETK_TRUE;
}

/* Called when the mouse leaves the paned separator */
static Etk_Bool _etk_paned_separator_mouse_out_cb(Etk_Object *object, Etk_Event_Mouse_Out *event, void *data)
{
   Etk_Paned *paned;
   Etk_Widget *separator_widget;

   if (!(separator_widget = ETK_WIDGET(object)) || !(paned = ETK_PANED(data)))
      return ETK_TRUE;

   if (ETK_IS_HPANED(paned))
      etk_toplevel_pointer_pop(etk_widget_toplevel_parent_get(separator_widget), ETK_POINTER_H_DOUBLE_ARROW);
   else
      etk_toplevel_pointer_pop(etk_widget_toplevel_parent_get(separator_widget), ETK_POINTER_V_DOUBLE_ARROW);

   return ETK_TRUE;
}

/* Called when the user presses the paned separator */
static Etk_Bool _etk_paned_separator_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data)
{
   Etk_Paned *paned;

   if (!(paned = ETK_PANED(data)))
      return ETK_TRUE;

   paned->drag = ETK_TRUE;
   if (ETK_IS_HPANED(paned))
      paned->drag_delta = event->canvas.x - paned->position;
   else
      paned->drag_delta = event->canvas.y - paned->position;

   return ETK_TRUE;
}

/* Called when the user releases the paned separator */
static Etk_Bool _etk_paned_separator_mouse_up_cb(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data)
{
   Etk_Paned *paned;

   if (!(paned = ETK_PANED(data)))
      return ETK_TRUE;
   paned->drag = ETK_FALSE;
   return ETK_TRUE;
}


/* Called whent the user moves the mouse above the separator of paned */
static Etk_Bool _etk_paned_separator_mouse_move_cb(Etk_Object *object, Etk_Event_Mouse_Move *event, void *data)
{
   Etk_Paned *paned;

   if (!(paned = ETK_PANED(data)) || !paned->drag)
      return ETK_TRUE;

   if (ETK_IS_HPANED(paned))
      paned->position = event->cur.canvas.x - paned->drag_delta;
   else
      paned->position = event->cur.canvas.y - paned->drag_delta;
   etk_widget_redraw_queue(ETK_WIDGET(paned));

   return ETK_TRUE;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Recalculates the best position of the separator of the hpaned */
static void _etk_hpaned_position_calc(Etk_Paned *paned)
{
   Etk_Size child1_size, child2_size, separator_size;
   Etk_Geometry geometry;
   int total_size;

   if (!paned)
      return;

   if (paned->child1 && paned->child1->geometry.w == -1)
      return;

   geometry = ETK_WIDGET(paned)->geometry;
   if (paned->child1)
      etk_widget_size_request(paned->child1, &child1_size);
   else
   {
      child1_size.w = 0;
      child1_size.h = 0;
   }
   if (paned->child2)
      etk_widget_size_request(paned->child2, &child2_size);
   else
   {
      child2_size.w = 0;
      child2_size.h = 0;
   }
   etk_widget_size_request(paned->separator, &separator_size);

   total_size = child1_size.w + child2_size.w + separator_size.w;
   if (geometry.w < total_size)
      paned->position = ((float)child1_size.w / total_size) * geometry.w;
   else
   {
      paned->position = ETK_MAX(paned->position, child1_size.w);
      paned->position = ETK_MIN(paned->position, geometry.w - child2_size.w - separator_size.w);
   }
}

/* Recalculates the best position of the separator of the vpaned */
static void _etk_vpaned_position_calc(Etk_Paned *paned)
{
   Etk_Size child1_size, child2_size, separator_size;
   Etk_Geometry geometry;
   int total_size;

   if (!paned)
      return;

   if (paned->child1 && paned->child1->geometry.h == -1)
      return;

   geometry = ETK_WIDGET(paned)->geometry;
   if (paned->child1)
      etk_widget_size_request(paned->child1, &child1_size);
   else
   {
      child1_size.w = 0;
      child1_size.h = 0;
   }
   if (paned->child2)
      etk_widget_size_request(paned->child2, &child2_size);
   else
   {
      child2_size.w = 0;
      child2_size.h = 0;
   }
   etk_widget_size_request(paned->separator, &separator_size);

   total_size = child1_size.h + child2_size.h + separator_size.h;
   if (geometry.h < total_size)
      paned->position = ((float)child1_size.h / total_size) * geometry.h;
   else
   {
      paned->position = ETK_MAX(paned->position, child1_size.h);
      paned->position = ETK_MIN(paned->position, geometry.h - child2_size.h - separator_size.h);
   }
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Paned
 *
 * @image html widgets/paned.png
 * Etk_Paned is the abstract class for two derived widgets: Etk_HPaned and Etk_VPaned.
 * - Etk_HPaned is a paned where the two children are arranged horizontally and
 * separated by a vertical draggable separator
 * - Etk_VPaned is a paned where the two children are arranged vertically and
 * separated by a horizontal draggable separator
 * @n @n
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Paned
 *         - Etk_HPaned
 *         - Etk_VPaned
 *
 * \par Properties:
 * @prop_name "position": The position of the separator in pixels.
 * This is the position from the left border for a hpaned and from the top border for a vpaned
 * @prop_type Integer
 * @prop_rw
 * @prop_val 0
 */
