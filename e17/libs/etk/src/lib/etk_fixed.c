/** @file etk_fixed.c */
#include "etk_fixed.h"
#include <stdlib.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Fixed
 * @{
 */

typedef struct Etk_Fixed_Child
{
   Etk_Widget *child;
   Etk_Position pos;
} Etk_Fixed_Child;

static void _etk_fixed_constructor(Etk_Fixed *fixed);
static void _etk_fixed_destructor(Etk_Fixed *fixed);
static void _etk_fixed_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_fixed_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_fixed_child_add(Etk_Container *container, Etk_Widget *widget);
static void _etk_fixed_child_remove(Etk_Container *container, Etk_Widget *widget);
static Evas_List *_etk_fixed_children_get(Etk_Container *container);
static void _etk_fixed_realized_cb(Etk_Object *object, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Fixed
 * @return Returns the type of an Etk_Fixed
 */
Etk_Type *etk_fixed_type_get(void)
{
   static Etk_Type *fixed_type = NULL;

   if (!fixed_type)
   {
      fixed_type = etk_type_new("Etk_Fixed", ETK_CONTAINER_TYPE, sizeof(Etk_Fixed),
         ETK_CONSTRUCTOR(_etk_fixed_constructor), ETK_DESTRUCTOR(_etk_fixed_destructor));
   }

   return fixed_type;
}

/**
 * @brief Creates a new fixed container
 * @return Returns the new fixed container
 */
Etk_Widget *etk_fixed_new(void)
{
   return etk_widget_new(ETK_FIXED_TYPE, NULL);
}

/**
 * @brief Puts a new child into the fixed container, at the position (x, y)
 * @param fixed a fixed container
 * @param child the child to add
 * @param x the x position where to put the child
 * @param y the y position where to put the child
 */
void etk_fixed_put(Etk_Fixed *fixed, Etk_Widget *widget, int x, int y)
{
   Etk_Fixed_Child *child;
   
   if (!fixed || !widget)
      return;
   
   child = malloc(sizeof(Etk_Fixed_Child));
   child->child = widget;
   child->pos.x = x;
   child->pos.y = y;
   fixed->children = evas_list_append(fixed->children, child);
   
   if (fixed->clip)
   {
      etk_widget_clip_set(widget, fixed->clip);
      evas_object_show(fixed->clip);
   }
   
   etk_widget_parent_set(widget, ETK_WIDGET(fixed));
   etk_signal_emit_by_name("child-added", ETK_OBJECT(fixed), NULL, widget);
}

/**
 * @brief Moves an existing child of the fixed container to the position (x, y)
 * @param fixed a fixed container
 * @param widget the child to move
 * @param x the x position where to move the child
 * @param y the y position where to move the child
 */
void etk_fixed_move(Etk_Fixed *fixed, Etk_Widget *widget, int x, int y)
{
   Evas_List *l;
   Etk_Fixed_Child *c;
   
   if (!fixed || !widget)
      return;
   
   for (l = fixed->children; l; l = l->next)
   {
      c = l->data;
      if (c->child == widget)
      {
         c->pos.x = x;
         c->pos.y = y;
         etk_widget_size_recalc_queue(ETK_WIDGET(fixed));
         break;
      }
   }
}

/**
 * @brief Gets the position of a child of the fixed container
 * @param fixed a fixed container
 * @param widget the child you want the position of
 * @param x the location where to store the x position of the child (it can be NULL)
 * @param y the location where to store the y position of the child (it can be NULL)
 * @note if the child is not contained by the fixed container, @a x and @a y will be set to (0, 0)
 */
void etk_fixed_child_position_get(Etk_Fixed *fixed, Etk_Widget *widget, int *x, int *y)
{
   Evas_List *l;
   Etk_Fixed_Child *c;
   
   if (x)   *x = 0;
   if (y)   *y = 0;
   
   if (!fixed || !widget)
      return;
   
   for (l = fixed->children; l; l = l->next)
   {
      c = l->data;
      if (c->child == widget)
      {
         if (x)   *x = c->pos.x;
         if (y)   *y = c->pos.y;
         break;
      }
   }
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the fixed container */
static void _etk_fixed_constructor(Etk_Fixed *fixed)
{
   if (!fixed)
      return;

   fixed->children = NULL;

   ETK_CONTAINER(fixed)->child_add = _etk_fixed_child_add;
   ETK_CONTAINER(fixed)->child_remove = _etk_fixed_child_remove;
   ETK_CONTAINER(fixed)->children_get = _etk_fixed_children_get;
   ETK_WIDGET(fixed)->size_request = _etk_fixed_size_request;
   ETK_WIDGET(fixed)->size_allocate = _etk_fixed_size_allocate;
   
   etk_signal_connect("realized", ETK_OBJECT(fixed), ETK_CALLBACK(_etk_fixed_realized_cb), NULL);
   etk_signal_connect_swapped("unrealized", ETK_OBJECT(fixed), ETK_CALLBACK(etk_callback_set_null), &fixed->clip);
}

/* Destroys the fixed container */
static void _etk_fixed_destructor(Etk_Fixed *fixed)
{
   if (!fixed)
      return;
   
   while (fixed->children)
   {
      free(fixed->children->data);
      fixed->children = evas_list_remove_list(fixed->children, fixed->children);
   }
}

/* Calculates the ideal size of the fixed container */
static void _etk_fixed_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Fixed *fixed;
   Etk_Fixed_Child *c;
   Etk_Size child_size;
   Evas_List *l;
   
   if (!(fixed = ETK_FIXED(widget)) || !size_requisition)
      return;

   size_requisition->w = 0;
   size_requisition->h = 0;
   
   for (l = fixed->children; l; l = l->next)
   {
      c = l->data;
      etk_widget_size_request(c->child, &child_size);
      size_requisition->w = ETK_MAX(size_requisition->w, c->pos.x + child_size.w);
      size_requisition->h = ETK_MAX(size_requisition->h, c->pos.y + child_size.h);
   }

   size_requisition->w += 2 * ETK_CONTAINER(fixed)->border_width;
   size_requisition->h += 2 * ETK_CONTAINER(fixed)->border_width;
}

/* Resizes the fixed to the size allocation */
static void _etk_fixed_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Fixed *fixed;
   Etk_Fixed_Child *c;
   Etk_Size child_size;
   Etk_Geometry child_geometry;
   Evas_List *l;
   
   if (!(fixed = ETK_FIXED(widget)))
      return;
   
   geometry.x += ETK_CONTAINER(fixed)->border_width;
   geometry.y += ETK_CONTAINER(fixed)->border_width;
   geometry.w -= 2 * ETK_CONTAINER(fixed)->border_width;
   geometry.h -= 2 * ETK_CONTAINER(fixed)->border_width;
   
   evas_object_move(fixed->clip, geometry.x, geometry.y);
   evas_object_resize(fixed->clip, geometry.w, geometry.h);
   
   for (l = fixed->children; l; l = l->next)
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

/* Adds a child to the fixed container */
static void _etk_fixed_child_add(Etk_Container *container, Etk_Widget *widget)
{
   etk_fixed_put(ETK_FIXED(container), widget, 0, 0);
}

/* Removes the child from the fixed container */
static void _etk_fixed_child_remove(Etk_Container *container, Etk_Widget *widget)
{   
   Etk_Fixed *fixed;
   Etk_Fixed_Child *c;
   Evas_List *l;

   if (!(fixed = ETK_FIXED(container)) || !widget || (widget->parent != ETK_WIDGET(container)))
      return;
   
   for (l = fixed->children; l; l = l->next)
   {
      c = l->data;
      if (c->child == widget)
      {
         free(c);
         fixed->children = evas_list_remove_list(fixed->children, l);
         break;
      }
   }
   
   if (fixed->clip)
   {
      etk_widget_clip_unset(widget);
      if (!fixed->children)
         evas_object_hide(fixed->clip);
   }
   
   etk_widget_parent_set_full(widget, NULL, ETK_FALSE);
   etk_signal_emit_by_name("child-removed", ETK_OBJECT(fixed), NULL, widget);
}

/* Gets the list of the children */
static Evas_List *_etk_fixed_children_get(Etk_Container *container)
{
   Etk_Fixed *fixed;
   Etk_Fixed_Child *c;
   Evas_List *children, *l;
   
   if (!(fixed = ETK_FIXED(container)))
      return NULL;
   
   children = NULL;
   for (l = fixed->children; l; l = l->next)
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

/* Called when the fixed container is realized */
static void _etk_fixed_realized_cb(Etk_Object *object, void *data)
{
   Etk_Fixed *fixed;
   Etk_Fixed_Child *c;
   Evas_List *l;
   Evas *evas;
   
   if (!(fixed = ETK_FIXED(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(object))))
      return;
   
   fixed->clip = evas_object_rectangle_add(evas);
   etk_widget_member_object_add(ETK_WIDGET(fixed), fixed->clip);
   
   for (l = fixed->children; l; l = l->next)
   {
      c = l->data;
      etk_widget_clip_set(c->child, fixed->clip);
   }
   
   if (fixed->children)
      evas_object_show(fixed->clip);
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Fixed
 *
 * Etk_Fixed allows you to easily position widgets at fixed coordinates. The children will have the same size as their
 * requested-size. So to force a child to have a fixed size, you can call etk_widget_size_request_set() on the child. @n
 * Fox example, to put a button at the position (20, 30), with the size 100x30:
 * @code
 * Etk_Widget *fixed;
 * Etk_Widget *child;
 *
 * fixed = etk_fixed_new();
 * child = etk_button_new();
 * etk_fixed_put(ETK_FIXED(fixed), child, 20, 30);
 * etk_widget_size_request_set(button, 100, 30);
 * @endcode @n
 *
 * Etk_Fixed may seem to make widget positioning simpler but you should actually avoid using it as much as possible.
 * Indeed, using fixed positions may make widgets overlap and the result can look different on different configurations
 * (different themes, different fonts, different languages, ...). Also, if you'll ever want to remove a child from the
 * fixed container, you will probably have to reposition all the other children of the fixed container. It's heavily
 * advised to use Etk_Box, Etk_Table or any other kind of containers instead of a fixed container.
 * 
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Fixed
 */
