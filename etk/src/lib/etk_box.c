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

/** @file etk_box.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_box.h"

#include <stdlib.h>

#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Box
 * @{
 */

enum Etk_Box_Property_Id
{
   ETK_BOX_SPACING_PROPERTY,
   ETK_BOX_HOMOGENEOUS_PROPERTY
};

static void _etk_box_constructor(Etk_Box *box);
static void _etk_box_destructor(Etk_Box *box);
static void _etk_box_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_box_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_box_child_add(Etk_Container *container, Etk_Widget *widget);
static void _etk_box_child_remove(Etk_Container *container, Etk_Widget *widget);
static Eina_List *_etk_box_children_get(Etk_Container *container);

static void _etk_hbox_constructor(Etk_HBox *hbox);
static void _etk_hbox_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_hbox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

static void _etk_vbox_constructor(Etk_VBox *vbox);
static void _etk_vbox_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_vbox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

static void _etk_box_insert_after_cell(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, Etk_Box_Cell *after, Etk_Box_Fill_Policy fill_policy, int padding);
static Etk_Box_Cell *_etk_box_cell_get(Etk_Widget *widget);
static Etk_Box_Cell *_etk_box_cell_nth_get(Etk_Box *box, Etk_Box_Group group, int n);


/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Box
 * @return Returns the type of an Etk_Box
 */
Etk_Type *etk_box_type_get(void)
{
   static Etk_Type *box_type = NULL;

   if (!box_type)
   {
      box_type = etk_type_new("Etk_Box", ETK_CONTAINER_TYPE, sizeof(Etk_Box),
         ETK_CONSTRUCTOR(_etk_box_constructor),
         ETK_DESTRUCTOR(_etk_box_destructor), NULL);

      etk_type_property_add(box_type, "spacing", ETK_BOX_SPACING_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(0));
      etk_type_property_add(box_type, "homogeneous", ETK_BOX_HOMOGENEOUS_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));

      box_type->property_set = _etk_box_property_set;
      box_type->property_get = _etk_box_property_get;
   }

   return box_type;
}

/**
 * @internal
 * @brief Gets the type of an Etk_HBox
 * @return Returns the type of an Etk_HBox
 */
Etk_Type *etk_hbox_type_get(void)
{
   static Etk_Type *hbox_type = NULL;

   if (!hbox_type)
   {
      hbox_type = etk_type_new("Etk_HBox", ETK_BOX_TYPE, sizeof(Etk_HBox),
         ETK_CONSTRUCTOR(_etk_hbox_constructor), NULL, NULL);
   }

   return hbox_type;
}

/**
 * @internal
 * @brief Gets the type of an Etk_VBox
 * @return Returns the type of an Etk_VBox
 */
Etk_Type *etk_vbox_type_get(void)
{
   static Etk_Type *vbox_type = NULL;

   if (!vbox_type)
   {
      vbox_type = etk_type_new("Etk_VBox", ETK_BOX_TYPE, sizeof(Etk_VBox),
         ETK_CONSTRUCTOR(_etk_vbox_constructor), NULL, NULL);
   }

   return vbox_type;
}

/**
 * @brief Creates a new hbox
 * @param homogeneous if @a homogeneous == ETK_TRUE, all the cells of the box will have the same size
 * @param spacing the amount of space between two cells
 */
Etk_Widget *etk_hbox_new(Etk_Bool homogeneous, int spacing)
{
   return etk_widget_new(ETK_HBOX_TYPE, "homogeneous", homogeneous, "spacing", spacing, NULL);
}

/**
 * @brief Creates a new vbox
 * @param homogeneous if @a homogeneous == ETK_TRUE, all the cells of the box will have the same size
 * @param spacing the amount of space between two cells
 */
Etk_Widget *etk_vbox_new(Etk_Bool homogeneous, int spacing)
{
   return etk_widget_new(ETK_VBOX_TYPE, "homogeneous", homogeneous, "spacing", spacing, NULL);
}

/**
 * @brief Packs a widget at the start of one of the two child-groups of the box
 * @param box a box
 * @param child the widget to pack
 * @param group the group where to pack the child
 * @param fill_policy the fill-policy of the child, it indicates how it should fill its cell
 * @param padding the amount of free space on the two sides of the child, in pixels
 */
