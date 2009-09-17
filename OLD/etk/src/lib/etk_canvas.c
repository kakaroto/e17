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

/** @file etk_canvas.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_canvas.h"

#include <stdlib.h>

#include "etk_evas_object.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Canvas
 * @{
 */

typedef struct Etk_Canvas_Child
{
   Etk_Widget *child;
   Etk_Position pos;
} Etk_Canvas_Child;

static void _etk_canvas_constructor(Etk_Canvas *canvas);
static void _etk_canvas_destructor(Etk_Canvas *canvas);
static void _etk_canvas_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_canvas_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_canvas_child_add(Etk_Container *container, Etk_Widget *widget);
static void _etk_canvas_child_remove(Etk_Container *container, Etk_Widget *widget);
static Eina_List *_etk_canvas_children_get(Etk_Container *container);
static Etk_Bool _etk_canvas_realized_cb(Etk_Object *object, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Canvas
 * @return Returns the type of an Etk_Canvas
 */
Etk_Type *etk_canvas_type_get(void)
{
   static Etk_Type *canvas_type = NULL;

   if (!canvas_type)
   {
      canvas_type = etk_type_new("Etk_Canvas", ETK_CONTAINER_TYPE,
         sizeof(Etk_Canvas),
         ETK_CONSTRUCTOR(_etk_canvas_constructor),
         ETK_DESTRUCTOR(_etk_canvas_destructor), NULL);
   }

   return canvas_type;
}

/**
 * @brief Creates a new canvas container
 * @return Returns the new canvas container
 */
Etk_Widget *etk_canvas_new(void)
{
   return etk_widget_new(ETK_CANVAS_TYPE, NULL);
}

/**
 * @brief Puts a new child into the canvas container, at the position (x, y)
 * @param canvas a canvas container
 * @param child the child to add
 * @param x the x position where to put the child
 * @param y the y position where to put the child
 */
void etk_canvas_put(Etk_Canvas *canvas, Etk_Widget *widget, int x, int y)
{
   Etk_Canvas_Child *child;

   if (!canvas || !widget)
      return;

   child = malloc(sizeof(Etk_Canvas_Child));
   child->child = widget;
   child->pos.x = x;
   child->pos.y = y;
   canvas->children = eina_list_append(canvas->children, child);
   etk_object_data_set(ETK_OBJECT(widget), "_Etk_Canvas::Node", eina_list_last(canvas->children));

   if (canvas->clip)
   {
      etk_widget_clip_set(widget, canvas->clip);
      evas_object_show(canvas->clip);
   }

   etk_widget_parent_set(widget, ETK_WIDGET(canvas));
   etk_signal_emit(ETK_CONTAINER_CHILD_ADDED_SIGNAL, ETK_OBJECT(canvas), widget);
}

/**
 * @brief Moves an existing child of the canvas container to the position (x, y)
 * @param canvas a canvas container
 * @param widget the child to move
 * @param x the x position where to move the child
 * @param y the y position where to move the child
 */
void etk_canvas_move(Etk_Canvas *canvas, Etk_Widget *widget, int x, int y)
{
   Eina_List *l;
   Etk_Canvas_Child *c;

   if (!canvas || !widget)
      return;

   for (l = canvas->children; l; l = l->next)
   {
      c = l->data;
      if (c->child == widget)
      {
         c->pos.x = x;
         c->pos.y = y;
         etk_widget_size_recalc_queue(ETK_WIDGET(canvas));
         break;
      }
   }
}

/**
 * @brief Gets the position of a child of the canvas container
 * @param canvas a canvas container
 * @param widget the child you want the position of
 * @param x the location where to store the x position of the child (it can be NULL)
 * @param y the location where to store the y position of the child (it can be NULL)
 * @note if the child is not contained by the canvas container, @a x and @a y will be set to (0, 0)
 */
void etk_canvas_child_position_get(Etk_Canvas *canvas, Etk_Widget *widget, int *x, int *y)
{
   Eina_List *l;
   Etk_Canvas_Child *c;

   if (x)   *x = 0;
   if (y)   *y = 0;

   if (!canvas || !widget)
      return;

   if ((l = etk_object_data_get(ETK_OBJECT(widget), "_Etk_Canvas::Node")))
   {
      c = l->data;
      if (x)   *x = c->pos.x;
      if (y)   *y = c->pos.y;
   }
}

/**
 * @brief Adds an Evas_Object to the canvas. This is a convenience function
 * that uses Etk_Evas_Object.
 * @param canvas the canvas container
 * @parem evas_object the evas object to add
 * @return returns the Etk_Evas_Object holding the @a evas_object
 * @note You will need to show the widget after it has been added.
 */
Etk_Widget *etk_canvas_object_add(Etk_Canvas *canvas, Evas_Object *evas_object)
{
   Etk_Widget *etk_evas_object;

   if (!canvas || !evas_object)
	    return NULL;

   etk_evas_object = etk_evas_object_new_from_object(evas_object);	 
	 etk_canvas_put(canvas, etk_evas_object, 0, 0);
	 return etk_evas_object;
}

/**
 * @brief Removed an Evas_Object from the canvas.
 * @param canvas the canvas container
 * @param evas_object the Evas_Object to remove
 */
void etk_canvas_object_remove(Etk_Canvas *canvas, Evas_Object *evas_object)
{
   Etk_Widget *etk_evas_object = NULL;

   if (!canvas || !evas_object)
	    return;

   if ((etk_evas_object = evas_object_data_get(evas_object, "_Etk_Evas_Object::Widget")) == NULL)
	    return;

   evas_object_data_set(evas_object, "_Etk_Evas_Object::Widget", NULL);
   _etk_canvas_child_remove(ETK_CONTAINER(canvas), etk_evas_object);
}

/**
 * @brief Moves an Evas_Object around in the canvas
 * @param canvas the canvas container
 * @param evas_object the Evas_Object to move
 * @param x the x coordinate to move to inside the canvas
 * @param y the y coordinate to move to inside the canvas
 */
void etk_canvas_object_move(Etk_Canvas *canvas, Evas_Object *evas_object, int x, int y)
{
   Etk_Widget *etk_evas_object = NULL;

   if (!canvas || !evas_object)
	    return;

   if ((etk_evas_object = evas_object_data_get(evas_object, "_Etk_Evas_Object::Widget")) == NULL)
	    return;

   etk_canvas_move(canvas, etk_evas_object, x, y);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the canvas container */
static void _etk_canvas_constructor(Etk_Canvas *canvas)
{
   if (!canvas)
      return;

   canvas->children = NULL;
   canvas->clip = NULL;

   ETK_CONTAINER(canvas)->child_add = _etk_canvas_child_add;
   ETK_CONTAINER(canvas)->child_remove = _etk_canvas_child_remove;
   ETK_CONTAINER(canvas)->children_get = _etk_canvas_children_get;
   ETK_WIDGET(canvas)->size_request = _etk_canvas_size_request;
   ETK_WIDGET(canvas)->size_allocate = _etk_canvas_size_allocate;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(canvas), ETK_CALLBACK(_etk_canvas_realized_cb), NULL);
   etk_signal_connect_swapped_by_code(ETK_WIDGET_UNREALIZED_SIGNAL, ETK_OBJECT(canvas), ETK_CALLBACK(etk_callback_set_null), &canvas->clip);
}

/* Destroys the canvas container */
static void _etk_canvas_destructor(Etk_Canvas *canvas)
{
   if (!canvas)
      return;

   while (canvas->children)
   {
      free(canvas->children->data);
      canvas->children = eina_list_remove_list(canvas->children, canvas->children);
   }
}

/* Calculates the ideal size of the canvas container */
static void _etk_canvas_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Canvas *canvas;
   Etk_Canvas_Child *c;
   Etk_Size child_size;
   Eina_List *l;

   if (!(canvas = ETK_CANVAS(widget)) || !size_requisition)
      return;

   size_requisition->w = 0;
   size_requisition->h = 0;

   for (l = canvas->children; l; l = l->next)
   {
      c = l->data;
      etk_widget_size_request(c->child, &child_size);
      size_requisition->w = ETK_MAX(size_requisition->w, c->pos.x + child_size.w);
      size_requisition->h = ETK_MAX(size_requisition->h, c->pos.y + child_size.h);
   }

   size_requisition->w += 2 * ETK_CONTAINER(canvas)->border_width;
   size_requisition->h += 2 * ETK_CONTAINER(canvas)->border_width;
}

