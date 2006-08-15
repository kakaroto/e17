/** @file etk_paned.c */
#include "etk_paned.h"
#include <stdlib.h>
#include <Evas.h>
#include "etk_separator.h"
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_toplevel_widget.h"

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
static Evas_List *_etk_paned_children_get(Etk_Container *container);

static void _etk_paned_separator_mouse_in_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data);
static void _etk_paned_separator_mouse_out_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data);
static void _etk_paned_separator_mouse_up_cb(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data);
static void _etk_paned_separator_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data);
static void _etk_paned_separator_mouse_move_cb(Etk_Object *object, Etk_Event_Mouse_Move *event, void *data);

static void _etk_hpaned_position_calc(Etk_Paned *paned);
static void _etk_vpaned_position_calc(Etk_Paned *paned);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Paned
 * @return Returns the type of an Etk_Paned
 */
Etk_Type *etk_paned_type_get()
{
   static Etk_Type *paned_type = NULL;

   if (!paned_type)
   {
      paned_type = etk_type_new("Etk_Paned", ETK_CONTAINER_TYPE, sizeof(Etk_Paned),
         ETK_CONSTRUCTOR(_etk_paned_constructor), NULL);
      
      etk_type_property_add(paned_type, "position", ETK_PANED_POSITION_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(0));
   }

   return paned_type;
}

/**
 * @brief Gets the type of an Etk_HPaned
 * @return Returns the type of an Etk_HPaned
 */
Etk_Type *etk_hpaned_type_get()
{
   static Etk_Type *hpaned_type = NULL;

   if (!hpaned_type)
   {
      hpaned_type = etk_type_new("Etk_HPaned", ETK_PANED_TYPE, sizeof(Etk_HPaned),
         ETK_CONSTRUCTOR(_etk_hpaned_constructor), NULL);
   }
   
   return hpaned_type;
}

/**
 * @brief Gets the type of an Etk_VPaned
 * @return Returns the type of an Etk_VPaned
 */
Etk_Type *etk_vpaned_type_get()
{
   static Etk_Type *vpaned_type = NULL;

   if (!vpaned_type)
   {
      vpaned_type = etk_type_new("Etk_VPaned", ETK_PANED_TYPE, sizeof(Etk_VPaned),
         ETK_CONSTRUCTOR(_etk_vpaned_constructor), NULL);
   }
   
   return vpaned_type;
}

/**
 * @brief Creates a new horizontal paned widget
 * @return Returns the horizontal paned widget
 */
Etk_Widget *etk_hpaned_new()
{
   return etk_widget_new(ETK_HPANED_TYPE, "theme_group", "hpaned", NULL);
}

/**
 * @brief Creates a new vertical paned widget
 * @return Returns the vertical paned widget
 */
