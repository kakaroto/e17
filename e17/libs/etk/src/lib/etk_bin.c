/** @file etk_bin.c */
#include "etk_bin.h"
#include <stdlib.h>
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
static Evas_List *_etk_bin_children_get(Etk_Container *container);
static void _etk_bin_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_bin_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_bin_realize_cb(Etk_Object *object, void *data);
static void _etk_bin_child_realize_cb(Etk_Object *object, void *data);

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
      bin_type = etk_type_new("Etk_Bin", ETK_CONTAINER_TYPE, sizeof(Etk_Bin), ETK_CONSTRUCTOR(_etk_bin_constructor), ETK_DESTRUCTOR(_etk_bin_destructor));

   return bin_type;
}

/**
 * @brief Gets the child of the bin
 * @param bin a bin
 * @return Returns the child of the bin or NULL if it doesn't have a child
 */
Etk_Widget *etk_bin_child_get(Etk_Bin *bin)
{
   if (!bin)
      return NULL;
   return bin->child;
}

/**
 * @brief Sets the child of the bin
 * @param bin a bin
 * @param child the child to set
 */
void etk_bin_child_set(Etk_Bin *bin, Etk_Widget *child)
{
   if (!bin || bin->child == child)
      return;

   _etk_bin_child_remove(ETK_CONTAINER(bin), bin->child);

   if (child)
   {
      if (child->parent && ETK_IS_CONTAINER(child->parent))
         etk_container_remove(ETK_CONTAINER(child->parent), child);

      etk_signal_connect("realize", ETK_OBJECT(child), ETK_CALLBACK(_etk_bin_child_realize_cb), bin);
      etk_widget_parent_set(child, ETK_WIDGET(bin));
      bin->child = child;
      bin->child_list = evas_list_append(bin->child_list, child);
      
      etk_signal_emit_by_name("child_added", ETK_OBJECT(bin), NULL, child);
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

   bin->child = NULL;
   bin->child_list = NULL;
   ETK_CONTAINER(bin)->child_add = _etk_bin_child_add;
   ETK_CONTAINER(bin)->child_remove = _etk_bin_child_remove;
   ETK_CONTAINER(bin)->children_get = _etk_bin_children_get;
   ETK_WIDGET(bin)->size_request = _etk_bin_size_request;
   ETK_WIDGET(bin)->size_allocate = _etk_bin_size_allocate;
   
   etk_signal_connect("realize", ETK_OBJECT(bin), ETK_CALLBACK(_etk_bin_realize_cb), NULL);
}

/* Destroys the bin */
static void _etk_bin_destructor(Etk_Bin *bin)
{
   if (!bin)
      return;
   bin->child_list = evas_list_free(bin->child_list);
}

/* Calculates the ideal size of the bin */
static void _etk_bin_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Bin *bin;
   Etk_Container *container;

   if (!(bin = ETK_BIN(widget)) || !size_requisition)
      return;
   container = ETK_CONTAINER(bin);

   if (!bin->child || etk_widget_is_swallowed(bin->child))
   {
      size_requisition->w = 0;
      size_requisition->h = 0;
   }
   else
      etk_widget_size_request(bin->child, size_requisition);

   size_requisition->w += 2 * etk_container_border_width_get(container);
   size_requisition->h += 2 * etk_container_border_width_get(container);
}

/* Resizes the bin to the size allocation */
static void _etk_bin_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Bin *bin;
   Etk_Container *container;
   int border;

   if (!(bin = ETK_BIN(widget)))
      return;
   container = ETK_CONTAINER(widget);
   
   if (bin->child && !etk_widget_is_swallowed(bin->child))
   {
      border = etk_container_border_width_get(container);
      geometry.x += border;
      geometry.y += border;
      geometry.w -= 2 * border;
      geometry.h -= 2 * border;
      etk_widget_size_allocate(bin->child, geometry);
   }
}

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
   Etk_Bin *bin;

   if (!(bin = ETK_BIN(container)) || !widget || bin->child != widget)
      return;

   etk_widget_parent_set(widget, NULL);
   etk_signal_disconnect("realize", ETK_OBJECT(bin->child), ETK_CALLBACK(_etk_bin_child_realize_cb));
   
   bin->child = NULL;
   bin->child_list = evas_list_free(bin->child_list);
   
   etk_signal_emit_by_name("child_removed", ETK_OBJECT(bin), NULL, widget);
}

/* Gets the children (the child actually) of the bin */
static Evas_List *_etk_bin_children_get(Etk_Container *container)
{
   Etk_Bin *bin;
   
   if (!(bin = ETK_BIN(container)))
      return NULL;
   return bin->child_list;
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the bin is realized */
static void _etk_bin_realize_cb(Etk_Object *object, void *data)
{
   Etk_Bin *bin;

   if (!(bin = ETK_BIN(object)) || !bin->child)
      return;

   if (ETK_WIDGET(bin)->realized && bin->child->realized)
      etk_widget_swallow_widget(ETK_WIDGET(bin), "swallow_area", bin->child);
}

/* Called when the child of the bin is realized */
static void _etk_bin_child_realize_cb(Etk_Object *object, void *data)
{
   Etk_Bin *bin;

   if (!(bin = ETK_BIN(data)) || !bin->child || ETK_OBJECT(bin->child) != object)
      return;

   if (ETK_WIDGET(bin)->realized && bin->child->realized)
      etk_widget_swallow_widget(ETK_WIDGET(bin), "swallow_area", bin->child);
}

/** @} */
