/** @file etk_box.c */
#include "etk_box.h"
#include <stdlib.h>
#include <Ecore_Data.h>

/**
 * @addtogroup Etk_Box
* @{
 */

enum _Etk_Box_Property_Id
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
static void _etk_box_pack_full(Etk_Box *box, Etk_Widget *child, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_end);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Box
 * @return Returns the type on an Etk_Box
 */
Etk_Type *etk_box_type_get()
{
   static Etk_Type *box_type = NULL;

   if (!box_type)
   {
      box_type = etk_type_new("Etk_Box", ETK_CONTAINER_TYPE, sizeof(Etk_Box), ETK_CONSTRUCTOR(_etk_box_constructor), ETK_DESTRUCTOR(_etk_box_destructor), NULL);
   
      etk_type_property_add(box_type, "spacing", ETK_BOX_SPACING_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(0));
      etk_type_property_add(box_type, "homogeneous", ETK_BOX_HOMOGENEOUS_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(FALSE));
      
      box_type->property_set = _etk_box_property_set;
      box_type->property_get = _etk_box_property_get;
   }

   return box_type;
}

/**
 * @brief Adds the child to the start of the box
 * @param box a box
 * @param child the child to add
 * @param expand if @a expand == TRUE, the cell where the child will be will take all the available space
 * @param fill if @a fill == TRUE, the child will take all the available space in its cell
 * @param padding the size of the space on the sides of the child
 */
void etk_box_pack_start(Etk_Box *box, Etk_Widget *child, Etk_Bool expand, Etk_Bool fill, int padding)
{
   _etk_box_pack_full(box, child, expand, fill, padding, 0);
}

/**
 * @brief Adds the child to the end of the box
 * @param box a box
 * @param child the child to add
 * @param expand if @a expand == TRUE, the cell where the child will be will take all the available space
 * @param fill if @a fill == TRUE, the child will take all the available space in its cell
 * @param padding the size of the space on the sides of the child
 */
void etk_box_pack_end(Etk_Box *box, Etk_Widget *child, Etk_Bool expand, Etk_Bool fill, int padding)
{
   _etk_box_pack_full(box, child, expand, fill, padding, 1);
}

/**
 * @brief Moves the child to a new position in the box
 * @param box a box
 * @param child the child to move
 * @param position the new position of the child
 */ 
void etk_box_child_reorder(Etk_Box *box, Etk_Widget *child, int position)
{
   Etk_Container *container;
   int current_position;

   if (!(container = ETK_CONTAINER(box)) || !child || (child->parent != container))
      return;

   if (position < 0)
      position = 0;
   else if (position >= ecore_dlist_nodes(container->children))
      position = ecore_dlist_nodes(container->children) - 1;

   if (ecore_dlist_goto(container->children, child))
   {
      current_position = ecore_dlist_index(container->children);
      if (current_position == position)
         return;
      else if (current_position < position)
      {
         ecore_dlist_remove(container->children);
         ecore_dlist_goto_index(container->children, position);
         ecore_dlist_insert(container->children, child);
      }
      else
      {
         ecore_dlist_goto_index(container->children, position);
         ecore_dlist_insert(container->children, child);
         ecore_dlist_remove(container->children);
      }
   }
}

/**
 * @brief Sets the size of the space between two cells
 * @param box a box
 * @param spacing the size of the space between two cells
 */
void etk_box_spacing_set(Etk_Box *box, int spacing)
{
   if (!box)
      return;

   box->spacing = spacing;
   etk_widget_resize_queue(ETK_WIDGET(box));
   etk_object_notify(ETK_OBJECT(box), "spacing");
}

/**
 * @brief Gets the size of the space between two cells
 * @param box a box
 * @return Returns the size of the space between two cells
 */
int etk_box_spacing_get(Etk_Box *box)
{
   if (!box)
      return 0;
   return box->spacing;
}

/**
 * @brief Changes the homogenous property of the box
 * @param box a box
 * @param homogeneous if homogeneous == TRUE, all the cells will have the same size
 */
void etk_box_homogeneous_set(Etk_Box *box, Etk_Bool homogeneous)
{
   if (!box)
      return;

   box->homogeneous = homogeneous;
   etk_widget_resize_queue(ETK_WIDGET(box));
   etk_object_notify(ETK_OBJECT(box), "homogeneous");
}

/**
 * @brief Checks if the box is homogenous
 * @param box a box
 * @return Returns TRUE if the box is homogeneous, FALSE otherwise
 */
Etk_Bool etk_box_homogeneous_get(Etk_Box *box)
{
   if (!box)
      return FALSE;
   return box->homogeneous;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the box */
static void _etk_box_constructor(Etk_Box *box)
{
   if (!box)
      return;

   box->spacing = 0;
   box->homogeneous = FALSE;

   ETK_CONTAINER(box)->child_add = _etk_box_child_add;
   ETK_CONTAINER(box)->child_remove = _etk_box_child_remove;
}

/* Destroys the box */
static void _etk_box_destructor(Etk_Box *box)
{
   Etk_Container *container;
   Etk_Widget *child;

   if (!(container = ETK_CONTAINER(box)))
      return;

   ecore_dlist_goto_first(container->children);
   while ((child = ecore_dlist_next(container->children)))
      _etk_box_child_remove(container, child);
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

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Adds a child to the box */
static void _etk_box_child_add(Etk_Container *container, Etk_Widget *widget)
{
   etk_box_pack_start(ETK_BOX(container), widget, TRUE, TRUE, 0);
}

/* Removes the child from the box */
static void _etk_box_child_remove(Etk_Container *container, Etk_Widget *widget)
{
   Etk_Box *box;

   if (!(box = ETK_BOX(container)) || !widget || (widget->parent != container))
      return;

   if (ecore_dlist_goto(container->children, widget))
   {
      free(widget->child_properties);
      widget->child_properties = NULL;

      etk_widget_parent_set(widget, NULL);

      ecore_dlist_remove(container->children);
      etk_widget_resize_queue(ETK_WIDGET(box));
   }
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Adds the "child" to the "box" : "expand" and "fill" describe its fill policy, "padding" is the size of the space
 * on the sides of the child and if "pack_end" == TRUE, the child will be packed at the end of the box */
static void _etk_box_pack_full(Etk_Box *box, Etk_Widget *child, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_end)
{
   Etk_Container *container;
   Etk_Box_Child_Properties *child_properties;
   
   if (!(container = ETK_CONTAINER(box)) || !child)
      return;

   if (child->parent)
      etk_container_remove(child->parent, child);
   etk_widget_parent_set(child, container);
   child_properties = malloc(sizeof(Etk_Box_Child_Properties));
   child_properties->expand = expand;
   child_properties->fill = fill;
   child_properties->padding = padding;
   child_properties->pack_end = pack_end;
   child->child_properties = child_properties;

   ecore_dlist_append(container->children, child);
   etk_widget_resize_queue(ETK_WIDGET(box));
}


/** @} */
