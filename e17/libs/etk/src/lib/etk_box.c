/** @file etk_box.c */
#include "etk_box.h"
#include <stdlib.h>
#include <Evas.h>
#include "etk_utils.h"

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
static Evas_List *_etk_box_children_get(Etk_Container *container);
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
      box_type = etk_type_new("Etk_Box", ETK_CONTAINER_TYPE, sizeof(Etk_Box), ETK_CONSTRUCTOR(_etk_box_constructor), ETK_DESTRUCTOR(_etk_box_destructor));
   
      etk_type_property_add(box_type, "spacing", ETK_BOX_SPACING_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(0));
      etk_type_property_add(box_type, "homogeneous", ETK_BOX_HOMOGENEOUS_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(ETK_FALSE));
      
      box_type->property_set = _etk_box_property_set;
      box_type->property_get = _etk_box_property_get;
   }

   return box_type;
}

/**
 * @brief Adds the child to the start of the box
 * @param box a box
 * @param child the child to add
 * @param expand if @a expand == ETK_TRUE, the cell where the child will be will take all the available space
 * @param fill if @a fill == ETK_TRUE, the child will take all the available space in its cell
 * @param padding the size of the space on the sides of the child
 */
void etk_box_pack_start(Etk_Box *box, Etk_Widget *child, Etk_Bool expand, Etk_Bool fill, int padding)
{
   _etk_box_pack_full(box, child, expand, fill, padding, ETK_FALSE);
}

/**
 * @brief Adds the child to the end of the box
 * @param box a box
 * @param child the child to add
 * @param expand if @a expand == ETK_TRUE, the cell where the child will be will take all the available space
 * @param fill if @a fill == ETK_TRUE, the child will take all the available space in its cell
 * @param padding the size of the space on the sides of the child
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
   int i;

   if (!box || !child || (child->parent != ETK_WIDGET(box)))
      return;

   position = ETK_CLAMP(0, evas_list_count(box->children) - 1, position);

   for (l = box->children, i = 0; l; l = l->next, i++)
   {
      if (l->data == child)
      {
         if (i == position)
            return;
         else if (i < position)
         {
            box->children = evas_list_append_relative(box->children, child, evas_list_nth(box->children, position));
            box->children = evas_list_remove_list(box->children, l);
         }
         else
         {
            box->children = evas_list_prepend_relative(box->children, child, evas_list_nth(box->children, position));
            box->children = evas_list_remove_list(box->children, l);
         }
         return;
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
   etk_widget_size_recalc_queue(ETK_WIDGET(box));
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
 * @param homogeneous if homogeneous == ETK_TRUE, all the cells will have the same size
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
 * @brief Checks if the box is homogenous
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

/* Initializes the default values of the box */
static void _etk_box_constructor(Etk_Box *box)
{
   if (!box)
      return;

   box->children = NULL;
   box->spacing = 0;
   box->homogeneous = ETK_FALSE;

   ETK_CONTAINER(box)->child_add = _etk_box_child_add;
   ETK_CONTAINER(box)->child_remove = _etk_box_child_remove;
   ETK_CONTAINER(box)->children_get = _etk_box_children_get;
}

/* Destroys the box */
static void _etk_box_destructor(Etk_Box *box)
{
   Etk_Widget *child;

   if (!box)
      return;

   while (box->children)
   {
      child = ETK_WIDGET(box->children->data);
      free(child->child_properties);
      child->child_properties = NULL;
      box->children = evas_list_remove_list(box->children, box->children);
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

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Adds a child to the box */
static void _etk_box_child_add(Etk_Container *container, Etk_Widget *widget)
{
   etk_box_pack_start(ETK_BOX(container), widget, ETK_TRUE, ETK_TRUE, 0);
}

/* Removes the child from the box */
static void _etk_box_child_remove(Etk_Container *container, Etk_Widget *widget)
{
   Etk_Box *box;

   if (!(box = ETK_BOX(container)) || !widget || (widget->parent != ETK_WIDGET(container)))
      return;

   free(widget->child_properties);
   widget->child_properties = NULL;
   box->children = evas_list_remove(box->children, widget);
   etk_widget_parent_set(widget, NULL);
   etk_widget_size_recalc_queue(ETK_WIDGET(box));
}

/* Gets the list of the children of the box */
static Evas_List *_etk_box_children_get(Etk_Container *container)
{
   Etk_Box *box;
   
   if (!(box = ETK_BOX(container)))
      return NULL;
   return box->children;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Adds the "child" to the "box" : "expand" and "fill" describe its fill policy, "padding" is the size of the space
 * on the sides of the child and if "pack_end" == ETK_TRUE, the child will be packed at the end of the box */
static void _etk_box_pack_full(Etk_Box *box, Etk_Widget *child, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_end)
{
   Etk_Box_Child_Props *child_props;
   
   if (!box || !child)
      return;

   /* TODO: con_remove */
   if (child->parent && ETK_IS_CONTAINER(child->parent))
      etk_container_remove(ETK_CONTAINER(child->parent), child);
   child_props = calloc(1,sizeof(Etk_Box_Child_Props));
   child_props->expand = expand;
   child_props->fill = fill;
   child_props->padding = padding;
   child_props->pack_end = pack_end;
   child->child_properties = child_props;

   box->children = evas_list_append(box->children, child);
   etk_widget_parent_set(child, ETK_WIDGET(box));
}


/** @} */
