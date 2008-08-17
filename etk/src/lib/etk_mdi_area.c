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

/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
/** @file etk_mdi_area.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_mdi_area.h"

#include <stdlib.h>

#include "etk_mdi_window.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Mdi_Area
 * @{
 */

typedef struct Etk_Mdi_Area_Child
{
   Etk_Widget *child;
   Etk_Position pos;
} Etk_Mdi_Area_Child;

static void _etk_mdi_area_constructor(Etk_Mdi_Area *mdi_area);
static void _etk_mdi_area_destructor(Etk_Mdi_Area *mdi_area);
static void _etk_mdi_area_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_mdi_area_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_mdi_area_child_add(Etk_Container *container, Etk_Widget *widget);
static void _etk_mdi_area_child_remove(Etk_Container *container, Etk_Widget *widget);
static Evas_List *_etk_mdi_area_children_get(Etk_Container *container);
static Etk_Bool _etk_mdi_area_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_mdi_area_child_moved_cb(Etk_Widget *child, int x, int y, void *data);
static void _etk_mdi_area_child_maximized_cb(Etk_Object *object, const char *property_name, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Mdi_Area
 * @return Returns the type of an Etk_Mdi_Area
 */
Etk_Type *etk_mdi_area_type_get(void)
{
   static Etk_Type *mdi_area_type = NULL;

   if (!mdi_area_type)
   {
      mdi_area_type = etk_type_new("Etk_Mdi_Area", ETK_CONTAINER_TYPE,
         sizeof(Etk_Mdi_Area),
         ETK_CONSTRUCTOR(_etk_mdi_area_constructor),
         ETK_DESTRUCTOR(_etk_mdi_area_destructor), NULL);
   }

   return mdi_area_type;
}

/**
 * @brief Creates a new mdi_area container
 * @return Returns the new mdi_area container
 */
Etk_Widget *etk_mdi_area_new(void)
{
   return etk_widget_new(ETK_MDI_AREA_TYPE, "theme-group", "mdi_area", NULL);
}

/**
 * @brief Puts a new child into the mdi_area container, at the position (x, y)
 * @param mdi_area a mdi_area container
 * @param child the child to add
 * @param x the x position where to put the child
 * @param y the y position where to put the child
 */
void etk_mdi_area_put(Etk_Mdi_Area *mdi_area, Etk_Widget *widget, int x, int y)
{
   Etk_Mdi_Area_Child *c;

   if (!mdi_area || !widget)
      return;

   c = malloc(sizeof(Etk_Mdi_Area_Child));
   c->child = widget;
   c->pos.x = x;
   c->pos.y = y;
   mdi_area->children = evas_list_append(mdi_area->children, c);
   etk_object_data_set(ETK_OBJECT(widget), "_Etk_Mdi_Area::Node", evas_list_last(mdi_area->children));

   if (mdi_area->clip)
   {
      etk_widget_clip_set(widget, mdi_area->clip);
      evas_object_show(mdi_area->clip);
   }

   if (ETK_IS_MDI_WINDOW(widget))
   {
      etk_signal_connect_by_code(ETK_MDI_WINDOW_MOVED_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_mdi_area_child_moved_cb), mdi_area);
      etk_object_notification_callback_add(ETK_OBJECT(widget), "maximized", _etk_mdi_area_child_maximized_cb, mdi_area);
   }

   etk_widget_parent_set(widget, ETK_WIDGET(mdi_area));
   etk_signal_emit(ETK_CONTAINER_CHILD_ADDED_SIGNAL, ETK_OBJECT(mdi_area), widget);
}

/**
 * @brief Moves an existing child of the mdi_area container to the position (x, y)
 * @param mdi_area a mdi_area container
 * @param widget the child to move
 * @param x the x position where to move the child
 * @param y the y position where to move the child
 */
void etk_mdi_area_move(Etk_Mdi_Area *mdi_area, Etk_Widget *widget, int x, int y)
{
   Evas_List *l;
   Etk_Mdi_Area_Child *c;

   if (!mdi_area || !widget)
      return;

   for (l = mdi_area->children; l; l = l->next)
   {
      c = l->data;
      if (c->child == widget)
      {
         c->pos.x = x;
         c->pos.y = y;
         etk_widget_size_recalc_queue(ETK_WIDGET(mdi_area));
         break;
      }
   }
}