/* Resizes the canvas to the size allocation */
static void _etk_canvas_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Canvas *canvas;
   Etk_Canvas_Child *c;
   Etk_Size child_size;
   Etk_Geometry child_geometry;
   Eina_List *l;

   if (!(canvas = ETK_CANVAS(widget)))
      return;

   geometry.x += ETK_CONTAINER(canvas)->border_width;
   geometry.y += ETK_CONTAINER(canvas)->border_width;
   geometry.w -= 2 * ETK_CONTAINER(canvas)->border_width;
   geometry.h -= 2 * ETK_CONTAINER(canvas)->border_width;

   evas_object_move(canvas->clip, geometry.x, geometry.y);
   evas_object_resize(canvas->clip, geometry.w, geometry.h);

   for (l = canvas->children; l; l = l->next)
   {
      c = l->data;
      etk_widget_size_request(c->child, &child_size);
      child_geometry.x = geometry.x + c->pos.x;
      child_geometry.y = geometry.y + c->pos.y;
      child_geometry.w = child_size.w;
      child_geometry.h = child_size.h;
      etk_widget_size_allocate(c->child, child_geometry);
   }
}

/* Adds a child to the canvas container */
static void _etk_canvas_child_add(Etk_Container *container, Etk_Widget *widget)
{
   etk_canvas_put(ETK_CANVAS(container), widget, 0, 0);
}