Etk_Widget *etk_vpaned_new()
{
   return etk_widget_new(ETK_VPANED_TYPE, "theme_group", "vpaned", NULL);
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
      _etk_paned_child_remove(ETK_CONTAINER(paned), paned->child1);
   
   paned->child1 = child;
   paned->expand1 = expand;
   if (child)
   {
      if (child->parent && ETK_IS_CONTAINER(child->parent))
         etk_container_remove(ETK_CONTAINER(child->parent), child);
      etk_widget_parent_set(child, ETK_WIDGET(paned));
      
      etk_widget_raise(paned->separator);
      
      etk_signal_emit_by_name("child_added", ETK_OBJECT(paned), NULL, child);
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
      _etk_paned_child_remove(ETK_CONTAINER(paned), paned->child2);
   
   paned->child2 = child;
   paned->expand2 = expand;
   if (child)
   {
      if (child->parent && ETK_IS_CONTAINER(child))
         etk_container_remove(ETK_CONTAINER(child->parent), child);
      etk_widget_parent_set(child, ETK_WIDGET(paned));
      
      etk_widget_raise(paned->separator);
      
      etk_signal_emit_by_name("child_added", ETK_OBJECT(paned), NULL, child);
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

   paned->separator = etk_widget_new(ETK_WIDGET_TYPE, "theme_group", "separator", NULL);
   etk_widget_parent_set(paned->separator, ETK_WIDGET(paned));
   etk_widget_visibility_locked_set(paned->separator, ETK_TRUE);

   etk_signal_connect("mouse_in", ETK_OBJECT(paned->separator), ETK_CALLBACK(_etk_paned_separator_mouse_in_cb), paned);
   etk_signal_connect("mouse_out", ETK_OBJECT(paned->separator), ETK_CALLBACK(_etk_paned_separator_mouse_out_cb), paned);
   etk_signal_connect("mouse_move", ETK_OBJECT(paned->separator), ETK_CALLBACK(_etk_paned_separator_mouse_move_cb), paned);
   etk_signal_connect("mouse_down", ETK_OBJECT(paned->separator), ETK_CALLBACK(_etk_paned_separator_mouse_down_cb), paned);
   etk_signal_connect("mouse_up", ETK_OBJECT(paned->separator), ETK_CALLBACK(_etk_paned_separator_mouse_up_cb), paned);
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
      
      prev_size = paned->child1->geometry.w + paned->child2->geometry.w + paned->separator->geometry.w;
      if (paned->expand1 == paned->expand2)
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
      
      prev_size = paned->child1->geometry.h + paned->child2->geometry.h + paned->separator->geometry.h;
      if (paned->expand1 == paned->expand2)
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
   if (widget != paned->child1 && widget != paned->child2)
      return;

   etk_widget_parent_set_full(widget, NULL, ETK_FALSE);
   etk_widget_size_recalc_queue(ETK_WIDGET(paned));
   etk_signal_emit_by_name("child_removed", ETK_OBJECT(paned), NULL, widget);
}

/* Returns the children of the paned */
static Evas_List *_etk_paned_children_get(Etk_Container *container)
{
   Etk_Paned *paned;
   Evas_List *children;
   
   if (!(paned = ETK_PANED(container)))
      return NULL;
   
   children = NULL;
   if (paned->child1)
      children = evas_list_append(children, paned->child1);
   if (paned->child2)
      children = evas_list_append(children, paned->child2);
   
   return children;
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the mouse enters the paned separator */
static void _etk_paned_separator_mouse_in_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data)
{
   Etk_Paned *paned;
   Etk_Widget *separator_widget;

   if (!(separator_widget = ETK_WIDGET(object)) || !(paned = ETK_PANED(data)))
      return;

   if (ETK_IS_HPANED(paned))
      etk_toplevel_widget_pointer_push(etk_widget_toplevel_parent_get(separator_widget), ETK_POINTER_H_DOUBLE_ARROW);
   else
      etk_toplevel_widget_pointer_push(etk_widget_toplevel_parent_get(separator_widget), ETK_POINTER_V_DOUBLE_ARROW);
}

/* Called when the mouse leaves the paned separator */
static void _etk_paned_separator_mouse_out_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data)
{
   Etk_Paned *paned;
   Etk_Widget *separator_widget;

   if (!(separator_widget = ETK_WIDGET(object)) || !(paned = ETK_PANED(data)))
      return;

   if (ETK_IS_HPANED(paned))
      etk_toplevel_widget_pointer_pop(etk_widget_toplevel_parent_get(separator_widget), ETK_POINTER_H_DOUBLE_ARROW);
   else
      etk_toplevel_widget_pointer_pop(etk_widget_toplevel_parent_get(separator_widget), ETK_POINTER_V_DOUBLE_ARROW);
}

/* Called when the user presses the paned separator */
static void _etk_paned_separator_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data)
{
   Etk_Paned *paned;

   if (!(paned = ETK_PANED(data)))
      return;

   paned->drag = ETK_TRUE;
   if (ETK_IS_HPANED(paned))
      paned->drag_delta = event->canvas.x - paned->position;
   else
      paned->drag_delta = event->canvas.y - paned->position;
}

/* Called when the user releases the paned separator */
static void _etk_paned_separator_mouse_up_cb(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data)
{
   Etk_Paned *paned;

   if (!(paned = ETK_PANED(data)))
      return;
   paned->drag = ETK_FALSE;
}


/* Called whent the user moves the mouse above the separator of paned */ 
static void _etk_paned_separator_mouse_move_cb(Etk_Object *object, Etk_Event_Mouse_Move *event, void *data)
{
   Etk_Paned *paned;

   if (!(paned = ETK_PANED(data)) || !paned->drag)
      return;

   if (ETK_IS_HPANED(paned))
      paned->position = event->cur.canvas.x - paned->drag_delta;
   else
      paned->position = event->cur.canvas.y - paned->drag_delta;
   etk_widget_redraw_queue(ETK_WIDGET(paned));
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