/**
 * @brief Gets the position of a child of the mdi_area container
 * @param mdi_area a mdi_area container
 * @param widget the child you want the position of
 * @param x the location where to store the x position of the child (it can be NULL)
 * @param y the location where to store the y position of the child (it can be NULL)
 * @note if the child is not contained by the mdi_area container, @a x and @a y will be set to (0, 0)
 */
void etk_mdi_area_child_position_get(Etk_Mdi_Area *mdi_area, Etk_Widget *widget, int *x, int *y)
{
   Evas_List *l;
   Etk_Mdi_Area_Child *c;

   if (x) *x = 0;
   if (y) *y = 0;

   if (!mdi_area || !widget)
      return;

   if ((l = etk_object_data_get(ETK_OBJECT(widget), "_Etk_Mdi_Area::Node")))
   {
      c = l->data;
      if (x) *x = c->pos.x;
      if (y) *y = c->pos.y;
   }
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the mdi_area container */
static void _etk_mdi_area_constructor(Etk_Mdi_Area *mdi_area)
{
   if (!mdi_area)
      return;

   mdi_area->children = NULL;
   mdi_area->clip = NULL;

   ETK_WIDGET(mdi_area)->size_request = _etk_mdi_area_size_request;
   ETK_WIDGET(mdi_area)->size_allocate = _etk_mdi_area_size_allocate;
   ETK_CONTAINER(mdi_area)->child_add = _etk_mdi_area_child_add;
   ETK_CONTAINER(mdi_area)->child_remove = _etk_mdi_area_child_remove;
   ETK_CONTAINER(mdi_area)->children_get = _etk_mdi_area_children_get;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(mdi_area), ETK_CALLBACK(_etk_mdi_area_realized_cb), NULL);
   etk_signal_connect_swapped_by_code(ETK_WIDGET_UNREALIZED_SIGNAL, ETK_OBJECT(mdi_area), ETK_CALLBACK(etk_callback_set_null), &mdi_area->clip);
}

/* Destroys the mdi_area container */
static void _etk_mdi_area_destructor(Etk_Mdi_Area *mdi_area)
{
   if (!mdi_area)
      return;

   while (mdi_area->children)
   {
      free(mdi_area->children->data);
      mdi_area->children = evas_list_remove_list(mdi_area->children, mdi_area->children);
   }
}

/* Calculates the ideal size of the mdi_area container */
static void _etk_mdi_area_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Mdi_Area *mdi_area;
   Etk_Mdi_Area_Child *c;
   Etk_Size child_size;
   Evas_List *l;

   if (!(mdi_area = ETK_MDI_AREA(widget)) || !size_requisition)
      return;

   size_requisition->w = 0;
   size_requisition->h = 0;

   for (l = mdi_area->children; l; l = l->next)
   {
      c = l->data;
      etk_widget_size_request(c->child, &child_size);
      size_requisition->w = ETK_MAX(size_requisition->w, c->pos.x + child_size.w);
      size_requisition->h = ETK_MAX(size_requisition->h, c->pos.y + child_size.h);
   }

   size_requisition->w += 2 * ETK_CONTAINER(mdi_area)->border_width;
   size_requisition->h += 2 * ETK_CONTAINER(mdi_area)->border_width;
}

/* Resizes the mdi_area to the size allocation */
static void _etk_mdi_area_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Mdi_Area *mdi_area;
   Etk_Mdi_Area_Child *c;
   Etk_Size child_size;
   Etk_Geometry child_geometry;
   Evas_List *l;

   if (!(mdi_area = ETK_MDI_AREA(widget)))
      return;

   geometry.x += ETK_CONTAINER(mdi_area)->border_width;
   geometry.y += ETK_CONTAINER(mdi_area)->border_width;
   geometry.w -= 2 * ETK_CONTAINER(mdi_area)->border_width;
   geometry.h -= 2 * ETK_CONTAINER(mdi_area)->border_width;

   evas_object_move(mdi_area->clip, geometry.x, geometry.y);
   evas_object_resize(mdi_area->clip, geometry.w, geometry.h);

   for (l = mdi_area->children; l; l = l->next)
   {
      c = l->data;
      etk_widget_size_request(c->child, &child_size);
      if (ETK_IS_MDI_WINDOW(c->child) && ETK_MDI_WINDOW(c->child)->maximized)
      {
         child_geometry.x = geometry.x;
         child_geometry.y = geometry.y;
         child_geometry.w = geometry.w;
         child_geometry.h = geometry.h;
      }
      else
      {
         child_geometry.x = geometry.x + c->pos.x;
         child_geometry.y = geometry.y + c->pos.y;
         child_geometry.w = child_size.w;
         child_geometry.h = child_size.h;
      }
      etk_widget_size_allocate(c->child, child_geometry);
   }
}