void etk_box_prepend(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, Etk_Box_Fill_Policy fill_policy, int padding)
{
   if (!box || !child)
      return;
   _etk_box_insert_after_cell(box, child, group, NULL, fill_policy, padding);
}

/**
 * @brief Packs a widget at the end of one of the two child-groups of the box
 * @param box a box
 * @param child the widget to pack
 * @param group the group where to pack the child
 * @param fill_policy the fill-policy of the child, it indicates how it should fill its cell
 * @param padding the amount of free space on the two sides of the child, in pixels
 */
void etk_box_append(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, Etk_Box_Fill_Policy fill_policy, int padding)
{
   if (!box || !child)
      return;
   _etk_box_insert_after_cell(box, child, group, box->last_cell[group], fill_policy, padding);
}

/**
 * @brief Packs a widget after another widget, in one of the two child-groups of the box
 * @param box a box
 * @param child the widget to pack
 * @param group the group where to pack the child
 * @param after the child after which @a child will be packed. If @a after is NULL, @a child will be
 * packed at the start of the group
 * @param fill_policy the fill-policy of the child, it indicates how it should fill its cell
 * @param padding the amount of free space on the two sides of the child, in pixels
 * @warning @a after has to be packed in @a group, or the function will have no effect
 */
void etk_box_insert(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, Etk_Widget *after, Etk_Box_Fill_Policy fill_policy, int padding)
{
   Etk_Box_Cell *after_cell;

   if (!box || !child)
      return;

   after_cell = _etk_box_cell_get(after);
   _etk_box_insert_after_cell(box, child, group, after_cell, fill_policy, padding);
}

/**
 * @brief Packs a widget at a given position, in one of the two child-groups of the box
 * @param box a box
 * @param child the widget to pack
 * @param group the group where to pack the child
 * @param pos the position where to pack @a child (starting from 0). If @a pos <= 0, @a child will be packed at the
 * start of the child-group, and if @a pos is greater than the number of children in the group, it will be packed
 * at the end
 * @param fill_policy the fill-policy of the child, it indicates how it should fill its cell
 * @param padding the amount of free space on the two sides of the child, in pixels
 */
void etk_box_insert_at(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, int pos, Etk_Box_Fill_Policy fill_policy, int padding)
{
   Etk_Box_Cell *after_cell;

   if (!box || !child)
      return;

   if (pos <= 0)
      after_cell = NULL;
   else if (pos >= box->cells_count[group])
      after_cell = box->last_cell[group];
   else
      after_cell = _etk_box_cell_nth_get(box, group, pos - 1);

   _etk_box_insert_after_cell(box, child, group, after_cell, fill_policy, padding);
}

/**
 * @brief Gets the child at a given position in the box
 * @param box a box
 * @param group the group in which the child you want to get is packed
 * @param pos the position of the child you want to get (starting from 0)
 * @return Returns the child at the given position, or NULL if there no childat this position
 */
Etk_Widget *etk_box_child_get_at(Etk_Box *box, Etk_Box_Group group, int pos)
{
   Etk_Box_Cell *cell;

   if (!box)
      return NULL;

   if (!(cell = _etk_box_cell_nth_get(box, group, pos)))
      return NULL;
   else
      return cell->child;
}

/**
 * @brief Changes the position of a child of the box
 * @param box a box
 * @param child the child you want to change the position of
 * @param group the group in which the child will be repacked
 * @param pos the position where to repack @a child (starting from 0). If @a pos <= 0, @a child will be packed at the
 * start of the child-group, and if @a pos is greater than the number of children in the group, it will be packed
 * at the end
 */
void etk_box_child_position_set(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, int pos)
{
   Etk_Box_Cell *cell;
   Etk_Box_Fill_Policy fill_policy;
   int padding;

   if (!box || !child || !(cell = _etk_box_cell_get(child)))
      return;

   padding = cell->padding;
   fill_policy = cell->fill_policy;
   etk_widget_parent_set(child, NULL);
   etk_box_insert_at(box, child, group, pos, fill_policy, padding);
}

