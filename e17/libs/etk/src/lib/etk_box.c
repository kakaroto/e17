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
   int padding;
   Etk_Bool expand;
   Etk_Bool fill;
   Etk_Bool pack_end;
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
static void _etk_box_pack_full(Etk_Box *box, Etk_Widget *child, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_end);

static void _etk_hbox_constructor(Etk_HBox *hbox);
static void _etk_hbox_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_hbox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

static void _etk_vbox_constructor(Etk_VBox *vbox);
static void _etk_vbox_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_vbox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

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
 * @brief Adds the child at the start of the box. @n
 * Repeated calls to etk_box_pack_start() will pack widgets from start to end
 * (left to right for an Etk_HBbox, top to bottom for an Etk_VBox)
 * @param box a box
 * @param child the child to add
 * @param expand if @a expand == ETK_TRUE, the cell that contains the child will take all the available space
 * @param fill if @a fill == ETK_TRUE, the child will fill all the available space in its cell
 * @param padding the amount of free space on the two sides of the child
 */
void etk_box_pack_start(Etk_Box *box, Etk_Widget *child, Etk_Bool expand, Etk_Bool fill, int padding)
{
   _etk_box_pack_full(box, child, expand, fill, padding, ETK_FALSE);
}

/**
 * @brief Adds the child at the end of the box. @n
 * Repeated calls to etk_box_pack_end() will pack widgets from end to start
 * (right to left for Etk_HBbox, bottom to top for Etk_VBox)
 * @param box a box
 * @param child the child to add
 * @param expand if @a expand == ETK_TRUE, the cell that contains the child will take all the available space
 * @param fill if @a fill == ETK_TRUE, the child will fill all the available space in its cell
 * @param padding the amount of free space on the two sides of the child
 * @note To pack several widgets in a specific order, at the end of a box, you have to call etk_box_pack_end() in
 * the reverse order
 */
void etk_box_pack_end(Etk_Box *box, Etk_Widget *child, Etk_Bool expand, Etk_Bool fill, int padding)
{
   _etk_box_pack_full(box, child, expand, fill, padding, ETK_TRUE);
}

/**
 * @brief Moves the child to a new position in the box
 * @param box a box
 * @param child the child to move
 * @param position the new position of the child
 */ 
void etk_box_child_reorder(Etk_Box *box, Etk_Widget *child, int position)
{
   Evas_List *l;
   Etk_Box_Cell *cell;
   int i;

   if (!box || !child || (child->parent != ETK_WIDGET(box)))
      return;

   position = ETK_CLAMP(0, evas_list_count(box->cells) - 1, position);

   for (l = box->cells, i = 0; l; l = l->next, i++)
   {
      cell = l->data;
      if (cell->child == child)
      {
         if (i == position)
            return;
         else if (i < position)
         {
            box->cells = evas_list_append_relative(box->cells, cell, evas_list_nth(box->cells, position));
            box->cells = evas_list_remove_list(box->cells, l);
         }
         else
         {
            box->cells = evas_list_prepend_relative(box->cells, cell, evas_list_nth(box->cells, position));
            box->cells = evas_list_remove_list(box->cells, l);
         }
         return;
      }
   }
}

/**
 * @brief Sets the amount of free space between two cells
 * @param box a box
 * @param spacing the amount of free space between two cells
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
 * @return Returns the amount of free space between two cells
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
 * @param homogeneous if @a homogeneous == ETK_TRUE, all the cells will have the same size
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

   box->cells = NULL;
   box->spacing = 0;
   box->homogeneous = ETK_FALSE;
   box->requested_sizes = NULL;

   ETK_CONTAINER(box)->child_add = _etk_box_child_add;
   ETK_CONTAINER(box)->child_remove = _etk_box_child_remove;
   ETK_CONTAINER(box)->children_get = _etk_box_children_get;
   ETK_WIDGET(box)->use_focus_order = ETK_TRUE;
}

/* Destroys the box */
static void _etk_box_destructor(Etk_Box *box)
{
   if (!box)
      return;

   while (box->cells)
   {
      free(box->cells->data);
      box->cells = evas_list_remove_list(box->cells, box->cells);
   }
   free(box->requested_sizes);
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
   etk_box_pack_start(ETK_BOX(container), widget, ETK_TRUE, ETK_TRUE, 0);
}

