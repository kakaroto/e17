/** @file etk_hbox.c */
#include "etk_hbox.h"
#include <stdlib.h>
#include <Evas.h>

/**
 * @addtogroup Etk_HBox
* @{
 */

static void _etk_hbox_constructor(Etk_HBox *hbox);
static void _etk_hbox_destructor(Etk_HBox *hbox);
static void _etk_hbox_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_hbox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_HBox
 * @return Returns the type on an Etk_HBox
 */
Etk_Type *etk_hbox_type_get()
{
   static Etk_Type *hbox_type = NULL;

   if (!hbox_type)
      hbox_type = etk_type_new("Etk_HBox", ETK_BOX_TYPE, sizeof(Etk_HBox), ETK_CONSTRUCTOR(_etk_hbox_constructor), ETK_DESTRUCTOR(_etk_hbox_destructor));

   return hbox_type;
}

/**
 * @brief Creates a new hbox
 * @param homogeneous if homogeneous == ETK_TRUE, all the cells will have the same size
 * @param spacing the size of the space between two cells
 */
Etk_Widget *etk_hbox_new(Etk_Bool homogeneous, int spacing)
{
   return etk_widget_new(ETK_HBOX_TYPE, "homogeneous", homogeneous, "spacing", spacing, NULL);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the hbox */
static void _etk_hbox_constructor(Etk_HBox *hbox)
{
   if (!hbox)
      return;

   hbox->requested_sizes = NULL;
   ETK_WIDGET(hbox)->size_request = _etk_hbox_size_request;
   ETK_WIDGET(hbox)->size_allocate = _etk_hbox_size_allocate;
}

/* Destroys the hbox */
static void _etk_hbox_destructor(Etk_HBox *hbox)
{
   if (!hbox)
      return;
   free(hbox->requested_sizes);
}

/* Calculates the ideal size of the hbox */
static void _etk_hbox_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Evas_List *l;
   Etk_HBox *hbox;
   Etk_Box *box;
   Etk_Container *container;
   Etk_Widget *child;
   Etk_Box_Child_Props *child_properties;
   Etk_Size child_requisition;
   int num_children, i;

   if (!(hbox = ETK_HBOX(widget)) || !size_requisition)
      return;

   box = ETK_BOX(hbox);
   container = ETK_CONTAINER(hbox);
   size_requisition->w = 0;
   size_requisition->h = 0;
   num_children = evas_list_count(box->children);
   hbox->requested_sizes = realloc(hbox->requested_sizes, num_children * sizeof(int));

   for (l = box->children, i = 0; l; l = l->next, i++)
   {
      child = ETK_WIDGET(l->data);
      child_properties = child->child_properties;
      etk_widget_size_request(child, &child_requisition);

      if (box->homogeneous)
      {
         if (size_requisition->w < child_requisition.w + 2 * child_properties->padding)
            size_requisition->w = child_requisition.w + 2 * child_properties->padding;
      }
      else
      {
         hbox->requested_sizes[i] = child_requisition.w + 2 * child_properties->padding;
         size_requisition->w += hbox->requested_sizes[i];
      }

      if (size_requisition->h < child_requisition.h)
         size_requisition->h = child_requisition.h;
   }
   if (box->homogeneous)
   {
      for (i = 0; i < num_children; i++)
         hbox->requested_sizes[i] = size_requisition->w;
      size_requisition->w *= num_children;
   }
   if (num_children > 1)
      size_requisition->w += (num_children - 1) * box->spacing;

   size_requisition->w += 2 * container->border_width;
   size_requisition->h += 2 * container->border_width;
}

/* Resizes the hbox to the size allocation */
static void _etk_hbox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Evas_List *l;
   Etk_HBox *hbox;
   Etk_Box *box;
   Etk_Container *container;
   Etk_Widget *child;
   Etk_Box_Child_Props *child_properties;
   Etk_Size requested_inner_size;
   Etk_Size allocated_inner_size;
   Etk_Geometry child_geometry;
   int num_children_to_expand = 0, i;
   float start_offset = 0, end_offset = 0;

   if (!(hbox = ETK_HBOX(widget)))
      return;

   box = ETK_BOX(hbox);
   widget = ETK_WIDGET(hbox);
   container = ETK_CONTAINER(hbox);

   _etk_hbox_size_request(widget, &requested_inner_size);
   requested_inner_size.w -= 2 * container->border_width;
   requested_inner_size.h -= 2 * container->border_width;
   allocated_inner_size.w = geometry.w - 2 * container->border_width;
   allocated_inner_size.h = geometry.h - 2 * container->border_width;

   if (allocated_inner_size.w <= requested_inner_size.w)
   {
      float ratio;

      ratio = (float)allocated_inner_size.w / requested_inner_size.w;
      for (l = box->children, i = 0; l; l = l->next, i++)
      {
         child = ETK_WIDGET(l->data);
         child_properties = child->child_properties;

         child_geometry.y = geometry.y;
         child_geometry.w = hbox->requested_sizes[i] * ratio;
         child_geometry.h = allocated_inner_size.h;

         if (!child_properties->pack_end)
         {
            child_geometry.x = geometry.x + start_offset;
            start_offset += hbox->requested_sizes[i] * ratio + box->spacing;
         }
         else
         {
            child_geometry.x = geometry.x + geometry.w - end_offset - child_geometry.w;
            end_offset += hbox->requested_sizes[i] * ratio + box->spacing;
         }

         etk_container_child_space_fill(child, &child_geometry, child_properties->fill, 1); 
         etk_widget_size_allocate(child, child_geometry);
      }
   }
   else
   {
      float free_space;

      for (l = box->children; l; l = l->next)
      {
         child = ETK_WIDGET(l->data);
         child_properties = child->child_properties;
         if (child_properties->expand)
            num_children_to_expand++;
      }

      if (num_children_to_expand <= 0)
         free_space = 0;
      else
         free_space = (float)(allocated_inner_size.w - requested_inner_size.w) / num_children_to_expand;

      for (l = box->children, i = 0; l; l = l->next, i++)
      {
         child = ETK_WIDGET(l->data);
         child_properties = child->child_properties;

         child_geometry.y = geometry.y;
         child_geometry.w = hbox->requested_sizes[i];
         if (child_properties->expand)
            child_geometry.w += free_space;
         child_geometry.h = allocated_inner_size.h;

         if (!child_properties->pack_end)
         {
            child_geometry.x = geometry.x + start_offset;
            start_offset += hbox->requested_sizes[i] + box->spacing;
            if (child_properties->expand)
               start_offset += free_space;
         }
         else
         {
            child_geometry.x = geometry.x + geometry.w - end_offset - child_geometry.w;
            end_offset += hbox->requested_sizes[i] + box->spacing;
            if (child_properties->expand)
               end_offset += free_space;
         }

         etk_container_child_space_fill(child, &child_geometry, child_properties->fill, 1); 
         etk_widget_size_allocate(child, child_geometry);
      }
   }
}
