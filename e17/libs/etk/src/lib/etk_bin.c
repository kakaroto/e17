/** @file etk_bin.c */
#include "etk_bin.h"
#include <stdlib.h>
#include <Ecore_Data.h>
/* TODO */
#include <Edje.h>
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Bin
* @{
 */

static void _etk_bin_constructor(Etk_Bin *bin);
static void _etk_bin_destructor(Etk_Bin *bin);
static void _etk_bin_child_add(Etk_Container *container, Etk_Widget *widget);
static void _etk_bin_child_remove(Etk_Container *container, Etk_Widget *widget);
static void _etk_bin_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_bin_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_bin_realize_cb(Etk_Object *object, void *data);
static void _etk_bin_unrealize_cb(Etk_Object *object, void *data);
static void _etk_bin_child_set(Etk_Bin *bin, Etk_Widget *widget);
static void _etk_bin_child_set_normal(Etk_Bin *bin, Etk_Widget *child);
static Etk_Bool _etk_bin_child_set_swallow(Etk_Bin *bin, Etk_Widget *child);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Bin
 * @return Returns the type on an Etk_Bin
 */
Etk_Type *etk_bin_type_get()
{
   static Etk_Type *bin_type = NULL;

   if (!bin_type)
   {
      bin_type = etk_type_new("Etk_Bin", ETK_CONTAINER_TYPE, sizeof(Etk_Bin), ETK_CONSTRUCTOR(_etk_bin_constructor), ETK_DESTRUCTOR(_etk_bin_destructor), NULL);
   }

   return bin_type;
}

/**
 * @brief Gets the child of the bin
 * @param bin a bin
 * @return Returns the child of the bin or NULL if it doesn't have a child
 * @note It doesn't increase the reference count of the returned widget, so you don't need to unref it.
 */
Etk_Widget *etk_bin_child_get(Etk_Bin *bin)
{
   Etk_Container *container;

   if (!bin)
      return NULL;

   container = ETK_CONTAINER(bin);
   ecore_dlist_goto_first(container->children);
   return ETK_WIDGET(ecore_dlist_next(container->children));
}

/**
 * @brief Sets the child of the bin, and swallows it if the part called "swallow_area" exists
 * @param bin a bin
 * @param child the child to set
 */
void etk_bin_child_set(Etk_Bin *bin, Etk_Widget *child)
{
   if (!bin)
      return;

   if (!ETK_WIDGET(bin)->realized || !ETK_WIDGET(child)->realized)
   {
      etk_signal_connect_after("realize", ETK_OBJECT(child), ETK_CALLBACK(_etk_bin_realize_cb), bin);
      bin->swallow_on_realize = TRUE;
      _etk_bin_child_set_normal(bin, child);
      return;
   }
   else
   {
      if (!_etk_bin_child_set_swallow(bin, child))
         _etk_bin_child_set_normal(bin, child);
   }
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the bin */
static void _etk_bin_constructor(Etk_Bin *bin)
{
   if (!bin)
      return;

   bin->swallow_child = FALSE;
   bin->swallow_on_realize = FALSE;
   ETK_CONTAINER(bin)->child_add = _etk_bin_child_add;
   ETK_CONTAINER(bin)->child_remove = _etk_bin_child_remove;
   ETK_WIDGET(bin)->size_request = _etk_bin_size_request;
   ETK_WIDGET(bin)->size_allocate = _etk_bin_size_allocate;

   etk_signal_connect_after("realize", ETK_OBJECT(bin), ETK_CALLBACK(_etk_bin_realize_cb), bin);
   etk_signal_connect("unrealize", ETK_OBJECT(bin), ETK_CALLBACK(_etk_bin_unrealize_cb), NULL);
}

/* Destroys the bin */
static void _etk_bin_destructor(Etk_Bin *bin)
{
   if (!bin)
      return;

   /* TODO: Unswallow ? */
}

/* Calculates the ideal size of the bin */
static void _etk_bin_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Bin *bin;
   Etk_Container *container;
   Etk_Widget *child;

   if (!(bin = ETK_BIN(widget)) || !size_requisition)
      return;

   container = ETK_CONTAINER(bin);

   if (!(child = etk_bin_child_get(bin)))
   {
      size_requisition->w = 0;
      size_requisition->h = 0;
   }
   else
      etk_widget_size_request(child, size_requisition);

   size_requisition->w += 2 * etk_container_border_width_get(container);
   size_requisition->h += 2 * etk_container_border_width_get(container);
}

/* Resizes the bin to the size allocation */
static void _etk_bin_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Bin *bin;
   Etk_Widget *child;
   Etk_Container *container;

   if (!(bin = ETK_BIN(widget)))
      return;

   container = ETK_CONTAINER(widget);

   if ((child = etk_bin_child_get(bin)))
   {
      geometry.x += etk_container_border_width_get(container);
      geometry.y += etk_container_border_width_get(container);
      geometry.w -= 2 * etk_container_border_width_get(container);
      geometry.h -= 2 * etk_container_border_width_get(container);
      etk_widget_size_allocate(child, geometry);
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Adds a child to the bin */
static void _etk_bin_child_add(Etk_Container *container, Etk_Widget *widget)
{
   if (!container || !widget)
      return;

   etk_bin_child_set(ETK_BIN(container), widget);
}

/* Removes the child from the bin */
static void _etk_bin_child_remove(Etk_Container *container, Etk_Widget *widget)
{
   Etk_Widget *child;
   Etk_Bin *bin;
   Etk_Widget *bin_widget;

   if (!(bin = ETK_BIN(container)) || !widget)
      return;

   bin_widget = ETK_WIDGET(bin);
   child = etk_bin_child_get(bin);

   if (child != widget)
      return;

   etk_widget_parent_set(widget, NULL);
   ecore_dlist_clear(container->children);
   if (bin->swallow_child)
      etk_widget_theme_object_signal_emit(bin_widget, "unswallow_child");
   bin->swallow_child = FALSE;
   etk_widget_resize_queue(bin_widget);
}

/* Called when the bin or the child is realized */
static void _etk_bin_realize_cb(Etk_Object *object, void *data)
{
   Etk_Bin *bin;
   Etk_Widget *child;

   if (!(bin = ETK_BIN(data)) || !(child = etk_bin_child_get(bin)))
      return;

   if (bin->swallow_on_realize && ETK_WIDGET(bin)->realized && child->realized)
   {
      _etk_bin_child_set_swallow(bin, child);
      bin->swallow_on_realize = FALSE;
   }
}

/* Called when the bin is unrealized */
static void _etk_bin_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Bin *bin;

   if (!(bin = ETK_BIN(object)))
      return;

   if (bin->swallow_child)
      bin->swallow_on_realize = TRUE;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Sets the child of the bin */
static void _etk_bin_child_set(Etk_Bin *bin, Etk_Widget *widget)
{
   Etk_Container *container;

   if (!(container = ETK_CONTAINER(bin)) || (etk_bin_child_get(bin) == widget))
      return;

   _etk_bin_child_remove(container, etk_bin_child_get(bin));

   if (widget)
   {
      if (widget->parent)
         etk_container_remove(widget->parent, widget);
      etk_widget_parent_set(widget, container);
      ecore_dlist_append(container->children, widget);
   }
}

/* Sets the child of the bin (without swallowing it) */
static void _etk_bin_child_set_normal(Etk_Bin *bin, Etk_Widget *child)
{
   Etk_Widget *bin_widget;

   if (!(bin_widget = ETK_WIDGET(bin)))
      return;

   if (bin->swallow_child)
      etk_widget_theme_object_signal_emit(bin_widget, "unswallow_child");
   _etk_bin_child_set(bin, child);
   bin->swallow_child = FALSE;
   etk_widget_resize_queue(bin_widget);
}

/* Sets the child and swallows it into the part called "swallow_area". Returns TRUE on success, FALSE on failure */
static Etk_Bool _etk_bin_child_set_swallow(Etk_Bin *bin, Etk_Widget *child)
{
   Etk_Widget *bin_widget;

   if (!(bin_widget = ETK_WIDGET(bin)) || !child)
      return FALSE;

   if (!etk_widget_swallow_widget(bin_widget, "swallow_area", child))
      return FALSE;

   _etk_bin_child_set(bin, child);
   if (!bin->swallow_child)
      etk_widget_theme_object_signal_emit(bin_widget, "swallow_child");
   bin->swallow_child = TRUE;
   etk_widget_resize_queue(bin_widget);

   return TRUE;
}

/** @} */