/* Removes the child from the box */
static void _etk_box_child_remove(Etk_Container *container, Etk_Widget *widget)
{
   Etk_Box *box;
   Evas_List *l;
   Etk_Box_Cell *cell;

   if (!(box = ETK_BOX(container)) || !widget || (widget->parent != ETK_WIDGET(container)))
      return;

   for (l = box->cells; l; l = l->next)
   {
      cell = l->data;
      if (cell->child == widget)
      {
         etk_widget_parent_set_full(widget, NULL, ETK_FALSE);
         free(cell);
         box->cells = evas_list_remove_list(box->cells, l);
         ETK_WIDGET(box)->focus_order = evas_list_remove(ETK_WIDGET(box)->focus_order, widget);
         etk_widget_size_recalc_queue(ETK_WIDGET(box));
         return;
      }
   }
}

/* Gets the list of the children of the box */
static Evas_List *_etk_box_children_get(Etk_Container *container)
{
   Etk_Box *box;
   Evas_List *children, *l;
   Etk_Box_Cell *cell;
   
   if (!(box = ETK_BOX(container)))
      return NULL;
   
   children = NULL;
   for (l = box->cells; l; l = l->next)
   {
      cell = l->data;
      children = evas_list_append(children, cell->child);
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
   int i;

   if (!(hbox = ETK_HBOX(widget)) || !size)
      return;

   box = ETK_BOX(hbox);
   container = ETK_CONTAINER(hbox);
   size->w = 0;
   size->h = 0;
   num_children = evas_list_count(box->cells);
   num_visible_children = 0;
   box->requested_sizes = realloc(box->requested_sizes, num_children * sizeof(int));

   for (l = box->cells, i = 0; l; l = l->next, i++)
   {
      cell = l->data;
      child = cell->child;
      box->requested_sizes[i] = 0;
      
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
         box->requested_sizes[i] = child_size.w + 2 * cell->padding;
         size->w += box->requested_sizes[i];
      }

      if (size->h < child_size.h)
         size->h = child_size.h;
      
      num_visible_children++;
   }
   if (box->homogeneous)
   {
      for (i = 0; i < num_children; i++)
         box->requested_sizes[i] = size->w;
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
   int num_children_to_expand = 0, i;
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
      for (l = box->cells, i = 0; l; l = l->next, i++)
      {
         cell = l->data;
         child = cell->child;
         
         if (!etk_widget_is_visible(child))
            continue;

         child_geometry.y = geometry.y + container->border_width;
         child_geometry.w = box->requested_sizes[i] * ratio;
         child_geometry.h = allocated_size.h;

         if (!cell->pack_end)
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
         
         etk_container_child_space_fill(child, &child_geometry, cell->fill, ETK_TRUE, 0.5, 0.5); 
         etk_widget_size_allocate(child, child_geometry);
      }
   }
   else
   {
      float free_space;

      for (l = box->cells; l; l = l->next)
      {
         cell = l->data;
         child = cell->child;
         
         if (!etk_widget_is_visible(child))
            continue;
         
         if (cell->expand)
            num_children_to_expand++;
      }

      if (num_children_to_expand <= 0)
         free_space = 0;
      else
         free_space = (float)(allocated_size.w - requested_size.w) / num_children_to_expand;

      for (l = box->cells, i = 0; l; l = l->next, i++)
      {
         cell = l->data;
         child = cell->child;
         
         if (!etk_widget_is_visible(child))
            continue;
         
         child_geometry.y = geometry.y + container->border_width;
         child_geometry.w = box->requested_sizes[i];
         if (cell->expand)
            child_geometry.w += free_space;
         child_geometry.h = allocated_size.h;

         if (!cell->pack_end)
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
         
         etk_container_child_space_fill(child, &child_geometry, cell->fill, ETK_TRUE, 0.5, 0.5); 
         etk_widget_size_allocate(child, child_geometry);
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
   int i;

   if (!(vbox = ETK_VBOX(widget)) || !size)
      return;

   box = ETK_BOX(vbox);
   container = ETK_CONTAINER(vbox);
   size->w = 0;
   size->h = 0;
   num_children = evas_list_count(box->cells);
   num_visible_children = 0;
   box->requested_sizes = realloc(box->requested_sizes, num_children * sizeof(int));

   for (l = box->cells, i = 0; l; l = l->next, i++)
   {
      cell = l->data;
      child = cell->child;
      box->requested_sizes[i] = 0;
      
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
         box->requested_sizes[i] = child_size.h + 2 * cell->padding;
         size->h += box->requested_sizes[i];
      }

      if (size->w < child_size.w)
         size->w = child_size.w;
      
      num_visible_children++;
   }
   if (box->homogeneous)
   {
      for (i = 0; i < num_children; i++)
         box->requested_sizes[i] = size->h;
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
   int num_children_to_expand = 0, i;
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
      for (l = box->cells, i = 0; l; l = l->next, i++)
      {
         cell = l->data;
         child = cell->child;
         
         if (!etk_widget_is_visible(child))
            continue;

         child_geometry.x = geometry.x + container->border_width;
         child_geometry.w = allocated_size.w;
         child_geometry.h = box->requested_sizes[i] * ratio;

         if (!cell->pack_end)
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
         
         etk_container_child_space_fill(child, &child_geometry, ETK_TRUE, cell->fill, 0.5, 0.5); 
         etk_widget_size_allocate(child, child_geometry);
      }
   }
   else
   {
      float free_space;

      for (l = box->cells; l; l = l->next)
      {
         cell = l->data;
         child = cell->child;
         
         if (!etk_widget_is_visible(child))
            continue;
         
         if (cell->expand)
            num_children_to_expand++;
      }

      if (num_children_to_expand <= 0)
         free_space = 0;
      else
         free_space = (float)(allocated_size.h - requested_size.h) / num_children_to_expand;

      for (l = box->cells, i = 0; l; l = l->next, i++)
      {
         cell = l->data;
         child = cell->child;
         
         if (!etk_widget_is_visible(child))
            continue;
         
         child_geometry.x = geometry.x + container->border_width;
         child_geometry.w = allocated_size.w;
         child_geometry.h = box->requested_sizes[i];
         if (cell->expand)
            child_geometry.h += free_space;

         if (!cell->pack_end)
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
         
         etk_container_child_space_fill(child, &child_geometry, ETK_TRUE, cell->fill, 0.5, 0.5); 
         etk_widget_size_allocate(child, child_geometry);
      }
   }
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Adds the "child" to the "box" : "expand" and "fill" describe its fill policy, "padding" is the amount of space
 * on the sides of the child and. If "pack_end" == ETK_TRUE, the child will be packed at the end of the box */
static void _etk_box_pack_full(Etk_Box *box, Etk_Widget *child, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_end)
{
   Etk_Box_Cell *cell, *last_cell;
   Evas_List *l;
   Etk_Widget *w;
   
   if (!box || !child)
      return;

   cell = malloc(sizeof(Etk_Box_Cell));
   cell->expand = expand;
   cell->fill = fill;
   cell->padding = padding;
   cell->pack_end = pack_end;
   cell->child = child;
   
   /* Adds the child in the focus_order list, at the right place */
   last_cell = evas_list_data(evas_list_last(box->cells));
   if (!last_cell)
      ETK_WIDGET(box)->focus_order = evas_list_append(ETK_WIDGET(box)->focus_order, child);
   else
   {
      for (l = ETK_WIDGET(box)->focus_order; l; l = l->next)
      {
         w = ETK_WIDGET(l->data);
         if (w == last_cell->child)
         {
            if (last_cell->pack_end)
               ETK_WIDGET(box)->focus_order = evas_list_prepend_relative(ETK_WIDGET(box)->focus_order, child, l);
            else
               ETK_WIDGET(box)->focus_order = evas_list_append_relative(ETK_WIDGET(box)->focus_order, child, l);
            break;
         }
      }
   }
   
   box->cells = evas_list_append(box->cells, cell);
   etk_widget_parent_set(child, ETK_WIDGET(box));
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
