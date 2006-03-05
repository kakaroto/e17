/** @file etk_vbox.c */
#include "etk_vbox.h"
#include <stdlib.h>
#include <Evas.h>

/**
 * @addtogroup Etk_VBox
 * @{
 */

static void _etk_vbox_constructor(Etk_VBox *vbox);
static void _etk_vbox_destructor(Etk_VBox *vbox);
static void _etk_vbox_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_vbox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_VBox
 * @return Returns the type on an Etk_VBox
 */
Etk_Type *etk_vbox_type_get()
{
   static Etk_Type *vbox_type = NULL;

   if (!vbox_type)
      vbox_type = etk_type_new("Etk_VBox", ETK_BOX_TYPE, sizeof(Etk_VBox), ETK_CONSTRUCTOR(_etk_vbox_constructor), ETK_DESTRUCTOR(_etk_vbox_destructor));

   return vbox_type;
}

/**
 * @brief Creates a new vbox
 * @param homogeneous if homogeneous == ETK_TRUE, all the cells will have the same size
 * @param spacing the size of the space between two cells
 */
Etk_Widget *etk_vbox_new(Etk_Bool homogeneous, int spacing)
{
   return etk_widget_new(ETK_VBOX_TYPE, "homogeneous", homogeneous, "spacing", spacing, NULL);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the vbox */
static void _etk_vbox_constructor(Etk_VBox *vbox)
{
   if (!vbox)
      return;

   vbox->requested_sizes = NULL;
   ETK_WIDGET(vbox)->size_request = _etk_vbox_size_request;
   ETK_WIDGET(vbox)->size_allocate = _etk_vbox_size_allocate;
}

/* Destroys the vbox */
static void _etk_vbox_destructor(Etk_VBox *vbox)
{
   if (!vbox)
      return;
   free(vbox->requested_sizes);
}

/* Calculates the ideal size of the vbox */
static void _etk_vbox_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Evas_List *l;
   Etk_VBox *vbox;
   Etk_Box *box; 
   Etk_Container *container;
   Etk_Widget *child;
   Etk_Box_Child_Props *child_properties;
   Etk_Size child_requisition;
   int num_children, i;

   if (!(vbox = ETK_VBOX(widget)) || !size_requisition)
      return;

   box = ETK_BOX(vbox);
   container = ETK_CONTAINER(box);
   size_requisition->w = 0;
   size_requisition->h = 0;
   num_children = evas_list_count(box->children);
   vbox->requested_sizes = realloc(vbox->requested_sizes, num_children * sizeof(int));

   for (l = box->children, i = 0; l; l = l->next, i++)
   {
      child = ETK_WIDGET(l->data);
      child_properties = child->child_properties;
      etk_widget_size_request(child, &child_requisition);

      if (size_requisition->w < child_requisition.w)
         size_requisition->w = child_requisition.w;

      if (box->homogeneous)
      {
         if (size_requisition->h < child_requisition.h + 2 * child_properties->padding)
            size_requisition->h = child_requisition.h + 2 * child_properties->padding;
      }
      else
      {
         vbox->requested_sizes[i] = child_requisition.h + 2 * child_properties->padding;
         size_requisition->h += vbox->requested_sizes[i];
      }
   }
   if (box->homogeneous)
   {
      for (i = 0; i < num_children; i++)
         vbox->requested_sizes[i] = size_requisition->h;
      size_requisition->h *= num_children;
   }
   if (num_children > 1)
      size_requisition->h += (num_children - 1) * box->spacing;

   size_requisition->w += 2 * container->border_width;
   size_requisition->h += 2 * container->border_width;
}

/* Resizes the vbox to the size allocation */
static void _etk_vbox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Evas_List *l;
   Etk_VBox *vbox;
   Etk_Box *box;
   Etk_Container *container;
   Etk_Widget *child;
   Etk_Box_Child_Props *child_properties;
   Etk_Size requested_inner_size;
   Etk_Size allocated_inner_size;
   Etk_Geometry child_geometry;
   int num_children_to_expand = 0, i;
   float start_offset = 0, end_offset = 0;

   if ((!(vbox = ETK_VBOX(widget))))
      return;

   box = ETK_BOX(vbox);
   widget = ETK_WIDGET(vbox);
   container = ETK_CONTAINER(vbox);

   _etk_vbox_size_request(widget, &requested_inner_size);
   requested_inner_size.w -= 2 * container->border_width;
   requested_inner_size.h -= 2 * container->border_width;
   allocated_inner_size.w = geometry.w - 2 * container->border_width;
   allocated_inner_size.h = geometry.h - 2 * container->border_width;

   if (allocated_inner_size.h <= requested_inner_size.h)
   {
      float ratio;

      ratio = (float)allocated_inner_size.h / requested_inner_size.h;
      for (l = box->children, i = 0; l; l = l->next, i++)
      {
         child = ETK_WIDGET(l->data);
         child_properties = child->child_properties;

         child_geometry.x = geometry.x;
         child_geometry.h = vbox->requested_sizes[i] * ratio;
         child_geometry.w = allocated_inner_size.w;

         if (!child_properties->pack_end)
         {
            child_geometry.y = geometry.y + start_offset;
            start_offset += vbox->requested_sizes[i] * ratio + box->spacing;
         }
         else
         {
            child_geometry.y = geometry.y + geometry.h - end_offset - child_geometry.h;
            end_offset += vbox->requested_sizes[i] * ratio + box->spacing;
         }

         etk_container_child_space_fill(child, &child_geometry, 1, child_properties->fill); 
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
         free_space = (float)(allocated_inner_size.h - requested_inner_size.h) / num_children_to_expand;

      for (l = box->children, i = 0; l; l = l->next, i++)
      {
         child = ETK_WIDGET(l->data);
         child_properties = child->child_properties;

         child_geometry.x = geometry.x;
         child_geometry.w = allocated_inner_size.w;
         child_geometry.h = vbox->requested_sizes[i];
         if (child_properties->expand)
            child_geometry.h += free_space;

         if (!child_properties->pack_end)
         {
            child_geometry.y = geometry.y + start_offset;
            start_offset += vbox->requested_sizes[i] + box->spacing;
            if (child_properties->expand)
               start_offset += free_space;
         }
         else
         {
            child_geometry.y = geometry.y + geometry.h - end_offset - child_geometry.h;
            end_offset += vbox->requested_sizes[i] + box->spacing;
            if (child_properties->expand)
               end_offset += free_space;
         }

         etk_container_child_space_fill(child, &child_geometry, 1, child_properties->fill); 
         etk_widget_size_allocate(child, child_geometry);
      }
   }
}