/**
 * @brief Gets the position of a child of the box
 * @param box a box
 * @param child the child you want to get the position of
 * @param group the location where to store the group in which the child is
 * @param pos the location where to store the position of the child
 * @return Returns ETK_TRUE on success, or ETK_FALSE if @a child is not a child of the box
 */
Etk_Bool etk_box_child_position_get(Etk_Box *box, Etk_Widget *child, Etk_Box_Group *group, int *pos)
{
   Etk_Box_Cell *cell, *c;
   int i;

   if (!box || !child)
      return ETK_FALSE;

   if (!(cell = _etk_box_cell_get(child)))
      return ETK_FALSE;

   for (i = 0, c = box->first_cell[cell->group]; c; i++, c = c->next)
   {
      if (cell == c)
      {
         if (group)
            *group = cell->group;
         if (pos)
            *pos = i;
         return ETK_TRUE;
      }
   }
   return ETK_FALSE;
}

/**
 * @brief Changes the packing settings of a child of the box
 * @param box a box
 * @param child a child of the box. If @a child is not packed in the box, this function has no effect
 * @param fill_policy the new fill-policy of the child
 * @param padding the new amount of free space on the two sides of the child, in pixels
 */
void etk_box_child_packing_set(Etk_Box *box, Etk_Widget *child, Etk_Box_Fill_Policy fill_policy, int padding)
{
   Etk_Box_Cell *cell;

   if (!box || !child)
      return;

   if ((cell = _etk_box_cell_get(child)))
   {
      cell->fill_policy = fill_policy;
      cell->padding = padding;
      etk_widget_size_recalc_queue(ETK_WIDGET(box));
   }
}

/**
 * @brief Gets the packing settings of a child of the box
 * @param box a box
 * @param child a child of the box
 * @param fill_policy the location where to store the fill-policy of the child
 * @param padding the location where to store the padding of the child
 * @return Returns ETK_TRUE on success, or ETK_FALSE if @a child is not a child of the box
 */
Etk_Bool etk_box_child_packing_get(Etk_Box *box, Etk_Widget *child, Etk_Box_Fill_Policy *fill_policy, int *padding)
{
   Etk_Box_Cell *cell;

   if (!box || !child)
      return ETK_FALSE;

   if ((cell = _etk_box_cell_get(child)))
   {
      if (fill_policy)
         *fill_policy = cell->fill_policy;
      if (padding)
         *padding = cell->padding;
      return ETK_TRUE;
   }

   return ETK_FALSE;
}

/**
 * @brief Sets the amount of free space between two cells
 * @param box a box
 * @param spacing the new amount of free space between two cells, in pixels
 */
void etk_box_spacing_set(Etk_Box *box, int spacing)
{
   if (!box)
      return;

   box->spacing = spacing;
   etk_widget_size_recalc_queue(ETK_WIDGET(box));
   etk_object_notify(ETK_OBJECT(box), "spacing");
}

/**
 * @brief Gets the amount of free space between two cells
 * @param box a box
 * @return Returns the amount of free space between two cells, in pixels
 */
int etk_box_spacing_get(Etk_Box *box)
{
   if (!box)
      return 0;
   return box->spacing;
}

/**
 * @brief Sets whether or not all the cells of the box should have the same size
 * @param box a box
 * @param homogeneous if @a homogeneous is ETK_TRUE, all the cells will have the same size
 */
void etk_box_homogeneous_set(Etk_Box *box, Etk_Bool homogeneous)
{
   if (!box)
      return;

   box->homogeneous = homogeneous;
   etk_widget_size_recalc_queue(ETK_WIDGET(box));
   etk_object_notify(ETK_OBJECT(box), "homogeneous");
}

/**
 * @brief Gets whether or not all the cells of the box have the same size
 * @param box a box
 * @return Returns ETK_TRUE if the box is homogeneous, ETK_FALSE otherwise
 */
