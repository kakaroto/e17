/** @file etk_box.c */
#include "etk_box.h"
#include <stdlib.h>
#include "etk_utils.h"

/**
 * @addtogroup Etk_Box
 * @{
 */

typedef struct Etk_Box_Cell
{
   Etk_Widget *child;
   Etk_Box_Group group;
   Etk_Box_Fill_Policy fill_policy;
   int padding;
} Etk_Box_Cell;

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
static Evas_List *_etk_box_children_get(Etk_Container *container);

static void _etk_hbox_constructor(Etk_HBox *hbox);
static void _etk_hbox_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_hbox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

static void _etk_vbox_constructor(Etk_VBox *vbox);
static void _etk_vbox_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_vbox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

static void _etk_box_insert_after_cell(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, Etk_Box_Cell *after, Etk_Box_Fill_Policy fill_policy, int padding);
static Etk_Box_Cell *_etk_box_cell_get(Etk_Box *box, Etk_Widget *widget);


/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Box
 * @return Returns the type of an Etk_Box
 */
Etk_Type *etk_box_type_get()
{
   static Etk_Type *box_type = NULL;

   if (!box_type)
   {
      box_type = etk_type_new("Etk_Box", ETK_CONTAINER_TYPE, sizeof(Etk_Box),
         ETK_CONSTRUCTOR(_etk_box_constructor), ETK_DESTRUCTOR(_etk_box_destructor));
   
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
 * @brief Gets the type of an Etk_HBox
 * @return Returns the type of an Etk_HBox
 */
Etk_Type *etk_hbox_type_get()
{
   static Etk_Type *hbox_type = NULL;

   if (!hbox_type)
   {
      hbox_type = etk_type_new("Etk_HBox", ETK_BOX_TYPE, sizeof(Etk_HBox),
         ETK_CONSTRUCTOR(_etk_hbox_constructor), NULL);
   }

   return hbox_type;
}

/**
 * @brief Gets the type of an Etk_VBox
 * @return Returns the type of an Etk_VBox
 */
Etk_Type *etk_vbox_type_get()
{
   static Etk_Type *vbox_type = NULL;

   if (!vbox_type)
   {
      vbox_type = etk_type_new("Etk_VBox", ETK_BOX_TYPE, sizeof(Etk_VBox),
         ETK_CONSTRUCTOR(_etk_vbox_constructor), NULL);
   }

   return vbox_type;
}

/**
 * @brief Creates a new hbox
 * @param homogeneous if @a homogeneous == ETK_TRUE, all the children will have the same size
 * @param spacing the amount of space between two children
 */
Etk_Widget *etk_hbox_new(Etk_Bool homogeneous, int spacing)
{
   return etk_widget_new(ETK_HBOX_TYPE, "homogeneous", homogeneous, "spacing", spacing, NULL);
}

/**
 * @brief Creates a new vbox
 * @param homogeneous if @a homogeneous == ETK_TRUE, all the cells will have the same size
 * @param spacing the amount of space between two cells
 */
Etk_Widget *etk_vbox_new(Etk_Bool homogeneous, int spacing)
{
   return etk_widget_new(ETK_VBOX_TYPE, "homogeneous", homogeneous, "spacing", spacing, NULL);
}

/**
 * @brief Packs a widget at the start of one of the two groups of children of the box
 * @param box a box
 * @param child the widget to pack
 * @param group the group in which the child will be packed
 * @param fill_policy the fill policy of the child
 * @param padding the amount of free space on the two sides of the child, in pixels
 */
void etk_box_prepend(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, Etk_Box_Fill_Policy fill_policy, int padding)
{
   if (!box || !child)
      return;
   _etk_box_insert_after_cell(box, child, group, NULL, fill_policy, padding);
}

/**
 * @brief Packs a widget at the end of one of the groups of children of the box
 * @param box a box
 * @param child the widget to pack
 * @param group the group in which the child will be packed
 * @param fill_policy the fill policy of the child
 * @param padding the amount of free space on the two sides of the child, in pixels
 */
void etk_box_append(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, Etk_Box_Fill_Policy fill_policy, int padding)
{
   Etk_Box_Cell *after_cell;
   
   if (!box || !child)
      return;
   
   after_cell = evas_list_data(evas_list_last(box->cells[group]));
   _etk_box_insert_after_cell(box, child, group, after_cell, fill_policy, padding);
}

/**
 * @brief Packs a widget after another widget, in one of the groups of children of the box
 * @param box a box
 * @param child the widget to pack
 * @param group the group in which the child will be packed
 * @param after the child after which @a child will be packed. If @a after is NULL, @a child will be packed at the start
 * of the group
 * @param fill_policy the fill policy of the child
 * @param padding the amount of free space on the two sides of the child, in pixels
 * @warning @a after has to be packed in the group @a group, or it'll have no effect
 */
void etk_box_insert(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, Etk_Widget *after, Etk_Box_Fill_Policy fill_policy, int padding)
{
   Etk_Box_Cell *after_cell;
   
   if (!box || !child)
      return;
   
   after_cell = _etk_box_cell_get(box, after);
   _etk_box_insert_after_cell(box, child, group, after_cell, fill_policy, padding);
}

/**
 * @brief Packs a widget at a given position, in one of the groups of children of the box
 * @param box a box
 * @param child the widget to pack
 * @param group the group in which the child will be packed
 * @param pos the position where to pack @a child (starting from 0). If @a pos <= 0, @a child will be packed at the
 * start of the group of children, and if @a pos is greater than the number of children in the group, it will be packed
 * at the end
 * @param fill_policy the fill policy of the child
 * @param padding the amount of free space on the two sides of the child, in pixels
 */
void etk_box_insert_at(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, int pos, Etk_Box_Fill_Policy fill_policy, int padding)
{
   Etk_Box_Cell *after_cell;
   
   if (!box || !child)
      return;
   
   if (pos <= 0)
      after_cell = NULL;
   else if (pos >= evas_list_count(box->cells[group]))
      after_cell = evas_list_data(evas_list_last(box->cells[group]));
   else
      after_cell = evas_list_nth(box->cells[group], pos - 1);
   
   _etk_box_insert_after_cell(box, child, group, after_cell, fill_policy, padding);
}

/**
 * @brief Gets the child at a given position in the box
 * @param box a box
 * @param group the group in which the child you want to get is
 * @param pos the position of the child you want to get (starting from 0)
 * @return Returns the child at the given position, or NULL on failure
 */
Etk_Widget *etk_box_child_get_at(Etk_Box *box, Etk_Box_Group group, int pos)
{
   Etk_Box_Cell *cell;
   
   if (!box)
      return NULL;
   
   if (!(cell = evas_list_nth(box->cells[group], pos)))
      return NULL;
   else
      return cell->child;
}

/**
 * @brief Sets the position of a child of the box
 * @param box a box
 * @param child the child you want to change the position of
 * @param group the group in which the child will be repacked
 * @param pos the position where to repack @a child (starting from 0). If @a pos <= 0, @a child will be packed at the
 * start of the group of children, and if @a pos is greater than the number of children in the group, it will be packed
 * at the end
 */
void etk_box_child_position_set(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, int pos)
{
   Etk_Box_Cell *cell;
   Evas_List *l;

   if (!box || !child || !(cell = _etk_box_cell_get(box, child)))
      return;
   
   box->cells[cell->group] = evas_list_remove(box->cells[cell->group], cell);
   if (pos <= 0)
      box->cells[group] = evas_list_prepend(box->cells[group], cell);
   else if (pos >= evas_list_count(box->cells[group]))
      box->cells[group] = evas_list_append(box->cells[group], cell);
   else
   {
      l = evas_list_nth_list(box->cells[group], pos - 1);
      box->cells[group] = evas_list_append_relative_list(box->cells[group], cell, l);
   }
   etk_widget_redraw_queue(ETK_WIDGET(box));
}

/**
 * @brief Gets the position of a child of the box
 * @param box a box
 * @param child the child you want to get the position of
 * @param group the location where to store the group in which the child is
 * @param pos the location where to store the position of the child
 * @return Returns ETK_TRUE on success, ETK_FALSE on failure (because @a child is not a child of the box)
 */
Etk_Bool etk_box_child_position_get(Etk_Box *box, Etk_Widget *child, Etk_Box_Group *group, int *pos)
{
   Evas_List *l;
   Etk_Box_Cell *cell;
   int i;
   
   if (!box || !child)
      return ETK_FALSE;
   
   if (!(cell = _etk_box_cell_get(box, child)))
      return ETK_FALSE;
   else
   {
      for (i = 0, l = box->cells[cell->group]; l; i++, l = l->next)
      {
         if (cell == (Etk_Box_Cell *)l->data)
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
}

/**
 * @brief Sets the packing settings of a child of the box
 * @param box a box
 * @param child a child of the box. If @a child is not packed in the box, this function has no effect
 * @param fill policy the fill policy of the child
 * @param padding the amount of free space on the two sides of the child, in pixels
 */
void etk_box_child_packing_set(Etk_Box *box, Etk_Widget *child, Etk_Box_Fill_Policy fill_policy, int padding)
{
   Etk_Box_Cell *cell;
   
   if (!box || !child)
      return;
   
   if ((cell = _etk_box_cell_get(box, child)))
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
 * @param fill policy the location where to store the fill policy of the child
 * @param padding the location where to store the padding of the child
 * @param Returns ETK_TRUE on success, and ETK_FALSE on failure (because @a child is not packed in @a box)
 */
Etk_Bool etk_box_child_packing_get(Etk_Box *box, Etk_Widget *child, Etk_Box_Fill_Policy *fill_policy, int *padding)
{
   Etk_Box_Cell *cell;
   
   if (!box || !child)
      return ETK_FALSE;
   
   if ((cell = _etk_box_cell_get(box, child)))
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
 * @param spacing the amount of free space between two cells, in pixels
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
 * @brief Checks whether all the cells of the box have the same size
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

   box->cells[ETK_BOX_START] = NULL;
   box->cells[ETK_BOX_END] = NULL;
   box->request_sizes[ETK_BOX_START] = NULL;
   box->request_sizes[ETK_BOX_END] = NULL;
   box->spacing = 0;
   box->homogeneous = ETK_FALSE;

   ETK_CONTAINER(box)->child_add = _etk_box_child_add;
   ETK_CONTAINER(box)->child_remove = _etk_box_child_remove;
   ETK_CONTAINER(box)->children_get = _etk_box_children_get;
   ETK_WIDGET(box)->use_focus_order = ETK_TRUE;
}

/* Destroys the box */
static void _etk_box_destructor(Etk_Box *box)
{
   int i;
   
   if (!box)
      return;

   for (i = 0; i < 2; i++)
   {
      while (box->cells[i])
      {
         free(box->cells[i]->data);
         box->cells[i] = evas_list_remove_list(box->cells[i], box->cells[i]);
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
   Etk_Widget *box_widget;
   Etk_Box *box;
   Etk_Box_Cell *cell;

   if (!(box_widget = ETK_WIDGET(container)) || !widget || (widget->parent != box_widget))
      return;
   
   box = ETK_BOX(box_widget);
   if ((cell = _etk_box_cell_get(box, widget)))
   {
      box->cells[cell->group] = evas_list_remove(box->cells[cell->group], cell);
      box_widget->focus_order = evas_list_remove(box_widget->focus_order, widget);
      etk_widget_parent_set_full(widget, NULL, ETK_FALSE);
      etk_widget_size_recalc_queue(box_widget);
      free(cell);
   }
}

/* Gets the list of the children of the box */
static Evas_List *_etk_box_children_get(Etk_Container *container)
{
   Etk_Box *box;
   Evas_List *children, *l;
   Etk_Box_Cell *cell;
   int i;
   
   if (!(box = ETK_BOX(container)))
      return NULL;
   
   children = NULL;
   for (i = 0; i < 2; i++)
   {
      for (l = box->cells[i]; l; l = l->next)
      {
         cell = l->data;
         children = evas_list_append(children, cell->child);
      }
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
   Evas_List *l;
   Etk_HBox *hbox;
   Etk_Box *box;
   Etk_Container *container;
   Etk_Box_Cell *cell;
   Etk_Widget *child;
   Etk_Size child_size;
   int num_children, num_visible_children;
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
      num_children = evas_list_count(box->cells[i]);
      box->request_sizes[i] = realloc(box->request_sizes[i], num_children * sizeof(int));
      for (l = box->cells[i], j = 0; l; l = l->next, j++)
      {
         cell = l->data;
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
         num_children = evas_list_count(box->cells[i]);
         for (j = 0; j < num_children; j++)
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
   Evas_List *l;
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
         j = 0;
         l = (i == ETK_BOX_START) ? box->cells[i] : evas_list_last(box->cells[i]);
         while (l)
         {
            cell = l->data;
            child = cell->child;
            
            if (etk_widget_is_visible(child))
            {
               child_geometry.y = geometry.y + container->border_width;
               child_geometry.w = box->request_sizes[i][j] * ratio;
               child_geometry.h = allocated_size.h;

               if (cell->group == ETK_BOX_START)
               {
                  child_geometry.x = geometry.x + start_offset;
                  start_offset += child_geometry.w + box->spacing;
               }
               else
               {
                  child_geometry.x = geometry.x + geometry.w - end_offset - child_geometry.w;
                  end_offset += child_geometry.w + box->spacing;
               }
               
               child_geometry.x += cell->padding;
               child_geometry.w -= 2 * cell->padding;
               
               etk_container_child_space_fill(child, &child_geometry, cell->fill_policy & ETK_BOX_FILL, ETK_TRUE, 0.5, 0.5); 
               etk_widget_size_allocate(child, child_geometry);
            }
            
            l = (i == ETK_BOX_START) ? l->next : l->prev;
            j = (i == ETK_BOX_START) ? (j + 1) : (j - 1);
         }
      }
   }
   else
   {
      float free_space;

      for (i = 0; i < 2; i++)
      {
         for (l = box->cells[i]; l; l = l->next)
         {
            cell = l->data;
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
         j = (i == ETK_BOX_START) ? 0 : (evas_list_count(box->cells[i]) - 1);
         l = (i == ETK_BOX_START) ? box->cells[i] : evas_list_last(box->cells[i]);
         while (l)
         {
            cell = l->data;
            child = cell->child;
            
            if (etk_widget_is_visible(child))
            {
               child_geometry.y = geometry.y + container->border_width;
               child_geometry.w = box->request_sizes[i][j];
               child_geometry.h = allocated_size.h;
               if (cell->fill_policy & ETK_BOX_EXPAND)
                  child_geometry.w += free_space;

               if (cell->group == ETK_BOX_START)
               {
                  child_geometry.x = geometry.x + start_offset;
                  start_offset += child_geometry.w + box->spacing;
               }
               else
               {
                  child_geometry.x = geometry.x + geometry.w - end_offset - child_geometry.w;
                  end_offset += child_geometry.w + box->spacing;
               }

               child_geometry.x += cell->padding;
               child_geometry.w -= 2 * cell->padding;
               
               etk_container_child_space_fill(child, &child_geometry, cell->fill_policy & ETK_BOX_FILL, ETK_TRUE, 0.5, 0.5); 
               etk_widget_size_allocate(child, child_geometry);
            }
            
            l = (i == ETK_BOX_START) ? l->next : l->prev;
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
   Evas_List *l;
   Etk_VBox *vbox;
   Etk_Box *box;
   Etk_Container *container;
   Etk_Box_Cell *cell;
   Etk_Widget *child;
   Etk_Size child_size;
   int num_children, num_visible_children;
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
      num_children = evas_list_count(box->cells[i]);
      box->request_sizes[i] = realloc(box->request_sizes[i], num_children * sizeof(int));
      for (l = box->cells[i], j = 0; l; l = l->next, j++)
      {
         cell = l->data;
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
         num_children = evas_list_count(box->cells[i]);
         for (j = 0; j < num_children; j++)
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
   Evas_List *l;
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
         j = 0;
         l = (i == ETK_BOX_START) ? box->cells[i] : evas_list_last(box->cells[i]);
         while (l)
         {
            cell = l->data;
            child = cell->child;
            
            if (etk_widget_is_visible(child))
            {
               child_geometry.x = geometry.x + container->border_width;
               child_geometry.w = allocated_size.w;
               child_geometry.h = box->request_sizes[i][j] * ratio;

               if (cell->group == ETK_BOX_START)
               {
                  child_geometry.y = geometry.y + start_offset;
                  start_offset += child_geometry.h + box->spacing;
               }
               else
               {
                  child_geometry.y = geometry.y + geometry.h - end_offset - child_geometry.h;
                  end_offset += child_geometry.h + box->spacing;
               }
               
               child_geometry.y += cell->padding;
               child_geometry.h -= 2 * cell->padding;
               
               etk_container_child_space_fill(child, &child_geometry, ETK_TRUE, cell->fill_policy & ETK_BOX_FILL, 0.5, 0.5); 
               etk_widget_size_allocate(child, child_geometry);
            }
            
            l = (i == ETK_BOX_START) ? l->next : l->prev;
            j = (i == ETK_BOX_START) ? (j + 1) : (j - 1);
         }
      }
   }
   else
   {
      float free_space;

      for (i = 0; i < 2; i++)
      {
         for (l = box->cells[i]; l; l = l->next)
         {
            cell = l->data;
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
         j = 0;
         l = (i == ETK_BOX_START) ? box->cells[i] : evas_list_last(box->cells[i]);
         while (l)
         {
            cell = l->data;
            child = cell->child;
            
            if (etk_widget_is_visible(child))
            {
               child_geometry.x = geometry.x + container->border_width;
               child_geometry.w = allocated_size.w;
               child_geometry.h = box->request_sizes[i][j];
               if (cell->fill_policy & ETK_BOX_EXPAND)
                  child_geometry.h += free_space;

               if (cell->group == ETK_BOX_START)
               {
                  child_geometry.y = geometry.y + start_offset;
                  start_offset += child_geometry.h + box->spacing;
               }
               else
               {
                  child_geometry.y = geometry.y + geometry.h - end_offset - child_geometry.h;
                  end_offset += child_geometry.h + box->spacing;
               }

               child_geometry.y += cell->padding;
               child_geometry.h -= 2 * cell->padding;
               
               etk_container_child_space_fill(child, &child_geometry, ETK_TRUE, cell->fill_policy & ETK_BOX_FILL, 0.5, 0.5); 
               etk_widget_size_allocate(child, child_geometry);
            }
            
            l = (i == ETK_BOX_START) ? l->next : l->prev;
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

/* Adds a child to the box, after the child contained in the cell "after" */
static void _etk_box_insert_after_cell(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, Etk_Box_Cell *after, Etk_Box_Fill_Policy fill_policy, int padding)
{
   Etk_Box_Cell *cell, *c;
   Etk_Widget *box_widget;
   
   if (!box || !child)
      return;
   if (after && after->group != group)
   {
      ETK_WARNING("The child to pack and the widget after which the child should be packed "
         "do not belong to the same start/end-group");
      return;
   }
   
   box_widget = ETK_WIDGET(box);

   cell = malloc(sizeof(Etk_Box_Cell));
   cell->child = child;
   cell->group = group;
   cell->fill_policy = fill_policy;
   cell->padding = padding;
   
   if (!after)
   {
      box->cells[group] = evas_list_prepend(box->cells[group], cell);
      if (group == ETK_BOX_START)
         box_widget->focus_order = evas_list_prepend(box_widget->focus_order, child);
      else
      {
         if ((c = evas_list_data(box->cells[ETK_BOX_END])))
            box_widget->focus_order = evas_list_prepend_relative(box_widget->focus_order, child, c->child);
         else
            box_widget->focus_order = evas_list_append(box_widget->focus_order, child);
      }
   }
   else
   {
      box->cells[group] = evas_list_append_relative(box->cells[group], cell, after);
      box_widget->focus_order = evas_list_append_relative(box_widget->focus_order, child, after->child);
   }
   
   etk_widget_parent_set(child, ETK_WIDGET(box));
}

/* Gets the cell of the box containing the widget */
static Etk_Box_Cell *_etk_box_cell_get(Etk_Box *box, Etk_Widget *widget)
{
   int i;
   Evas_List *l;
   Etk_Box_Cell *cell;
   
   if (!box || !widget)
      return NULL;
   
   for (i = 0; i < 2; i++)
   {
      for (l = box->cells[i]; l; l = l->next)
      {
         cell = l->data;
         if (cell->child == widget)
            return cell;
      }
   }
   return NULL;
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
 * A child can be packed at the start of the box (left for the hbox, top for the vbox),
 * or at the end of the box (right for the hbox, bottom for the vbox). @n @n
 * The "homogeneous" setting controls whether or not all the children of the box should take the same amount of space. @n
 * The "spacing" setting determines the amount of space between two children.
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
 * @prop_name "homogeneous": Whether or not the children of the box should take the same amount of space
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par 
 * @prop_name "spacing": The amount of space between two children
 * @prop_type Integer
 * @prop_rw
 * @prop_val 0
 */