/* Removes the child from the canvas container */
static void _etk_canvas_child_remove(Etk_Container *container, Etk_Widget *widget)
{
   Etk_Canvas *canvas;
   Eina_List *l;

   if (!(canvas = ETK_CANVAS(container)) || !widget)
      return;

   if ((l = etk_object_data_get(ETK_OBJECT(widget), "_Etk_Canvas::Node")))
   {
      free(l->data);
      etk_object_data_set(ETK_OBJECT(widget), "_Etk_Canvas::Node", NULL);
      canvas->children = eina_list_remove_list(canvas->children, l);

      if (canvas->clip)
      {
         etk_widget_clip_unset(widget);
         if (!canvas->children)
            evas_object_hide(canvas->clip);
      }

      etk_signal_emit(ETK_CONTAINER_CHILD_REMOVED_SIGNAL, ETK_OBJECT(canvas), widget);
   }
}

/* Gets the list of the children */
static Eina_List *_etk_canvas_children_get(Etk_Container *container)
{
   Etk_Canvas *canvas;
   Etk_Canvas_Child *c;
   Eina_List *children, *l;

   if (!(canvas = ETK_CANVAS(container)))
      return NULL;

   children = NULL;
   for (l = canvas->children; l; l = l->next)
   {
      c = l->data;
      children = eina_list_append(children, c->child);
   }
   return children;
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the canvas container is realized */
static Etk_Bool _etk_canvas_realized_cb(Etk_Object *object, void *data)
{
   Etk_Canvas *canvas;
   Etk_Canvas_Child *c;
   Eina_List *l;
   Evas *evas;

   if (!(canvas = ETK_CANVAS(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(object))))
      return ETK_TRUE;

   canvas->clip = evas_object_rectangle_add(evas);
   etk_widget_member_object_add(ETK_WIDGET(canvas), canvas->clip);

   for (l = canvas->children; l; l = l->next)
   {
      c = l->data;
      etk_widget_clip_set(c->child, canvas->clip);
   }

   if (canvas->children)
      evas_object_show(canvas->clip);

   return ETK_TRUE;
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
 * Etk_Canvas allows you to easily position widgets at canvas coordinates. The children will have the same size as their
 * requested-size. So to force a child to have a canvas size, you can call etk_widget_size_request_set() on the child. @n
 * Fox example, to put a button at the position (20, 30), with the size 100x30:
 * @code
 * Etk_Widget *canvas;
 * Etk_Widget *child;
 *
 * canvas = etk_canvas_new();
 * child = etk_button_new();
 * etk_canvas_put(ETK_CANVAS(canvas), child, 20, 30);
 * etk_widget_size_request_set(button, 100, 30);
 * @endcode @n
 *
 * Etk_Canvas may seem to make widget positioning simpler but you should actually avoid using it as much as possible.
 * Indeed, using canvas positions may make widgets overlap and the result can look differently on different configurations
 * (different themes, different fonts, different languages, ...). Also, if you'll ever want to remove a child from the
 * canvas container, you will probably have to reposition all the other children of the canvas container. It's heavily
 * advised to use Etk_Box, Etk_Table or any other kind of containers instead of a canvas container.
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Canvas
 */