Etk_Bool etk_box_homogeneous_get(Etk_Box *box)
{
   if (!box)
      return ETK_FALSE;
   return box->homogeneous;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/**************************
 * Box
 **************************/

/* Initializes the box */
static void _etk_box_constructor(Etk_Box *box)
{
   if (!box)
      return;

   box->first_cell[ETK_BOX_START] = box->first_cell[ETK_BOX_END] = NULL;
   box->last_cell[ETK_BOX_START] = box->last_cell[ETK_BOX_END] = NULL;
   box->cells_count[ETK_BOX_START] = box->cells_count[ETK_BOX_END] = 0;
   box->request_sizes[ETK_BOX_START] = box->request_sizes[ETK_BOX_END] = NULL;
   box->spacing = 0;
   box->homogeneous = ETK_FALSE;

   ETK_CONTAINER(box)->child_add = _etk_box_child_add;
   ETK_CONTAINER(box)->child_remove = _etk_box_child_remove;
   ETK_CONTAINER(box)->children_get = _etk_box_children_get;
}

/* Destroys the box */
static void _etk_box_destructor(Etk_Box *box)
{
   Etk_Box_Cell *cell, *next;
   int i;

   if (!box)
      return;

   for (i = 0; i < 2; i++)
   {
      for (cell = box->first_cell[i]; cell; cell = next)
      {
         next = cell->next;
         free(cell);
      }
      free(box->request_sizes[i]);
   }
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_box_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Box *box;

   if (!(box = ETK_BOX(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_BOX_SPACING_PROPERTY:
         etk_box_spacing_set(box, etk_property_value_int_get(value));
         break;
      case ETK_BOX_HOMOGENEOUS_PROPERTY:
         etk_box_homogeneous_set(box, etk_property_value_bool_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_box_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Box *box;

   if (!(box = ETK_BOX(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_BOX_SPACING_PROPERTY:
         etk_property_value_int_set(value, box->spacing);
         break;
      case ETK_BOX_HOMOGENEOUS_PROPERTY:
         etk_property_value_bool_set(value, box->homogeneous);
         break;
      default:
         break;
   }
}

/* Adds a child to the box */
static void _etk_box_child_add(Etk_Container *container, Etk_Widget *widget)
{
   etk_box_append(ETK_BOX(container), widget, ETK_BOX_START, ETK_BOX_NONE, 0);
}

/* Removes the child from the box */
static void _etk_box_child_remove(Etk_Container *container, Etk_Widget *widget)
{
   Etk_Box *box;
   Etk_Box_Cell *cell;

   if (!(box = ETK_BOX(container)) || !widget)
      return;

   if ((cell = _etk_box_cell_get(widget)))
   {
      if (cell->prev)
         cell->prev->next = cell->next;
      if (cell->next)
         cell->next->prev = cell->prev;
      if (cell == box->first_cell[cell->group])
         box->first_cell[cell->group] = cell->next;
      if (cell == box->last_cell[cell->group])
         box->last_cell[cell->group] = cell->prev;
      box->cells_count[cell->group]--;

      ETK_WIDGET(box)->focus_order = eina_list_remove_list(ETK_WIDGET(box)->focus_order, cell->focus_node);
      etk_object_data_set(ETK_OBJECT(widget), "_Etk_Box::Cell", NULL);
      free(cell);

      etk_widget_size_recalc_queue(ETK_WIDGET(box));
      etk_signal_emit(ETK_CONTAINER_CHILD_REMOVED_SIGNAL, ETK_OBJECT(box), widget);
   }
}

/* Gets the list of the children of the box */
static Eina_List *_etk_box_children_get(Etk_Container *container)
{
   Etk_Box *box;
   Eina_List *children;
   Etk_Box_Cell *cell;
   int i;

   if (!(box = ETK_BOX(container)))
      return NULL;

   children = NULL;
   for (i = 0; i < 2; i++)
   {
      for (cell = box->first_cell[i]; cell; cell = cell->next)
         children = eina_list_append(children, cell->child);
   }

   return children;
}

/**************************
 * HBox
 **************************/

/* Initializes the hbox */
static void _etk_hbox_constructor(Etk_HBox *hbox)
{
   if (!hbox)
      return;

   ETK_WIDGET(hbox)->size_request = _etk_hbox_size_request;
   ETK_WIDGET(hbox)->size_allocate = _etk_hbox_size_allocate;
}

/* Calculates the ideal size of the hbox */
static void _etk_hbox_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_HBox *hbox;
   Etk_Box *box;
   Etk_Container *container;
   Etk_Box_Cell *cell;
   Etk_Widget *child;
   Etk_Size child_size;
   int num_visible_children;
   int i, j;

   if (!(hbox = ETK_HBOX(widget)) || !size)
      return;

   box = ETK_BOX(hbox);
   container = ETK_CONTAINER(hbox);
   size->w = 0;
   size->h = 0;
   num_visible_children = 0;

   for (i = 0; i < 2; i++)
   {
      box->request_sizes[i] = realloc(box->request_sizes[i], box->cells_count[i] * sizeof(int));
      for (cell = box->first_cell[i], j = 0; cell; cell = cell->next, j++)
      {
         child = cell->child;
         box->request_sizes[i][j] = 0;

         if (!etk_widget_is_visible(child))
            continue;

         etk_widget_size_request(child, &child_size);

         if (box->homogeneous)
         {
            if (size->w < child_size.w + 2 * cell->padding)
               size->w = child_size.w + 2 * cell->padding;
         }
         else
         {
            box->request_sizes[i][j] = child_size.w + 2 * cell->padding;
            size->w += box->request_sizes[i][j];
         }

         if (size->h < child_size.h)
            size->h = child_size.h;

         num_visible_children++;
      }
   }
   if (box->homogeneous)
   {
      for (i = 0; i < 2; i++)
      {
         for (j = 0; j < box->cells_count[i]; j++)
            box->request_sizes[i][j] = size->w;
      }
      size->w *= num_visible_children;
   }
   if (num_visible_children > 1)
      size->w += (num_visible_children - 1) * box->spacing;

   size->w += 2 * container->border_width;
   size->h += 2 * container->border_width;
}

/* Resizes the hbox to the allocated size */
static void _etk_hbox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_HBox *hbox;
   Etk_Box *box;
   Etk_Container *container;
   Etk_Widget *child;
   Etk_Box_Cell *cell;
   Etk_Size requested_size;
   Etk_Size allocated_size;
   Etk_Geometry child_geometry;
   int num_children_to_expand = 0;
   int i, j;
   float start_offset, end_offset;
   float width;

   if (!(hbox = ETK_HBOX(widget)))
      return;

   box = ETK_BOX(hbox);
   container = ETK_CONTAINER(hbox);

   _etk_hbox_size_request(widget, &requested_size);
   requested_size.w -= 2 * container->border_width;
   requested_size.h -= 2 * container->border_width;
   allocated_size.w = geometry.w - 2 * container->border_width;
   allocated_size.h = geometry.h - 2 * container->border_width;

   start_offset = container->border_width;
   end_offset = container->border_width;

   if (allocated_size.w <= requested_size.w)
   {
      float ratio;

      ratio = (float)allocated_size.w / requested_size.w;
      for (i = 0; i < 2; i++)
      {
         j = (i == ETK_BOX_START) ? 0 : box->cells_count[i] - 1;
         cell = (i == ETK_BOX_START) ? box->first_cell[i] : box->last_cell[i];
         while (cell)
         {
            child = cell->child;

            if (etk_widget_is_visible(child))
            {
               width = box->request_sizes[i][j] * ratio;
               child_geometry.y = geometry.y + container->border_width;
               child_geometry.w = width;
               child_geometry.h = allocated_size.h;

               if (cell->group == ETK_BOX_START)
               {
                  child_geometry.x = geometry.x + start_offset;
                  start_offset += width + box->spacing;
               }
               else
               {
                  child_geometry.x = geometry.x + geometry.w - end_offset - child_geometry.w;
                  end_offset += width + box->spacing;
               }

               child_geometry.x += cell->padding;
               child_geometry.w -= 2 * cell->padding;

               etk_container_child_space_fill(child, &child_geometry,
                     cell->fill_policy & ETK_BOX_FILL,
                     !(cell->fill_policy & ETK_BOX_SHRINK_OPPOSITE), 0.5, 0.5);
               etk_widget_size_allocate(child, child_geometry);
            }

            cell = (i == ETK_BOX_START) ? cell->next : cell->prev;
            j = (i == ETK_BOX_START) ? (j + 1) : (j - 1);
         }
      }
   }
   else
   {
      float free_space;

      for (i = 0; i < 2; i++)
      {
         for (cell = box->first_cell[i]; cell; cell = cell->next)
         {
            child = cell->child;

            if (!etk_widget_is_visible(child))
               continue;

            if (cell->fill_policy & ETK_BOX_EXPAND)
               num_children_to_expand++;
         }
      }

      if (num_children_to_expand <= 0)
         free_space = 0;
      else
         free_space = (float)(allocated_size.w - requested_size.w) / num_children_to_expand;

      for (i = 0; i < 2; i++)
      {
         j = (i == ETK_BOX_START) ? 0 : box->cells_count[i] - 1;
         cell = (i == ETK_BOX_START) ? box->first_cell[i] : box->last_cell[i];
         while (cell)
         {
            child = cell->child;

            if (etk_widget_is_visible(child))
            {
               width = box->request_sizes[i][j];
               if (cell->fill_policy & ETK_BOX_EXPAND)
                  width += free_space;
               child_geometry.y = geometry.y + container->border_width;
               child_geometry.w = width;
               child_geometry.h = allocated_size.h;

               if (cell->group == ETK_BOX_START)
               {
                  child_geometry.x = geometry.x + start_offset;
                  start_offset += width + box->spacing;
               }
               else
               {
                  child_geometry.x = geometry.x + geometry.w - end_offset - child_geometry.w;
                  end_offset += width + box->spacing;
               }

               child_geometry.x += cell->padding;
               child_geometry.w -= 2 * cell->padding;

               etk_container_child_space_fill(child, &child_geometry,
                     cell->fill_policy & ETK_BOX_FILL,
                     !(cell->fill_policy & ETK_BOX_SHRINK_OPPOSITE), 0.5, 0.5);
               etk_widget_size_allocate(child, child_geometry);
            }

            cell = (i == ETK_BOX_START) ? cell->next : cell->prev;
            j = (i == ETK_BOX_START) ? (j + 1) : (j - 1);
         }
      }
   }
}

/**************************
 * VBox
 **************************/

/* Initializes the vbox */
static void _etk_vbox_constructor(Etk_VBox *vbox)
{
   if (!vbox)
      return;

   ETK_WIDGET(vbox)->size_request = _etk_vbox_size_request;
   ETK_WIDGET(vbox)->size_allocate = _etk_vbox_size_allocate;
}

/* Calculates the ideal size of the vbox */
static void _etk_vbox_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_VBox *vbox;
   Etk_Box *box;
   Etk_Container *container;
   Etk_Box_Cell *cell;
   Etk_Widget *child;
   Etk_Size child_size;
   int num_visible_children;
   int i, j;

   if (!(vbox = ETK_VBOX(widget)) || !size)
      return;

   box = ETK_BOX(vbox);
   container = ETK_CONTAINER(vbox);
   size->w = 0;
   size->h = 0;
   num_visible_children = 0;

   for (i = 0; i < 2; i++)
   {
      box->request_sizes[i] = realloc(box->request_sizes[i], box->cells_count[i] * sizeof(int));
      for (cell = box->first_cell[i], j = 0; cell; cell = cell->next, j++)
      {
         child = cell->child;
         box->request_sizes[i][j] = 0;

         if (!etk_widget_is_visible(child))
            continue;

         etk_widget_size_request(child, &child_size);

         if (box->homogeneous)
         {
            if (size->h < child_size.h + 2 * cell->padding)
               size->h = child_size.h + 2 * cell->padding;
         }
         else
         {
            box->request_sizes[i][j] = child_size.h + 2 * cell->padding;
            size->h += box->request_sizes[i][j];
         }

         if (size->w < child_size.w)
            size->w = child_size.w;

         num_visible_children++;
      }
   }
   if (box->homogeneous)
   {
      for (i = 0; i < 2; i++)
      {
         for (j = 0; j < box->cells_count[i]; j++)
            box->request_sizes[i][j] = size->h;
      }
      size->h *= num_visible_children;
   }
   if (num_visible_children > 1)
      size->h += (num_visible_children - 1) * box->spacing;

   size->w += 2 * container->border_width;
   size->h += 2 * container->border_width;
}

/* Resizes the vbox to the allocated size */
static void _etk_vbox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_VBox *vbox;
   Etk_Box *box;
   Etk_Container *container;
   Etk_Widget *child;
   Etk_Box_Cell *cell;
   Etk_Size requested_size;
   Etk_Size allocated_size;
   Etk_Geometry child_geometry;
   int num_children_to_expand = 0;
   int i, j;
   float start_offset, end_offset;
   float height;

   if (!(vbox = ETK_VBOX(widget)))
      return;

   box = ETK_BOX(vbox);
   container = ETK_CONTAINER(vbox);

   _etk_vbox_size_request(widget, &requested_size);
   requested_size.w -= 2 * container->border_width;
   requested_size.h -= 2 * container->border_width;
   allocated_size.w = geometry.w - 2 * container->border_width;
   allocated_size.h = geometry.h - 2 * container->border_width;

   start_offset = container->border_width;
   end_offset = container->border_width;

   if (allocated_size.h <= requested_size.h)
   {
      float ratio;

      ratio = (float)allocated_size.h / requested_size.h;
      for (i = 0; i < 2; i++)
      {
         j = (i == ETK_BOX_START) ? 0 : box->cells_count[i] - 1;
         cell = (i == ETK_BOX_START) ? box->first_cell[i] : box->last_cell[i];
         while (cell)
         {
            child = cell->child;

            if (etk_widget_is_visible(child))
            {
               height = box->request_sizes[i][j] * ratio;
               child_geometry.x = geometry.x + container->border_width;
               child_geometry.w = allocated_size.w;
               child_geometry.h = height;

               if (cell->group == ETK_BOX_START)
               {
                  child_geometry.y = geometry.y + start_offset;
                  start_offset += height + box->spacing;
               }
               else
               {
                  child_geometry.y = geometry.y + geometry.h - end_offset - child_geometry.h;
                  end_offset += height + box->spacing;
               }

               child_geometry.y += cell->padding;
               child_geometry.h -= 2 * cell->padding;

               etk_container_child_space_fill(child, &child_geometry,
                     !(cell->fill_policy & ETK_BOX_SHRINK_OPPOSITE),
                     cell->fill_policy & ETK_BOX_FILL, 0.5, 0.5);
               etk_widget_size_allocate(child, child_geometry);
            }

            cell = (i == ETK_BOX_START) ? cell->next : cell->prev;
            j = (i == ETK_BOX_START) ? (j + 1) : (j - 1);
         }
      }
   }
   else
   {
      float free_space;

      for (i = 0; i < 2; i++)
      {
         for (cell = box->first_cell[i]; cell; cell = cell->next)
         {
            child = cell->child;

            if (!etk_widget_is_visible(child))
               continue;

            if (cell->fill_policy & ETK_BOX_EXPAND)
               num_children_to_expand++;
         }
      }

      if (num_children_to_expand <= 0)
         free_space = 0;
      else
         free_space = (float)(allocated_size.h - requested_size.h) / num_children_to_expand;

      for (i = 0; i < 2; i++)
      {
         j = (i == ETK_BOX_START) ? 0 : box->cells_count[i] - 1;
         cell = (i == ETK_BOX_START) ? box->first_cell[i] : box->last_cell[i];
         while (cell)
         {
            child = cell->child;

            if (etk_widget_is_visible(child))
            {
               height = box->request_sizes[i][j];
               if (cell->fill_policy & ETK_BOX_EXPAND)
                  height += free_space;
               child_geometry.x = geometry.x + container->border_width;
               child_geometry.w = allocated_size.w;
               child_geometry.h = height;

               if (cell->group == ETK_BOX_START)
               {
                  child_geometry.y = geometry.y + start_offset;
                  start_offset += height + box->spacing;
               }
               else
               {
                  child_geometry.y = geometry.y + geometry.h - end_offset - child_geometry.h;
                  end_offset += height + box->spacing;
               }

               child_geometry.y += cell->padding;
               child_geometry.h -= 2 * cell->padding;

               etk_container_child_space_fill(child, &child_geometry,
                     !(cell->fill_policy & ETK_BOX_SHRINK_OPPOSITE),
                     cell->fill_policy & ETK_BOX_FILL, 0.5, 0.5);
               etk_widget_size_allocate(child, child_geometry);
            }

            cell = (i == ETK_BOX_START) ? cell->next : cell->prev;
            j = (i == ETK_BOX_START) ? (j + 1) : (j - 1);
         }
      }
   }
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Adds a new widget to the box, after the cell "after" */
static void _etk_box_insert_after_cell(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, Etk_Box_Cell *after, Etk_Box_Fill_Policy fill_policy, int padding)
{
   Etk_Box_Cell *cell;
   Etk_Widget *box_widget;

   if (!(box_widget = ETK_WIDGET(box)) || !child)
      return;
   if (after && after->group != group)
   {
      ETK_WARNING("The child to pack and the widget after which the child should be packed "
         "do not belong to the same child-group");
      return;
   }

   cell = malloc(sizeof(Etk_Box_Cell));
   cell->prev = NULL;
   cell->next = NULL;
   cell->child = child;
   cell->focus_node = NULL;
   cell->group = group;
   cell->fill_policy = fill_policy;
   cell->padding = padding;

   if (after)
   {
      cell->prev = after;
      cell->next = after->next;
      if (after->next)
         after->next->prev = cell;
      else
         box->last_cell[group] = cell;
      after->next = cell;

      box_widget->focus_order = eina_list_append_relative_list(box_widget->focus_order, child, after->focus_node);
      cell->focus_node = eina_list_next(after->focus_node);
   }
   else
   {
      cell->next = box->first_cell[group];
      if (box->first_cell[group])
         box->first_cell[group]->prev = cell;
      else
         box->last_cell[group] = cell;
      box->first_cell[group] = cell;

      if (group == ETK_BOX_START || !box->last_cell[ETK_BOX_START])
      {
         box_widget->focus_order = eina_list_prepend(box_widget->focus_order, child);
         cell->focus_node = box_widget->focus_order;
      }
      else
      {
         box_widget->focus_order = eina_list_append_relative_list(box_widget->focus_order,
               child, box->last_cell[ETK_BOX_START]->focus_node);
         cell->focus_node = eina_list_next(box->last_cell[ETK_BOX_START]->focus_node);
      }
   }
   box->cells_count[group]++;

   etk_widget_parent_set(child, ETK_WIDGET(box));
   etk_object_data_set(ETK_OBJECT(child), "_Etk_Box::Cell", cell);
   etk_signal_emit(ETK_CONTAINER_CHILD_ADDED_SIGNAL, ETK_OBJECT(box), child);
}

/* Gets the cell of the box containing the widget */
static Etk_Box_Cell *_etk_box_cell_get(Etk_Widget *widget)
{
   if (!widget)
      return NULL;
   return etk_object_data_get(ETK_OBJECT(widget), "_Etk_Box::Cell");
}

/* Gets the nth cell of the box in the corresponding group */
static Etk_Box_Cell *_etk_box_cell_nth_get(Etk_Box *box, Etk_Box_Group group, int n)
{
   Etk_Box_Cell *cell;
   int i;

   if (!box || box->cells_count[group] <= 0)
      return NULL;

   if (n < (box->cells_count[group] / 2))
   {
      cell = box->first_cell[group];
      for (i = 0; i < n && cell->next; i++)
         cell = cell->next;
   }
   else
   {
      cell = box->last_cell[group];
      for (i = 0; i < (box->cells_count[group] - n - 1) && cell->prev; i++)
         cell = cell->prev;
   }

   return cell;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Box
 *
 * Etk_Box is the base class for Etk_HBox and Etk_VBox:
 * in an Etk_HBox, the children are packed horizontally, and in an Etk_VBox, the children are packed vertically. @n
 * You can pack a child in two different groups of widgets: the start-group and the end-group. The children of the
 * start-group are packed at the start of the box (left for the hbox, top for the vbox) and the children of the
 * end-group are packed at the end of the box (right for the hbox, bottom for the vbox). @n
 * Widgets can be packed with the functions etk_box_prepend(), etk_box_append(), etk_box_insert() and
 * etk_box_insert_at().
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Box
 *         - Etk_HBox
 *         - Etk_VBox
 *
 * \par Properties:
 * @prop_name "homogeneous": Whether or not the children of the box take up the same amount of space
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "spacing": The amount of space between two children
 * @prop_type Integer
 * @prop_rw
 * @prop_val 0
 */