/* Adds a child to the mdi_area container */
static void _etk_mdi_area_child_add(Etk_Container *container, Etk_Widget *widget)
{
   etk_mdi_area_put(ETK_MDI_AREA(container), widget, 0, 0);
}

/* Removes the child from the mdi_area container */
static void _etk_mdi_area_child_remove(Etk_Container *container, Etk_Widget *widget)
{
   Etk_Mdi_Area *mdi_area;
   Evas_List *l;

   if (!(mdi_area = ETK_MDI_AREA(container)) || !widget)
      return;

   if ((l = etk_object_data_get(ETK_OBJECT(widget), "_Etk_Mdi_Area::Node")))
   {
      free(l->data);
      etk_object_data_set(ETK_OBJECT(widget), "_Etk_Mdi_Area::Node", NULL);
      mdi_area->children = evas_list_remove_list(mdi_area->children, l);

      if (mdi_area->clip)
      {
         etk_widget_clip_unset(widget);
         if (!mdi_area->children)
            evas_object_hide(mdi_area->clip);
      }

      if (ETK_IS_MDI_WINDOW(widget))
      {
         etk_signal_disconnect_by_code(ETK_MDI_WINDOW_MOVED_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_mdi_area_child_moved_cb), mdi_area);
         etk_object_notification_callback_remove(ETK_OBJECT(widget), "maximized", _etk_mdi_area_child_maximized_cb);
      }

      etk_signal_emit(ETK_CONTAINER_CHILD_REMOVED_SIGNAL, ETK_OBJECT(mdi_area), widget);
   }
}

/* Gets the list of the children */
static Evas_List *_etk_mdi_area_children_get(Etk_Container *container)
{
   Etk_Mdi_Area *mdi_area;
   Etk_Mdi_Area_Child *c;
   Evas_List *children, *l;

   if (!(mdi_area = ETK_MDI_AREA(container)))
      return NULL;

   children = NULL;
   for (l = mdi_area->children; l; l = l->next)
   {
      c = l->data;
      children = evas_list_append(children, c->child);
   }
   return children;
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the mdi_area container is realized */
static Etk_Bool _etk_mdi_area_realized_cb(Etk_Object *object, void *data)
{
   Etk_Mdi_Area *mdi_area;
   Etk_Mdi_Area_Child *c;
   Evas_List *l;
   Evas *evas;

   if (!(mdi_area = ETK_MDI_AREA(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(object))))
      return ETK_TRUE;

   mdi_area->clip = evas_object_rectangle_add(evas);
   etk_widget_member_object_add(ETK_WIDGET(mdi_area), mdi_area->clip);

   for (l = mdi_area->children; l; l = l->next)
   {
      c = l->data;
      etk_widget_clip_set(c->child, mdi_area->clip);
   }

   if (mdi_area->children)
      evas_object_show(mdi_area->clip);

   return ETK_TRUE;
}

static Etk_Bool _etk_mdi_area_child_moved_cb(Etk_Widget *child, int x, int y, void *data)
{
   etk_mdi_area_move(ETK_MDI_AREA(data), child, x, y);
   return ETK_TRUE;
}

static void _etk_mdi_area_child_maximized_cb(Etk_Object *object, const char *property_name, void *data)
{
   etk_widget_size_recalc_queue(ETK_WIDGET(data));
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Mdi_Area
 *
 * Etk_Mdi_Area allows you to easily position mdi_windows and/or other widgets at fixed coordinates. The children will have the same size as their
 * requested-size (unless a mdi_window is maximized). So to force a child to have a fixed size, you can call etk_widget_size_request_set() on the child. @n
 * Fox example, to put a mdi_window at the position (20, 30), with the size 100x30:
 * @code
 * Etk_Widget *mdi_area;
 * Etk_Widget *mdi_window;
 *
 * mdi_area = etk_mdi_area_new();
 * mdi_window = etk_mdi_window_new();
 * etk_mdi_area_put(ETK_MDI_AREA(mdi_area), mdi_window, 20, 30);
 * etk_widget_size_request_set(mdi_window, 100, 30);
 * @endcode @n
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Mdi_Area
 */
